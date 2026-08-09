

#pragma once

#include <atomic>

namespace Sentry {

inline std::atomic<int>  g_score{ 0 };
inline std::atomic<bool> g_tripped{ false };
inline std::atomic<bool> g_choke{ false };

inline HMODULE g_selfModule = nullptr;

inline bool ShouldChoke() { return g_choke.load(std::memory_order_relaxed); }
inline bool Tripped()     { return g_tripped.load(std::memory_order_relaxed); }
inline int  Score()       { return g_score.load(std::memory_order_relaxed); }

void Start();

}

#if defined(USE_SENTRY)

#include <Windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <thread>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>
#include "VMProtectMarkers.h"
#include "XorStr.h"

#include "IATIntegrity.h"
#include "SelfCRC.h"
#include "VEHAntiDebug.h"
#include "Manifest.h"

namespace Sentry {

namespace detail {

template <class T>
inline T Resolve(const wchar_t* mod, const char* name) {
    HMODULE h = GetModuleHandleW(mod);
    if (!h) return nullptr;
    return reinterpret_cast<T>(GetProcAddress(h, name));
}

using fnNtQueryInformationProcess =
    NTSTATUS(NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);

inline fnNtQueryInformationProcess ResolveNtQIP() {
    return Resolve<fnNtQueryInformationProcess>(L"ntdll.dll",
        XS("NtQueryInformationProcess"));
}

constexpr ULONG kProcessDebugPort         = 7;
constexpr ULONG kProcessDebugObjectHandle = 0x1E;
constexpr ULONG kProcessDebugFlags        = 0x1F;

struct DenyListCache {
    std::vector<std::string> entries;
    bool fallback_used  = false;
    bool fallback_logged = false;
    bool from_manifest  = false;
};

inline DenyListCache g_hostileProcCache;
inline DenyListCache g_toolWindowCache;
inline std::mutex    g_denyMutex;

inline std::string ToLowerAscii(const char* s) {
    std::string out;
    if (!s) return out;
    out.reserve(strlen(s));
    for (; *s; ++s) {
        unsigned char c = static_cast<unsigned char>(*s);
        out.push_back((c >= 'A' && c <= 'Z') ? char(c + 32) : char(c));
    }
    return out;
}

inline std::string ToLowerAscii(const wchar_t* w) {
    std::string out;
    if (!w) return out;
    for (; *w; ++w) {
        wchar_t c = *w;
        if (c >= L'A' && c <= L'Z') c = wchar_t(c + 32);

        if (c < 128) out.push_back(static_cast<char>(c));
    }
    return out;
}

inline void RefreshHostileProcessList() {
    std::lock_guard<std::mutex> lk(g_denyMutex);
    if (g_hostileProcCache.from_manifest) return;

    const nlohmann::json* ac = Manifest::GetAnticheatSection();
    if (ac && ac->contains("hostile_processes") && (*ac)["hostile_processes"].is_array()) {
        std::vector<std::string> tmp;
        tmp.reserve((*ac)["hostile_processes"].size());
        for (const auto& v : (*ac)["hostile_processes"]) {
            if (v.is_string()) tmp.push_back(ToLowerAscii(v.get<std::string>().c_str()));
        }
        if (!tmp.empty()) {
            g_hostileProcCache.entries     = std::move(tmp);
            g_hostileProcCache.from_manifest = true;
            g_hostileProcCache.fallback_used = false;
            return;
        }
    }

    if (g_hostileProcCache.entries.empty()) {
        g_hostileProcCache.entries = {
            std::string("cheatengine-x86_64.exe"),
            std::string("x64dbg.exe"),
            std::string("processhacker.exe"),
        };
        g_hostileProcCache.fallback_used = true;
    }
    if (g_hostileProcCache.fallback_used && !g_hostileProcCache.fallback_logged) {
        std::cout << XS("[Sentry] hostile_processes manifest section unavailable — "
                        "using fallback skeleton (CE/x64dbg/PH).") << std::endl;
        g_hostileProcCache.fallback_logged = true;
    }
}

inline void RefreshToolWindowList() {
    std::lock_guard<std::mutex> lk(g_denyMutex);
    if (g_toolWindowCache.from_manifest) return;

    const nlohmann::json* ac = Manifest::GetAnticheatSection();
    if (ac && ac->contains("tool_window_classes") && (*ac)["tool_window_classes"].is_array()) {
        std::vector<std::string> tmp;
        tmp.reserve((*ac)["tool_window_classes"].size());
        for (const auto& v : (*ac)["tool_window_classes"]) {

            if (v.is_string()) tmp.push_back(v.get<std::string>());
        }
        if (!tmp.empty()) {
            g_toolWindowCache.entries     = std::move(tmp);
            g_toolWindowCache.from_manifest = true;
            g_toolWindowCache.fallback_used = false;
            return;
        }
    }

    if (g_toolWindowCache.entries.empty()) {
        g_toolWindowCache.entries = {
            std::string("OLLYDBG"),
            std::string("Qt5QWindowIcon"),
            std::string("ProcessHacker"),
        };
        g_toolWindowCache.fallback_used = true;
    }
    if (g_toolWindowCache.fallback_used && !g_toolWindowCache.fallback_logged) {
        std::cout << XS("[Sentry] tool_window_classes manifest section unavailable — "
                        "using fallback skeleton (OLLYDBG/Qt5/PH).") << std::endl;
        g_toolWindowCache.fallback_logged = true;
    }
}

inline std::vector<std::string> SnapshotHostileProcessList() {
    std::lock_guard<std::mutex> lk(g_denyMutex);
    return g_hostileProcCache.entries;
}
inline std::vector<std::string> SnapshotToolWindowList() {
    std::lock_guard<std::mutex> lk(g_denyMutex);
    return g_toolWindowCache.entries;
}

inline bool DebuggerAttached(fnNtQueryInformationProcess NtQIP) {
    if (!NtQIP) return false;
    HANDLE proc = GetCurrentProcess();

    ULONG_PTR dbgPort = 0;
    if (NtQIP(proc, kProcessDebugPort, &dbgPort, sizeof(dbgPort), nullptr) == 0 && dbgPort)
        return true;

    HANDLE dbgObj = nullptr;
    if (NtQIP(proc, kProcessDebugObjectHandle, &dbgObj, sizeof(dbgObj), nullptr) == 0 && dbgObj)
        return true;

    ULONG dbgFlags = 0;

    if (NtQIP(proc, kProcessDebugFlags, &dbgFlags, sizeof(dbgFlags), nullptr) == 0 && dbgFlags == 0)
        return true;

    return false;
}

inline bool HardwareBreakpointsSet() {
    static auto pSnap  = Resolve<decltype(&CreateToolhelp32Snapshot)>(L"kernel32.dll", XS("CreateToolhelp32Snapshot"));
    static auto pFirst = Resolve<decltype(&Thread32First)>(L"kernel32.dll", XS("Thread32First"));
    static auto pNext  = Resolve<decltype(&Thread32Next)>(L"kernel32.dll", XS("Thread32Next"));
    static auto pOpen  = Resolve<decltype(&OpenThread)>(L"kernel32.dll", XS("OpenThread"));
    static auto pCtx   = Resolve<decltype(&GetThreadContext)>(L"kernel32.dll", XS("GetThreadContext"));
    if (!pSnap || !pFirst || !pNext || !pOpen || !pCtx) return false;

    DWORD pid = GetCurrentProcessId();
    HANDLE snap = pSnap(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    THREADENTRY32 te{};
    te.dwSize = sizeof(te);
    if (pFirst(snap, &te)) {
        do {
            if (te.th32OwnerProcessID != pid) continue;
            if (te.th32ThreadID == GetCurrentThreadId()) continue;
            HANDLE th = pOpen(THREAD_GET_CONTEXT, FALSE, te.th32ThreadID);
            if (!th) continue;
            CONTEXT ctx{};
            ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
            if (pCtx(th, &ctx)) {
                if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3) found = true;
            }
            CloseHandle(th);
        } while (!found && pNext(snap, &te));
    }
    CloseHandle(snap);
    return found;
}

inline bool CheatEngineDriverPresent() {
    static auto pCreateFileW = Resolve<decltype(&CreateFileW)>(L"kernel32.dll", XS("CreateFileW"));
    if (!pCreateFileW) return false;
    HANDLE h = pCreateFileW(XSW(L"\\\\.\\DBKProcList64"), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h != INVALID_HANDLE_VALUE) { CloseHandle(h); return true; }
    return false;
}

inline int CheckHostileProcesses() {
    VMP_BEGIN_MUT("Sentry_CheckHostileProcesses");
    RefreshHostileProcessList();
    std::vector<std::string> deny = SnapshotHostileProcessList();
    if (deny.empty()) { VMP_END(); return 0; }

    static auto pSnap  = Resolve<decltype(&CreateToolhelp32Snapshot)>(L"kernel32.dll", XS("CreateToolhelp32Snapshot"));
    static auto pFirst = Resolve<decltype(&Process32FirstW)>(L"kernel32.dll", XS("Process32FirstW"));
    static auto pNext  = Resolve<decltype(&Process32NextW)>(L"kernel32.dll", XS("Process32NextW"));
    if (!pSnap || !pFirst || !pNext) { VMP_END(); return 0; }

    HANDLE snap = pSnap(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) { VMP_END(); return 0; }

    int hits = 0;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (pFirst(snap, &pe)) {
        do {
            std::string exe = ToLowerAscii(pe.szExeFile);
            if (exe.empty()) continue;
            for (const auto& d : deny) {
                if (exe == d) { ++hits; break; }
            }
        } while (pNext(snap, &pe));
    }
    CloseHandle(snap);
    VMP_END();
    return hits;
}

inline bool ToolWindowPresent() {
    RefreshToolWindowList();

    struct Ctx {
        bool hit;
        const std::vector<std::string>* deny;
    };
    std::vector<std::string> deny = SnapshotToolWindowList();
    Ctx c{ false, &deny };

    EnumWindows([](HWND hwnd, LPARAM lp) -> BOOL {
        Ctx* ctx = reinterpret_cast<Ctx*>(lp);
        char cls[128] = {}, title[256] = {};
        GetClassNameA(hwnd, cls, sizeof(cls));
        GetWindowTextA(hwnd, title, sizeof(title));

        auto contains = [](const char* h, const char* n) {
            return h[0] && n[0] && std::strstr(h, n) != nullptr;
        };

        bool hit = false;

        if (contains(cls, "TfrmMain") && contains(title, "Cheat Engine")) {
            hit = true;
        }
        else if ((contains(cls, "Qt5QWindowIcon") || contains(cls, "Qt6QWindowIcon")) &&
                 (contains(title, "x64dbg") || contains(title, "x32dbg"))) {
            hit = true;
        }
        else if (ctx->deny) {

            for (const auto& d : *ctx->deny) {
                if (d == "TfrmMain" || d == "Qt5QWindowIcon" || d == "Qt6QWindowIcon" ||
                    d == "MdiClient")
                {
                    continue;
                }
                if (std::strcmp(cls, d.c_str()) == 0) { hit = true; break; }
            }
        }

        if (hit) { ctx->hit = true; return FALSE; }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&c));
    return c.hit;
}

inline bool IsHooked(const uint8_t* p) {
    if (!p) return false;
    if (p[0] == 0xE9 || p[0] == 0xEB) return true;
    if (p[0] == 0xFF && p[1] == 0x25) return true;
    if (p[0] == 0x68 && p[5] == 0xC3) return true;
    return false;
}

inline int ScanNtdllHooks() {
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    if (!nt) return 0;
    int n = 0;
    auto chk = [&](const char* nm) {
        if (auto p = reinterpret_cast<const uint8_t*>(GetProcAddress(nt, nm)))
            if (IsHooked(p)) ++n;
    };
    chk(XS("NtQueryInformationProcess"));
    chk(XS("NtSetInformationThread"));
    chk(XS("NtClose"));
    chk(XS("NtQuerySystemInformation"));
    chk(XS("NtProtectVirtualMemory"));
    chk(XS("NtQueryObject"));
    return n;
}

inline bool NtGlobalFlagSet() {
    VMP_BEGIN_MUT("Sentry_NtGlobalFlagSet");
    auto peb = reinterpret_cast<const uint8_t*>(__readgsqword(0x60));
    if (!peb) { VMP_END(); return false; }
    ULONG flag = *reinterpret_cast<const ULONG*>(peb + 0xBC);
    bool hit = (flag & 0x70u) != 0;
    VMP_END();
    return hit;
}

inline bool HeapForceFlags() {
    VMP_BEGIN_MUT("Sentry_HeapForceFlags");
    auto peb = reinterpret_cast<const uint8_t*>(__readgsqword(0x60));
    if (!peb) { VMP_END(); return false; }
    auto heap = *reinterpret_cast<const uint8_t* const*>(peb + 0x30);
    if (!heap) { VMP_END(); return false; }
    ULONG flags = *reinterpret_cast<const ULONG*>(heap + 0x70);
    ULONG forceFlags = *reinterpret_cast<const ULONG*>(heap + 0x74);
    bool hit = (flags != 0) || (forceFlags != 0);
    VMP_END();
    return hit;
}

inline bool ParentIsDebugger() {
    VMP_BEGIN_MUT("Sentry_ParentIsDebugger");
    static auto pSnap   = Resolve<decltype(&CreateToolhelp32Snapshot)>(L"kernel32.dll", XS("CreateToolhelp32Snapshot"));
    static auto pFirst  = Resolve<decltype(&Process32FirstW)>(L"kernel32.dll", XS("Process32FirstW"));
    static auto pNext   = Resolve<decltype(&Process32NextW)>(L"kernel32.dll", XS("Process32NextW"));
    if (!pSnap || !pFirst || !pNext) { VMP_END(); return false; }

    DWORD myPid = GetCurrentProcessId();
    HANDLE snap = pSnap(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) { VMP_END(); return false; }

    DWORD parentPid = 0;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (pFirst(snap, &pe)) {
        do {
            if (pe.th32ProcessID == myPid) { parentPid = pe.th32ParentProcessID; break; }
        } while (pNext(snap, &pe));
    }

    bool hit = false;
    if (parentPid) {

        pe = {};
        pe.dwSize = sizeof(pe);
        if (pFirst(snap, &pe)) {
            do {
                if (pe.th32ProcessID != parentPid) continue;

                auto ieq = [](const wchar_t* a, const wchar_t* b) {
                    while (*a && *b) {
                        wchar_t ca = (*a >= L'A' && *a <= L'Z') ? wchar_t(*a + 32) : *a;
                        wchar_t cb = (*b >= L'A' && *b <= L'Z') ? wchar_t(*b + 32) : *b;
                        if (ca != cb) return false;
                        ++a; ++b;
                    }
                    return *a == 0 && *b == 0;
                };

                const wchar_t* deny[] = {
                    XSW(L"x64dbg.exe"),
                    XSW(L"x32dbg.exe"),
                    XSW(L"ollydbg.exe"),
                    XSW(L"ida.exe"),
                    XSW(L"ida64.exe"),
                    XSW(L"windbg.exe"),
                    XSW(L"cheatengine-x86_64.exe"),
                };
                for (const wchar_t* d : deny) {
                    if (ieq(pe.szExeFile, d)) { hit = true; break; }
                }
                break;
            } while (pNext(snap, &pe));
        }
    }

    CloseHandle(snap);
    VMP_END();
    return hit;
}

using fnNtClose = NTSTATUS(NTAPI*)(HANDLE);

inline bool NtCloseSEHProbe(fnNtClose pNtClose) {
    volatile bool exceptionFired = false;
    __try {
        pNtClose(reinterpret_cast<HANDLE>(static_cast<uintptr_t>(0xDEADBEEFull)));
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        exceptionFired = true;
    }
    return !exceptionFired;
}

inline bool NtCloseInvalidHandle() {
    VMP_BEGIN_MUT("Sentry_NtCloseInvalidHandle");
    static auto NtClose = Resolve<fnNtClose>(L"ntdll.dll", XS("NtClose"));
    bool hit = NtClose ? NtCloseSEHProbe(NtClose) : false;

    VMP_END();
    return hit;
}

inline bool TimingDelta() {
    VMP_BEGIN_MUT("Sentry_TimingDelta");
    uint64_t accum = 0;
    int regs[4];
    for (int i = 0; i < 5; ++i) {
        uint64_t a = __rdtsc();
        __cpuid(regs, 0);
        uint64_t b = __rdtsc();
        accum += (b - a);
    }
    bool hit = (accum / 5) > 200000ull;
    VMP_END();
    return hit;
}

inline bool DebuggerWindowVisible() {
    VMP_BEGIN_MUT("Sentry_DebuggerWindowVisible");
    RefreshToolWindowList();
    std::vector<std::string> deny = SnapshotToolWindowList();
    struct Ctx {
        bool hit;
        const std::vector<std::string>* deny;
    };
    Ctx c{ false, &deny };

    EnumWindows([](HWND hwnd, LPARAM lp) -> BOOL {
        if (!IsWindowVisible(hwnd)) return TRUE;
        Ctx* ctx = reinterpret_cast<Ctx*>(lp);
        char cls[128] = {};
        GetClassNameA(hwnd, cls, sizeof(cls));

        auto eq = [](const char* a, const char* b) {
            return a[0] && b[0] && std::strcmp(a, b) == 0;
        };
        auto contains = [](const char* h, const char* n) {
            return h[0] && n[0] && std::strstr(h, n) != nullptr;
        };

        bool hit = false;

        if (contains(cls, "MdiClient")) {
            HWND parent = GetParent(hwnd);
            if (parent) {
                char pcls[128] = {};
                GetClassNameA(parent, pcls, sizeof(pcls));
                if (eq(pcls, XS("ProcessHacker")) || eq(pcls, XS("SystemInformer"))) {
                    hit = true;
                }
            }
        }

        else if (ctx->deny) {
            for (const auto& d : *ctx->deny) {
                if (d == "TfrmMain" || d == "Qt5QWindowIcon" || d == "Qt6QWindowIcon" ||
                    d == "MdiClient")
                {
                    continue;
                }
                if (std::strcmp(cls, d.c_str()) == 0) { hit = true; break; }
            }
        }

        if (hit) { ctx->hit = true; return FALSE; }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&c));
    VMP_END();
    return c.hit;
}

inline void WatchLoop() {

    constexpr ULONG kThreadHideFromDebugger = 0x11;
    if (HMODULE nt = GetModuleHandleW(L"ntdll.dll")) {
        using fnNtSIT = NTSTATUS(NTAPI*)(HANDLE, ULONG, PVOID, ULONG);
        if (auto NtSIT = reinterpret_cast<fnNtSIT>(GetProcAddress(nt, "NtSetInformationThread")))
            NtSIT(GetCurrentThread(), kThreadHideFromDebugger, nullptr, 0);
    }

    fnNtQueryInformationProcess NtQIP = ResolveNtQIP();

    uint32_t rng = GetTickCount() ^ (GetCurrentThreadId() << 8);
    auto nextRng = [&rng]() { rng = rng * 1664525u + 1013904223u; return rng; };
    bool chokeScheduled = false;
    uint32_t chokeAtTick = 0;

    for (;;) {
        VMP_BEGIN_MUT("Sentry_tick");
        int delta = 0;
        bool highConfidence = false;

        if (DebuggerAttached(NtQIP))       { delta += 5; highConfidence = true; }
        if (CheatEngineDriverPresent())    { delta += 5; highConfidence = true; }
        if (HardwareBreakpointsSet())      { delta += 3; }
        if (ToolWindowPresent())           { delta += 2; }
        if (int h = ScanNtdllHooks())      { delta += h * 2; }
        if (NtGlobalFlagSet())             { delta += 2; }
        if (HeapForceFlags())              { delta += 2; }
        if (ParentIsDebugger())            { delta += 5; highConfidence = true; }
        if (NtCloseInvalidHandle())        { delta += 3; }
        if (TimingDelta())                 { delta += 1; }
        if (DebuggerWindowVisible())       { delta += 2; }

        if (int h = CheckHostileProcesses()) { delta += h * 5; highConfidence = true; }

        if (int h = IATIntegrity::Scan(g_selfModule)) { delta += h * 3; }
        if (SelfCRC::Initialized() && SelfCRC::Scan()) { delta += 4; }
        if (VEHAntiDebug::ProbeInt2d() || VEHAntiDebug::ProbeInt3() ||
            VEHAntiDebug::ProbeCloseHandleAbuse()) { delta += 3; }

        if (delta > 0) {
            int total = g_score.fetch_add(delta, std::memory_order_relaxed) + delta;
            if (total >= 5) g_tripped.store(true, std::memory_order_relaxed);
        }

        if (highConfidence && !chokeScheduled) {
            chokeScheduled = true;
            chokeAtTick = GetTickCount() + 30000u + (nextRng() % 90000u);
        }
        if (chokeScheduled && !g_choke.load(std::memory_order_relaxed) &&
            (int)(GetTickCount() - chokeAtTick) >= 0) {
            g_choke.store(true, std::memory_order_relaxed);
        }
        VMP_END();

        Sleep(2000 + (nextRng() % 3000));
    }
}

}

inline void Start() {
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCSTR>(&Start), &g_selfModule);
    std::thread(detail::WatchLoop).detach();
}

}

#else

namespace Sentry {
inline void Start() {}
}

#endif

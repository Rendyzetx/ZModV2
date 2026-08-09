

#pragma once

#include <Windows.h>
#include <cstdio>
#include <atomic>

namespace CrashLogger {

inline PVOID  g_veh        = nullptr;
inline char   g_path[MAX_PATH] = {0};
inline std::atomic<bool> g_inHandler{false};
inline uintptr_t g_zwBase  = 0;
inline uintptr_t g_zwEnd   = 0;

inline void RawAppend(const char* buf, DWORD len) {
    HANDLE h = CreateFileA(g_path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote = 0;
    WriteFile(h, buf, len, &wrote, nullptr);
    FlushFileBuffers(h);
    CloseHandle(h);
}

inline LONG CALLBACK Handler(EXCEPTION_POINTERS* ep) {
    if (!ep || !ep->ExceptionRecord) return EXCEPTION_CONTINUE_SEARCH;
    const DWORD code = ep->ExceptionRecord->ExceptionCode;

    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_ILLEGAL_INSTRUCTION:
        case EXCEPTION_PRIV_INSTRUCTION:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_STACK_OVERFLOW:
        case EXCEPTION_IN_PAGE_ERROR:
            break;
        default:
            return EXCEPTION_CONTINUE_SEARCH;
    }

    bool expected = false;
    if (!g_inHandler.compare_exchange_strong(expected, true))
        return EXCEPTION_CONTINUE_SEARCH;

    void* rip = ep->ExceptionRecord->ExceptionAddress;

    char modName[MAX_PATH] = "(unknown)";
    uintptr_t modBase = 0;
    HMODULE hMod = nullptr;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)rip, &hMod) && hMod) {
        modBase = (uintptr_t)hMod;
        char full[MAX_PATH];
        if (GetModuleFileNameA(hMod, full, MAX_PATH)) {

            const char* b = full;
            for (const char* p = full; *p; ++p) if (*p == '\\' || *p == '/') b = p + 1;
            lstrcpynA(modName, b, MAX_PATH);
        }
    }
    uintptr_t off = modBase ? ((uintptr_t)rip - modBase) : 0;

    const char* acc = "?";
    uintptr_t target = 0;
    if (code == EXCEPTION_ACCESS_VIOLATION && ep->ExceptionRecord->NumberParameters >= 2) {
        switch (ep->ExceptionRecord->ExceptionInformation[0]) {
            case 0: acc = "read";    break;
            case 1: acc = "write";   break;
            case 8: acc = "execute"; break;
        }
        target = (uintptr_t)ep->ExceptionRecord->ExceptionInformation[1];
    }

    char line[512];
    int n = _snprintf_s(line, sizeof(line), _TRUNCATE,
        "[FAULT] code=0x%08lX rip=0x%p module=%s +0x%llX access=%s target=0x%llX tid=%lu\r\n",
        code, rip, modName, (unsigned long long)off, acc,
        (unsigned long long)target, GetCurrentThreadId());
    if (n > 0) RawAppend(line, (DWORD)n);

    if (g_zwBase && g_zwEnd && ep->ContextRecord) {
        uintptr_t rsp = (uintptr_t)ep->ContextRecord->Rsp;
        if (rsp) {
            const int kMaxHits = 16;
            int hits = 0;
            uintptr_t prev = 0;

            for (uintptr_t p = rsp; p < rsp + 0x1000 && hits < kMaxHits; p += sizeof(uintptr_t)) {
                uintptr_t val = 0;

                __try { val = *(uintptr_t*)p; }
                __except (EXCEPTION_EXECUTE_HANDLER) { break; }
                if (val >= g_zwBase && val < g_zwEnd && val != prev) {
                    prev = val;
                    char sl[160];
                    int sn = _snprintf_s(sl, sizeof(sl), _TRUNCATE,
                        "[FAULT]   stack[%d] ZwSpark.dll +0x%llX\r\n",
                        hits, (unsigned long long)(val - g_zwBase));
                    if (sn > 0) RawAppend(sl, (DWORD)sn);
                    ++hits;
                }
            }
            if (hits == 0) {
                const char* nz = "[FAULT]   stack: no ZwSpark.dll frames found in top 4KB\r\n";
                RawAppend(nz, (DWORD)lstrlenA(nz));
            }
        }
    }

    char hint[256];
    int hn = _snprintf_s(hint, sizeof(hint), _TRUNCATE,
        "[FAULT] AV in %s +0x%llX (%s 0x%llX) — see %s\r\n",
        modName, (unsigned long long)off, acc, (unsigned long long)target, g_path);
    if (hn > 0) {
        DWORD w = 0;
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut && hOut != INVALID_HANDLE_VALUE) WriteFile(hOut, hint, (DWORD)hn, &w, nullptr);
    }

    g_inHandler.store(false);

    return EXCEPTION_CONTINUE_SEARCH;
}

inline void Install() {
    if (g_veh) return;

    char appdata[MAX_PATH] = {0};
    DWORD len = GetEnvironmentVariableA("APPDATA", appdata, MAX_PATH);
    if (len && len < MAX_PATH) {
        _snprintf_s(g_path, sizeof(g_path), _TRUNCATE,
            "%s\\ZMod\\logs\\zwspark-crash-%lu.txt", appdata, GetCurrentProcessId());
    } else {
        _snprintf_s(g_path, sizeof(g_path), _TRUNCATE,
            "C:\\zwspark-crash-%lu.txt", GetCurrentProcessId());
    }

    HMODULE self = nullptr;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)&Install, &self) && self) {
        g_zwBase = (uintptr_t)self;

        auto dos = (IMAGE_DOS_HEADER*)self;
        auto nt  = (IMAGE_NT_HEADERS*)(g_zwBase + dos->e_lfanew);
        g_zwEnd  = g_zwBase + nt->OptionalHeader.SizeOfImage;
    }

    g_veh = AddVectoredExceptionHandler(1, &Handler);
}

}



#pragma once

#include <Windows.h>
#include <cstdint>
#include <atomic>
#include "VMProtectMarkers.h"

namespace VEHAntiDebug {

inline thread_local bool g_vehRan = false;

inline std::atomic<void*> g_vehHandle{ nullptr };

using StubFn = void(*)(void);
inline StubFn g_int2dStub = nullptr;
inline StubFn g_int3Stub  = nullptr;

inline LONG CALLBACK Handler(EXCEPTION_POINTERS* ep)
{
    if (!ep || !ep->ExceptionRecord || !ep->ContextRecord)
        return EXCEPTION_CONTINUE_SEARCH;

    const DWORD code = ep->ExceptionRecord->ExceptionCode;

    switch (code)
    {
    case EXCEPTION_BREAKPOINT:
    case 0x4000001F:
    {

        BYTE* rip = reinterpret_cast<BYTE*>(ep->ContextRecord->Rip);
        if (rip && rip[0] == 0xCD && rip[1] == 0x2D)
            ep->ContextRecord->Rip += 2;
        else
            ep->ContextRecord->Rip += 1;
        g_vehRan = true;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    case EXCEPTION_SINGLE_STEP:
        g_vehRan = true;
        return EXCEPTION_CONTINUE_EXECUTION;
    case 0xC0000008:
        g_vehRan = true;

        return EXCEPTION_CONTINUE_SEARCH;
    default:
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

inline bool Install()
{
    if (g_vehHandle.load(std::memory_order_acquire) != nullptr)
        return true;

    BYTE* int2d = static_cast<BYTE*>(VirtualAlloc(
        nullptr, 16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    BYTE* int3  = static_cast<BYTE*>(VirtualAlloc(
        nullptr, 16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!int2d || !int3) {
        if (int2d) VirtualFree(int2d, 0, MEM_RELEASE);
        if (int3)  VirtualFree(int3,  0, MEM_RELEASE);
        return false;
    }

    int2d[0] = 0xCD; int2d[1] = 0x2D; int2d[2] = 0xC3;

    int3 [0] = 0xCC; int3 [1] = 0xC3;

    DWORD oldProt = 0;
    VirtualProtect(int2d, 16, PAGE_EXECUTE_READ, &oldProt);
    VirtualProtect(int3,  16, PAGE_EXECUTE_READ, &oldProt);
    FlushInstructionCache(GetCurrentProcess(), int2d, 16);
    FlushInstructionCache(GetCurrentProcess(), int3,  16);

    g_int2dStub = reinterpret_cast<StubFn>(int2d);
    g_int3Stub  = reinterpret_cast<StubFn>(int3);

    PVOID h = AddVectoredExceptionHandler(1, &Handler);
    if (!h) {
        VirtualFree(int2d, 0, MEM_RELEASE);
        VirtualFree(int3,  0, MEM_RELEASE);
        g_int2dStub = g_int3Stub = nullptr;
        return false;
    }
    g_vehHandle.store(h, std::memory_order_release);
    return true;
}

inline void Uninstall()
{
    if (PVOID h = g_vehHandle.exchange(nullptr, std::memory_order_acq_rel))
        RemoveVectoredExceptionHandler(h);

    if (g_int2dStub) {
        VirtualFree(reinterpret_cast<void*>(g_int2dStub), 0, MEM_RELEASE);
        g_int2dStub = nullptr;
    }
    if (g_int3Stub) {
        VirtualFree(reinterpret_cast<void*>(g_int3Stub), 0, MEM_RELEASE);
        g_int3Stub = nullptr;
    }
}

inline bool ProbeInt2d()
{
    VMP_BEGIN_MUT("VEH_ProbeInt2d");
    if (!g_int2dStub) { VMP_END(); return false; }

    g_vehRan = false;
    bool caughtBySEH = false;
    __try {
        g_int2dStub();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        caughtBySEH = true;
    }

    const bool debugged = caughtBySEH || !g_vehRan;
    VMP_END();
    return debugged;
}

inline bool ProbeInt3()
{
    VMP_BEGIN_MUT("VEH_ProbeInt3");
    if (!g_int3Stub) { VMP_END(); return false; }

    g_vehRan = false;
    bool caughtBySEH = false;
    __try {
        g_int3Stub();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        caughtBySEH = true;
    }

    const bool debugged = caughtBySEH || !g_vehRan;
    VMP_END();
    return debugged;
}

inline bool ProbeCloseHandleAbuse()
{
    VMP_BEGIN_MUT("VEH_ProbeCloseHandle");
    g_vehRan = false;
    bool caughtBySEH = false;
    BOOL ret = TRUE;
    __try {
        ret = CloseHandle(reinterpret_cast<HANDLE>(static_cast<uintptr_t>(0xDEADBEEF)));
    }
    __except (GetExceptionCode() == 0xC0000008 
              ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caughtBySEH = true;
    }

    (void)ret;
    const bool debugged = caughtBySEH || g_vehRan;
    VMP_END();
    return debugged;
}

}


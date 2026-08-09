

#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>
#include "../util/XorStr.h"
#include "VMProtectMarkers.h"

namespace IATIntegrity {

inline bool IEqPrefix(const char* s, const char* prefix) {
    if (!s || !prefix) return false;
    for (; *prefix; ++s, ++prefix) {
        char a = *s;
        char b = *prefix;
        if (!a) return false;
        if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 0x20);
        if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 0x20);
        if (a != b) return false;
    }
    return true;
}

inline bool IsModuleAddr(uintptr_t addr, HMODULE owner) {
    if (!addr || !owner) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)) != sizeof(mbi))
        return false;
    if (mbi.State != MEM_COMMIT) return false;

    if (mbi.Type != MEM_IMAGE) return false;
    return reinterpret_cast<HMODULE>(mbi.AllocationBase) == owner;
}

inline bool IsCrtImport(const char* dll) {
    return IEqPrefix(dll, "MSVCP140")
        || IEqPrefix(dll, "VCRUNTIME140")
        || IEqPrefix(dll, "api-ms-win-crt-")
        || IEqPrefix(dll, "ucrtbase");
}

inline int Scan(HMODULE self) {
    VMP_BEGIN_MUT("IATIntegrity_Scan");

    if (!self) { VMP_END(); return 0; }

    auto base = reinterpret_cast<uint8_t*>(self);
    auto dos  = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) { VMP_END(); return 0; }

    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) { VMP_END(); return 0; }

    const IMAGE_DATA_DIRECTORY& impDir =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (impDir.VirtualAddress == 0 || impDir.Size == 0) { VMP_END(); return 0; }

    auto desc = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(
        base + impDir.VirtualAddress);

    int suspicious = 0;

    for (; desc->Name != 0; ++desc) {
        const char* dllName = reinterpret_cast<const char*>(base + desc->Name);
        if (!dllName || !*dllName) continue;

        if (IsCrtImport(dllName)) continue;

        HMODULE owner = GetModuleHandleA(dllName);
        if (!owner) continue;

        auto thunk = reinterpret_cast<const IMAGE_THUNK_DATA*>(
            base + desc->FirstThunk);
        if (!thunk) continue;

        for (; thunk->u1.Function != 0; ++thunk) {

            uintptr_t addr = static_cast<uintptr_t>(thunk->u1.Function);
            if (!addr) continue;

            if (!IsModuleAddr(addr, owner)) {
                ++suspicious;
            }
        }
    }

    VMP_END();
    return suspicious;
}

}

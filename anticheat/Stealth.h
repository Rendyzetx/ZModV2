

#pragma once

#include <Windows.h>
#include <winternl.h>

namespace Stealth {

struct LdrDataTableEntryFull {
    LIST_ENTRY     InLoadOrderLinks;
    LIST_ENTRY     InMemoryOrderLinks;
    LIST_ENTRY     InInitializationOrderLinks;
    PVOID          DllBase;
    PVOID          EntryPoint;
    ULONG          SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;

};

inline void UnlinkListEntry(PLIST_ENTRY e) {
    if (!e || !e->Flink || !e->Blink) return;
    e->Blink->Flink = e->Flink;
    e->Flink->Blink = e->Blink;

    e->Flink = e;
    e->Blink = e;
}

inline void UnlinkSelfFromPeb(HMODULE self) {
    if (!self) return;

#ifdef _M_X64
    PPEB peb = reinterpret_cast<PPEB>(__readgsqword(0x60));
#else
    PPEB peb = reinterpret_cast<PPEB>(__readfsdword(0x30));
#endif
    if (!peb || !peb->Ldr) return;

    PPEB_LDR_DATA ldr = peb->Ldr;
    PLIST_ENTRY head = &ldr->InMemoryOrderModuleList;

    for (PLIST_ENTRY p = head->Flink; p && p != head; p = p->Flink) {

        auto* entry = reinterpret_cast<LdrDataTableEntryFull*>(
            reinterpret_cast<BYTE*>(p) - sizeof(LIST_ENTRY));

        if (entry->DllBase != self) continue;

        UnlinkListEntry(&entry->InLoadOrderLinks);
        UnlinkListEntry(&entry->InMemoryOrderLinks);
        UnlinkListEntry(&entry->InInitializationOrderLinks);

        if (entry->FullDllName.Buffer && entry->FullDllName.MaximumLength) {
            SecureZeroMemory(entry->FullDllName.Buffer,
                              entry->FullDllName.MaximumLength);
            entry->FullDllName.Length = 0;
        }
        if (entry->BaseDllName.Buffer && entry->BaseDllName.MaximumLength) {
            SecureZeroMemory(entry->BaseDllName.Buffer,
                              entry->BaseDllName.MaximumLength);
            entry->BaseDllName.Length = 0;
        }
        break;
    }
}

inline void EraseHeaderSignatures(HMODULE self) {
    if (!self) return;

    BYTE* base = reinterpret_cast<BYTE*>(self);
    DWORD oldProtect = 0;

    if (!VirtualProtect(base, 0x1000, PAGE_READWRITE, &oldProtect)) return;

    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic == IMAGE_DOS_SIGNATURE) {
        dos->e_magic = 0;
    }

    DWORD tmp = 0;
    VirtualProtect(base, 0x1000, oldProtect, &tmp);
}

inline void Apply(HMODULE self) {
    UnlinkSelfFromPeb(self);
    EraseHeaderSignatures(self);
}

}

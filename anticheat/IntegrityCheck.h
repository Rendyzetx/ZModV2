

#pragma once

#include <Windows.h>
#include <atomic>
#include <cstdint>
#include <mutex>

#include "VMProtectMarkers.h"

namespace IntegrityCheck {

inline uint32_t Crc32(const uint8_t* data, size_t len) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int k = 0; k < 8; ++k) {
            uint32_t mask = 0u - (crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & mask);
        }
    }
    return crc ^ 0xFFFFFFFFu;
}

inline uint32_t ComputeTextCrc() {
    VMP_BEGIN_VIRT("IntegrityCheck::ComputeTextCrc");

    HMODULE self = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&ComputeTextCrc),
            &self) || !self)
    {
        VMP_END();
        return 0;
    }

    auto base = reinterpret_cast<uintptr_t>(self);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        VMP_END();
        return 0;
    }

    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        VMP_END();
        return 0;
    }

    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
        if (sec->Name[0] == '.' &&
            sec->Name[1] == 't' &&
            sec->Name[2] == 'e' &&
            sec->Name[3] == 'x' &&
            sec->Name[4] == 't' &&
            sec->Name[5] == 0)
        {

            const uint8_t* p = reinterpret_cast<const uint8_t*>(
                base + sec->VirtualAddress);
            uint32_t crc = Crc32(p, sec->Misc.VirtualSize);
            VMP_END();
            return crc;
        }
    }

    VMP_END();
    return 0;
}

namespace detail {
    inline std::once_flag      g_pinOnce;
    inline std::atomic<uint32_t> g_pinnedCrc{ 0 };
}

inline uint32_t TextCrcPinned() {
    std::call_once(detail::g_pinOnce, [] {
        detail::g_pinnedCrc.store(ComputeTextCrc(),
                                  std::memory_order_release);
    });
    return detail::g_pinnedCrc.load(std::memory_order_acquire);
}

}

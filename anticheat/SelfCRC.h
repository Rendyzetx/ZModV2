

#pragma once

#include <Windows.h>
#include <bcrypt.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#pragma comment(lib, "Bcrypt.lib")

#include "VMProtectMarkers.h"
#include "../util/XorStr.h"

namespace SelfCRC {

namespace detail {

inline std::atomic<bool>   g_initialized{ false };
inline std::once_flag      g_initOnce;
inline const uint8_t*      g_textBase = nullptr;
inline size_t              g_textSize = 0;
inline uint8_t             g_baseline[32] = {};

inline bool Sha256Range(const uint8_t* base, size_t size, uint8_t out[32]) {
    if (!base || size == 0) return false;

    BCRYPT_ALG_HANDLE  hAlg  = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;

    if (BCryptOpenAlgorithmProvider(&hAlg, XSW(L"SHA256"), nullptr, 0) != 0)
        return false;

    DWORD objLen = 0, dataLen = 0;
    ULONG cb = 0;
    if (BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objLen,  sizeof(objLen),  &cb, 0) != 0 ||
        BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,   (PUCHAR)&dataLen, sizeof(dataLen), &cb, 0) != 0 ||
        dataLen != 32)
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    PUCHAR obj = (PUCHAR)HeapAlloc(GetProcessHeap(), 0, objLen);
    if (!obj) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    if (BCryptCreateHash(hAlg, &hHash, obj, objLen, nullptr, 0, 0) != 0) {
        HeapFree(GetProcessHeap(), 0, obj);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    bool ok = true;
    constexpr size_t kChunk = 1 << 16;
    __try {
        for (size_t off = 0; off < size; off += kChunk) {
            ULONG n = (ULONG)((size - off) < kChunk ? (size - off) : kChunk);
            if (BCryptHashData(hHash, (PUCHAR)(base + off), n, 0) != 0) { ok = false; break; }
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        ok = false;
    }

    if (ok) {
        if (BCryptFinishHash(hHash, out, dataLen, 0) != 0) ok = false;
    }

    BCryptDestroyHash(hHash);
    HeapFree(GetProcessHeap(), 0, obj);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok;
}

inline bool FindTextSection(HMODULE self, const uint8_t*& outBase, size_t& outSize) {
    if (!self) return false;
    const uint8_t* moduleBase = reinterpret_cast<const uint8_t*>(self);

    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(moduleBase);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;

    const IMAGE_NT_HEADERS* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(moduleBase + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return false;

    const IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (sec[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) {

            DWORD vsz = sec[i].Misc.VirtualSize;
            if (vsz == 0) continue;
            outBase = moduleBase + sec[i].VirtualAddress;
            outSize = static_cast<size_t>(vsz);
            return true;
        }
    }
    return false;
}

}

inline void Initialize(HMODULE self) {
    VMP_BEGIN_VIRT("SelfCRC_Init");

    std::call_once(detail::g_initOnce, [self]() {
        const uint8_t* base = nullptr;
        size_t         size = 0;
        if (!detail::FindTextSection(self, base, size)) {

            return;
        }

        uint8_t digest[32] = {};
        if (!detail::Sha256Range(base, size, digest)) return;

        detail::g_textBase = base;
        detail::g_textSize = size;
        std::memcpy(detail::g_baseline, digest, sizeof(digest));

        detail::g_initialized.store(true, std::memory_order_release);
    });

    VMP_END();
}

inline bool Initialized() {
    return detail::g_initialized.load(std::memory_order_acquire);
}

inline int Scan() {
    VMP_BEGIN_MUT("SelfCRC_Scan");

    if (!detail::g_initialized.load(std::memory_order_acquire)) {
        VMP_END();
        return 0;
    }

    uint8_t digest[32] = {};
    if (!detail::Sha256Range(detail::g_textBase, detail::g_textSize, digest)) {

        VMP_END();
        return 0;
    }

    int diff = std::memcmp(digest, detail::g_baseline, sizeof(digest)) != 0 ? 1 : 0;

    VMP_END();
    return diff;
}

}

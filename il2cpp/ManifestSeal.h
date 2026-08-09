

#pragma once

#include <Windows.h>
#include <bcrypt.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "VMProtectMarkers.h"

#pragma comment(lib, "Bcrypt.lib")

#ifndef NT_SUCCESS
#define NT_SUCCESS(s) ((s) >= 0)
#endif

namespace ManifestSeal {

constexpr uint32_t kVersionV1 = 1;
constexpr uint32_t kVersionV2 = 2;

inline void DeriveKey(uint8_t out[32]) {
    VMP_BEGIN_VIRT("ManifestSeal_DeriveKey");
    static const uint8_t stored[32] = {
        0xAB, 0xDD, 0x9F, 0x68, 0xA0, 0x50, 0x27, 0x50,
        0x33, 0xF0, 0xD1, 0x5E, 0xCD, 0x9B, 0xAA, 0x8B,
        0x40, 0x79, 0x94, 0x5B, 0x4E, 0xFC, 0x3E, 0x68,
        0xE6, 0x14, 0x1A, 0x0A, 0x90, 0x67, 0x4D, 0x45
    };
    uint32_t s = 0xC7E1A3F5u;
    for (int i = 0; i < 32; ++i) {
        s = s * 1664525u + 1013904223u;
        uint8_t pad = static_cast<uint8_t>((s >> 24) & 0xFF);
        out[i] = static_cast<uint8_t>(stored[i] ^ pad);
    }
    VMP_END();
}

inline bool Decrypt(const std::vector<uint8_t>& blob, std::string& out) {
    if (blob.size() < 8) return false;
    if (std::memcmp(blob.data(), "ZMAN", 4) != 0) return false;

    uint32_t ver = 0;
    std::memcpy(&ver, blob.data() + 4, 4);

    size_t nonceOff, tagOff, ctOff;
    if (ver == kVersionV1) {
        if (blob.size() <= 4 + 4 + 12 + 16) return false;
        nonceOff = 8; tagOff = 20; ctOff = 36;
    } else if (ver == kVersionV2) {
        if (blob.size() <= 4 + 4 + 8 + 12 + 16) return false;
        uint64_t valid_until_unix = 0;
        std::memcpy(&valid_until_unix, blob.data() + 8, 8);
        if (valid_until_unix) {
            uint64_t now_unix = static_cast<uint64_t>(std::time(nullptr));
            if (now_unix > valid_until_unix) {

                std::cout << "[ManifestSeal] sealed manifest expired (unix "
                          << valid_until_unix << " < now " << now_unix
                          << "); refusing to decrypt." << std::endl;
                return false;
            }
        }
        nonceOff = 16; tagOff = 28; ctOff = 44;
    } else {
        return false;
    }

    const uint8_t* nonce = blob.data() + nonceOff;
    const uint8_t* tag   = blob.data() + tagOff;
    const uint8_t* ct    = blob.data() + ctOff;
    const size_t   ctLen = blob.size() - ctOff;

    uint8_t key[32];
    DeriveKey(key);

    bool ok = false;
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (NT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0))) {
        if (NT_SUCCESS(BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
                (PUCHAR)BCRYPT_CHAIN_MODE_GCM, sizeof(BCRYPT_CHAIN_MODE_GCM), 0))) {
            BCRYPT_KEY_HANDLE hKey = nullptr;
            if (NT_SUCCESS(BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0, key, 32, 0))) {
                BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
                BCRYPT_INIT_AUTH_MODE_INFO(info);
                info.pbNonce = const_cast<PUCHAR>(nonce);
                info.cbNonce = 12;
                info.pbTag   = const_cast<PUCHAR>(tag);
                info.cbTag   = 16;

                std::vector<uint8_t> pt(ctLen);
                ULONG written = 0;
                NTSTATUS st = BCryptDecrypt(hKey,
                    const_cast<PUCHAR>(ct), (ULONG)ctLen,
                    &info, nullptr, 0,
                    pt.data(), (ULONG)pt.size(), &written, 0);
                if (NT_SUCCESS(st)) {
                    out.assign(reinterpret_cast<char*>(pt.data()), written);
                    ok = true;
                }
                SecureZeroMemory(pt.data(), pt.size());
                BCryptDestroyKey(hKey);
            }
        }
        BCryptCloseAlgorithmProvider(hAlg, 0);
    }
    SecureZeroMemory(key, sizeof(key));
    return ok;
}

}

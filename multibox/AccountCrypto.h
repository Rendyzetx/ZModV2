

#pragma once

#include <Windows.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Advapi32.lib")

#ifndef NT_SUCCESS
#define NT_SUCCESS(s) ((s) >= 0)
#endif

namespace AccountCrypto {

inline constexpr char     kMagic[4]      = { 'Z', 'A', 'C', 'C' };
inline constexpr uint32_t kVersion       = 1;
inline constexpr size_t   kIvBytes       = 12;
inline constexpr size_t   kTagBytes      = 16;
inline constexpr size_t   kKeyBytes      = 32;
inline constexpr size_t   kHeaderBytes   = 4 + 4 + kIvBytes + kTagBytes;
inline constexpr const char* kKeySalt    = "ZMod-Acct-v1";

inline std::string ReadMachineGuid() {
    HKEY hKey = nullptr;
    LSTATUS rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Cryptography", 0,
        KEY_READ | KEY_WOW64_64KEY, &hKey);
    if (rc != ERROR_SUCCESS) return {};

    char  buf[128] = {};
    DWORD sz = sizeof(buf);
    rc = RegQueryValueExA(hKey, "MachineGuid", nullptr, nullptr,
        reinterpret_cast<BYTE*>(buf), &sz);
    RegCloseKey(hKey);
    if (rc != ERROR_SUCCESS) return {};

    return std::string(buf, sz > 0 ? sz - 1 : 0);
}

inline std::string ReadVolumeSerial() {
    DWORD serial = 0;
    if (!GetVolumeInformationA("C:\\", nullptr, 0, &serial, nullptr, nullptr,
        nullptr, 0))
    {
        return {};
    }
    char hex[16] = {};
    sprintf_s(hex, "%08X", serial);
    return std::string(hex);
}

inline bool DeriveKey(uint8_t outKey[kKeyBytes]) {
    std::string material = ReadMachineGuid() + ReadVolumeSerial() + kKeySalt;
    if (material.size() < 16) return false;

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS s = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
    if (!NT_SUCCESS(s)) return false;

    BCRYPT_HASH_HANDLE hHash = nullptr;
    s = BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0);
    if (!NT_SUCCESS(s)) { BCryptCloseAlgorithmProvider(hAlg, 0); return false; }

    BCryptHashData(hHash,
        reinterpret_cast<UCHAR*>(material.data()),
        static_cast<ULONG>(material.size()), 0);

    s = BCryptFinishHash(hHash, outKey, kKeyBytes, 0);

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return NT_SUCCESS(s);
}

inline bool RandomIv(uint8_t out[kIvBytes]) {
    NTSTATUS s = BCryptGenRandom(nullptr, out, kIvBytes,
        BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return NT_SUCCESS(s);
}

inline std::vector<uint8_t> Encrypt(const std::string& plaintext) {
    uint8_t key[kKeyBytes];
    if (!DeriveKey(key)) return {};

    uint8_t iv[kIvBytes];
    if (!RandomIv(iv)) return {};

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM,
        nullptr, 0))) return {};

    if (!NT_SUCCESS(BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
        (PUCHAR)BCRYPT_CHAIN_MODE_GCM,
        sizeof(BCRYPT_CHAIN_MODE_GCM), 0)))
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    BCRYPT_KEY_HANDLE hKey = nullptr;
    if (!NT_SUCCESS(BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0,
        key, kKeyBytes, 0)))
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    uint8_t tag[kTagBytes] = {};
    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
    BCRYPT_INIT_AUTH_MODE_INFO(info);
    info.pbNonce      = iv;
    info.cbNonce      = kIvBytes;
    info.pbTag        = tag;
    info.cbTag        = kTagBytes;

    std::vector<uint8_t> ciphertext(plaintext.size());
    ULONG written = 0;
    NTSTATUS s = BCryptEncrypt(hKey,
        (PUCHAR)plaintext.data(), (ULONG)plaintext.size(),
        &info, nullptr, 0,
        ciphertext.data(), (ULONG)ciphertext.size(), &written, 0);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    SecureZeroMemory(key, sizeof(key));

    if (!NT_SUCCESS(s)) return {};
    ciphertext.resize(written);

    std::vector<uint8_t> out;
    out.reserve(kHeaderBytes + ciphertext.size());
    out.insert(out.end(), kMagic, kMagic + 4);
    uint32_t ver = kVersion;
    out.insert(out.end(), (uint8_t*)&ver, (uint8_t*)&ver + 4);
    out.insert(out.end(), iv,  iv  + kIvBytes);
    out.insert(out.end(), tag, tag + kTagBytes);
    out.insert(out.end(), ciphertext.begin(), ciphertext.end());
    return out;
}

inline std::string Decrypt(const std::vector<uint8_t>& blob) {
    if (blob.size() < kHeaderBytes) return {};
    if (memcmp(blob.data(), kMagic, 4) != 0) return {};
    uint32_t ver = 0;
    memcpy(&ver, blob.data() + 4, 4);
    if (ver != kVersion) return {};

    const uint8_t* iv         = blob.data() + 8;
    const uint8_t* tag        = blob.data() + 8 + kIvBytes;
    const uint8_t* ciphertext = blob.data() + kHeaderBytes;
    const size_t   ctLen      = blob.size()  - kHeaderBytes;

    uint8_t key[kKeyBytes];
    if (!DeriveKey(key)) return {};

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM,
        nullptr, 0))) return {};

    if (!NT_SUCCESS(BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
        (PUCHAR)BCRYPT_CHAIN_MODE_GCM,
        sizeof(BCRYPT_CHAIN_MODE_GCM), 0)))
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    BCRYPT_KEY_HANDLE hKey = nullptr;
    if (!NT_SUCCESS(BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0,
        key, kKeyBytes, 0)))
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
    BCRYPT_INIT_AUTH_MODE_INFO(info);
    info.pbNonce      = (PUCHAR)iv;
    info.cbNonce      = kIvBytes;
    info.pbTag        = (PUCHAR)tag;
    info.cbTag        = kTagBytes;

    std::vector<uint8_t> plaintext(ctLen);
    ULONG written = 0;
    NTSTATUS s = BCryptDecrypt(hKey,
        (PUCHAR)ciphertext, (ULONG)ctLen,
        &info, nullptr, 0,
        plaintext.data(), (ULONG)plaintext.size(), &written, 0);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    SecureZeroMemory(key, sizeof(key));

    if (!NT_SUCCESS(s)) return {};

    return std::string(reinterpret_cast<char*>(plaintext.data()), written);
}

inline std::string NewUuid() {
    uint8_t b[16] = {};
    BCryptGenRandom(nullptr, b, 16, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    b[6] = (b[6] & 0x0F) | 0x40;
    b[8] = (b[8] & 0x3F) | 0x80;

    char out[40] = {};
    sprintf_s(out, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        b[0], b[1], b[2],  b[3],  b[4],  b[5],  b[6],  b[7],
        b[8], b[9], b[10], b[11], b[12], b[13], b[14], b[15]);
    return std::string(out);
}

}

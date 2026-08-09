

#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <bcrypt.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <streambuf>
#include <string>
#include <vector>

#pragma comment(lib, "Bcrypt.lib")

#ifndef NT_SUCCESS
#define NT_SUCCESS(s) ((s) >= 0)
#endif

namespace LogStream {

inline const unsigned char g_RsaPubKeyBlob[] = {
    0x52, 0x53, 0x41, 0x31, 0x00, 0x08, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x01, 0xBA, 0x7D, 0xAF, 0x3B, 0xA8, 0x84, 0x20, 0xEF, 0x8E,
    0x14, 0xFC, 0x97, 0x94, 0x82, 0x60, 0x29, 0xDC, 0x5E, 0xB5, 0x0C, 0xDF,
    0xF8, 0x96, 0xD3, 0x57, 0xB0, 0xE3, 0x42, 0xE9, 0x82, 0x5F, 0xC6, 0xDD,
    0xE7, 0xB7, 0xE5, 0x7B, 0x4F, 0xF6, 0xD6, 0x7C, 0xA7, 0x63, 0xE6, 0x89,
    0xA8, 0x46, 0x64, 0x9B, 0x40, 0x53, 0xCF, 0xBA, 0x32, 0x38, 0xFB, 0x72,
    0x4B, 0xBA, 0x1D, 0xF4, 0x2C, 0xE4, 0xCA, 0x12, 0x9E, 0x7E, 0xA5, 0xD0,
    0x9D, 0x7D, 0xAD, 0x05, 0xAA, 0x3D, 0x5E, 0x44, 0xDF, 0x1D, 0x89, 0xEB,
    0x86, 0x17, 0xEC, 0xE3, 0x8B, 0x9C, 0xF1, 0x70, 0xBD, 0xAA, 0xEB, 0xDF,
    0x1A, 0xE8, 0xC9, 0x82, 0x37, 0x26, 0xFA, 0x40, 0xC9, 0x7B, 0x19, 0xA2,
    0xC7, 0x49, 0xC2, 0xB3, 0x6F, 0x67, 0x35, 0xD6, 0x7D, 0xF9, 0xF8, 0x16,
    0x76, 0xA2, 0x3B, 0x3B, 0x92, 0x94, 0xAD, 0x56, 0x45, 0x11, 0x06, 0xAF,
    0x54, 0x06, 0x88, 0x39, 0x99, 0x69, 0x96, 0xEA, 0x7A, 0x34, 0x4B, 0x91,
    0x72, 0x8A, 0x06, 0xC3, 0xD1, 0x9F, 0x92, 0xE6, 0x86, 0x49, 0x51, 0xFF,
    0x26, 0x87, 0x43, 0x1A, 0x02, 0xBC, 0xF4, 0xD7, 0x15, 0x6F, 0x4B, 0x7E,
    0x77, 0x25, 0x80, 0x16, 0xC2, 0xB0, 0x31, 0x3E, 0x76, 0x01, 0xEC, 0x84,
    0x7D, 0xD2, 0x78, 0x90, 0x20, 0x29, 0x89, 0x4B, 0xA6, 0x16, 0x7E, 0xB7,
    0x72, 0x78, 0xDF, 0x5F, 0x8F, 0x25, 0xAA, 0xD0, 0x8E, 0xC9, 0x26, 0x15,
    0x93, 0xBF, 0x32, 0x82, 0xE3, 0x83, 0xC4, 0x61, 0x3B, 0x67, 0xA8, 0x7D,
    0xAD, 0x81, 0xC4, 0x53, 0xC8, 0xBB, 0x53, 0x78, 0x62, 0x24, 0x08, 0xF0,
    0xCB, 0x17, 0x46, 0x50, 0x0C, 0xBC, 0x9A, 0xAA, 0x04, 0xE3, 0x06, 0x8A,
    0x97, 0x23, 0x7E, 0xB3, 0x37, 0x32, 0x1D, 0xD2, 0x09, 0x89, 0x4C, 0x77,
    0xE5, 0x53, 0x2E, 0xCB, 0x4D, 0x06, 0x31
};

struct State {
    std::ofstream     file;
    std::mutex        mutex;
    BCRYPT_KEY_HANDLE aesKey      = nullptr;
    BCRYPT_ALG_HANDLE aesAlg      = nullptr;
    uint8_t           ivPrefix[12]= {};
    std::atomic<uint64_t> lineCounter{ 0 };
    bool              initialized = false;
};

inline State& Get() {
    static State s;
    return s;
}

inline std::string LogFilePath() {
    PWSTR appData = nullptr;
    std::string base = "C:\\";
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appData))) {
        char buf[MAX_PATH] = {};
        WideCharToMultiByte(CP_UTF8, 0, appData, -1, buf, sizeof(buf), nullptr, nullptr);
        CoTaskMemFree(appData);
        base = buf;
    }
    std::string dir = base + "\\ZMod\\logs";
    CreateDirectoryA((base + "\\ZMod").c_str(), nullptr);
    CreateDirectoryA(dir.c_str(),                     nullptr);

    char name[64];
    std::snprintf(name, sizeof(name), "\\session-%lu-%llu.zlog",
        (unsigned long)GetCurrentProcessId(),
        (unsigned long long)GetTickCount64());
    return dir + name;
}

inline std::vector<uint8_t> RsaSealKey(const uint8_t* pt, size_t ptLen) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_RSA_ALGORITHM,
        nullptr, 0))) return {};

    BCRYPT_KEY_HANDLE hKey = nullptr;
    NTSTATUS s = BCryptImportKeyPair(hAlg, nullptr, BCRYPT_RSAPUBLIC_BLOB,
        &hKey,
        const_cast<PUCHAR>(reinterpret_cast<const UCHAR*>(g_RsaPubKeyBlob)),
        sizeof(g_RsaPubKeyBlob), 0);
    if (!NT_SUCCESS(s)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    BCRYPT_OAEP_PADDING_INFO oaep = {};
    oaep.pszAlgId = BCRYPT_SHA256_ALGORITHM;
    oaep.pbLabel  = nullptr;
    oaep.cbLabel  = 0;

    DWORD ctLen = 0;
    s = BCryptEncrypt(hKey,
        const_cast<PUCHAR>(pt), (ULONG)ptLen,
        &oaep, nullptr, 0, nullptr, 0, &ctLen, BCRYPT_PAD_OAEP);
    if (!NT_SUCCESS(s)) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    std::vector<uint8_t> ct(ctLen);
    s = BCryptEncrypt(hKey,
        const_cast<PUCHAR>(pt), (ULONG)ptLen,
        &oaep, nullptr, 0, ct.data(), ctLen, &ctLen, BCRYPT_PAD_OAEP);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    if (!NT_SUCCESS(s)) return {};
    ct.resize(ctLen);
    return ct;
}

inline bool ImportAesKey(State& s, const uint8_t key[32]) {
    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(&s.aesAlg, BCRYPT_AES_ALGORITHM,
        nullptr, 0))) return false;
    if (!NT_SUCCESS(BCryptSetProperty(s.aesAlg, BCRYPT_CHAINING_MODE,
        (PUCHAR)BCRYPT_CHAIN_MODE_GCM, sizeof(BCRYPT_CHAIN_MODE_GCM), 0)))
        return false;
    if (!NT_SUCCESS(BCryptGenerateSymmetricKey(s.aesAlg, &s.aesKey,
        nullptr, 0, const_cast<PUCHAR>(key), 32, 0)))
        return false;
    return true;
}

class EncryptedStreambuf : public std::streambuf {
public:
    int_type overflow(int_type ch) override {
        if (ch == EOF) return 0;
        char c = static_cast<char>(ch);
        m_buf.push_back(c);
        if (c == '\n' || m_buf.size() >= kMaxLineBytes) FlushRecord();
        return ch;
    }

    int sync() override {
        if (!m_buf.empty()) FlushRecord();
        return 0;
    }

private:
    static constexpr size_t kMaxLineBytes = 4096;
    std::vector<char> m_buf;

    void FlushRecord() {
        State& s = Get();
        if (!s.initialized) { m_buf.clear(); return; }

        std::lock_guard<std::mutex> lk(s.mutex);

        uint8_t nonce[12] = {};
        std::memcpy(nonce, s.ivPrefix, 4);
        uint64_t ctr = s.lineCounter.fetch_add(1);
        std::memcpy(nonce + 4, &ctr, 8);

        BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
        BCRYPT_INIT_AUTH_MODE_INFO(info);
        uint8_t tag[16] = {};
        info.pbNonce = nonce;
        info.cbNonce = sizeof(nonce);
        info.pbTag   = tag;
        info.cbTag   = sizeof(tag);

        std::vector<uint8_t> ct(m_buf.size());
        ULONG written = 0;
        NTSTATUS st = BCryptEncrypt(s.aesKey,
            (PUCHAR)m_buf.data(), (ULONG)m_buf.size(),
            &info, nullptr, 0,
            ct.data(), (ULONG)ct.size(), &written, 0);

        if (NT_SUCCESS(st)) {
            uint32_t lineLen = (uint32_t)written;
            s.file.write(reinterpret_cast<const char*>(&lineLen), 4);
            s.file.write(reinterpret_cast<const char*>(nonce),    12);
            s.file.write(reinterpret_cast<const char*>(tag),      16);
            s.file.write(reinterpret_cast<const char*>(ct.data()), written);
            s.file.flush();
        } else {

            uint32_t marker = 0xFFFFFFFF;
            s.file.write(reinterpret_cast<const char*>(&marker), 4);
            s.file.flush();
        }

        m_buf.clear();
    }
};

inline EncryptedStreambuf& Streambuf() {
    static EncryptedStreambuf sb;
    return sb;
}

inline bool Init() {
    State& s = Get();
    if (s.initialized) return true;

    std::string path = LogFilePath();
    s.file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!s.file) return false;

    uint8_t aesKey[32] = {};
    uint8_t ivPrefix[12] = {};
    if (!NT_SUCCESS(BCryptGenRandom(nullptr, aesKey,    sizeof(aesKey),    BCRYPT_USE_SYSTEM_PREFERRED_RNG)) ||
        !NT_SUCCESS(BCryptGenRandom(nullptr, ivPrefix,  sizeof(ivPrefix),  BCRYPT_USE_SYSTEM_PREFERRED_RNG)))
    {
        s.file.close();
        return false;
    }
    std::memcpy(s.ivPrefix, ivPrefix, sizeof(ivPrefix));

    uint8_t pt[32 + 12] = {};
    std::memcpy(pt,      aesKey,   32);
    std::memcpy(pt + 32, ivPrefix, 12);
    auto sealed = RsaSealKey(pt, sizeof(pt));
    SecureZeroMemory(pt, sizeof(pt));
    if (sealed.size() != 256) {
        SecureZeroMemory(aesKey, sizeof(aesKey));
        s.file.close();
        return false;
    }

    const char     magic[4] = { 'Z', 'L', 'O', 'G' };
    const uint32_t version  = 1;
    const uint32_t headerLen = 4 + 4 + 4 + 4 + 256;
    const uint32_t rsaLen    = 256;
    s.file.write(magic, 4);
    s.file.write(reinterpret_cast<const char*>(&version),  4);
    s.file.write(reinterpret_cast<const char*>(&headerLen), 4);
    s.file.write(reinterpret_cast<const char*>(&rsaLen),    4);
    s.file.write(reinterpret_cast<const char*>(sealed.data()), rsaLen);
    s.file.flush();

    if (!ImportAesKey(s, aesKey)) {
        SecureZeroMemory(aesKey, sizeof(aesKey));
        s.file.close();
        return false;
    }
    SecureZeroMemory(aesKey, sizeof(aesKey));

    s.initialized = true;

    std::cout.rdbuf(&Streambuf());
    std::cerr.rdbuf(&Streambuf());

    return true;
}

inline void Shutdown() {
    State& s = Get();
    if (!s.initialized) return;
    std::lock_guard<std::mutex> lk(s.mutex);
    if (s.aesKey)  { BCryptDestroyKey(s.aesKey);              s.aesKey = nullptr; }
    if (s.aesAlg)  { BCryptCloseAlgorithmProvider(s.aesAlg, 0); s.aesAlg = nullptr; }
    if (s.file)    { s.file.flush(); s.file.close(); }
    s.initialized = false;
}

}



#pragma once

#include <Windows.h>
#include <bcrypt.h>
#include <winhttp.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "DllAuthClient.h"
#include "ManifestSeal.h"
#include "ManifestSecret.gen.h"
#include "CertPin.h"
#include "IndirectImports.h"
#include "XorStr.h"
#include "VMProtectMarkers.h"

#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Winhttp.lib")

#ifndef NT_SUCCESS
#define NT_SUCCESS(s) ((s) >= 0)
#endif

namespace ManifestFetch {

inline bool Sha256FileHex(const char* path, std::string& outHex) {
    outHex.clear();
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0)))
        return false;

    bool ok = false;
    DWORD objLen = 0, dataLen = 0;
    ULONG cb = 0;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objLen,  sizeof(objLen),  &cb, 0);
    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,   (PUCHAR)&dataLen, sizeof(dataLen), &cb, 0);
    if (dataLen == 32) {
        std::vector<UCHAR> obj(objLen);
        BCRYPT_HASH_HANDLE hHash = nullptr;
        if (NT_SUCCESS(BCryptCreateHash(hAlg, &hHash, obj.data(), objLen, nullptr, 0, 0))) {
            std::vector<char> buf(1 << 20);
            ok = true;
            while (f) {
                f.read(buf.data(), (std::streamsize)buf.size());
                std::streamsize n = f.gcount();
                if (n > 0) {
                    if (!NT_SUCCESS(BCryptHashData(hHash, (PUCHAR)buf.data(), (ULONG)n, 0))) {
                        ok = false;
                        break;
                    }
                }
            }
            uint8_t digest[32] = {};
            if (ok && !NT_SUCCESS(BCryptFinishHash(hHash, digest, 32, 0))) ok = false;
            if (ok) {
                static const char hexd[] = "0123456789abcdef";
                outHex.resize(64);
                for (int i = 0; i < 32; ++i) {
                    outHex[i * 2 + 0] = hexd[(digest[i] >> 4) & 0xF];
                    outHex[i * 2 + 1] = hexd[(digest[i] >> 0) & 0xF];
                }
            }
            SecureZeroMemory(digest, sizeof(digest));
            BCryptDestroyHash(hHash);
            SecureZeroMemory(buf.data(), buf.size());
        }
    }
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok;
}

inline bool HmacSha256(const uint8_t* key, size_t keyLen,
                       const uint8_t* data, size_t dataLen,
                       uint8_t out[32]) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(
            &hAlg, BCRYPT_SHA256_ALGORITHM, nullptr,
            BCRYPT_ALG_HANDLE_HMAC_FLAG))) return false;

    BCRYPT_HASH_HANDLE hHash = nullptr;
    bool ok = false;
    if (NT_SUCCESS(BCryptCreateHash(
            hAlg, &hHash, nullptr, 0,
            const_cast<PUCHAR>(key), (ULONG)keyLen, 0))) {
        if (NT_SUCCESS(BCryptHashData(hHash, const_cast<PUCHAR>(data), (ULONG)dataLen, 0)) &&
            NT_SUCCESS(BCryptFinishHash(hHash, out, 32, 0))) {
            ok = true;
        }
        BCryptDestroyHash(hHash);
    }
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok;
}

inline bool AesGcmDecrypt(const uint8_t key[32],
                          const uint8_t* nonce, size_t nonceLen,
                          const uint8_t* tag,   size_t tagLen,
                          const uint8_t* ct,    size_t ctLen,
                          std::vector<uint8_t>& out) {
    bool ok = false;
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (NT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0))) {
        if (NT_SUCCESS(BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE,
                (PUCHAR)BCRYPT_CHAIN_MODE_GCM, sizeof(BCRYPT_CHAIN_MODE_GCM), 0))) {
            BCRYPT_KEY_HANDLE hKey = nullptr;
            if (NT_SUCCESS(BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0,
                    const_cast<PUCHAR>(key), 32, 0))) {
                BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
                BCRYPT_INIT_AUTH_MODE_INFO(info);
                info.pbNonce = const_cast<PUCHAR>(nonce);
                info.cbNonce = (ULONG)nonceLen;
                info.pbTag   = const_cast<PUCHAR>(tag);
                info.cbTag   = (ULONG)tagLen;

                out.resize(ctLen);
                ULONG written = 0;
                NTSTATUS st = BCryptDecrypt(hKey,
                    const_cast<PUCHAR>(ct), (ULONG)ctLen,
                    &info, nullptr, 0,
                    out.data(), (ULONG)out.size(), &written, 0);
                if (NT_SUCCESS(st)) {
                    out.resize(written);
                    ok = true;
                } else {
                    SecureZeroMemory(out.data(), out.size());
                    out.clear();
                }
                BCryptDestroyKey(hKey);
            }
        }
        BCryptCloseAlgorithmProvider(hAlg, 0);
    }
    return ok;
}

inline bool HttpsPost(const wchar_t* host, const wchar_t* path,
                      const std::string& jwt, const std::string& body,
                      DWORD& outStatus, std::vector<uint8_t>& outBody,
                      std::string& outErr) {
    outStatus = 0;
    outBody.clear();

    HINTERNET hSess = IMP(winhttp.dll, WinHttpOpen)(
        XSW(L"ZwSpark/1.0"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSess) { outErr = "manifest: network"; return false; }
    IMP(winhttp.dll, WinHttpSetTimeouts)(hSess, 5000, 5000, 10000, 15000);

    HINTERNET hConn = IMP(winhttp.dll, WinHttpConnect)(
        hSess, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConn) {
        IMP(winhttp.dll, WinHttpCloseHandle)(hSess);
        outErr = "manifest: network";
        return false;
    }

    HINTERNET hReq = IMP(winhttp.dll, WinHttpOpenRequest)(
        hConn, XSW(L"POST"), path, nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hReq) {
        IMP(winhttp.dll, WinHttpCloseHandle)(hConn);
        IMP(winhttp.dll, WinHttpCloseHandle)(hSess);
        outErr = "manifest: network";
        return false;
    }

    {

        const wchar_t* ctHdr = XSW(L"Content-Type: application/json\r\n"
                                    L"X-ZMod-Version: v2.01b\r\n");
        IMP(winhttp.dll, WinHttpAddRequestHeaders)(hReq, ctHdr, (DWORD)-1L,
            WINHTTP_ADDREQ_FLAG_ADD);
    }
    if (!jwt.empty()) {
        std::wstring hdr = L"Authorization: Bearer ";
        hdr.reserve(hdr.size() + jwt.size() + 2);
        for (char c : jwt) hdr.push_back(static_cast<wchar_t>(c));
        hdr += L"\r\n";
        IMP(winhttp.dll, WinHttpAddRequestHeaders)(hReq, hdr.c_str(),
            (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD);
    }

    bool sent = IMP(winhttp.dll, WinHttpSendRequest)(
        hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)body.data(), (DWORD)body.size(),
        (DWORD)body.size(), 0) != FALSE;
    if (sent) sent = IMP(winhttp.dll, WinHttpReceiveResponse)(hReq, nullptr) != FALSE;

    if (sent && !CertPin::VerifyServerCertPin(hReq)) {
        outErr = "manifest: cert pin";
        IMP(winhttp.dll, WinHttpCloseHandle)(hReq);
        IMP(winhttp.dll, WinHttpCloseHandle)(hConn);
        IMP(winhttp.dll, WinHttpCloseHandle)(hSess);
        return false;
    }

    bool ok = false;
    if (sent) {
        DWORD code = 0;
        DWORD cb = sizeof(code);
        IMP(winhttp.dll, WinHttpQueryHeaders)(
            hReq, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            nullptr, &code, &cb, WINHTTP_NO_HEADER_INDEX);
        outStatus = code;

        for (;;) {
            DWORD avail = 0;
            if (!IMP(winhttp.dll, WinHttpQueryDataAvailable)(hReq, &avail) || !avail) break;
            size_t off = outBody.size();
            outBody.resize(off + avail);
            DWORD read = 0;
            if (!IMP(winhttp.dll, WinHttpReadData)(hReq, outBody.data() + off, avail, &read) || !read) {
                outBody.resize(off);
                break;
            }
            outBody.resize(off + read);
        }
        ok = true;
    } else {
        outErr = "manifest: network";
    }

    IMP(winhttp.dll, WinHttpCloseHandle)(hReq);
    IMP(winhttp.dll, WinHttpCloseHandle)(hConn);
    IMP(winhttp.dll, WinHttpCloseHandle)(hSess);
    return ok;
}

inline bool GameAssemblyPath(std::string& outPath) {
    HMODULE h = GetModuleHandleA("GameAssembly.dll");
    if (!h) return false;
    char buf[MAX_PATH] = {};
    DWORD n = GetModuleFileNameA(h, buf, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return false;
    outPath.assign(buf, n);
    return true;
}

inline std::string JsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 4);
    for (char c : s) {
        if (c == '\\' || c == '"') { out.push_back('\\'); out.push_back(c); }
        else if (c == '\n') { out.append("\\n"); }
        else if (c == '\r') { out.append("\\r"); }
        else if (c == '\t') { out.append("\\t"); }
        else if (static_cast<unsigned char>(c) < 0x20) {
            char esc[8] = {};
            std::snprintf(esc, sizeof(esc), "\\u%04x", static_cast<unsigned int>(c) & 0xFF);
            out.append(esc);
        } else {
            out.push_back(c);
        }
    }
    return out;
}

inline bool Fetch(std::string& outPlaintextJson, std::string& outErr) {
    VMP_BEGIN_VIRT("ManifestFetch_Fetch");

    outPlaintextJson.clear();
    outErr.clear();

    std::string jwt = DllAuthClient::CurrentJwt();
    int64_t     iat = DllAuthClient::CurrentIat();
    std::string fp  = DllAuthClient::CurrentFingerprint();
    std::string user = DllAuthClient::CurrentUser();
    if (jwt.empty() || iat == 0 || fp.empty() || user.empty()) {
        outErr = "manifest: no session";
        VMP_END();
        return false;
    }

    std::string gaPath, gaSha;
    if (!GameAssemblyPath(gaPath)) {
        outErr = "manifest: no game module";
        VMP_END();
        return false;
    }
    if (!Sha256FileHex(gaPath.c_str(), gaSha)) {
        outErr = "manifest: sha256 fail";
        VMP_END();
        return false;
    }

    std::string body;
    body.reserve(jwt.size() + fp.size() + user.size() + gaSha.size() + 96);
    body.append("{\"token\":\"");        body.append(JsonEscape(jwt));
    body.append("\",\"fingerprint\":\""); body.append(JsonEscape(fp));
    body.append("\",\"username\":\"");   body.append(JsonEscape(user));
    body.append("\",\"game_sha256\":\""); body.append(gaSha);
    body.append("\"}");

    DWORD status = 0;
    std::vector<uint8_t> frame;
    if (!HttpsPost(XSW(L"zmod-auth.example.workers.dev"),
                   XSW(L"/manifest"),
                   jwt, body, status, frame, outErr)) {

        SecureZeroMemory(body.data(), body.size());
        VMP_END();
        return false;
    }
    SecureZeroMemory(body.data(), body.size());

    if (status != 200) {
        char buf[40] = {};
        std::snprintf(buf, sizeof(buf), "manifest: http %lu", static_cast<unsigned long>(status));
        outErr = buf;
        VMP_END();
        return false;
    }

    constexpr size_t kHdr = 4 + 4 + 12 + 16;
    if (frame.size() <= kHdr) {
        outErr = "manifest: bad frame";
        VMP_END();
        return false;
    }
    if (std::memcmp(frame.data(), "ZMWO", 4) != 0) {
        outErr = "manifest: bad frame";
        VMP_END();
        return false;
    }
    uint32_t frameVer = 0;
    std::memcpy(&frameVer, frame.data() + 4, 4);
    if (frameVer != 1u) {
        outErr = "manifest: bad frame";
        VMP_END();
        return false;
    }
    const uint8_t* nonce = frame.data() + 8;
    const uint8_t* tag   = frame.data() + 20;
    const uint8_t* ct    = frame.data() + 36;
    const size_t   ctLen = frame.size() - 36;

    uint8_t  secretBytes[32] = {};
    char     secretHex[65]   = {};
    bool secretOk = ManifestSecret::Decode(secretHex);
    if (secretOk) secretOk = ManifestSecret::HexAsciiToBytes(secretHex, secretBytes);
    SecureZeroMemory(secretHex, sizeof(secretHex));
    if (!secretOk) {
        SecureZeroMemory(secretBytes, sizeof(secretBytes));
        SecureZeroMemory(frame.data(), frame.size());
        outErr = "manifest: embedded MANIFEST_SECRET is the placeholder. Regenerate via tools/encode_manifest_secret.ps1.";
        VMP_END();
        return false;
    }

    char hdrBuf[64] = {};
    int hdrN = std::snprintf(hdrBuf, sizeof(hdrBuf), "%lld:", (long long)iat);
    std::string canon;
    canon.reserve(static_cast<size_t>(hdrN) + fp.size() + user.size() + 1);
    canon.append(hdrBuf, hdrBuf + hdrN);
    canon.append(fp);
    canon.push_back(':');
    canon.append(user);

    uint8_t sessionKey[32] = {};
    bool hmacOk = HmacSha256(secretBytes, sizeof(secretBytes),
                             reinterpret_cast<const uint8_t*>(canon.data()),
                             canon.size(), sessionKey);
    SecureZeroMemory(secretBytes, sizeof(secretBytes));
    SecureZeroMemory(canon.data(), canon.size());
    if (!hmacOk) {
        SecureZeroMemory(sessionKey, sizeof(sessionKey));
        SecureZeroMemory(frame.data(), frame.size());
        outErr = "manifest: hmac fail";
        VMP_END();
        return false;
    }

    std::vector<uint8_t> sealedBlob;
    bool gcmOk = AesGcmDecrypt(sessionKey, nonce, 12, tag, 16, ct, ctLen, sealedBlob);
    SecureZeroMemory(sessionKey, sizeof(sessionKey));
    SecureZeroMemory(frame.data(), frame.size());
    if (!gcmOk) {
        if (!sealedBlob.empty()) SecureZeroMemory(sealedBlob.data(), sealedBlob.size());
        outErr = "manifest: outer auth";
        VMP_END();
        return false;
    }

    std::string plaintext;
    bool sealOk = ManifestSeal::Decrypt(sealedBlob, plaintext);
    SecureZeroMemory(sealedBlob.data(), sealedBlob.size());
    if (!sealOk) {

        outErr = "manifest: inner seal";
        if (!plaintext.empty()) SecureZeroMemory(plaintext.data(), plaintext.size());
        VMP_END();
        return false;
    }

    outPlaintextJson = std::move(plaintext);
    VMP_END();
    return true;
}

}

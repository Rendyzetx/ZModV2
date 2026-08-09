

#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <winhttp.h>
#include <bcrypt.h>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "XorStr.h"
#include "IndirectImports.h"
#include "CertPin.h"
#include "DllAuthSecret.gen.h"

#include "../anticheat/IntegrityCheck.h"
#include "RuntimeBlob.h"
#include "ExitFlags.h"
#include "../core/ProxyConfig.h"
#include "../core/ProxyPool.h"

namespace DllAuthClient {

enum class Result {
    Ok,
    Transient,
    Permanent,
    Skipped
};

inline std::mutex          g_sessionMutex;
inline std::string         g_sessionJwt;
inline int64_t             g_sessionExp = 0;

inline int64_t             g_sessionIat = 0;
inline std::string         g_sessionFp;
inline std::string         g_sessionUser;
inline std::atomic<bool>   g_authenticated{ false };
inline std::atomic<bool>   g_authFatal{ false };

inline std::string         g_lastErrorMsg;

inline std::string         g_sessionToken;
inline std::string         g_fingerprint;
inline std::thread         g_heartbeat;
inline std::atomic<bool>   g_heartbeatRunning{ false };
inline std::atomic<int>    g_heartbeatStrikes{ 0 };

inline void HeartbeatLoop();
inline void StartHeartbeat() {
    if (!g_heartbeatRunning.exchange(true)) {
        g_heartbeat = std::thread(HeartbeatLoop);
        g_heartbeat.detach();
    }
}

inline std::string CurrentJwt() {
    std::lock_guard<std::mutex> lk(g_sessionMutex);
    return g_sessionJwt;
}
inline int64_t CurrentIat() {
    std::lock_guard<std::mutex> lk(g_sessionMutex);
    return g_sessionIat;
}
inline std::string CurrentFingerprint() {
    std::lock_guard<std::mutex> lk(g_sessionMutex);
    return g_sessionFp;
}
inline std::string CurrentUser() {
    std::lock_guard<std::mutex> lk(g_sessionMutex);
    return g_sessionUser;
}
inline void ClearSession() {
    std::lock_guard<std::mutex> lk(g_sessionMutex);
    g_sessionJwt.clear();
    g_sessionExp = 0;
    g_sessionIat = 0;
    g_sessionFp.clear();
    g_sessionUser.clear();
    g_authenticated.store(false, std::memory_order_release);
}

inline void WriteAuthErrorFile(const std::string& msg) {
    PWSTR appData = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appData))) return;
    char base[MAX_PATH] = {};
    WideCharToMultiByte(CP_UTF8, 0, appData, -1, base, sizeof(base), nullptr, nullptr);
    CoTaskMemFree(appData);
    if (!base[0]) return;

    char dir[MAX_PATH] = {};
    std::snprintf(dir, sizeof(dir), "%s\\ZMod", base);
    CreateDirectoryA(dir, nullptr);

    char path[MAX_PATH] = {};
    std::snprintf(path, sizeof(path), "%s\\auth_error.txt", dir);

    std::ofstream f(path, std::ios::out | std::ios::trunc);
    if (!f) return;
    time_t now = std::time(nullptr);
    char ts[32] = {};
    struct tm gmt{};
    if (gmtime_s(&gmt, &now) == 0) std::strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &gmt);
    f << ts << "  " << msg << "\n";
}

#if defined(USE_DLLAUTH)

inline std::string GenerateFingerprint() {
    DWORD volSerial = 0;
    GetVolumeInformationA("C:\\", nullptr, 0, &volSerial, nullptr, nullptr, nullptr, 0);

    char machineGuid[128] = {};
    HKEY hk = nullptr;
    DWORD cb = sizeof(machineGuid);
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            XS("SOFTWARE\\Microsoft\\Cryptography"),
            0, KEY_READ | KEY_WOW64_64KEY, &hk) == ERROR_SUCCESS) {
        DWORD type = 0;
        RegQueryValueExA(hk, XS("MachineGuid"), nullptr, &type,
                         reinterpret_cast<LPBYTE>(machineGuid), &cb);
        RegCloseKey(hk);
    }
    char buf[160] = {};

    std::snprintf(buf, sizeof(buf), "%08X|%s", volSerial, machineGuid);
    return std::string(buf);
}

inline std::string ReadUsername() {
    HKEY hk = nullptr;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, XS("Software\\ZMod"),
            0, KEY_READ, &hk) != ERROR_SUCCESS) {
        return {};
    }
    char buf[128] = {};
    DWORD cb = sizeof(buf);
    DWORD type = 0;
    LSTATUS s = RegQueryValueExA(hk, XS("LastUser"), nullptr, &type,
                                 reinterpret_cast<LPBYTE>(buf), &cb);
    RegCloseKey(hk);
    if (s != ERROR_SUCCESS || type != REG_SZ) return {};
    return std::string(buf);
}

inline bool HmacSha256(const uint8_t* key, size_t keyLen,
                      const uint8_t* data, size_t dataLen,
                      uint8_t out[32]) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptOpenAlgorithmProvider)(
            &hAlg, BCRYPT_SHA256_ALGORITHM, nullptr,
            BCRYPT_ALG_HANDLE_HMAC_FLAG))) return false;

    BCRYPT_HASH_HANDLE hHash = nullptr;
    bool ok = false;
    if (BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptCreateHash)(
            hAlg, &hHash, nullptr, 0,
            const_cast<PUCHAR>(key), (ULONG)keyLen, 0))) {
        if (BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptHashData)(
                hHash, const_cast<PUCHAR>(data), (ULONG)dataLen, 0))) {
            if (BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptFinishHash)(
                    hHash, out, 32, 0))) {
                ok = true;
            }
        }
        IMP(bcrypt.dll, BCryptDestroyHash)(hHash);
    }
    IMP(bcrypt.dll, BCryptCloseAlgorithmProvider)(hAlg, 0);
    return ok;
}

inline void HexLower(const uint8_t* in, size_t n, char* out) {
    static const char hexd[] = "0123456789abcdef";
    for (size_t i = 0; i < n; ++i) {
        out[i * 2 + 0] = hexd[(in[i] >> 4) & 0xF];
        out[i * 2 + 1] = hexd[(in[i] >> 0) & 0xF];
    }
    out[n * 2] = '\0';
}

inline bool HttpPostJson(const wchar_t* host, const wchar_t* path,
                         const std::string& body,
                         DWORD& outStatus, std::string& outBody) {
    outStatus = 0;
    outBody.clear();

    HINTERNET hSess = IMP(winhttp.dll, WinHttpOpen)(
        XSW(L"ZwSpark/1.0"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSess) return false;
    IMP(winhttp.dll, WinHttpSetTimeouts)(hSess, 5000, 5000, 10000, 10000);

    static bool s_proxyLoaded = false;
    if (!s_proxyLoaded) { ProxyCfg::LoadFromDisk(); ProxyPool::LoadFromDisk(); s_proxyLoaded = true; }

    std::string apHost, apUser, apPass; int apPort = 0; bool apUseAuth = false;
    bool useAuthProxy = false;
    if (ProxyCfg::g_enabled) {
        ProxyPool::Entry ae;
        if (ProxyPool::NextAuth(ae)) {
            apHost = ae.host; apPort = ae.port; apUser = ae.user; apPass = ae.pass; apUseAuth = ae.useAuth;
            useAuthProxy = true;
        } else if (!ProxyCfg::g_host.empty()) {
            apHost = ProxyCfg::g_host; apPort = ProxyCfg::g_port;
            apUser = ProxyCfg::g_user; apPass = ProxyCfg::g_pass; apUseAuth = ProxyCfg::g_useAuth;
            useAuthProxy = true;
        }
    }

    std::wstring proxyWide;
    if (useAuthProxy) {
        proxyWide = ProxyCfg::Widen(apHost + ":" + std::to_string(apPort));
        WINHTTP_PROXY_INFO pi = {};
        pi.dwAccessType    = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
        pi.lpszProxy       = (LPWSTR)proxyWide.c_str();
        pi.lpszProxyBypass = nullptr;
        IMP(winhttp.dll, WinHttpSetOption)(
            hSess, WINHTTP_OPTION_PROXY, &pi, sizeof(pi));
    }

    HINTERNET hConn = IMP(winhttp.dll, WinHttpConnect)(
        hSess, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConn) {
        IMP(winhttp.dll, WinHttpCloseHandle)(hSess);
        return false;
    }

    HINTERNET hReq = IMP(winhttp.dll, WinHttpOpenRequest)(
        hConn, XSW(L"POST"), path, nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hReq) {
        IMP(winhttp.dll, WinHttpCloseHandle)(hConn);
        IMP(winhttp.dll, WinHttpCloseHandle)(hSess);
        return false;
    }

    if (useAuthProxy && apUseAuth && !apUser.empty()) {
        std::wstring pu = ProxyCfg::Widen(apUser);
        std::wstring pp = ProxyCfg::Widen(apPass);
        IMP(winhttp.dll, WinHttpSetOption)(hReq, WINHTTP_OPTION_PROXY_USERNAME,
            (LPVOID)pu.c_str(), (DWORD)pu.size());
        IMP(winhttp.dll, WinHttpSetOption)(hReq, WINHTTP_OPTION_PROXY_PASSWORD,
            (LPVOID)pp.c_str(), (DWORD)pp.size());
    }

    const wchar_t* hdr = XSW(L"Content-Type: application/json\r\n"
                              L"X-ZMod-Version: v2.01b\r\n");
    bool sent = IMP(winhttp.dll, WinHttpSendRequest)(
        hReq, hdr, (DWORD)-1L,
        (LPVOID)body.data(), (DWORD)body.size(),
        (DWORD)body.size(), 0) != FALSE;
    if (sent) sent = IMP(winhttp.dll, WinHttpReceiveResponse)(hReq, nullptr) != FALSE;

    if (sent && !CertPin::VerifyServerCertPin(hReq)) {
        outStatus = 495;
        IMP(winhttp.dll, WinHttpCloseHandle)(hReq);
        IMP(winhttp.dll, WinHttpCloseHandle)(hConn);
        IMP(winhttp.dll, WinHttpCloseHandle)(hSess);
        return false;
    }

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
            std::vector<char> chunk(avail + 1, 0);
            DWORD read = 0;
            if (!IMP(winhttp.dll, WinHttpReadData)(hReq, chunk.data(), avail, &read) || !read) break;
            outBody.append(chunk.data(), read);
        }
    }

    IMP(winhttp.dll, WinHttpCloseHandle)(hReq);
    IMP(winhttp.dll, WinHttpCloseHandle)(hConn);
    IMP(winhttp.dll, WinHttpCloseHandle)(hSess);
    return sent;
}

inline bool ExtractJsonString(const std::string& body, const char* key, std::string& out) {
    std::string needle = std::string("\"") + key + "\"";
    size_t k = body.find(needle);
    if (k == std::string::npos) return false;
    size_t c = body.find(':', k);
    if (c == std::string::npos) return false;
    size_t q = body.find('"', c);
    if (q == std::string::npos) return false;
    size_t e = body.find('"', q + 1);
    if (e == std::string::npos) return false;
    out.assign(body, q + 1, e - (q + 1));
    return true;
}

inline bool DecodeJwtIat(const std::string& jwt, int64_t& outIat) {
    size_t dot1 = jwt.find('.');
    if (dot1 == std::string::npos) return false;
    size_t dot2 = jwt.find('.', dot1 + 1);
    if (dot2 == std::string::npos) return false;
    std::string b64 = jwt.substr(dot1 + 1, dot2 - dot1 - 1);

    for (char& c : b64) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }
    while (b64.size() % 4) b64.push_back('=');

    static const int8_t T[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    };
    std::string payload;
    payload.reserve((b64.size() * 3) / 4);
    uint32_t acc = 0; int bits = 0;
    for (unsigned char c : b64) {
        int8_t v = T[c];
        if (v == -2) break;
        if (v < 0) return false;
        acc = (acc << 6) | static_cast<uint32_t>(v);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            payload.push_back(static_cast<char>((acc >> bits) & 0xFF));
        }
    }

    size_t k = payload.find("\"iat\"");
    if (k == std::string::npos) return false;
    size_t c = payload.find(':', k);
    if (c == std::string::npos) return false;
    size_t p = c + 1;
    while (p < payload.size() && (payload[p] == ' ' || payload[p] == '\t')) ++p;
    int64_t v = 0;
    bool any = false;
    while (p < payload.size() && payload[p] >= '0' && payload[p] <= '9') {
        v = v * 10 + (payload[p] - '0');
        ++p; any = true;
    }
    if (!any) return false;
    outIat = v;
    return true;
}

inline bool ExtractJsonInt(const std::string& body, const char* key, int64_t& out) {
    std::string needle = std::string("\"") + key + "\"";
    size_t k = body.find(needle);
    if (k == std::string::npos) return false;
    size_t c = body.find(':', k);
    if (c == std::string::npos) return false;
    size_t p = c + 1;
    while (p < body.size() && (body[p] == ' ' || body[p] == '\t')) ++p;
    out = 0;
    bool neg = false;
    if (p < body.size() && body[p] == '-') { neg = true; ++p; }
    bool any = false;
    while (p < body.size() && body[p] >= '0' && body[p] <= '9') {
        out = out * 10 + (body[p] - '0');
        ++p; any = true;
    }
    if (!any) return false;
    if (neg) out = -out;
    return true;
}

inline bool Base64Decode(const std::string& in, std::vector<uint8_t>& out) {
    static const int8_t T[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    };
    out.clear();
    out.reserve((in.size() * 3) / 4);
    uint32_t acc = 0;
    int bits = 0;
    for (unsigned char c : in) {
        int8_t v = T[c];
        if (v == -2) break;
        if (v < 0) return false;
        acc = (acc << 6) | static_cast<uint32_t>(v);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<uint8_t>((acc >> bits) & 0xFF));
        }
    }
    return true;
}

inline bool AesGcm128Decrypt(const uint8_t key[16],
                             const uint8_t* blob, size_t blobLen,
                             std::vector<uint8_t>& out) {
    out.clear();
    if (blobLen < 12 + 16) return false;
    const uint8_t* nonce = blob;
    const uint8_t* ctTag = blob + 12;
    const size_t   ctTagLen = blobLen - 12;
    const uint8_t* ct  = ctTag;
    const size_t   ctLen = ctTagLen - 16;
    const uint8_t* tag = ctTag + ctLen;

    bool ok = false;
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptOpenAlgorithmProvider)(
            &hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0))) {
        if (BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptSetProperty)(
                hAlg, BCRYPT_CHAINING_MODE,
                (PUCHAR)BCRYPT_CHAIN_MODE_GCM,
                sizeof(BCRYPT_CHAIN_MODE_GCM), 0))) {
            BCRYPT_KEY_HANDLE hKey = nullptr;
            if (BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptGenerateSymmetricKey)(
                    hAlg, &hKey, nullptr, 0,
                    const_cast<PUCHAR>(key), 16, 0))) {
                BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
                BCRYPT_INIT_AUTH_MODE_INFO(info);
                info.pbNonce = const_cast<PUCHAR>(nonce);
                info.cbNonce = 12;
                info.pbTag   = const_cast<PUCHAR>(tag);
                info.cbTag   = 16;

                out.resize(ctLen);
                ULONG written = 0;
                NTSTATUS st = IMP(bcrypt.dll, BCryptDecrypt)(hKey,
                    const_cast<PUCHAR>(ct), (ULONG)ctLen,
                    &info, nullptr, 0,
                    out.data(), (ULONG)out.size(), &written, 0);
                if (BCRYPT_SUCCESS(st)) {
                    out.resize(written);
                    ok = true;
                } else {
                    SecureZeroMemory(out.data(), out.size());
                    out.clear();
                }
                IMP(bcrypt.dll, BCryptDestroyKey)(hKey);
            }
        }
        IMP(bcrypt.dll, BCryptCloseAlgorithmProvider)(hAlg, 0);
    }
    return ok;
}

inline Result CallAuth(const std::string& username, const std::string& fingerprint) {

    std::string userLower = username;
    for (auto& c : userLower) {
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }

    char body[512] = {};
    std::snprintf(body, sizeof(body),
        "{\"username\":\"%s\",\"fingerprint\":\"%s\"}",
        userLower.c_str(), fingerprint.c_str());

    DWORD status = 0;
    std::string resp;
    bool transport = HttpPostJson(XSW(L"zmod-auth.example.workers.dev"),
                                  XSW(L"/auth"),
                                  std::string(body), status, resp);

    if (!transport) {
        if (status == 495) g_lastErrorMsg = "TLS cert pin failed.";
        else               g_lastErrorMsg = "Network error reaching /auth.";
        return Result::Transient;
    }

    if (status >= 400 && status < 500) {
        std::string errMsg;
        if (ExtractJsonString(resp, "error", errMsg) && !errMsg.empty()) {
            g_lastErrorMsg = std::string("auth: ") + errMsg;
        } else {
            g_lastErrorMsg = "auth: status " + std::to_string(status);
        }
        SecureZeroMemory(resp.data(), resp.size());
        return Result::Permanent;
    }

    if (status != 200) {
        g_lastErrorMsg = "auth: status " + std::to_string(status);
        SecureZeroMemory(resp.data(), resp.size());
        return Result::Transient;
    }

    std::string jwt;
    if (!ExtractJsonString(resp, "session", jwt) || jwt.empty()) {
        g_lastErrorMsg = "auth: response missing session";
        SecureZeroMemory(resp.data(), resp.size());
        return Result::Transient;
    }

    int64_t sessionExp = 0;
    if (!ExtractJsonInt(resp, "session_exp", sessionExp) || sessionExp <= 0) {
        g_lastErrorMsg = "auth: response missing session_exp";
        SecureZeroMemory(resp.data(), resp.size());
        return Result::Transient;
    }

    int64_t iat = 0;
    if (!DecodeJwtIat(jwt, iat) || iat <= 0) {
        g_lastErrorMsg = "auth: JWT iat parse failed";
        SecureZeroMemory(resp.data(), resp.size());
        return Result::Transient;
    }

    {
        std::lock_guard<std::mutex> lk(g_sessionMutex);
        g_sessionJwt  = jwt;
        g_sessionIat  = iat;
        g_sessionFp   = fingerprint;
        g_sessionUser = userLower;
        g_sessionExp  = sessionExp;
    }
    g_authenticated.store(true, std::memory_order_release);
    g_lastErrorMsg.clear();

    SecureZeroMemory(resp.data(), resp.size());
    SecureZeroMemory(body, sizeof(body));
    SecureZeroMemory(jwt.data(), jwt.size());

    return Result::Ok;
}

inline Result Authenticate() {
    g_authenticated.store(false, std::memory_order_release);

    std::string user = ReadUsername();
    if (user.empty()) {
        g_lastErrorMsg = "No username in registry — loader did not authenticate.";
        WriteAuthErrorFile(g_lastErrorMsg);

        g_authFatal.store(true, std::memory_order_release);
        ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
        return Result::Permanent;
    }

    std::string fp = GenerateFingerprint();

    Result authR = CallAuth(user, fp);
    if (authR == Result::Transient) {

        ExitFlags::TripExit("auth: /auth transient");
        return Result::Transient;
    }
    if (authR == Result::Permanent) {
        WriteAuthErrorFile(g_lastErrorMsg);
        g_authFatal.store(true, std::memory_order_release);

        ExitFlags::TripExit(g_lastErrorMsg);
        return Result::Permanent;
    }

    std::string userLower = user;
    for (auto& c : userLower) {
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }

    char secretHex[65] = {};
    uint8_t secretBytes[32] = {};
    bool ok = DllAuthSecret::Decode(secretHex);
    if (ok) ok = DllAuthSecret::HexAsciiToBytes(secretHex, secretBytes);
    if (!ok) {
        SecureZeroMemory(secretHex,   sizeof(secretHex));
        SecureZeroMemory(secretBytes, sizeof(secretBytes));
        g_lastErrorMsg = "Embedded DLL_AUTH_SECRET is the placeholder. Regenerate via tools/encode_dll_auth_secret.ps1.";
        WriteAuthErrorFile(g_lastErrorMsg);
        g_authFatal.store(true, std::memory_order_release);
        ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
        return Result::Permanent;
    }

    uint8_t challengeBytes[32] = {};
    NTSTATUS rngSt = IMP(bcrypt.dll, BCryptGenRandom)(
        nullptr, challengeBytes, sizeof(challengeBytes),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(rngSt)) {
        SecureZeroMemory(secretHex,   sizeof(secretHex));
        SecureZeroMemory(secretBytes, sizeof(secretBytes));
        SecureZeroMemory(challengeBytes, sizeof(challengeBytes));
        g_lastErrorMsg = "BCryptGenRandom failed.";
        ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
        return Result::Permanent;
    }
    char challengeHex[65] = {};
    HexLower(challengeBytes, sizeof(challengeBytes), challengeHex);
    SecureZeroMemory(challengeBytes, sizeof(challengeBytes));

    int64_t ts = static_cast<int64_t>(time(nullptr));

    char canon[512] = {};
    int n = std::snprintf(canon, sizeof(canon), "%s:%s:%s:%lld",
                          challengeHex, fp.c_str(), userLower.c_str(),
                          (long long)ts);

    uint8_t digest[32] = {};
    bool hmacOk = (n > 0 && n < (int)sizeof(canon)) &&
                  HmacSha256(secretBytes, sizeof(secretBytes),
                             reinterpret_cast<const uint8_t*>(canon), (size_t)n,
                             digest);

    SecureZeroMemory(canon,       sizeof(canon));

    if (!hmacOk) {
        SecureZeroMemory(secretHex,   sizeof(secretHex));
        SecureZeroMemory(secretBytes, sizeof(secretBytes));
        SecureZeroMemory(digest, sizeof(digest));
        g_lastErrorMsg = "HMAC computation failed.";
        return Result::Transient;
    }

    char sigHex[65] = {};
    HexLower(digest, sizeof(digest), sigHex);
    SecureZeroMemory(digest, sizeof(digest));

    char body[1024] = {};
    std::snprintf(body, sizeof(body),
        "{\"challenge\":\"%s\",\"fingerprint\":\"%s\",\"username\":\"%s\",\"ts\":%lld,\"sig\":\"%s\"}",
        challengeHex, fp.c_str(), userLower.c_str(), (long long)ts, sigHex);

    DWORD status = 0;
    std::string resp;
    bool transport = HttpPostJson(XSW(L"zmod-auth.example.workers.dev"),
                                  XSW(L"/dll-attach"),
                                  std::string(body), status, resp);

    if (!transport) {
        SecureZeroMemory(secretHex,   sizeof(secretHex));
        SecureZeroMemory(secretBytes, sizeof(secretBytes));
        if (status == 495) g_lastErrorMsg = "TLS cert pin failed.";
        else               g_lastErrorMsg = "Network error reaching auth worker.";
        return Result::Transient;
    }

    if (status == 200) {

        std::string token;
        if (!ExtractJsonString(resp, "token", token) || token.empty()) {
            SecureZeroMemory(secretHex,   sizeof(secretHex));
            SecureZeroMemory(secretBytes, sizeof(secretBytes));
            g_lastErrorMsg = "Auth response missing token.";
            ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
            return Result::Permanent;
        }

        int64_t ttl = 0;
        if (!ExtractJsonInt(resp, "ttl", ttl) || ttl <= 0) {
            SecureZeroMemory(secretHex,   sizeof(secretHex));
            SecureZeroMemory(secretBytes, sizeof(secretBytes));
            g_lastErrorMsg = "Auth response missing ttl.";
            ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
            return Result::Permanent;
        }
        int64_t exp = ts + ttl;

        std::string runtimeB64;
        if (!ExtractJsonString(resp, "runtime_blob", runtimeB64) || runtimeB64.empty()) {
            SecureZeroMemory(secretHex,   sizeof(secretHex));
            SecureZeroMemory(secretBytes, sizeof(secretBytes));
            g_lastErrorMsg = "Auth response missing runtime_blob.";
            ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
            return Result::Permanent;
        }

        char keyCanon[256] = {};
        int kn = std::snprintf(keyCanon, sizeof(keyCanon), "%s:%s",
                               challengeHex, token.c_str());
        uint8_t fullDigest[32] = {};
        bool keyOk = (kn > 0 && kn < (int)sizeof(keyCanon)) &&
                     HmacSha256(secretBytes, sizeof(secretBytes),
                                reinterpret_cast<const uint8_t*>(keyCanon),
                                (size_t)kn, fullDigest);

        SecureZeroMemory(keyCanon,    sizeof(keyCanon));
        SecureZeroMemory(secretHex,   sizeof(secretHex));
        SecureZeroMemory(secretBytes, sizeof(secretBytes));

        if (!keyOk) {
            SecureZeroMemory(fullDigest, sizeof(fullDigest));
            g_lastErrorMsg = "Failed to derive attach key.";
            ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
            return Result::Permanent;
        }

        uint8_t attachKey[16] = {};
        std::memcpy(attachKey, fullDigest, 16);
        SecureZeroMemory(fullDigest, sizeof(fullDigest));

        std::vector<uint8_t> blobBytes;
        if (!Base64Decode(runtimeB64, blobBytes) || blobBytes.size() < 12 + 16) {
            SecureZeroMemory(attachKey, sizeof(attachKey));
            if (!blobBytes.empty()) SecureZeroMemory(blobBytes.data(), blobBytes.size());
            g_lastErrorMsg = "runtime_blob base64 decode failed.";
            ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
            return Result::Permanent;
        }

        std::vector<uint8_t> plain;
        bool decOk = AesGcm128Decrypt(attachKey, blobBytes.data(),
                                      blobBytes.size(), plain);
        SecureZeroMemory(attachKey, sizeof(attachKey));
        SecureZeroMemory(blobBytes.data(), blobBytes.size());

        if (!decOk) {
            if (!plain.empty()) SecureZeroMemory(plain.data(), plain.size());
            g_lastErrorMsg = "runtime_blob auth tag mismatch.";
            ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
            return Result::Permanent;
        }

        std::string plainJson(reinterpret_cast<const char*>(plain.data()),
                              plain.size());
        SecureZeroMemory(plain.data(), plain.size());

        if (!RuntimeBlob::ApplyFromJson(plainJson)) {
            SecureZeroMemory(plainJson.data(), plainJson.size());
            g_lastErrorMsg = "runtime_blob schema rejected.";
            ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
            return Result::Permanent;
        }
        SecureZeroMemory(plainJson.data(), plainJson.size());

        {
            std::lock_guard<std::mutex> lk(g_sessionMutex);
            g_sessionExp  = exp;
            g_sessionFp   = fp;
            g_sessionUser = userLower;
        }
        g_sessionToken = token;
        g_fingerprint  = fp;

        (void)IntegrityCheck::TextCrcPinned();

        g_authenticated.store(true, std::memory_order_release);
        g_lastErrorMsg.clear();

        StartHeartbeat();

        return Result::Ok;
    }

    SecureZeroMemory(secretHex,   sizeof(secretHex));
    SecureZeroMemory(secretBytes, sizeof(secretBytes));

    if (status >= 400 && status < 500) {
        std::string errMsg;
        if (ExtractJsonString(resp, "error", errMsg) && !errMsg.empty()) {
            g_lastErrorMsg = errMsg;
            WriteAuthErrorFile(g_lastErrorMsg);
            g_authFatal.store(true, std::memory_order_release);
            ExitFlags::TripExit(std::string("auth: ") + g_lastErrorMsg);
            return Result::Permanent;
        }
    }
    g_lastErrorMsg = "Auth failed (status " + std::to_string(status) + ").";
    return Result::Transient;
}

inline void HeartbeatLoop() {
    using namespace std::chrono_literals;

    while (!ExitFlags::ShouldExit()) {

        for (int slept = 0; slept < 60 && !ExitFlags::ShouldExit(); ++slept) {
            std::this_thread::sleep_for(1s);
        }
        if (ExitFlags::ShouldExit()) break;

        const uint32_t crc = IntegrityCheck::TextCrcPinned();

        char body[512] = {};
        std::snprintf(body, sizeof(body),
            "{\"token\":\"%s\",\"fingerprint\":\"%s\",\"text_crc\":%u}",
            g_sessionToken.c_str(),
            g_fingerprint.c_str(),
            static_cast<unsigned int>(crc));

        DWORD status = 0;
        std::string resp;
        bool ok = HttpPostJson(
            XSW(L"zmod-auth.example.workers.dev"),
            XSW(L"/dll-ping"),
            std::string(body), status, resp);

        if (!ok) {

            int prior = g_heartbeatStrikes.fetch_add(1, std::memory_order_acq_rel);
            if (prior + 1 >= 3) {
                ExitFlags::TripExit("heartbeat: transport-3strike");
                break;
            }
            continue;
        }
        g_heartbeatStrikes.store(0, std::memory_order_release);

        if (status == 401 || status == 403) {

            ExitFlags::TripExit(std::string("heartbeat: ") +
                                std::to_string(status));
            break;
        }
        if (status != 200) {

            continue;
        }

        std::string newToken;
        if (ExtractJsonString(resp, "token", newToken) && !newToken.empty()) {

            g_sessionToken = std::move(newToken);
        }

        if (resp.find("\"v\":1") != std::string::npos) {
            RuntimeBlob::ApplyFromJson(resp);
        }
    }

    g_heartbeatRunning.store(false, std::memory_order_release);
}

#else

inline Result Authenticate() { return Result::Skipped; }

inline void HeartbeatLoop() {}

#endif

}

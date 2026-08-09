

#pragma once

#include <Windows.h>
#include <winhttp.h>
#include <wincrypt.h>
#include <bcrypt.h>
#include <cstdint>
#include <cstring>

#include "XorStr.h"
#include "IndirectImports.h"

namespace CertPin {

inline void HexEncodeLower(const uint8_t* in, size_t n, char* out) {
    static const char hexd[] = "0123456789abcdef";
    for (size_t i = 0; i < n; ++i) {
        out[i * 2 + 0] = hexd[(in[i] >> 4) & 0xF];
        out[i * 2 + 1] = hexd[(in[i] >> 0) & 0xF];
    }
    out[n * 2] = '\0';
}

inline bool Sha256(const uint8_t* data, size_t len, uint8_t out[32]) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptOpenAlgorithmProvider)(
            &hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0))) return false;

    BCRYPT_HASH_HANDLE hHash = nullptr;
    bool ok = false;
    if (BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptCreateHash)(
            hAlg, &hHash, nullptr, 0, nullptr, 0, 0))) {
        if (BCRYPT_SUCCESS(IMP(bcrypt.dll, BCryptHashData)(
                hHash, const_cast<PUCHAR>(data), (ULONG)len, 0))) {
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

inline bool VerifyServerCertPin(HINTERNET hReq) {
    PCCERT_CONTEXT ctx = nullptr;
    DWORD cbCtx = sizeof(ctx);
    if (!IMP(winhttp.dll, WinHttpQueryOption)(
            hReq, WINHTTP_OPTION_SERVER_CERT_CONTEXT, &ctx, &cbCtx) || !ctx) {
        return false;
    }

    uint8_t digest[32] = {};
    bool hashed = Sha256(ctx->pbCertEncoded, ctx->cbCertEncoded, digest);
    IMP(crypt32.dll, CertFreeCertificateContext)(ctx);
    if (!hashed) return false;

    char hex[65] = {};
    HexEncodeLower(digest, 32, hex);

    if (std::strcmp(hex,
        XS("9c08efc60940140bad4ed53e3dd7590009a434dc1bbec27cb09d98f2e896f374")) == 0) return true;

    if (std::strcmp(hex,
        XS("7ea62379f8951bbfc6aba831c3b40b77286688426756d4789c17707a9bb1d6ad")) == 0) return true;

    return false;
}

}

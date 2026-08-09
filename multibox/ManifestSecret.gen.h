

#pragma once

#include <cstdint>
#include <cstring>
#include <Windows.h>

namespace ManifestSecret {

inline const uint8_t kEncoded[64] = {

    0x54, 0x7D, 0x7C, 0x34, 0xA6, 0x76, 0xA1, 0x87,
    0x44, 0x10, 0xAE, 0x94, 0xF0, 0x1A, 0x00, 0x49,
    0x9A, 0xB4, 0xEB, 0x59, 0xB4, 0x48, 0xA0, 0xAB,
    0x4B, 0x3B, 0x81, 0x38, 0x67, 0xA7, 0x47, 0x38,
    0x68, 0x2E, 0x64, 0x88, 0x4F, 0xFF, 0xF7, 0x27,
    0x73, 0x35, 0xD7, 0x6B, 0xCB, 0xCE, 0x31, 0xFB,
    0xF6, 0x12, 0xD3, 0x48, 0x70, 0x78, 0x1A, 0x8A,
    0xAE, 0x07, 0x50, 0x19, 0x3C, 0xE7, 0xD8, 0x33
};

inline uint32_t HashPos(uint32_t i) {
    uint32_t h = (i + 0xB1A7F00Du);
    h ^= h >> 16; h *= 0x85EBCA6Bu;
    h ^= h >> 13; h *= 0xC2B2AE35u;
    h ^= h >> 16; h *= 0x27D4EB2Fu;
    return h;
}

inline uint8_t Ror8(uint8_t b, uint8_t r) {
    r &= 7;
    return static_cast<uint8_t>((b >> r) | (b << (8 - r)));
}

inline bool Decode(char out[65]) {
    for (int i = 0; i < 64; ++i) {
        uint32_t h = HashPos(static_cast<uint32_t>(i));
        uint8_t mask = static_cast<uint8_t>((h ^ (h >> 8) ^ (h >> 16) ^ (h >> 24)) & 0xFF);
        uint8_t rot  = static_cast<uint8_t>((h >> 11) & 0x7);
        uint8_t b = kEncoded[i];
        b ^= mask;
        b  = Ror8(b, rot);
        char c = static_cast<char>(b);
        bool isHex = (c >= '0' && c <= '9') ||
                     (c >= 'a' && c <= 'f') ||
                     (c >= 'A' && c <= 'F');
        if (!isHex) {
            SecureZeroMemory(out, 65);
            return false;
        }
        out[i] = c;
    }
    out[64] = '\0';
    return true;
}

inline bool HexAsciiToBytes(const char hex[65], uint8_t out[32]) {
    auto hv = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    for (int i = 0; i < 32; ++i) {
        int hi = hv(hex[i * 2 + 0]);
        int lo = hv(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) return false;
        out[i] = static_cast<uint8_t>((hi << 4) | lo);
    }
    return true;
}

}

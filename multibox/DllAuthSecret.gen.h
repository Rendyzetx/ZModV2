

#pragma once

#include <cstdint>
#include <cstring>
#include <Windows.h>

namespace DllAuthSecret {

inline const uint8_t kEncoded[64] = {

    0xD8, 0xD2, 0x03, 0x4D, 0xEE, 0x53, 0x1B, 0x58,
    0x47, 0x09, 0x19, 0x3E, 0x2B, 0x17, 0x0E, 0x4A,
    0x9E, 0x6E, 0xC9, 0x02, 0x01, 0x57, 0x2A, 0xCD,
    0x33, 0xD4, 0xC7, 0x48, 0xC8, 0x91, 0x6D, 0x46,
    0x53, 0xD6, 0x28, 0xC3, 0x96, 0xC3, 0xD5, 0xEB,
    0xC3, 0x43, 0x1D, 0x51, 0x25, 0xEF, 0x93, 0x16,
    0xB4, 0xEC, 0xF4, 0xDC, 0xC4, 0xDC, 0xFA, 0xA5,
    0x89, 0x1A, 0x47, 0xFE, 0x58, 0xCE, 0xA5, 0x84
};

inline uint32_t HashPos(uint32_t i) {
    uint32_t h = (i + 0xCAFEBABEu);
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

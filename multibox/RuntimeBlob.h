

#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include "json.hpp"
#include "../anticheat/VMProtectMarkers.h"

namespace RuntimeBlob {

constexpr uint32_t kFlagsXorKey   = 0xA5C39E7Bu;
constexpr int      kSchemaVersion = 1;

constexpr uint32_t kAllFeatures   = 0xFFFFFFFFu;

inline std::atomic<uint32_t> g_flags{ 0 };
inline std::atomic<uint32_t> g_flags_inv{ ~0u };
inline std::atomic<uint32_t> g_flags_xor{ kFlagsXorKey };
inline std::atomic<uint64_t> g_server_time{ 0 };
inline std::atomic<bool>     g_applied{ false };

inline bool ApplyFromJson(const std::string& blobJson) {
    VMP_BEGIN_VIRT("RuntimeBlob::ApplyFromJson");

    if (blobJson.empty()) { VMP_END(); return false; }

    nlohmann::json j;
    try {
        j = nlohmann::json::parse(blobJson);
    } catch (...) {
        VMP_END();
        return false;
    }
    if (!j.is_object()) { VMP_END(); return false; }
    if (j.value("v", 0) != kSchemaVersion) { VMP_END(); return false; }

    if (!j.contains("feature_flags")) { VMP_END(); return false; }
    if (!j.contains("server_time"))   { VMP_END(); return false; }

    const uint32_t ff = static_cast<uint32_t>(j.value("feature_flags", (uint64_t)0));
    const uint64_t st = j.value("server_time", (uint64_t)0);

    g_flags_inv.store(~ff,                std::memory_order_release);
    g_flags_xor.store(ff ^ kFlagsXorKey,  std::memory_order_release);
    g_flags.store(ff,                     std::memory_order_release);
    g_server_time.store(st,               std::memory_order_release);
    g_applied.store(true,                 std::memory_order_release);

    VMP_END();
    return true;
}

inline bool BlobApplied() {
    VMP_BEGIN_VIRT("RuntimeBlob::BlobApplied");

    if (!g_applied.load(std::memory_order_acquire)) { VMP_END(); return false; }

    const uint32_t a = g_flags.load(std::memory_order_acquire);
    const uint32_t b = g_flags_inv.load(std::memory_order_acquire);
    const uint32_t c = g_flags_xor.load(std::memory_order_acquire);
    if ((~a) != b)                  { VMP_END(); return false; }
    if ((a ^ kFlagsXorKey) != c)    { VMP_END(); return false; }

    VMP_END();
    return true;
}

inline bool FeatureEnabled(uint32_t bit) {
    VMP_BEGIN_VIRT("RuntimeBlob::FeatureEnabled");

    const uint32_t a = g_flags.load(std::memory_order_acquire);
    const uint32_t b = g_flags_inv.load(std::memory_order_acquire);
    const uint32_t c = g_flags_xor.load(std::memory_order_acquire);
    if ((~a) != b)               { VMP_END(); return false; }
    if ((a ^ kFlagsXorKey) != c) { VMP_END(); return false; }
    if (!g_applied.load(std::memory_order_acquire)) { VMP_END(); return false; }

    const bool on = (a & bit) != 0;
    VMP_END();
    return on;
}

inline uint64_t ServerTime() {
    return g_server_time.load(std::memory_order_acquire);
}

}

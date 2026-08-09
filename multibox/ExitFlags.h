

#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <atomic>
#include <chrono>
#include <ctime>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "../anticheat/VMProtectMarkers.h"

namespace ExitFlags {

inline std::atomic<bool>     g_flagBool{ false };
inline std::atomic<int>      g_flagCounter{ 0 };
inline std::atomic<uint64_t> g_flagXorEnc{ 0xDEADBEEFull };
inline std::atomic<bool>     g_flagExtern{ false };

inline bool ShouldExit() {
    if (g_flagBool.load(std::memory_order_acquire))                         return true;
    if (g_flagCounter.load(std::memory_order_acquire) != 0)                 return true;
    if (g_flagXorEnc.load(std::memory_order_acquire) != 0xDEADBEEFull)      return true;
    if (g_flagExtern.load(std::memory_order_acquire))                       return true;
    return false;
}

namespace detail {

inline std::filesystem::path LogPath() {
    wchar_t buf[MAX_PATH] = {};
    if (SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, buf) != S_OK)
        return {};
    std::filesystem::path p(buf);
    p /= L"ZMod";
    std::error_code ec;
    std::filesystem::create_directories(p, ec);
    p /= L"exit_reason.log";
    return p;
}

inline std::string IsoUtcNow() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm gt{};
    gmtime_s(&gt, &t);
    std::ostringstream os;
    os << std::put_time(&gt, "%Y-%m-%dT%H:%M:%SZ");
    return os.str();
}

inline std::atomic<bool>& LogGate() {
    static std::atomic<bool> g{ false };
    return g;
}

}

inline void TripExit(const std::string& reason) {
    VMP_BEGIN_MUT("ExitFlags::TripExit");

    g_flagBool.store(true,                          std::memory_order_release);
    g_flagCounter.fetch_add(1,                      std::memory_order_acq_rel);
    g_flagXorEnc.store(0xDEADBEEFull ^ 0x1ull,      std::memory_order_release);
    g_flagExtern.store(true,                        std::memory_order_release);

    bool expected = false;
    if (detail::LogGate().compare_exchange_strong(expected, true,
                                                  std::memory_order_acq_rel))
    {
        try {
            auto path = detail::LogPath();
            if (!path.empty()) {
                std::ofstream f(path, std::ios::app);
                if (f) {
                    f << detail::IsoUtcNow() << "  " << reason << '\n';
                }
            }
        } catch (...) {

        }
    }

    VMP_END();
}

}

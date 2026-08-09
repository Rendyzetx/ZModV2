

#pragma once

#include <atomic>
#include <mutex>
#include <string>

namespace BootState {

enum class Stage : int {
    Init            = 0,
    AuthPending     = 1,
    AuthOk          = 2,
    ManifestPending = 3,
    ManifestOk      = 4,
    FeaturesReady   = 5,
    Failed          = 99,
};

inline std::atomic<Stage> g_stage{ Stage::Init };
inline std::atomic<int>   g_progress_percent{ 0 };

inline std::mutex   g_errMutex;
inline std::string  g_errorMsg;

inline void Set(Stage s, int pct) {
    g_stage.store(s, std::memory_order_release);
    g_progress_percent.store(pct, std::memory_order_release);
}

inline void Fail(const std::string& msg) {
    {
        std::lock_guard<std::mutex> lk(g_errMutex);
        g_errorMsg = msg;
    }
    g_stage.store(Stage::Failed, std::memory_order_release);
}

}



#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <mutex>

#include "json.hpp"
#include "main.h"
#include "XorStr.h"

namespace KickLogger {

inline std::mutex g_logMutex;

inline std::filesystem::path LogPath() {
    wchar_t buf[MAX_PATH] = {};
    if (SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, buf) != S_OK)
        return {};
    std::filesystem::path p(buf);
    p /= L"ZMod";
    std::error_code ec;
    std::filesystem::create_directories(p, ec);
    p /= L"kick_log.txt";
    return p;
}

inline std::string IsoTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                   now.time_since_epoch()) % 1000;
    std::tm lt{};
    localtime_s(&lt, &t);
    std::ostringstream oss;
    oss << std::put_time(&lt, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

inline std::string BsonToJsonString(void* bsonPacket) {
    if (!bsonPacket || !oBsonDump) return "";
    try {
        Il2CppArray* arr = oBsonDump(bsonPacket);
        if (!arr || arr->max_length <= 0) return "";
        std::vector<uint8_t> bytes((uint8_t*)arr->items,
                                    (uint8_t*)arr->items + arr->max_length);
        nlohmann::json j = nlohmann::json::from_bson(bytes);
        return j.dump(2);
    } catch (...) {
        return "";
    }
}

inline void Append(const std::string& eventName, const std::string& bsonJson) {
    std::lock_guard<std::mutex> lock(g_logMutex);

    auto path = LogPath();
    if (path.empty()) return;

    std::ofstream f(path, std::ios::app);
    if (!f) return;

    f << "[" << IsoTimestamp() << "] " << eventName << "\n";
    if (!bsonJson.empty()) f << bsonJson << "\n";
    f << "----\n";
    f.close();

    std::cout << XS("[Kick] ") << eventName;
    if (!bsonJson.empty()) std::cout << XS(" :: ") << bsonJson;
    std::cout << std::endl;

    ShowNotification(
        "Server event",
        eventName + (bsonJson.empty() ? "" : "\n" + bsonJson.substr(0, 200)),
        NotificationType::Warning,
        12.0f);
}

typedef void (__fastcall* tHandleWarnPlayerMessage)      (void* thisPtr, void* bson);
typedef void (__fastcall* tHandleOfflineMessage)         (void* thisPtr, void* bson);
typedef void (__fastcall* tHandleBanLimitExceededMessage)(void* thisPtr, void* bson);
typedef void (__fastcall* tHandleWarpPlayerMessage)      (void* thisPtr, void* bson);
typedef void (__fastcall* tForceDisconnect)              (void* thisPtr);

inline tHandleWarnPlayerMessage       oHandleWarnPlayerMessage       = nullptr;
inline tHandleOfflineMessage          oHandleOfflineMessage          = nullptr;
inline tHandleBanLimitExceededMessage oHandleBanLimitExceededMessage = nullptr;
inline tHandleWarpPlayerMessage       oHandleWarpPlayerMessage       = nullptr;
inline tForceDisconnect               oForceDisconnect               = nullptr;

inline void __fastcall hkHandleWarnPlayerMessage(void* thisPtr, void* bson) {
    Append("HandleWarnPlayerMessage", BsonToJsonString(bson));
    if (oHandleWarnPlayerMessage) oHandleWarnPlayerMessage(thisPtr, bson);
}

inline void __fastcall hkHandleOfflineMessage(void* thisPtr, void* bson) {
    Append("HandleOfflineMessage", BsonToJsonString(bson));
    if (oHandleOfflineMessage) oHandleOfflineMessage(thisPtr, bson);
}

inline void __fastcall hkHandleBanLimitExceededMessage(void* thisPtr, void* bson) {
    Append("HandleBanLimitExceededMessage", BsonToJsonString(bson));
    if (oHandleBanLimitExceededMessage) oHandleBanLimitExceededMessage(thisPtr, bson);
}

inline void __fastcall hkHandleWarpPlayerMessage(void* thisPtr, void* bson) {
    Append("HandleWarpPlayerMessage", BsonToJsonString(bson));
    if (oHandleWarpPlayerMessage) oHandleWarpPlayerMessage(thisPtr, bson);
}

inline void __fastcall hkForceDisconnect(void* thisPtr) {
    Append("ForceDisconnect", "");
    if (oForceDisconnect) oForceDisconnect(thisPtr);
}

inline void Install() {
    DoHook(GetMethodPtr("NetworkClient", "HandleWarnPlayerMessage"),
           &hkHandleWarnPlayerMessage,
           (void**)&oHandleWarnPlayerMessage,
           "NetworkClient::HandleWarnPlayerMessage");
    DoHook(GetMethodPtr("NetworkClient", "HandleOfflineMessage"),
           &hkHandleOfflineMessage,
           (void**)&oHandleOfflineMessage,
           "NetworkClient::HandleOfflineMessage");
    DoHook(GetMethodPtr("NetworkClient", "HandleBanLimitExceededMessage"),
           &hkHandleBanLimitExceededMessage,
           (void**)&oHandleBanLimitExceededMessage,
           "NetworkClient::HandleBanLimitExceededMessage");
    DoHook(GetMethodPtr("NetworkClient", "HandleWarpPlayerMessage"),
           &hkHandleWarpPlayerMessage,
           (void**)&oHandleWarpPlayerMessage,
           "NetworkClient::HandleWarpPlayerMessage");
    DoHook(GetMethodPtr("NetworkClient", "ForceDisconnect"),
           &hkForceDisconnect,
           (void**)&oForceDisconnect,
           "NetworkClient::ForceDisconnect");
}

}



#pragma once

#include "imgui.h"
#include "BootState.h"

#include <ShlObj.h>
#include <Windows.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <mutex>
#include <string>

namespace AuthBanner {

namespace detail {

inline const char* MapFriendly(const std::string& raw) {

    if (raw.empty()) return "Something went wrong. Please relaunch.";

    auto contains = [&](const char* needle) {
        return raw.find(needle) != std::string::npos;
    };

    if (contains("network") || contains("Network error") ||
        contains("TLS cert pin") || contains("transient")) {
        return "Could not reach servers. Check your internet.";
    }

    if (contains("manifest: expired")) {
        return "Update available. Please relaunch.";
    }
    if (contains("401")) {
        return "Session expired. Please relaunch.";
    }
    if (contains("403")) {
        return "Account suspended or device not authorized.";
    }
    if (contains("404")) {
        return "This game build isn't supported yet.";
    }
    if (contains("429")) {
        return "Too many requests. Try again in a few minutes.";
    }

    if (contains("bad frame") || contains("outer auth") ||
        contains("inner seal") || contains("seal") ||
        contains("decrypt")) {
        return "Could not load required data. Please relaunch.";
    }
    return "Something went wrong. Please relaunch.";
}

inline void AppendErrorLog(const std::string& raw) {
    PWSTR appData = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appData)))
        return;
    char base[MAX_PATH] = {};
    WideCharToMultiByte(CP_UTF8, 0, appData, -1, base, sizeof(base), nullptr, nullptr);
    CoTaskMemFree(appData);
    if (!base[0]) return;

    char dir[MAX_PATH] = {};
    std::snprintf(dir, sizeof(dir), "%s\\ZMod", base);
    CreateDirectoryA(dir, nullptr);

    char path[MAX_PATH] = {};
    std::snprintf(path, sizeof(path), "%s\\boot_error.log", dir);

    std::ofstream f(path, std::ios::out | std::ios::app);
    if (!f) return;
    time_t now = std::time(nullptr);
    char ts[32] = {};
    struct tm gmt{};
    if (gmtime_s(&gmt, &now) == 0)
        std::strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &gmt);
    f << ts << "  " << raw << "\n";
}

inline void LogIfChanged(const std::string& raw) {
    static std::mutex      s_logMutex;
    static std::string     s_lastLogged;
    std::lock_guard<std::mutex> lk(s_logMutex);
    if (raw == s_lastLogged) return;
    AppendErrorLog(raw);
    s_lastLogged = raw;
}

}

inline void Render() {

    std::string raw;
    {
        std::lock_guard<std::mutex> lk(BootState::g_errMutex);
        raw = BootState::g_errorMsg;
    }

    detail::LogIfChanged(raw);

    const char* friendly = detail::MapFriendly(raw);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(460.0f, 170.0f), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(22, 24, 28, 240));
    ImGui::PushStyleColor(ImGuiCol_Border,   IM_COL32(70, 74, 82, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(22.0f, 18.0f));

    ImGui::Begin("##zws_boot_failure", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav);

    ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.30f, 1.0f), "Connection failed");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextWrapped("%s", friendly);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextDisabled("Press END to close.");

    ImGui::End();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

}

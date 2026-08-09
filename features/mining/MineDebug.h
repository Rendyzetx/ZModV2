#pragma once

#include "main.h"
#include <atomic>
#include <deque>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>

namespace MineDebug {

    inline std::atomic<bool> g_enabled{ false };
    inline bool              g_pauseLog       = false;
    inline bool              g_fileLogEnabled = true;
    inline bool              g_fileSnapEvery  = true;
    inline int               g_fileSnapStride = 1;
    inline std::ofstream     g_logFile;
    inline std::string       g_logPath;

    struct Snap {

        double  tSec        = 0.0;
        int     frame       = 0;

        bool    inWorld     = false;
        bool    hasLocalPlayer = false;
        const char* worldName = "-";

        Vector3  pos         = {};
        Vector2i tile        = { 0, 0 };
        Vector3  vel         = {};
        bool     grounded    = false;

        bool     amEnabled   = false;
        bool     hasTarget   = false;
        Vector2i targetTile  = { 0, 0 };
        const char* targetKind = "-";
        bool     breakingActive = false;
        Vector2i breakingTile   = { 0, 0 };
        int      breakingHits   = 0;
        bool     lingerActive   = false;
        Vector2i lingerTile     = { 0, 0 };

        int      moveMethod   = 0;
        int      pathLen      = 0;
        int      pathIdx      = 0;
        Vector2i waypointTile = { 0, 0 };
        Vector3  waypointPos  = {};
        float    wdist        = 0.f;
        float    tileW        = 0.32f;

        int      intervalMs       = 0;
        double   lastPortalSec    = 0.0;
        double   sinceLastPortalS = 0.0;
        bool     cooldownReady    = false;
        bool     nextTilePassable = false;
        const char* fastDecision  = "-";
    };
    inline Snap g_snap;

    inline std::string _MakeLogPath() {
        char* appdata = nullptr; size_t alen = 0;
        std::string root;
        if (_dupenv_s(&appdata, &alen, "APPDATA") == 0 && appdata) {
            root = std::string(appdata) + "\\ZMod\\diagnostics";
            free(appdata);
        } else {
            root = ".\\diagnostics";
        }
        std::error_code ec;
        std::filesystem::create_directories(root, ec);
        time_t now = time(nullptr);
        struct tm tmv{}; localtime_s(&tmv, &now);
        char ts[32];
        strftime(ts, sizeof(ts), "%Y%m%d-%H%M%S", &tmv);
        return root + "\\minedebug-" + ts + ".log";
    }

    inline void OpenLogFile() {
        if (g_logFile.is_open()) return;
        if (!g_fileLogEnabled)   return;
        g_logPath = _MakeLogPath();
        g_logFile.open(g_logPath, std::ios::out | std::ios::app);
        if (g_logFile.is_open()) {
            g_logFile << "# MineDebug log opened at " << g_logPath << "\n";
            g_logFile.flush();
        }
    }
    inline void CloseLogFile() {
        if (g_logFile.is_open()) {
            g_logFile << "# MineDebug log closed\n";
            g_logFile.flush();
            g_logFile.close();
        }
    }

    inline const char* MoveMethodName(int m);
    inline void _FileWriteSnap(const Snap& s) {
        if (!g_fileLogEnabled || !g_fileSnapEvery) return;
        if (!g_logFile.is_open()) return;
        if (g_fileSnapStride > 1 && (s.frame % g_fileSnapStride) != 0) return;
        char buf[1024];
        int n = snprintf(buf, sizeof(buf),
            "%9.3f SNAP f=%d method=%s tileW=%.3f "
            "pos=(%.3f,%.3f,%.3f) tile=(%d,%d) vel=(%.3f,%.3f,%.3f) grnd=%d "
            "amOn=%d hasTgt=%d tgtKind=%s tgtTile=(%d,%d) "
            "brk=%d brkTile=(%d,%d) hits=%d "
            "linger=%d lingerTile=(%d,%d) "
            "path len=%d idx=%d wp=(%d,%d) wpos=(%.3f,%.3f,%.3f) wdist=%.4f "
            "intervalMs=%d sinceLastPortal=%.4f cdReady=%d nextPass=%d dec=%s\n",
            s.tSec, s.frame, MoveMethodName(s.moveMethod), s.tileW,
            s.pos.x, s.pos.y, s.pos.z, s.tile.x, s.tile.y,
            s.vel.x, s.vel.y, s.vel.z, (int)s.grounded,
            (int)s.amEnabled, (int)s.hasTarget,
            s.targetKind ? s.targetKind : "-",
            s.targetTile.x, s.targetTile.y,
            (int)s.breakingActive, s.breakingTile.x, s.breakingTile.y, s.breakingHits,
            (int)s.lingerActive, s.lingerTile.x, s.lingerTile.y,
            s.pathLen, s.pathIdx, s.waypointTile.x, s.waypointTile.y,
            s.waypointPos.x, s.waypointPos.y, s.waypointPos.z, s.wdist,
            s.intervalMs, s.sinceLastPortalS,
            (int)s.cooldownReady, (int)s.nextTilePassable,
            s.fastDecision ? s.fastDecision : "-");
        (void)n;
        g_logFile.write(buf, strlen(buf));
        g_logFile.flush();
    }

    inline void SetSnap(const Snap& s) {
        g_snap = s;
        _FileWriteSnap(s);
    }

    struct LogLine {
        double      tSec;
        std::string msg;
    };
    inline std::deque<LogLine> g_log;
    inline const size_t        kLogMax = 1024;

    inline void Log(const char* fmt, ...) {
        if (!g_enabled.load(std::memory_order_relaxed)) return;
        if (g_pauseLog) return;
        char buf[512];
        va_list ap; va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        double t = (double)ImGui::GetTime();
        g_log.push_back({ t, std::string(buf) });
        while (g_log.size() > kLogMax) g_log.pop_front();
        if (g_fileLogEnabled && g_logFile.is_open()) {
            g_logFile << std::fixed;
            g_logFile.precision(3);
            g_logFile << t << " LOG  " << buf << "\n";
            g_logFile.flush();
        }
    }

    inline void ClearLog() { g_log.clear(); }

    inline const char* MoveMethodName(int m) {
        switch (m) {
            case 0: return "Fast";
            case 1: return "Smooth";
            case 2: return "Portal";
        }
        return "?";
    }

    inline void SetEnabled(bool on) {
        bool prev = g_enabled.exchange(on, std::memory_order_relaxed);
        if (on && !prev)      OpenLogFile();
        else if (!on && prev) CloseLogFile();
    }
    inline void HandleHotkey() {
        if (ImGui::IsKeyPressed(ImGuiKey_F8, false))
            SetEnabled(!g_enabled.load(std::memory_order_relaxed));
    }

    inline void Render() {
        HandleHotkey();
        if (!g_enabled.load(std::memory_order_relaxed)) return;

        ImGui::SetNextWindowSize(ImVec2(540, 660), ImGuiCond_FirstUseEver);
        bool open = true;
        if (!ImGui::Begin("Mine Debug  (F8)", &open,
                          ImGuiWindowFlags_NoCollapse))
        {
            ImGui::End();
            if (!open) SetEnabled(false);
            return;
        }
        if (!open) {
            SetEnabled(false);
            ImGui::End();
            return;
        }

        const Snap& s = g_snap;
        char b1[96], b2[96], b3[96];
        auto Fv2 = [&](char* b, size_t n, Vector2i v) {
            snprintf(b, n, "(%d,%d)", v.x, v.y); return b;
        };
        auto Fv3 = [&](char* b, size_t n, Vector3 v) {
            snprintf(b, n, "(%.3f, %.3f, %.3f)", v.x, v.y, v.z); return b;
        };

        ImGui::Text("t=%.3fs  frame=%d  world=%s  inWorld=%s  player=%s",
                    s.tSec, s.frame, s.worldName ? s.worldName : "-",
                    s.inWorld ? "Y" : "N",
                    s.hasLocalPlayer ? "Y" : "N");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("pos    %s", Fv3(b1, sizeof(b1), s.pos));
            ImGui::Text("tile   %s", Fv2(b2, sizeof(b2), s.tile));
            ImGui::Text("vel    %s   grounded=%s",
                        Fv3(b1, sizeof(b1), s.vel), s.grounded ? "Y" : "N");
        }

        if (ImGui::CollapsingHeader("AutoMine", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("enabled=%s  hasTarget=%s",
                        s.amEnabled ? "Y" : "N", s.hasTarget ? "Y" : "N");
            ImGui::Text("target kind=%s tile=%s",
                        s.targetKind ? s.targetKind : "-",
                        s.hasTarget ? Fv2(b1, sizeof(b1), s.targetTile) : "-");
            ImGui::Text("breaking=%s tile=%s hits=%d",
                        s.breakingActive ? "Y" : "N",
                        s.breakingActive ? Fv2(b1, sizeof(b1), s.breakingTile) : "-",
                        s.breakingHits);
            ImGui::Text("linger=%s tile=%s",
                        s.lingerActive ? "Y" : "N",
                        s.lingerActive ? Fv2(b1, sizeof(b1), s.lingerTile) : "-");
        }

        if (ImGui::CollapsingHeader("Path / Mover",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("method=%s   tileW=%.3f", MoveMethodName(s.moveMethod), s.tileW);
            ImGui::Text("path len=%d  idx=%d  waypoint tile=%s",
                        s.pathLen, s.pathIdx,
                        Fv2(b1, sizeof(b1), s.waypointTile));
            ImGui::Text("waypoint pos %s", Fv3(b2, sizeof(b2), s.waypointPos));
            ImGui::Text("wdist  %.4f", s.wdist);
        }

        if (ImGui::CollapsingHeader("Fast branch decisions",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("intervalMs=%d  lastPortal=%.3f  since=%.3fs  cdReady=%s",
                        s.intervalMs, s.lastPortalSec, s.sinceLastPortalS,
                        s.cooldownReady ? "Y" : "N");
            ImGui::Text("nextTilePassable=%s   decision=%s",
                        s.nextTilePassable ? "Y" : "N",
                        s.fastDecision ? s.fastDecision : "-");
        }

        ImGui::Separator();

        ImGui::Checkbox("Write to file", &g_fileLogEnabled);
        ImGui::SameLine();
        ImGui::Checkbox("Snap per frame", &g_fileSnapEvery);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::InputInt("Snap stride", &g_fileSnapStride);
        if (g_fileSnapStride < 1) g_fileSnapStride = 1;
        if (!g_logPath.empty()) {
            ImGui::TextWrapped("file: %s", g_logPath.c_str());
        } else {
            ImGui::TextDisabled("file: (not opened yet)");
        }

        ImGui::Separator();

        ImGui::Checkbox("Pause log", &g_pauseLog);
        ImGui::SameLine();
        if (ImGui::Button("Clear log")) ClearLog();
        ImGui::SameLine();
        ImGui::Text("entries=%zu / %zu   F8 to hide", g_log.size(), kLogMax);

        if (ImGui::BeginChild("##minedbg_log", ImVec2(0, 0), true,
                              ImGuiWindowFlags_HorizontalScrollbar)) {
            for (const auto& l : g_log) {
                ImGui::Text("%9.3f  %s", l.tSec, l.msg.c_str());
            }
            if (!g_pauseLog) ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        ImGui::End();
    }

}

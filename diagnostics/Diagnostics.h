#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <windows.h>
#include "main.h"
#include "XorStr.h"

namespace Diagnostics {

    inline bool        g_enabled = false;
    inline std::mutex  g_mutex;
    inline bool        g_runActive   = false;
    inline std::string g_runDir;
    inline std::string g_runBot;
    inline std::string g_runWorld;
    inline std::chrono::steady_clock::time_point g_runStarted;
    inline std::ofstream g_eventsStream;
    inline std::ofstream g_progressStream;
    inline std::chrono::steady_clock::time_point g_lastProgressSample;
    inline bool        g_snapshotWritten = false;

    struct RunStats {
        int targetsPicked        = 0;
        int targetsBroken        = 0;
        int targetsUnreachable   = 0;
        int targetsStuck         = 0;
        int totalBreakHits       = 0;
        int dropsBound           = 0;
        int dropsConsumed        = 0;
        int dropsNoBail          = 0;
        int dropsHardCap         = 0;
        int pathBuilt            = 0;
        int pathFailed           = 0;
        int combatHits           = 0;
        int repairsDone          = 0;

        std::map<std::string,int> picksByKind;
        std::map<std::string,int> brokenByKind;
        std::map<std::string,int> unreachByKind;
    };
    inline RunStats g_stats;

    inline std::string GetAppDataDiagnosticsRoot() {
        char* appdata = nullptr;
        size_t len = 0;
        if (_dupenv_s(&appdata, &len, "APPDATA") != 0 || !appdata) return {};
        std::string root = std::string(appdata) + "\\ZMod\\diagnostics";
        free(appdata);
        return root;
    }

    inline std::string MakeUTCTimestamp() {
        time_t now = time(nullptr);
        struct tm gmt {};
        gmtime_s(&gmt, &now);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y%m%dT%H%M%SZ", &gmt);
        return buf;
    }

    inline long long MsSinceStart() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - g_runStarted).count();
    }

    struct ProgressRow {
        long long ms;
        int enemiesKilled, totalEnemies;
        int gemsBroken, totalGems;
        int nuggets, totalNuggets;
        Vector3 playerPos;
    };

    inline bool ReadProgressDirect(ProgressRow& out) {
        if (!g_LocalPlayer) return false;
        __try {
            void* pd = *(void**)((uintptr_t)g_LocalPlayer + 0x58);
            if (!pd) return false;
            void* wcsd = *(void**)((uintptr_t)pd + 0x28);
            if (!wcsd) return false;
            out.enemiesKilled = *(int*)((uintptr_t)wcsd + 0x18);
            out.totalEnemies  = *(int*)((uintptr_t)wcsd + 0x1C);
            out.gemsBroken    = *(int*)((uintptr_t)wcsd + 0x20);
            out.totalGems     = *(int*)((uintptr_t)wcsd + 0x24);
            out.nuggets       = *(int*)((uintptr_t)wcsd + 0x28);
            out.totalNuggets  = *(int*)((uintptr_t)wcsd + 0x2C);
            return (out.totalGems >= 0 && out.totalGems < 1000 &&
                    out.totalNuggets >= 0 && out.totalNuggets < 1000);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    }

    inline Vector3 ReadPlayerPosSafe() {
        Vector3 p{};
        if (!g_LocalPlayer || !PathRenderer::oGetTransform || !PathRenderer::oGetPosition)
            return p;
        __try {
            void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
            if (lt) p = PathRenderer::oGetPosition(lt);
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
        return p;
    }

    inline void DumpBlockMatrix(std::ofstream& out) {
        if (!g_WorldController) return;
        __try {
            void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
            if (!worldObj) return;
            void* layer = *(void**)((uintptr_t)worldObj + g_off_WorldBlockLayer);
            if (!layer || IsBadReadPtr(layer, 0x20)) return;
            int worldSizeX = *(int*)((uintptr_t)layer + 0x18);
            if (worldSizeX <= 0 || worldSizeX > 500) return;
            void** pCols = (void**)((uintptr_t)layer + 0x20);
            int worldSizeY = 0;
            for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
                if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
            if (worldSizeY <= 0 || worldSizeY > 500) return;

            out << "{\n  \"sizeX\":" << worldSizeX << ",\n  \"sizeY\":" << worldSizeY << ",\n";
            out << "  \"format\":\"per row, RLE: id*count comma-separated; rows in y-descending order (top first)\",\n";
            out << "  \"rows\":[\n";
            for (int y = worldSizeY - 1; y >= 0; --y) {
                out << "    \"";
                int prev = -1, runLen = 0;
                bool first = true;
                for (int x = 0; x < worldSizeX; ++x) {
                    void* col = pCols[x];
                    int bt = 0;
                    if (col) {
                        char* colData = (char*)((uintptr_t)col + 0x20);
                        bt = *(int*)((uintptr_t)(colData + (y * 0x28)));
                    }
                    if (bt == prev) ++runLen;
                    else {
                        if (runLen > 0) {
                            if (!first) out << ",";
                            out << prev << "*" << runLen;
                            first = false;
                        }
                        prev = bt; runLen = 1;
                    }
                }
                if (runLen > 0) {
                    if (!first) out << ",";
                    out << prev << "*" << runLen;
                }
                out << "\"" << (y > 0 ? "," : "") << "\n";
            }
            out << "  ]\n}\n";
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    inline nlohmann::json TargetSnapshot() {
        nlohmann::json j = nlohmann::json::object();
        auto v2 = [](Vector2i p){ return nlohmann::json::array({ p.x, p.y }); };

        {
            std::lock_guard<std::mutex> lk(g_gemstoneLocationsMutex);
            auto a = nlohmann::json::array();
            for (const auto& g : g_gemstoneLocations)
                a.push_back({ {"mp", v2(g.mapPoint)}, {"bt", (int)g.blockType} });
            j["gemstones"] = a;
        }
        {
            std::lock_guard<std::mutex> lk(g_nuggetLocationsMutex);
            auto a = nlohmann::json::array();
            for (const auto& g : g_nuggetLocations)
                a.push_back({ {"mp", v2(g.mapPoint)}, {"bt", (int)g.blockType} });
            j["nuggets"] = a;
        }
        {
            std::lock_guard<std::mutex> lk(g_lightCrystalLocationsMutex);
            auto a = nlohmann::json::array();
            for (const auto& g : g_lightCrystalLocations)
                a.push_back({ {"mp", v2(g.mapPoint)}, {"bt", (int)g.blockType} });
            j["light_crystals"] = a;
        }
        {
            std::lock_guard<std::mutex> lk(g_potLocationsMutex);
            auto a = nlohmann::json::array();
            for (const auto& g : g_potLocations)
                a.push_back({ {"mp", v2(g.mapPoint)}, {"bt", (int)g.blockType} });
            j["pots"] = a;
        }
        {
            std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);
            auto a = nlohmann::json::array();
            for (const auto& p : g_giftBoxLocations) a.push_back(v2(p));
            j["gift_boxes"] = a;
        }
        {
            std::lock_guard<std::mutex> lk(g_treasureLocationsMutex);
            auto a = nlohmann::json::array();
            for (const auto& t : g_treasureLocations)
                a.push_back({ {"mp", v2(t.mapPoint)}, {"bt", (int)t.blockType} });
            j["treasures"] = a;
        }
        {
            std::lock_guard<std::mutex> lk(g_netherKeyLocationsMutex);
            auto a = nlohmann::json::array();
            for (const auto& p : g_netherKeyLocations) a.push_back(v2(p));
            j["nether_keys"] = a;
        }
        {
            std::lock_guard<std::mutex> lk(g_exitPortalLocationsMutex);
            auto a = nlohmann::json::array();
            for (const auto& p : g_exitPortalLocations) a.push_back(v2(p));
            j["exits"] = a;
        }
        {
            std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
            auto a = nlohmann::json::array();
            for (const auto& fc : g_floorCollectables)
                a.push_back({ {"id", fc.id}, {"mp", v2(fc.mapPoint)}, {"bt", (int)fc.blockType} });
            j["floor_collectables"] = a;
        }
        return j;
    }

    inline void StartRun(const std::string& bot, const std::string& worldName) {
        std::lock_guard<std::mutex> lk(g_mutex);
        if (!g_enabled) return;
        if (g_runActive) return;

        std::string root = GetAppDataDiagnosticsRoot();
        if (root.empty()) return;
        std::error_code ec;
        std::filesystem::create_directories(root, ec);

        std::string ts = MakeUTCTimestamp();
        std::string safeWorld = worldName;
        for (auto& c : safeWorld) {
            if (!isalnum((unsigned char)c) && c != '-' && c != '_') c = '_';
        }
        if (safeWorld.empty()) safeWorld = "unknown";
        g_runDir = root + "\\" + ts + "_" + bot + "_" + safeWorld;
        std::filesystem::create_directories(g_runDir, ec);

        g_runBot     = bot;
        g_runWorld   = worldName;
        g_runStarted = std::chrono::steady_clock::now();
        g_lastProgressSample = g_runStarted - std::chrono::seconds(60);
        g_stats      = RunStats{};
        g_runActive  = true;

        nlohmann::json meta;
        meta["bot"]       = bot;
        meta["world"]     = worldName;
        meta["startedUTC"]= ts;
        meta["dirAbs"]    = g_runDir;
        std::ofstream metaOut(g_runDir + "\\meta.json", std::ios::trunc);
        if (metaOut) metaOut << meta.dump(2);

        g_snapshotWritten = false;

        g_eventsStream.open(g_runDir + "\\events.jsonl", std::ios::trunc);
        g_progressStream.open(g_runDir + "\\progress_timeline.csv", std::ios::trunc);
        if (g_progressStream) {
            g_progressStream << "ms,enemiesKilled,totalEnemies,gemsBroken,totalGems,"
                             << "nuggets,totalNuggets,playerX,playerY\n";
        }
    }

    inline void SnapshotWorld() {
        if (!g_runActive) return;

        nlohmann::json targets = TargetSnapshot();
        Vector3 pp = ReadPlayerPosSafe();
        ProgressRow pr{};
        bool gotProg = ReadProgressDirect(pr);

        std::lock_guard<std::mutex> lk(g_mutex);
        if (g_snapshotWritten) return;
        g_snapshotWritten = true;

        std::ofstream snap(g_runDir + "\\world_snapshot.json", std::ios::trunc);
        if (snap) {
            nlohmann::json j;
            j["player_pos_at_snapshot"] = { pp.x, pp.y, pp.z };
            if (gotProg) {
                j["mine_progress_initial"] = {
                    {"enemiesKilled", pr.enemiesKilled}, {"totalEnemies", pr.totalEnemies},
                    {"gemsBroken",    pr.gemsBroken},    {"totalGems",    pr.totalGems},
                    {"nuggets",       pr.nuggets},       {"totalNuggets", pr.totalNuggets}
                };
            }
            j["snapshot_ms"]     = MsSinceStart();
            j["targets_initial"] = targets;
            snap << j.dump(2);
        }

        std::ofstream bm(g_runDir + "\\block_matrix.json", std::ios::trunc);
        if (bm) DumpBlockMatrix(bm);
    }

    inline void EndRun(const std::string& reason) {
        std::lock_guard<std::mutex> lk(g_mutex);
        if (!g_runActive) return;

        nlohmann::json sum;
        sum["bot"]              = g_runBot;
        sum["world"]            = g_runWorld;
        sum["endReason"]        = reason;
        sum["durationMs"]       = MsSinceStart();
        sum["targetsPicked"]    = g_stats.targetsPicked;
        sum["targetsBroken"]    = g_stats.targetsBroken;
        sum["targetsUnreachable"] = g_stats.targetsUnreachable;
        sum["targetsStuck"]     = g_stats.targetsStuck;
        sum["totalBreakHits"]   = g_stats.totalBreakHits;
        sum["dropsBound"]       = g_stats.dropsBound;
        sum["dropsConsumed"]    = g_stats.dropsConsumed;
        sum["dropsNoBail"]      = g_stats.dropsNoBail;
        sum["dropsHardCap"]     = g_stats.dropsHardCap;
        sum["pathBuilt"]        = g_stats.pathBuilt;
        sum["pathFailed"]       = g_stats.pathFailed;
        sum["combatHits"]       = g_stats.combatHits;
        sum["repairsDone"]      = g_stats.repairsDone;
        sum["picksByKind"]      = g_stats.picksByKind;
        sum["brokenByKind"]     = g_stats.brokenByKind;
        sum["unreachByKind"]    = g_stats.unreachByKind;
        if (g_stats.targetsBroken > 0)
            sum["avgHitsPerBreak"] = (double)g_stats.totalBreakHits / g_stats.targetsBroken;

        ProgressRow pr{};
        if (ReadProgressDirect(pr)) {
            sum["mine_progress_final"] = {
                {"enemiesKilled", pr.enemiesKilled}, {"totalEnemies", pr.totalEnemies},
                {"gemsBroken",    pr.gemsBroken},    {"totalGems",    pr.totalGems},
                {"nuggets",       pr.nuggets},       {"totalNuggets", pr.totalNuggets}
            };
        }

        std::ofstream sumOut(g_runDir + "\\summary.json", std::ios::trunc);
        if (sumOut) sumOut << sum.dump(2);

        if (g_eventsStream)   { g_eventsStream.flush();   g_eventsStream.close(); }
        if (g_progressStream) { g_progressStream.flush(); g_progressStream.close(); }

        g_runActive = false;
        g_runDir.clear();
    }

    inline void LogEvent(const nlohmann::json& evt) {
        std::lock_guard<std::mutex> lk(g_mutex);
        if (!g_runActive || !g_eventsStream) return;
        nlohmann::json wrapped = evt;
        wrapped["ms"] = MsSinceStart();
        g_eventsStream << wrapped.dump() << "\n";

        static int flushCounter = 0;
        if (++flushCounter >= 50) { g_eventsStream.flush(); flushCounter = 0; }
    }

    inline void Ev(const char* type, nlohmann::json extra = {}) {
        if (!g_enabled || !g_runActive) return;
        nlohmann::json j = std::move(extra);
        j["t"] = type;
        LogEvent(j);
    }

    inline void StatPick(const std::string& kind) {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_stats.targetsPicked++;
        g_stats.picksByKind[kind]++;
    }
    inline void StatBreak(const std::string& kind, int hits) {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_stats.targetsBroken++;
        g_stats.totalBreakHits += hits;
        g_stats.brokenByKind[kind]++;
    }
    inline void StatUnreach(const std::string& kind) {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_stats.targetsUnreachable++;
        g_stats.unreachByKind[kind]++;
    }
    inline void StatStuck()        { std::lock_guard<std::mutex> lk(g_mutex); g_stats.targetsStuck++; }
    inline void StatDropBound()    { std::lock_guard<std::mutex> lk(g_mutex); g_stats.dropsBound++; }
    inline void StatDropConsumed() { std::lock_guard<std::mutex> lk(g_mutex); g_stats.dropsConsumed++; }
    inline void StatDropNoBail()   { std::lock_guard<std::mutex> lk(g_mutex); g_stats.dropsNoBail++; }
    inline void StatDropHardCap()  { std::lock_guard<std::mutex> lk(g_mutex); g_stats.dropsHardCap++; }
    inline void StatPathBuilt()    { std::lock_guard<std::mutex> lk(g_mutex); g_stats.pathBuilt++; }
    inline void StatPathFailed()   { std::lock_guard<std::mutex> lk(g_mutex); g_stats.pathFailed++; }
    inline void StatCombatHit()    { std::lock_guard<std::mutex> lk(g_mutex); g_stats.combatHits++; }
    inline void StatRepair()       { std::lock_guard<std::mutex> lk(g_mutex); g_stats.repairsDone++; }

    inline void TickProgressSample() {
        if (!g_enabled || !g_runActive) return;

        if (!g_snapshotWritten && MsSinceStart() >= 3000) SnapshotWorld();
        auto now = std::chrono::steady_clock::now();
        auto sinceMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_lastProgressSample).count();
        if (sinceMs < 5000) return;
        g_lastProgressSample = now;
        ProgressRow pr{};
        if (!ReadProgressDirect(pr)) return;
        Vector3 pp = ReadPlayerPosSafe();
        std::lock_guard<std::mutex> lk(g_mutex);
        if (g_progressStream) {
            g_progressStream << MsSinceStart() << ","
                             << pr.enemiesKilled << "," << pr.totalEnemies << ","
                             << pr.gemsBroken    << "," << pr.totalGems    << ","
                             << pr.nuggets       << "," << pr.totalNuggets << ","
                             << pp.x             << "," << pp.y << "\n";
            g_progressStream.flush();
        }
    }

    inline bool RunActive() { return g_runActive; }
    inline std::string CurrentRunDir() {
        std::lock_guard<std::mutex> lk(g_mutex);
        return g_runDir;
    }

}

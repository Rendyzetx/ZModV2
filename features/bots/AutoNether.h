#pragma once

#include "main.h"
#include "PathFinding.h"
#include "Diagnostics.h"
#include "imgui/imgui.h"
#include <cmath>
#include <chrono>

namespace AutoNether {

    inline bool  g_enabled        = false;

    inline std::chrono::steady_clock::time_point g_resumeAfter =
        std::chrono::steady_clock::now();
    inline bool  g_autoMove       = true;
    inline bool  g_showPath       = true;
    inline float g_hitInterval    = 0.25f;
    inline float g_stopRange      = 0.18f;
    inline float g_maxHitRange    = 2.0f;
    inline float g_minMonsterHp   = 1.0f;
    inline float g_flightScale    = 0.9f;
    inline bool  g_targetEnemies  = true;
    inline bool  g_targetTreasure = true;
    inline bool  g_targetGifts    = true;
    inline bool  g_targetFloor    = true;
    inline bool  g_targetCollectables = true;
    inline bool  g_netherOnly     = false;

    inline bool  g_deepLoop   = false;
    inline bool  g_deepEntryNetwork = false;
    inline std::chrono::steady_clock::time_point g_deepNextAttempt =
        std::chrono::steady_clock::now() - std::chrono::seconds(30);
    inline std::chrono::steady_clock::time_point g_deepVerifyDeadline;
    inline bool  g_deepAwaiting     = false;
    inline int   g_deepScrollSnap   = -1;
    inline int   g_deepEntryFails   = 0;
    inline int   g_deepRuns         = 0;
    inline bool  g_deepHalted       = false;
    inline char  g_deepMsg[112]     = {};

    inline bool  g_regLoop        = false;
    inline bool  g_regLevels[5]   = {};
    inline int   g_regCurLevel    = 0;
    inline int   g_regRuns        = 0;
    inline int   g_regScrolls     = 0;
    inline char  g_regMsg[112]    = {};
    inline bool  g_regAwaiting    = false;
    inline bool  g_regHalted      = false;
    inline int   g_regEntryFails  = 0;
    inline int   g_regScrollSnap  = -1;
    inline int   g_regRotIdx      = 0;
    inline std::chrono::steady_clock::time_point g_regNextAttempt =
        std::chrono::steady_clock::now() - std::chrono::seconds(30);
    inline std::chrono::steady_clock::time_point g_regVerifyDeadline;

    inline bool  g_priorityClosest = false;

    inline bool  g_antiSleep = true;
    inline bool  g_antiAfk   = true;

    inline PathRenderer::PathOptions g_pathOpts = {
         true,
           10,
               false,
         0.25f,
         false,
    };

    inline PathRenderer::MoveMethod g_moveMethod = PathRenderer::MoveMethod::InputDrive;

    inline float  g_portalIntervalMs = 0.f;
    inline double g_lastPortalSec    = 0.0;
    inline float g_moveSpeed      = 2.5f;
    inline float g_pfStopRange    = 0.18f;
    inline float g_bodyOffsetFrac = 0.4f;

    inline bool  g_pathGlow       = true;
    inline bool  g_pathFlow       = true;
    inline bool  g_pathPulse      = true;

    inline float g_hitJitterPct   = 0.15f;
    inline float g_stopJitterPct  = 0.10f;
    inline int   g_idlePauseMinMs = 50;
    inline int   g_idlePauseMaxMs = 200;

    inline unsigned RngNext() {
        static unsigned s = (GetCurrentProcessId() * 2654435761u) | 1u;
        s ^= s << 13; s ^= s >> 17; s ^= s << 5;
        return s;
    }
    inline float RngUnit() { return (RngNext() & 0xFFFFFF) / (float)0x1000000; }
    inline float Jitter(float base, float pct) {
        if (pct <= 0.f) return base;
        float f = 1.f + (RngUnit() * 2.f - 1.f) * pct;
        return base * f;
    }
    inline float g_curHitInterval = 0.25f;
    inline float g_curStopRange   = 0.18f;

    inline long long                              g_stallKey    = 0;
    inline float                                  g_stallAnchor = FLT_MAX;
    inline std::chrono::steady_clock::time_point  g_stallSince  =
        std::chrono::steady_clock::now();
    static constexpr float STALL_PROGRESS_EPS = 0.30f;
    static constexpr int   STALL_TIMEOUT_MS   = 4000;

    inline void StopDriving() {
        if (g_moveMethod == PathRenderer::MoveMethod::InputDrive) {
            PathFinder_ClearInputs();
            if (oSetVelocity && g_LocalPlayer)
                PathRenderer::SafeSetVelocity(g_LocalPlayer,{ 0.f, 0.f, 0.f });
        }
    }

    inline std::vector<Vector2i>      g_pathTiles;
    inline std::vector<Vector2>       g_pathOffsets;
    inline PathRenderer::PathStatus   g_pathStatus = PathRenderer::PathStatus::Success;
    inline int                        g_pathIdx    = 0;
    inline std::mutex                 g_pathMutex;
    inline Vector2i                   g_pathLastGoal = { -9999, -9999 };
    inline std::chrono::steady_clock::time_point g_pathLastBuild =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);

    struct OpenedTile { Vector2i mp; std::chrono::steady_clock::time_point at; };
    inline std::vector<OpenedTile> g_recentlyOpened;
    inline std::mutex              g_recentlyOpenedMutex;

    static constexpr int   RECENT_OPEN_TTL_MS = 60000;
    static constexpr size_t RECENT_OPEN_CAP   = 256;

    inline void PurgeRecentlyOpened_locked() {
        auto now = std::chrono::steady_clock::now();
        g_recentlyOpened.erase(std::remove_if(
            g_recentlyOpened.begin(), g_recentlyOpened.end(),
            [&](const OpenedTile& t) {
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - t.at).count() >= RECENT_OPEN_TTL_MS;
            }),
            g_recentlyOpened.end());
        if (g_recentlyOpened.size() > RECENT_OPEN_CAP)
            g_recentlyOpened.erase(g_recentlyOpened.begin(),
                g_recentlyOpened.begin() + (g_recentlyOpened.size() - RECENT_OPEN_CAP));
    }

    inline bool WasRecentlyOpened(Vector2i mp) {
        std::lock_guard<std::mutex> lk(g_recentlyOpenedMutex);
        PurgeRecentlyOpened_locked();
        for (const auto& t : g_recentlyOpened)
            if (t.mp.x == mp.x && t.mp.y == mp.y) return true;
        return false;
    }
    inline void MarkOpened(Vector2i mp) {
        {
            std::lock_guard<std::mutex> lk(g_recentlyOpenedMutex);
            g_recentlyOpened.push_back({ mp, std::chrono::steady_clock::now() });
            PurgeRecentlyOpened_locked();
        }

        {
            std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);
            g_giftBoxLocations.erase(std::remove_if(
                g_giftBoxLocations.begin(), g_giftBoxLocations.end(),
                [&](const Vector2i& p) { return p.x == mp.x && p.y == mp.y; }),
                g_giftBoxLocations.end());
        }
        {
            std::lock_guard<std::mutex> lk(g_treasureLocationsMutex);
            g_treasureLocations.erase(std::remove_if(
                g_treasureLocations.begin(), g_treasureLocations.end(),
                [&](const TreasureInfo& t) { return t.mapPoint.x == mp.x && t.mapPoint.y == mp.y; }),
                g_treasureLocations.end());
        }
    }

    inline std::chrono::steady_clock::time_point g_lastCollectAttempt =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);
    inline std::chrono::steady_clock::time_point g_collectFirstSent =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);
    inline int       g_collectRetries = 0;
    inline Vector2i  g_collectMp      = { -9999, -9999 };

    inline int g_collectMaxRetries = 40;
    inline int g_giftHardCapMs     = 3500;
    inline int g_giftResendMs      = 2000;

    inline bool GiftBoxStillPresent(Vector2i mp) {
        std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);
        for (const auto& p : g_giftBoxLocations)
            if (p.x == mp.x && p.y == mp.y) return true;
        return false;
    }
    inline bool TreasureStillPresent(Vector2i mp) {
        std::lock_guard<std::mutex> lk(g_treasureLocationsMutex);
        for (const auto& t : g_treasureLocations)
            if (t.mapPoint.x == mp.x && t.mapPoint.y == mp.y) return true;
        return false;
    }

    inline void ResetCollectState(Vector2i newMp) {
        if (g_collectMp.x != newMp.x || g_collectMp.y != newMp.y) {
            g_collectMp = newMp;
            g_collectRetries = 0;
            auto t10 = std::chrono::steady_clock::now() - std::chrono::seconds(10);
            g_lastCollectAttempt = t10;
            g_collectFirstSent   = t10;
        }
    }

    inline void SendXYPacket(const char* id, Vector2i mp) {
        if (!oSimpleBSON_Load || !oAddOneMessageToList || !il2cpp_array_new || !byteClass) return;
        try {
            nlohmann::json j;
            j["ID"] = id;
            j["x"]  = mp.x;
            j["y"]  = mp.y;
            std::vector<uint8_t> bson_data = nlohmann::json::to_bson(j);
            Il2CppArray* arr = il2cpp_array_new(byteClass, bson_data.size());
            if (!arr) return;
            memcpy(arr->items, bson_data.data(), bson_data.size());
            void* pkt = oSimpleBSON_Load(arr);
            if (!pkt) return;
            oAddOneMessageToList(pkt);
        } catch (const std::exception&) {}
    }

    inline void SendRawBson(const nlohmann::json& j) {
        if (!oSimpleBSON_Load || !oAddOneMessageToList || !il2cpp_array_new || !byteClass) return;
        try {
            std::vector<uint8_t> bson = nlohmann::json::to_bson(j);
            Il2CppArray* arr = il2cpp_array_new(byteClass, bson.size());
            if (!arr) return;
            memcpy(arr->items, bson.data(), bson.size());
            void* pkt = oSimpleBSON_Load(arr);
            if (!pkt) return;
            oAddOneMessageToList(pkt);
        } catch (const std::exception&) {}
    }

    inline void SendDeepNetherJoin() {

        { nlohmann::json j; j["ID"] = "yEkd"; j["rpfu"] = nlohmann::json::array({ 0 }); SendRawBson(j); }
        { nlohmann::json j; j["ID"] = "lgfH"; j["JSck"] = true; j["PgfU"] = "DEEPNETHER";
          j["QGtV"] = 0; j["HUeF"] = 0; SendRawBson(j); }
    }

    inline void SendOpenGiftBox(Vector2i mp) {
        Seh_SendRequestItemFromGiftBox(mp);
    }

    inline void SendHitBlock(Vector2i mp) {
        if (!oSendHitBlockMessage) return;
        __try { oSendHitBlockMessage(mp, GetDateTimeTicksGlobal(), false); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    inline void SendNetherExit(Vector2i mp) {
        if (g_currentWeatherType == World::WeatherType::DeepNether)
            Seh_SendRequestDeepNetherExit(mp);
        else
            Seh_SendRequestNetherWorldExit(mp);
    }

    inline bool IsInNetherWorld();

    inline int BloodScrollCount() {

        if (!g_LocalPlayer || !oGetCountByKey) return -1;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return -1;
        PlayerData_InventoryKey k{ (World::BlockType)1467, (InventoryItemType)7 };
        short qty = 0;
        __try { qty = oGetCountByKey(pd, k); } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
        return (int)qty;
    }

    inline void DeepLoopTick() {
        if (!g_enabled || !g_deepLoop || !g_LocalPlayer) return;
        auto now = std::chrono::steady_clock::now();

        if (IsInNetherWorld()) {
            if (g_deepAwaiting) { g_deepAwaiting = false; g_deepEntryFails = 0; ++g_deepRuns; }
            g_deepHalted = false;
            return;
        }

        if (g_deepHalted) {
            if (BloodScrollCount() > 0) { g_deepHalted = false; g_deepEntryFails = 0; g_deepMsg[0] = 0; }
            else return;
        }

        if (g_deepAwaiting) {
            if (now < g_deepVerifyDeadline) return;
            g_deepAwaiting = false;
            int after = BloodScrollCount();
            if (after >= 0 && after < g_deepScrollSnap) {

                g_deepEntryFails  = 0;
                g_deepNextAttempt = now + std::chrono::seconds(15);
                snprintf(g_deepMsg, sizeof(g_deepMsg),
                         "Scroll consumed - waiting for deep-nether to load...");
                return;
            }
            if (++g_deepEntryFails >= 3) {
                g_deepHalted = true;
                snprintf(g_deepMsg, sizeof(g_deepMsg), "Deep entry failed 3x - paused.");
                return;
            }
        }

        if (!g_IsInWorld) return;
        if (now < g_deepNextAttempt) return;
        int scrolls = BloodScrollCount();
        if (scrolls < 0) {
            snprintf(g_deepMsg, sizeof(g_deepMsg), "Reading inventory - waiting...");
            return;
        }
        if (scrolls == 0) {
            g_deepHalted = true;
            snprintf(g_deepMsg, sizeof(g_deepMsg), "Out of Blood Scrolls - paused.");
            return;
        }
        g_deepScrollSnap = scrolls;
        if (g_deepEntryNetwork)
            SendDeepNetherJoin();
        else
            AutoFossil::_SehUseConsumable(g_InventoryControl,
                PlayerData_InventoryKey{ (World::BlockType)1467, (InventoryItemType)7 });
        g_deepAwaiting       = true;
        g_deepVerifyDeadline = now + std::chrono::seconds(8);
        g_deepNextAttempt    = now + std::chrono::seconds(10);
        g_deepMsg[0] = 0;
    }

    inline int RedScrollId(int level) { return 5015 + level; }

    inline int CountRedScroll(int level) {
        if (!g_LocalPlayer || !oGetCountByKey || level < 1 || level > 5) return -1;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return -1;
        PlayerData_InventoryKey k{ (World::BlockType)RedScrollId(level), (InventoryItemType)7 };
        short qty = 0;
        __try { qty = oGetCountByKey(pd, k); } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
        return (int)qty;
    }

    inline void RegularLoopTick() {
        if (!g_enabled || !g_regLoop || !g_LocalPlayer) return;
        if (g_deepLoop) return;
        auto now = std::chrono::steady_clock::now();

        { int tot = 0; for (int i = 0; i < 5; ++i) if (g_regLevels[i]) { int c = CountRedScroll(i + 1); if (c > 0) tot += c; } g_regScrolls = tot; }

        if (IsInNetherWorld()) {
            if (g_regAwaiting) {
                g_regAwaiting = false; g_regEntryFails = 0; ++g_regRuns;
                snprintf(g_regMsg, sizeof(g_regMsg), "Farming level %d (run %d)", g_regCurLevel, g_regRuns);
            }
            g_regHalted = false;
            return;
        }

        if (g_regHalted) {
            if (g_regScrolls > 0) { g_regHalted = false; g_regEntryFails = 0; g_regMsg[0] = 0; }
            else return;
        }

        if (g_regAwaiting) {
            if (now < g_regVerifyDeadline) return;
            g_regAwaiting = false;
            int after = CountRedScroll(g_regCurLevel);
            if (after >= 0 && after < g_regScrollSnap) {
                g_regEntryFails = 0;
                g_regNextAttempt = now + std::chrono::seconds(15);
                snprintf(g_regMsg, sizeof(g_regMsg), "Scroll consumed - entering level %d...", g_regCurLevel);
                return;
            }
            if (++g_regEntryFails >= 3) {
                g_regHalted = true;
                snprintf(g_regMsg, sizeof(g_regMsg), "Level %d entry failed 3x - paused.", g_regCurLevel);
                return;
            }
        }

        if (!g_IsInWorld) return;
        if (now < g_regNextAttempt) return;

        int level = 0;
        for (int n = 0; n < 5; ++n) {
            int idx = (g_regRotIdx + n) % 5;
            if (!g_regLevels[idx]) continue;
            int c = CountRedScroll(idx + 1);
            if (c < 0) { snprintf(g_regMsg, sizeof(g_regMsg), "Reading inventory - waiting..."); return; }
            if (c > 0) { level = idx + 1; g_regRotIdx = (idx + 1) % 5; break; }
        }
        if (level <= 0) {
            bool anySel = false; for (int i = 0; i < 5; ++i) if (g_regLevels[i]) anySel = true;
            g_regHalted = true; g_regCurLevel = 0;
            snprintf(g_regMsg, sizeof(g_regMsg), anySel ? "Out of Red Scrolls - paused." : "Select a level (1-5).");
            return;
        }

        g_regCurLevel   = level;
        g_regScrollSnap = CountRedScroll(level);
        AutoFossil::_SehUseConsumable(g_InventoryControl,
            PlayerData_InventoryKey{ (World::BlockType)RedScrollId(level), (InventoryItemType)7 });
        g_regAwaiting       = true;
        g_regVerifyDeadline = now + std::chrono::seconds(8);
        g_regNextAttempt    = now + std::chrono::seconds(10);
        snprintf(g_regMsg, sizeof(g_regMsg), "Using level %d Red Scroll...", level);
    }

    inline void SendCollectPickup(Vector2i mp) { SendXYPacket("CPu", mp); }

    inline Vector3 PathWaypointWorld(int i, float wz, float tileW) {
        Vector2i t = g_pathTiles[i];
        Vector3  c = PathRenderer::TileToWorldCenter(t.x, t.y, wz, tileW);
        if (i >= 0 && i < (int)g_pathOffsets.size()) {
            c.x += g_pathOffsets[i].x * tileW;
            c.y += g_pathOffsets[i].y * tileW;
        }
        return c;
    }

    enum class Phase { Active, SeekExit };
    inline Phase g_phase = Phase::Active;

    enum class TargetKind { None, Enemy, Treasure, Gift, Exit, Key, Collectable };
    inline TargetKind g_targetKind    = TargetKind::None;
    inline Vector3    g_targetWorldPos = {};
    inline Vector2i   g_targetMapPt   = {};
    inline int        g_targetEnemyID = -1;
    inline int        g_targetCollectId = -1;
    inline bool       g_hasTarget     = false;

    inline std::vector<int> g_collectedIds;
    inline std::mutex       g_collectedIdsMutex;
    inline bool WasCollectableConsumed(int id) {
        std::lock_guard<std::mutex> lk(g_collectedIdsMutex);
        for (int v : g_collectedIds) if (v == id) return true;
        return false;
    }
    inline void MarkCollectableConsumed(int id) {
        std::lock_guard<std::mutex> lk(g_collectedIdsMutex);
        g_collectedIds.push_back(id);
        if (g_collectedIds.size() > 256) g_collectedIds.erase(g_collectedIds.begin());
    }

    inline float g_hitTimer  = 0.f;
    inline float g_runtime   = 0.f;
    inline int   g_kills     = 0;
    inline int   g_collects  = 0;

    inline bool  g_hadFault  = false;
    inline char  g_faultMsg[128] = {};
    inline int   g_faultStreak = 0;
    inline std::chrono::steady_clock::time_point g_faultRetryAt{};

    inline bool IsInNetherWorld() {
        if (!g_WorldController || !g_off_WCWorld) return false;
        __try {
            void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
            if (!worldObj) return false;
            int layout = *(int*)((uintptr_t)worldObj + 0x110);
            if (layout == World::WorldLayoutType::LayoutNetherWorld) return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }

        return g_currentWeatherType == World::WeatherType::DeepNether;
    }

    inline bool IsInNether() {
        if (!g_netherOnly) return true;
        return IsInNetherWorld();
    }

    inline bool ReadNetherKeyProgress(int& collected, int& total) {
        if (!g_LocalPlayer) return false;
        __try {
            void* pd = *(void**)((uintptr_t)g_LocalPlayer + 0x58);
            if (!pd) return false;
            void* wcsd = *(void**)((uintptr_t)pd + 0x28);
            if (!wcsd) return false;
            collected = *(int*)((uintptr_t)wcsd + 0x1C);
            total     = *(int*)((uintptr_t)wcsd + 0x20);

            if (total < 0 || total > 100 || collected < 0 || collected > total + 4)
                return false;
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    }

    inline bool IsNetherExitUnlocked() {
        int collected = 0, total = 0;
        if (ReadNetherKeyProgress(collected, total)) {
            return total > 0 && collected >= total;
        }

        std::lock_guard<std::mutex> lk(g_netherKeyLocationsMutex);
        return g_netherKeyLocations.empty();
    }

    inline void ResetPhase() {
        g_phase      = Phase::Active;
        g_hasTarget  = false;
        g_targetKind = TargetKind::None;
    }

    inline bool PickTarget() {
        if (!g_WorldController || !g_LocalPlayer) return false;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return false;

        void*   lt        = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!lt) return false;
        Vector3 playerPos = PathRenderer::oGetPosition(lt);

        float       bestDist = FLT_MAX;
        TargetKind  bestKind = TargetKind::None;
        Vector3     bestPos  = {};
        Vector2i    bestPt   = {};
        int         bestID   = -1;

        if (bestKind == TargetKind::Key) {
            g_targetKind     = bestKind;
            g_targetWorldPos = bestPos;
            g_targetMapPt    = bestPt;
            g_targetEnemyID  = -1;
            g_hasTarget      = true;
            return true;
        }

        if (g_targetEnemies && g_phase == Phase::Active) {
            std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
            for (void* inst : g_AIInstances) {
                if (!inst || IsBadReadPtr(inst, sizeof(void*))) continue;
                AIBase* base = *(AIBase**)((uintptr_t)inst + g_off_AIBase);
                if (!base || IsBadReadPtr(base, sizeof(AIBase))) continue;
                if (base->health <= (int)g_minMonsterHp) continue;
                if (IsProjectileEnemyType((int)base->enemyType)) continue;
                Vector3 ep = *(Vector3*)((uintptr_t)inst + g_off_AITempPosition);
                float dx = ep.x - playerPos.x, dy = ep.y - playerPos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < bestDist) {
                    bestDist = dist;
                    bestKind = TargetKind::Enemy;
                    bestPos  = ep;
                    bestID   = base->id;
                    if (oConvertWorldPointToMapPoint && g_WorldController)
                        bestPt = oConvertWorldPointToMapPoint(g_WorldController, ep);
                }
            }
        }

        const float tileWLocal = (PathRenderer::g_tileSize.x > 0.001f)
                                   ? PathRenderer::g_tileSize.x : 0.32f;
        auto tileCenter = [&](Vector2i mp) -> Vector3 {
            return PathRenderer::TileToWorldCenter(mp.x, mp.y, 0.f, tileWLocal);
        };

        bool keysRemain = false;
        {
            std::lock_guard<std::mutex> lk(g_netherKeyLocationsMutex);
            for (const auto& pt : g_netherKeyLocations) {
                if (WasRecentlyOpened(pt)) continue;
                keysRemain = true;
                Vector3 wp = tileCenter(pt);
                float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < bestDist) {
                    bestDist = dist; bestKind = TargetKind::Key;
                    bestPos = wp; bestPt = pt;
                }
            }
        }
        bool exitUnlocked = !keysRemain;

        const bool noGate = g_priorityClosest;

        if ((noGate || bestKind == TargetKind::None) && g_targetTreasure && g_phase == Phase::Active) {
            std::lock_guard<std::mutex> lk(g_treasureLocationsMutex);
            for (const auto& tr : g_treasureLocations) {
                if (WasRecentlyOpened(tr.mapPoint)) continue;
                Vector3 wp = tileCenter(tr.mapPoint);
                float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < bestDist) {
                    bestDist = dist; bestKind = TargetKind::Treasure;
                    bestPos = wp; bestPt = tr.mapPoint;
                }
            }
        }

        if ((noGate || bestKind == TargetKind::None) && g_targetGifts) {
            std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);
            for (const auto& pt : g_giftBoxLocations) {
                if (WasRecentlyOpened(pt)) continue;
                Vector3 wp = tileCenter(pt);
                float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < bestDist) {
                    bestDist = dist; bestKind = TargetKind::Gift;
                    bestPos = wp; bestPt = pt;
                }
            }
        }

        int bestCollectId = -1;
        if ((noGate || bestKind == TargetKind::None) && g_targetCollectables) {
            std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
            for (const auto& fc : g_floorCollectables) {
                if (WasCollectableConsumed(fc.id)) continue;
                Vector3 wp = tileCenter(fc.mapPoint);
                float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < bestDist) {
                    bestDist = dist; bestKind = TargetKind::Collectable;
                    bestPos = wp; bestPt = fc.mapPoint;
                    bestCollectId = fc.id;
                }
            }
        }

        if (exitUnlocked && g_targetFloor &&
            (bestKind == TargetKind::None || g_phase == Phase::SeekExit)) {
            std::lock_guard<std::mutex> lk(g_exitPortalLocationsMutex);
            float exitBest = FLT_MAX;
            Vector3 exitPos = {}; Vector2i exitPt = {};
            for (const auto& pt : g_exitPortalLocations) {
                Vector3 wp = tileCenter(pt);
                float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < exitBest) { exitBest = dist; exitPos = wp; exitPt = pt; }
            }
            if (exitBest < FLT_MAX &&
                (bestKind == TargetKind::None || g_phase == Phase::SeekExit)) {
                bestDist = exitBest; bestKind = TargetKind::Exit;
                bestPos = exitPos; bestPt = exitPt;
            }
        }

        if (g_priorityClosest && bestKind != TargetKind::Key) {
            struct Cand { TargetKind kind; Vector3 pos; Vector2i pt; int id; float strDist; };
            std::vector<Cand> cands;
            cands.reserve(32);

            if (g_targetEnemies && g_phase == Phase::Active) {
                std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
                for (void* inst : g_AIInstances) {
                    if (!inst || IsBadReadPtr(inst, sizeof(void*))) continue;
                    AIBase* base = *(AIBase**)((uintptr_t)inst + g_off_AIBase);
                    if (!base || IsBadReadPtr(base, sizeof(AIBase))) continue;
                    if (base->health <= (int)g_minMonsterHp) continue;
                    if (IsProjectileEnemyType((int)base->enemyType)) continue;
                    Vector3 ep = *(Vector3*)((uintptr_t)inst + g_off_AITempPosition);
                    float dx = ep.x - playerPos.x, dy = ep.y - playerPos.y;
                    Vector2i mp{};
                    if (oConvertWorldPointToMapPoint && g_WorldController)
                        mp = oConvertWorldPointToMapPoint(g_WorldController, ep);
                    cands.push_back({ TargetKind::Enemy, ep, mp, base->id,
                                      sqrtf(dx*dx + dy*dy) });
                }
            }
            if (g_targetTreasure && g_phase == Phase::Active) {
                std::lock_guard<std::mutex> lk(g_treasureLocationsMutex);
                for (const auto& tr : g_treasureLocations) {
                    if (WasRecentlyOpened(tr.mapPoint)) continue;
                    Vector3 wp = tileCenter(tr.mapPoint);
                    float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                    cands.push_back({ TargetKind::Treasure, wp, tr.mapPoint, -1,
                                      sqrtf(dx*dx + dy*dy) });
                }
            }
            if (g_targetGifts) {
                std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);
                for (const auto& pt : g_giftBoxLocations) {
                    if (WasRecentlyOpened(pt)) continue;
                    Vector3 wp = tileCenter(pt);
                    float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                    cands.push_back({ TargetKind::Gift, wp, pt, -1,
                                      sqrtf(dx*dx + dy*dy) });
                }
            }
            if (g_targetCollectables) {
                std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
                for (const auto& fc : g_floorCollectables) {
                    if (WasCollectableConsumed(fc.id)) continue;
                    Vector3 wp = tileCenter(fc.mapPoint);
                    float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                    cands.push_back({ TargetKind::Collectable, wp, fc.mapPoint, fc.id,
                                      sqrtf(dx*dx + dy*dy) });
                }
            }

            std::sort(cands.begin(), cands.end(),
                [](const Cand& a, const Cand& b){ return a.strDist < b.strDist; });
            if (cands.size() > 5) cands.resize(5);

            void* worldObj = (g_off_WCWorld && g_WorldController)
                ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld) : nullptr;
            if (worldObj) {
                Vector2i startTile = PathRenderer::WorldToTile(
                    playerPos.x, playerPos.y + tileWLocal * 0.5f, tileWLocal);
                int     bestPathLen = INT_MAX;
                int     bestIdx     = -1;
                for (size_t i = 0; i < cands.size(); ++i) {
                    Vector2i goal = PathRenderer::WorldToTile(
                        cands[i].pos.x, cands[i].pos.y, tileWLocal);
                    PathRenderer::PathResult r = PathRenderer::BuildPath(
                        g_WorldController, worldObj, startTile, goal, g_pathOpts);
                    if (r.status != PathRenderer::PathStatus::Success) continue;
                    int len = (int)r.tiles.size();
                    if (len < bestPathLen) {
                        bestPathLen = len;
                        bestIdx = (int)i;
                    }
                }
                if (bestIdx >= 0) {
                    bestKind = cands[bestIdx].kind;
                    bestPos  = cands[bestIdx].pos;
                    bestPt   = cands[bestIdx].pt;
                    bestID   = cands[bestIdx].id;
                    if (bestKind == TargetKind::Collectable)
                        bestCollectId = cands[bestIdx].id;
                }
            }
        }

        if (bestKind == TargetKind::None) return false;

        g_targetKind    = bestKind;
        g_targetWorldPos = bestPos;
        g_targetMapPt   = bestPt;
        g_targetEnemyID = bestID;
        g_targetCollectId = (bestKind == TargetKind::Collectable) ? bestCollectId : -1;
        g_hasTarget     = true;

        const char* kindStr = "?";
        switch (bestKind) {
        case TargetKind::Enemy:       kindStr = "Enemy";       break;
        case TargetKind::Treasure:    kindStr = "Treasure";    break;
        case TargetKind::Gift:        kindStr = "Gift";        break;
        case TargetKind::Exit:        kindStr = "Exit";        break;
        case TargetKind::Key:         kindStr = "Key";         break;
        case TargetKind::Collectable: kindStr = "Collectable"; break;
        default: break;
        }
        Diagnostics::StatPick(kindStr);
        Diagnostics::Ev("target_picked", {
            {"kind", kindStr},
            {"target", { bestPt.x, bestPt.y }},
            {"enemyId", bestID}
        });
        return true;
    }

    inline bool ValidateTarget() {
        if (!g_hasTarget) return false;

        if (g_targetKind == TargetKind::Enemy) {
            std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
            bool foundAlive = false;
            bool foundDead  = false;
            for (void* inst : g_AIInstances) {
                if (!inst || IsBadReadPtr(inst, sizeof(void*))) continue;
                AIBase* base = *(AIBase**)((uintptr_t)inst + g_off_AIBase);
                if (!base || IsBadReadPtr(base, sizeof(AIBase))) continue;
                if (base->id != g_targetEnemyID) continue;
                if (base->health > (int)g_minMonsterHp) {
                    foundAlive = true;
                } else {
                    foundDead = true;
                }
                break;
            }
            if (foundAlive) return true;

            if (foundDead) g_kills++;
            g_hasTarget = false; g_targetKind = TargetKind::None;
            return false;
        }

        if (g_targetKind == TargetKind::Treasure) {
            std::lock_guard<std::mutex> lk(g_treasureLocationsMutex);
            for (const auto& tr : g_treasureLocations)
                if (tr.mapPoint.x == g_targetMapPt.x && tr.mapPoint.y == g_targetMapPt.y)
                    return true;
            g_hasTarget = false; g_targetKind = TargetKind::None;
            return false;
        }

        if (g_targetKind == TargetKind::Gift) {
            std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);
            for (const auto& pt : g_giftBoxLocations)
                if (pt.x == g_targetMapPt.x && pt.y == g_targetMapPt.y)
                    return true;
            g_hasTarget = false; g_targetKind = TargetKind::None;
            return false;
        }

        if (g_targetKind == TargetKind::Key) {
            std::lock_guard<std::mutex> lk(g_netherKeyLocationsMutex);
            for (const auto& pt : g_netherKeyLocations)
                if (pt.x == g_targetMapPt.x && pt.y == g_targetMapPt.y)
                    return true;
            g_hasTarget = false; g_targetKind = TargetKind::None;
            return false;
        }

        if (g_targetKind == TargetKind::Collectable) {
            std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
            for (const auto& fc : g_floorCollectables)
                if (fc.id == g_targetCollectId) return true;
            g_hasTarget = false; g_targetKind = TargetKind::None;
            return false;
        }

        return g_targetFloor;
    }

    inline void Tick(float dt) {
        if (!g_enabled || !g_IsInWorld || !g_LocalPlayer) {

            if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                PathFinder_ClearInputs();
            return;
        }

        GodModeEnabled = true;
        g_antiTrap     = true;
        AntiBounce     = true;
        AntiDarkness   = true;

        if (g_antiSleep) SetAntiSleepFields();

        DeepLoopTick();
        RegularLoopTick();

        if (std::chrono::steady_clock::now() < g_resumeAfter) return;
        if (!IsInNether()) return;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return;

        ExtendedPickup::Tick();

        if (g_antiAfk && g_LocalPlayer && g_off_Player_jumpButtonDown > 0) {
            static auto s_lastAfk = std::chrono::steady_clock::now();
            auto secs = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - s_lastAfk).count();
            if (secs >= 30) {
                bool* jp = (bool*)((char*)g_LocalPlayer + g_off_Player_jumpButtonDown);
                if (!IsBadWritePtr(jp, sizeof(bool))) *jp = true;
                s_lastAfk = std::chrono::steady_clock::now();
            }
        }

        g_runtime += dt;
        g_hitTimer += dt;

        void*   lt   = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!lt) return;
        Vector3 here = PathRenderer::oGetPosition(lt);

        if (!ValidateTarget()) {
            if (!PickTarget()) { StopDriving(); return; }
            g_curStopRange   = Jitter(g_stopRange,   g_stopJitterPct);
            g_curHitInterval = Jitter(g_hitInterval, g_hitJitterPct);
            if (g_idlePauseMaxMs > 0) {
                int lo = g_idlePauseMinMs, hi = g_idlePauseMaxMs;
                if (hi < lo) hi = lo;
                int span = hi - lo + 1;
                int ms = lo + (int)(RngUnit() * span);
                g_resumeAfter = std::chrono::steady_clock::now()
                              + std::chrono::milliseconds(ms);
                StopDriving();
                return;
            }
        }

        if (g_targetKind == TargetKind::Enemy) {
            std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
            for (void* inst : g_AIInstances) {
                if (!inst || IsBadReadPtr(inst, sizeof(void*))) continue;
                AIBase* base = *(AIBase**)((uintptr_t)inst + g_off_AIBase);
                if (!base || IsBadReadPtr(base, sizeof(AIBase))) continue;
                if (base->id != g_targetEnemyID) continue;
                Vector3 ep = *(Vector3*)((uintptr_t)inst + g_off_AITempPosition);
                g_targetWorldPos = ep;
                if (oConvertWorldPointToMapPoint && g_WorldController)
                    g_targetMapPt = oConvertWorldPointToMapPoint(g_WorldController, ep);
                break;
            }
        }

        float dx   = g_targetWorldPos.x - here.x;
        float dy   = g_targetWorldPos.y - here.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (g_targetKind == TargetKind::Enemy &&
            dist <= g_maxHitRange &&
            g_hitTimer >= g_curHitInterval)
        {
            g_hitTimer = 0.f;
            g_curHitInterval = Jitter(g_hitInterval, g_hitJitterPct);
            TryHitNearestAI(g_maxHitRange);
        }

        if (dist <= g_curStopRange) {
            if (g_targetKind == TargetKind::Gift) {

                ResetCollectState(g_targetMapPt);
                if (!GiftBoxStillPresent(g_targetMapPt)) {
                    MarkOpened(g_targetMapPt);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    g_collects++;
                    return;
                }
                auto nowT = std::chrono::steady_clock::now();
                if (g_collectRetries == 0) {

                    SendOpenGiftBox(g_targetMapPt);
                    g_collectFirstSent   = nowT;
                    g_lastCollectAttempt = nowT;
                    g_collectRetries     = 1;
                    return;
                }
                auto sinceFirst = std::chrono::duration_cast<std::chrono::milliseconds>(
                    nowT - g_collectFirstSent).count();
                if (sinceFirst >= g_giftHardCapMs) {

                    MarkOpened(g_targetMapPt);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    g_collects++;
                    return;
                }

                auto sinceLast = std::chrono::duration_cast<std::chrono::milliseconds>(
                    nowT - g_lastCollectAttempt).count();
                if (g_collectRetries == 1 && sinceLast >= g_giftResendMs) {
                    SendOpenGiftBox(g_targetMapPt);
                    g_lastCollectAttempt = nowT;
                    g_collectRetries     = 2;
                }
                return;
            }
            if (g_targetKind == TargetKind::Collectable) {

                static auto s_holdStart = std::chrono::steady_clock::now() - std::chrono::seconds(10);
                static int s_holdId = -1;
                if (s_holdId != g_targetCollectId) {
                    s_holdId    = g_targetCollectId;
                    s_holdStart = std::chrono::steady_clock::now();
                }
                bool stillInScanner = false;
                if (g_targetCollectId >= 0) {
                    std::lock_guard<std::mutex> lkS(g_floorCollectablesMutex);
                    for (const auto& fc : g_floorCollectables)
                        if (fc.id == g_targetCollectId) { stillInScanner = true; break; }
                }
                if (!stillInScanner) {
                    if (g_targetCollectId >= 0) MarkCollectableConsumed(g_targetCollectId);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    g_collects++;
                    return;
                }
                auto held = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - s_holdStart).count();
                if (held >= 3000) {
                    if (g_targetCollectId >= 0) MarkCollectableConsumed(g_targetCollectId);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    return;
                }

                PathFinder_PulseJumpDown();
                return;
            }
            if (g_targetKind == TargetKind::Key) {

                static auto s_keyHoldStart = std::chrono::steady_clock::now() - std::chrono::seconds(10);
                static Vector2i s_keyHoldTile = { -9999, -9999 };
                if (s_keyHoldTile.x != g_targetMapPt.x || s_keyHoldTile.y != g_targetMapPt.y) {
                    s_keyHoldTile  = g_targetMapPt;
                    s_keyHoldStart = std::chrono::steady_clock::now();
                }
                bool stillInScanner = false;
                {
                    std::lock_guard<std::mutex> lk(g_netherKeyLocationsMutex);
                    for (const auto& p : g_netherKeyLocations) {
                        if (p.x == g_targetMapPt.x && p.y == g_targetMapPt.y) {
                            stillInScanner = true; break;
                        }
                    }
                }
                if (!stillInScanner) {

                    MarkOpened(g_targetMapPt);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    g_collects++;
                    return;
                }
                auto held = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - s_keyHoldStart).count();
                if (held >= 4000) {

                    MarkOpened(g_targetMapPt);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    return;
                }

                PathFinder_PulseJumpDown();
                return;
            }
            if (g_targetKind == TargetKind::Treasure) {

                ResetCollectState(g_targetMapPt);
                if (!TreasureStillPresent(g_targetMapPt)) {
                    MarkOpened(g_targetMapPt);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    g_collects++;
                    return;
                }
                if (g_hitTimer >= g_hitInterval) {
                    g_hitTimer = 0.f;
                    SendHitBlock(g_targetMapPt);
                    g_collectRetries++;
                    if (g_collectRetries >= g_collectMaxRetries) {
                        MarkOpened(g_targetMapPt);
                        g_hasTarget = false; g_targetKind = TargetKind::None;
                    }
                }
                return;
            }
            if (g_targetKind == TargetKind::Exit) {

                if (g_hitTimer >= g_hitInterval) {
                    g_hitTimer = 0.f;
                    SendNetherExit(g_targetMapPt);
                }
                return;
            }
        }

        if (!g_autoMove) { StopDriving(); return; }
        if (!oSetPosition) return;
        if (!PathRenderer::g_originCached) return;

        {
            long long key = ((long long)g_targetMapPt.x << 20)
                          ^  (long long)(g_targetMapPt.y & 0xFFFFF)
                          ^ ((long long)g_targetKind << 42);
            auto nowS = std::chrono::steady_clock::now();
            if (key != g_stallKey) {
                g_stallKey = key; g_stallAnchor = dist; g_stallSince = nowS;
            } else if (dist + STALL_PROGRESS_EPS < g_stallAnchor) {
                g_stallAnchor = dist; g_stallSince = nowS;
            } else if (std::chrono::duration_cast<std::chrono::milliseconds>(
                           nowS - g_stallSince).count() >= STALL_TIMEOUT_MS) {
                switch (g_targetKind) {
                case TargetKind::Collectable:
                    if (g_targetCollectId >= 0) MarkCollectableConsumed(g_targetCollectId);
                    break;
                case TargetKind::Gift:
                case TargetKind::Treasure:
                case TargetKind::Key:
                    MarkOpened(g_targetMapPt);
                    break;
                default: break;
                }
                g_hasTarget = false; g_targetKind = TargetKind::None;
                g_stallKey = 0; g_stallAnchor = FLT_MAX;
                StopDriving();
                return;
            }
        }

        const float tileW    = (PathRenderer::g_tileSize.x > 0.001f)
                                 ? PathRenderer::g_tileSize.x : 0.32f;
        const float bodyOff  = tileW * g_bodyOffsetFrac;

        Vector3 hereBody = { here.x, here.y + bodyOff, here.z };

        Vector2i startTile = PathRenderer::WorldToTile(
            here.x, here.y + tileW * 0.5f, tileW);
        Vector2i goalTile  = PathRenderer::WorldToTile(
            g_targetWorldPos.x, g_targetWorldPos.y, tileW);

        auto now = std::chrono::steady_clock::now();
        long long sinceLastMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_pathLastBuild).count();
        bool needRebuild = (goalTile.x != g_pathLastGoal.x || goalTile.y != g_pathLastGoal.y)
                        || (sinceLastMs > 500);

        if (needRebuild) {
            void* worldObj = g_off_WCWorld
                ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld)
                : nullptr;

            PathRenderer::PathResult res = PathRenderer::BuildPath(
                g_WorldController, worldObj, startTile, goalTile, g_pathOpts);

            if (res.status != PathRenderer::PathStatus::Success) {
                Vector2i bestAlt = goalTile;
                int      bestAltScore = INT_MAX;
                PathRenderer::PathResult bestAltRes = res;
                for (int dy2 = -2; dy2 <= 2; ++dy2) {
                    for (int dx2 = -2; dx2 <= 2; ++dx2) {
                        if (dx2 == 0 && dy2 == 0) continue;
                        if (dx2*dx2 + dy2*dy2 > 4) continue;
                        Vector2i alt{ goalTile.x + dx2, goalTile.y + dy2 };
                        PathRenderer::PathResult ar = PathRenderer::BuildPath(
                            g_WorldController, worldObj, startTile, alt, g_pathOpts);
                        if (ar.status != PathRenderer::PathStatus::Success) continue;

                        int score = dx2*dx2 + dy2*dy2;
                        if (score < bestAltScore) {
                            bestAltScore = score;
                            bestAlt = alt;
                            bestAltRes = std::move(ar);
                            if (score == 1) break;
                        }
                    }
                    if (bestAltScore == 1) break;
                }
                if (bestAltScore < INT_MAX) {
                    res = std::move(bestAltRes);
                    goalTile = bestAlt;
                }
            }

            bool fresh = (res.status == PathRenderer::PathStatus::Success);

            if (!fresh) {
                switch (g_targetKind) {
                case TargetKind::Collectable:
                    if (g_targetCollectId >= 0)
                        MarkCollectableConsumed(g_targetCollectId);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    break;
                case TargetKind::Gift:
                case TargetKind::Treasure:
                case TargetKind::Key:
                    MarkOpened(g_targetMapPt);
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    break;
                case TargetKind::Exit:
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    break;
                default:
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                    break;
                }
                StopDriving();
            }

            {
                std::lock_guard<std::mutex> lk(g_pathMutex);
                g_pathTiles    = std::move(res.tiles);
                g_pathOffsets  = std::move(res.offsets);
                g_pathStatus   = res.status;
                g_pathIdx      = 0;
                g_pathLastGoal = goalTile;
                g_pathLastBuild = now;
            }

            if (fresh && g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                PathFinder_PulseJumpDown();
        }

        std::lock_guard<std::mutex> lk(g_pathMutex);
        if (g_pathStatus != PathRenderer::PathStatus::Success || g_pathTiles.empty()) {
            StopDriving();
            return;
        }

        while (g_pathIdx < (int)g_pathTiles.size() - 1) {
            Vector3 wpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
            float wdx = wpc.x - hereBody.x, wdy = wpc.y - hereBody.y;
            if (sqrtf(wdx*wdx + wdy*wdy) <= g_pfStopRange)
                ++g_pathIdx;
            else
                break;
        }
        if (g_pathIdx >= (int)g_pathTiles.size()) { StopDriving(); return; }

        Vector3 wpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
        float wdx = wpc.x - hereBody.x;
        float wdy = wpc.y - hereBody.y;
        float wdist = sqrtf(wdx*wdx + wdy*wdy);

        if (g_moveMethod == PathRenderer::MoveMethod::InputDrive) {

            const float DEAD = 0.04f;
            bool wantLeft  = (wdx < -DEAD);
            bool wantRight = (wdx >  DEAD);
            bool wantJump  = (wdy >  DEAD);
            PathFinder_WriteInputs(wantLeft, wantRight, wantJump);

            if (wdist >= 0.001f && oSetVelocity) {
                float speed = g_moveSpeed * g_flightScale;
                float vx = (wdx / wdist) * speed;
                float vy = (wdy / wdist) * speed;
                PathRenderer::SafeSetVelocity(g_LocalPlayer,{ vx, vy, 0.f });
            }
        } else if (g_moveMethod == PathRenderer::MoveMethod::Portal) {

            PathRenderer::TryPortalStep(g_pathTiles, g_pathIdx,
                                        g_portalIntervalMs, g_lastPortalSec);
        } else if (g_moveMethod == PathRenderer::MoveMethod::Teleport) {

            PathRenderer::TpApplyAutoNether();
            double nowSec = (double)ImGui::GetTime();
            double dtTick = (double)g_portalIntervalMs / 1000.0;
            if ((nowSec - g_lastPortalSec) < dtTick) {
                if (PathRenderer::g_tp_zeroVelocity && oSetVelocity)
                    PathRenderer::SafeSetVelocity(g_LocalPlayer,{ 0.f, 0.f, 0.f });
            } else {
                g_lastPortalSec = nowSec;

                int last = (int)g_pathTiles.size() - 1;
                if (last < 0) return;
                if (g_pathIdx > last) g_pathIdx = last;
                if (g_pathIdx < 0)    g_pathIdx = 0;

                Vector3 jumpWpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
                Vector3 newFeet = { jumpWpc.x, jumpWpc.y - bodyOff, jumpWpc.z };
                oSetPosition(lt, newFeet);
                if (PathRenderer::oGetKukouriCamInst && PathRenderer::oForceUpdatePos) {
                    void* cam = PathRenderer::oGetKukouriCamInst();
                    if (cam) PathRenderer::oForceUpdatePos(cam);
                }
                if (PathRenderer::g_tp_zeroVelocity && oSetVelocity)
                    PathRenderer::SafeSetVelocity(g_LocalPlayer,{ 0.f, 0.f, 0.f });
                if (PathRenderer::g_tp_throttleMovePackets &&
                    PathRenderer::oSendPlayerPosition0_orig)
                {
                    PathRenderer::g_tp_allowOnePacket.store(true);
                    PathRenderer::SehInvokeSendPos0(PathRenderer::oSendPlayerPosition0_orig);
                    PathRenderer::g_tp_allowOnePacket.store(false);
                }
            }
        }
    }

    inline void DrawPathOverlay(ImDrawList* dl, Vector3 here,
                                Vector2 playerS, bool pOk, float ortho)
    {
        const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                              ? PathRenderer::g_tileSize.x : 0.32f;
        const float t = (float)ImGui::GetTime();

        struct PtS { ImVec2 p; bool ok; float dist; };
        std::vector<PtS> pts;
        bool pathFailed = false;
        {
            std::lock_guard<std::mutex> lk(g_pathMutex);
            pathFailed = (g_pathStatus != PathRenderer::PathStatus::Success);
            pts.reserve(g_pathTiles.size() + 1);
            ImVec2 prev(playerS.x, playerS.y);
            bool prevOk = pOk;
            float acc = 0.f;
            pts.push_back({ prev, prevOk, 0.f });
            for (int i = g_pathIdx; i < (int)g_pathTiles.size(); ++i) {
                Vector3 wpc = PathWaypointWorld(i, here.z, tileW);
                Vector2 cS; bool oC = PathRenderer::NavWorldToScreen(wpc, cS);
                ImVec2 cur(cS.x, cS.y);
                if (oC && prevOk) {
                    float ddx = cur.x - prev.x, ddy = cur.y - prev.y;
                    acc += sqrtf(ddx*ddx + ddy*ddy);
                }
                pts.push_back({ cur, oC, acc });
                prev = cur; prevOk = oC;
            }
        }
        float totalLen = pts.empty() ? 0.f : pts.back().dist;

        auto hsv = [](float h, float s, float v, float a) -> ImU32 {
            float r=0,g=0,b=0;
            ImGui::ColorConvertHSVtoRGB(fmodf(h, 1.f), s, v, r, g, b);
            return IM_COL32((int)(r*255), (int)(g*255), (int)(b*255), (int)(a*255));
        };

        if (g_pathGlow && !pathFailed && pts.size() > 1) {
            for (int pass = 0; pass < 3; ++pass) {
                float thick = 10.f - pass * 3.f;
                int   alpha = 30 + pass * 25;
                for (size_t i = 1; i < pts.size(); ++i) {
                    if (!pts[i-1].ok || !pts[i].ok) continue;
                    float hue = pts[i].dist / fmaxf(totalLen, 1.f) * 0.55f
                                + t * 0.05f + 0.10f;
                    dl->AddLine(pts[i-1].p, pts[i].p,
                                hsv(hue, 0.85f, 1.f, alpha / 255.f), thick);
                }
            }
        }

        if (pts.size() > 1) {
            for (size_t i = 1; i < pts.size(); ++i) {
                if (!pts[i-1].ok || !pts[i].ok) continue;
                if (pathFailed) {
                    dl->AddLine(pts[i-1].p, pts[i].p,
                                IM_COL32(255, 70, 70, 240), 3.f);
                } else {
                    float hue = pts[i].dist / fmaxf(totalLen, 1.f) * 0.55f
                                + t * 0.05f + 0.10f;
                    dl->AddLine(pts[i-1].p, pts[i].p,
                                hsv(hue, 0.4f, 1.f, 0.9f), 2.f);
                }
            }
            for (size_t i = 1; i < pts.size(); ++i) {
                if (!pts[i].ok) continue;
                float hue = pts[i].dist / fmaxf(totalLen, 1.f) * 0.55f
                            + t * 0.05f + 0.10f;
                dl->AddCircleFilled(pts[i].p, 2.5f,
                    pathFailed ? IM_COL32(255, 90, 90, 220)
                               : hsv(hue, 0.3f, 1.f, 0.85f));
            }
        }

        if (g_pathFlow && !pathFailed && totalLen > 1.f) {
            const float spacing = 36.f;
            const float speed   = 90.f;
            int n = (int)(totalLen / spacing) + 1;
            if (n > 60) n = 60;
            float phase = fmodf(t * speed, spacing);
            size_t cursor = 1;
            for (int k = 0; k < n; ++k) {
                float d = k * spacing + phase;
                if (d > totalLen) break;
                while (cursor < pts.size() && pts[cursor].dist < d) ++cursor;
                if (cursor >= pts.size() || !pts[cursor].ok || !pts[cursor-1].ok) continue;
                float seg = pts[cursor].dist - pts[cursor-1].dist;
                float u = (seg > 0.001f) ? (d - pts[cursor-1].dist) / seg : 0.f;
                ImVec2 p(pts[cursor-1].p.x + (pts[cursor].p.x - pts[cursor-1].p.x) * u,
                         pts[cursor-1].p.y + (pts[cursor].p.y - pts[cursor-1].p.y) * u);
                float hue = d / fmaxf(totalLen, 1.f) * 0.55f + t * 0.15f + 0.05f;
                dl->AddCircleFilled(p, 5.f, hsv(hue, 0.5f, 1.f, 0.25f));
                dl->AddCircleFilled(p, 2.5f, hsv(hue, 0.7f, 1.f, 1.f));
            }
        }

        if (pathFailed) {
            Vector2 lblS;
            if (PathRenderer::NavWorldToScreen(g_targetWorldPos, lblS)) {
                char buf[64];
                snprintf(buf, sizeof(buf), "X  %s",
                    PathRenderer::PathStatusText(g_pathStatus));
                dl->AddText(ImVec2(lblS.x + 11.f, lblS.y - 19.f),
                            IM_COL32(0, 0, 0, 220), buf);
                dl->AddText(ImVec2(lblS.x + 10.f, lblS.y - 20.f),
                            IM_COL32(255, 90, 90, 255), buf);
            }
        }
    }

    inline void HandleResetIfPending() {
        if (!g_autoNetherResetPending) return;
        g_resumeAfter = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        g_collectMp     = { -9999, -9999 };
        g_collectRetries = 0;
        g_hasTarget     = false;
        g_targetKind    = TargetKind::None;
        g_targetEnemyID = -1;
        g_phase         = Phase::Active;
        {
            std::lock_guard<std::mutex> lk(g_pathMutex);
            g_pathTiles.clear();
            g_pathOffsets.clear();
            g_pathStatus = PathRenderer::PathStatus::Success;
            g_pathIdx    = 0;
            g_pathLastGoal = { -9999, -9999 };
        }
        {
            std::lock_guard<std::mutex> lk(g_recentlyOpenedMutex);
            g_recentlyOpened.clear();
        }
        PathFinder_ClearInputs();

        PathFinder_EndNav();
        g_autoNetherResetPending = false;

        if (g_enabled) {
            Diagnostics::EndRun("world_transition");
            Diagnostics::StartRun("AutoNether", GetCurrentWorldName());
            Diagnostics::Ev("world_entered", { {"world", GetCurrentWorldName()} });
        }
    }

    inline void Render() {

        if (g_isTransitioning.load(std::memory_order_acquire)) return;

        HandleResetIfPending();

        if (!g_enabled) { g_hadFault = false; g_faultStreak = 0; return; }
        if (g_hadFault) return;

        auto nowf = std::chrono::steady_clock::now();
        if (g_faultStreak > 0 && nowf < g_faultRetryAt) return;

        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        if (dt <= 0.f || dt > 0.25f) dt = 0.016f;

        __try {
            Tick(dt);
            g_faultStreak = 0;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {

            unsigned long code = GetExceptionCode();
            ++g_faultStreak;
            g_faultRetryAt = nowf + std::chrono::milliseconds(500);
            if (g_faultStreak >= 12) {
                g_hadFault = true;
                sprintf_s(g_faultMsg, "[AutoNether] repeated tick fault 0x%08lX — paused (toggle Enable to retry).", code);
            } else {
                sprintf_s(g_faultMsg, "[AutoNether] transient fault 0x%08lX — auto-retrying.", code);
            }
        }

        if (g_isTransitioning.load(std::memory_order_acquire)) return;
        if (!g_showPath || !g_hasTarget || !g_IsInWorld || !g_LocalPlayer) return;
        if (!g_WorldController) return;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return;

        void*   lt   = nullptr;
        Vector3 here = {};
        bool readOk = false;
        __try {
            lt = PathRenderer::oGetTransform(g_LocalPlayer);
            if (lt) {
                here = PathRenderer::oGetPosition(lt);
                readOk = true;
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) { return; }
        if (!readOk) return;

        Vector2 playerS, targetS;
        bool pOk = PathRenderer::NavWorldToScreen(here, playerS);
        bool tOk = PathRenderer::NavWorldToScreen(g_targetWorldPos, targetS);

        ImDrawList* dl    = ImGui::GetForegroundDrawList();
        float       ortho = g_cachedCamOrtho.load(std::memory_order_relaxed);

        DrawPathOverlay(dl, here, playerS, pOk, ortho);

        if (tOk) {
            ImU32 ringCol = IM_COL32(255, 200, 80, 255);
            if (g_targetKind == TargetKind::Enemy)       ringCol = IM_COL32(255, 80,  80,  255);
            if (g_targetKind == TargetKind::Exit)        ringCol = IM_COL32(80,  255, 80,  255);
            if (g_targetKind == TargetKind::Gift)        ringCol = IM_COL32(255, 255, 80,  255);
            if (g_targetKind == TargetKind::Collectable) ringCol = IM_COL32(120, 255, 180, 255);
            dl->AddCircle(ImVec2(targetS.x, targetS.y), 10.f, ringCol, 16, 2.f);
            if (ortho > 0.f) {
                float pxPerUnit = ImGui::GetIO().DisplaySize.y / (ortho * 2.f);
                dl->AddCircle(ImVec2(targetS.x, targetS.y),
                              g_stopRange * pxPerUnit,
                              IM_COL32(255, 200, 80, 60), 24, 1.f);
            }
        }

        if (pOk)
            dl->AddCircleFilled(ImVec2(playerS.x, playerS.y),
                                4.f, IM_COL32(255, 140, 60, 220));

        if (pOk && ortho > 0.f && g_hasTarget) {
            Vector2 dir = PathRenderer::GreedyDir({ here.x, here.y },
                                                   { g_targetWorldPos.x, g_targetWorldPos.y });
            if (dir.x != 0.f || dir.y != 0.f) {
                float scale    = ImGui::GetIO().DisplaySize.y / (ortho * 2.f);
                float arrowLen = 0.4f * scale;
                ImVec2 tip = {
                    playerS.x + dir.x * arrowLen,
                    playerS.y - dir.y * arrowLen
                };
                dl->AddLine(ImVec2(playerS.x, playerS.y), tip,
                            IM_COL32(255, 255, 255, 200), 3.f);
                float hl = arrowLen * 0.35f;
                ImVec2 left = { tip.x - dir.x * hl + dir.y * hl * 0.5f,
                                tip.y + dir.y * hl + dir.x * hl * 0.5f };
                ImVec2 rght = { tip.x - dir.x * hl - dir.y * hl * 0.5f,
                                tip.y + dir.y * hl - dir.x * hl * 0.5f };
                dl->AddTriangleFilled(tip, left, rght, IM_COL32(255, 255, 255, 160));
            }
        }
    }

    inline void RenderTab() {
        ImGui::TextColored(ImVec4(1.0f, 0.55f, 0.2f, 1.0f), "Auto Nether");
        ImGui::TextWrapped("Only runs in NETHERWORLD or DEEPNETHER. (toggle gate below)");

        bool inNether = IsInNetherWorld();
        ImVec4 statCol = inNether ? ImVec4(0.3f, 1.0f, 0.4f, 1.0f)
                                  : ImVec4(1.0f, 0.4f, 0.3f, 1.0f);
        ImGui::TextColored(statCol, inNether
            ? "Status: IN nether world"
            : "Status: NOT in nether world");
        ImGui::Spacing();

        if (g_hadFault) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", g_faultMsg);
            if (ImGui::Button("Acknowledge & re-enable")) {
                g_hadFault = false; g_enabled = false;
                memset(g_faultMsg, 0, sizeof(g_faultMsg));
            }
            ImGui::Spacing();
        }

        ImGui::Checkbox("Auto-Nether##AN", &g_enabled);
        ImGui::BeginDisabled(!g_enabled);
        {
            ImGui::Indent(12.f);

            ImGui::Checkbox("Auto move##AN", &g_autoMove);
            ImGui::SameLine();
            ImGui::TextDisabled("\"90 degree path finding\"");

            g_netherOnly = true;
            ImGui::Checkbox("Draw route on screen##AN", &g_showPath);

            ImGui::SliderFloat("##flight_scale_AN", &g_flightScale, 0.5f, 5.0f, "%.2fx flight speed");
            ImGui::SameLine(); ImGui::TextDisabled("Nether flight speed multiplier");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextUnformatted("Targets");

            ImGui::Checkbox("AI enemies##AN",             &g_targetEnemies);
            ImGui::Checkbox("Treasure chests (1400)##AN", &g_targetTreasure);
            ImGui::Checkbox("Gift boxes##AN",             &g_targetGifts);
            ImGui::Checkbox("Floor collectables##AN",     &g_targetCollectables);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextWrapped("Item drops on the floor (enemy gem drops, mining loot, etc). Walk-to-collect — game auto-pickups on overlap.");
                ImGui::EndTooltip();
            }
            ImGui::Checkbox("Exit portal##AN",            &g_targetFloor);
            ImGui::Spacing();
            ImGui::Checkbox("Anti-Sleep##AN", &g_antiSleep);
            ImGui::SameLine();
            ImGui::Checkbox("Anti-AFK##AN",   &g_antiAfk);
            ImGui::TextDisabled("Locks: GodMode, Anti-Trap, AntiBounce, AntiDarkness (always ON while bot runs)");
            ImGui::Spacing();
            ImGui::Checkbox("Prioritise closest##AN",     &g_priorityClosest);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextWrapped("Pick target by SHORTEST A* PATH instead of strict hierarchy "
                    "(enemy>treasure>gift>exit). Top 5 nearest by line-of-sight are path-evaluated; "
                    "shortest path wins. Keys still mandatory & top priority.");
                ImGui::EndTooltip();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextUnformatted("Combat And Movement");

            ImGui::SliderFloat("Melee cooldown (s)##AN",      &g_hitInterval,  0.05f, 2.0f,  "%.2f s");
            ImGui::SliderFloat("Max melee range (tiles)##AN", &g_maxHitRange,  0.5f,  8.0f,  "%.1f");
            ImGui::SliderFloat("Stop distance (world)##AN",   &g_stopRange,    0.05f, 2.0f,  "%.2f");
            ImGui::SliderFloat("Min enemy max HP##AN",        &g_minMonsterHp, 0.0f,  500.0f,"%.0f");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.7f, 0.85f, 1.f, 1.f), "Pathfinder (AutoNether)");
            ImGui::TextWrapped("Independent of standalone pathfinder. Same controls, separate values.");

            ImGui::TextColored(ImVec4(0.8f,1.0f,0.8f,1.f),
                g_moveMethod == PathRenderer::MoveMethod::Portal
                    ? "Movement Method: Teleport (Portal)"
                    : "Movement Method: Smooth");
            ImGui::Spacing();

            ImGui::SliderFloat("Move Speed##AN-PF",      &g_moveSpeed,      0.5f, 20.0f, "%.1f u/s");
            if (g_moveSpeed > 4.5f) {
                ImGui::TextColored(ImVec4(1.f, 0.6f, 0.3f, 1.f),
                    "WARNING: > 1.5x base — may trigger fly-detect kick");
            }
            ImGui::SliderFloat("Stop Range##AN-PF",      &g_pfStopRange,    0.0f,  1.0f, "%.2f u");

            ImGui::Spacing();

            ImGui::Checkbox("Airborne Check##AN-PF",     &g_pathOpts.airborneCheck);
            ImGui::BeginDisabled(!g_pathOpts.airborneCheck);
            ImGui::SliderInt("Max Airborne Tiles##AN-PF", &g_pathOpts.maxAirborne, 1, 20);
            ImGui::EndDisabled();
            ImGui::Spacing();

            ImGui::SliderFloat("Body-Centre Offset##AN-PF", &g_bodyOffsetFrac, 0.0f, 1.0f, "%.2f x tile");
            ImGui::Spacing();

            ImGui::Checkbox("Curve Around Solid Corners##AN-PF", &g_pathOpts.chamfer);
            ImGui::BeginDisabled(!g_pathOpts.chamfer);
            ImGui::SliderFloat("Chamfer Amount##AN-PF", &g_pathOpts.chamferAmount, 0.0f, 0.45f, "%.2f x tile");
            ImGui::EndDisabled();
            ImGui::Spacing();

            ImGui::Checkbox("Walk on Insta-Kill Tiles##AN-PF", &g_pathOpts.walkInstakill);
            ImGui::Spacing();

            ImGui::Checkbox("Glow Halo##AN-PF",      &g_pathGlow);
            ImGui::Checkbox("Flow Particles##AN-PF", &g_pathFlow);
            ImGui::Checkbox("Pulsing Target##AN-PF", &g_pathPulse);

            ImGui::Spacing();
            ImGui::Separator();

            const char* phaseName = (g_phase == Phase::Active) ? "Active" : "Seek exit";
            ImGui::Text("Phase: %s", phaseName);
            ImGui::SameLine();
            if (ImGui::Button("Reset phase##AN")) ResetPhase();

            ImGui::Spacing();
            int rt = (int)g_runtime;
            ImGui::Text("Runtime: %02d:%02d:%02d", rt / 3600, (rt % 3600) / 60, rt % 60);
            ImGui::Text("Kills: %d   Collects: %d", g_kills, g_collects);

            ImGui::Spacing();
            if (g_hasTarget) {
                const char* kindStr = "Unknown";
                switch (g_targetKind) {
                case TargetKind::Enemy:    kindStr = "Enemy";    break;
                case TargetKind::Treasure: kindStr = "Treasure"; break;
                case TargetKind::Gift:     kindStr = "Gift box"; break;
                case TargetKind::Exit:     kindStr = "Exit";     break;
                case TargetKind::Key:      kindStr = "Nether Key"; break;
                case TargetKind::Collectable: kindStr = "Collectable"; break;
                default: break;
                }
                ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.4f, 1.0f),
                    "Target: %s @ (%.2f, %.2f)", kindStr,
                    g_targetWorldPos.x, g_targetWorldPos.y);
            } else {
                ImGui::TextDisabled("No target");
            }

            ImGui::Unindent(12.f);
        }
        ImGui::EndDisabled();

        if (g_netherOnly && g_enabled &&
            g_currentWeatherType != World::WeatherType::DeepNether) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f),
                "Join a Nether world to use this.");
        }
    }

}

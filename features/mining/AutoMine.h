#pragma once

#include "main.h"
#include "PathFinding.h"
#include "BankBot.h"
#include "GemSeller.h"
#include "Diagnostics.h"
#include "MineDebug.h"
#include "imgui/imgui.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include <unordered_set>

namespace AutoMine {

    inline bool _PlayerNearExitTile_3x3();

    extern std::atomic<bool> g_gmExitFiredThisSession;

    extern std::atomic<bool> g_doShowAnimationSeen;

    inline std::chrono::steady_clock::time_point g_exitBlockedSince =
        std::chrono::steady_clock::now();
    inline bool g_exitBlockedValid = false;
    static constexpr int EXIT_FORCE_AFTER_MS = 12000;

    extern std::atomic<long long> g_lastGMExitSentMs;
    inline long long MsSinceGMExit();

    namespace AutoLoop {
        inline void Tick();
        inline void RenderUI();
        inline void StopWalk();
    }

    inline void ScheduleMineExitWarp(int msFromNow, const char* reason);

    inline void NoteGMExitSent();
    inline void DiagInspectIncoming(const nlohmann::json& m);

    inline bool  g_enabled        = false;
    inline std::chrono::steady_clock::time_point g_resumeAfter =
        std::chrono::steady_clock::now();
    inline bool  g_autoMove       = true;
    inline bool  g_showPath       = true;
    inline float g_hitInterval    = 0.16f;
    inline float g_stopRange      = 0.18f;
    inline float g_flightScale    = 0.9f;
    inline bool  g_mineOnly       = true;

    inline bool  g_targetGemstones = true;
    inline bool  g_targetNuggets   = true;
    inline bool  g_targetCrystals  = true;
    inline bool  g_targetPots      = true;
    inline bool  g_targetCollect   = true;

    inline bool  g_targetAI         = true;
    inline bool  g_targetAINearbyOnly = true;
    inline bool  g_targetExit      = true;

    inline bool  g_antiSleep = true;
    inline bool  g_antiAfk   = true;

    inline bool  g_requirePickaxe = false;

    inline bool g_autoRepairPickaxe = true;
    inline int  g_repairThreshold   = 50;
    static constexpr int REPAIR_KIT_BLOCK_TYPE = 4161;
    static constexpr int CONSUMABLE_ITEM_TYPE  = 7;
    static constexpr int WEAPON_ITEM_TYPE      = 5;
    static constexpr int REPAIR_COOLDOWN_MS    = 1000;

    inline std::atomic<bool> g_repairPending{ false };

    inline std::atomic<int>  g_preRepairKitCount{ -1 };

    inline std::chrono::steady_clock::time_point g_lastRepairAttempt =
        std::chrono::steady_clock::now() - std::chrono::seconds(60);

    inline int  ReadPickaxeDurability(int* outBT);
    inline void PickaxeRepairTick();

    typedef int(__fastcall* tGetTopArmBlockType)(void* player);
    typedef int(__fastcall* tGetItemDurability)(void* playerData, uint64_t inventoryKey);
    typedef bool(__fastcall* tHasItemAmount)(void* playerData, int blockType, uint8_t itemType, int16_t amount);
    typedef void* (__fastcall* tGetInventoryData)(void* playerData, uint64_t inventoryKey);
    typedef int   (__fastcall* tGetBlockDurability)(int blockType);

    typedef float (__fastcall* tGetItemDurabilityProgress)(void* playerData, int blockType);
    inline tGetTopArmBlockType        oGetTopArmBlockType_AM       = nullptr;
    inline tGetItemDurability         oGetItemDurability_AM        = nullptr;
    inline tHasItemAmount             oHasItemAmount_AM            = nullptr;
    inline tGetInventoryData          oGetInventoryData_AM         = nullptr;
    inline tGetBlockDurability        oGetBlockDurability_AM       = nullptr;
    inline tGetItemDurabilityProgress oGetItemDurabilityProgress_AM = nullptr;
    inline bool                       g_repairResolved             = false;

    inline std::map<int,int> g_maxDurCache;

    inline void ResolveRepairFunctions() {
        if (g_repairResolved) return;
        oGetTopArmBlockType_AM = (tGetTopArmBlockType)IL2CPP::Class::Utils::GetMethodPointer(
            "Player", "GetTopArmBlockType", 0);
        oGetItemDurability_AM = (tGetItemDurability)IL2CPP::Class::Utils::GetMethodPointer(
            "PlayerData", "GetItemDurability", 1);
        oHasItemAmount_AM = (tHasItemAmount)IL2CPP::Class::Utils::GetMethodPointer(
            "PlayerData", "HasItemAmountInInventory", 3);
        oGetInventoryData_AM = (tGetInventoryData)IL2CPP::Class::Utils::GetMethodPointer(
            "PlayerData", "GetInventoryData", 1);
        oGetBlockDurability_AM = (tGetBlockDurability)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "GetBlockDurability", 1);
        oGetItemDurabilityProgress_AM = (tGetItemDurabilityProgress)IL2CPP::Class::Utils::GetMethodPointer(
            "PlayerData", "GetItemDurabilityProgress", 1);
        g_repairResolved = true;
    }

    inline int PackInventoryKey(int blockType, int itemType) {
        return (itemType << 24) | (blockType & 0xFFFFFF);
    }

    inline uint64_t PackInventoryKeyStruct(int blockType, int itemType) {

        return ((uint64_t)(uint32_t)blockType) | (((uint64_t)(uint32_t)(itemType & 0xFF)) << 32);
    }

    inline bool IsPickaxeBlockType(int bt) {

        return bt == 53 || (bt >= 4087 && bt <= 4093) || bt == 4195;
    }

    inline bool  g_crystalDetour            = false;
    inline float g_lightCrystalThreshold    = 0.0f;
    inline bool  g_crystalBreakIfClose      = true;
    inline float g_crystalBreakIfCloseThreshold = 0.95f;

    typedef float(__fastcall* tGetLightCrystalTimeLeft)(void* worldLighting);
    typedef float(__fastcall* tGetMaxLightCrystalTime)(void* worldLighting);
    inline tGetLightCrystalTimeLeft oGetLightTime_AM = nullptr;
    inline tGetMaxLightCrystalTime  oGetMaxLightTime_AM = nullptr;
    inline bool                     g_lightResolved   = false;

    inline void ResolveLightingFunctions() {
        if (g_lightResolved) return;
        oGetLightTime_AM = (tGetLightCrystalTimeLeft)IL2CPP::Class::Utils::GetMethodPointer(
            "WorldLighting", "GetLightCrystalTimeLeft", 0);
        oGetMaxLightTime_AM = (tGetMaxLightCrystalTime)IL2CPP::Class::Utils::GetMethodPointer(
            "WorldLighting", "GetMaxLightCrystalTime", 0);
        g_lightResolved = true;
    }

    inline void* GetWorldLightingInstance() {
        __try {
            void* rootUI = IL2CPP::Class::Utils::GetStaticField("ControllerHelper", "rootUI");
            if (!rootUI) return nullptr;

            return *(void**)((uintptr_t)rootUI + 0x118);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    }

    inline float GetLightCrystalFraction() {
        ResolveLightingFunctions();
        if (!oGetLightTime_AM || !oGetMaxLightTime_AM) return -1.f;
        void* wl = GetWorldLightingInstance();
        if (!wl) return -1.f;
        __try {
            float cur = oGetLightTime_AM(wl);
            float mx  = oGetMaxLightTime_AM(wl);
            if (mx <= 0.001f) return -1.f;
            float frac = cur / mx;
            if (frac < 0.f)  frac = 0.f;
            if (frac > 1.f)  frac = 1.f;
            return frac;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return -1.f; }
    }

    inline bool ShouldTargetCrystals() {
        if (!g_crystalDetour) return false;
        float frac = GetLightCrystalFraction();
        if (frac < 0.f) return true;
        return frac <= g_lightCrystalThreshold;
    }

    inline PathRenderer::MoveMethod g_moveMethod = PathRenderer::MoveMethod::InputDrive;

    inline float  g_portalIntervalMs = 0.f;
    inline double g_lastPortalSec    = 0.0;
    inline float g_moveSpeed      = 2.5f;
    inline float g_pfStopRange    = 0.18f;
    inline float g_bodyOffsetFrac = 0.4f;

    inline PathRenderer::MiningWeights g_weights = {
            25,
          60,
           true
    };

    inline bool  g_pathGlow = true;
    inline bool  g_pathFlow = true;

    inline std::vector<Vector2i>      g_pathTiles;
    inline std::vector<Vector2>       g_pathOffsets;
    inline PathRenderer::PathStatus   g_pathStatus = PathRenderer::PathStatus::Success;
    inline int                        g_pathIdx    = 0;
    inline std::mutex                 g_pathMutex;
    inline Vector2i                   g_pathLastGoal = { -9999, -9999 };
    inline std::chrono::steady_clock::time_point g_pathLastBuild =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);

    inline std::chrono::steady_clock::time_point g_pathIdxLastChange =
        std::chrono::steady_clock::now();
    inline int g_pathIdxLastSeen = -1;
    static constexpr int PATH_STALL_REBUILD_MS = 1000;

    struct RestTileKey {
        int x, y;
        bool operator==(const RestTileKey& o) const { return x == o.x && y == o.y; }
    };
    struct RestTileHash {
        size_t operator()(const RestTileKey& k) const {
            return ((size_t)(uint32_t)k.x << 32) | (uint32_t)k.y;
        }
    };
    inline std::unordered_set<long long> g_pendingRestTiles;
    inline std::unordered_set<long long> g_consumedRestTiles;
    inline std::mutex g_restTilesMutex;
    inline std::chrono::steady_clock::time_point g_restHoldUntil =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);
    static constexpr int REST_HOLD_MS = 500;
    inline bool RestHoldActive() {
        return std::chrono::steady_clock::now() < g_restHoldUntil;
    }
    static inline long long PackTile(int x, int y) {
        return ((long long)(unsigned int)x << 32) | (unsigned int)y;
    }
    inline void ClearRestTilesAll() {
        std::lock_guard<std::mutex> lk(g_restTilesMutex);
        g_pendingRestTiles.clear();
        g_consumedRestTiles.clear();
    }

    static constexpr int OPENED_TTL_SEC = 30;
    struct OpenedTile { Vector2i mp; std::chrono::steady_clock::time_point at; };
    inline std::vector<OpenedTile> g_recentlyOpened;
    inline std::mutex              g_recentlyOpenedMutex;

    inline void EvictExpiredOpened_locked() {
        auto now = std::chrono::steady_clock::now();
        g_recentlyOpened.erase(
            std::remove_if(g_recentlyOpened.begin(), g_recentlyOpened.end(),
                [&](const OpenedTile& t) {
                    return std::chrono::duration_cast<std::chrono::seconds>(
                               now - t.at).count() > OPENED_TTL_SEC;
                }),
            g_recentlyOpened.end());
    }

    inline bool WasRecentlyOpened(Vector2i mp) {
        std::lock_guard<std::mutex> lk(g_recentlyOpenedMutex);
        EvictExpiredOpened_locked();
        for (const auto& t : g_recentlyOpened)
            if (t.mp.x == mp.x && t.mp.y == mp.y) return true;
        return false;
    }
    inline void MarkOpened(Vector2i mp) {
        std::lock_guard<std::mutex> lk(g_recentlyOpenedMutex);
        g_recentlyOpened.push_back({ mp, std::chrono::steady_clock::now() });
    }
    inline void FlushOpened() {
        std::lock_guard<std::mutex> lk(g_recentlyOpenedMutex);
        g_recentlyOpened.clear();
    }

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

    inline int ReadBlockType(Vector2i mp);

    inline bool  g_combatEnabled    = true;
    inline int   g_combatTileRange  = 2;
    inline float g_combatCadence    = 0.33f;
    inline float g_combatTimer      = 0.f;

    inline bool IsDeepMaggotInvincible(void* inst, AIBase* base) {
        if (!inst || !base) return false;
        if ((int)base->enemyType != AI_DeepMaggot) return false;
        if (oDeepMaggotCanBeHit) {
            __try {
                return !oDeepMaggotCanBeHit(inst);
            } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        }

        if (IsBadReadPtr(inst, sizeof(void*))) return false;
        __try {
            float secs    = *(float*)((uintptr_t)inst + 0x1D8);
            float counter = *(float*)((uintptr_t)inst + 0x1DC);
            if (!(secs > 0.f && secs < 10.f)) return false;
            if (counter > 0.0f) return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        return false;
    }

    inline bool IsEnemyHittableForBot(void* inst, AIBase* base) {
        if (!inst || !base) return false;
        if (base->health <= 0) return false;
        if (IsProjectileEnemyType((int)base->enemyType)) return false;

        if ((int)base->enemyType == (int)Gassy) return false;
        if (IsDeepMaggotInvincible(inst, base)) return false;

        if (oCanPlayerHitAIEnemy && !oCanPlayerHitAIEnemy(base->id)) return false;
        return true;
    }

    inline bool EnemyInHitRange(int rangeTiles) {
        if (!g_LocalPlayer || !g_WorldController) return false;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return false;
        if (!oConvertWorldPointToMapPoint) return false;
        void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!lt) return false;
        Vector3 p = PathRenderer::oGetPosition(lt);

        const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                              ? PathRenderer::g_tileSize.x : 0.32f;
        Vector2i playerTile = PathRenderer::WorldToTile(
            p.x, p.y + tileW * 0.5f, tileW);
        std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
        for (void* inst : g_AIInstances) {
            if (!inst || IsBadReadPtr(inst, sizeof(void*))) continue;
            AIBase* base = *(AIBase**)((uintptr_t)inst + g_off_AIBase);
            if (!base || IsBadReadPtr(base, sizeof(AIBase))) continue;
            if (!IsEnemyHittableForBot(inst, base)) continue;
            Vector3 ep = *(Vector3*)((uintptr_t)inst + g_off_AITempPosition);
            Vector2i et = oConvertWorldPointToMapPoint(g_WorldController, ep);
            int dx = et.x - playerTile.x; if (dx < 0) dx = -dx;
            int dy = et.y - playerTile.y; if (dy < 0) dy = -dy;
            int cheb = (dx > dy) ? dx : dy;
            if (cheb <= rangeTiles) return true;
        }
        return false;
    }

    inline bool MineHitNearest(int rangeTiles) {
        if (!g_LocalPlayer || !g_WorldController) return false;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return false;
        if (!oConvertWorldPointToMapPoint || !oSendHitAIEnemyMessage)     return false;

        void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!lt) return false;
        Vector3 p = PathRenderer::oGetPosition(lt);
        const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                              ? PathRenderer::g_tileSize.x : 0.32f;
        Vector2i playerTile = PathRenderer::WorldToTile(
            p.x, p.y + tileW * 0.5f, tileW);

        void* bestInst = nullptr;
        int   bestCheb = rangeTiles + 1;
        std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
        for (void* inst : g_AIInstances) {
            if (!inst || IsBadReadPtr(inst, sizeof(void*))) continue;
            AIBase* base = *(AIBase**)((uintptr_t)inst + g_off_AIBase);
            if (!base || IsBadReadPtr(base, sizeof(AIBase))) continue;
            if (!IsEnemyHittableForBot(inst, base)) continue;
            Vector3 ep = *(Vector3*)((uintptr_t)inst + g_off_AITempPosition);
            Vector2i et = oConvertWorldPointToMapPoint(g_WorldController, ep);
            int dx = et.x - playerTile.x; if (dx < 0) dx = -dx;
            int dy = et.y - playerTile.y; if (dy < 0) dy = -dy;
            int cheb = (dx > dy) ? dx : dy;
            if (cheb <= rangeTiles && cheb < bestCheb) {
                bestCheb = cheb;
                bestInst = inst;
            }
        }
        if (!bestInst) return false;
        AIBase* tb = *(AIBase**)((uintptr_t)bestInst + g_off_AIBase);
        if (!tb || IsBadReadPtr(tb, sizeof(AIBase))) return false;
        Vector3 tp = *(Vector3*)((uintptr_t)bestInst + g_off_AITempPosition);
        Vector2i tmp = oConvertWorldPointToMapPoint(g_WorldController, tp);
        oSendHitAIEnemyMessage(tmp, tb->id, -1);
        return true;
    }

    struct BreakSetup {
        bool      valid;
        Vector2i  standTile;
        Vector2i  targetTile;
        int       reach;
        bool      vertical;
    };

    inline bool IsAirTile(int bt) {
        return bt == 0 || PathRenderer::PF_IsHardcodedPassable(bt);
    }
    inline bool IsFloorTile(int bt) {
        if (bt <= 0) return false;
        if (PathRenderer::PF_IsHardcodedPassable(bt)) return false;
        if (PathRenderer::PF_IsHardcodedSolid(bt)) return true;
        if (PathRenderer::oCfgIsInstakill && PathRenderer::oCfgIsInstakill((World::BlockType)bt)) return false;
        if (PathRenderer::oCfgDoesCollide && PathRenderer::oCfgDoesCollide((World::BlockType)bt)) return true;
        return false;
    }

    static constexpr int HIT_REACH = 2;

    inline BreakSetup ComputeBreakSetup(Vector2i target, Vector2i playerTile) {
        BreakSetup best{ false, target, target, 0, false };
        int bestDist = INT_MAX;

        auto consider = [&](Vector2i stand, bool vertical, int reach) {
            int dx = stand.x - playerTile.x, dy = stand.y - playerTile.y;
            int dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
            if (dist < bestDist) {
                bestDist = dist;
                best.valid = true;
                best.standTile = stand;
                best.targetTile = target;
                best.reach = reach;
                best.vertical = vertical;
            }
        };

        auto tryRanged = [&](Vector2i stand, bool axisX, bool vertical, int reach) {
            int sBt = ReadBlockType(stand);
            int floorBt = ReadBlockType({ stand.x, stand.y - 1 });
            if (!IsAirTile(sBt) || !IsFloorTile(floorBt)) return;
            bool clear = true;
            if (axisX) {
                int xa = (stand.x < target.x ? stand.x : target.x) + 1;
                int xb = (stand.x > target.x ? stand.x : target.x);
                for (int xi = xa; xi < xb; ++xi)
                    if (!IsAirTile(ReadBlockType({ xi, target.y }))) { clear = false; break; }
            } else {
                int ya = (stand.y < target.y ? stand.y : target.y) + 1;
                int yb = (stand.y > target.y ? stand.y : target.y);
                for (int yi = ya; yi < yb; ++yi)
                    if (!IsAirTile(ReadBlockType({ target.x, yi }))) { clear = false; break; }
            }
            if (!clear) return;
            consider(stand, vertical, reach);
        };

        for (int dy = 1; dy <= HIT_REACH; ++dy)
            tryRanged({ target.x, target.y - dy }, false, true, dy);

        for (int dy = 1; dy <= HIT_REACH; ++dy)
            tryRanged({ target.x, target.y + dy }, false, true, dy);

        for (int dx = 1; dx <= HIT_REACH; ++dx)
            tryRanged({ target.x - dx, target.y }, true, false, dx);

        for (int dx = 1; dx <= HIT_REACH; ++dx)
            tryRanged({ target.x + dx, target.y }, true, false, dx);

        if (best.valid) return best;

        const Vector2i neighbours[4] = {
            {  0, -1 }, { -1,  0 }, {  1,  0 }, {  0,  1 }
        };
        for (auto& n : neighbours) {
            Vector2i s{ target.x + n.x, target.y + n.y };
            int floorBt = ReadBlockType({ s.x, s.y - 1 });
            if (!IsFloorTile(floorBt)) continue;
            consider(s, n.y != 0, 1);
        }
        return best;
    }

    inline int MiningCost(int bt) {
        if (bt < 0) return -1;
        if (bt == 0) return 1;
        if (PathRenderer::PF_IsHardcodedPassable(bt)) return 1;

        if (IsNugget((World::BlockType)bt)) return 1;
        if (PathRenderer::PF_IsHardcodedSolid(bt))    return 7;
        if (PathRenderer::oCfgIsInstakill &&
            PathRenderer::oCfgIsInstakill((World::BlockType)bt)) return -1;
        return GetMiningResistance(bt);
    }

    enum class Phase { Active, SeekExit };
    inline Phase g_phase = Phase::Active;

    enum class TargetKind { None, Gemstone, Nugget, Crystal, Pot, Collectable, Exit, AIEnemy };

    inline int g_targetAIEnemyId = -1;
    inline TargetKind g_targetKind     = TargetKind::None;
    inline Vector3    g_targetWorldPos = {};
    inline Vector2i   g_targetMapPt    = {};
    inline Vector2i   g_targetStandPt  = {};
    inline bool       g_breakFromRange = false;
    inline int        g_targetCollectId = -1;
    inline bool       g_hasTarget      = false;

    inline float g_runtime  = 0.f;
    inline int   g_breaks   = 0;
    inline int   g_collects = 0;

    inline float g_hitTimer = 0.f;

    inline std::chrono::steady_clock::time_point g_lastUpPulse =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);
    static constexpr int UP_PULSE_MS = 180;

    inline Vector2i g_breakingTile = { -9999, -9999 };
    inline std::chrono::steady_clock::time_point g_breakingSince =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);
    inline std::chrono::steady_clock::time_point g_breakingLastHit =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);
    inline int g_breakingHits = 0;
    inline int g_breakingBlockType = 0;
    inline int g_breakingStartGems = -1;

    inline Vector2i g_lingerTile = { -9999, -9999 };
    inline std::chrono::steady_clock::time_point g_lingerStarted =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);

    static constexpr int LINGER_IDLE_MS = 500;
    static constexpr int LINGER_MAX_MS  = 3000;
    inline std::chrono::steady_clock::time_point g_lingerArrivedAt =
        std::chrono::steady_clock::now() - std::chrono::seconds(60);
    inline bool g_lingerArrivedFlag = false;

    static constexpr int POST_BREAK_HOLD_MS     = 250;
    static constexpr int POST_BREAK_HOLD_GEM_MS = 300;

    inline std::chrono::steady_clock::time_point g_postBreakUntil =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);
    inline bool PostBreakHoldActive() {
        return std::chrono::steady_clock::now() < g_postBreakUntil;
    }
    inline void StartPostBreakHold() {
        g_postBreakUntil = std::chrono::steady_clock::now()
                            + std::chrono::milliseconds(POST_BREAK_HOLD_MS);
    }
    inline void StartPostBreakHoldFor(int durationMs) {
        g_postBreakUntil = std::chrono::steady_clock::now()
                            + std::chrono::milliseconds(durationMs);
    }

    inline bool LingerActive() { return g_lingerTile.x != -9999; }
    inline void EndLinger() {
        g_lingerTile = { -9999, -9999 };
        g_lingerArrivedFlag = false;
    }
    inline void StartLinger(Vector2i mp) {
        g_lingerTile        = mp;
        g_lingerStarted     = std::chrono::steady_clock::now();
        g_lingerArrivedFlag = false;
    }
    inline void ClearBreaking() {
        g_breakingTile = { -9999, -9999 };
        g_breakingHits = 0;
        g_breakingBlockType = 0;
        g_breakingStartGems = -1;
    }
    inline void NoteBreakHit(Vector2i mp) {
        auto now = std::chrono::steady_clock::now();
        if (g_breakingTile.x != mp.x || g_breakingTile.y != mp.y) {
            g_breakingTile  = mp;
            g_breakingSince = now;
            g_breakingHits  = 0;

            g_breakingBlockType = ReadBlockType(mp);

            g_breakingStartGems = -1;
        }
        g_breakingLastHit = now;
        g_breakingHits++;
    }

    inline bool  g_hadFault = false;
    inline char  g_faultMsg[128] = {};

    inline void QueueSetVelocity(float vx, float vy, float vz) {
        ::g_pfPendingVelX.store(vx, std::memory_order_relaxed);
        ::g_pfPendingVelY.store(vy, std::memory_order_relaxed);
        ::g_pfPendingVelZ.store(vz, std::memory_order_relaxed);
        ::g_pfPendingVelValid.store(true, std::memory_order_release);
    }

    inline bool IsInMineWorld() {
        if (!g_WorldController || !g_off_WCWorld) return false;
        __try {
            void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
            if (!worldObj) return false;
            int layout = *(int*)((uintptr_t)worldObj + 0x110);
            if (layout == World::WorldLayoutType::LayoutGeneratedMine) return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        return false;
    }
    inline bool IsInMine() {
        if (!g_mineOnly) return true;
        return IsInMineWorld();
    }

    struct MineProgress {
        int enemiesKilled;
        int totalEnemies;
        int gemstonesBroken;
        int totalGemstones;
        int nuggetsCollected;
        int totalNuggets;
    };

    inline bool ReadMineProgress(MineProgress& out) {
        if (!g_LocalPlayer) return false;
        __try {
            void* pd = *(void**)((uintptr_t)g_LocalPlayer + 0x58);
            if (!pd) return false;
            void* wcsd = *(void**)((uintptr_t)pd + 0x28);
            if (!wcsd) return false;
            out.enemiesKilled    = *(int*)((uintptr_t)wcsd + 0x18);
            out.totalEnemies     = *(int*)((uintptr_t)wcsd + 0x1C);
            out.gemstonesBroken  = *(int*)((uintptr_t)wcsd + 0x20);
            out.totalGemstones   = *(int*)((uintptr_t)wcsd + 0x24);
            out.nuggetsCollected = *(int*)((uintptr_t)wcsd + 0x28);
            out.totalNuggets     = *(int*)((uintptr_t)wcsd + 0x2C);

            if (out.totalGemstones < 0 || out.totalGemstones > 1000) return false;
            if (out.totalNuggets   < 0 || out.totalNuggets   > 1000) return false;
            if (out.gemstonesBroken < 0 || out.gemstonesBroken > out.totalGemstones + 10) return false;
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    }

    inline int  g_breakStartGemsBroken = -1;
    inline Vector2i g_breakStartTile   = { -9999, -9999 };

    static constexpr int GEM_STUCK_HITS = 20;

    inline bool HasPickaxeEquipped() {

        return true;
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

    inline void SendHitBlock(Vector2i mp) {
        if (!oSendHitBlockMessage) return;

        if (g_repairPending.load(std::memory_order_acquire)) return;

        if (g_autoRepairPickaxe) {
            int pickBT = -1;
            int dur = ReadPickaxeDurability(&pickBT);
            if (dur >= 0 && dur <= g_repairThreshold) {
                PickaxeRepairTick();
                return;
            }
        }
        __try { oSendHitBlockMessage(mp, GetDateTimeTicksGlobal(), false); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    inline bool g_preMineHB              = false;
    inline int  g_preMineRadius          = 2;
    inline int  g_preMineIncludeHardSolid = 0;
    inline bool g_preMineDebugLog        = true;

    inline uint32_t g_preMineRotation = 0;

    inline Vector2i PickPreMineTarget(Vector2i playerTile, uint32_t rotation,
                                      int& outCandidateCount, int& outPathSize) {
        const Vector2i kNone{ -9999, -9999 };
        outCandidateCount = 0;
        outPathSize = (int)g_pathTiles.size();
        if (!g_WorldController || !g_off_WCWorld) return kNone;
        void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (!worldObj) return kNone;
        PathRenderer::PF_BlockLayerInfo bli = PathRenderer::PF_ReadBlockLayer(worldObj);
        if (!bli.ok) return kNone;
        if (g_pathTiles.empty()) return kNone;

        std::unordered_set<long long> pathSet;
        pathSet.reserve(g_pathTiles.size());
        for (const auto& t : g_pathTiles) {
            pathSet.insert(((long long)t.x << 32) | (uint32_t)t.y);
        }

        struct Cand { Vector2i mp; int cheb; int cost; };
        std::vector<Cand> tiles;
        tiles.reserve(16);
        const int R = (g_preMineRadius < 1) ? 1 : (g_preMineRadius > 4 ? 4 : g_preMineRadius);
        for (int dy = -R; dy <= R; ++dy) {
            for (int dx = -R; dx <= R; ++dx) {
                int tx = playerTile.x + dx;
                int ty = playerTile.y + dy;
                long long key = ((long long)tx << 32) | (uint32_t)ty;
                if (pathSet.find(key) == pathSet.end()) continue;
                int bt = PathRenderer::PF_BlockTypeAtTile(bli.layer, tx, ty, bli.w, bli.h);
                if (bt <= 0) continue;
                if (PathRenderer::PF_IsHardcodedPassable(bt)) continue;
                if (IsNugget((World::BlockType)bt)) continue;
                if (PathRenderer::oCfgIsInstakill &&
                    PathRenderer::oCfgIsInstakill((World::BlockType)bt)) continue;
                bool hardSolid = PathRenderer::PF_IsHardcodedSolid(bt);
                if (hardSolid && !g_preMineIncludeHardSolid) continue;
                int adx = std::abs(dx), ady = std::abs(dy);
                int cheb = (adx > ady) ? adx : ady;
                int cost = hardSolid ? 7 : GetMiningResistance(bt);
                tiles.push_back({ Vector2i{tx, ty}, cheb, cost });
            }
        }
        outCandidateCount = (int)tiles.size();
        if (tiles.empty()) return kNone;

        std::sort(tiles.begin(), tiles.end(),
                  [](const Cand& a, const Cand& b) {
                      if (a.cheb != b.cheb) return a.cheb < b.cheb;
                      return a.cost < b.cost;
                  });
        int idx = (int)(rotation % (uint32_t)tiles.size());
        return tiles[idx].mp;
    }

    inline Vector2i HitBlockPreMineOrSingle(Vector2i target, Vector3 playerWorldPos, float tileW) {
        if (g_preMineHB) {
            Vector2i playerTile = PathRenderer::WorldToTile(
                playerWorldPos.x, playerWorldPos.y + tileW * 0.5f, tileW);
            int candCount = 0;
            int pathSize = 0;
            uint32_t rotation = g_preMineRotation;
            Vector2i pick = PickPreMineTarget(playerTile, rotation, candCount, pathSize);
            if (g_preMineDebugLog) {
                static uint32_t s_dbg = 0;
                ++s_dbg;

                if (s_dbg <= 30 || s_dbg % 30 == 0) {
                    std::cout << "[PreMine] tick=" << s_dbg
                              << " player=(" << playerTile.x << "," << playerTile.y << ")"
                              << " pathSize=" << pathSize
                              << " candidates=" << candCount
                              << " rot=" << rotation
                              << " pick=(" << pick.x << "," << pick.y << ")"
                              << " explicitTarget=(" << target.x << "," << target.y << ")"
                              << " enabled=" << (int)g_preMineHB
                              << std::endl;
                }
            }
            if (pick.x != -9999) {
                ++g_preMineRotation;
                SendHitBlock(pick);
                return pick;
            }

        }
        SendHitBlock(target);
        return target;
    }

    inline void SendGFLi() {}

    inline void ApplyLocalRepairMirror(int pickBT);

    inline void SendPickaxeRepairing(PlayerData_InventoryKey ik) {
        if (!oMiningPickaxeRepairing) return;
        ReplyDispatch::NoteSendMPR(ik.blockType);
        __try { oMiningPickaxeRepairing(ik); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return; }

    }

    inline void OnMPRReplyDelivered(int pickBT, const nlohmann::json& bson) {

        bool success = !bson.is_object() || bson.value("S", true);
        Diagnostics::Ev("mpr_reply_confirmed", {
            {"pickaxe", pickBT},
            {"success", success}
        });
        if (!success) return;
        std::cout << "[ReplyMirror] MPR confirmed pick=" << pickBT
                  << " — applying repair mirror\n";
        ApplyLocalRepairMirror(pickBT);

        g_repairPending.store(false, std::memory_order_release);
    }

    inline void* ReadPlayerData() {
        if (!g_LocalPlayer) return nullptr;
        __try {
            return *(void**)((uintptr_t)g_LocalPlayer + 0x58);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    }

    inline int ReadPickaxeDurability(int* outBT = nullptr) {
        ResolveRepairFunctions();
        if (!oGetTopArmBlockType_AM || !oGetItemDurability_AM) return -1;
        if (!g_LocalPlayer) return -1;
        void* pd = ReadPlayerData();
        if (!pd) return -1;
        int bt = -1;
        __try {
            bt = oGetTopArmBlockType_AM(g_LocalPlayer);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
        if (bt <= 0 || !IsPickaxeBlockType(bt)) return -1;
        if (outBT) *outBT = bt;
        __try {
            uint64_t ik = PackInventoryKeyStruct(bt, WEAPON_ITEM_TYPE);
            return oGetItemDurability_AM(pd, ik);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    }

    inline int EstimateRepairKitCount() {
        if (!g_LocalPlayer || !oGetCountByKey) return 0;
        void* pd = ReadPlayerData();
        if (!pd) return 0;
        PlayerData_InventoryKey k{ (World::BlockType)REPAIR_KIT_BLOCK_TYPE,
                                   (InventoryItemType)CONSUMABLE_ITEM_TYPE };
        short qty = 0;
        __try { qty = oGetCountByKey(pd, k); }
        __except (EXCEPTION_EXECUTE_HANDLER) { qty = 0; }
        return qty < 0 ? 0 : (int)qty;
    }

    inline bool HasRepairKit() {
        return EstimateRepairKitCount() > 0;
    }

    static inline void _Seh_RemoveKit(void* pd) {
        if (!oRemoveItemsFromInventory) return;
        PlayerData_InventoryKey kitKey{
            (World::BlockType)REPAIR_KIT_BLOCK_TYPE,
            (InventoryItemType)CONSUMABLE_ITEM_TYPE
        };
        __try { oRemoveItemsFromInventory(pd, kitKey, 1); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static inline int _Seh_GetMaxDurability(int pickBT) {
        if (!oGetBlockDurability_AM) return 0;
        __try { return oGetBlockDurability_AM(pickBT); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    }
    static inline int _Seh_GetCurrentDurability(void* pd, int pickBT) {
        if (!oGetItemDurability_AM || !pd) return 0;
        __try {
            uint64_t k = PackInventoryKeyStruct(pickBT, WEAPON_ITEM_TYPE);
            return oGetItemDurability_AM(pd, k);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    }
    static inline float _Seh_GetDurabilityProgress(void* pd, int pickBT) {
        if (!oGetItemDurabilityProgress_AM || !pd) return 0.f;
        __try { return oGetItemDurabilityProgress_AM(pd, pickBT); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return 0.f; }
    }

    inline int ComputeMaxDurability(void* pd, int pickBT) {

        int direct = _Seh_GetMaxDurability(pickBT);
        if (direct > 0) {
            g_maxDurCache[pickBT] = direct;
            return direct;
        }

        int cur = _Seh_GetCurrentDurability(pd, pickBT);
        float prog = _Seh_GetDurabilityProgress(pd, pickBT);
        if (cur > 0 && prog > 0.001f && prog < 0.999f) {
            int derived = (int)((float)cur / prog + 0.5f);
            if (derived > 0) {
                g_maxDurCache[pickBT] = derived;
                return derived;
            }
        }

        auto it = g_maxDurCache.find(pickBT);
        if (it != g_maxDurCache.end() && it->second > 0) {
            return it->second;
        }

        return 1000;
    }
    static inline void _Seh_SetWornDur(void* pd, int dur) {
        if (!oSetWornWeaponDurability) return;
        __try { oSetWornWeaponDurability(pd, dur); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static inline void* _Seh_GetInvData(void* pd, uint64_t k) {
        if (!oGetInventoryData_AM) return nullptr;
        __try { return oGetInventoryData_AM(pd, k); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    }
    static inline void _Seh_WriteDur(void* pickData, int maxDur) {
        if (!pickData) return;

        __try { *(int*)((uintptr_t)pickData + 0x18) = maxDur; }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static inline void _Seh_RefreshInv() {
        if (!oForceRefreshInventory || !g_InventoryControl) return;
        __try { oForceRefreshInventory(g_InventoryControl); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    inline void ApplyLocalRepairMirror(int pickBT) {
        if (!g_LocalPlayer) {
            std::cout << "[Repair] g_LocalPlayer null — mirror skipped.\n";
            return;
        }
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) {
            std::cout << "[Repair] PlayerData null — mirror skipped.\n";
            return;
        }

        _Seh_RemoveKit(pd);

        ResolveRepairFunctions();

        int maxDur = ComputeMaxDurability(pd, pickBT);

        if (maxDur <= 0) {
            std::cout << "[Repair] could not resolve max durability — skipped.\n";
            _Seh_RefreshInv();
            return;
        }

        bool setViaHelper = false;
        if (oSetWornWeaponDurability) {
            _Seh_SetWornDur(pd, maxDur);
            setViaHelper = true;
        }

        uint64_t pickKey = PackInventoryKeyStruct(pickBT, WEAPON_ITEM_TYPE);
        void* pickData = _Seh_GetInvData(pd, pickKey);
        if (pickData) {
            _Seh_WriteDur(pickData, maxDur);
        } else {
            std::cout << "[Repair] GetInventoryData(pickBT=" << pickBT
                      << ") returned null — direct durability write skipped.\n";
        }

        _Seh_RefreshInv();

        std::cout << "[Repair] pickBT=" << pickBT
                  << " durability restored to " << maxDur
                  << " (helper=" << (setViaHelper ? "yes" : "no")
                  << ", direct=" << (pickData ? "yes" : "no") << ")\n";
    }

    inline void DumpEquippedItemInfo() {
        ResolveRepairFunctions();
        std::cout << "[EquipInfo] ===== equipped hand item =====\n";
        std::cout << "[EquipInfo] fns topArm=" << (void*)oGetTopArmBlockType_AM
                  << " getDur=" << (void*)oGetItemDurability_AM
                  << " getProg=" << (void*)oGetItemDurabilityProgress_AM
                  << " getBlockDur=" << (void*)oGetBlockDurability_AM << "\n";
        if (!g_LocalPlayer) { std::cout << "[EquipInfo] g_LocalPlayer NULL\n"; return; }
        void* pd = ReadPlayerData();
        std::cout << "[EquipInfo] player=" << g_LocalPlayer << " playerData=" << pd << "\n";
        if (!oGetTopArmBlockType_AM) { std::cout << "[EquipInfo] GetTopArmBlockType UNRESOLVED\n"; return; }

        int  bt = -1;
        bool faulted = false;
        __try { bt = oGetTopArmBlockType_AM(g_LocalPlayer); }
        __except (EXCEPTION_EXECUTE_HANDLER) { faulted = true; }
        if (faulted) { std::cout << "[EquipInfo] GetTopArmBlockType FAULTED\n"; return; }

        const char* name = BlockName(bt);
        bool isPick = IsPickaxeBlockType(bt);
        std::cout << "[EquipInfo] topArmBlockType=" << bt
                  << " name='" << (name ? name : "?") << "'"
                  << " isPickaxe(gate 53,4087-4093,4195)=" << (int)isPick << "\n";

        if (!pd) { std::cout << "[EquipInfo] PlayerData NULL — cannot read durability\n"; return; }

        int  durW = -999; bool durWf = false;
        __try { durW = oGetItemDurability_AM ? oGetItemDurability_AM(pd, PackInventoryKeyStruct(bt, WEAPON_ITEM_TYPE)) : -998; }
        __except (EXCEPTION_EXECUTE_HANDLER) { durWf = true; }

        float prog = -1.f; bool progf = false;
        __try { prog = oGetItemDurabilityProgress_AM ? oGetItemDurabilityProgress_AM(pd, bt) : -1.f; }
        __except (EXCEPTION_EXECUTE_HANDLER) { progf = true; }

        int maxDur = ComputeMaxDurability(pd, bt);

        std::cout << "[EquipInfo] durability(WEAPON itemType=5)=" << durW << (durWf ? " [FAULTED]" : "")
                  << " progress=" << prog << (progf ? " [FAULTED]" : "")
                  << " computedMax=" << maxDur << "\n";
        std::cout << "[EquipInfo] repairThreshold=" << g_repairThreshold
                  << "  -> auto-gate wouldRepair="
                  << ((!durWf && durW >= 0 && isPick && durW <= g_repairThreshold) ? 1 : 0)
                  << " (needs isPickaxe=1 AND 0<=dur<=threshold)\n";
        std::cout << "[EquipInfo] ==============================\n";
    }

    inline void PickaxeRepairTick() {
        if (!g_autoRepairPickaxe) return;
        auto now = std::chrono::steady_clock::now();
        auto sinceMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_lastRepairAttempt).count();
        if (sinceMs < REPAIR_COOLDOWN_MS) return;

        int pickBT = -1;
        int dur = ReadPickaxeDurability(&pickBT);
        if (dur < 0)  return;
        if (dur > g_repairThreshold) {

            g_repairPending.store(false, std::memory_order_release);
            return;
        }
        if (!HasRepairKit()) {

            g_repairPending.store(false, std::memory_order_release);
            g_lastRepairAttempt = now;
            ShowNotification("Auto-Repair",
                "Pickaxe at/below repair threshold but NO repair kits in inventory. "
                "Mining paused — buy repair kits.", NotificationType::Warning, 2.5f);
            std::cout << "[Repair] no repair kit — packet suppressed, mining paused, user notified\n";
            return;
        }

        int preKit = EstimateRepairKitCount();
        g_preRepairKitCount.store(preKit, std::memory_order_release);
        g_repairPending.store(true, std::memory_order_release);

        g_lastRepairAttempt = now;

        SendPickaxeRepairing(PlayerData_InventoryKey{ pickBT, WEAPON_ITEM_TYPE });

        Diagnostics::StatRepair();
        Diagnostics::Ev("pickaxe_repair", {
            {"pickaxe",   pickBT},
            {"durBefore", dur},
            {"threshold", g_repairThreshold}
        });
    }

    inline int ReadBlockType(Vector2i mp) {
        if (!g_WorldController || !g_off_WCWorld) return -1;
        __try {
            void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
            if (!worldObj) return -1;
            void* layer = *(void**)((uintptr_t)worldObj + g_off_WorldBlockLayer);
            if (!layer || IsBadReadPtr(layer, 0x20)) return -1;
            int worldSizeX = *(int*)((uintptr_t)layer + 0x18);
            void** pCols = (void**)((uintptr_t)layer + 0x20);
            if (mp.x < 0 || mp.x >= worldSizeX) return -1;
            void* col = pCols[mp.x];
            if (!col) return -1;
            int worldSizeY = *(int*)((uintptr_t)col + 0x18);
            if (mp.y < 0 || mp.y >= worldSizeY) return -1;
            char* colData = (char*)((uintptr_t)col + 0x20);
            return *(int*)((uintptr_t)(colData + (mp.y * 0x28)));
        } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    }

    inline bool IsTilePassableLive(Vector2i mp) {
        int bt = ReadBlockType(mp);
        if (bt < 0) return false;
        if (bt == 0) return true;
        if (IsNugget((World::BlockType)bt)) return true;
        if (PathRenderer::PF_IsHardcodedSolid(bt)) return false;
        if (PathRenderer::PF_IsHardcodedPassable(bt)) return true;

        if (::Door::IsDoor(bt)) return true;
        if (PathRenderer::oCfgIsHatch   && PathRenderer::oCfgIsHatch((World::BlockType)bt))   return true;
        if (PathRenderer::oCfgIsPortal  && PathRenderer::oCfgIsPortal((World::BlockType)bt))  return true;
        if (PathRenderer::oCfgIsOneWay && PathRenderer::oCfgIsOneWay((World::BlockType)bt)) return true;
        if (PathRenderer::oCfgIsPlatform && PathRenderer::oCfgIsPlatform((World::BlockType)bt)) return true;
        if (PathRenderer::oCfgIsInstakill && PathRenderer::oCfgIsInstakill((World::BlockType)bt)) return false;
        if (PathRenderer::oCfgDoesCollide && !PathRenderer::oCfgDoesCollide((World::BlockType)bt)) return true;
        return false;
    }

    inline Vector3 PathWaypointWorld(int i, float wz, float tileW) {
        Vector2i t = g_pathTiles[i];
        Vector3  c = PathRenderer::TileToWorldCenter(t.x, t.y, wz, tileW);
        if (i >= 0 && i < (int)g_pathOffsets.size()) {
            c.x += g_pathOffsets[i].x * tileW;
            c.y += g_pathOffsets[i].y * tileW;
        }
        return c;
    }

    struct Cand {
        TargetKind kind;
        Vector2i   mp;
        int        collectId;
        Vector3    pos;
        float      strDist;
    };

    inline std::vector<Cand> GatherCandidates(Vector3 playerPos, float tileW) {
        std::vector<Cand> out;
        out.reserve(64);
        auto tileCenter = [&](Vector2i mp) {
            return PathRenderer::TileToWorldCenter(mp.x, mp.y, 0.f, tileW);
        };
        auto pushIfFresh = [&](TargetKind k, Vector2i mp, int collectId) {
            if (WasRecentlyOpened(mp)) return;
            Vector3 wp = tileCenter(mp);
            float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
            out.push_back({ k, mp, collectId, wp, sqrtf(dx*dx + dy*dy) });
        };

        if (g_targetGemstones) {
            std::lock_guard<std::mutex> lk(g_gemstoneLocationsMutex);
            for (const auto& g : g_gemstoneLocations) pushIfFresh(TargetKind::Gemstone, g.mapPoint, -1);
        }
        if (g_targetNuggets) {
            std::lock_guard<std::mutex> lk(g_nuggetLocationsMutex);
            for (const auto& g : g_nuggetLocations) pushIfFresh(TargetKind::Nugget, g.mapPoint, -1);
        }
        if (g_targetCrystals && ShouldTargetCrystals()) {
            std::lock_guard<std::mutex> lk(g_lightCrystalLocationsMutex);
            for (const auto& g : g_lightCrystalLocations) pushIfFresh(TargetKind::Crystal, g.mapPoint, -1);
        }
        if (g_targetPots) {
            std::lock_guard<std::mutex> lk(g_potLocationsMutex);
            for (const auto& g : g_potLocations) pushIfFresh(TargetKind::Pot, g.mapPoint, -1);
        }
        if (g_targetCollect) {
            std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
            for (const auto& fc : g_floorCollectables) {
                if (WasCollectableConsumed(fc.id)) continue;
                Vector3 wp = tileCenter(fc.mapPoint);
                float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                out.push_back({ TargetKind::Collectable, fc.mapPoint, fc.id, wp,
                                sqrtf(dx*dx + dy*dy) });
            }
        }

        if (g_targetAI && !g_targetAINearbyOnly &&
            g_WorldController && oConvertWorldPointToMapPoint) {
            std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
            for (void* inst : g_AIInstances) {
                if (!inst || IsBadReadPtr(inst, sizeof(void*))) continue;
                AIBase* base = *(AIBase**)((uintptr_t)inst + g_off_AIBase);
                if (!base || IsBadReadPtr(base, sizeof(AIBase))) continue;
                if (!IsEnemyHittableForBot(inst, base)) continue;
                Vector3 ep = *(Vector3*)((uintptr_t)inst + g_off_AITempPosition);
                Vector2i emp = oConvertWorldPointToMapPoint(g_WorldController, ep);
                float dx = ep.x - playerPos.x, dy = ep.y - playerPos.y;
                out.push_back({ TargetKind::AIEnemy, emp, base->id, ep,
                                sqrtf(dx*dx + dy*dy) });
            }
        }
        return out;
    }

    inline bool IsGemDropBlockType(int bt) {
        return bt >= 4012 && bt <= 4056;
    }

    inline bool PickTarget() {
        if (!g_WorldController || !g_LocalPlayer) return false;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return false;

        void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!lt) return false;
        Vector3 playerPos = PathRenderer::oGetPosition(lt);
        const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                              ? PathRenderer::g_tileSize.x : 0.32f;

        if (MineScheduler::IsSessionActive()) {
            MineScheduler::PopFrontIfComplete();

            MineScheduler::ReactiveSync(g_targetGemstones,
                                        g_targetNuggets,
                                        g_targetCollect);

            if (g_targetCrystals && g_crystalDetour) {
                Vector2i playerTile = PathRenderer::WorldToTile(
                    playerPos.x, playerPos.y + tileW * 0.5f, tileW);
                MineScheduler::ReactiveSyncCrystal(
                    true, GetLightCrystalFraction(),
                    g_lightCrystalThreshold, playerTile);
            }

            MineScheduler::Target front;
            if (MineScheduler::TryGetFront(front)) {
                Vector2i mp = front.mapPoint;
                Vector3 worldP = PathRenderer::TileToWorldCenter(
                    mp.x, mp.y, 0.f, tileW);

                if (front.kind == MineScheduler::TargetKind::GemOre) {

                    Vector2i startTile = PathRenderer::WorldToTile(
                        playerPos.x, playerPos.y + tileW * 0.5f, tileW);
                    BreakSetup bs = ComputeBreakSetup(mp, startTile);
                    if (!bs.valid) {

                        std::lock_guard<std::mutex> lk(MineScheduler::g_seqMutex);
                        if (!MineScheduler::g_sequence.empty())
                            MineScheduler::g_sequence.erase(MineScheduler::g_sequence.begin());
                        return false;
                    }
                    g_targetKind      = TargetKind::Gemstone;
                    g_targetMapPt     = bs.targetTile;
                    g_targetStandPt   = bs.standTile;
                    g_targetWorldPos  = worldP;
                    g_breakFromRange  = (bs.reach > 0);
                    g_targetCollectId = -1;
                } else if (front.kind == MineScheduler::TargetKind::Crystal) {

                    g_targetKind      = TargetKind::Crystal;
                    g_targetMapPt     = mp;
                    g_targetStandPt   = mp;
                    g_targetWorldPos  = worldP;
                    g_breakFromRange  = false;
                    g_targetCollectId = -1;
                } else if (front.kind == MineScheduler::TargetKind::Drop) {

                    g_targetKind      = TargetKind::Collectable;
                    g_targetMapPt     = mp;
                    g_targetStandPt   = mp;
                    g_targetWorldPos  = worldP;
                    g_breakFromRange  = false;
                    g_targetCollectId = front.collectId;
                } else {
                    g_targetKind      = TargetKind::Nugget;
                    g_targetMapPt     = mp;
                    g_targetStandPt   = mp;
                    g_targetWorldPos  = worldP;
                    g_breakFromRange  = false;
                    g_targetCollectId = -1;
                }
                g_hasTarget = true;
                const char* kindStr = "Nugget";
                if (front.kind == MineScheduler::TargetKind::GemOre)       kindStr = "Gemstone";
                else if (front.kind == MineScheduler::TargetKind::Crystal) kindStr = "Crystal";
                else if (front.kind == MineScheduler::TargetKind::Drop)    kindStr = "Drop";
                Diagnostics::StatPick(kindStr);
                Diagnostics::Ev("target_picked", {
                    {"kind", kindStr},
                    {"target", { mp.x, mp.y }},
                    {"reason", "scheduler_sequence"},
                    {"seqRemaining", (int)MineScheduler::SequenceSize()}
                });
                return true;
            }

            {
                MineProgress prog{};
                bool gotProg = ReadMineProgress(prog);
                bool runComplete = true;
                if (gotProg) {
                    if (g_targetGemstones && prog.totalGemstones > 0 &&
                        prog.gemstonesBroken < prog.totalGemstones)
                        runComplete = false;
                    if (g_targetNuggets && prog.totalNuggets > 0 &&
                        prog.nuggetsCollected < prog.totalNuggets)
                        runComplete = false;
                }

                {
                    static double s_lastRC = -10.0;
                    double nowRC = (double)ImGui::GetTime();
                    if (nowRC - s_lastRC >= 1.0) {
                        s_lastRC = nowRC;
                        std::cout << "[ExitDecide] seqEmpty gotProg=" << (int)gotProg
                                  << " gemsBroke=" << prog.gemstonesBroken << "/" << prog.totalGemstones
                                  << " nugColl=" << prog.nuggetsCollected << "/" << prog.totalNuggets
                                  << " (tGems=" << (int)g_targetGemstones << " tNugs=" << (int)g_targetNuggets << ")"
                                  << " runComplete=" << (int)runComplete
                                  << " haveExit=" << (int)MineScheduler::g_haveExit
                                  << " exitTile=(" << MineScheduler::g_exitTile.x << ","
                                  << MineScheduler::g_exitTile.y << ")\n";
                    }
                }
                if (!runComplete) {
                    UpdateGemstonesDynamic();
                    UpdateNuggetsDynamic();
                    UpdateFloorCollectablesDynamic();
                    FlushOpened();
                    {
                        std::lock_guard<std::mutex> lk(g_collectedIdsMutex);
                        g_collectedIds.clear();
                    }
                    MineScheduler::Recompute(g_targetGemstones, g_targetNuggets);
                    Diagnostics::Ev("scheduler_exit_deferred_full_clear", {
                        {"gems",    { prog.gemstonesBroken, prog.totalGemstones }},
                        {"nuggets", { prog.nuggetsCollected, prog.totalNuggets }}
                    });
                    g_hasTarget  = false;
                    g_targetKind = TargetKind::None;
                    return false;
                }
            }

            bool dropNearExit = false;
            if (MineScheduler::g_haveExit) {
                Vector2i exitT = MineScheduler::g_exitTile;
                std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
                for (const auto& fc : g_floorCollectables) {
                    if (WasCollectableConsumed(fc.id)) continue;
                    if (!IsGemDropBlockType((int)fc.blockType)) continue;
                    int ddx = fc.mapPoint.x - exitT.x; if (ddx < 0) ddx = -ddx;
                    int ddy = fc.mapPoint.y - exitT.y; if (ddy < 0) ddy = -ddy;
                    if ((ddx > ddy ? ddx : ddy) <= 4) { dropNearExit = true; break; }
                }
            }

            if (MineScheduler::g_haveExit && !dropNearExit) {
                Vector2i exitT = MineScheduler::g_exitTile;
                Vector3 exitW = PathRenderer::TileToWorldCenter(
                    exitT.x, exitT.y, 0.f, tileW);
                g_targetKind      = TargetKind::Exit;
                g_targetMapPt     = exitT;
                g_targetStandPt   = exitT;
                g_targetWorldPos  = exitW;
                g_breakFromRange  = false;
                g_targetCollectId = -1;
                g_hasTarget       = true;
                Diagnostics::Ev("target_picked", {
                    {"kind",   "Exit"},
                    {"target", { exitT.x, exitT.y }},
                    {"reason", "scheduler_done"}
                });
                return true;
            }
            if (!dropNearExit) return false;

        }

        UpdateFloorCollectablesForce();
        {
            std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
            int   bestId = -1;
            Vector2i bestMp{};
            Vector3  bestPos{};
            float   bestDist = FLT_MAX;
            for (const auto& fc : g_floorCollectables) {
                if (WasCollectableConsumed(fc.id)) continue;
                if (!IsGemDropBlockType((int)fc.blockType)) continue;
                Vector3 wp = PathRenderer::TileToWorldCenter(
                    fc.mapPoint.x, fc.mapPoint.y, 0.f, tileW);
                float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                float d = sqrtf(dx*dx + dy*dy);
                if (d < bestDist) {
                    bestDist = d;
                    bestId   = fc.id;
                    bestMp   = fc.mapPoint;
                    bestPos  = wp;
                }
            }
            if (bestId >= 0) {
                g_targetKind      = TargetKind::Collectable;
                g_targetMapPt     = bestMp;
                g_targetStandPt   = bestMp;
                g_breakFromRange  = false;
                g_targetWorldPos  = bestPos;
                g_targetCollectId = bestId;
                g_hasTarget       = true;
                Diagnostics::StatPick("Collectable");
                Diagnostics::Ev("target_picked", {
                    {"kind", "Collectable"},
                    {"target", { bestMp.x, bestMp.y }},
                    {"reason", "gem_drop_priority"},
                    {"strDist", bestDist}
                });
                return true;
            }
        }

        auto allCands = GatherCandidates(playerPos, tileW);
        std::vector<Cand> cands;
        cands.reserve(32);
        const size_t PER_TYPE_CAP = 4;
        for (int kindIdx = 0; kindIdx < 7; ++kindIdx) {
            TargetKind k;
            switch (kindIdx) {
                case 0: k = TargetKind::Gemstone;    break;
                case 1: k = TargetKind::Nugget;      break;
                case 2: k = TargetKind::Crystal;     break;
                case 3: k = TargetKind::Pot;         break;
                case 4: k = TargetKind::Collectable; break;
                case 5: k = TargetKind::AIEnemy;     break;
                default: continue;
            }
            std::vector<Cand> typed;
            typed.reserve(16);
            for (const auto& c : allCands) if (c.kind == k) typed.push_back(c);
            std::sort(typed.begin(), typed.end(),
                [](const Cand& a, const Cand& b){ return a.strDist < b.strDist; });
            if (typed.size() > PER_TYPE_CAP) typed.resize(PER_TYPE_CAP);
            cands.insert(cands.end(), typed.begin(), typed.end());
        }

        void* worldObj = (g_off_WCWorld && g_WorldController)
            ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld) : nullptr;

        if (!cands.empty() && worldObj) {
            Vector2i startTile = PathRenderer::WorldToTile(
                playerPos.x, playerPos.y + tileW * 0.5f, tileW);

            int   bestLen   = INT_MAX;
            int   bestIdx   = -1;
            std::vector<BreakSetup> setups(cands.size());
            for (size_t i = 0; i < cands.size(); ++i) {
                Vector2i pathGoal;
                if (cands[i].kind == TargetKind::Collectable
                 || cands[i].kind == TargetKind::Nugget
                 || cands[i].kind == TargetKind::AIEnemy) {
                    setups[i].valid      = true;
                    setups[i].standTile  = cands[i].mp;
                    setups[i].targetTile = cands[i].mp;
                    setups[i].reach      = 0;
                    setups[i].vertical   = false;
                    pathGoal = cands[i].mp;
                } else {
                    setups[i] = ComputeBreakSetup(cands[i].mp, startTile);
                    if (!setups[i].valid) continue;
                    pathGoal = setups[i].standTile;
                }
                PathRenderer::PathResult r = PathRenderer::BuildPathMining(
                    worldObj, startTile, pathGoal, &MiningCost, g_weights);
                if (r.status != PathRenderer::PathStatus::Success) continue;
                int len = (int)r.tiles.size();
                if (len < bestLen) { bestLen = len; bestIdx = (int)i; }
            }
            if (bestIdx >= 0) {
                const BreakSetup& bs = setups[bestIdx];
                g_targetKind      = cands[bestIdx].kind;
                g_targetMapPt     = bs.targetTile;
                g_targetStandPt   = bs.standTile;
                g_breakFromRange  = bs.valid && (bs.standTile.x != bs.targetTile.x ||
                                                  bs.standTile.y != bs.targetTile.y);
                g_targetWorldPos  = PathRenderer::TileToWorldCenter(
                    bs.standTile.x, bs.standTile.y, 0.f, tileW);
                g_targetCollectId = cands[bestIdx].collectId;
                if (g_targetKind == TargetKind::AIEnemy) {
                    g_targetAIEnemyId = cands[bestIdx].collectId;
                    g_targetCollectId = -1;
                } else {
                    g_targetAIEnemyId = -1;
                }
                g_hasTarget       = true;

                const char* kindStr = "?";
                switch (g_targetKind) {
                case TargetKind::Gemstone:    kindStr = "Gemstone";    break;
                case TargetKind::Nugget:      kindStr = "Nugget";      break;
                case TargetKind::Crystal:     kindStr = "Crystal";     break;
                case TargetKind::Pot:         kindStr = "Pot";         break;
                case TargetKind::Collectable: kindStr = "Collectable"; break;
                case TargetKind::Exit:        kindStr = "Exit";        break;
                case TargetKind::AIEnemy:     kindStr = "AIEnemy";     break;
                default: break;
                }
                Diagnostics::StatPick(kindStr);
                Diagnostics::Ev("target_picked", {
                    {"kind", kindStr},
                    {"target", { bs.targetTile.x, bs.targetTile.y }},
                    {"stand",  { bs.standTile.x,  bs.standTile.y }},
                    {"breakFromRange", g_breakFromRange},
                    {"reach", bs.reach},
                    {"vertical", bs.vertical},
                    {"candidates", (int)cands.size()},
                    {"pathLen", bestLen}
                });
                return true;
            }

            for (size_t i = 0; i < cands.size(); ++i) {
                if (!setups[i].valid && cands[i].kind != TargetKind::Collectable) {
                    MarkOpened(cands[i].mp);
                    const char* k = "Unknown";
                    switch (cands[i].kind) {
                    case TargetKind::Gemstone: k = "Gemstone"; break;
                    case TargetKind::Nugget:   k = "Nugget";   break;
                    case TargetKind::Crystal:  k = "Crystal";  break;
                    case TargetKind::Pot:      k = "Pot";      break;
                    default: break;
                    }
                    Diagnostics::StatUnreach(k);
                    Diagnostics::Ev("target_unreachable", {
                        {"kind", k},
                        {"target", { cands[i].mp.x, cands[i].mp.y }},
                        {"reason", "noStandTile"}
                    });
                }
            }
        }

        if (g_targetExit) {
            MineProgress mp{};
            bool gotProg = ReadMineProgress(mp);
            bool runComplete = true;
            if (gotProg) {

                if (g_targetGemstones &&
                    mp.totalGemstones > 0 && mp.gemstonesBroken < mp.totalGemstones)
                    runComplete = false;
                if (g_targetNuggets &&
                    mp.totalNuggets > 0 && mp.nuggetsCollected < mp.totalNuggets)
                    runComplete = false;
            }
            if (!runComplete) {

                UpdateGemstonesDynamic();
                UpdateNuggetsDynamic();
                UpdateFloorCollectablesDynamic();

                FlushOpened();
                {
                    std::lock_guard<std::mutex> lk(g_collectedIdsMutex);
                    g_collectedIds.clear();
                }
                Diagnostics::Ev("exit_deferred_for_full_clear", {
                    {"gems",    { mp.gemstonesBroken, mp.totalGemstones }},
                    {"nuggets", { mp.nuggetsCollected, mp.totalNuggets }},
                    {"opened_flushed", true}
                });

                g_hasTarget = false;
                g_targetKind = TargetKind::None;
                return false;
            }

            std::lock_guard<std::mutex> lk(g_exitPortalLocationsMutex);
            float bestDist = FLT_MAX;
            Vector3 bestPos = {}; Vector2i bestPt = {};
            for (const auto& pt : g_exitPortalLocations) {
                Vector3 wp = PathRenderer::TileToWorldCenter(pt.x, pt.y, 0.f, tileW);
                float dx = wp.x - playerPos.x, dy = wp.y - playerPos.y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < bestDist) { bestDist = dist; bestPos = wp; bestPt = pt; }
            }
            if (bestDist < FLT_MAX) {
                g_targetKind      = TargetKind::Exit;
                g_targetMapPt     = bestPt;
                g_targetStandPt   = bestPt;
                g_targetWorldPos  = bestPos;
                g_targetCollectId = -1;
                g_breakFromRange  = false;
                g_hasTarget       = true;
                return true;
            }
        }
        g_hasTarget = false;
        g_targetKind = TargetKind::None;
        return false;
    }

    inline bool ValidateTarget() {
        if (!g_hasTarget) return false;

        constexpr int TARGET_LOCK_MS = 1500;
        if (g_targetKind == TargetKind::Gemstone || g_targetKind == TargetKind::Nugget
         || g_targetKind == TargetKind::Crystal  || g_targetKind == TargetKind::Pot) {
            if (g_breakingTile.x == g_targetMapPt.x &&
                g_breakingTile.y == g_targetMapPt.y) {
                auto sinceMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - g_breakingLastHit).count();
                if (sinceMs < TARGET_LOCK_MS) {
                    int bt = ReadBlockType(g_targetMapPt);
                    if (bt > 0) return true;
                    if (bt < 0) return true;

                }
            }
        }

        if (MineScheduler::IsSessionActive() &&
            g_targetKind != TargetKind::Exit &&
            g_targetKind != TargetKind::Collectable) {
            MineScheduler::Target front;
            if (MineScheduler::TryGetFront(front)) {
                if (front.mapPoint.x != g_targetMapPt.x ||
                    front.mapPoint.y != g_targetMapPt.y) {
                    return false;
                }
            }
        }

        switch (g_targetKind) {

        case TargetKind::Gemstone:
        case TargetKind::Nugget:
        case TargetKind::Crystal:
        case TargetKind::Pot: {
            int bt = ReadBlockType(g_targetMapPt);
            if (bt == 0) {

                if (g_breakingTile.x == g_targetMapPt.x &&
                    g_breakingTile.y == g_targetMapPt.y) {
                    return true;
                }
                break;
            }
            if (bt < 0) {

                if (g_targetKind == TargetKind::Gemstone) {
                    std::lock_guard<std::mutex> lk(g_gemstoneLocationsMutex);
                    for (const auto& g : g_gemstoneLocations)
                        if (g.mapPoint.x == g_targetMapPt.x && g.mapPoint.y == g_targetMapPt.y) return true;
                } else if (g_targetKind == TargetKind::Nugget) {
                    std::lock_guard<std::mutex> lk(g_nuggetLocationsMutex);
                    for (const auto& g : g_nuggetLocations)
                        if (g.mapPoint.x == g_targetMapPt.x && g.mapPoint.y == g_targetMapPt.y) return true;
                } else if (g_targetKind == TargetKind::Crystal) {
                    std::lock_guard<std::mutex> lk(g_lightCrystalLocationsMutex);
                    for (const auto& g : g_lightCrystalLocations)
                        if (g.mapPoint.x == g_targetMapPt.x && g.mapPoint.y == g_targetMapPt.y) return true;
                } else if (g_targetKind == TargetKind::Pot) {
                    std::lock_guard<std::mutex> lk(g_potLocationsMutex);
                    for (const auto& g : g_potLocations)
                        if (g.mapPoint.x == g_targetMapPt.x && g.mapPoint.y == g_targetMapPt.y) return true;
                }
                break;
            }

            return true;
        }
        case TargetKind::Collectable: {

            std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
            for (const auto& g : g_floorCollectables) {
                if (g.id == g_targetCollectId) {
                    const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                                          ? PathRenderer::g_tileSize.x : 0.32f;
                    g_targetMapPt    = g.mapPoint;
                    g_targetStandPt  = g.mapPoint;
                    g_targetWorldPos = PathRenderer::TileToWorldCenter(
                        g.mapPoint.x, g.mapPoint.y, 0.f, tileW);
                    g_breakFromRange = false;
                    return true;
                }
            }
            break;
        }
        case TargetKind::Exit:
            return true;
        case TargetKind::AIEnemy: {

            if (g_targetAIEnemyId < 0) break;
            if (!g_WorldController || !oConvertWorldPointToMapPoint) break;
            std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
            for (void* inst : g_AIInstances) {
                if (!inst || IsBadReadPtr(inst, sizeof(void*))) continue;
                AIBase* base = *(AIBase**)((uintptr_t)inst + g_off_AIBase);
                if (!base || IsBadReadPtr(base, sizeof(AIBase))) continue;
                if (base->id != g_targetAIEnemyId) continue;
                if (!IsEnemyHittableForBot(inst, base)) break;
                Vector3 ep = *(Vector3*)((uintptr_t)inst + g_off_AITempPosition);
                Vector2i emp = oConvertWorldPointToMapPoint(g_WorldController, ep);
                const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                                      ? PathRenderer::g_tileSize.x : 0.32f;
                g_targetMapPt    = emp;
                g_targetStandPt  = emp;
                g_targetWorldPos = ep;
                return true;
            }
            break;
        }
        case TargetKind::None:
            break;
        }
        g_hasTarget = false;
        g_targetKind = TargetKind::None;
        return false;
    }

    inline void ResetPhase() {
        g_phase = Phase::Active;
        g_hasTarget = false;
        g_targetKind = TargetKind::None;
        std::lock_guard<std::mutex> lk(g_pathMutex);
        g_pathTiles.clear();
        g_pathOffsets.clear();
        g_pathIdx = 0;
        g_pathLastGoal = { -9999, -9999 };
    }

    inline void Tick(float dt) {

        {
            static bool s_lastInMine = false;
            static bool s_lastGems   = g_targetGemstones;
            static bool s_lastNugs   = g_targetNuggets;
            static auto s_lastStartAttempt = std::chrono::steady_clock::now() - std::chrono::seconds(60);
            bool curInMine = g_enabled && IsInMineWorld();
            bool worldChanged = curInMine && MineScheduler::IsSessionActive()
                                          && MineScheduler::WorldObjectChanged();

            auto nowL = std::chrono::steady_clock::now();
            bool startNeeded = curInMine && (!s_lastInMine || worldChanged ||
                                             !MineScheduler::IsSessionActive());
            bool retryReady = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  nowL - s_lastStartAttempt).count() >= 500;
            if (startNeeded && retryReady) {
                MineScheduler::EndSession();
                MineScheduler::StartSession(g_targetGemstones, g_targetNuggets);
                s_lastGems = g_targetGemstones;
                s_lastNugs = g_targetNuggets;
                s_lastStartAttempt = nowL;

                g_gmExitFiredThisSession.store(false);
                g_doShowAnimationSeen.store(false);
                g_lastGMExitSentMs.store(-1);
                g_exitBlockedValid = false;
            } else if (!curInMine && s_lastInMine) {
                MineScheduler::EndSession();

                AutoLoop::StopWalk();

                g_gmExitFiredThisSession.store(false);
                g_doShowAnimationSeen.store(false);
                g_lastGMExitSentMs.store(-1);
            } else if (curInMine && MineScheduler::IsSessionActive() &&
                       (s_lastGems != g_targetGemstones || s_lastNugs != g_targetNuggets)) {
                MineScheduler::Recompute(g_targetGemstones, g_targetNuggets);
                s_lastGems = g_targetGemstones;
                s_lastNugs = g_targetNuggets;
            }
            s_lastInMine = curInMine;
        }

        auto amBailLog = [&](const char* gate) {
            static std::unordered_map<std::string, std::chrono::steady_clock::time_point> s_last;
            auto& tp = s_last[gate];
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - tp).count() < 1) return;
            tp = now;

            if (!g_enabled) return;
            std::cout << "[AMTick] bail @ " << gate
                      << " (enabled=" << (int)g_enabled
                      << " inWorld=" << (int)g_IsInWorld
                      << " hasPlayer=" << (g_LocalPlayer ? 1 : 0)
                      << " sched=" << (int)MineScheduler::IsSessionActive()
                      << ")\n";
        };

        if (!g_enabled || !g_IsInWorld || !g_LocalPlayer) {
            if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                PathFinder_ClearInputs();
            amBailLog(!g_enabled       ? "!g_enabled"
                    : !g_IsInWorld     ? "!g_IsInWorld"
                                       : "!g_LocalPlayer");
            return;
        }

        if (g_antiSleep) SetAntiSleepFields();

        if (std::chrono::steady_clock::now() < g_resumeAfter) {
            auto remMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                g_resumeAfter - std::chrono::steady_clock::now()).count();
            static auto s_lastResumeBail = std::chrono::steady_clock::now() - std::chrono::seconds(10);
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - s_lastResumeBail).count() >= 1) {
                s_lastResumeBail = now;
                if (g_enabled)
                    std::cout << "[AMTick] bail @ g_resumeAfter (waiting " << remMs << "ms)\n";
            }
            return;
        }
        if (!IsInMine()) { amBailLog("!IsInMine (mineOnly + not in mine layout)"); return; }
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) {
            amBailLog("PathRenderer fn ptrs unresolved");
            return;
        }
        if (g_requirePickaxe && !HasPickaxeEquipped()) { amBailLog("requirePickaxe + none equipped"); return; }

        if (g_antiAfk && g_LocalPlayer && g_off_Player_jumpButtonDown > 0
            && !g_botBusy.load(std::memory_order_relaxed))
        {
            static auto s_lastAfk = std::chrono::steady_clock::now();
            auto secs = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - s_lastAfk).count();
            if (secs >= 30) {
                bool* jp = (bool*)((char*)g_LocalPlayer + g_off_Player_jumpButtonDown);
                if (!IsBadWritePtr(jp, sizeof(bool))) *jp = true;
                s_lastAfk = std::chrono::steady_clock::now();
            }
        }

        g_runtime  += dt;
        g_hitTimer += dt;

        void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!lt) return;
        Vector3 here = PathRenderer::oGetPosition(lt);
        const float tileWPre = (PathRenderer::g_tileSize.x > 0.001f)
                                  ? PathRenderer::g_tileSize.x : 0.32f;

        MineDebug::Snap _mdbg{};
        _mdbg.tSec           = (double)ImGui::GetTime();
        { static int s_dbgFrame = 0; _mdbg.frame = ++s_dbgFrame; }
        _mdbg.inWorld        = g_IsInWorld;
        _mdbg.hasLocalPlayer = (g_LocalPlayer != nullptr);
        _mdbg.pos            = here;
        _mdbg.tile           = PathRenderer::WorldToTile(here.x, here.y + tileWPre * 0.5f, tileWPre);
        _mdbg.amEnabled      = g_enabled;
        _mdbg.hasTarget      = g_hasTarget;
        _mdbg.targetTile     = g_hasTarget ? PathRenderer::WorldToTile(
                                                 g_targetWorldPos.x,
                                                 g_targetWorldPos.y,
                                                 tileWPre)
                                           : Vector2i{ 0, 0 };
        _mdbg.targetKind     = [](TargetKind k) -> const char* {
            switch (k) {
                case TargetKind::None:        return "None";
                case TargetKind::Gemstone:    return "Gemstone";
                case TargetKind::Nugget:      return "Nugget";
                case TargetKind::Crystal:     return "Crystal";
                case TargetKind::Pot:         return "Pot";
                case TargetKind::Collectable: return "Collectable";
                case TargetKind::Exit:        return "Exit";
                case TargetKind::AIEnemy:     return "AIEnemy";
            }
            return "?";
        }(g_targetKind);
        _mdbg.breakingActive = (g_breakingTile.x != -9999);
        _mdbg.breakingTile   = g_breakingTile;
        _mdbg.breakingHits   = g_breakingHits;
        _mdbg.lingerActive   = LingerActive();
        _mdbg.lingerTile     = g_lingerTile;
        _mdbg.moveMethod     = (int)g_moveMethod;
        _mdbg.tileW          = tileWPre;
        _mdbg.intervalMs     = g_portalIntervalMs;
        _mdbg.lastPortalSec  = g_lastPortalSec;
        _mdbg.sinceLastPortalS = _mdbg.tSec - g_lastPortalSec;
        _mdbg.fastDecision   = "-";

        MineDebug::SetSnap(_mdbg);

        if (LingerActive()) {
            auto nowL = std::chrono::steady_clock::now();
            auto sinceMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                nowL - g_lingerStarted).count();

            Vector2i playerTileNow = PathRenderer::WorldToTile(
                here.x, here.y + tileWPre * 0.5f, tileWPre);
            if (!g_lingerArrivedFlag &&
                playerTileNow.x == g_lingerTile.x &&
                playerTileNow.y == g_lingerTile.y) {
                g_lingerArrivedFlag = true;
                g_lingerArrivedAt   = nowL;
                Diagnostics::Ev("linger_arrived", {
                    {"tile", { g_lingerTile.x, g_lingerTile.y }},
                    {"walkMs", sinceMs}
                });
            }

            bool idleDone = false;
            if (g_lingerArrivedFlag) {
                auto idleMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                    nowL - g_lingerArrivedAt).count();
                idleDone = (idleMs >= LINGER_IDLE_MS);
            }
            bool maxHit = (sinceMs >= LINGER_MAX_MS);
            if (idleDone || maxHit) {
                Diagnostics::Ev("linger_window_done", {
                    {"ms",       sinceMs},
                    {"arrived",  g_lingerArrivedFlag},
                    {"viaMaxCap",maxHit}
                });
                EndLinger();
                if (!PickTarget()) return;
            } else {

                UpdateFloorCollectablesForce();

                Vector2i closestDropTile = g_lingerTile;
                int closestDropId = -1;
                float closestDropDist = FLT_MAX;
                {
                    std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
                    for (const auto& fc : g_floorCollectables) {
                        bool isGem = IsGemDropBlockType((int)fc.blockType);
                        bool isNug = IsNugget(fc.blockType);
                        if (!isGem && !isNug) {

                        }

                        int dxT = fc.mapPoint.x - g_lingerTile.x;
                        int dyT = fc.mapPoint.y - g_lingerTile.y;
                        if (std::abs(dxT) > 6 || std::abs(dyT) > 8) continue;
                        Vector3 wp = PathRenderer::TileToWorldCenter(
                            fc.mapPoint.x, fc.mapPoint.y, 0.f, tileWPre);
                        float ddx = wp.x - here.x, ddy = wp.y - here.y;
                        float dPlayer = sqrtf(ddx*ddx + ddy*ddy);
                        if (dPlayer < closestDropDist) {
                            closestDropDist = dPlayer;
                            closestDropTile = fc.mapPoint;
                            closestDropId   = fc.id;
                        }
                    }
                }

                Vector2i targetTile = (closestDropId >= 0) ? closestDropTile
                                                            : g_lingerTile;

                if (g_lingerArrivedFlag &&
                    (targetTile.x != g_targetMapPt.x ||
                     targetTile.y != g_targetMapPt.y)) {
                    g_lingerArrivedFlag = false;
                }
                Vector3 targetWorld = PathRenderer::TileToWorldCenter(
                    targetTile.x, targetTile.y, here.z, tileWPre);
                g_targetKind      = TargetKind::Collectable;
                g_targetWorldPos  = targetWorld;
                g_targetMapPt     = targetTile;
                g_targetStandPt   = targetTile;
                g_breakFromRange  = false;

                g_targetCollectId = (closestDropId >= 0) ? closestDropId : -2;
                g_hasTarget       = true;
            }
        } else if (!ValidateTarget()) {
            if (!PickTarget()) return;
        }

        const float tileW = tileWPre;
        const float bodyOff = tileW * g_bodyOffsetFrac;
        Vector3 hereBody = { here.x, here.y + bodyOff, here.z };

        Vector2i startTile = PathRenderer::WorldToTile(here.x, here.y + tileW * 0.5f, tileW);
        Vector2i goalTile  = PathRenderer::WorldToTile(g_targetWorldPos.x, g_targetWorldPos.y, tileW);

        if (g_hasTarget && g_breakFromRange) {
            int floorBt = ReadBlockType({ g_targetStandPt.x, g_targetStandPt.y - 1 });
            bool floorSolid = (floorBt > 0
                && !PathRenderer::PF_IsHardcodedPassable(floorBt));
            if (!floorSolid) {
                BreakSetup bs = ComputeBreakSetup(g_targetMapPt, startTile);
                if (bs.valid) {
                    g_targetStandPt  = bs.standTile;
                    g_targetWorldPos = PathRenderer::TileToWorldCenter(
                        bs.standTile.x, bs.standTile.y, here.z, tileW);
                    g_breakFromRange = (bs.standTile.x != bs.targetTile.x ||
                                        bs.standTile.y != bs.targetTile.y);
                    Diagnostics::Ev("breakspot_revalidated", {
                        {"target",   { g_targetMapPt.x,    g_targetMapPt.y    }},
                        {"newStand", { g_targetStandPt.x, g_targetStandPt.y }},
                        {"reason",   "floor_lost"}
                    });
                    goalTile = bs.standTile;
                } else {
                    Diagnostics::Ev("breakspot_abandoned", {
                        {"target", { g_targetMapPt.x, g_targetMapPt.y }},
                        {"reason", "no_valid_stand_after_floor_lost"}
                    });
                    MarkOpened(g_targetMapPt);
                    g_hasTarget = false;
                    g_targetKind = TargetKind::None;
                    return;
                }
            }
        }

        auto now = std::chrono::steady_clock::now();

        if (g_pathIdx != g_pathIdxLastSeen) {
            g_pathIdxLastSeen   = g_pathIdx;
            g_pathIdxLastChange = now;
        }
        long long sinceProgress = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_pathIdxLastChange).count();
        bool goalChanged = (goalTile.x != g_pathLastGoal.x || goalTile.y != g_pathLastGoal.y);
        bool pathInvalid = g_pathTiles.empty()
                        || g_pathStatus != PathRenderer::PathStatus::Success;
        bool stalled = (sinceProgress > PATH_STALL_REBUILD_MS);
        bool needRebuild = goalChanged || pathInvalid || stalled;

        if (needRebuild) {
            void* worldObj = g_off_WCWorld
                ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld) : nullptr;
            PathRenderer::PathResult res = PathRenderer::BuildPathMining(
                worldObj, startTile, goalTile, &MiningCost, g_weights);

            if (res.status == PathRenderer::PathStatus::Success) {
                Diagnostics::StatPathBuilt();
                nlohmann::json tilesArr = nlohmann::json::array();
                size_t cap = res.tiles.size();
                bool truncated = false;
                if (cap > 400) {
                    cap = 400;
                    truncated = true;
                }
                for (size_t i = 0; i < res.tiles.size(); ++i) {
                    if (i == 200 && truncated) {

                        i = res.tiles.size() - 200;
                        tilesArr.push_back(nlohmann::json::array({ -1, -1 }));
                    }
                    tilesArr.push_back(nlohmann::json::array({ res.tiles[i].x, res.tiles[i].y }));
                }
                Diagnostics::Ev("path_built", {
                    {"start",     { startTile.x, startTile.y }},
                    {"goal",      { goalTile.x,  goalTile.y }},
                    {"tileCount", (int)res.tiles.size()},
                    {"tiles",     tilesArr},
                    {"truncated", truncated},
                    {"targetKind", (int)g_targetKind},
                    {"targetMp", { g_targetMapPt.x, g_targetMapPt.y }}
                });
            } else {
                Diagnostics::StatPathFailed();
                Diagnostics::Ev("path_failed", {
                    {"start",  { startTile.x, startTile.y }},
                    {"goal",   { goalTile.x,  goalTile.y }},
                    {"status", (int)res.status},
                    {"targetKind", (int)g_targetKind},
                    {"targetMp", { g_targetMapPt.x, g_targetMapPt.y }}
                });
            }
            std::lock_guard<std::mutex> lk(g_pathMutex);
            g_pathTiles    = std::move(res.tiles);
            g_pathOffsets  = std::move(res.offsets);
            g_pathStatus   = res.status;
            g_pathIdx      = 0;
            g_pathLastGoal = goalTile;
            g_pathLastBuild = now;

            g_pathIdxLastSeen   = 0;
            g_pathIdxLastChange = now;

            auto isSolidFloor = [](int bt) -> bool {
                if (bt <= 0) return false;
                if (PathRenderer::PF_IsHardcodedPassable(bt)) return false;
                if (IsNugget((World::BlockType)bt)) return false;
                if (::Door::IsDoor(bt)) return false;
                if (PathRenderer::oCfgIsHatch   && PathRenderer::oCfgIsHatch((World::BlockType)bt))   return false;
                if (PathRenderer::oCfgIsPortal  && PathRenderer::oCfgIsPortal((World::BlockType)bt))  return false;
                if (PathRenderer::PF_IsHardcodedSolid(bt)) return true;
                if (PathRenderer::oCfgIsInstakill && PathRenderer::oCfgIsInstakill((World::BlockType)bt)) return false;
                if (PathRenderer::oCfgDoesCollide) {
                    return PathRenderer::oCfgDoesCollide((World::BlockType)bt);
                }
                return true;
            };

            int airStreak = 0;
            std::lock_guard<std::mutex> lkR(g_restTilesMutex);
            for (size_t i = 0; i < g_pathTiles.size(); ++i) {
                Vector2i t = g_pathTiles[i];
                int btBelow = ReadBlockType({ t.x, t.y - 1 });
                bool grounded = isSolidFloor(btBelow);
                if (!grounded) {
                    airStreak++;
                } else {
                    if (airStreak >= 3 && i > 0) {
                        long long key = PackTile(t.x, t.y);
                        if (g_consumedRestTiles.find(key) == g_consumedRestTiles.end())
                            g_pendingRestTiles.insert(key);
                    }
                    airStreak = 0;
                }
            }
        }

        std::lock_guard<std::mutex> lk(g_pathMutex);
        if (g_pathStatus != PathRenderer::PathStatus::Success || g_pathTiles.empty()) {

            MarkOpened(g_targetMapPt);
            if (g_targetKind == TargetKind::Collectable) MarkCollectableConsumed(g_targetCollectId);
            g_hasTarget = false; g_targetKind = TargetKind::None;
            return;
        }

        {
            Vector2i playerNow = PathRenderer::WorldToTile(
                here.x, here.y + tileW * 0.5f, tileW);
            std::lock_guard<std::mutex> lkR(g_restTilesMutex);
            long long key = PackTile(playerNow.x, playerNow.y);
            if (g_pendingRestTiles.find(key) != g_pendingRestTiles.end()) {
                int btBelow = ReadBlockType({ playerNow.x, playerNow.y - 1 });
                bool floorSolid = (btBelow > 0
                    && !PathRenderer::PF_IsHardcodedPassable(btBelow));
                if (!floorSolid) {
                    g_pendingRestTiles.erase(key);
                    g_consumedRestTiles.insert(key);
                    Diagnostics::Ev("rest_pocket_dropped", {
                        {"tile",   { playerNow.x, playerNow.y }},
                        {"reason", "no_solid_below"},
                        {"btBelow", btBelow}
                    });
                    goto rest_check_done;
                }
                g_pendingRestTiles.erase(key);
                g_consumedRestTiles.insert(key);
                g_restHoldUntil = std::chrono::steady_clock::now()
                                  + std::chrono::milliseconds(REST_HOLD_MS);

                if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                    PathFinder_ClearInputs();
                if (oSetVelocity && g_LocalPlayer)
                    QueueSetVelocity(0.f, 0.f, 0.f);
                Diagnostics::Ev("rest_pocket_hold", {
                    {"tile", { playerNow.x, playerNow.y }},
                    {"ms",   REST_HOLD_MS},
                    {"trigger", "player_on_tile"}
                });
            }
            rest_check_done: ;
        }

        while (g_pathIdx < (int)g_pathTiles.size() - 1) {
            Vector3 wpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
            float wdx = wpc.x - hereBody.x, wdy = wpc.y - hereBody.y;
            if (sqrtf(wdx*wdx + wdy*wdy) <= g_pfStopRange) {
                ++g_pathIdx;
            } else {
                break;
            }
        }

        if (RestHoldActive()) {
            if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                PathFinder_ClearInputs();
            return;
        }

        if (PostBreakHoldActive()) {
            if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                PathFinder_ClearInputs();
            return;
        }

        PickaxeRepairTick();

        ExtendedPickup::Tick();

        const int combatRange = g_combatTileRange;
        g_combatTimer += dt;
        const bool combatActive = g_targetAI && g_combatEnabled && EnemyInHitRange(combatRange);
        if (combatActive && g_combatTimer >= g_combatCadence) {
            g_combatTimer = 0.f;
            if (MineHitNearest(combatRange)) {
                Diagnostics::StatCombatHit();
                Diagnostics::Ev("combat_hit_ai", { {"rangeTiles", combatRange} });
            }
        }

        if (!combatActive && g_targetCrystals && g_crystalBreakIfClose &&
            g_hitTimer >= g_hitInterval) {
            float frac = GetLightCrystalFraction();
            if (frac >= 0.f && frac < g_crystalBreakIfCloseThreshold) {

                Vector2i playerTile = PathRenderer::WorldToTile(
                    here.x, here.y + tileW * 0.5f, tileW);

                bool grounded = false;
                if (g_WorldController && g_off_WCWorld) {
                    void* worldObj = *(void**)((uintptr_t)g_WorldController +
                                               g_off_WCWorld);
                    PathRenderer::PF_BlockLayerInfo bli =
                        PathRenderer::PF_ReadBlockLayer(worldObj);
                    if (bli.ok) {
                        grounded = PathRenderer::PF_IsGrounded_Safe(
                            bli.layer, playerTile.x, playerTile.y,
                            bli.w, bli.h, false);
                    }
                }
                if (!grounded) {

                    Diagnostics::Ev("crystal_break_if_close_skip_airborne", {
                        {"playerTile", { playerTile.x, playerTile.y }},
                        {"barFrac", frac}
                    });
                } else {
                    Vector2i closeCrystal{ -1, -1 };
                    {
                        std::lock_guard<std::mutex> lk(g_lightCrystalLocationsMutex);
                        int bestCheb = HIT_REACH + 1;
                        for (const auto& c : g_lightCrystalLocations) {
                            int dx = c.mapPoint.x - playerTile.x; if (dx < 0) dx = -dx;
                            int dy = c.mapPoint.y - playerTile.y; if (dy < 0) dy = -dy;
                            int cheb = (dx > dy) ? dx : dy;
                            if (cheb <= HIT_REACH && cheb < bestCheb) {
                                bestCheb = cheb;
                                closeCrystal = c.mapPoint;
                            }
                        }
                    }
                    if (closeCrystal.x >= 0) {
                        SendHitBlock(closeCrystal);
                        NoteBreakHit(closeCrystal);
                        g_hitTimer = 0.f;
                        Diagnostics::Ev("crystal_break_if_close", {
                            {"tile", { closeCrystal.x, closeCrystal.y }},
                            {"playerTile", { playerTile.x, playerTile.y }},
                            {"barFrac", frac}
                        });
                    }
                }
            }
        }

        Diagnostics::TickProgressSample();

        if (g_pathIdx >= (int)g_pathTiles.size() - 1) {

            float dx = g_targetWorldPos.x - hereBody.x;
            float dy = g_targetWorldPos.y - hereBody.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist <= g_stopRange) {
                if (g_targetKind == TargetKind::Collectable) {

                    if (g_targetCollectId < 0) {
                        if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                            PathFinder_ClearInputs();
                        return;
                    }

                    UpdateFloorCollectablesForce();
                    bool stillInScanner = false;
                    {
                        std::lock_guard<std::mutex> lkS(g_floorCollectablesMutex);
                        for (const auto& fc : g_floorCollectables) {
                            if (fc.id == g_targetCollectId) { stillInScanner = true; break; }
                        }
                    }
                    if (!stillInScanner) {

                        MarkCollectableConsumed(g_targetCollectId);
                        g_collects++;
                        g_hasTarget = false; g_targetKind = TargetKind::None;
                        return;
                    }

                    if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                        PathFinder_ClearInputs();
                    return;
                }
                if (g_targetKind == TargetKind::Nugget) {

                    int btNow = ReadBlockType(g_targetMapPt);
                    bool collected = (btNow == 0);
                    if (!collected) {
                        std::lock_guard<std::mutex> lkN(g_nuggetLocationsMutex);
                        bool stillInScanner = false;
                        for (const auto& n : g_nuggetLocations) {
                            if (n.mapPoint.x == g_targetMapPt.x &&
                                n.mapPoint.y == g_targetMapPt.y) {
                                stillInScanner = true; break;
                            }
                        }
                        if (!stillInScanner) collected = true;
                    }
                    static std::chrono::steady_clock::time_point s_nugHoldStart =
                        std::chrono::steady_clock::now() - std::chrono::seconds(10);
                    static Vector2i s_nugHoldTile = { -9999, -9999 };
                    if (s_nugHoldTile.x != g_targetMapPt.x ||
                        s_nugHoldTile.y != g_targetMapPt.y) {
                        s_nugHoldTile = g_targetMapPt;
                        s_nugHoldStart = std::chrono::steady_clock::now();
                    }
                    auto held = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - s_nugHoldStart).count();
                    bool bail = (held >= 3000);
                    if (collected || bail) {
                        Diagnostics::StatBreak("Nugget", 0);
                        Diagnostics::Ev("nugget_collected", {
                            {"tile", { g_targetMapPt.x, g_targetMapPt.y }},
                            {"viaBail", bail}
                        });
                        MarkOpened(g_targetMapPt);
                        g_collects++;
                        g_hasTarget = false;
                        g_targetKind = TargetKind::None;
                        return;
                    }

                    if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                        PathFinder_ClearInputs();
                    return;
                }
                if (g_targetKind == TargetKind::AIEnemy) {

                    if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                        PathFinder_ClearInputs();
                    return;
                }
                if (g_targetKind == TargetKind::Exit) {

                    bool nearExit       = _PlayerNearExitTile_3x3();
                    bool targetsCleared =
                        (!g_targetGemstones || MineScheduler::g_lastGemCount    == 0) &&
                        (!g_targetNuggets   || MineScheduler::g_lastNuggetCount == 0);
                    bool popupSeen      = g_doShowAnimationSeen.load();
                    long long sinceLast = MsSinceGMExit();
                    bool retryReady     = (sinceLast < 0) || (sinceLast >= 200);

                    bool forceExit = false;
                    if (!popupSeen && !targetsCleared) {
                        if (!g_exitBlockedValid) {
                            g_exitBlockedValid = true;
                            g_exitBlockedSince = std::chrono::steady_clock::now();
                        }
                        long long blockedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - g_exitBlockedSince).count();
                        if (blockedMs < EXIT_FORCE_AFTER_MS) {

                            static double s_lastExitRescan = -10.0;
                            double nowS2 = (double)ImGui::GetTime();
                            if (nowS2 - s_lastExitRescan >= 1.0) {
                                s_lastExitRescan = nowS2;
                                UpdateGemstonesDynamic();
                                UpdateNuggetsDynamic();
                                UpdateFloorCollectablesDynamic();
                                MineScheduler::Recompute(g_targetGemstones, g_targetNuggets);
                            }
                            std::cout << "[ExitGate] uncleared (gems="
                                      << MineScheduler::g_lastGemCount << " nugs="
                                      << MineScheduler::g_lastNuggetCount << " blockedMs="
                                      << blockedMs << ") -> re-acquiring remaining targets\n";
                            g_hasTarget  = false;
                            g_targetKind = TargetKind::None;
                            return;
                        }

                        forceExit = true;
                        std::cout << "[ExitGate] uncleared " << blockedMs
                                  << "ms (remainder unreachable: gems="
                                  << MineScheduler::g_lastGemCount << " nugs="
                                  << MineScheduler::g_lastNuggetCount
                                  << ") -> FORCING exit\n";
                    } else if (targetsCleared) {
                        g_exitBlockedValid = false;
                    }

                    {
                        static double s_lastExitLog = -10.0;
                        double nowS = (double)ImGui::GetTime();
                        if (nowS - s_lastExitLog >= 0.5) {
                            s_lastExitLog = nowS;
                            MineDebug::Log("exit-gate: near=%d cleared=%d popupSeen=%d "
                                           "combat=%d retryReady=%d sinceLastMs=%lld "
                                           "gems=%d nugs=%d "
                                           "(toggleGems=%d toggleNugs=%d) tile=(%d,%d)",
                                           (int)nearExit, (int)targetsCleared,
                                           (int)popupSeen, (int)combatActive,
                                           (int)retryReady, sinceLast,
                                           MineScheduler::g_lastGemCount,
                                           MineScheduler::g_lastNuggetCount,
                                           (int)g_targetGemstones,
                                           (int)g_targetNuggets,
                                           g_targetMapPt.x, g_targetMapPt.y);

                            std::cout << "[ExitGate] near=" << (int)nearExit
                                      << " cleared=" << (int)targetsCleared
                                      << " popupSeen=" << (int)popupSeen
                                      << " combat=" << (int)combatActive
                                      << " retryReady=" << (int)retryReady
                                      << " gems=" << MineScheduler::g_lastGemCount
                                      << " nugs=" << MineScheduler::g_lastNuggetCount
                                      << " (tGems=" << (int)g_targetGemstones
                                      << " tNugs=" << (int)g_targetNuggets
                                      << ") tile=(" << g_targetMapPt.x << ","
                                      << g_targetMapPt.y << ")\n";
                        }
                    }
                    if (retryReady && !combatActive && !popupSeen &&
                        nearExit && (targetsCleared || forceExit)) {
                        g_hitTimer = 0.f;
                        Seh_SendRequestGeneratedMineExit(g_targetMapPt);
                        g_gmExitFiredThisSession.store(true);
                        NoteGMExitSent();
                        MineDebug::Log("exit: GMExit SENT to tile=(%d,%d) "
                                       "(retry until DoShowAnimation)",
                                       g_targetMapPt.x, g_targetMapPt.y);

                    }
                    return;
                }

                if (g_breakStartTile.x != g_targetMapPt.x ||
                    g_breakStartTile.y != g_targetMapPt.y) {
                    g_breakStartTile = g_targetMapPt;
                    MineProgress mp0{};
                    g_breakStartGemsBroken = ReadMineProgress(mp0) ? mp0.gemstonesBroken : -1;
                }

                Vector2i playerTileNow = PathRenderer::WorldToTile(
                    here.x, here.y + tileW * 0.5f, tileW);
                int dxTgt = std::abs(playerTileNow.x - g_targetMapPt.x);
                int dyTgt = std::abs(playerTileNow.y - g_targetMapPt.y);
                int chebTgt = (dxTgt > dyTgt) ? dxTgt : dyTgt;
                bool inHitRange = (chebTgt <= HIT_REACH);

                if (!combatActive && inHitRange && g_hitTimer >= g_hitInterval) {
                    g_hitTimer = 0.f;
                    Vector2i hitTile = HitBlockPreMineOrSingle(g_targetMapPt, here, tileW);
                    NoteBreakHit(hitTile);
                    g_breaks++;
                }

                bool serverBroke = false;
                if (g_targetKind == TargetKind::Gemstone && g_breakStartGemsBroken >= 0) {
                    MineProgress mp1{};
                    if (ReadMineProgress(mp1) &&
                        mp1.gemstonesBroken > g_breakStartGemsBroken) {
                        serverBroke = true;
                    }
                }
                int btNow = ReadBlockType(g_targetMapPt);
                bool localCleared = (btNow == 0);
                bool stuck        = (g_breakingHits >= GEM_STUCK_HITS);

                bool advance = serverBroke || localCleared || stuck;
                if (advance) {

                    const char* kindStr = "?";
                    switch (g_targetKind) {
                    case TargetKind::Gemstone:    kindStr = "Gemstone";    break;
                    case TargetKind::Nugget:      kindStr = "Nugget";      break;
                    case TargetKind::Crystal:     kindStr = "Crystal";     break;
                    case TargetKind::Pot:         kindStr = "Pot";         break;
                    default: break;
                    }
                    if (stuck) {
                        Diagnostics::StatStuck();
                        Diagnostics::Ev("break_stuck", {
                            {"kind", kindStr},
                            {"target", { g_targetMapPt.x, g_targetMapPt.y }},
                            {"hits", g_breakingHits}
                        });
                    } else {
                        Diagnostics::StatBreak(kindStr, g_breakingHits);
                        Diagnostics::Ev(serverBroke ? "break_confirmed_server" : "break_confirmed_local", {
                            {"kind", kindStr},
                            {"target", { g_targetMapPt.x, g_targetMapPt.y }},
                            {"hits", g_breakingHits}
                        });
                    }

                    MarkOpened(g_targetMapPt);
                    ClearBreaking();
                    g_breakStartTile = { -9999, -9999 };
                    g_breakStartGemsBroken = -1;

                    if (g_targetKind != TargetKind::Crystal) {
                        bool wasGemTgt = (g_targetKind == TargetKind::Gemstone);
                        StartPostBreakHoldFor(wasGemTgt ? POST_BREAK_HOLD_GEM_MS
                                                        : POST_BREAK_HOLD_MS);
                        StartLinger(g_targetMapPt);
                        Diagnostics::Ev("linger_started", {
                            {"tile", { g_targetMapPt.x, g_targetMapPt.y }},
                            {"holdMs", wasGemTgt ? POST_BREAK_HOLD_GEM_MS
                                                 : POST_BREAK_HOLD_MS}
                        });
                    }
                    if (!stuck) g_collects++;
                    g_hasTarget = false; g_targetKind = TargetKind::None;
                }

                return;
            }
        }

        if (g_pathIdx < (int)g_pathTiles.size()) {
            Vector2i nextTile = g_pathTiles[g_pathIdx];

            if (!IsTilePassableLive(nextTile)) {

                {
                    long long key = PackTile(nextTile.x, nextTile.y);
                    std::lock_guard<std::mutex> lkR(g_restTilesMutex);
                    if (g_pendingRestTiles.find(key) != g_pendingRestTiles.end()
                        && !RestHoldActive())
                    {

                        int btBelow = ReadBlockType({ nextTile.x, nextTile.y - 1 });
                        bool floorSolid = (btBelow > 0
                            && !PathRenderer::PF_IsHardcodedPassable(btBelow));
                        if (!floorSolid) {
                            g_pendingRestTiles.erase(key);
                            g_consumedRestTiles.insert(key);
                            Diagnostics::Ev("rest_pocket_dropped", {
                                {"tile",   { nextTile.x, nextTile.y }},
                                {"reason", "no_solid_below"},
                                {"btBelow", btBelow}
                            });
                            goto rest_pocket_skip;
                        }
                        g_pendingRestTiles.erase(key);
                        g_consumedRestTiles.insert(key);
                        g_restHoldUntil = std::chrono::steady_clock::now()
                                          + std::chrono::milliseconds(REST_HOLD_MS);
                        Diagnostics::Ev("rest_pocket_hold", {
                            {"tile", { nextTile.x, nextTile.y }},
                            {"ms",   REST_HOLD_MS},
                            {"trigger", "pre_break_rest"}
                        });
                        if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                            PathFinder_ClearInputs();

                        if (oSetVelocity && g_LocalPlayer)
                            QueueSetVelocity(0.f, 0.f, 0.f);
                        return;
                    }
                    rest_pocket_skip: ;
                }
                if (!combatActive && g_hitTimer >= g_hitInterval) {
                    g_hitTimer = 0.f;
                    Vector2i hitTile = HitBlockPreMineOrSingle(nextTile, here, tileW);
                    NoteBreakHit(hitTile);

                    if (g_breakingStartGems < 0) {
                        MineProgress mp0{};
                        if (ReadMineProgress(mp0)) g_breakingStartGems = mp0.gemstonesBroken;
                    }
                    g_breaks++;
                }

                if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                    PathFinder_ClearInputs();
                return;
            } else {

                if (g_breakingTile.x == nextTile.x && g_breakingTile.y == nextTile.y) {

                    int clearedBT = g_breakingBlockType;
                    int hits      = g_breakingHits;
                    bool wasGem      = (clearedBT >= 3995 && clearedBT <= 4003);
                    bool wasNugget   = IsNugget((World::BlockType)clearedBT);
                    bool wasCrystal  = IsLightCrystal((World::BlockType)clearedBT);
                    bool wasPot      = IsPot((World::BlockType)clearedBT);
                    bool isMineBlock = wasGem || wasNugget || wasCrystal || wasPot;
                    if (isMineBlock) {
                        const char* k = wasGem ? "Gemstone"
                                       : wasNugget ? "Nugget"
                                       : wasCrystal ? "Crystal" : "Pot";

                        bool serverConfirmed = false;
                        if (wasGem && g_breakingStartGems >= 0) {
                            MineProgress mpc{};
                            if (ReadMineProgress(mpc) &&
                                mpc.gemstonesBroken > g_breakingStartGems)
                                serverConfirmed = true;
                        }
                        Diagnostics::StatBreak(k, hits);
                        Diagnostics::Ev(serverConfirmed ? "break_confirmed_server"
                                                        : "break_confirmed_local", {
                            {"kind",   k},
                            {"target", { nextTile.x, nextTile.y }},
                            {"hits",   hits},
                            {"viaPath", true},
                            {"blockType", clearedBT}
                        });

                        if (!wasCrystal && !LingerActive()) {
                            StartPostBreakHoldFor(wasGem ? POST_BREAK_HOLD_GEM_MS
                                                         : POST_BREAK_HOLD_MS);
                            StartLinger(nextTile);
                            Diagnostics::Ev("linger_started", {
                                {"tile", { nextTile.x, nextTile.y }},
                                {"viaPath", true},
                                {"serverConfirmed", serverConfirmed},
                                {"holdMs", wasGem ? POST_BREAK_HOLD_GEM_MS
                                                  : POST_BREAK_HOLD_MS}
                            });
                        } else if (!wasCrystal && LingerActive()) {
                            Diagnostics::Ev("linger_skipped_in_flight", {
                                {"newTile", { nextTile.x, nextTile.y }},
                                {"existingTile", { g_lingerTile.x, g_lingerTile.y }}
                            });
                        }
                        g_collects++;
                    }
                    ClearBreaking();

                    Vector2i playerTile = PathRenderer::WorldToTile(
                        here.x, here.y + tileW * 0.5f, tileW);
                    if (nextTile.y > playerTile.y) {
                        PathFinder_PulseJumpDown();
                        g_lastUpPulse = std::chrono::steady_clock::now();
                    }
                }
            }
        }

        if (!g_autoMove || !oSetPosition) {
            MineDebug::Log("move skip: autoMove=%d oSetPosition=%p",
                           (int)g_autoMove, (void*)oSetPosition);
            return;
        }
        if (!PathRenderer::g_originCached) {
            MineDebug::Log("move skip: origin not cached");
            return;
        }
        if (g_pathIdx >= (int)g_pathTiles.size()) {
            MineDebug::Log("move skip: pathIdx=%d >= pathLen=%d",
                           g_pathIdx, (int)g_pathTiles.size());
            return;
        }

        Vector3 wpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
        float wdx = wpc.x - hereBody.x;
        float wdy = wpc.y - hereBody.y;
        float wdist = sqrtf(wdx*wdx + wdy*wdy);

        _mdbg.pathLen      = (int)g_pathTiles.size();
        _mdbg.pathIdx      = g_pathIdx;
        _mdbg.waypointTile = (g_pathIdx < _mdbg.pathLen) ? g_pathTiles[g_pathIdx]
                                                         : Vector2i{ 0, 0 };
        _mdbg.waypointPos  = wpc;
        _mdbg.wdist        = wdist;
        MineDebug::SetSnap(_mdbg);

        if (g_moveMethod == PathRenderer::MoveMethod::InputDrive) {
            const float DEAD = 0.04f;
            bool wantLeft  = (wdx < -DEAD);
            bool wantRight = (wdx >  DEAD);
            bool wantJump  = (wdy >  DEAD);
            PathFinder_WriteInputs(wantLeft, wantRight, wantJump);

            if (wantJump) {
                auto now2 = std::chrono::steady_clock::now();
                auto sinceMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now2 - g_lastUpPulse).count();
                if (sinceMs >= UP_PULSE_MS) {
                    PathFinder_PulseJumpDown();
                    g_lastUpPulse = now2;
                }
            }

            if (wdist >= 0.001f && oSetVelocity) {
                float speed = g_moveSpeed * g_flightScale;
                float vx = (wdx / wdist) * speed;
                float vy = (wdy / wdist) * speed;

                if (wantJump && vy < speed * 0.85f) vy = speed * 0.85f;
                QueueSetVelocity(vx, vy, 0.f);
            }
        } else if (g_moveMethod == PathRenderer::MoveMethod::Portal) {

            PathRenderer::TryPortalStep(g_pathTiles, g_pathIdx,
                                        g_portalIntervalMs, g_lastPortalSec);
        } else if (g_moveMethod == PathRenderer::MoveMethod::Teleport) {

            PathRenderer::TpApplyAutoMine();

            static auto s_wiggleStarted   = std::chrono::steady_clock::now();
            static auto s_wiggleLast      = std::chrono::steady_clock::now()
                                            - std::chrono::seconds(10);
            static bool s_wiggleAtAdj     = false;
            static bool s_wiggleActive    = false;
            static Vector2i s_wiggleAnchor= { -9999, -9999 };
            static int s_wiggleDirIdx     = 0;
            const int kWiggleIntervalMs   = 70;
            const int kWiggleMaxMs        = 2500;
            const int kWiggleMinMs        = 350;

            Vector2i playerTileNow = PathRenderer::WorldToTile(
                here.x, here.y + tileW * 0.5f, tileW);

            bool isCollectibleKind =
                (g_targetKind == TargetKind::Nugget ||
                 g_targetKind == TargetKind::Collectable ||
                 g_targetKind == TargetKind::Gemstone);
            bool atCollectTarget = g_hasTarget && isCollectibleKind &&
                playerTileNow.x == g_targetMapPt.x &&
                playerTileNow.y == g_targetMapPt.y;
            bool atLingerTile = LingerActive() &&
                playerTileNow.x == g_lingerTile.x &&
                playerTileNow.y == g_lingerTile.y;

            if (!s_wiggleActive && (atCollectTarget || atLingerTile)) {
                s_wiggleActive  = true;
                s_wiggleAnchor  = atCollectTarget ? g_targetMapPt : g_lingerTile;
                s_wiggleAtAdj   = false;
                s_wiggleDirIdx  = 0;
                s_wiggleStarted = std::chrono::steady_clock::now();
                s_wiggleLast    = s_wiggleStarted
                                  - std::chrono::milliseconds(kWiggleIntervalMs);
                MineDebug::Log("fast: WIGGLE ENGAGE at (%d,%d) reason=%s tgtKind=%d",
                               s_wiggleAnchor.x, s_wiggleAnchor.y,
                               atCollectTarget ? "on_target" : "linger",
                               (int)g_targetKind);
            }

            if (s_wiggleActive) {
                auto nowW = std::chrono::steady_clock::now();
                auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                    nowW - s_wiggleStarted).count();

                bool anchorStillOwned = false;
                if (g_hasTarget && isCollectibleKind &&
                    g_targetMapPt.x == s_wiggleAnchor.x &&
                    g_targetMapPt.y == s_wiggleAnchor.y) anchorStillOwned = true;
                if (LingerActive() &&
                    g_lingerTile.x == s_wiggleAnchor.x &&
                    g_lingerTile.y == s_wiggleAnchor.y) anchorStillOwned = true;

                bool minHeld   = (elapsedMs >= kWiggleMinMs);
                bool maxOver   = (elapsedMs >= kWiggleMaxMs);
                bool collected = (!anchorStillOwned && minHeld);

                if (collected || maxOver) {
                    MineDebug::Log("fast: WIGGLE DISENGAGE at (%d,%d) "
                                   "elapsed=%lldms anchorOwned=%d "
                                   "%s linger=%d hasTgt=%d tgtTile=(%d,%d)",
                                   s_wiggleAnchor.x, s_wiggleAnchor.y,
                                   (long long)elapsedMs, (int)anchorStillOwned,
                                   maxOver ? "(timeout)" : "(collected)",
                                   (int)LingerActive(), (int)g_hasTarget,
                                   g_targetMapPt.x, g_targetMapPt.y);
                    s_wiggleActive = false;
                    s_wiggleAnchor = { -9999, -9999 };
                    s_wiggleAtAdj  = false;

                } else {
                    auto sinceHopMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                        nowW - s_wiggleLast).count();
                    if (sinceHopMs >= kWiggleIntervalMs) {
                        s_wiggleLast = nowW;

                        Vector2i hopTarget = s_wiggleAnchor;
                        if (!s_wiggleAtAdj) {

                            const Vector2i dirs[4] = {
                                {1,0}, {-1,0}, {0,1}, {0,-1}
                            };
                            bool found = false;
                            if (g_WorldController && g_off_WCWorld) {
                                void* world = *(void**)((uintptr_t)g_WorldController
                                                                    + g_off_WCWorld);
                                if (world) {
                                    auto bli = PathRenderer::PF_ReadBlockLayer(world);
                                    if (bli.ok) {
                                        for (int i = 0; i < 4 && !found; ++i) {
                                            int idx = (s_wiggleDirIdx + i) % 4;
                                            Vector2i t = {
                                                s_wiggleAnchor.x + dirs[idx].x,
                                                s_wiggleAnchor.y + dirs[idx].y
                                            };
                                            int bt = PathRenderer::PF_BlockTypeAtTile(
                                                bli.layer, t.x, t.y, bli.w, bli.h);
                                            if (PathRenderer::PF_IsTilePassable_Tile(
                                                    bt, t.x, t.y, false, false,
                                                    PathRenderer::g_walkInstakill))
                                            {
                                                hopTarget = t;
                                                s_wiggleDirIdx = (idx + 1) % 4;
                                                found = true;
                                            }
                                        }
                                    }
                                }
                            }
                            s_wiggleAtAdj = found;
                        } else {
                            s_wiggleAtAdj = false;
                            hopTarget     = s_wiggleAnchor;
                        }

                        if (oSetPosition) {
                            Vector3 wpc = PathRenderer::TileToWorldCenter(
                                hopTarget.x, hopTarget.y, here.z, tileW);
                            Vector3 feet = { wpc.x, wpc.y - bodyOff, wpc.z };
                            oSetPosition(lt, feet);
                            if (PathRenderer::oGetKukouriCamInst &&
                                PathRenderer::oForceUpdatePos)
                            {
                                void* cam = PathRenderer::oGetKukouriCamInst();
                                if (cam) PathRenderer::oForceUpdatePos(cam);
                            }
                            if (PathRenderer::g_tp_zeroVelocity && oSetVelocity)
                                QueueSetVelocity(0.f, 0.f, 0.f);
                        }

                        MineDebug::Log("fast: WIGGLE %s -> (%d,%d) anchor=(%d,%d) "
                                       "elapsed=%lldms",
                                       s_wiggleAtAdj ? "out" : "back",
                                       hopTarget.x, hopTarget.y,
                                       s_wiggleAnchor.x, s_wiggleAnchor.y,
                                       (long long)elapsedMs);
                    }

                    _mdbg.fastDecision = "wiggle";
                    MineDebug::SetSnap(_mdbg);
                    return;
                }
            }

            {
                const float kMaxHopDist = tileW * 1.5f;
                if (wdist > kMaxHopDist) {
                    if (PathRenderer::g_tp_zeroVelocity && oSetVelocity)
                        QueueSetVelocity(0.f, 0.f, 0.f);
                    _mdbg.fastDecision = "wdist_skip";
                    MineDebug::SetSnap(_mdbg);
                    MineDebug::Log("fast: wdist=%.3f > max=%.3f (idx=%d wp=(%d,%d)) -> skip hop",
                                   wdist, kMaxHopDist, g_pathIdx,
                                   _mdbg.waypointTile.x, _mdbg.waypointTile.y);
                    return;
                }
            }

            double nowSec = (double)ImGui::GetTime();
            double dtTick = (double)g_portalIntervalMs / 1000.0;
            bool cooldownReady = ((nowSec - g_lastPortalSec) >= dtTick);
            _mdbg.cooldownReady = cooldownReady;
            if (!cooldownReady) {
                if (PathRenderer::g_tp_zeroVelocity && oSetVelocity)
                    QueueSetVelocity(0.f, 0.f, 0.f);
                _mdbg.fastDecision = "wait_cooldown";
                MineDebug::SetSnap(_mdbg);
            } else {
                int last = (int)g_pathTiles.size() - 1;
                if (last < 0) {
                    _mdbg.fastDecision = "no_path";
                    MineDebug::SetSnap(_mdbg);
                    MineDebug::Log("fast: no path");
                    return;
                }
                if (g_pathIdx > last) g_pathIdx = last;
                if (g_pathIdx < 0)    g_pathIdx = 0;

                bool nextPassable = true;
                int  nextBT       = -1;
                Vector2i nextTile = g_pathTiles[g_pathIdx];
                if (g_WorldController && g_off_WCWorld) {
                    void* world = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
                    if (world) {
                        auto bli = PathRenderer::PF_ReadBlockLayer(world);
                        if (bli.ok) {
                            nextBT = PathRenderer::PF_BlockTypeAtTile(bli.layer,
                                nextTile.x, nextTile.y, bli.w, bli.h);
                            nextPassable = PathRenderer::PF_IsTilePassable_Tile(
                                    nextBT, nextTile.x, nextTile.y,
                                     false,  false,
                                    PathRenderer::g_walkInstakill);
                        }
                    }
                }
                _mdbg.nextTilePassable = nextPassable;
                if (!nextPassable) {

                    if (PathRenderer::g_tp_zeroVelocity && oSetVelocity)
                        QueueSetVelocity(0.f, 0.f, 0.f);
                    _mdbg.fastDecision = "wait_passable";
                    MineDebug::SetSnap(_mdbg);
                    MineDebug::Log("fast: next tile (%d,%d) blocked bt=%d -> wait",
                                   nextTile.x, nextTile.y, nextBT);
                    return;
                }

                g_lastPortalSec = nowSec;

                Vector3 jumpWpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
                Vector3 newFeet = { jumpWpc.x, jumpWpc.y - bodyOff, jumpWpc.z };
                oSetPosition(lt, newFeet);
                if (PathRenderer::oGetKukouriCamInst && PathRenderer::oForceUpdatePos) {
                    void* cam = PathRenderer::oGetKukouriCamInst();
                    if (cam) PathRenderer::oForceUpdatePos(cam);
                }
                if (PathRenderer::g_tp_zeroVelocity && oSetVelocity)
                    QueueSetVelocity(0.f, 0.f, 0.f);
                if (PathRenderer::g_tp_throttleMovePackets &&
                    PathRenderer::oSendPlayerPosition0_orig)
                {
                    PathRenderer::g_tp_allowOnePacket.store(true);
                    PathRenderer::SehInvokeSendPos0(PathRenderer::oSendPlayerPosition0_orig);
                    PathRenderer::g_tp_allowOnePacket.store(false);
                }
                _mdbg.fastDecision = "hop";
                MineDebug::SetSnap(_mdbg);
                MineDebug::Log("fast: HOP idx=%d -> (%d,%d) wpos=(%.3f,%.3f) wdist=%.3f",
                               g_pathIdx, nextTile.x, nextTile.y,
                               jumpWpc.x, jumpWpc.y, wdist);
            }
        }
    }

    inline void DrawPathOverlay(ImDrawList* dl, Vector3 here,
                                Vector2 playerS, bool pOk)
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
                                + t * 0.05f + 0.55f;
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
                                + t * 0.05f + 0.55f;
                    dl->AddLine(pts[i-1].p, pts[i].p,
                                hsv(hue, 0.4f, 1.f, 0.9f), 2.f);
                }
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
                float hue = d / fmaxf(totalLen, 1.f) * 0.55f + t * 0.15f + 0.55f;
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

        static bool s_lastInMine = false;
        bool nowIn = IsInMineWorld();
        if (nowIn && !s_lastInMine) {
            g_resumeAfter = std::chrono::steady_clock::now() + std::chrono::seconds(2);
            std::lock_guard<std::mutex> lk(g_pathMutex);
            g_pathTiles.clear(); g_pathOffsets.clear();
            g_pathIdx = 0; g_pathLastGoal = { -9999, -9999 };
            g_hasTarget = false; g_targetKind = TargetKind::None;
            {
                std::lock_guard<std::mutex> lk2(g_recentlyOpenedMutex);
                g_recentlyOpened.clear();
            }
            {
                std::lock_guard<std::mutex> lk2(g_collectedIdsMutex);
                g_collectedIds.clear();
            }
            ClearRestTilesAll();

            if (g_enabled) {
                Diagnostics::EndRun("world_transition");
                Diagnostics::StartRun("AutoMine", GetCurrentWorldName());
                Diagnostics::Ev("world_entered", { {"world", GetCurrentWorldName()} });
            }
        } else if (!nowIn && s_lastInMine) {

            Diagnostics::EndRun("left_mine_world");
        }
        s_lastInMine = nowIn;
    }

    inline void Render() {
        if (g_isTransitioning.load(std::memory_order_acquire)) return;
        HandleResetIfPending();
        if (!g_enabled) return;

        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        if (dt <= 0.f || dt > 0.25f) dt = 0.016f;

        DWORD faultCode = 0;
        __try { Tick(dt); }
        __except (faultCode = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
            g_enabled  = false;
            g_hadFault = true;
            sprintf_s(g_faultMsg, "[AutoMine] tick fault 0x%08lX — bot disabled.",
                faultCode);

            std::cout << "[AutoMine] !!! tick AV 0x" << std::hex << faultCode
                      << std::dec << " — bot DISABLED.\n"
                      << "[AutoMine] If 0xC0000005 (AV), check that velocity "
                      << "writes route through QueueSetVelocity (line ~3173) "
                      << "and not direct oSetVelocity. Direct calls AV on "
                      << "render thread via ObscuredVector3 lazy-init.\n";
        }

        if (g_isTransitioning.load(std::memory_order_acquire)) return;
        if (!g_showPath || !g_hasTarget || !g_IsInWorld || !g_LocalPlayer) return;
        if (!g_WorldController) return;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return;

        void* lt = nullptr; Vector3 here = {}; bool readOk = false;
        __try {
            lt = PathRenderer::oGetTransform(g_LocalPlayer);
            if (lt) { here = PathRenderer::oGetPosition(lt); readOk = true; }
        } __except (EXCEPTION_EXECUTE_HANDLER) { return; }
        if (!readOk) return;

        Vector2 playerS, targetS;
        bool pOk = PathRenderer::NavWorldToScreen(here, playerS);
        bool tOk = PathRenderer::NavWorldToScreen(g_targetWorldPos, targetS);

        ImDrawList* dl = ImGui::GetForegroundDrawList();
        DrawPathOverlay(dl, here, playerS, pOk);

        if (tOk) {
            ImU32 ringCol = IM_COL32(255, 200, 80, 255);
            switch (g_targetKind) {
                case TargetKind::Gemstone:    ringCol = IM_COL32(120, 220, 255, 255); break;
                case TargetKind::Crystal:     ringCol = IM_COL32(220, 120, 255, 255); break;
                case TargetKind::Pot:         ringCol = IM_COL32(255, 180, 100, 255); break;
                case TargetKind::Collectable: ringCol = IM_COL32(120, 255, 180, 255); break;
                case TargetKind::Exit:        ringCol = IM_COL32(80,  255, 80,  255); break;
                default: break;
            }

            dl->AddCircle(ImVec2(targetS.x, targetS.y), 10.f, ringCol, 16, 2.f);
        }

        if (g_hasTarget && g_breakFromRange &&
            g_targetKind != TargetKind::Collectable &&
            g_targetKind != TargetKind::Exit)
        {
            const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                                  ? PathRenderer::g_tileSize.x : 0.32f;
            Vector3 hitC = PathRenderer::TileToWorldCenter(
                g_targetMapPt.x, g_targetMapPt.y, here.z, tileW);
            Vector2 hitS;
            if (PathRenderer::NavWorldToScreen(hitC, hitS)) {
                float ortho = g_cachedCamOrtho.load(std::memory_order_relaxed);
                float pxPerUnit = (ortho > 0.f) ? (ImGui::GetIO().DisplaySize.y / (ortho * 2.f)) : 32.f;
                float halfPx = (tileW * 0.5f) * pxPerUnit;
                ImVec2 a(hitS.x - halfPx, hitS.y - halfPx);
                ImVec2 b(hitS.x + halfPx, hitS.y + halfPx);
                dl->AddRect(a, b, IM_COL32(255, 230, 100, 200), 0.f, 0, 2.f);

                if (tOk) {
                    dl->AddLine(ImVec2(targetS.x, targetS.y),
                                ImVec2(hitS.x, hitS.y),
                                IM_COL32(255, 230, 100, 140), 1.5f);
                }
            }
        }

        auto sinceHit = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - g_breakingLastHit).count();
        if (g_breakingTile.x != -9999 && sinceHit < 1500) {
            const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                                  ? PathRenderer::g_tileSize.x : 0.32f;
            Vector3 c = PathRenderer::TileToWorldCenter(
                g_breakingTile.x, g_breakingTile.y, here.z, tileW);
            Vector2 cs;
            if (PathRenderer::NavWorldToScreen(c, cs)) {
                float tt = (float)ImGui::GetTime();
                float pulse = 0.5f + 0.5f * sinf(tt * 6.0f);
                float ortho = g_cachedCamOrtho.load(std::memory_order_relaxed);
                float pxPerUnit = (ortho > 0.f) ? (ImGui::GetIO().DisplaySize.y / (ortho * 2.f)) : 32.f;
                float halfPx = (tileW * 0.5f) * pxPerUnit;
                ImVec2 a(cs.x - halfPx, cs.y - halfPx);
                ImVec2 b(cs.x + halfPx, cs.y + halfPx);
                int alphaFill = (int)(60 + 80 * pulse);
                int alphaEdge = (int)(180 + 75 * pulse);
                dl->AddRectFilled(a, b, IM_COL32(255, 90, 30, alphaFill), 2.f);
                dl->AddRect(a, b, IM_COL32(255, 200, 60, alphaEdge), 2.f, 0, 2.5f);

                float ext = halfPx * 0.6f;
                dl->AddLine(ImVec2(cs.x - ext, cs.y), ImVec2(cs.x + ext, cs.y),
                            IM_COL32(255, 255, 255, alphaEdge), 2.f);
                dl->AddLine(ImVec2(cs.x, cs.y - ext), ImVec2(cs.x, cs.y + ext),
                            IM_COL32(255, 255, 255, alphaEdge), 2.f);

                if (g_breakingHits > 0) {
                    char buf[16]; snprintf(buf, sizeof(buf), "x%d", g_breakingHits);
                    dl->AddText(ImVec2(cs.x + halfPx + 4, cs.y - halfPx),
                                IM_COL32(0, 0, 0, 220), buf);
                    dl->AddText(ImVec2(cs.x + halfPx + 3, cs.y - halfPx - 1),
                                IM_COL32(255, 220, 80, 255), buf);
                }
            }
        }

        if (LingerActive() && g_lingerTile.x != -9999) {
            const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                                  ? PathRenderer::g_tileSize.x : 0.32f;
            Vector3 c = PathRenderer::TileToWorldCenter(
                g_lingerTile.x, g_lingerTile.y, here.z, tileW);
            Vector2 cs;
            if (PathRenderer::NavWorldToScreen(c, cs)) {
                float tt = (float)ImGui::GetTime();
                float pulse = 0.5f + 0.5f * sinf(tt * 4.0f);
                int alpha = (int)(120 + 80 * pulse);
                dl->AddCircle(ImVec2(cs.x, cs.y), 12.f,
                              IM_COL32(120, 255, 180, alpha), 16, 2.f);
            }
        }

        if (pOk)
            dl->AddCircleFilled(ImVec2(playerS.x, playerS.y),
                                4.f, IM_COL32(255, 200, 60, 220));
    }

    inline void RenderTab() {
        ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.2f, 1.0f), "Auto Mine");
        ImGui::TextWrapped("Mines through walls to reach gems / nuggets / crystals / pots / drops. Weighted A* + TSP-NN.");

        bool inMine = IsInMineWorld();
        ImVec4 statCol = inMine ? ImVec4(0.3f, 1.0f, 0.4f, 1.0f)
                                : ImVec4(1.0f, 0.4f, 0.3f, 1.0f);
        ImGui::TextColored(statCol, inMine
            ? "Status: IN mine world"
            : "Status: NOT in mine world");
        ImGui::Spacing();

        if (g_hadFault) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", g_faultMsg);
            if (ImGui::Button("Acknowledge & re-enable")) {
                g_hadFault = false; g_enabled = false;
                memset(g_faultMsg, 0, sizeof(g_faultMsg));
            }
            ImGui::Spacing();
        }

        ImGui::Checkbox("Auto-Mine##AM", &g_enabled);
        ImGui::BeginDisabled(!g_enabled);
        {
            ImGui::Indent(12.f);

            g_mineOnly = true;
            ImGui::Checkbox("Auto move##AM",         &g_autoMove);
            ImGui::Checkbox("Draw route##AM",        &g_showPath);
            ImGui::SliderFloat("##flight_scale_AM",  &g_flightScale, 0.5f, 5.0f, "%.2fx flight speed");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextUnformatted("Targets");
            ImGui::Checkbox("Gemstones##AM",          &g_targetGemstones);
            ImGui::Checkbox("Nuggets##AM",            &g_targetNuggets);
            ImGui::Checkbox("Light crystals##AM",     &g_targetCrystals);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextWrapped("Crystals only replenish the light bar (no profit). "
                    "Two independent modes — detour (abandon tour to chase a crystal) "
                    "and break-if-close (HB nearby crystals opportunistically while "
                    "doing other targets). Set the detour threshold low and the "
                    "break-if-close threshold high to look natural to the server.");
                ImGui::EndTooltip();
            }
            ImGui::Indent(20.f);

            g_crystalDetour                = false;
            g_lightCrystalThreshold        = 0.0f;
            g_crystalBreakIfClose          = true;
            g_crystalBreakIfCloseThreshold = 0.95f;

            float lcFrac = GetLightCrystalFraction();
            if (lcFrac >= 0.f) {
                bool willTarget = ShouldTargetCrystals() && g_targetCrystals;
                ImVec4 col = willTarget
                    ? ImVec4(1.f, 0.7f, 0.2f, 1.f)
                    : ImVec4(0.6f, 0.85f, 0.6f, 1.f);
                ImGui::TextColored(col, "Light bar: %.0f%%  %s",
                    lcFrac * 100.f,
                    willTarget ? "(targeting crystals)" : "(skipping)");
            } else {
                ImGui::TextDisabled("Light bar: unavailable");
            }
            ImGui::Unindent(20.f);
            ImGui::Checkbox("Pots##AM",               &g_targetPots);
            ImGui::Checkbox("Floor collectables##AM", &g_targetCollect);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextWrapped("Item drops on the floor (gems from broken blocks, enemy drops, etc). Walks player onto tile to auto-collect.");
                ImGui::EndTooltip();
            }
            ImGui::Checkbox("AI Enemy##AM", &g_targetAI);
            ImGui::SameLine();
            ImGui::BeginDisabled(!g_targetAI);
            ImGui::Checkbox("Close by only##AM", &g_targetAINearbyOnly);
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextWrapped("AI Enemy: master toggle for engaging mobs.\n"
                                   "  ON + 'Close by only' ON: only kill mobs within combat range "
                                   "while doing other tasks (current behavior).\n"
                                   "  ON + 'Close by only' OFF: also pathfind to mobs and kill them "
                                   "as primary targets — useful in higher-level mines where mobs drop loot.\n"
                                   "  OFF: ignore mobs entirely.");
                ImGui::EndTooltip();
            }
            ImGui::Checkbox("Exit portal (last)##AM", &g_targetExit);

            ImGui::Spacing();
            ImGui::Checkbox("Anti-Sleep##AM", &g_antiSleep);
            ImGui::SameLine();
            ImGui::Checkbox("Anti-AFK##AM",   &g_antiAfk);

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::Checkbox("Auto-repair pickaxe##AM", &g_autoRepairPickaxe);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextWrapped("Reads equipped pickaxe durability via PlayerData::GetItemDurability. "
                    "When it hits 0 AND a ConsumableMiningPickaxeRepairKit (4161) is in inventory, "
                    "sends GFLi + MiningPickaxeRepairing packets. 4s cooldown.");
                ImGui::EndTooltip();
            }

            int curBT = -1;
            int curDur = ReadPickaxeDurability(&curBT);
            if (curDur >= 0) {
                int kitCount = EstimateRepairKitCount();
                ImVec4 col = (curDur == 0)
                    ? (kitCount > 0 ? ImVec4(1.f, 0.7f, 0.2f, 1.f) : ImVec4(1.f, 0.3f, 0.3f, 1.f))
                    : ImVec4(0.7f, 1.f, 0.7f, 1.f);
                ImGui::TextColored(col, "Pickaxe %d  durability: %d  kits: %d",
                    curBT, curDur, kitCount);
            } else {
                ImGui::TextDisabled("Pickaxe: unavailable (not equipped or hook not resolved)");
            }

            ImGui::TextUnformatted("Combat And Movement");
            ImGui::Checkbox("Combat (HitAI)##AM",       &g_combatEnabled);
            ImGui::SliderInt("Combat range (tiles)##AM",   &g_combatTileRange, 1, 6);
            ImGui::SliderFloat("Combat cadence (s)##AM",   &g_combatCadence,   0.10f, 1.5f, "%.2f s");
            ImGui::TextDisabled("Skips Gassy (unkillable). Default 2 tiles = 3-tile incl player. 0.33s cadence avoids HitAI flooding.");
            ImGui::SliderFloat("HB cadence (s)##AM",   &g_hitInterval, 0.05f, 1.0f, "%.2f s");

            g_stopRange = 0.10f;

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.7f, 0.85f, 1.f, 1.f), "Pathfinder (AutoMine)");

            ImGui::TextColored(ImVec4(0.8f,1.0f,0.8f,1.f),
                g_moveMethod == PathRenderer::MoveMethod::Portal
                    ? "Movement Method: Teleport (Portal)"
                    : "Movement Method: Smooth");
            ImGui::SliderFloat("Move Speed##AM-PF",   &g_moveSpeed,   0.5f, 20.0f, "%.1f u/s");
            if (g_moveSpeed > 4.5f)
                ImGui::TextColored(ImVec4(1.f, 0.6f, 0.3f, 1.f),
                    "WARNING: > 1.5x base — may trigger fly-detect kick");
            ImGui::SliderFloat("Stop Range##AM-PF",   &g_pfStopRange, 0.0f, 1.0f, "%.2f u");
            ImGui::SliderFloat("Body Offset##AM-PF",  &g_bodyOffsetFrac, 0.0f, 1.0f, "%.2f x tile");

            ImGui::Spacing();
            ImGui::TextDisabled("Mining weights (lower = bot prefers route)");
            ImGui::Checkbox("Prefer staircase pattern##AM-W", &g_weights.preferStaircase);
            ImGui::BeginDisabled(!g_weights.preferStaircase);
            ImGui::SliderInt("Up-break penalty##AM-W",        &g_weights.upPenaltySolid,    0, 100);
            ImGui::SliderInt("Unsupported up-break extra##AM-W", &g_weights.unsupportedExtra, 0, 200);
            ImGui::EndDisabled();
            ImGui::TextDisabled("Staircase: discourage straight-up tunnels by adding cost when breaking upward without solid floor below.");

            ImGui::Spacing();
            ImGui::Checkbox("Airborne check##AM-W", &g_weights.airborneCheck);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextWrapped("Reject paths with > N consecutive tiles lacking solid floor "
                    "below. Forces A* to route via tiles WITH floor (rest pockets / stairs) "
                    "when long airborne stretches would otherwise occur. Goal tile is "
                    "always reachable regardless of airborne count.");
                ImGui::EndTooltip();
            }
            ImGui::BeginDisabled(!g_weights.airborneCheck);
            ImGui::SliderInt("Max airborne tiles##AM-W", &g_weights.maxAirborne, 1, 20);
            ImGui::EndDisabled();

            ImGui::Spacing();
            ImGui::Checkbox("Glow halo##AM-PF",      &g_pathGlow);
            ImGui::Checkbox("Flow particles##AM-PF", &g_pathFlow);

            ImGui::Spacing();
            ImGui::Separator();
            int rt = (int)g_runtime;
            ImGui::Text("Runtime: %02d:%02d:%02d", rt / 3600, (rt % 3600) / 60, rt % 60);
            ImGui::Text("Breaks: %d   Collects: %d", g_breaks, g_collects);

            MineProgress mp{};
            if (ReadMineProgress(mp)) {
                ImGui::TextColored(ImVec4(0.7f, 1.f, 0.7f, 1.f),
                    "Server: %d/%d enemies   %d/%d gems   %d/%d nuggets",
                    mp.enemiesKilled,    mp.totalEnemies,
                    mp.gemstonesBroken,  mp.totalGemstones,
                    mp.nuggetsCollected, mp.totalNuggets);
            }

            if (g_hasTarget) {
                const char* k = "?";
                switch (g_targetKind) {
                case TargetKind::Gemstone:    k = "Gemstone";    break;
                case TargetKind::Nugget:      k = "Nugget";      break;
                case TargetKind::Crystal:     k = "Crystal";     break;
                case TargetKind::Pot:         k = "Pot";         break;
                case TargetKind::Collectable: k = "Collectable"; break;
                case TargetKind::Exit:        k = "Exit";        break;
                default: break;
                }
                ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.4f, 1.0f),
                    "Target: %s @ (%.2f, %.2f)", k,
                    g_targetWorldPos.x, g_targetWorldPos.y);
            } else {
                ImGui::TextDisabled("No target");
            }

            if (ImGui::Button("Reset state##AM")) ResetPhase();

            AutoLoop::RenderUI();

            ImGui::Unindent(12.f);
        }
        ImGui::EndDisabled();
    }

    typedef void (__fastcall* tGenMineResults_SetValues)(
        void* self, bool failure, int gatheredXP, int exitXPBonus,
        void* drops, void* exitDrops, void* positiveButtonAction);
    typedef void (__fastcall* tGenMineResults_Done)(void* self);

    inline tGenMineResults_SetValues g_orig_GenMineResults_SetValues = nullptr;
    inline tGenMineResults_Done      o_GenMineResults_Done           = nullptr;

    inline void* g_mineResultsUI_self = nullptr;
    inline std::atomic<bool> g_mineResultsPopupOpen{ false };
    inline std::chrono::steady_clock::time_point g_mineResultsPopupOpenedAt;

    inline std::atomic<long long> g_lastGMExitSentMs{ -1 };

    inline std::atomic<bool> g_gmExitFiredThisSession{ false };

    inline std::atomic<bool> g_doShowAnimationSeen{ false };

    inline long long NowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    inline long long MsSinceGMExit() {
        long long sent = g_lastGMExitSentMs.load();
        if (sent < 0) return -1;
        return NowMs() - sent;
    }

    inline void NoteGMExitSent() {
        g_lastGMExitSentMs.store(NowMs());
        std::cout << "[AutoMine][ExitDiag] +0   GMExit sent (T0).\n";
    }

    inline void __fastcall hk_GenMineResults_SetValues(
        void* self, bool failure, int gatheredXP, int exitXPBonus,
        void* drops, void* exitDrops, void* positiveButtonAction)
    {
        g_mineResultsUI_self = self;
        g_mineResultsPopupOpen.store(true);
        g_mineResultsPopupOpenedAt = std::chrono::steady_clock::now();
        std::cout << "[AutoMine][ExitDiag] +" << MsSinceGMExit()
                  << "ms GeneratedMineResultsUI.SetValues(failure="
                  << (failure ? 1 : 0)
                  << ", gatheredXP=" << gatheredXP
                  << ", exitXPBonus=" << exitXPBonus
                  << ", positiveAction=" << (positiveButtonAction ? "set" : "null")
                  << ", self=" << self << ")\n";
        if (g_orig_GenMineResults_SetValues) {
            g_orig_GenMineResults_SetValues(self, failure, gatheredXP, exitXPBonus,
                                             drops, exitDrops, positiveButtonAction);
        }
    }

    typedef void* (__fastcall* tGenMineResults_DoShowAnimation)(void* self, bool doInstant);
    inline tGenMineResults_DoShowAnimation g_orig_GenMineResults_DoShowAnimation = nullptr;
    inline void* __fastcall hk_GenMineResults_DoShowAnimation(void* self, bool doInstant) {

        g_doShowAnimationSeen.store(true);
        std::cout << "[AutoMine][ExitDiag] +" << MsSinceGMExit()
                  << "ms GeneratedMineResultsUI.DoShowAnimation(doInstant="
                  << (doInstant ? 1 : 0) << ", self=" << self << ")\n";

        ScheduleMineExitWarp(0, "DoShowAnimation");
        if (g_orig_GenMineResults_DoShowAnimation)
            return g_orig_GenMineResults_DoShowAnimation(self, doInstant);
        return nullptr;
    }

    typedef void (__fastcall* tHandleGenMineSWD)(void* thisPtr, void* currentMessage);
    inline tHandleGenMineSWD g_orig_HandleGenMineSWD = nullptr;

    inline bool _PlayerNearExitTile_3x3() {
        if (!g_LocalPlayer || !MineScheduler::g_haveExit) return false;
        Vector2i pmp;
        if (!::GetPlayerMapPoint(pmp)) return false;
        Vector2i e = MineScheduler::g_exitTile;
        int dx = pmp.x - e.x; if (dx < 0) dx = -dx;
        int dy = pmp.y - e.y; if (dy < 0) dy = -dy;
        int cheb = (dx > dy) ? dx : dy;
        return cheb <= 2;
    }

    inline void __fastcall hk_HandleGenMineSWD(void* thisPtr, void* currentMessage) {

        if (_PlayerNearExitTile_3x3()) {
            std::cout << "[AutoMine][ExitDiag] +" << MsSinceGMExit()
                      << "ms HandleGeneratedMineSpecialWorldDataPacket fired "
                      << "(this=" << thisPtr << ", msg=" << currentMessage << ")\n";
        }
        if (g_orig_HandleGenMineSWD)
            g_orig_HandleGenMineSWD(thisPtr, currentMessage);
    }

    inline std::atomic<bool> g_pendingMineExitWarp{ false };

    inline std::chrono::steady_clock::time_point g_pendingMineExitWarpAt;

    inline void ScheduleMineExitWarp(int msFromNow, const char* reason) {
        if (!AutoMine::g_enabled) return;
        auto fireAt = std::chrono::steady_clock::now() +
                      std::chrono::milliseconds(msFromNow);

        if (g_pendingMineExitWarp.load() &&
            fireAt > g_pendingMineExitWarpAt)
        {
            return;
        }
        g_pendingMineExitWarpAt = fireAt;
        g_pendingMineExitWarp.store(true);
        std::cout << "[AutoMine] mine-exit warp scheduled (+"
                  << msFromNow << "ms, reason=" << reason << ")\n";
    }

    inline void OnMineCompleteSWD() {

        std::cout << "[AutoMine][ExitDiag] +" << MsSinceGMExit()
                  << "ms SWDt=4 + SWDe=4 packet seen (diagnostic only).\n";
    }

    inline void DiagInspectIncoming(const nlohmann::json& m) {
        long long t = MsSinceGMExit();
        if (t < 0 || t > 5000) return;
        if (!m.is_object()) return;
        std::string id = m.value("ID", "");

        if (id == "PMv" || id == "PAiP" || id == "PAoP" ||
            id == "ASI" || id == "TPpos") return;

        std::string fields;
        for (auto it = m.begin(); it != m.end(); ++it) {
            if (it.key() == "ID") continue;
            if (!fields.empty()) fields += ",";
            fields += it.key();
            if (it.value().is_number_integer())
                fields += "=" + std::to_string(it.value().get<long long>());
        }
        std::cout << "[AutoMine][ExitDiag] +" << t
                  << "ms incoming ID=" << (id.empty() ? "<none>" : id.c_str())
                  << " fields=" << fields << "\n";
    }

    inline void InstallMineResultsHook() {
        DoHook(GetMethodPtr("GeneratedMineResultsUI", "SetGeneratedMineResultsUIValues"),
               &AutoMine::hk_GenMineResults_SetValues,
               (void**)&AutoMine::g_orig_GenMineResults_SetValues,
               "GeneratedMineResultsUI::SetGeneratedMineResultsUIValues");
        DoFuncPtr(GetMethodPtr("GeneratedMineResultsUI", "PositiveButtonPressed"),
                  AutoMine::o_GenMineResults_Done,
                  "GeneratedMineResultsUI::PositiveButtonPressed");

        DoHook(GetMethodPtr("GeneratedMineResultsUI", "DoShowAnimation"),
               &AutoMine::hk_GenMineResults_DoShowAnimation,
               (void**)&AutoMine::g_orig_GenMineResults_DoShowAnimation,
               "GeneratedMineResultsUI::DoShowAnimation");
        DoHook(GetMethodPtr("NetworkClient", "HandleGeneratedMineSpecialWorldDataPacket"),
               &AutoMine::hk_HandleGenMineSWD,
               (void**)&AutoMine::g_orig_HandleGenMineSWD,
               "NetworkClient::HandleGeneratedMineSpecialWorldDataPacket");
    }

    namespace AutoLoop {
        inline bool g_enabled         = false;

        inline bool g_autoCombine     = true;
        inline bool g_autoEnter       = false;

        struct PortalCoord {
            float worldX, worldY;
            int tileX, tileY;
            int level;
        };
        static constexpr PortalCoord kPortals[5] = {
            { 11.52f, 9.60f, 18, 15, 1 },
            { 12.16f, 9.60f, 19, 15, 2 },
            { 12.80f, 9.60f, 20, 15, 3 },
            { 13.44f, 9.60f, 21, 15, 4 },
            { 14.08f, 9.60f, 22, 15, 5 },
        };

        inline std::vector<Vector2i> g_livePortalTiles;
        inline bool g_livePortalsScanned = false;

        inline int ScanPortalsInWorld() {
            g_livePortalTiles.clear();
            if (!g_WorldController || !g_off_WCWorld) return 0;
            void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
            PathRenderer::PF_BlockLayerInfo bli =
                PathRenderer::PF_ReadBlockLayer(worldObj);
            if (!bli.ok) return 0;
            for (int x = 0; x < bli.w; ++x) {
                for (int y = 0; y < bli.h; ++y) {
                    int bt = PathRenderer::PF_BlockTypeAtTile(
                        bli.layer, x, y, bli.w, bli.h);
                    if (bt == 3965) g_livePortalTiles.push_back({ x, y });
                }
            }
            std::sort(g_livePortalTiles.begin(), g_livePortalTiles.end(),
                [](const Vector2i& a, const Vector2i& b) {
                    if (a.x != b.x) return a.x < b.x;
                    return a.y < b.y;
                });
            g_livePortalsScanned = true;
            std::cout << "[AutoLoop] scanned " << g_livePortalTiles.size()
                      << " PortalPixelMines tiles:";
            for (auto& t : g_livePortalTiles)
                std::cout << " (" << t.x << "," << t.y << ")";
            std::cout << "\n";
            return (int)g_livePortalTiles.size();
        }

        inline bool GetPortalTile(int level, Vector2i& out) {
            if (level < 1 || level > 5) return false;
            int idx = level - 1;
            if (g_livePortalsScanned && idx < (int)g_livePortalTiles.size()) {
                out = g_livePortalTiles[idx];
                return true;
            }
            out = { kPortals[idx].tileX, kPortals[idx].tileY };
            return true;
        }

        static constexpr int kMineKeyBlockType[5] = { 0, 3969, 3970, 3971, 3972 };
        static constexpr int kMineKeyItemType   = 7;

        static constexpr int LEVEL_REQ[5] = { 0, 10, 20, 40, 60 };

        inline bool g_levelEnabled[5] = { true, false, false, false, false };

        inline int  g_combineCadenceMs = 200;
        inline std::chrono::steady_clock::time_point g_lastCombine =
            std::chrono::steady_clock::now() - std::chrono::seconds(10);

        inline int g_combinesSent = 0;

        struct NuggetTier {
            int blockType;
            int itemType;
            int targetKeyLevel;
            const char* name;
        };
        static constexpr int NUGGET_ITEM_TYPE = 7;
        static constexpr NuggetTier kTiers[] = {
            { 4162, NUGGET_ITEM_TYPE, 0, "Dark"     },
            { 4157, NUGGET_ITEM_TYPE, 5, "Platinum" },
            { 4156, NUGGET_ITEM_TYPE, 4, "Gold"     },
            { 4155, NUGGET_ITEM_TYPE, 3, "Silver"   },
            { 4154, NUGGET_ITEM_TYPE, 2, "Bronze"   },
        };

        static constexpr int kDarkstoneBlockType = 4170;

        inline bool IsInPixelMines() {
            std::string n = GetCachedWorldName();
            if (n.empty()) n = GetCurrentWorldName();
            auto upper = n;
            for (auto& c : upper) c = (char)toupper((unsigned char)c);
            if (upper == "PIXELMINES") return true;
            if (upper.rfind("PIXELMINES", 0) == 0) return true;
            return false;
        }

        inline float DistanceToPortalTiles(int level) {
            if (level < 1 || level > 5) return -1.f;
            Vector2i pt;
            if (!GetPortalTile(level, pt)) return -1.f;
            if (!g_LocalPlayer || !PathRenderer::oGetTransform || !PathRenderer::oGetPosition)
                return -1.f;
            void* tr = PathRenderer::oGetTransform(g_LocalPlayer);
            if (!tr) return -1.f;
            Vector3 wp = PathRenderer::oGetPosition(tr);
            float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                ? PathRenderer::g_tileSize.x : 0.64f;
            Vector3 portalCenter = PathRenderer::TileToWorldCenter(
                pt.x, pt.y, wp.z, tileW);
            float dx = wp.x - portalCenter.x;
            float dy = wp.y - portalCenter.y;
            float wuDist = sqrtf(dx*dx + dy*dy);
            return wuDist / tileW;
        }

        inline bool TierWantedByEnabledLevels(int targetKeyLevel) {
            int idx = targetKeyLevel - 1;
            if (idx < 0 || idx > 4) return false;
            return g_levelEnabled[idx];
        }

        inline void OnCombineSent(int srcBT, int srcIT);

        inline bool SafeConvertItems(PlayerData_InventoryKey ik) {
            if (!oConvertItems) return false;
            ReplyDispatch::NoteSendCI(ik.blockType, ik.itemType);
            bool ok = false;
            __try { oConvertItems(ik); ok = true; }
            __except (EXCEPTION_EXECUTE_HANDLER) { ok = false; }
            if (ok) {

                OnCombineSent(ik.blockType, ik.itemType);
            }
            return ok;
        }

        inline bool ApplyCombineDeltaLocal(int srcBT, int srcIT, int dstBT, int dstIT);

        inline void OnCIReplyDelivered(int srcBT, int srcIT, const nlohmann::json& bson) {
            int dstBT = 0, dstIT = 0;

            if (bson.is_object() && bson.contains("HFhK") && bson["HFhK"].is_number_integer()) {
                int packed = bson["HFhK"].get<int>();
                dstBT = packed & 0xFFFFFF;
                dstIT = (packed >> 24) & 0xFF;
            }
            if (dstBT <= 0) {
                for (const auto& t : kTiers) {
                    if (t.blockType != srcBT || t.itemType != srcIT) continue;
                    if (t.targetKeyLevel == 0)
                        dstBT = kDarkstoneBlockType;
                    else if (t.targetKeyLevel >= 2 && t.targetKeyLevel <= 5)
                        dstBT = kMineKeyBlockType[t.targetKeyLevel - 1];
                    dstIT = kMineKeyItemType;
                    break;
                }
            }
            Diagnostics::Ev("ci_reply_confirmed", {
                {"srcBT", srcBT},
                {"srcIT", srcIT},
                {"dstBT", dstBT},
                {"dstIT", dstIT}
            });
            if (dstBT <= 0) return;
            std::cout << "[ReplyMirror] CI confirmed src=(" << srcBT << ","
                      << srcIT << ") -> dst=(" << dstBT << "," << dstIT
                      << ") — applying -100/+1 delta\n";
            ApplyCombineDeltaLocal(srcBT, srcIT, dstBT, dstIT);
        }

        inline bool ApplyCombineDeltaLocal(int srcBT, int srcIT, int dstBT, int dstIT) {
            if (!g_LocalPlayer) return false;
            void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
            if (!pd) return false;

            if (oRemoveItemsFromInventory) {
                PlayerData_InventoryKey srcKey{ srcBT, srcIT };
                __try { oRemoveItemsFromInventory(pd, srcKey, 100); }
                __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
            }
            if (oAddItemToInventory) {

                void* dataClass = nullptr;
                if (oSpawnDataClassForEnum) {
                    __try { dataClass = oSpawnDataClassForEnum(dstBT); }
                    __except (EXCEPTION_EXECUTE_HANDLER) { dataClass = nullptr; }
                }
                __try { oAddItemToInventory(pd, dstBT, dstIT, (short)1, dataClass); }
                __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
            }

            if (oForceRefreshInventory && g_InventoryControl) {
                __try { oForceRefreshInventory(g_InventoryControl); }
                __except (EXCEPTION_EXECUTE_HANDLER) {  }
            }
            return true;
        }

        inline void OnCombineSent(int srcBT, int srcIT) {
            for (const auto& t : kTiers) {
                if (t.blockType != srcBT || t.itemType != srcIT) continue;
                int dstBT;
                if (t.targetKeyLevel == 0) {
                    dstBT = kDarkstoneBlockType;
                } else if (t.targetKeyLevel >= 2 && t.targetKeyLevel <= 5) {
                    dstBT = kMineKeyBlockType[t.targetKeyLevel - 1];
                } else {
                    std::cout << "[AutoLoop] OnCombineSent: tier '" << t.name
                              << "' has unsupported targetKeyLevel="
                              << t.targetKeyLevel << ", skipping.\n";
                    return;
                }
                int dstIT = kMineKeyItemType;
                std::cout << "[AutoLoop] OnCombineSent: tier='" << t.name
                          << "' src=(" << srcBT << "," << srcIT
                          << ") expected-dst=(" << dstBT << "," << dstIT
                          << ") — mirror deferred to OnCIReplyDelivered.\n";
                return;
            }
        }

        inline void RequestInventoryRefresh() {
            ::RequestInventoryRefresh();
            Diagnostics::Ev("autoloop_inv_refresh_skipped", {
                {"reason", "helper_auto_sync"}
            });
        }

        inline void TickAutoCombine() {
            if (!g_enabled || !g_autoCombine) return;
            if (!IsInPixelMines()) return;
            if (!oConvertItems) return;

            auto now = std::chrono::steady_clock::now();
            auto sinceMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                              now - g_lastCombine).count();
            if (sinceMs < g_combineCadenceMs) return;

            for (const auto& t : kTiers) {
                int qty = GetInventoryCount(t.blockType, t.itemType);
                if (qty < 100) continue;
                PlayerData_InventoryKey ik{ t.blockType, t.itemType };
                if (SafeConvertItems(ik)) {
                    g_combinesSent++;
                    g_lastCombine = now;
                    RequestInventoryRefresh();
                    Diagnostics::Ev("autoloop_combine", {
                        {"tier", t.name},
                        {"into", t.targetKeyLevel},
                        {"qty",  qty}
                    });
                }
                return;
            }
        }

        inline int PickEntryLevel() {

            for (int i = 4; i >= 0; --i) {
                int lvl = i + 1;
                if (!g_levelEnabled[i]) continue;
                if (lvl == 1) return 1;
                int keyCount = GetInventoryCount(kMineKeyBlockType[i], kMineKeyItemType);
                if (keyCount > 0) return lvl;
            }
            return 0;
        }

        static constexpr float ENTRY_PROXIMITY_TILES = 1.0f;

        inline void SendEntrySequence(int level) {
            if (level < 1 || level > 5) return;
            if (!oGoFromPortal || !g_LocalPlayer) {
                Diagnostics::Ev("autoloop_entry_blocked", {
                    {"level", level},
                    {"reason", "gofromportal_unresolved"}
                });
                return;
            }
            Vector2i portalTile;
            if (!GetPortalTile(level, portalTile)) {
                Diagnostics::Ev("autoloop_entry_blocked", {
                    {"level", level},
                    {"reason", "portal_tile_unknown"}
                });
                return;
            }
            float distTiles = DistanceToPortalTiles(level);
            if (distTiles < 0.f || distTiles > ENTRY_PROXIMITY_TILES) {
                Diagnostics::Ev("autoloop_entry_blocked", {
                    {"level", level},
                    {"distTiles", distTiles},
                    {"reason", "too_far_from_portal"}
                });
                return;
            }

            int lvlIdx = level - 1;
            ::SendWorldLoadArgs(lvlIdx);

            Vector2i pt = portalTile;
            std::thread([pt]() {
                Il2cppThreadScope _gc;
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                Seh_GoFromPortal(g_LocalPlayer, pt);
            }).detach();

            Diagnostics::Ev("autoloop_entry_sent", {
                {"level",     level},
                {"eID",       lvlIdx},
                {"portalX",   portalTile.x},
                {"portalY",   portalTile.y},
                {"method",    "SetAndSendWorldLoadArgs+oGoFromPortal"}
            });
        }

        inline std::chrono::steady_clock::time_point g_lastEntryAttempt =
            std::chrono::steady_clock::now() - std::chrono::seconds(60);
        inline std::chrono::steady_clock::time_point g_pixelMinesEnteredAt =
            std::chrono::steady_clock::now();
        inline bool g_pixelMinesSettleValid = false;
        inline bool g_wasInPixelMines = false;
        inline int g_entriesSent = 0;
        static constexpr int PIXELMINES_SETTLE_MS = 4000;
        static constexpr int WALK_TIMEOUT_MS = 15000;

        inline bool g_walking = false;
        inline int  g_walkLevel = 0;
        inline std::chrono::steady_clock::time_point g_walkStartedAt;
        inline bool g_savedAutoMove = false;
        inline bool g_savedAutoMoveValid = false;

        inline PathRenderer::MoveMethod g_savedMoveMethod = PathRenderer::MoveMethod::InputDrive;
        inline bool g_savedMoveMethodValid = false;
        inline bool g_saved_pf_tp_throttle      = false;
        inline bool g_saved_pf_tp_zeroVel       = false;
        inline bool g_saved_pf_tp_forceGrounded = false;
        inline bool g_saved_pf_tp_suppressGrav  = false;
        inline bool g_savedTpRowValid           = false;
        inline float g_saved_pf_moveSpeed       = 0.f;
        inline bool  g_savedMoveSpeedValid      = false;

        inline void StopWalk() {
            if (g_savedAutoMoveValid) {
                PathRenderer::g_autoMove = g_savedAutoMove;
                g_savedAutoMoveValid = false;
            }

            if (g_savedMoveMethodValid) {
                PathRenderer::g_moveMethod = g_savedMoveMethod;
                g_savedMoveMethodValid = false;
            }
            if (g_savedTpRowValid) {
                PathRenderer::g_pf_tp_throttleMovePackets = g_saved_pf_tp_throttle;
                PathRenderer::g_pf_tp_zeroVelocity        = g_saved_pf_tp_zeroVel;
                PathRenderer::g_pf_tp_forceGrounded       = g_saved_pf_tp_forceGrounded;
                PathRenderer::g_pf_tp_suppressGravity     = g_saved_pf_tp_suppressGrav;
                g_savedTpRowValid = false;
            }
            if (g_savedMoveSpeedValid) {
                PathRenderer::g_moveSpeed = g_saved_pf_moveSpeed;
                g_savedMoveSpeedValid = false;
            }
            PathRenderer::g_isNavigating  = false;
            PathRenderer::g_isTeleporting = false;
            PathRenderer::g_hasTarget     = false;
            {
                std::lock_guard<std::mutex> lk(PathRenderer::g_pathMutex);
                PathRenderer::g_pathTiles.clear();
                PathRenderer::g_pathOffsets.clear();
                PathRenderer::g_pathIdx = 0;
            }
            PathFinder_RestoreGravity();
            PathFinder_ClearInputs();
            g_walking = false;
            g_walkLevel = 0;
        }

        inline bool StartWalk(int level) {
            if (level < 1 || level > 5) return false;
            if (!g_LocalPlayer || !g_WorldController) return false;
            if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return false;

            void* tr = PathRenderer::oGetTransform(g_LocalPlayer);
            if (!tr) return false;
            Vector3 pos = PathRenderer::oGetPosition(tr);
            float tileW = PathRenderer::g_tileSize.x > 0 ? PathRenderer::g_tileSize.x : 0.64f;

            Vector2i startTile = PathRenderer::WorldToTile(pos.x, pos.y + tileW * 0.5f, tileW);

            if (!g_livePortalsScanned) ScanPortalsInWorld();
            Vector2i goalTile;
            if (!GetPortalTile(level, goalTile)) return false;

            void* worldObj = g_off_WCWorld
                ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld)
                : nullptr;
            if (!worldObj) return false;

            PathRenderer::PathResult res =
                PathRenderer::BuildPath(g_WorldController, worldObj, startTile, goalTile);
            if (res.status != PathRenderer::PathStatus::Success) {
                std::cout << "[AutoLoop] BuildPath FAIL L" << level
                          << " status=" << PathRenderer::PathStatusText(res.status)
                          << " start=(" << startTile.x << "," << startTile.y << ")"
                          << " goal=(" << goalTile.x << "," << goalTile.y << ")\n";

                PathRenderer::PF_BlockLayerInfo bli =
                    PathRenderer::PF_ReadBlockLayer(worldObj);
                if (bli.ok) {
                    std::cout << "[AutoLoop] tile dump (5x5 around goal):\n";
                    for (int dy = 2; dy >= -2; --dy) {
                        std::cout << "  y=" << (goalTile.y + dy) << ":";
                        for (int dx = -2; dx <= 2; ++dx) {
                            int bt = PathRenderer::PF_BlockTypeAtTile(
                                bli.layer, goalTile.x + dx, goalTile.y + dy,
                                bli.w, bli.h);
                            std::cout << " " << bt;
                        }
                        std::cout << "\n";
                    }
                    int sBt = PathRenderer::PF_BlockTypeAtTile(
                        bli.layer, startTile.x, startTile.y, bli.w, bli.h);
                    std::cout << "[AutoLoop] start tile bt=" << sBt << "\n";
                }
                Diagnostics::Ev("autoloop_walk_pathfail", {
                    {"level",  level},
                    {"status", (int)res.status},
                    {"start",  { startTile.x, startTile.y }},
                    {"goal",   { goalTile.x,  goalTile.y  }}
                });
                return false;
            }

            {
                std::lock_guard<std::mutex> lk(PathRenderer::g_pathMutex);
                PathRenderer::g_pathTiles   = std::move(res.tiles);
                PathRenderer::g_pathOffsets = std::move(res.offsets);
                PathRenderer::g_pathStatus  = res.status;
                PathRenderer::g_pathIdx     = 0;
            }
            PathRenderer::g_navTarget =
                PathRenderer::TileToWorldCenter(goalTile.x, goalTile.y, pos.z, tileW);
            PathRenderer::g_hasTarget     = true;
            PathRenderer::g_isNavigating  = true;
            PathRenderer::g_isTeleporting = true;

            if (!g_savedAutoMoveValid) {
                g_savedAutoMove = PathRenderer::g_autoMove;
                g_savedAutoMoveValid = true;
            }
            PathRenderer::g_autoMove = true;

            if (!g_savedMoveMethodValid) {
                g_savedMoveMethod = PathRenderer::g_moveMethod;
                g_savedMoveMethodValid = true;
            }
            PathRenderer::g_moveMethod = g_moveMethod;
            if (!g_savedTpRowValid) {
                g_saved_pf_tp_throttle      = PathRenderer::g_pf_tp_throttleMovePackets;
                g_saved_pf_tp_zeroVel       = PathRenderer::g_pf_tp_zeroVelocity;
                g_saved_pf_tp_forceGrounded = PathRenderer::g_pf_tp_forceGrounded;
                g_saved_pf_tp_suppressGrav  = PathRenderer::g_pf_tp_suppressGravity;
                g_savedTpRowValid           = true;
            }
            PathRenderer::g_pf_tp_throttleMovePackets = PathRenderer::g_am_tp_throttleMovePackets;
            PathRenderer::g_pf_tp_zeroVelocity        = PathRenderer::g_am_tp_zeroVelocity;
            PathRenderer::g_pf_tp_forceGrounded       = PathRenderer::g_am_tp_forceGrounded;
            PathRenderer::g_pf_tp_suppressGravity     = PathRenderer::g_am_tp_suppressGravity;
            if (!g_savedMoveSpeedValid) {
                g_saved_pf_moveSpeed   = PathRenderer::g_moveSpeed;
                g_savedMoveSpeedValid  = true;
            }
            PathRenderer::g_moveSpeed = g_moveSpeed;

            PathFinder_DisableGravity();
            if (g_moveMethod == PathRenderer::MoveMethod::InputDrive)
                PathFinder_PulseJumpDown();

            g_walking = true;
            g_walkLevel = level;
            g_walkStartedAt = std::chrono::steady_clock::now();
            Diagnostics::Ev("autoloop_walk_start", {
                {"level", level},
                {"goal",  { goalTile.x, goalTile.y }},
                {"method","pathfind"}
            });
            return true;
        }

        inline void TickAutoEnter() {

            bool inPM = IsInPixelMines();
            auto now = std::chrono::steady_clock::now();
            if (inPM && !g_wasInPixelMines) {
                g_pixelMinesEnteredAt = now;
                g_pixelMinesSettleValid = true;

                g_livePortalsScanned = false;
                g_livePortalTiles.clear();

                BankBot::g_bankWalkAbandonedAt = now - std::chrono::seconds(120);
                GemSeller::g_walkAbandonedAt   = now - std::chrono::seconds(120);

                g_lastEntryAttempt = now - std::chrono::seconds(60);
            } else if (!inPM) {
                g_pixelMinesSettleValid = false;
                g_livePortalsScanned = false;
            }
            g_wasInPixelMines = inPM;

            if (inPM && g_pixelMinesSettleValid && !g_livePortalsScanned) {
                auto sinceEnter = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - g_pixelMinesEnteredAt).count();
                if (sinceEnter >= PIXELMINES_SETTLE_MS) {
                    ScanPortalsInWorld();
                }
            }

            if (!g_enabled) {
                if (g_walking) StopWalk();
                return;
            }
            if (!inPM) {
                if (g_walking) StopWalk();
                return;
            }

            if (!g_pixelMinesSettleValid) return;
            auto sinceEntered = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  now - g_pixelMinesEnteredAt).count();
            if (sinceEntered < PIXELMINES_SETTLE_MS) return;

            bool combining = false;
            if (oConvertItems && g_autoCombine) {
                for (const auto& t : kTiers) {
                    int qty = GetInventoryCount(t.blockType, t.itemType);
                    if (qty >= 100) { combining = true; break; }
                }
            }
            if (combining) {
                if (g_walking) StopWalk();
                return;
            }

            if (BankBot::AutoLoopTick()) {
                if (g_walking) StopWalk();
                return;
            }

            if (GemSeller::AutoLoopTick()) {
                if (g_walking) StopWalk();
                return;
            }

            if (!g_autoEnter && g_walking) StopWalk();
            if (!g_autoEnter) return;

            if (std::chrono::duration_cast<std::chrono::seconds>(
                    now - g_lastEntryAttempt).count() < 30) return;

            int lvl = g_walking ? g_walkLevel : PickEntryLevel();
            if (lvl <= 0) {
                if (g_walking) StopWalk();
                return;
            }

            float distTiles = DistanceToPortalTiles(lvl);

            if (distTiles >= 0.f && distTiles <= ENTRY_PROXIMITY_TILES) {
                if (g_walking) StopWalk();
                g_lastEntryAttempt = now;
                g_entriesSent++;
                SendEntrySequence(lvl);
                return;
            }

            if (!g_walking) {
                StartWalk(lvl);
                return;
            }

            auto walkMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now - g_walkStartedAt).count();
            if (walkMs > WALK_TIMEOUT_MS) {
                Diagnostics::Ev("autoloop_walk_timeout", {
                    {"level", lvl},
                    {"distTiles", distTiles}
                });
                StopWalk();
            }
        }

        inline bool g_autoRejoin       = true;
        inline int  g_rejoinDelayMs    = 3000;
        inline int  g_rejoinsSent      = 0;
        inline std::chrono::steady_clock::time_point g_inMenuSince =
            std::chrono::steady_clock::now();
        inline bool g_inMenuSinceValid = false;
        inline std::chrono::steady_clock::time_point g_lastRejoinAttempt =
            std::chrono::steady_clock::now() - std::chrono::seconds(60);
        static constexpr int REJOIN_COOLDOWN_MS = 15000;

        static inline void _SafeJoinDynamicWorld(Il2CppString* w, Il2CppString* e) {
            __try { oJoinDynamicWorld(w, e, true, nullptr); }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }

        inline std::chrono::steady_clock::time_point g_lastPlayerUpdateAt =
            std::chrono::steady_clock::now() - std::chrono::seconds(60);

        static constexpr int MENU_CONFIRM_MS = 2000;

        inline void StampPlayerUpdateHeartbeat() {
            g_lastPlayerUpdateAt = std::chrono::steady_clock::now();
        }

        inline bool IsInMenuSafe() {

            if (!g_LocalPlayer || !g_WorldController) return true;

            auto now = std::chrono::steady_clock::now();
            auto sinceMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                              now - g_lastPlayerUpdateAt).count();
            if (sinceMs >= MENU_CONFIRM_MS) return true;
            return false;
        }

        inline void TickAutoRejoin() {

            if (!AutoMine::g_enabled || !g_enabled || !g_autoRejoin) {
                g_inMenuSinceValid = false;
                return;
            }
            auto now = std::chrono::steady_clock::now();

            if (!IsInMenuSafe()) {
                g_inMenuSinceValid = false;
                return;
            }

            if (!g_inMenuSinceValid) {
                g_inMenuSince      = now;
                g_inMenuSinceValid = true;
                Diagnostics::Ev("autoloop_menu_detected", {});
                return;
            }

            auto inMenuMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                              now - g_inMenuSince).count();
            if (inMenuMs < g_rejoinDelayMs) return;

            auto sinceLast = std::chrono::duration_cast<std::chrono::milliseconds>(
                               now - g_lastRejoinAttempt).count();
            if (sinceLast < REJOIN_COOLDOWN_MS) return;

            if (!oJoinDynamicWorld || !oIl2CppStringNew) {
                Diagnostics::Ev("autoloop_rejoin_blocked", {
                    {"reason", "joindynamic_unresolved"}
                });
                return;
            }
            Il2CppString* w = oIl2CppStringNew("PIXELMINES");
            Il2CppString* e = oIl2CppStringNew("default");
            if (!w || !e) return;
            _SafeJoinDynamicWorld(w, e);
            g_lastRejoinAttempt = now;
            g_rejoinsSent++;
            Diagnostics::Ev("autoloop_rejoin_sent", {
                {"target",  "PIXELMINES"},
                {"inMenuMs", (long long)inMenuMs},
                {"count",    g_rejoinsSent}
            });

        }

        inline void TickAutoExitWarp() {
            if (!AutoMine::g_pendingMineExitWarp.load()) return;

            if (std::chrono::steady_clock::now() <
                AutoMine::g_pendingMineExitWarpAt) {
                return;
            }
            if (!oJoinDynamicWorld || !oIl2CppStringNew) {
                AutoMine::g_pendingMineExitWarp.store(false);
                return;
            }
            std::cout << "[AutoMine] firing JoinDynamicWorld(PIXELMINES) "
                         "to skip the 10-20s popup wait.\n";
            __try {
                oJoinDynamicWorld(
                    oIl2CppStringNew("PIXELMINES"),
                    oIl2CppStringNew("default"),
                    false, nullptr);
            } __except (EXCEPTION_EXECUTE_HANDLER) {}
            AutoMine::g_pendingMineExitWarp.store(false);

            AutoMine::g_mineResultsPopupOpen.store(false);
            AutoMine::g_mineResultsUI_self = nullptr;
        }

        inline void TickAutoCloseMineResults() {
            if (!AutoMine::g_mineResultsPopupOpen.load()) return;
            auto since = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() -
                AutoMine::g_mineResultsPopupOpenedAt).count();
            if (since < 200) return;
            void* self = AutoMine::g_mineResultsUI_self;
            if (self && AutoMine::o_GenMineResults_Done) {
                __try {
                    AutoMine::o_GenMineResults_Done(self);
                } __except (EXCEPTION_EXECUTE_HANDLER) {}
            }
            AutoMine::g_mineResultsPopupOpen.store(false);
            AutoMine::g_mineResultsUI_self = nullptr;
        }

        inline void Tick() {

            if (!AutoMine::g_enabled) return;
            if (!g_enabled) return;

            TickAutoCombine();
            TickAutoEnter();

            TickAutoExitWarp();
        }

        inline void RenderUI() {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.6f, 1.0f, 1.0f, 1.0f), "Auto-Loop");
            ImGui::Checkbox("Enable Auto-Loop##AM", &g_enabled);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Phase 1: auto-combines nuggets in PIXELMINES.\n"
                                  "Phase 2 (coming): auto-enters chosen portal level.");

            int playerLvl = GetLocalPlayerLevel();
            ImGui::Text("Player level: %d", playerLvl);

            ImGui::Text("Levels to play:");

            for (int i = 0; i < 5; ++i) {
                int lvl = i + 1;
                int req = LEVEL_REQ[i];
                bool gated = (playerLvl < req);

                if (i != 0 && i != 3) ImGui::SameLine();
                ImGui::BeginDisabled(gated);
                char lbl[32];
                if (req > 0) sprintf_s(lbl, "%d (lvl %d+)##AMlvl%d", lvl, req, lvl);
                else         sprintf_s(lbl, "%d##AMlvl%d", lvl, lvl);
                ImGui::Checkbox(lbl, &g_levelEnabled[i]);
                ImGui::EndDisabled();
            }

            ImGui::Checkbox("Auto-Combine nuggets##AM", &g_autoCombine);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("In PIXELMINES, sends ConvertItems for EVERY\n"
                                  "nugget tier (Bronze/Silver/Gold/Plat) with\n"
                                  ">=100 stack. Compresses regardless of which\n"
                                  "levels you've toggled — keeps inventory\n"
                                  "hygiene since players accumulate higher-tier\n"
                                  "nuggets they can't yet use. Throttled by\n"
                                  "combine cadence ms.");
            ImGui::SliderInt("Combine cadence (ms)##AM", &g_combineCadenceMs, 200, 2000, "%d ms");

            ImGui::Checkbox("Auto-Enter portal##AM", &g_autoEnter);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("In PIXELMINES, after combines finish, sends wlA+TTjW+Gw\n"
                                  "to enter the highest enabled level with a key (or level 1).\n"
                                  "Throttled 5s between attempts.");

            ImGui::Checkbox("Auto-Rejoin on disconnect##AM", &g_autoRejoin);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("If dropped to main menu (e.g. random DC),\n"
                                  "auto-call JoinDynamicWorld(PIXELMINES, default)\n"
                                  "after the delay below. 15s cooldown between attempts.");
            ImGui::SliderInt("Menu wait (ms)##AMrejoin", &g_rejoinDelayMs, 500, 15000, "%d ms");
            {
                bool inMenu = IsInMenuSafe();
                if (inMenu && g_inMenuSinceValid) {
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - g_inMenuSince).count();
                    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f),
                        "In menu: %lld / %d ms", (long long)ms, g_rejoinDelayMs);
                } else {
                    ImGui::TextDisabled("In menu: --");
                }
                ImGui::Text("Rejoins sent: %d", g_rejoinsSent);
            }

            BankBot::RenderUI();

            GemSeller::RenderUI();

            ImGui::Text("Entries sent: %d", g_entriesSent);

            if (ImGui::TreeNode("Inventory")) {
                int br = GetInventoryCount(4154, NUGGET_ITEM_TYPE);
                int si = GetInventoryCount(4155, NUGGET_ITEM_TYPE);
                int go = GetInventoryCount(4156, NUGGET_ITEM_TYPE);
                int pl = GetInventoryCount(4157, NUGGET_ITEM_TYPE);
                int da = GetInventoryCount(4162, NUGGET_ITEM_TYPE);
                int k2 = GetInventoryCount(3969, NUGGET_ITEM_TYPE);
                int k3 = GetInventoryCount(3970, NUGGET_ITEM_TYPE);
                int k4 = GetInventoryCount(3971, NUGGET_ITEM_TYPE);
                int k5 = GetInventoryCount(3972, NUGGET_ITEM_TYPE);
                ImGui::Text("Bronze: %d   Silver: %d   Gold: %d   Plat: %d   Dark: %d",
                            br, si, go, pl, da);
                ImGui::Text("Key2: %d   Key3: %d   Key4: %d   Key5: %d",
                            k2, k3, k4, k5);
                ImGui::TreePop();
            }

            ImGui::Text("Combines sent: %d", g_combinesSent);

            std::string cachedWorld = GetCachedWorldName();
            std::string curWorld = GetCurrentWorldName();
            ImGui::Text("World name (live):   \"%s\"", curWorld.c_str());
            ImGui::Text("World name (cached): \"%s\"  match=%s",
                        cachedWorld.c_str(),
                        IsInPixelMines() ? "YES" : "NO");
            if (!IsInPixelMines())
                ImGui::TextColored(ImVec4(0.7f,0.7f,0.7f,1.0f),
                                   "(not in PIXELMINES — actions paused)");

            if (IsInPixelMines()) {
                auto now = std::chrono::steady_clock::now();
                long long settleMs = g_pixelMinesSettleValid
                    ? std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - g_pixelMinesEnteredAt).count()
                    : 0;
                bool settled = g_pixelMinesSettleValid && (settleMs >= PIXELMINES_SETTLE_MS);
                ImGui::TextColored(settled ? ImVec4(0.4f,1.0f,0.4f,1.0f)
                                            : ImVec4(1.0f,0.7f,0.3f,1.0f),
                    "Settle: %lld / %d ms %s",
                    settleMs,
                    PIXELMINES_SETTLE_MS,
                    settled ? "[ready]" : "[wait]");
                ImGui::Text("Portal distances (tiles):");
                for (int i = 0; i < 5; ++i) {
                    float d = DistanceToPortalTiles(i + 1);
                    if (i > 0) ImGui::SameLine();
                    ImVec4 col = (d >= 0 && d <= ENTRY_PROXIMITY_TILES)
                                 ? ImVec4(0.4f, 1.0f, 0.4f, 1.0f)
                                 : ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
                    ImGui::TextColored(col, "L%d=%.1f", i + 1, d);
                }
                ImGui::TextDisabled("Server requires proximity to portal — green = within %.0f tiles.",
                                    ENTRY_PROXIMITY_TILES);
                if (g_walking) {
                    ImGui::TextColored(ImVec4(0.6f,1.0f,0.6f,1.0f),
                        "Walking to L%d portal...", g_walkLevel);
                }
            }
        }
    }

}

#pragma once

#include "main.h"
#include "IL2CPP_Resolver/IL2CPP_Resolver.hpp"
#include <cmath>
#include <mutex>
#include <algorithm>
#include <vector>
#include <queue>
#include <unordered_map>
#include <chrono>
#include <atomic>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "imgui/imgui.h"

namespace Minimap {
    extern bool g_blockTooltip;
    int  Seh_ReadFgID(uintptr_t worldObject, int x, int y);
    void RenderTileTooltip(uintptr_t worldObject, int tileX, int tileY,
                           int fgID, ImVec2 anchor,
                           const char* windowId);
}

namespace PathRenderer {

    typedef void* (__fastcall* tGetMainCamera)();
    typedef Vector3 (__fastcall* tGetPosition)(void* instance);
    typedef void*   (__fastcall* tGetTransform)(void* instance);

    inline tGetMainCamera oGetMainCamera = nullptr;
    inline tGetPosition   oGetPosition   = nullptr;
    inline tGetTransform  oGetTransform  = nullptr;

    typedef bool (__fastcall* tCfgDoesCollide)(int blockType);
    typedef bool (__fastcall* tCfgIsOneWay)   (int blockType);
    typedef bool (__fastcall* tCfgIsInstakill)(int blockType);
    typedef bool (__fastcall* tCfgIsWater)    (int blockType);
    typedef bool (__fastcall* tCfgIsSwimming) (int blockType);
    typedef bool (__fastcall* tCfgIsPlatform) (int blockType);
    typedef bool (__fastcall* tCfgIsHatch)    (int blockType);
    typedef bool (__fastcall* tCfgIsPortal)   (int blockType);
    typedef bool (__fastcall* tCfgDeathByColl)(int blockType);
    typedef bool (__fastcall* tCfgIsDisapBlk) (int blockType);
    typedef bool (__fastcall* tCfgIsBattleBar)(int blockType);
    typedef bool (__fastcall* tCfgIsHot)      (int blockType);

    typedef bool (__fastcall* tCfgCollectableBlocked)(void* world, Vector2i mp);
    inline tCfgDoesCollide oCfgDoesCollide  = nullptr;
    inline tCfgIsOneWay    oCfgIsOneWay     = nullptr;
    inline tCfgIsInstakill oCfgIsInstakill  = nullptr;
    inline tCfgIsWater     oCfgIsWater      = nullptr;
    inline tCfgIsSwimming  oCfgIsSwimming   = nullptr;
    inline tCfgIsPlatform  oCfgIsPlatform   = nullptr;
    inline tCfgIsHatch     oCfgIsHatch      = nullptr;
    inline tCfgIsPortal    oCfgIsPortal     = nullptr;
    inline tCfgDeathByColl oCfgDeathByColl  = nullptr;
    inline tCfgIsDisapBlk  oCfgIsDisapBlk   = nullptr;
    inline tCfgIsBattleBar oCfgIsBattleBar  = nullptr;
    inline tCfgIsHot       oCfgIsHot        = nullptr;

    typedef bool (__fastcall* tIsBattleOn)(void* world, Vector2i mp, void* methodInfo);
    inline tIsBattleOn     oIsBattleOn      = nullptr;

    inline tCfgCollectableBlocked oCfgCollectableBlocked = nullptr;

    inline bool g_avoidWater = false;

    typedef bool (*tExtraPassableCheck)(int tileX, int tileY);
    inline tExtraPassableCheck g_extraPassableCheck = nullptr;

    typedef void* (__fastcall* tGetKukouriCamInst)();
    typedef void  (__fastcall* tForceUpdatePos)(void* instance);
    inline tGetKukouriCamInst oGetKukouriCamInst = nullptr;
    inline tForceUpdatePos    oForceUpdatePos    = nullptr;

    typedef void (__fastcall* tSendPlayerActivateInPortal)(Vector2i mapPoint);
    inline tSendPlayerActivateInPortal oSendPlayerActivateInPortal = nullptr;

    typedef void (__fastcall* tSendPlayerPosition)();
    inline tSendPlayerPosition oSendPlayerPosition = nullptr;

    typedef bool (__fastcall* tPlayerIsGrounded)(void* self);
    typedef void (__fastcall* tPlayerFixedUpdate)(void* self);
    typedef void (__fastcall* tSendPlayerPositionBson)(void* bsonMsg);

    inline tPlayerIsGrounded         oPlayerIsGrounded_orig         = nullptr;
    inline tPlayerFixedUpdate        oPlayerFixedUpdate_orig        = nullptr;
    inline tSendPlayerPosition       oSendPlayerPosition0_orig      = nullptr;
    inline tSendPlayerPositionBson   oSendPlayerPosition1_orig      = nullptr;

    inline Vector2 g_tileSize            = { 0.32f, 0.32f };
    inline bool    g_needsTileSizeRecalc = true;
    inline Vector3 g_worldOrigin         = {};
    inline bool    g_originCached        = false;

    inline bool  g_navEnabled     = false;
    inline float g_stopRange      = 0.18f;

    inline float g_moveSpeed      = 2.0f;
    inline float g_verticalSpeed  = 2.5f;

    inline float g_descentSpeedMul = 1.0f;
    inline float g_flightScale    = 1.0f;
    inline bool  g_showPath       = true;
    inline bool  g_autoMove       = true;

    inline bool g_airborneCheck = true;
    inline int  g_maxAirborne  = 5;

    inline float g_bodyOffsetFrac = 0.4f;

    inline bool  g_pathChamfer       = false;
    inline float g_pathChamferAmount = 0.f;

    inline bool g_walkInstakill = false;

    struct PathOptions {
        bool  airborneCheck  = true;
        int   maxAirborne    = 5;
        bool  chamfer        = false;
        float chamferAmount  = 0.f;
        bool  walkInstakill  = false;
    };

    enum class MoveMethod : int { Teleport = 0, InputDrive = 1, Portal = 2 };
    inline const char* MoveMethodName(MoveMethod m) {
        switch (m) {
            case MoveMethod::Teleport:   return "Fast";
            case MoveMethod::InputDrive: return "Smooth";
            case MoveMethod::Portal:     return "Portal Warp";
        }
        return "?";
    }
    inline MoveMethod g_moveMethod = MoveMethod::InputDrive;

    inline bool  g_tp_throttleMovePackets = false;
    inline bool  g_tp_zeroVelocity        = false;
    inline bool  g_tp_forceGrounded       = false;
    inline bool  g_tp_suppressGravity     = false;

    inline bool  g_pf_tp_throttleMovePackets = false;
    inline bool  g_pf_tp_zeroVelocity        = false;
    inline bool  g_pf_tp_forceGrounded       = false;
    inline bool  g_pf_tp_suppressGravity     = false;

    inline bool  g_am_tp_throttleMovePackets = false;
    inline bool  g_am_tp_zeroVelocity        = false;
    inline bool  g_am_tp_forceGrounded       = false;
    inline bool  g_am_tp_suppressGravity     = false;

    inline bool  g_an_tp_throttleMovePackets = false;
    inline bool  g_an_tp_zeroVelocity        = false;
    inline bool  g_an_tp_forceGrounded       = false;
    inline bool  g_an_tp_suppressGravity     = false;

    inline void TpApplyStandalone() {
        g_tp_throttleMovePackets = g_pf_tp_throttleMovePackets;
        g_tp_zeroVelocity        = g_pf_tp_zeroVelocity;
        g_tp_forceGrounded       = g_pf_tp_forceGrounded;
        g_tp_suppressGravity     = g_pf_tp_suppressGravity;
    }
    inline void TpApplyAutoMine() {
        g_tp_throttleMovePackets = g_am_tp_throttleMovePackets;
        g_tp_zeroVelocity        = g_am_tp_zeroVelocity;
        g_tp_forceGrounded       = g_am_tp_forceGrounded;
        g_tp_suppressGravity     = g_am_tp_suppressGravity;
    }
    inline void TpApplyAutoNether() {
        g_tp_throttleMovePackets = g_an_tp_throttleMovePackets;
        g_tp_zeroVelocity        = g_an_tp_zeroVelocity;
        g_tp_forceGrounded       = g_an_tp_forceGrounded;
        g_tp_suppressGravity     = g_an_tp_suppressGravity;
    }

    inline void TpClearActive() {
        g_tp_throttleMovePackets = false;
        g_tp_zeroVelocity        = false;
        g_tp_forceGrounded       = false;
        g_tp_suppressGravity     = false;
    }
    inline float g_tp_tickIntervalMs      = 100.f;

    inline int   g_tp_tickJitterMs        = 20;
    inline double g_tp_lastSec            = 0.0;

    inline float g_tp_packetCapHz         = 60.f;
    inline std::atomic<long long> g_tp_lastSendNs{ 0 };

    inline std::atomic<bool> g_tp_allowOnePacket{ false };

    inline float g_portalIntervalMs = 0.f;
    inline double g_lastPortalSec   = 0.0;

    inline bool TryPortalStep(const std::vector<Vector2i>& tiles,
                              int pathIdx,
                              float intervalMs,
                              double& lastSec)
    {
        if (!oSendPlayerActivateInPortal) return false;
        if (!g_WorldController || !g_off_WCWorld) return false;
        if (pathIdx < 0 || pathIdx >= (int)tiles.size()) return false;

        double nowSec = (double)ImGui::GetTime();
        if ((nowSec - lastSec) < (intervalMs / 1000.0)) return false;

        Vector2i target = tiles[pathIdx];

        void* world = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (world) {
            constexpr uintptr_t kPlayerStartPositionOff = 0xf0;
            *(Vector2i*)((uintptr_t)world + kPlayerStartPositionOff) = target;
        }

        oSendPlayerActivateInPortal(target);

        if (oSendPlayerPosition) {
            __try { oSendPlayerPosition(); }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }

        lastSec = nowSec;
        return true;
    }

    inline PathOptions StandaloneOptions() {
        return {
            g_airborneCheck,
            g_maxAirborne,
            false,
            0.f,
            g_walkInstakill,
        };
    }

    inline bool NavigateToTile(Vector2i tile);

    inline bool  g_pathGlow     = true;
    inline bool  g_pathFlow     = true;
    inline bool  g_pathPulse    = true;

    inline bool g_debugBlockOverlay = false;

    inline bool g_dbgStatsHud    = false;
    inline bool g_dbgInspect     = false;
    inline bool g_dbgVerboseLog  = false;

    inline long long g_dbgLastBuildMicros = 0;
    inline int       g_dbgLastTileCount   = 0;
    inline Vector2i  g_dbgLastStart       = { 0, 0 };
    inline Vector2i  g_dbgLastGoal        = { 0, 0 };
    inline const char* g_dbgLastSubsystem = "(none)";

    inline bool g_bclhResolved      = false;
    inline bool g_kukouriResolved   = false;

    inline bool              g_isNavigating = false;
    inline Vector3           g_navTarget    = {};
    inline bool              g_hasTarget    = false;
    inline std::vector<Vector2i> g_pathTiles = {};

    inline std::vector<Vector2>  g_pathOffsets = {};
    inline int               g_pathIdx      = 0;
    inline std::mutex        g_pathMutex;

    enum class PathStatus {
        Success,
        NoPath,
        TargetSolid,
        StartSolid,
        OutOfBounds,
        NoWorld
    };
    inline PathStatus g_pathStatus = PathStatus::Success;

    inline const char* PathStatusText(PathStatus s) {
        switch (s) {
            case PathStatus::Success:     return "OK";
            case PathStatus::NoPath:      return "No path (blocked)";
            case PathStatus::TargetSolid: return "Target is solid";
            case PathStatus::StartSolid:  return "Player inside solid";
            case PathStatus::OutOfBounds: return "Out of world bounds";
            case PathStatus::NoWorld:     return "World not loaded";
            default:                      return "Unknown";
        }
    }

    inline bool g_isTeleporting  = false;
    inline bool g_showVisualizer = true;
    inline bool g_initialized    = false;

    inline Vector2i WorldToTile(float wx, float wy, float tileW) {
        float ox = g_originCached ? g_worldOrigin.x : 0.f;
        float oy = g_originCached ? g_worldOrigin.y : 0.f;
        return {
            (int)floorf((wx - ox) / tileW),
            (int)floorf((wy - oy) / tileW)
        };
    }

    inline Vector3 TileToWorldCenter(int tx, int ty, float wz, float tileW) {
        float ox = g_originCached ? g_worldOrigin.x : 0.f;
        float oy = g_originCached ? g_worldOrigin.y : 0.f;
        return {
            ox + (tx + 0.5f) * tileW,
            oy + (ty + 0.5f) * tileW,
            wz
        };
    }

    inline Vector3 PathWaypointWorld(int i, float wz, float tileW) {
        if (i < 0 || i >= (int)g_pathTiles.size()) return Vector3{ 0.f, 0.f, wz };
        Vector2i t = g_pathTiles[i];
        Vector3  c = TileToWorldCenter(t.x, t.y, wz, tileW);
        if (i >= 0 && i < (int)g_pathOffsets.size()) {
            c.x += g_pathOffsets[i].x * tileW;
            c.y += g_pathOffsets[i].y * tileW;
        }
        return c;
    }

    inline bool NavWorldToScreen(const Vector3& wp, Vector2& sp) {
        if (!g_cachedCamValid.load(std::memory_order_acquire)) return false;
        float camX  = g_cachedCamX    .load(std::memory_order_relaxed);
        float camY  = g_cachedCamY    .load(std::memory_order_relaxed);
        float ortho = g_cachedCamOrtho.load(std::memory_order_relaxed);
        if (ortho <= 0.f) return false;
        const ImGuiIO& io = ImGui::GetIO();
        float scale = io.DisplaySize.y / (ortho * 2.f);
        sp.x = (wp.x - camX) *  scale + io.DisplaySize.x * 0.5f;
        sp.y = (wp.y - camY) * -scale + io.DisplaySize.y * 0.5f;
        return true;
    }

    inline Vector2 GreedyDir(Vector2 from, Vector2 to) {
        float dx = to.x - from.x;
        float dy = to.y - from.y;
        if (fabsf(dx) < 0.0001f && fabsf(dy) < 0.0001f) return { 0.f, 0.f };
        if (fabsf(dx) >= fabsf(dy))
            return { (dx > 0.f) ? 1.f : -1.f, 0.f };
        else
            return { 0.f, (dy > 0.f) ? 1.f : -1.f };
    }

    struct PF_LayerBlock { int blockType; char _pad[0x24]; };

    static int PF_BlockTypeAtTile(Il2CppArray* outerArr, int tileX, int tileY,
                                  int worldSizeX, int worldSizeY) {
        if (tileX < 0 || tileX >= worldSizeX) return -1;
        if (tileY < 0 || tileY >= worldSizeY) return -1;
        __try {
            Il2CppArray* col = (Il2CppArray*)outerArr->items[tileX];
            if (!col) return -1;
            return ((PF_LayerBlock*)&col->items[0])[tileY].blockType;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    }

    static const int kExtendedHazardIDs[] = {
        453,
        959,
        1409,
        2345,
        4464,

        957,
        1401,
        2999,
        3007,
        3042,
        3141,
        3153,

        1410,
        2005,
        2344,
        3051,
        3110,
        3210,
        3498,
    };

    inline int g_pfHazardBufferRadius = 2;

    inline bool g_pfStrictTraps = true;

    static inline bool PF_HasHazardousNeighbor(Il2CppArray* worldBlocks,
                                               int tx, int ty,
                                               int worldW, int worldH) {
        int r = g_pfHazardBufferRadius;
        if (r < 1) r = 1;
        if (r > 3) r = 3;
        for (int dy = -r; dy <= r; ++dy) {
            for (int dx = -r; dx <= r; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int nx = tx + dx, ny = ty + dy;
                if (nx < 0 || nx >= worldW || ny < 0 || ny >= worldH) continue;
                int bt = PF_BlockTypeAtTile(worldBlocks, nx, ny, worldW, worldH);
                for (int id : kExtendedHazardIDs) {
                    if (id == bt) return true;
                }
            }
        }
        return false;
    }

    static inline bool PF_IsHardcodedPassable(int bt) {
        switch (bt) {
            case 238: case 319:
            case 656: case 956:

            case 961:

            case 1372: case 1377: case 1383:
            case 1420:
            case 110:
            case 588:
            case 3216: case 3224: case 3284:
            case 3965:
            case 3973:
            case 3974: case 3975: case 3976:
            case 3977: case 3978: case 3979:
            case 4127: case 4128: case 4129:
            case 4130: case 4149: case 4150:
            case 4143: case 4144:
            case 4145: case 4146:
            case 4147: case 4148:
            case 4151: case 4152: case 4153:
            case 4286: case 4366:
            case 4372: case 4103: case 3966:
            case 5025: case 5027: case 5029: case 5031:
            case 5033:
                return true;
            default: break;
        }

        for (int id : ::kKnownCheckpointIDs) {
            if (id == bt) return true;
        }
        if (::oIsBlockCheckPoint && ::oIsBlockCheckPoint(bt)) return true;

        return false;
    }

    static inline bool PF_IsHardcodedSolid(int bt) {
        switch (bt) {

            case 3:
            case 343:

            case 75:
            case 757:
            case 1382:
            case 1626: case 1627:
            case 2998:

            case 5034:

            case 954:
                return true;
            default: break;
        }

        if (::GodModeEnabled) {
            for (int id : ::kForcedSolidIDs) {
                if (id == bt) return true;
            }
            if (::g_gmMasterSwap) {
                for (int id : ::kGodmodeStandTiles) {
                    if (id == bt) return true;
                }

                if (::oIsBlockInstakill        && ::oIsBlockInstakill(bt))        return true;
                if (::oIsBlockInstaDamageWater && ::oIsBlockInstaDamageWater(bt)) return true;
                if (::oIsBlockTrap             && ::oIsBlockTrap(bt))             return true;
            }
        }
        return false;
    }

    static bool PF_IsTilePassable_Tile(int blockType, int tileX, int tileY,
                                       bool fromBelow, bool fromAbove,
                                       bool walkInstakill) {
        if (blockType == 0)  return true;
        if (blockType < 0)   return false;
        if (PF_IsHardcodedSolid(blockType))    return false;
        if (PF_IsHardcodedPassable(blockType)) return true;

        if (!walkInstakill && g_pfStrictTraps) {
            if (::oIsBlockTrap && ::oIsBlockTrap(blockType))   return false;
            if (oCfgIsHot    && oCfgIsHot(blockType))          return false;
        }

        if (oCfgIsBattleBar && oCfgIsBattleBar(blockType)) {
            if (tileX > -9000 && tileY > -9000 &&
                g_WorldController && g_off_WCWorld)
            {
                bool open = false;
                bool gotItem = false;
                __try {
                    void* world = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
                    if (world) {
                        uintptr_t itLayer = *(uintptr_t*)((uintptr_t)world + 0x60);
                        if (itLayer) {
                            void** cols = (void**)(itLayer + 0x20);
                            if (cols && cols[tileX]) {
                                void** arr = (void**)((uintptr_t)cols[tileX] + 0x20);
                                if (arr) {
                                    void* item = arr[tileY];
                                    if (item) {
                                        open = *(bool*)((uintptr_t)item + 0x1D);
                                        gotItem = true;
                                    }
                                }
                            }
                        }
                    }
                } __except (EXCEPTION_EXECUTE_HANDLER) { gotItem = false; }
                if (gotItem) return open;
            }

            return false;
        }

        if (g_extraPassableCheck && tileX != -9999 && tileY != -9999 &&
            g_extraPassableCheck(tileX, tileY)) {
            return true;
        }

        if (!oCfgDoesCollide) return false;
        __try {

            if (!walkInstakill) {
                if (oCfgIsInstakill && oCfgIsInstakill(blockType)) return false;
                if (oCfgDeathByColl && oCfgDeathByColl(blockType)) return false;
            }

            if (g_avoidWater && oCfgIsWater && oCfgIsWater(blockType))
                return false;

            if (::Door::IsDoor(blockType)) {
                if (tileX > -9000 && tileY > -9000)
                    return ::Door::PlayerHasAccess(tileX, tileY);
                return true;
            }
            if (oCfgIsHatch   && oCfgIsHatch(blockType))    return true;
            if (oCfgIsPortal  && oCfgIsPortal(blockType))   return true;

            bool hasColl = oCfgDoesCollide(blockType);
            if (!hasColl) return true;

            if (oCfgIsOneWay && oCfgIsOneWay(blockType))
                return !fromAbove;

            if (tileX > -9000 && tileY > -9000 && ::Door::PlayerHasAccess(tileX, tileY))
                return true;

            return false;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    }

    static bool PF_IsTilePassable_Safe(int blockType, bool fromBelow,
                                       bool fromAbove, bool walkInstakill) {
        return PF_IsTilePassable_Tile(blockType, -9999, -9999,
                                      fromBelow, fromAbove, walkInstakill);
    }

    static bool PF_IsTilePassable_Safe(int blockType, bool fromBelow, bool walkInstakill) {
        return PF_IsTilePassable_Tile(blockType, -9999, -9999,
                                      fromBelow, false, walkInstakill);
    }
    static inline bool PF_IsTilePassable_Safe(int blockType, bool fromBelow) {
        return PF_IsTilePassable_Safe(blockType, fromBelow, false, g_walkInstakill);
    }

    inline bool PF_InBounds(Vector2i pt, int w, int h) {
        return pt.x >= 0 && pt.x < w && pt.y >= 0 && pt.y < h;
    }

    struct PF_BlockLayerInfo { Il2CppArray* layer; int w; int h; bool ok; };
    static PF_BlockLayerInfo PF_ReadBlockLayer(void* worldObj) {
        PF_BlockLayerInfo info = {};
        __try {
            Il2CppArray* outer =
                *(Il2CppArray**)((uintptr_t)worldObj + g_off_WorldBlockLayer);
            if (!outer) return info;
            info.layer = outer;
            info.w     = (int)outer->max_length;
            if (info.w > 0) {
                Il2CppArray* col0 = (Il2CppArray*)outer->items[0];
                if (col0) info.h = (int)col0->max_length;
            }
            info.ok = (info.layer != nullptr && info.w > 0 && info.h > 0);
        } __except (EXCEPTION_EXECUTE_HANDLER) { info.ok = false; }
        return info;
    }

    static bool PF_IsGrounded_Safe(Il2CppArray* worldBlocks,
                                   int tileX, int tileY,
                                   int worldSizeX, int worldSizeY,
                                   bool walkInstakill)
    {
        if (tileY <= 0) return true;

        int btSelf = PF_BlockTypeAtTile(worldBlocks, tileX, tileY, worldSizeX, worldSizeY);
        if (btSelf > 0 && ::Door::IsDoor(btSelf)) return true;

        int bt = PF_BlockTypeAtTile(worldBlocks, tileX, tileY - 1, worldSizeX, worldSizeY);
        if (bt <= 0) return false;
        if (PF_IsHardcodedSolid(bt)) return true;

        if (oCfgIsOneWay) {
            __try { if (oCfgIsOneWay(bt)) return true; }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
        if (oCfgIsPlatform) {
            __try { if (oCfgIsPlatform(bt)) return true; }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }

        if (::Door::IsDoor(bt)) return true;

        if (oCfgDoesCollide) {
            __try { if (oCfgDoesCollide(bt)) return true; }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
        return false;
    }

    static std::vector<Vector2i> RunAStar(
        Vector2i start, Vector2i goal,
        Il2CppArray* worldBlocks,
        int worldW, int worldH,
        bool airborneCheck, int maxAirborne,
        bool walkInstakill)
    {
        if (start.x == goal.x && start.y == goal.y) return {};
        if (!PF_InBounds(start, worldW, worldH) ||
            !PF_InBounds(goal,  worldW, worldH)) return {};

        int airDim = (airborneCheck ? maxAirborne : 0) + 1;

        struct PQNode {
            int      f;
            int      airborne;
            Vector2i pos;
            bool operator>(const PQNode& o) const { return f > o.f; }
        };
        std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> open;

        auto key = [&](Vector2i p, int air) {
            return ((p.y * worldW + p.x) * airDim) + air;
        };
        auto heuristic = [](Vector2i a, Vector2i b) {
            return (abs(a.x - b.x) + abs(a.y - b.y)) * 10;
        };

        std::unordered_map<int,int>      gScore;
        std::unordered_map<int,int>      cameFromKey;
        std::unordered_map<int,Vector2i> keyToPos;

        int startAir = 0;
        int startKey = key(start, startAir);
        gScore[startKey]    = 0;
        keyToPos[startKey]  = start;
        open.push({ heuristic(start, goal), startAir, start });

        const Vector2i dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
        const int MAX_ITER = 16384;
        int iter = 0;

        int goalKey = -1;

        while (!open.empty() && iter++ < MAX_ITER) {
            PQNode top = open.top(); open.pop();
            Vector2i cur = top.pos;
            int curAir   = top.airborne;
            int curKey   = key(cur, curAir);

            if (cur.x == goal.x && cur.y == goal.y) {
                goalKey = curKey;
                break;
            }

            int curG = gScore.count(curKey) ? gScore[curKey] : INT_MAX;

            for (auto& d : dirs) {
                Vector2i nb = { cur.x + d.x, cur.y + d.y };
                if (!PF_InBounds(nb, worldW, worldH)) continue;

                int bt = PF_BlockTypeAtTile(worldBlocks, nb.x, nb.y, worldW, worldH);
                bool fromBelow = (d.y == 1);
                bool fromAbove = (d.y == -1);
                if (!PF_IsTilePassable_Tile(bt, nb.x, nb.y, fromBelow, fromAbove, walkInstakill)) continue;

                if (!walkInstakill &&
                    PF_HasHazardousNeighbor(worldBlocks, nb.x, nb.y, worldW, worldH))
                    continue;

                int nbAir = 0;
                if (airborneCheck) {
                    bool grounded = PF_IsGrounded_Safe(worldBlocks, nb.x, nb.y,
                                                       worldW, worldH, walkInstakill);
                    nbAir = grounded ? 0 : (curAir + 1);
                    if (nbAir > maxAirborne) continue;
                }

                int ng = curG + 10;
                int nbKey = key(nb, nbAir);
                if (!gScore.count(nbKey) || ng < gScore[nbKey]) {
                    gScore[nbKey]     = ng;
                    cameFromKey[nbKey] = curKey;
                    keyToPos[nbKey]   = nb;
                    open.push({ ng + heuristic(nb, goal), nbAir, nb });
                }
            }
        }

        if (goalKey < 0) return {};

        std::vector<Vector2i> path;
        int k = goalKey;
        while (k != startKey) {
            auto pIt = keyToPos.find(k);
            if (pIt == keyToPos.end()) break;
            path.push_back(pIt->second);
            auto cIt = cameFromKey.find(k);
            if (cIt == cameFromKey.end()) break;
            k = cIt->second;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    static bool CacheWorldOrigin_Safe(void* wc) {
        __try {
            Vector3 p0 = oConvertMapPointToWorldPoint_V3(wc, { 0, 0 });
            Vector3 p1 = oConvertMapPointToWorldPoint_V3(wc, { 1, 0 });
            float sz = fabsf(p1.x - p0.x);
            if (sz > 0.001f) {
                g_tileSize = { sz, sz };

                g_worldOrigin = { p0.x - sz * 0.5f, p0.y - sz * 0.5f, p0.z };
                g_originCached        = true;
                g_needsTileSizeRecalc = false;
                return true;
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
        return false;
    }

    struct PathResult {
        std::vector<Vector2i> tiles;
        std::vector<Vector2>  offsets;
        PathStatus            status;
    };

    static void ApplyChamfer(std::vector<Vector2>& offsets,
                             const std::vector<Vector2i>& tiles,
                             Il2CppArray* layer, int wsx, int wsy,
                             float amount, bool walkInstakill)
    {
        offsets.assign(tiles.size(), { 0.f, 0.f });
        if (tiles.size() < 3 || amount <= 0.f) return;
        for (size_t i = 1; i + 1 < tiles.size(); ++i) {
            Vector2i p = tiles[i-1], c = tiles[i], n = tiles[i+1];
            int d1x = c.x - p.x, d1y = c.y - p.y;
            int d2x = n.x - c.x, d2y = n.y - c.y;
            if (d1x == d2x && d1y == d2y) continue;
            if (d1x + d2x == 0 && d1y + d2y == 0) continue;

            int icx = c.x + (-d1x + d2x);
            int icy = c.y + (-d1y + d2y);
            int bt = PF_BlockTypeAtTile(layer, icx, icy, wsx, wsy);
            bool insideSolid = !PF_IsTilePassable_Tile(bt, icx, icy, false, false, walkInstakill) &&
                               !PF_IsTilePassable_Tile(bt, icx, icy, true,  false, walkInstakill);
            if (!insideSolid) continue;

            float ox = (float)(d1x - d2x);
            float oy = (float)(d1y - d2y);
            float l  = sqrtf(ox*ox + oy*oy);
            if (l < 0.001f) continue;
            offsets[i].x = (ox / l) * amount;
            offsets[i].y = (oy / l) * amount;
        }
    }

    static inline void PF_RecordBuild(const char* subsystem,
                                       Vector2i startTile, Vector2i goalTile,
                                       const PathResult& res,
                                       long long buildMicros)
    {
        g_dbgLastSubsystem    = subsystem;
        g_dbgLastStart        = startTile;
        g_dbgLastGoal         = goalTile;
        g_dbgLastTileCount    = (int)res.tiles.size();
        g_dbgLastBuildMicros  = buildMicros;
        if (g_dbgVerboseLog) {
            const char* statusName = PathStatusText(res.status);
            std::cout << "[PathDebug] " << subsystem
                      << " status=" << statusName
                      << " tiles=" << res.tiles.size()
                      << " start=(" << startTile.x << "," << startTile.y << ")"
                      << " goal=(" << goalTile.x << "," << goalTile.y << ")"
                      << " " << buildMicros << "us\n";
        }
    }

    static PathResult BuildPath(
        void* wc, void* worldObj,
        Vector2i startTile, Vector2i goalTile,
        const PathOptions& opt)
    {
        auto _dbgT0 = std::chrono::steady_clock::now();
        auto _dbgEmit = [&](const PathResult& r) -> const PathResult& {
            auto micros = std::chrono::duration_cast<std::chrono::microseconds>(
                              std::chrono::steady_clock::now() - _dbgT0).count();
            PF_RecordBuild("BuildPath", startTile, goalTile, r, micros);
            return r;
        };

        if (!worldObj) {
            PathResult r = { { startTile, goalTile }, {}, PathStatus::NoWorld };
            return _dbgEmit(r);
        }

        PF_BlockLayerInfo bli = PF_ReadBlockLayer(worldObj);
        if (!bli.ok) {
            PathResult r = { { startTile, goalTile }, {}, PathStatus::NoWorld };
            return _dbgEmit(r);
        }

        if (!PF_InBounds(startTile, bli.w, bli.h) ||
            !PF_InBounds(goalTile,  bli.w, bli.h)) {
            PathResult r = { { startTile, goalTile }, {}, PathStatus::OutOfBounds };
            return _dbgEmit(r);
        }

        {
            int tBt = PF_BlockTypeAtTile(bli.layer, goalTile.x, goalTile.y, bli.w, bli.h);
            bool passSide  = PF_IsTilePassable_Tile(tBt, goalTile.x, goalTile.y, false, false, opt.walkInstakill);
            bool passUp    = PF_IsTilePassable_Tile(tBt, goalTile.x, goalTile.y, true,  false, opt.walkInstakill);
            bool passDown  = PF_IsTilePassable_Tile(tBt, goalTile.x, goalTile.y, false, true,  opt.walkInstakill);
            if (!passSide && !passUp && !passDown) {
                PathResult r = { { startTile, goalTile }, {}, PathStatus::TargetSolid };
                return _dbgEmit(r);
            }

            int sBt = PF_BlockTypeAtTile(bli.layer, startTile.x, startTile.y, bli.w, bli.h);
            if (!PF_IsTilePassable_Tile(sBt, startTile.x, startTile.y, false, false, opt.walkInstakill) &&
                !PF_IsTilePassable_Tile(sBt, startTile.x, startTile.y, true,  false, opt.walkInstakill) &&
                !PF_IsTilePassable_Tile(sBt, startTile.x, startTile.y, false, true,  opt.walkInstakill)) {
                PathResult r = { { startTile, goalTile }, {}, PathStatus::StartSolid };
                return _dbgEmit(r);
            }
        }

        auto path = RunAStar(startTile, goalTile, bli.layer, bli.w, bli.h,
                             opt.airborneCheck, opt.maxAirborne,
                             opt.walkInstakill);
        if (path.empty()) {
            PathResult r = { { startTile, goalTile }, {}, PathStatus::NoPath };
            return _dbgEmit(r);
        }

        path.insert(path.begin(), startTile);

        std::vector<Vector2> offsets;

        offsets.assign(path.size(), { 0.f, 0.f });

        PathResult r = { std::move(path), std::move(offsets), PathStatus::Success };
        return _dbgEmit(r);
    }

    static inline PathResult BuildPath(void* wc, void* worldObj,
                                       Vector2i startTile, Vector2i goalTile)
    {
        return BuildPath(wc, worldObj, startTile, goalTile, StandaloneOptions());
    }

    struct MiningWeights {
        int  upPenaltySolid    = 25;
        int  unsupportedExtra  = 60;
        bool preferStaircase   = true;

        bool airborneCheck     = true;
        int  maxAirborne       = 5;
    };

    using MiningCostFn = int(*)(int blockType);

    static std::vector<Vector2i> RunAStarMining(
        Vector2i start, Vector2i goal,
        Il2CppArray* worldBlocks,
        int worldW, int worldH,
        MiningCostFn costFn,
        const MiningWeights& w)
    {
        if (start.x == goal.x && start.y == goal.y) return {};
        if (!PF_InBounds(start, worldW, worldH) ||
            !PF_InBounds(goal,  worldW, worldH)) return {};

        const int airDim = (w.airborneCheck ? w.maxAirborne : 0) + 1;

        struct PQNode {
            int      f;
            int      airborne;
            Vector2i pos;
            bool operator>(const PQNode& o) const { return f > o.f; }
        };
        std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> open;

        auto key = [&](Vector2i p, int air) {
            return ((p.y * worldW + p.x) * airDim) + air;
        };
        auto heuristic = [](Vector2i a, Vector2i b) {
            return (abs(a.x - b.x) + abs(a.y - b.y));
        };

        std::unordered_map<int,int>      gScore;
        std::unordered_map<int,int>      cameFromKey;
        std::unordered_map<int,Vector2i> keyToPos;

        auto costAt = [&](int x, int y) -> int {
            int bt = PF_BlockTypeAtTile(worldBlocks, x, y, worldW, worldH);
            return costFn ? costFn(bt) : 1;
        };

        auto isGrounded = [&](Vector2i p) -> bool {
            int below = costAt(p.x, p.y - 1);
            return (below > 1);
        };

        int startKey = key(start, 0);
        gScore[startKey]   = 0;
        keyToPos[startKey] = start;
        open.push({ heuristic(start, goal), 0, start });

        const Vector2i dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
        const int MAX_ITER = 65536;
        int iter = 0;
        int goalKey = -1;

        while (!open.empty() && iter++ < MAX_ITER) {
            PQNode top = open.top(); open.pop();
            Vector2i cur = top.pos;
            int curAir = top.airborne;
            int curKey = key(cur, curAir);
            if (cur.x == goal.x && cur.y == goal.y) { goalKey = curKey; break; }
            int curG = gScore.count(curKey) ? gScore[curKey] : INT_MAX;

            for (auto& d : dirs) {
                Vector2i nb = { cur.x + d.x, cur.y + d.y };
                if (!PF_InBounds(nb, worldW, worldH)) continue;
                int nbBT = PF_BlockTypeAtTile(worldBlocks, nb.x, nb.y, worldW, worldH);

                int aboveBT = PF_BlockTypeAtTile(worldBlocks, nb.x, nb.y + 1, worldW, worldH);
                if (aboveBT == 3966 || aboveBT == 4103) continue;
                int stepCost = costFn ? costFn(nbBT) : 1;
                if (stepCost < 0) continue;
                if (PF_IsHardcodedSolid(nbBT) && !(nb.x == goal.x && nb.y == goal.y))
                    continue;

                int nbAir = 0;
                if (w.airborneCheck) {
                    bool grounded = isGrounded(nb);
                    nbAir = grounded ? 0 : (curAir + 1);
                    bool isGoalTile = (nb.x == goal.x && nb.y == goal.y);
                    if (nbAir > w.maxAirborne && !isGoalTile) continue;
                }

                if (w.preferStaircase && d.y > 0 && stepCost > 1) {
                    stepCost += w.upPenaltySolid;
                    int belowCost = costAt(cur.x, cur.y - 1);
                    if (belowCost >= 0 && belowCost <= 1) {
                        stepCost += w.unsupportedExtra;
                    }
                }

                {
                    int floorBT = PF_BlockTypeAtTile(worldBlocks, nb.x, nb.y - 1, worldW, worldH);
                    if (floorBT > 0) {

                        bool hot = oCfgIsHot ? oCfgIsHot(floorBT) : false;

                        if (!hot && floorBT == 3987) hot = true;

                        if (hot) {
                            bool isGoalTile = (nb.x == goal.x && nb.y == goal.y);
                            if (!isGoalTile) stepCost += 200;
                        }
                    }
                }

                int ng = curG + stepCost;
                int nbKey = key(nb, nbAir);
                if (!gScore.count(nbKey) || ng < gScore[nbKey]) {
                    gScore[nbKey]      = ng;
                    cameFromKey[nbKey] = curKey;
                    keyToPos[nbKey]    = nb;
                    open.push({ ng + heuristic(nb, goal), nbAir, nb });
                }
            }
        }
        if (goalKey < 0) return {};

        std::vector<Vector2i> path;
        int k = goalKey;
        while (k != startKey) {
            auto pIt = keyToPos.find(k);
            if (pIt == keyToPos.end()) break;
            path.push_back(pIt->second);
            auto cIt = cameFromKey.find(k);
            if (cIt == cameFromKey.end()) break;
            k = cIt->second;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    static PathResult BuildPathMining(
        void* worldObj,
        Vector2i startTile, Vector2i goalTile,
        MiningCostFn costFn,
        const MiningWeights& w)
    {
        if (!worldObj) return { { startTile, goalTile }, {}, PathStatus::NoWorld };
        PF_BlockLayerInfo bli = PF_ReadBlockLayer(worldObj);
        if (!bli.ok) return { { startTile, goalTile }, {}, PathStatus::NoWorld };
        if (!PF_InBounds(startTile, bli.w, bli.h) ||
            !PF_InBounds(goalTile,  bli.w, bli.h))
            return { { startTile, goalTile }, {}, PathStatus::OutOfBounds };

        if (startTile.x == goalTile.x && startTile.y == goalTile.y) {
            return { { startTile }, { Vector2{ 0.f, 0.f } }, PathStatus::Success };
        }

        auto path = RunAStarMining(startTile, goalTile, bli.layer, bli.w, bli.h, costFn, w);
        if (path.empty()) return { { startTile, goalTile }, {}, PathStatus::NoPath };
        path.insert(path.begin(), startTile);
        std::vector<Vector2> offsets(path.size(), Vector2{ 0.f, 0.f });
        return { std::move(path), std::move(offsets), PathStatus::Success };
    }

    static void SehInvokeSendPos0(tSendPlayerPosition fn) {
        if (!fn) return;
        __try { fn(); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    static bool __fastcall hkPlayerIsGrounded(void* self) {
        if (g_tp_forceGrounded) return true;
        if (oPlayerIsGrounded_orig) return oPlayerIsGrounded_orig(self);
        return false;
    }

    static void __fastcall hkPlayerFixedUpdate(void* self) {
        if (g_tp_suppressGravity) return;
        if (oPlayerFixedUpdate_orig) oPlayerFixedUpdate_orig(self);
    }

    static bool TpPacketCooldownReady() {
        float hz = g_tp_packetCapHz;
        if (hz <= 0.f) return true;
        long long minNs = (long long)(1.0e9 / (double)hz);
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        long long nowNs = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
        long long last  = g_tp_lastSendNs.load(std::memory_order_acquire);
        if ((nowNs - last) < minNs) return false;
        return g_tp_lastSendNs.compare_exchange_strong(last, nowNs,
                                                       std::memory_order_release,
                                                       std::memory_order_acquire);
    }

    static void __fastcall hkSendPlayerPosition0() {
        if (g_tp_throttleMovePackets) {
            bool expected = true;
            if (!g_tp_allowOnePacket.compare_exchange_strong(expected, false))
                return;
        }
        if (oSendPlayerPosition0_orig) oSendPlayerPosition0_orig();
    }

    static void __fastcall hkSendPlayerPosition1(void* bsonMsg) {
        if (g_tp_throttleMovePackets) {
            bool expected = true;
            if (!g_tp_allowOnePacket.compare_exchange_strong(expected, false))
                return;
        }
        if (oSendPlayerPosition1_orig) oSendPlayerPosition1_orig(bsonMsg);
    }

    inline void InstallTileTpHooks() {
        static bool installed = false;
        if (installed) return;

        DoHook(GetMethodPtr("Player", "IsGrounded"),
               (void*)&hkPlayerIsGrounded,
               (void**)&oPlayerIsGrounded_orig,
               "Player::IsGrounded (TileTP)");

        DoHook(GetMethodPtr("Player", "FixedUpdate"),
               (void*)&hkPlayerFixedUpdate,
               (void**)&oPlayerFixedUpdate_orig,
               "Player::FixedUpdate (TileTP)");

        void* sp0 = Manifest::GetMethod("OutgoingMessages", "SendPlayerPosition");
        void* sp1 = Manifest::GetMethod("OutgoingMessages", "SendPlayerPosition_0");

        if (!sp0) sp0 = oSendPlayerPosition;
        DoHook(sp0,
               (void*)&hkSendPlayerPosition0,
               (void**)&oSendPlayerPosition0_orig,
               "OutgoingMessages::SendPlayerPosition (0-arg, TileTP)");

        if (sp1 && sp1 != sp0) {
            DoHook(sp1,
                   (void*)&hkSendPlayerPosition1,
                   (void**)&oSendPlayerPosition1_orig,
                   "OutgoingMessages::SendPlayerPosition (1-arg, TileTP)");
        }

        installed = true;
    }

    inline bool Initialize() {
        oGetMainCamera = (tGetMainCamera)IL2CPP::Class::Utils::GetMethodPointer(
            "UnityEngine.Camera", "get_main", 0);
        if (!oGetMainCamera) oGetMainCamera = ::oGetMainCamera;

        oGetPosition = (tGetPosition)IL2CPP::Class::Utils::GetMethodPointer(
            "UnityEngine.Transform", "get_position", 0);
        if (!oGetPosition) oGetPosition = (tGetPosition)::oGetPosition;

        oGetTransform = (tGetTransform)IL2CPP::Class::Utils::GetMethodPointer(
            "UnityEngine.Component", "get_transform", 0);
        if (!oGetTransform) oGetTransform = (tGetTransform)::oGetTransform;

        oCfgDoesCollide = (tCfgDoesCollide)::oDoesBlockHaveCollider;
        if (!oCfgDoesCollide) {

            oCfgDoesCollide = (tCfgDoesCollide)IL2CPP::Class::Utils::GetMethodPointer(
                "ConfigData", "DoesBlockHaveCollider", 1);
        }
        oCfgIsOneWay = (tCfgIsOneWay)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockColliderOneWay", 1);
        oCfgIsInstakill = (tCfgIsInstakill)::oIsBlockInstakill;
        if (!oCfgIsInstakill) {
            oCfgIsInstakill = (tCfgIsInstakill)IL2CPP::Class::Utils::GetMethodPointer(
                "ConfigData", "IsBlockInstakill", 1);
        }
        oCfgIsWater = (tCfgIsWater)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockWater", 1);
        oCfgIsSwimming = (tCfgIsSwimming)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockSwimming", 1);
        oCfgIsPlatform = (tCfgIsPlatform)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockPlatform", 1);
        oCfgIsHatch = (tCfgIsHatch)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockHatch", 1);
        oCfgIsPortal = (tCfgIsPortal)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockPortal", 1);
        oCfgDeathByColl = (tCfgDeathByColl)::oDoesBlockCauseDeathByCollider;
        if (!oCfgDeathByColl) {
            oCfgDeathByColl = (tCfgDeathByColl)IL2CPP::Class::Utils::GetMethodPointer(
                "ConfigData", "DoesBlockCauseDeathByCollider", 1);
        }
        oCfgIsDisapBlk = (tCfgIsDisapBlk)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockDisappearingBlock", 1);
        oCfgIsBattleBar = (tCfgIsBattleBar)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockBattleBarrier", 1);
        oCfgIsHot = (tCfgIsHot)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsBlockHot", 1);
        oCfgCollectableBlocked = (tCfgCollectableBlocked)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "IsCollectableBlockedByColliderBlock", 2);

        oIsBattleOn = (tIsBattleOn)IL2CPP::Class::Utils::GetMethodPointer(
            "World", "IsBattleOn", 1);
        std::cout << "[PathRenderer] World.IsBattleOn "
                  << (oIsBattleOn ? "resolved OK" : "NOT resolved")
                  << " @ " << (void*)oIsBattleOn << "\n";
        g_bclhResolved = (oCfgDoesCollide != nullptr);
        if (g_bclhResolved)
            std::cout << "[PathRenderer] ConfigData.DoesBlockHaveCollider resolved OK\n";
        else
            std::cout << "[PathRenderer] ConfigData.DoesBlockHaveCollider NOT resolved — blockType==0 fallback\n";

        oGetKukouriCamInst = (tGetKukouriCamInst)IL2CPP::Class::Utils::GetMethodPointer(
            "KukouriCamera", "get_Instance", 0);
        oForceUpdatePos = (tForceUpdatePos)IL2CPP::Class::Utils::GetMethodPointer(
            "KukouriCamera", "ForceUpdatePosition", 0);
        g_kukouriResolved = (oGetKukouriCamInst && oForceUpdatePos);
        if (g_kukouriResolved)
            std::cout << "[PathRenderer] KukouriCamera.ForceUpdatePosition resolved OK\n";
        else
            std::cout << "[PathRenderer] KukouriCamera.ForceUpdatePosition NOT resolved\n";

        oSendPlayerActivateInPortal = (tSendPlayerActivateInPortal)IL2CPP::Class::Utils::GetMethodPointer(
            "OutgoingMessages", "SendPlayerActivateInPortal", 1);
        if (oSendPlayerActivateInPortal)
            std::cout << "[PathRenderer] OutgoingMessages.SendPlayerActivateInPortal resolved OK\n";
        else
            std::cout << "[PathRenderer] OutgoingMessages.SendPlayerActivateInPortal NOT resolved\n";

        oSendPlayerPosition = (tSendPlayerPosition)IL2CPP::Class::Utils::GetMethodPointer(
            "OutgoingMessages", "SendPlayerPosition", 0);
        if (oSendPlayerPosition)
            std::cout << "[PathRenderer] OutgoingMessages.SendPlayerPosition resolved OK\n";
        else
            std::cout << "[PathRenderer] OutgoingMessages.SendPlayerPosition NOT resolved\n";

        InstallTileTpHooks();

        g_initialized = true;
        return true;
    }

    inline void EnsureThreadAttached() {
        thread_local bool s_attached = false;
        if (s_attached) return;
        __try {
            void* dom = IL2CPP::Domain::Get();
            if (dom) IL2CPP::Thread::Attach(dom);
            s_attached = true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {

        }
    }

    inline int  g_pf_setPosFailCount  = 0;
    inline int  g_pf_setVelFailCount  = 0;
    inline int  g_pf_getTrFailCount   = 0;

    static void* SafeGetTransform(void* player) {
        if (!oGetTransform || !player) return nullptr;
        void* r = nullptr;
        __try { r = oGetTransform(player); }
        __except (EXCEPTION_EXECUTE_HANDLER) { g_pf_getTrFailCount++; return nullptr; }
        return r;
    }
    static bool SafeSetPosition(void* transform, Vector3 v) {
        if (!oSetPosition || !transform) return false;
        __try { oSetPosition(transform, v); return true; }
        __except (EXCEPTION_EXECUTE_HANDLER) { g_pf_setPosFailCount++; return false; }
    }
    static bool SafeSetVelocity(void* player, Vector3 v) {
        if (!oSetVelocity || !player) return false;

        g_pfPendingVelX.store(v.x, std::memory_order_relaxed);
        g_pfPendingVelY.store(v.y, std::memory_order_relaxed);
        g_pfPendingVelZ.store(v.z, std::memory_order_relaxed);
        g_pfPendingVelValid.store(true, std::memory_order_release);
        return true;
    }

    inline void Render() {
        EnsureThreadAttached();
        if (g_isTransitioning.load(std::memory_order_acquire)) return;
        if (!g_IsInWorld || !g_LocalPlayer)                     return;
        if (!oGetTransform || !oGetPosition)                    return;

        {
            static int     s_diagTick = 0;
            static Vector3 s_prevPos { 0, 0, 0 };
            if (g_isNavigating) {
                ++s_diagTick;
                if (s_diagTick % 30 == 1) {
                    Vector3 curPos { 0, 0, 0 };
                    void* tr = SafeGetTransform(g_LocalPlayer);
                    if (tr) curPos = oGetPosition(tr);
                    float dx_30 = curPos.x - s_prevPos.x;
                    float dy_30 = curPos.y - s_prevPos.y;
                    std::cout << "[PFDiag] frame=" << s_diagTick
                              << " navigating=1"
                              << " pathIdx=" << g_pathIdx << "/" << g_pathTiles.size()
                              << " suppressGravity=" << (int)g_tp_suppressGravity
                              << " moveMethod=" << (int)g_moveMethod
                              << " pos={" << curPos.x << "," << curPos.y << "}"
                              << " d30={" << dx_30 << "," << dy_30 << "}"
                              << " fails(p/v/t)=" << g_pf_setPosFailCount
                              << "/" << g_pf_setVelFailCount
                              << "/" << g_pf_getTrFailCount
                              << std::endl;
                    s_prevPos = curPos;
                }
            } else {
                s_diagTick = 0;
            }
        }

        TpClearActive();

        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        if (dt <= 0.f || dt > 0.25f) dt = 0.016f;

        if (g_pathRendererResetPending) {
            g_originCached        = false;
            g_needsTileSizeRecalc = true;
            g_isNavigating        = false;
            g_isTeleporting       = false;
            g_hasTarget           = false;

            g_pfGravityOverridden = false;
            {
                std::lock_guard<std::mutex> lk(g_pathMutex);
                g_pathTiles.clear();
                g_pathOffsets.clear();
            }
            PathFinder_ClearInputs();
            g_pathRendererResetPending = false;
        }

        float camX  = g_cachedCamX    .load(std::memory_order_relaxed);
        float camY  = g_cachedCamY    .load(std::memory_order_relaxed);
        float ortho = g_cachedCamOrtho.load(std::memory_order_relaxed);
        bool  camOk = g_cachedCamValid.load(std::memory_order_acquire) && ortho > 0.f;
        float scale = camOk ? (io.DisplaySize.y / (ortho * 2.f)) : 0.f;
        float tileW = (g_tileSize.x > 0.001f) ? g_tileSize.x : 0.32f;

        if (g_debugBlockOverlay && g_WorldController && g_originCached && camOk) {
            void* dbgWorldObj = g_off_WCWorld
                ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld) : nullptr;
            if (dbgWorldObj) {
                PF_BlockLayerInfo bli = PF_ReadBlockLayer(dbgWorldObj);
                if (bli.ok) {
                    ImDrawList* dl2 = ImGui::GetForegroundDrawList();

                    float viewW = ortho * (io.DisplaySize.x / io.DisplaySize.y);
                    float viewH = ortho;
                    int txMin = (int)floorf((camX - viewW - g_worldOrigin.x) / tileW) - 1;
                    int txMax = (int) ceilf((camX + viewW - g_worldOrigin.x) / tileW) + 1;
                    int tyMin = (int)floorf((camY - viewH - g_worldOrigin.y) / tileW) - 1;
                    int tyMax = (int) ceilf((camY + viewH - g_worldOrigin.y) / tileW) + 1;
                    txMin = txMin < 0 ? 0 : txMin;
                    tyMin = tyMin < 0 ? 0 : tyMin;
                    if (txMax >= bli.w) txMax = bli.w - 1;
                    if (tyMax >= bli.h) tyMax = bli.h - 1;

                    for (int ty2 = tyMin; ty2 <= tyMax; ++ty2) {
                        for (int tx2 = txMin; tx2 <= txMax; ++tx2) {
                            int bt = PF_BlockTypeAtTile(bli.layer, tx2, ty2, bli.w, bli.h);
                            ImU32 col;
                            if (bt < 0)       col = IM_COL32(255,255,255, 40);
                            else if (bt == 0) col = IM_COL32(  0,255,  0, 18);
                            else {

                                bool passable = PF_IsTilePassable_Tile(bt, tx2, ty2, false, false, g_walkInstakill);
                                bool passUp   = PF_IsTilePassable_Tile(bt, tx2, ty2, true,  false, g_walkInstakill);
                                if (!passable && !passUp)
                                    col = IM_COL32(255, 50, 50, 70);
                                else if (passUp && !passable)
                                    col = IM_COL32(255,220, 30, 70);
                                else
                                    col = IM_COL32(120,120,120, 40);
                            }
                            Vector3 ctr2 = TileToWorldCenter(tx2, ty2, 0.f, tileW);
                            Vector2 cs;
                            if (NavWorldToScreen(ctr2, cs)) {
                                float h2 = tileW * scale * 0.5f;
                                dl2->AddRectFilled(
                                    ImVec2(cs.x - h2, cs.y - h2),
                                    ImVec2(cs.x + h2, cs.y + h2), col);

                                if (bt > 0 && tileW * scale > 18.f) {
                                    char buf[8]; snprintf(buf, sizeof(buf), "%d", bt);

                                    ImFont* fnt = ImGui::GetFont();
                                    dl2->AddText(fnt, 9.0f,
                                                 ImVec2(cs.x - h2 + 2, cs.y - h2 + 2),
                                                 IM_COL32(255,255,255,160), buf);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (g_dbgInspect && g_WorldController && g_originCached && camOk &&
            (GetAsyncKeyState(VK_MENU) & 0x8000))
        {
            void* dbgWorldObj = g_off_WCWorld
                ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld) : nullptr;
            if (dbgWorldObj) {
                PF_BlockLayerInfo bli = PF_ReadBlockLayer(dbgWorldObj);
                if (bli.ok) {

                    ImVec2 mp = ImGui::GetIO().MousePos;
                    float pxToWorld = 1.f / scale;
                    float wx = camX + (mp.x - io.DisplaySize.x * 0.5f) * pxToWorld;
                    float wy = camY - (mp.y - io.DisplaySize.y * 0.5f) * pxToWorld;
                    int tileX = (int)floorf((wx - g_worldOrigin.x) / tileW);
                    int tileY = (int)floorf((wy - g_worldOrigin.y) / tileW);
                    if (PF_InBounds({tileX, tileY}, bli.w, bli.h)) {
                        int bt = PF_BlockTypeAtTile(bli.layer, tileX, tileY, bli.w, bli.h);

                        bool hSolid  = PF_IsHardcodedSolid(bt);
                        bool hPass   = PF_IsHardcodedPassable(bt);
                        auto safeCall = [](auto fn, int btx) -> int {
                            if (!fn) return -1;
                            __try { return fn(btx) ? 1 : 0; }
                            __except (EXCEPTION_EXECUTE_HANDLER) { return -2; }
                        };
                        int instakill  = safeCall(oCfgIsInstakill,  bt);
                        int deathColl  = safeCall(oCfgDeathByColl,  bt);
                        int water      = safeCall(oCfgIsWater,      bt);
                        int swimming   = safeCall(oCfgIsSwimming,   bt);
                        int platform   = safeCall(oCfgIsPlatform,   bt);
                        int oneway     = safeCall(oCfgIsOneWay,     bt);
                        int hatch      = safeCall(oCfgIsHatch,      bt);
                        int portal     = safeCall(oCfgIsPortal,     bt);

                        int anydoor    = safeCall(::Door::oIsAnyDoor,                       bt);
                        int doorNorm   = safeCall(::Door::oIsNormalDoor,                    bt);
                        int doorClan   = safeCall(::Door::oIsClanDoor,                      bt);
                        int doorScifi  = safeCall(::Door::oIsScifiDoor,                     bt);
                        int doorVIP    = safeCall(::Door::oIsVIPDoor,                       bt);
                        int doorLvlVIP = safeCall(::Door::oIsLevelVIPDoor,                  bt);
                        int doorFDark  = safeCall(::Door::oIsClanFactionDark,               bt);
                        int doorFLight = safeCall(::Door::oIsClanFactionLight,              bt);
                        int collide    = safeCall(oCfgDoesCollide,  bt);
                        bool passSide  = PF_IsTilePassable_Tile(bt, tileX, tileY, false, false, g_walkInstakill);
                        bool passUp    = PF_IsTilePassable_Tile(bt, tileX, tileY, true,  false, g_walkInstakill);
                        bool passDown  = PF_IsTilePassable_Tile(bt, tileX, tileY, false, true,  g_walkInstakill);

                        auto fmtTri = [](int v) -> const char* {
                            return v == 1 ? "yes" : v == 0 ? "no" : v == -1 ? "n/a" : "?? (SEH)";
                        };
                        ImDrawList* dl3 = ImGui::GetForegroundDrawList();
                        char line[512];
                        snprintf(line, sizeof(line),
                            "Tile (%d, %d)  blockType=%d\n"
                            "Hardcoded:   solid=%s  passable=%s\n"
                            "ConfigData:  collide=%s  oneway=%s  platform=%s\n"
                            "Hazard:      instakill=%s  deathCollider=%s\n"
                            "Door:        any=%s  norm=%s  clan=%s  scifi=%s\n"
                            "             vip=%s  lvlVIP=%s  fDark=%s  fLight=%s\n"
                            "Other:       hatch=%s  portal=%s  water=%s  swim=%s\n"
                            "FINAL:       side=%s  up=%s  down=%s",
                            tileX, tileY, bt,
                            hSolid ? "YES" : "no", hPass ? "YES" : "no",
                            fmtTri(collide), fmtTri(oneway), fmtTri(platform),
                            fmtTri(instakill), fmtTri(deathColl),
                            fmtTri(anydoor),  fmtTri(doorNorm),  fmtTri(doorClan), fmtTri(doorScifi),
                            fmtTri(doorVIP),  fmtTri(doorLvlVIP), fmtTri(doorFDark), fmtTri(doorFLight),
                            fmtTri(hatch), fmtTri(portal), fmtTri(water), fmtTri(swimming),
                            passSide ? "PASS" : "BLOCK",
                            passUp   ? "PASS" : "BLOCK",
                            passDown ? "PASS" : "BLOCK");
                        ImVec2 ts = ImGui::CalcTextSize(line);
                        ImVec2 ta(mp.x + 16.f, mp.y + 16.f);
                        ImVec2 tb(ta.x + ts.x + 14.f, ta.y + ts.y + 12.f);

                        if (tb.x > io.DisplaySize.x - 4.f) {
                            float dx = tb.x - (io.DisplaySize.x - 4.f);
                            ta.x -= dx; tb.x -= dx;
                        }
                        if (tb.y > io.DisplaySize.y - 4.f) {
                            float dy = tb.y - (io.DisplaySize.y - 4.f);
                            ta.y -= dy; tb.y -= dy;
                        }
                        dl3->AddRectFilled(ta, tb, IM_COL32(15, 18, 24, 235), 6.f);
                        dl3->AddRect      (ta, tb, IM_COL32(80, 200, 255, 180), 6.f, 0, 1.f);
                        dl3->AddText(ImVec2(ta.x + 7.f, ta.y + 6.f),
                                     IM_COL32_WHITE, line);

                        Vector3 ctr3 = TileToWorldCenter(tileX, tileY, 0.f, tileW);
                        Vector2 cs3;
                        if (NavWorldToScreen(ctr3, cs3)) {
                            float h3 = tileW * scale * 0.5f;
                            ImU32 outline = (!passSide && !passUp && !passDown)
                                ? IM_COL32(255, 80, 80, 230)
                                : IM_COL32(80, 220, 120, 230);
                            dl3->AddRect(ImVec2(cs3.x - h3, cs3.y - h3),
                                         ImVec2(cs3.x + h3, cs3.y + h3),
                                         outline, 2.f, 0, 2.5f);
                        }
                    }
                }
            }
        }

        if (g_dbgStatsHud) {
            ImDrawList* dl4 = ImGui::GetForegroundDrawList();
            char line[256];
            snprintf(line, sizeof(line),
                "PathDebug: %s\n"
                "  status   %s\n"
                "  tiles    %d\n"
                "  build    %lld us\n"
                "  start    (%d, %d)\n"
                "  goal     (%d, %d)",
                g_dbgLastSubsystem ? g_dbgLastSubsystem : "(none)",
                PathStatusText(g_pathStatus),
                g_dbgLastTileCount,
                g_dbgLastBuildMicros,
                g_dbgLastStart.x, g_dbgLastStart.y,
                g_dbgLastGoal.x,  g_dbgLastGoal.y);
            ImVec2 ts = ImGui::CalcTextSize(line);
            float pad = 8.f;
            ImVec2 ta(io.DisplaySize.x - ts.x - pad * 2.f - 12.f,
                      io.DisplaySize.y - ts.y - pad * 2.f - 12.f);
            ImVec2 tb(ta.x + ts.x + pad * 2.f, ta.y + ts.y + pad * 2.f);
            dl4->AddRectFilled(ta, tb, IM_COL32(10, 12, 18, 220), 6.f);
            dl4->AddRect      (ta, tb, IM_COL32(80, 200, 255, 170), 6.f, 0, 1.f);

            dl4->AddRectFilled(ImVec2(ta.x, ta.y), ImVec2(ta.x + 3.f, tb.y),
                               IM_COL32(80, 200, 255, 220), 6.f,
                               ImDrawFlags_RoundCornersLeft);
            dl4->AddText(ImVec2(ta.x + pad + 4.f, ta.y + pad),
                         IM_COL32_WHITE, line);
        }

        if ((!g_originCached || g_needsTileSizeRecalc) &&
            g_WorldController && oConvertMapPointToWorldPoint_V3)
        {
            CacheWorldOrigin_Safe(g_WorldController);
        }

        tileW = (g_tileSize.x > 0.001f) ? g_tileSize.x : 0.32f;

        const bool altHeld = (GetAsyncKeyState(VK_MENU)   & 0x8000) != 0;
        const bool escHeld = (GetAsyncKeyState(VK_ESCAPE)  & 0x8000) != 0;
        const bool rHeld   = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

        static bool s_rPrev   = false;
        static bool s_escPrev = false;
        const bool rClicked   = rHeld   && !s_rPrev;
        const bool escPressed = escHeld && !s_escPrev;
        s_rPrev   = rHeld;
        s_escPrev = escHeld;

        const ImVec2 mp = ImGui::GetMousePos();
        float cx = mp.x;
        float cy = mp.y;

        const bool overImGui      = ImGui::GetIO().WantCaptureMouse;
        const bool wantAltHover   = (g_navEnabled || ::Minimap::g_blockTooltip)
                                    && altHeld && camOk && g_originCached
                                    && !overImGui;
        if (wantAltHover) {

            float worldX = (cx - io.DisplaySize.x * 0.5f) /  scale + camX;
            float worldY = -(cy - io.DisplaySize.y * 0.5f) / scale + camY;

            Vector2i hovTile = WorldToTile(worldX, worldY, tileW);
            Vector3  tileCtr = TileToWorldCenter(hovTile.x, hovTile.y, 0.f, tileW);

            Vector2 hoverS;
            if (NavWorldToScreen(tileCtr, hoverS)) {
                float half = tileW * scale * 0.5f;
                ImGui::GetForegroundDrawList()->AddRect(
                    ImVec2(hoverS.x - half, hoverS.y - half),
                    ImVec2(hoverS.x + half, hoverS.y + half),
                    IM_COL32(100, 200, 255, 220), 0.f, 0, 2.f);
            }

            if (::Minimap::g_blockTooltip && g_WorldController) {
                void* worldObj = g_off_WCWorld
                    ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld)
                    : nullptr;
                if (worldObj) {
                    int fgID = ::Minimap::Seh_ReadFgID((uintptr_t)worldObj,
                                                      hovTile.x, hovTile.y);
                    ImVec2 mp = ImGui::GetMousePos();
                    ::Minimap::RenderTileTooltip((uintptr_t)worldObj,
                                                 hovTile.x, hovTile.y, fgID,
                                                 ImVec2(mp.x + 18.f, mp.y + 18.f),
                                                 "##BlockTooltipAlt");
                }
            }

            if (g_navEnabled && rClicked && oGetTransform && oGetPosition && g_WorldController) {
                Vector3  playerPos = oGetPosition(oGetTransform(g_LocalPlayer));

                Vector2i startTile = WorldToTile(playerPos.x, playerPos.y + tileW * 0.5f, tileW);

                void* worldObj = g_off_WCWorld
                    ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld)
                    : nullptr;

                PathResult result =
                    BuildPath(g_WorldController, worldObj, startTile, hovTile);

                {
                    std::lock_guard<std::mutex> lk(g_pathMutex);
                    g_pathTiles   = std::move(result.tiles);
                    g_pathOffsets = std::move(result.offsets);
                    g_pathStatus  = result.status;
                    g_pathIdx     = 0;
                }
                g_navTarget     = tileCtr;
                g_hasTarget     = true;

                bool ok         = (result.status == PathStatus::Success);
                g_isNavigating  = ok;
                g_isTeleporting = ok;
                if (ok) {
                    PathFinder_DisableGravity();

                    if (g_moveMethod == MoveMethod::InputDrive)
                        PathFinder_PulseJumpDown();
                } else {
                    PathFinder_RestoreGravity();
                }
            }
        }

        if (escPressed) {
            g_isNavigating  = false;
            g_isTeleporting = false;
            PathFinder_RestoreGravity();
            PathFinder_ClearInputs();
            std::lock_guard<std::mutex> lk(g_pathMutex);
            g_pathTiles.clear();
            g_pathOffsets.clear();
        }

        if (g_isNavigating && g_hasTarget && g_autoMove && g_originCached &&
            oSetPosition && oGetTransform && oGetPosition && camOk)
        {

            PathFinder_DisableGravity();

            const float bodyOff = tileW * g_bodyOffsetFrac;

            Vector3 hereFeet  = oGetPosition(oGetTransform(g_LocalPlayer));
            Vector3 hereBody  = { hereFeet.x, hereFeet.y + bodyOff, hereFeet.z };

            std::lock_guard<std::mutex> lk(g_pathMutex);

            while (g_pathIdx < (int)g_pathTiles.size() - 1) {
                Vector3 wpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
                float dx = wpc.x - hereBody.x, dy = wpc.y - hereBody.y;
                if (sqrtf(dx*dx + dy*dy) <= g_stopRange)
                    ++g_pathIdx;
                else
                    break;
            }

            bool done = false;
            if (g_pathIdx >= (int)g_pathTiles.size()) {
                done = true;
            } else if (g_pathIdx == (int)g_pathTiles.size() - 1) {
                Vector2i bodyTile = WorldToTile(hereBody.x, hereBody.y, tileW);
                Vector2i finalTile = g_pathTiles.back();
                Vector3 wpc = PathWaypointWorld((int)g_pathTiles.size() - 1, hereBody.z, tileW);
                float dx = wpc.x - hereBody.x, dy = wpc.y - hereBody.y;
                float distFinal = sqrtf(dx*dx + dy*dy);

                if (bodyTile.x == finalTile.x && bodyTile.y == finalTile.y &&
                    distFinal <= g_stopRange) {
                    done = true;
                }
            }

            if (done) {
                g_isNavigating  = false;
                g_isTeleporting = false;
                PathFinder_RestoreGravity();
                PathFinder_ClearInputs();
            } else {
                Vector3 wpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
                float dx   = wpc.x - hereBody.x;
                float dy   = wpc.y - hereBody.y;
                float dist = sqrtf(dx*dx + dy*dy);

                if (g_moveMethod == MoveMethod::InputDrive) {

                    const float DEAD = 0.04f;
                    bool wantLeft  = (dx < -DEAD);
                    bool wantRight = (dx >  DEAD);
                    bool wantJump  = (dy >  DEAD);
                    PathFinder_WriteInputs(wantLeft, wantRight, wantJump);

                    if (dist > 0.001f && oSetVelocity) {
                        float horizSpeed = g_moveSpeed * g_flightScale;
                        float vertSpeed  = g_verticalSpeed;
                        float vx = (dx / dist) * horizSpeed;
                        float vy = (dy / dist) * vertSpeed;
                        if (dy < 0.0f) vy *= g_descentSpeedMul;
                        bool svOk = SafeSetVelocity(g_LocalPlayer, { vx, vy, 0.f });

                        static bool s_firstSvDone = false;
                        if (!s_firstSvDone) {
                            s_firstSvDone = true;
                            std::cout << "[PFDiag] FIRST INPUT-DRIVE SetVelocity"
                                      << " ok=" << (int)svOk
                                      << " vel={" << vx << "," << vy << ",0}"
                                      << std::endl;
                        }
                    }
                } else if (g_moveMethod == MoveMethod::Portal) {

                    TryPortalStep(g_pathTiles, g_pathIdx, g_portalIntervalMs, g_lastPortalSec);
                } else if (dist > 0.001f) {

                    TpApplyStandalone();
                    double nowSec = (double)ImGui::GetTime();

                    int    jitter   = (g_tp_tickJitterMs > 0) ? g_tp_tickJitterMs : 0;
                    int    jitterMs = (jitter > 0) ? ((rand() % (jitter * 2 + 1)) - jitter) : 0;
                    double dtTick   = (double)(g_tp_tickIntervalMs + (float)jitterMs) / 1000.0;
                    if (dtTick < 0.0) dtTick = 0.0;
                    if ((nowSec - g_tp_lastSec) < dtTick) {

                        if (g_tp_zeroVelocity)
                            SafeSetVelocity(g_LocalPlayer, { 0.f, 0.f, 0.f });
                    } else {
                        g_tp_lastSec = nowSec;

                        int last = (int)g_pathTiles.size() - 1;
                        if (last < 0) return;
                        if (g_pathIdx > last) g_pathIdx = last;
                        if (g_pathIdx < 0)    g_pathIdx = 0;

                        Vector3 jumpWpc = PathWaypointWorld(g_pathIdx, hereBody.z, tileW);
                        Vector3 newFeet = { jumpWpc.x, jumpWpc.y - bodyOff, jumpWpc.z };

                        void* t = SafeGetTransform(g_LocalPlayer);
                        bool spOk = SafeSetPosition(t, newFeet);

                        static bool s_firstTpDone = false;
                        if (!s_firstTpDone) {
                            s_firstTpDone = true;
                            std::cout << "[PFDiag] FIRST TELEPORT"
                                      << " t=" << t
                                      << " getTrOk=" << (t != nullptr)
                                      << " setPosOk=" << (int)spOk
                                      << " feet={" << newFeet.x << "," << newFeet.y << "," << newFeet.z << "}"
                                      << std::endl;
                        }

                        if (oGetKukouriCamInst && oForceUpdatePos) {
                            void* cam = oGetKukouriCamInst();
                            if (cam) oForceUpdatePos(cam);
                        }

                        if (g_tp_zeroVelocity)
                            SafeSetVelocity(g_LocalPlayer, { 0.f, 0.f, 0.f });

                        if (g_tp_throttleMovePackets && oSendPlayerPosition0_orig) {
                            g_tp_allowOnePacket.store(true);
                            SehInvokeSendPos0(oSendPlayerPosition0_orig);
                            g_tp_allowOnePacket.store(false);
                        }
                    }
                }
            }
        }

        if ((g_showPath || g_showVisualizer) && g_hasTarget && camOk &&
            g_originCached && oGetTransform && oGetPosition)
        {
            Vector3     hereFeet = oGetPosition(oGetTransform(g_LocalPlayer));

            Vector3     here = { hereFeet.x, hereFeet.y + tileW * g_bodyOffsetFrac, hereFeet.z };
            ImDrawList* dl   = ImGui::GetForegroundDrawList();

            std::lock_guard<std::mutex> lk(g_pathMutex);

            const bool failed = (g_pathStatus != PathStatus::Success);
            const float t = (float)ImGui::GetTime();

            struct PtS { ImVec2 p; bool ok; float dist; };
            std::vector<PtS> pts;
            pts.reserve(g_pathTiles.size() + 1);
            {
                Vector2 sH; bool oH = NavWorldToScreen(here, sH);
                pts.push_back({ ImVec2(sH.x, sH.y), oH, 0.f });
                ImVec2 prev = ImVec2(sH.x, sH.y);
                bool prevOk = oH;
                float acc = 0.f;
                for (int i = g_pathIdx; i < (int)g_pathTiles.size(); ++i) {
                    Vector3 wpc = PathWaypointWorld(i, here.z, tileW);
                    Vector2 cS; bool oC = NavWorldToScreen(wpc, cS);
                    ImVec2 cur(cS.x, cS.y);
                    if (oC && prevOk) {
                        float dx = cur.x - prev.x, dy = cur.y - prev.y;
                        acc += sqrtf(dx*dx + dy*dy);
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

            if (g_pathGlow && !failed && pts.size() > 1) {
                for (int pass = 0; pass < 3; ++pass) {
                    float thick  = 10.f - pass * 3.f;
                    int   alpha  = 30 + pass * 25;
                    for (size_t i = 1; i < pts.size(); ++i) {
                        if (!pts[i-1].ok || !pts[i].ok) continue;

                        float hue = pts[i].dist / fmaxf(totalLen, 1.f) * 0.55f
                                    + t * 0.05f + 0.55f;
                        dl->AddLine(pts[i-1].p, pts[i].p,
                                    hsv(hue, 0.85f, 1.f, alpha / 255.f),
                                    thick);
                    }
                }
            }

            if (pts.size() > 1) {
                for (size_t i = 1; i < pts.size(); ++i) {
                    if (!pts[i-1].ok || !pts[i].ok) continue;
                    if (failed) {
                        dl->AddLine(pts[i-1].p, pts[i].p,
                                    IM_COL32(255, 70, 70, 240), 3.f);
                    } else {
                        float hue = pts[i].dist / fmaxf(totalLen, 1.f) * 0.55f
                                    + t * 0.05f + 0.55f;
                        dl->AddLine(pts[i-1].p, pts[i].p,
                                    hsv(hue, 0.4f, 1.f, 0.9f), 2.f);
                    }
                }

                for (size_t i = 1; i < pts.size(); ++i) {
                    if (!pts[i].ok) continue;
                    float hue = pts[i].dist / fmaxf(totalLen, 1.f) * 0.55f
                                + t * 0.05f + 0.55f;
                    dl->AddCircleFilled(pts[i].p, 2.5f,
                        failed ? IM_COL32(255, 90, 90, 220)
                               : hsv(hue, 0.3f, 1.f, 0.85f));
                }
            }

            if (g_pathFlow && !failed && totalLen > 1.f) {
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
                    ImVec2 p = ImVec2(
                        pts[cursor-1].p.x + (pts[cursor].p.x - pts[cursor-1].p.x) * u,
                        pts[cursor-1].p.y + (pts[cursor].p.y - pts[cursor-1].p.y) * u);
                    float hue = d / fmaxf(totalLen, 1.f) * 0.55f + t * 0.15f + 0.5f;

                    dl->AddCircleFilled(p, 5.f, hsv(hue, 0.5f, 1.f, 0.25f));
                    dl->AddCircleFilled(p, 2.5f, hsv(hue, 0.7f, 1.f, 1.f));
                }
            }

            if (failed) {
                Vector2 lblS;
                if (NavWorldToScreen(g_navTarget, lblS)) {
                    char buf[64];
                    snprintf(buf, sizeof(buf), "X  %s", PathStatusText(g_pathStatus));
                    dl->AddText(ImVec2(lblS.x + 11.f, lblS.y - 19.f),
                                IM_COL32(0, 0, 0, 220), buf);
                    dl->AddText(ImVec2(lblS.x + 10.f, lblS.y - 20.f),
                                IM_COL32(255, 90, 90, 255), buf);
                }
            }

            Vector2 targetS;
            if (NavWorldToScreen(g_navTarget, targetS)) {
                ImVec2 tp(targetS.x, targetS.y);
                ImU32 ringCol = failed ? IM_COL32(255, 80, 80, 220)
                                       : IM_COL32(255, 215, 100, 255);

                dl->AddCircle(tp, 9.f, ringCol, 24, 2.f);

                if (g_pathPulse) {
                    for (int r = 0; r < 3; ++r) {
                        float phase = fmodf(t * 0.9f + r * 0.33f, 1.f);
                        float rad   = 9.f + phase * 32.f;
                        int   a     = (int)((1.f - phase) * 180.f);
                        if (a > 0) {
                            ImU32 c = failed ? IM_COL32(255, 80, 80, a)
                                             : IM_COL32(255, 215, 100, a);
                            dl->AddCircle(tp, rad, c, 32, 1.5f);
                        }
                    }
                }

                for (int k = 0; k < 4; ++k) {
                    float ang = 1.5707963f * k;
                    float c = cosf(ang), s = sinf(ang);
                    dl->AddLine(ImVec2(tp.x + c * 7,  tp.y + s * 7),
                                ImVec2(tp.x + c * 13, tp.y + s * 13),
                                ringCol, 2.f);
                }
            }

            Vector2 playerS;
            if (NavWorldToScreen(here, playerS)) {
                ImVec2 pp(playerS.x, playerS.y);
                float breath = 0.5f + 0.5f * sinf(t * 3.f);
                ImU32 haloC = failed ? IM_COL32(255, 90, 90, (int)(60 * breath + 30))
                                     : IM_COL32(140, 220, 255, (int)(60 * breath + 30));
                dl->AddCircleFilled(pp, 7.f + breath * 3.f, haloC);
                dl->AddCircleFilled(pp, 4.f,
                    failed ? IM_COL32(255, 120, 120, 240)
                           : IM_COL32(150, 230, 255, 240));
            }

            if (g_isNavigating && g_pathIdx < (int)g_pathTiles.size()) {
                Vector3 wpc = PathWaypointWorld(g_pathIdx, here.z, tileW);
                Vector2 pS, wS;
                if (NavWorldToScreen(here, pS) && NavWorldToScreen(wpc, wS)) {
                    float adx = wS.x - pS.x, ady = wS.y - pS.y;
                    float alen = sqrtf(adx*adx + ady*ady);
                    if (alen > 4.f) {
                        float nx = adx/alen, ny = ady/alen;
                        float len = fminf(alen, scale * 0.4f);
                        ImVec2 tip = { pS.x + nx*len, pS.y + ny*len };
                        dl->AddLine(ImVec2(pS.x, pS.y), tip,
                                    IM_COL32(255, 255, 255, 220), 3.f);
                        float hl = len * 0.35f;
                        ImVec2 left = { tip.x - nx*hl + (-ny)*hl*0.5f,
                                        tip.y - ny*hl +   nx *hl*0.5f };
                        ImVec2 rght = { tip.x - nx*hl - (-ny)*hl*0.5f,
                                        tip.y - ny*hl -   nx *hl*0.5f };
                        dl->AddTriangleFilled(tip, left, rght,
                                             IM_COL32(255, 255, 255, 180));
                    }
                }
            }
        }
    }

    inline bool NavigateToTile(Vector2i tile) {
        if (!g_LocalPlayer || !g_WorldController || !oGetTransform || !oGetPosition)
            return false;

        if ((!g_originCached || g_needsTileSizeRecalc) && oConvertMapPointToWorldPoint_V3) {
            CacheWorldOrigin_Safe(g_WorldController);
        }

        const float tileW = (g_tileSize.x > 0.001f) ? g_tileSize.x : 0.32f;

        Vector3  playerPos = oGetPosition(oGetTransform(g_LocalPlayer));

        Vector2i startTile = WorldToTile(playerPos.x, playerPos.y + tileW * 0.5f, tileW);

        void* worldObj = g_off_WCWorld
            ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld)
            : nullptr;
        if (!worldObj) return false;

        PathResult result = BuildPath(g_WorldController, worldObj, startTile, tile);

        {
            std::lock_guard<std::mutex> lk(g_pathMutex);
            g_pathTiles   = std::move(result.tiles);
            g_pathOffsets = std::move(result.offsets);
            g_pathStatus  = result.status;
            g_pathIdx     = 0;
        }

        Vector3 tileCtr = TileToWorldCenter(tile.x, tile.y, playerPos.z, tileW);
        g_navTarget     = tileCtr;
        g_hasTarget     = true;

        bool ok         = (result.status == PathStatus::Success);
        g_isNavigating  = ok;
        g_isTeleporting = ok;
        if (ok) {
            PathFinder_DisableGravity();
            if (g_moveMethod == MoveMethod::InputDrive) PathFinder_PulseJumpDown();
            std::cout << "[Minimap] NavigateToTile (" << tile.x << "," << tile.y
                      << ") OK; start=(" << startTile.x << "," << startTile.y
                      << ") tiles=" << result.tiles.size()
                      << " tileW=" << tileW << "\n";
        } else {
            PathFinder_RestoreGravity();
            std::cout << "[Minimap] NavigateToTile (" << tile.x << "," << tile.y
                      << ") FAILED status=" << PathStatusText(result.status)
                      << " start=(" << startTile.x << "," << startTile.y
                      << ") tileW=" << tileW << "\n";
        }
        return ok;
    }

}

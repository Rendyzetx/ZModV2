#pragma once
#include "PWMath.h"
#include <algorithm>
#include <mutex>
#include "imgui/imgui.h"
#include "minimap.h"
#define NOMINMAX 
#include <algorithm>
#include <Windows.h>
#include <iostream>
#include <vector>

struct Vector2i;
struct GemstoneInfo;
struct LightCrystalInfo;
struct PotInfo;
struct TreasureInfo;

extern std::mutex g_AIInstancesMutex;
namespace ESP
{
   
#pragma pack(push, 1)
    struct CollectableData
    {
        char             pad_0000[0x10];
        int              id;
        World::BlockType blockType;
        int              inventoryItemType;
        char             pad_001C[0x4];
        void* inventoryData;
        float            posX; 
        float            posY; 
        Vector2i         mapPoint;
        short            amount;
        bool             isGem;
    };

    struct CollectablesList
    {
        char pad_0000[0x10];
        CollectableData** items;
        char pad_0018[0x8];
        int size;
    };
#pragma pack(pop)

    inline bool bMasterSwitch = false;
    inline bool bEnableDebugInfo = false;

    inline bool bEnablePlayerBoxes = false;
    inline int boxType = 1;
    inline bool bBoxFill = false;
    inline float boxThickness = 1.0f;
    inline ImVec4 boxColor = ImVec4(0.093f, 0.0f, 1.0f, 1.0f);
    inline ImVec4 boxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
    inline bool bEnableSnaplines = false;
    inline int lineOrigin = 0;
    inline float lineThickness = 1.0f;
    inline ImVec4 lineColor = ImVec4(0.093f, 0.0f, 1.0f, 1.0f);
    inline bool bEnablePlayerHealthESP = false;
    inline int PlayerHealthStyle = 0;
    inline ImVec4 healthColorHigh = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    inline ImVec4 healthColorLow = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    inline bool bEnableAIBoxes = false;
    inline int AIBoxType = 1;
    inline bool bAIBoxFill = false;
    inline float AIBoxThickness = 1.0f;
    inline ImVec4 AIBoxColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    inline ImVec4 AIBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
    inline bool bEnableAILines = false;
    inline int AILineOrigin = 0;
    inline float ailineThickness = 1.0f;
    inline ImVec4 AIlineColor = ImVec4(1.0f, 1.0f, 1.0f, 0.6f);

    inline constexpr int kNetherBossWraithType = 46;
    inline bool bEnableNetherBossLines = false;
    inline bool bEnableNetherBossBoxes = false;
    inline ImVec4 NetherBossColor = ImVec4(0.75f, 0.30f, 1.0f, 1.0f);

    inline bool bEnableGemstoneBoxes = false;
    inline bool bEnableGemstoneLines = false;
    inline int GemstoneLineOrigin = 0;
    inline float gemSnapThickness = 1.0f;
    inline ImVec4 gemSnapColor = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
    inline int gemBoxType = 1;
    inline float gemBoxThickness = 1.0f;
    inline bool bGemBoxFill = false;
    inline ImVec4 gemBoxColor = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
    inline ImVec4 gemBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);

    inline std::map<World::BlockType, bool> g_enabledGemstones = {
        {World::BlockType::MiningGemStoneDiamond, true},
        {World::BlockType::MiningGemStoneRuby, true},
        {World::BlockType::MiningGemStoneEmerald, true},
        {World::BlockType::MiningGemStoneSapphire, true},
        {World::BlockType::MiningGemStoneAmethyst, true},
        {World::BlockType::MiningGemStoneTopaz, true}
    };

    inline bool bEnableCollectableBoxes = false;
    inline bool bEnableCollectableLines = false;
    inline int collectableLineOrigin = 0;
    inline float collectableLineThickness = 1.0f;
    inline int collectableBoxType = 1;
    inline float collectableBoxThickness = 1.0f;
    inline bool bCollectableBoxFill = false;
    inline ImVec4 collectableBoxColor = ImVec4(1.0f, 0.84f, 0.0f, 1.0f);
    inline ImVec4 collectableLineColor = ImVec4(1.0f, 0.84f, 0.0f, 0.8f);
    inline ImVec4 collectableBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);

    inline bool bEnableLocalPlayerBoxes = true;
    inline int localPlayerBoxType = 1;
    inline bool bLocalPlayerBoxFill = false;
    inline float localPlayerBoxThickness = 1.0f;
    inline ImVec4 localPlayerBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
    inline bool bEnableLocalPlayerHealthESP = true;
    inline int localPlayerHealthStyle = 0;
    inline ImVec4 localPlayerHealthColorHigh = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    inline ImVec4 localPlayerHealthColorLow = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    inline bool bEnableGiftBoxLines = false;
    inline int giftBoxLineOrigin = 0;
    inline float giftBoxLineThickness = 1.0f;
    inline ImVec4 giftBoxLineColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    inline bool bEnableGiftBoxBoxes = false;
    inline int giftBoxBoxType = 1;
    inline bool bGiftBoxBoxFill = false;
    inline float giftBoxBoxThickness = 1.0f;
    inline ImVec4 giftBoxBoxColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    inline ImVec4 giftBoxBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
    inline bool bEnableExitPortalLines = false;
    inline int exitPortalLineOrigin = 0;
    inline float exitPortalLineThickness = 1.0f;
    inline ImVec4 exitPortalLineColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
    inline bool bEnableExitPortalBoxes = false;
    inline int exitPortalBoxType = 1;
    inline bool bExitPortalBoxFill = false;
    inline float exitPortalBoxThickness = 1.0f;
    inline ImVec4 exitPortalBoxColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
    inline ImVec4 exitPortalBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);

    inline bool bEnableLightCrystalBoxes = false;
    inline bool bEnableLightCrystalLines = false;
    inline int LightCrystalLineOrigin = 0;
    inline float lightCrystalLineThickness = 1.0f;
    inline ImVec4 lightCrystalLineColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
    inline int lightCrystalBoxType = 1;
    inline float lightCrystalBoxThickness = 1.0f;
    inline bool bLightCrystalBoxFill = false;
    inline ImVec4 lightCrystalBoxColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
    inline ImVec4 lightCrystalBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
    inline std::map<World::BlockType, bool> g_enabledLightCrystals = {
        {World::BlockType::OrbLightingMining, true},
        {World::BlockType::MiningLightCrystalSmall, true},
        {World::BlockType::MiningLightCrystalMedium, true},
        {World::BlockType::MiningLightCrystalLarge, true},
        {World::BlockType::MiningTimeCrystalSmall, true},
        {World::BlockType::MiningTimeCrystalMedium, true},
        {World::BlockType::MiningTimeCrystalLarge, true}
    };

    inline bool bEnablePotBoxes = false;
    inline bool bEnablePotLines = false;
    inline int PotLineOrigin = 0;
    inline float potLineThickness = 1.0f;
    inline ImVec4 potLineColor = ImVec4(0.5f, 0.25f, 0.0f, 1.0f);
    inline int potBoxType = 1;
    inline float potBoxThickness = 1.0f;
    inline bool bPotBoxFill = false;
    inline ImVec4 potBoxColor = ImVec4(0.5f, 0.25f, 0.0f, 1.0f);
    inline ImVec4 potBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);

    inline bool bEnableTreasureBoxes = false;
    inline bool bEnableTreasureLines = false;
    inline int TreasureLineOrigin = 0;
    inline float treasureLineThickness = 1.0f;
    inline ImVec4 treasureLineColor = ImVec4(1.0f, 0.84f, 0.0f, 1.0f);
    inline int treasureBoxType = 1;
    inline float treasureBoxThickness = 1.0f;
    inline bool bTreasureBoxFill = false;
    inline ImVec4 treasureBoxColor = ImVec4(1.0f, 0.84f, 0.0f, 1.0f);
    inline ImVec4 treasureBoxFillColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);

    const float ESP_Y_OFFSET = 0.35f;
    const float ESP_WIDTH_RATIO = 1.8f;
    inline float AI_Box_Height_Multiplier = 0.3f;
    inline float AI_Box_Width_Multiplier = 0.3f;

    inline bool bEnablePathfinder = true;
    const float TILE_WORLD_SIZE = 0.32f;

    typedef int(__fastcall* tGetHealth)(void* instance);
    inline tGetHealth oGetHealth = nullptr;

    typedef int(__fastcall* tGetMaxHealth)(void* instance);
    inline tGetMaxHealth oGetMaxHealth = nullptr;

    typedef bool(__fastcall* tShouldShowAIEnemyInAISpawnerMenu)(int enemyType);
    inline tShouldShowAIEnemyInAISpawnerMenu oShouldShowAI = nullptr;

    inline bool WorldToScreen_2D(const Vector3& world_pos, Vector2& screen_pos) {
        if (!g_cachedCamValid.load(std::memory_order_acquire)) return false;
        const float camX     = g_cachedCamX    .load(std::memory_order_relaxed);
        const float camY     = g_cachedCamY    .load(std::memory_order_relaxed);
        const float orthoSize = g_cachedCamOrtho.load(std::memory_order_relaxed);
        if (orthoSize <= 0.0f) return false;

        const ImGuiIO& io = ImGui::GetIO();
        const float scale = io.DisplaySize.y / (orthoSize * 2.0f);
        screen_pos.x = (world_pos.x - camX) *  scale + (io.DisplaySize.x / 2.0f);
        screen_pos.y = (world_pos.y - camY) * -scale + (io.DisplaySize.y / 2.0f);
        return true;
    }

    inline float g_tileSize = 0.32f;
    inline bool g_tileSizeCalculated = false;

    inline void CalculateTileSize() {
        if (g_tileSizeCalculated || !g_WorldController || !oConvertMapPointToWorldPoint_V3) {
            return;
        }

        Vector3 worldPos_0_0 = oConvertMapPointToWorldPoint_V3(g_WorldController, { 0, 0 });
        Vector3 worldPos_1_0 = oConvertMapPointToWorldPoint_V3(g_WorldController, { 1, 0 });

        float size = abs(worldPos_1_0.x - worldPos_0_0.x);

        if (size > 0.001f) {
            g_tileSize = size;
            g_tileSizeCalculated = true;
        }
    }

   

    inline void RenderESP()
    {
        if (!bMasterSwitch || !ImGui::GetCurrentContext())
            return;

        void* localPlayer   = nullptr;
        void* worldController = nullptr;
        {
            std::lock_guard<std::mutex> unityLock(g_UnityObjectMutex);
            if (!g_IsInWorld) return;
            localPlayer     = g_LocalPlayer;
            worldController = g_WorldController;
        }
        if (!localPlayer || !worldController) return;

        std::vector<std::pair<void*, Vector3>> playerSnapshot;
        {
            std::lock_guard<std::mutex> lock(g_PlayerInstancesMutex);
            playerSnapshot.reserve(g_PlayerInstances.size());
            for (void* p : g_PlayerInstances) {
                auto it = g_playerPosCache.find(p);
                if (it != g_playerPosCache.end())
                    playerSnapshot.push_back({ p, it->second });
            }
        }
        std::vector<std::pair<void*, Vector3>> aiSnapshot;
        {
            std::lock_guard<std::mutex> lock(g_AIInstancesMutex);
            aiSnapshot.reserve(g_AIInstances.size());
            for (void* p : g_AIInstances) {
                auto it = g_aiPosCache.find(p);
                if (it == g_aiPosCache.end()) continue;

                AIBase* base = *(AIBase**)((uintptr_t)p + g_off_AIBase);
                if (base && !IsBadReadPtr(base, sizeof(AIBase)) &&
                    IsProjectileEnemyType((int)base->enemyType))
                    continue;
                aiSnapshot.push_back({ p, it->second });
            }
        }

        std::vector<GemstoneInfo> gemSnapshot;
        { std::lock_guard<std::mutex> lk(g_gemstoneLocationsMutex);    gemSnapshot     = g_gemstoneLocations;     }
        std::vector<LightCrystalInfo> crystalSnapshot;
        { std::lock_guard<std::mutex> lk(g_lightCrystalLocationsMutex); crystalSnapshot = g_lightCrystalLocations; }
        std::vector<PotInfo> potSnapshot;
        { std::lock_guard<std::mutex> lk(g_potLocationsMutex);          potSnapshot     = g_potLocations;          }
        std::vector<TreasureInfo> treasureSnapshot;
        { std::lock_guard<std::mutex> lk(g_treasureLocationsMutex);     treasureSnapshot = g_treasureLocations;    }
        std::vector<Vector2i> giftBoxSnapshot;
        { std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);      giftBoxSnapshot  = g_giftBoxLocations;     }
        std::vector<Vector2i> exitPortalSnapshot;
        { std::lock_guard<std::mutex> lk(g_exitPortalLocationsMutex);   exitPortalSnapshot = g_exitPortalLocations; }

        if (g_isTransitioning.load(std::memory_order_acquire)) return;

        const bool anyESPEnabled =
            bEnablePlayerBoxes || bEnableSnaplines || bEnablePlayerHealthESP ||
            bEnableAIBoxes     || bEnableAILines   ||
            bEnableNetherBossBoxes || bEnableNetherBossLines       ||
            bEnableLocalPlayerBoxes || bEnableLocalPlayerHealthESP ||
            bEnableGemstoneBoxes    || bEnableGemstoneLines        ||
            bEnableCollectableBoxes || bEnableCollectableLines      ||
            bEnableLightCrystalBoxes || bEnableLightCrystalLines   ||
            bEnablePotBoxes   || bEnablePotLines   ||
            bEnableTreasureBoxes || bEnableTreasureLines           ||
            bEnableGiftBoxLines  || bEnableGiftBoxBoxes            ||
            bEnableExitPortalLines || bEnableExitPortalBoxes       ||
            bEnableDebugInfo;
        if (!anyESPEnabled) return;

        if (!g_cachedCamValid.load(std::memory_order_acquire)) return;

        Vector3 localPlayerPos;
        {
            std::lock_guard<std::mutex> lk(g_PlayerInstancesMutex);
            auto it = g_playerPosCache.find(localPlayer);
            if (it == g_playerPosCache.end()) return;
            localPlayerPos = it->second;
        }

        Vector2 localPlayerScreenPos;
        bool isLocalPlayerOnScreen = WorldToScreen_2D(localPlayerPos, localPlayerScreenPos);

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        ImGuiIO& io = ImGui::GetIO();

        auto get_line_start = [&](int origin_setting) -> ImVec2 {
            if (origin_setting == 0 && isLocalPlayerOnScreen) {
                return ImVec2(localPlayerScreenPos.x, localPlayerScreenPos.y);
            }
            if (origin_setting == 1) return ImVec2(io.DisplaySize.x / 2, 0);
            if (origin_setting == 2) return ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y);

            return ImVec2(io.DisplaySize.x / 2, 0);
            };

        if (bEnableDebugInfo)
        {
            int dbgAI = 0, dbgAICache = 0;
            { std::lock_guard<std::mutex> lk(g_AIInstancesMutex); dbgAI = (int)g_AIInstances.size(); dbgAICache = (int)g_aiPosCache.size(); }
            int dbgPlayers = 0;
            { std::lock_guard<std::mutex> lk(g_PlayerInstancesMutex); dbgPlayers = (int)g_PlayerInstances.size(); }
            int dbgGiftBoxes = 0, dbgGems = 0, dbgCrystals = 0, dbgPots = 0, dbgTreasures = 0, dbgPortals = 0;
            { std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);      dbgGiftBoxes  = (int)g_giftBoxLocations.size(); }
            { std::lock_guard<std::mutex> lk(g_gemstoneLocationsMutex);     dbgGems       = (int)g_gemstoneLocations.size(); }
            { std::lock_guard<std::mutex> lk(g_lightCrystalLocationsMutex); dbgCrystals   = (int)g_lightCrystalLocations.size(); }
            { std::lock_guard<std::mutex> lk(g_potLocationsMutex);          dbgPots       = (int)g_potLocations.size(); }
            { std::lock_guard<std::mutex> lk(g_treasureLocationsMutex);     dbgTreasures  = (int)g_treasureLocations.size(); }
            { std::lock_guard<std::mutex> lk(g_exitPortalLocationsMutex);   dbgPortals    = (int)g_exitPortalLocations.size(); }

            float camX = g_cachedCamX.load(std::memory_order_relaxed);
            float camY = g_cachedCamY.load(std::memory_order_relaxed);
            float ortho = g_cachedCamOrtho.load(std::memory_order_relaxed);

            const float dbgLineH = ImGui::GetTextLineHeight();
            const ImU32 dbgShadow = IM_COL32(0, 0, 0, 200);
            const ImU32 dbgColor  = IM_COL32(255, 220, 0, 255);
            float dbgY = 10.0f;
            auto dbgLine = [&](const char* txt) {
                drawList->AddText(ImVec2(11, dbgY + 1), dbgShadow, txt);
                drawList->AddText(ImVec2(10, dbgY),     dbgColor,  txt);
                dbgY += dbgLineH + 2.0f;
            };

            void* dbgWorldObj = nullptr;
            int   dbgWorldSzX = 0, dbgWorldSzY = 0;
            void* dbgBlockLayer = nullptr;
            if (worldController && !IsBadReadPtr(worldController, g_off_WCWorld + sizeof(void*))) {
                dbgWorldObj = *(void**)((uintptr_t)worldController + g_off_WCWorld);
                if (dbgWorldObj && !IsBadReadPtr(dbgWorldObj, g_off_WorldBlockLayer + sizeof(void*))) {
                    void* blk = *(void**)((uintptr_t)dbgWorldObj + g_off_WorldBlockLayer);
                    dbgBlockLayer = blk;
                    if (blk && !IsBadReadPtr(blk, 0x20)) {

                        int outerLen = *(int*)((uintptr_t)blk + 0x18);
                        void** rows = (void**)((uintptr_t)blk + 0x20);
                        dbgWorldSzY = outerLen;
                        for (int _i = 0; _i < outerLen && dbgWorldSzX == 0; ++_i)
                            if (rows[_i]) dbgWorldSzX = *(int*)((uintptr_t)rows[_i] + 0x18);
                    }
                }
            }

            char buf[192];
            dbgLine("[ZMod Debug]");
            snprintf(buf, sizeof(buf), "camValid=%d  cam=(%.2f,%.2f)  ortho=%.2f",
                (int)g_cachedCamValid.load(), camX, camY, ortho);
            dbgLine(buf);
            snprintf(buf, sizeof(buf), "players=%d  AI=%d (cache=%d)", dbgPlayers, dbgAI, dbgAICache);
            dbgLine(buf);
            snprintf(buf, sizeof(buf), "giftBoxes=%d  exitPortals=%d  gems=%d", dbgGiftBoxes, dbgPortals, dbgGems);
            dbgLine(buf);
            snprintf(buf, sizeof(buf), "crystals=%d  pots=%d  treasures=%d", dbgCrystals, dbgPots, dbgTreasures);
            dbgLine(buf);

            snprintf(buf, sizeof(buf), "worldObj=%s  size=(%d,%d)  blockLayer=%s",
                dbgWorldObj   ? "OK" : "NULL",
                dbgWorldSzX, dbgWorldSzY,
                dbgBlockLayer ? "OK" : "NULL");
            dbgLine(buf);
        }

        if (bEnablePlayerBoxes || bEnableSnaplines || bEnablePlayerHealthESP)
        {
            for (auto& [player, footPos] : playerSnapshot)
            {
                if (!player || player == localPlayer) continue;
                Vector3 headPos = { footPos.x, footPos.y + ESP_Y_OFFSET, footPos.z };
                Vector2 screenFoot, screenHead;
                if (!WorldToScreen_2D(footPos, screenFoot) || !WorldToScreen_2D(headPos, screenHead)) continue;
                float height = abs(screenFoot.y - screenHead.y);
                float width = height / ESP_WIDTH_RATIO;
                float left = screenHead.x - (width / 2.0f);
                float top = screenHead.y;
                float right = left + width;
                float bottom = top + height;

                if (bEnablePlayerBoxes) {
                    ImU32 col = ImGui::ColorConvertFloat4ToU32(boxColor);
                    if (bBoxFill && boxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(boxFillColor));
                    if (boxType == 1) {
                        float cornerLength = width / 4.0f;
                        drawList->AddLine(ImVec2(left, top), ImVec2(left + cornerLength, top), col, boxThickness);
                        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + cornerLength), col, boxThickness);
                        drawList->AddLine(ImVec2(right - cornerLength, top), ImVec2(right, top), col, boxThickness);
                        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + cornerLength), col, boxThickness);
                        drawList->AddLine(ImVec2(left, bottom - cornerLength), ImVec2(left, bottom), col, boxThickness);
                        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + cornerLength, bottom), col, boxThickness);
                        drawList->AddLine(ImVec2(right - cornerLength, bottom), ImVec2(right, bottom), col, boxThickness);
                        drawList->AddLine(ImVec2(right, bottom - cornerLength), ImVec2(right, bottom), col, boxThickness);
                    }
                    else if (boxType == 2) {
                        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, boxThickness);
                    }
                }
                if (bEnableSnaplines) {
                    drawList->AddLine(get_line_start(lineOrigin), ImVec2(screenFoot.x, screenFoot.y), ImGui::ColorConvertFloat4ToU32(lineColor), lineThickness);
                }
                if (bEnablePlayerHealthESP && oGetHealth && oGetMaxHealth) {
                    constexpr uintptr_t playerDataOffset = 0x50;
                    void* playerData = *(void**)((uintptr_t)player + playerDataOffset);
                    if (!playerData) continue;
                    int currentHealth = oGetHealth(playerData);
                    int maxHealth = oGetMaxHealth(playerData);
                    if (maxHealth <= 0 || currentHealth < 0) continue;
                    float healthPercent = (float)currentHealth / (float)maxHealth;
                    if (healthPercent > 1.0f) healthPercent = 1.0f;
                    ImVec4 healthColVec;
                    healthColVec.x = healthColorLow.x + (healthColorHigh.x - healthColorLow.x) * healthPercent;
                    healthColVec.y = healthColorLow.y + (healthColorHigh.y - healthColorLow.y) * healthPercent;
                    healthColVec.z = healthColorLow.z + (healthColorHigh.z - healthColorLow.z) * healthPercent;
                    healthColVec.w = 1.0f;
                    ImU32 healthCol = ImGui::ColorConvertFloat4ToU32(healthColVec);
                    if (PlayerHealthStyle == 0 || PlayerHealthStyle == 3) {
                        float barHeight = height * healthPercent;
                        drawList->AddRectFilled(ImVec2(left - 7, top), ImVec2(left - 3, bottom), IM_COL32(0, 0, 0, 180));
                        drawList->AddRectFilled(ImVec2(left - 7, bottom - barHeight), ImVec2(left - 3, bottom), healthCol);
                        drawList->AddRect(ImVec2(left - 7, top), ImVec2(left - 3, bottom), IM_COL32_BLACK);
                    }
                    if (PlayerHealthStyle == 1) {
                        float barWidth = width * healthPercent;
                        drawList->AddRectFilled(ImVec2(left, bottom + 3), ImVec2(right, bottom + 7), IM_COL32(0, 0, 0, 180));
                        drawList->AddRectFilled(ImVec2(left, bottom + 3), ImVec2(left + barWidth, bottom + 7), healthCol);
                        drawList->AddRect(ImVec2(left, bottom + 3), ImVec2(right, bottom + 7), IM_COL32_BLACK);
                    }
                    if (PlayerHealthStyle == 2 || PlayerHealthStyle == 3) {
                        int displayHealth = currentHealth / 10;
                        char buffer[16];
                        snprintf(buffer, sizeof(buffer), "%d", displayHealth);
                        ImVec2 textSize = ImGui::CalcTextSize(buffer);
                        ImVec2 textPos = (PlayerHealthStyle == 3) ? ImVec2(left - 5 - textSize.x / 2, top + (height / 2) - (textSize.y / 2)) : ImVec2(right + 5, top);
                        drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32_BLACK, buffer);
                        drawList->AddText(textPos, healthCol, buffer);
                    }
                }
            }
        }

        if (bEnableAIBoxes || bEnableAILines || bEnableNetherBossBoxes || bEnableNetherBossLines)
        {
            const float aiOrtho = g_cachedCamOrtho.load(std::memory_order_relaxed);
            const float aiScale = (aiOrtho > 0.0f) ? (io.DisplaySize.y / (aiOrtho * 2.0f)) : 0.0f;
            for (auto& [aiInstance, centerPos] : aiSnapshot)
            {
                if (!aiInstance) continue;

                Vector2 screenCenter;
                if (!WorldToScreen_2D(centerPos, screenCenter)) continue;

                float pixelWidth  = 20.0f;
                float pixelHeight = 20.0f;
                if (aiScale > 0.0f) {
                    pixelWidth  = AI_Box_Width_Multiplier  * aiScale;
                    pixelHeight = AI_Box_Height_Multiplier * aiScale;
                }

                int enemyType = -1;
                if (!IsBadReadPtr((void*)((uintptr_t)aiInstance + g_off_AIBase), sizeof(void*))) {
                    void* aiBase = *(void**)((uintptr_t)aiInstance + g_off_AIBase);
                    if (aiBase && !IsBadReadPtr(aiBase, 0x24)) {
                        enemyType = *(int*)((uintptr_t)aiBase + Offsets::AIBase_enemyType);

                        if (aiScale > 0.0f) {
                            int w = *(int*)((uintptr_t)aiBase + 0x1C);
                            int h = *(int*)((uintptr_t)aiBase + 0x20);
                            if (w > 0 && w < 30) pixelWidth  = (float)w * AI_Box_Width_Multiplier  * aiScale;
                            if (h > 0 && h < 30) pixelHeight = (float)h * AI_Box_Height_Multiplier * aiScale;
                        }
                    }
                }

                const bool isNetherBoss = (enemyType == kNetherBossWraithType);
                if (isNetherBoss) {
                    if (!(bEnableNetherBossLines || bEnableNetherBossBoxes)) continue;
                } else {
                    if (!(bEnableAIBoxes || bEnableAILines)) continue;
                    if (oShouldShowAI && enemyType >= 0 && !oShouldShowAI(enemyType)) continue;
                }

                if (pixelWidth  <= 0.0f) pixelWidth  = 20.0f;
                if (pixelHeight <= 0.0f) pixelHeight = 20.0f;

                float left   = screenCenter.x - (pixelWidth  / 2.0f);
                float right  = screenCenter.x + (pixelWidth  / 2.0f);
                float top    = screenCenter.y - (pixelHeight / 2.0f);
                float bottom = screenCenter.y + (pixelHeight / 2.0f);

                if (isNetherBoss ? bEnableNetherBossBoxes : bEnableAIBoxes)
                {
                    ImU32 col = ImGui::ColorConvertFloat4ToU32(isNetherBoss ? NetherBossColor : AIBoxColor);
                    if (bAIBoxFill && AIBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(AIBoxFillColor));
                    if (AIBoxType == 1) {
                        float corner = pixelWidth / 4.0f;
                        drawList->AddLine(ImVec2(left, top), ImVec2(left + corner, top), col, AIBoxThickness);
                        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + corner), col, AIBoxThickness);
                        drawList->AddLine(ImVec2(right - corner, top), ImVec2(right, top), col, AIBoxThickness);
                        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + corner), col, AIBoxThickness);
                        drawList->AddLine(ImVec2(left, bottom - corner), ImVec2(left, bottom), col, AIBoxThickness);
                        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + corner, bottom), col, AIBoxThickness);
                        drawList->AddLine(ImVec2(right - corner, bottom), ImVec2(right, bottom), col, AIBoxThickness);
                        drawList->AddLine(ImVec2(right, bottom - corner), ImVec2(right, bottom), col, AIBoxThickness);
                    }
                    else if (AIBoxType == 2) {
                        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, AIBoxThickness);
                    }
                }
                if (isNetherBoss ? bEnableNetherBossLines : bEnableAILines) {
                    ImU32 lineCol = ImGui::ColorConvertFloat4ToU32(isNetherBoss ? NetherBossColor : AIlineColor);
                    drawList->AddLine(get_line_start(AILineOrigin), ImVec2(screenCenter.x, bottom), lineCol, ailineThickness);
                }
            }
        }

        if ((g_isInMineworld || strstr(GetCurrentWorldName().c_str(), "MINEWORLD")) && (bEnableGemstoneBoxes || bEnableGemstoneLines))
        {
            if (oConvertMapPointToWorldPoint_V3)
            {
                float orthographicSize = g_cachedCamOrtho.load(std::memory_order_relaxed);
                if (orthographicSize > 0.0f) {
                    float scale = io.DisplaySize.y / (orthographicSize * 2.0f);
                    for (const auto& gemstone : gemSnapshot) {
                        if (g_enabledGemstones.count(gemstone.blockType) == 0 || !g_enabledGemstones.at(gemstone.blockType)) {
                            continue;
                        }
                        Vector3 worldPos = oConvertMapPointToWorldPoint_V3(worldController, gemstone.mapPoint);

                        worldPos.x += g_tileSize * 0.5f;
                        worldPos.y += g_tileSize * 0.5f;
                        Vector2 screenPos;
                        if (!WorldToScreen_2D(worldPos, screenPos)) continue;

                        if (bEnableGemstoneLines) {
                            drawList->AddLine(get_line_start(GemstoneLineOrigin), ImVec2(screenPos.x, screenPos.y), ImGui::ColorConvertFloat4ToU32(gemSnapColor), gemSnapThickness);
                        }

                        if (bEnableGemstoneBoxes) {
                            float boxWidth = scale * AI_Box_Width_Multiplier;
                            float boxHeight = scale * AI_Box_Height_Multiplier;
                            float left = screenPos.x - (boxWidth / 2.0f);
                            float right = screenPos.x + (boxWidth / 2.0f);
                            float top = screenPos.y - (boxHeight / 2.0f);
                            float bottom = screenPos.y + (boxHeight / 2.0f);
                            ImU32 col = ImGui::ColorConvertFloat4ToU32(gemBoxColor);
                            if (bGemBoxFill && gemBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(gemBoxFillColor));
                            if (gemBoxType == 1) {
                                float cornerLength = boxHeight / 3.0f;
                                drawList->AddLine(ImVec2(left, top), ImVec2(left + cornerLength, top), col, gemBoxThickness);
                                drawList->AddLine(ImVec2(left, top), ImVec2(left, top + cornerLength), col, gemBoxThickness);
                                drawList->AddLine(ImVec2(right - cornerLength, top), ImVec2(right, top), col, gemBoxThickness);
                                drawList->AddLine(ImVec2(right, top), ImVec2(right, top + cornerLength), col, gemBoxThickness);
                                drawList->AddLine(ImVec2(left, bottom - cornerLength), ImVec2(left, bottom), col, gemBoxThickness);
                                drawList->AddLine(ImVec2(left, bottom), ImVec2(left + cornerLength, bottom), col, gemBoxThickness);
                                drawList->AddLine(ImVec2(right - cornerLength, bottom), ImVec2(right, bottom), col, gemBoxThickness);
                                drawList->AddLine(ImVec2(right, bottom - cornerLength), ImVec2(right, bottom), col, gemBoxThickness);
                            }
                            else if (gemBoxType == 2) {
                                drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, gemBoxThickness);
                            }
                        }
                    }
                }
            }
        }

        if (bEnableCollectableBoxes || bEnableCollectableLines)
        {
            if (worldController && oConvertMapPointToWorldPoint_V3)
            {
                void* worldObject = *(void**)((uintptr_t)worldController + g_off_WCWorld);
                if (worldObject && !IsBadReadPtr(worldObject, sizeof(void*))) {
                    uintptr_t pCollectableList = *(uintptr_t*)((uintptr_t)worldObject + g_off_WorldCollectablesList);
                    if (pCollectableList && !IsBadReadPtr((void*)pCollectableList, 0x1C)) {
                        int size = *(int*)(pCollectableList + Offsets::List_size);
                        uintptr_t pItemsArray = *(uintptr_t*)(pCollectableList + Offsets::List_items);
                        if (size > 0 && size <= 10000 && pItemsArray && !IsBadReadPtr((void*)pItemsArray, 0x20 + sizeof(void*) * size)) {
                            for (int i = 0; i < size; i++) {
                                CollectableData* collectable = *(CollectableData**)(pItemsArray + 0x20 + (i * sizeof(void*)));
                                if (!collectable || IsBadReadPtr(collectable, sizeof(CollectableData))) continue;
                                Vector3 worldPos = oConvertMapPointToWorldPoint_V3(g_WorldController, collectable->mapPoint);
                                Vector2 screenCenter;
                                if (!WorldToScreen_2D(worldPos, screenCenter)) continue;
                                float orthographicSize = g_cachedCamOrtho.load(std::memory_order_relaxed);
                                if (orthographicSize <= 0.0f) continue;
                                float scale = io.DisplaySize.y / (orthographicSize * 2.0f);
                                float pixelHeight = (AI_Box_Height_Multiplier * 1.0f) * scale;
                                float pixelWidth = (AI_Box_Width_Multiplier * 1.0f) * scale;
                                if (pixelHeight <= 0.0f || pixelWidth <= 0.0f) continue;
                                float left = screenCenter.x - (pixelWidth / 2.0f);
                                float right = screenCenter.x + (pixelWidth / 2.0f);
                                float top = screenCenter.y - (pixelHeight / 2.0f);
                                float bottom = screenCenter.y + (pixelHeight / 2.0f);
                                if (bEnableCollectableBoxes) {
                                    ImU32 col = ImGui::ColorConvertFloat4ToU32(collectableBoxColor);
                                    if (bCollectableBoxFill && collectableBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(collectableBoxFillColor));
                                    if (collectableBoxType == 1) {
                                        float corner = pixelHeight / 3.0f;
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left + corner, top), col, collectableBoxThickness);
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + corner), col, collectableBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, top), ImVec2(right, top), col, collectableBoxThickness);
                                        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + corner), col, collectableBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom - corner), ImVec2(left, bottom), col, collectableBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + corner, bottom), col, collectableBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, bottom), ImVec2(right, bottom), col, collectableBoxThickness);
                                        drawList->AddLine(ImVec2(right, bottom - corner), ImVec2(right, bottom), col, collectableBoxThickness);
                                    }
                                    else if (collectableBoxType == 2) {
                                        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, collectableBoxThickness);
                                    }
                                }
                                if (bEnableCollectableLines) {
                                    drawList->AddLine(get_line_start(collectableLineOrigin), ImVec2(screenCenter.x, bottom), ImGui::ColorConvertFloat4ToU32(collectableLineColor), collectableLineThickness);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (bEnableLocalPlayerBoxes || bEnableLocalPlayerHealthESP)
        {
            if (localPlayer)
            {

                {
                    Vector3 footPos = localPlayerPos;
                    Vector3 headPos = { footPos.x, footPos.y + ESP_Y_OFFSET, footPos.z };
                    Vector2 screenFoot, screenHead;
                    if (WorldToScreen_2D(footPos, screenFoot) && WorldToScreen_2D(headPos, screenHead)) {
                        float height = abs(screenFoot.y - screenHead.y);
                        float width = height / ESP_WIDTH_RATIO;
                        float left = screenHead.x - (width / 2.0f);
                        float top = screenHead.y;
                        float right = left + width;
                        float bottom = top + height;
                        if (bEnableLocalPlayerBoxes) {
                            float hue = fmodf((float)ImGui::GetTime() * 0.25f, 1.0f);
                            ImVec4 rainbowColorVec;
                            ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, rainbowColorVec.x, rainbowColorVec.y, rainbowColorVec.z);
                            rainbowColorVec.w = 1.0f;
                            ImU32 col = ImGui::ColorConvertFloat4ToU32(rainbowColorVec);
                            if (bLocalPlayerBoxFill && localPlayerBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(localPlayerBoxFillColor));
                            if (localPlayerBoxType == 1) {
                                float cornerLength = width / 4.0f;
                                drawList->AddLine(ImVec2(left, top), ImVec2(left + cornerLength, top), col, localPlayerBoxThickness);
                                drawList->AddLine(ImVec2(left, top), ImVec2(left, top + cornerLength), col, localPlayerBoxThickness);
                                drawList->AddLine(ImVec2(right - cornerLength, top), ImVec2(right, top), col, localPlayerBoxThickness);
                                drawList->AddLine(ImVec2(right, top), ImVec2(right, top + cornerLength), col, localPlayerBoxThickness);
                                drawList->AddLine(ImVec2(left, bottom - cornerLength), ImVec2(left, bottom), col, localPlayerBoxThickness);
                                drawList->AddLine(ImVec2(left, bottom), ImVec2(left + cornerLength, bottom), col, localPlayerBoxThickness);
                                drawList->AddLine(ImVec2(right - cornerLength, bottom), ImVec2(right, bottom), col, localPlayerBoxThickness);
                                drawList->AddLine(ImVec2(right, bottom - cornerLength), ImVec2(right, bottom), col, localPlayerBoxThickness);
                            }
                            else if (localPlayerBoxType == 2) {
                                drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, localPlayerBoxThickness);
                            }
                        }
                        if (bEnableLocalPlayerHealthESP && oGetHealth && oGetMaxHealth) {
                            constexpr uintptr_t playerDataOffset = 0x50;
                            void* playerData = *(void**)((uintptr_t)localPlayer + playerDataOffset);
                            if (playerData) {
                                int currentHealth = oGetHealth(playerData);
                                int maxHealth = oGetMaxHealth(playerData);
                                if (maxHealth > 0 && currentHealth >= 0) {
                                    float healthPercent = (float)currentHealth / (float)maxHealth;
                                    if (healthPercent > 1.0f) healthPercent = 1.0f;
                                    ImVec4 healthColVec;
                                    healthColVec.x = localPlayerHealthColorLow.x + (localPlayerHealthColorHigh.x - localPlayerHealthColorLow.x) * healthPercent;
                                    healthColVec.y = localPlayerHealthColorLow.y + (localPlayerHealthColorHigh.y - localPlayerHealthColorLow.y) * healthPercent;
                                    healthColVec.z = localPlayerHealthColorLow.z + (localPlayerHealthColorHigh.z - localPlayerHealthColorLow.z) * healthPercent;
                                    healthColVec.w = 1.0f;
                                    ImU32 healthCol = ImGui::ColorConvertFloat4ToU32(healthColVec);
                                    if (localPlayerHealthStyle == 0 || localPlayerHealthStyle == 3) {
                                        float barHeight = height * healthPercent;
                                        drawList->AddRectFilled(ImVec2(left - 7, top), ImVec2(left - 3, bottom), IM_COL32(0, 0, 0, 180));
                                        drawList->AddRectFilled(ImVec2(left - 7, bottom - barHeight), ImVec2(left - 3, bottom), healthCol);
                                        drawList->AddRect(ImVec2(left - 7, top), ImVec2(left - 3, bottom), IM_COL32_BLACK);
                                    }
                                    if (localPlayerHealthStyle == 1) {
                                        float barWidth = width * healthPercent;
                                        drawList->AddRectFilled(ImVec2(left, bottom + 3), ImVec2(right, bottom + 7), IM_COL32(0, 0, 0, 180));
                                        drawList->AddRectFilled(ImVec2(left, bottom + 3), ImVec2(left + barWidth, bottom + 7), healthCol);
                                        drawList->AddRect(ImVec2(left, bottom + 3), ImVec2(right, bottom + 7), IM_COL32_BLACK);
                                    }
                                    if (localPlayerHealthStyle == 2 || localPlayerHealthStyle == 3) {
                                        int displayHealth = currentHealth / 10;
                                        char buffer[16];
                                        snprintf(buffer, sizeof(buffer), "%d", displayHealth);
                                        ImVec2 textSize = ImGui::CalcTextSize(buffer);
                                        ImVec2 textPos = (localPlayerHealthStyle == 3) ? ImVec2(left - 5 - textSize.x / 2, top + (height / 2) - (textSize.y / 2)) : ImVec2(right + 5, top);
                                        drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32_BLACK, buffer);
                                        drawList->AddText(textPos, healthCol, buffer);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (bEnableGiftBoxLines || bEnableGiftBoxBoxes)
        {
            if (g_WorldController && oConvertMapPointToWorldPoint_V3 && g_cachedCamValid.load(std::memory_order_acquire))
            {
                if (!giftBoxSnapshot.empty()) {
                    float orthographicSize = g_cachedCamOrtho.load(std::memory_order_relaxed);
                    if (orthographicSize > 0.0f) {
                        float scale = io.DisplaySize.y / (orthographicSize * 2.0f);
                        for (const auto& mapPoint : giftBoxSnapshot) {
                            Vector3 worldPos = oConvertMapPointToWorldPoint_V3(worldController, mapPoint);

                            worldPos.x += g_tileSize * 0.5f;
                            worldPos.y += g_tileSize * 0.5f;
                            Vector2 screenPos;
                            if (WorldToScreen_2D(worldPos, screenPos)) {
                                if (bEnableGiftBoxBoxes) {
                                    float boxHeight = scale * AI_Box_Height_Multiplier;
                                    float boxWidth = scale * AI_Box_Width_Multiplier;
                                    float left = screenPos.x - (boxWidth / 2.0f);
                                    float right = screenPos.x + (boxWidth / 2.0f);
                                    float top = screenPos.y - (boxHeight / 2.0f);
                                    float bottom = screenPos.y + (boxHeight / 2.0f);
                                    ImU32 col = ImGui::ColorConvertFloat4ToU32(giftBoxBoxColor);
                                    if (bGiftBoxBoxFill && giftBoxBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(giftBoxBoxFillColor));
                                    if (giftBoxBoxType == 1) {
                                        float corner = boxHeight / 3.0f;
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left + corner, top), col, giftBoxBoxThickness);
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + corner), col, giftBoxBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, top), ImVec2(right, top), col, giftBoxBoxThickness);
                                        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + corner), col, giftBoxBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom - corner), ImVec2(left, bottom), col, giftBoxBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + corner, bottom), col, giftBoxBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, bottom), ImVec2(right, bottom), col, giftBoxBoxThickness);
                                        drawList->AddLine(ImVec2(right, bottom - corner), ImVec2(right, bottom), col, giftBoxBoxThickness);
                                    }
                                    else if (giftBoxBoxType == 2) {
                                        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, giftBoxBoxThickness);
                                    }
                                }
                                if (bEnableGiftBoxLines) {
                                    drawList->AddLine(get_line_start(giftBoxLineOrigin), ImVec2(screenPos.x, screenPos.y), ImGui::ColorConvertFloat4ToU32(giftBoxLineColor), giftBoxLineThickness);
                                }
                            }
                        }
                    }
                }
            }
        }

        if ((bEnableExitPortalLines || bEnableExitPortalBoxes) && worldController && oConvertMapPointToWorldPoint_V3 && g_cachedCamValid.load(std::memory_order_acquire))
        {
            if (!exitPortalSnapshot.empty()) {
                float orthographicSize = g_cachedCamOrtho.load(std::memory_order_relaxed);
                if (orthographicSize > 0.0f) {
                    float scale = io.DisplaySize.y / (orthographicSize * 2.0f);
                    for (const auto& mapPoint : exitPortalSnapshot) {
                        Vector3 worldPos = oConvertMapPointToWorldPoint_V3(worldController, mapPoint);

                        worldPos.x += g_tileSize * 0.5f;
                        worldPos.y += g_tileSize * 0.5f;
                        Vector2 screenPos;
                        if (WorldToScreen_2D(worldPos, screenPos)) {
                            if (bEnableExitPortalBoxes) {
                                float boxHeight = scale * AI_Box_Height_Multiplier;
                                float boxWidth = scale * AI_Box_Width_Multiplier;
                                float left = screenPos.x - (boxWidth / 2.0f);
                                float right = screenPos.x + (boxWidth / 2.0f);
                                float top = screenPos.y - (boxHeight / 2.0f);
                                float bottom = screenPos.y + (boxHeight / 2.0f);
                                ImU32 col = ImGui::ColorConvertFloat4ToU32(exitPortalBoxColor);
                                if (bExitPortalBoxFill && exitPortalBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(exitPortalBoxFillColor));
                                if (exitPortalBoxType == 1) {
                                    float corner = boxHeight / 3.0f;
                                    drawList->AddLine(ImVec2(left, top), ImVec2(left + corner, top), col, exitPortalBoxThickness);
                                    drawList->AddLine(ImVec2(left, top), ImVec2(left, top + corner), col, exitPortalBoxThickness);
                                    drawList->AddLine(ImVec2(right - corner, top), ImVec2(right, top), col, exitPortalBoxThickness);
                                    drawList->AddLine(ImVec2(right, top), ImVec2(right, top + corner), col, exitPortalBoxThickness);
                                    drawList->AddLine(ImVec2(left, bottom - corner), ImVec2(left, bottom), col, exitPortalBoxThickness);
                                    drawList->AddLine(ImVec2(left, bottom), ImVec2(left + corner, bottom), col, exitPortalBoxThickness);
                                    drawList->AddLine(ImVec2(right - corner, bottom), ImVec2(right, bottom), col, exitPortalBoxThickness);
                                    drawList->AddLine(ImVec2(right, bottom - corner), ImVec2(right, bottom), col, exitPortalBoxThickness);
                                }
                                else if (exitPortalBoxType == 2) {
                                    drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, exitPortalBoxThickness);
                                }
                            }
                            if (bEnableExitPortalLines) {
                                drawList->AddLine(get_line_start(exitPortalLineOrigin), ImVec2(screenPos.x, screenPos.y), ImGui::ColorConvertFloat4ToU32(exitPortalLineColor), exitPortalLineThickness);
                            }
                        }
                    }
                }
            }
        }

        if (bEnableLightCrystalBoxes || bEnableLightCrystalLines)
        {
            if (worldController && oConvertMapPointToWorldPoint_V3 && g_cachedCamValid.load(std::memory_order_acquire))
            {
                if (!crystalSnapshot.empty()) {
                    float orthographicSize = g_cachedCamOrtho.load(std::memory_order_relaxed);
                    if (orthographicSize > 0.0f) {
                        float scale = io.DisplaySize.y / (orthographicSize * 2.0f);
                        for (const auto& crystal : crystalSnapshot) {
                            auto it = g_enabledLightCrystals.find(crystal.blockType);
                            if (it == g_enabledLightCrystals.end() || !it->second) {
                                continue;
                            }
                            Vector3 worldPos = oConvertMapPointToWorldPoint_V3(worldController, crystal.mapPoint);

                            worldPos.x += g_tileSize * 0.5f;
                            worldPos.y += g_tileSize * 0.5f;
                            Vector2 screenPos;
                            if (WorldToScreen_2D(worldPos, screenPos)) {
                                if (bEnableLightCrystalBoxes) {
                                    float boxHeight = scale * AI_Box_Height_Multiplier;
                                    float boxWidth = scale * AI_Box_Width_Multiplier;
                                    float left = screenPos.x - (boxWidth / 2.0f);
                                    float right = screenPos.x + (boxWidth / 2.0f);
                                    float top = screenPos.y - (boxHeight / 2.0f);
                                    float bottom = screenPos.y + (boxHeight / 2.0f);
                                    ImU32 col = ImGui::ColorConvertFloat4ToU32(lightCrystalBoxColor);
                                    if (bLightCrystalBoxFill && lightCrystalBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(lightCrystalBoxFillColor));
                                    if (lightCrystalBoxType == 1) {
                                        float corner = boxHeight / 3.0f;
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left + corner, top), col, lightCrystalBoxThickness);
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + corner), col, lightCrystalBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, top), ImVec2(right, top), col, lightCrystalBoxThickness);
                                        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + corner), col, lightCrystalBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom - corner), ImVec2(left, bottom), col, lightCrystalBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + corner, bottom), col, lightCrystalBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, bottom), ImVec2(right, bottom), col, lightCrystalBoxThickness);
                                        drawList->AddLine(ImVec2(right, bottom - corner), ImVec2(right, bottom), col, lightCrystalBoxThickness);
                                    }
                                    else if (lightCrystalBoxType == 2) {
                                        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, lightCrystalBoxThickness);
                                    }
                                }
                                if (bEnableLightCrystalLines) {
                                    drawList->AddLine(get_line_start(LightCrystalLineOrigin), ImVec2(screenPos.x, screenPos.y), ImGui::ColorConvertFloat4ToU32(lightCrystalLineColor), lightCrystalLineThickness);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (bEnablePotBoxes || bEnablePotLines)
        {
            if (worldController && oConvertMapPointToWorldPoint_V3 && g_cachedCamValid.load(std::memory_order_acquire))
            {
                if (!potSnapshot.empty()) {
                    float orthographicSize = g_cachedCamOrtho.load(std::memory_order_relaxed);
                    if (orthographicSize > 0.0f) {
                        float scale = io.DisplaySize.y / (orthographicSize * 2.0f);
                        for (const auto& pot : potSnapshot) {
                            Vector3 worldPos = oConvertMapPointToWorldPoint_V3(worldController, pot.mapPoint);

                            worldPos.x += g_tileSize * 0.5f;
                            worldPos.y += g_tileSize * 0.5f;
                            Vector2 screenPos;
                            if (WorldToScreen_2D(worldPos, screenPos)) {
                                if (bEnablePotBoxes) {
                                    float boxHeight = scale * AI_Box_Height_Multiplier;
                                    float boxWidth = scale * AI_Box_Width_Multiplier;
                                    float left = screenPos.x - (boxWidth / 2.0f);
                                    float right = screenPos.x + (boxWidth / 2.0f);
                                    float top = screenPos.y - (boxHeight / 2.0f);
                                    float bottom = screenPos.y + (boxHeight / 2.0f);
                                    ImU32 col = ImGui::ColorConvertFloat4ToU32(potBoxColor);
                                    if (bPotBoxFill && potBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(potBoxFillColor));
                                    if (potBoxType == 1) {
                                        float corner = boxHeight / 3.0f;
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left + corner, top), col, potBoxThickness);
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + corner), col, potBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, top), ImVec2(right, top), col, potBoxThickness);
                                        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + corner), col, potBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom - corner), ImVec2(left, bottom), col, potBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + corner, bottom), col, potBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, bottom), ImVec2(right, bottom), col, potBoxThickness);
                                        drawList->AddLine(ImVec2(right, bottom - corner), ImVec2(right, bottom), col, potBoxThickness);
                                    }
                                    else if (potBoxType == 2) {
                                        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, potBoxThickness);
                                    }
                                }
                                if (bEnablePotLines) {
                                    drawList->AddLine(get_line_start(PotLineOrigin), ImVec2(screenPos.x, screenPos.y), ImGui::ColorConvertFloat4ToU32(potLineColor), potLineThickness);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (bEnableTreasureBoxes || bEnableTreasureLines)
        {
            if (worldController && oConvertMapPointToWorldPoint_V3 && g_cachedCamValid.load(std::memory_order_acquire))
            {
                if (!treasureSnapshot.empty()) {
                    float orthographicSize = g_cachedCamOrtho.load(std::memory_order_relaxed);
                    if (orthographicSize > 0.0f) {
                        float scale = io.DisplaySize.y / (orthographicSize * 2.0f);
                        for (const auto& treasure : treasureSnapshot) {
                            Vector3 worldPos = oConvertMapPointToWorldPoint_V3(worldController, treasure.mapPoint);

                            worldPos.x += g_tileSize * 0.5f;
                            worldPos.y += g_tileSize * 0.5f;
                            Vector2 screenPos;
                            if (WorldToScreen_2D(worldPos, screenPos)) {
                                if (bEnableTreasureBoxes) {
                                    float boxHeight = scale * AI_Box_Height_Multiplier;
                                    float boxWidth = scale * AI_Box_Width_Multiplier;
                                    float left = screenPos.x - (boxWidth / 2.0f);
                                    float right = screenPos.x + (boxWidth / 2.0f);
                                    float top = screenPos.y - (boxHeight / 2.0f);
                                    float bottom = screenPos.y + (boxHeight / 2.0f);
                                    ImU32 col = ImGui::ColorConvertFloat4ToU32(treasureBoxColor);
                                    if (bTreasureBoxFill && treasureBoxType == 2) drawList->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertFloat4ToU32(treasureBoxFillColor));
                                    if (treasureBoxType == 1) {
                                        float corner = boxHeight / 3.0f;
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left + corner, top), col, treasureBoxThickness);
                                        drawList->AddLine(ImVec2(left, top), ImVec2(left, top + corner), col, treasureBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, top), ImVec2(right, top), col, treasureBoxThickness);
                                        drawList->AddLine(ImVec2(right, top), ImVec2(right, top + corner), col, treasureBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom - corner), ImVec2(left, bottom), col, treasureBoxThickness);
                                        drawList->AddLine(ImVec2(left, bottom), ImVec2(left + corner, bottom), col, treasureBoxThickness);
                                        drawList->AddLine(ImVec2(right - corner, bottom), ImVec2(right, bottom), col, treasureBoxThickness);
                                        drawList->AddLine(ImVec2(right, bottom - corner), ImVec2(right, bottom), col, treasureBoxThickness);
                                    }
                                    else if (treasureBoxType == 2) {
                                        drawList->AddRect(ImVec2(left, top), ImVec2(right, bottom), col, 0.0f, 0, treasureBoxThickness);
                                    }
                                }
                                if (bEnableTreasureLines) {
                                    drawList->AddLine(get_line_start(TreasureLineOrigin), ImVec2(screenPos.x, screenPos.y), ImGui::ColorConvertFloat4ToU32(treasureLineColor), treasureLineThickness);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    inline void SetupFunctionPointers()
    {

        DoFuncPtr(GetMethodPtrFromCandidates({
            {"Camera",        "get_orthographicSize"},
            {"KukouriCamera", "get_orthographicSize"},
            {"Camera",        "GetOrthographicSize"},
        }), oGetOrthographicSize, "Camera::get_orthographicSize");

        DoFuncPtr(GetMethodPtrFromCandidates({

            {"PlayerData",      "GetHealth"},
            {"PlayerData",      "get_Health"},
            {"PlayerData",      "GetCurrentHealth"},
            {"PlayerData",      "get_CurrentHealth"},
        }), oGetHealth, "PlayerData::GetHealth");

        DoFuncPtr(GetMethodPtrFromCandidates({
            {"PlayerData",      "GetMaxHitPoints"},
            {"PlayerData",      "GetMaxHealth"},
            {"PlayerData",      "get_MaxHealth"},
            {"PlayerData",      "GetMaximumHealth"},
            {"PlayerData",      "get_MaximumHealth"},
        }), oGetMaxHealth, "PlayerData::GetMaxHitPoints");

        DoFuncPtr(GetMethodPtrFromCandidates({
            {"AIEnemyConfigData", "ShouldShowAIEnemyInAISpawnerMenu"},
            {"ConfigData",        "ShouldShowAIEnemyInAISpawnerMenu"},
            {"ConfigData",        "ShouldShowAIEnemy"},
        }), oShouldShowAI, "AIEnemyConfigData::ShouldShowAIEnemyInAISpawnerMenu");

        DoFuncPtr(GetMethodPtrFromCandidates({
            {"ConfigData", "IsBlockGiftBox"},
        }), oIsBlockGiftBox, "ConfigData::IsBlockGiftBox");

    }

}
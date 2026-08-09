#pragma once

#include "main.h"
#include "PathFinding.h"
#include "imgui/imgui.h"
#include "XorStr.h"
#include <chrono>
#include <vector>
#include <climits>

namespace GemSeller {

    static constexpr int kGems[5][5] = {
        { 4012, 4013, 4014, 4015, 4016 },
        { 4017, 4018, 4019, 4020, 4021 },
        { 4032, 4033, 4034, 4035, 4036 },
        { 4037, 4038, 4039, 4040, 4041 },
        { 4047, 4048, 4049, 4050, 4051 },
    };
    static constexpr const char* kGemTypeNames[5] = {
        "Diamond", "Emerald", "Ruby", "Sapphire", "Topaz"
    };
    static constexpr const char* kGemSizeNames[5] = {
        "Tiny", "Small", "Medium", "Large", "Huge"
    };

    static constexpr int kGemPrices[5][5] = {
        { 20,  40,  80, 200, 400 },
        {  5,  10,  20,  50, 100 },
        { 15,  30,  60, 150, 300 },
        { 10,  20,  40, 100, 200 },
        {  3,   6,  12,  30,  90 },
    };

    static constexpr int   GEM_EXCHANGER_BLOCK_TYPE = 1568;
    static constexpr float kExchangerWorldX = 9.28f;
    static constexpr float kExchangerWorldY = 10.56f;

    static constexpr int   GEM_REACH_TILES  = 0;
    static constexpr int   CONSUMABLE_ITEM_TYPE = 7;
    static constexpr int   BANK_WALK_TIMEOUT_MS = 20000;

    inline const char* kRecycleAllWireId = "YnRg";

    inline bool g_sellMatrix[5][5] = {
        {true,true,true,true,true}, {true,true,true,true,true},
        {true,true,true,true,true}, {true,true,true,true,true},
        {true,true,true,true,true},
    };

    inline bool g_autoSell = true;
    inline int  g_sellCadenceMs = 500;
    inline std::chrono::steady_clock::time_point g_lastSellAttempt =
        std::chrono::steady_clock::now() - std::chrono::seconds(60);

    inline bool g_walking = false;
    inline std::chrono::steady_clock::time_point g_walkStartedAt;

    inline std::chrono::steady_clock::time_point g_walkAbandonedAt =
        std::chrono::steady_clock::now() - std::chrono::seconds(120);
    static constexpr int WALK_ABANDON_COOLDOWN_MS = 30000;

    inline char g_status[256] = "GemSeller: idle.";
    inline int  g_stat_recycles = 0;
    inline bool g_debug = false;

    inline int PackIK(int blockType, int itemType) {
        return (itemType << 24) | (blockType & 0xFFFFFF);
    }
    inline int Cheb(Vector2i a, Vector2i b) {
        int dx = a.x - b.x; if (dx < 0) dx = -dx;
        int dy = a.y - b.y; if (dy < 0) dy = -dy;
        return dx > dy ? dx : dy;
    }

    inline bool GetExchangerTile(Vector2i& outTile) {
        float tileW = PathRenderer::g_tileSize.x;
        if (tileW <= 0.001f) return false;
        if (!PathRenderer::g_originCached) return false;
        outTile = PathRenderer::WorldToTile(kExchangerWorldX, kExchangerWorldY, tileW);
        return true;
    }

    inline bool InReach() {
        if (!g_LocalPlayer) return false;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return false;
        Vector2i bt;
        if (!GetExchangerTile(bt)) return false;
        void* tr = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!tr) return false;
        Vector3 pos = PathRenderer::oGetPosition(tr);
        float tileW = (PathRenderer::g_tileSize.x > 0.001f)
            ? PathRenderer::g_tileSize.x : 0.32f;
        Vector2i pt = PathRenderer::WorldToTile(pos.x, pos.y + tileW * 0.5f, tileW);
        return Cheb(pt, bt) <= GEM_REACH_TILES;
    }

    inline int InventoryCountForBlock(int blockType, int itemType = CONSUMABLE_ITEM_TYPE) {
        if (!g_LocalPlayer || !oGetCountByKey) return 0;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return 0;
        PlayerData_InventoryKey k{ (World::BlockType)blockType,
                                   (InventoryItemType)itemType };
        short qty = 0;
        __try { qty = oGetCountByKey(pd, k); }
        __except (EXCEPTION_EXECUTE_HANDLER) { qty = 0; }
        return qty;
    }

    struct GemEntry {
        int blockType;
        int qty;
        int typeIdx;
        int sizeIdx;
    };
    inline std::vector<GemEntry> CollectSellable() {
        std::vector<GemEntry> out;
        for (int t = 0; t < 5; ++t) {
            for (int s = 0; s < 5; ++s) {
                if (!g_sellMatrix[t][s]) continue;
                int bt = kGems[t][s];
                int qty = InventoryCountForBlock(bt, CONSUMABLE_ITEM_TYPE);
                if (qty > 0) out.push_back({ bt, qty, t, s });
            }
        }
        return out;
    }

    inline bool HasGemsToSell() {
        for (int t = 0; t < 5; ++t)
            for (int s = 0; s < 5; ++s)
                if (g_sellMatrix[t][s] && InventoryCountForBlock(kGems[t][s]) > 0) return true;
        return false;
    }

    inline void _SafeRemoveItem(void* pd, PlayerData_InventoryKey k, int n) {
        __try { oRemoveItemsFromInventory(pd, k, n); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline void _SafeRefreshInv(void* invCtrl) {
        __try { oForceRefreshInventory(invCtrl); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    inline void ApplyLocalRecycleMirror(const std::vector<GemEntry>& gems) {
        if (!g_LocalPlayer || !oRemoveItemsFromInventory) return;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return;
        for (auto& g : gems) {
            PlayerData_InventoryKey k{
                (World::BlockType)g.blockType,
                (InventoryItemType)CONSUMABLE_ITEM_TYPE
            };
            _SafeRemoveItem(pd, k, g.qty);
        }
        if (g_InventoryControl && oForceRefreshInventory)
            _SafeRefreshInv(g_InventoryControl);
        if (g_debug)
            std::cout << XS("[GemSeller/Inv] -") << gems.size()
                      << XS(" gem stacks deducted locally\n");
    }

    inline int _SafeGetGemAmount(void* pd) {
        if (!oGetGemAmount || !pd) return 0;
        __try { return oGetGemAmount(pd); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    }
    inline void _SafeAddGems(void* pd, int delta) {
        if (!oAddGems || !pd || delta == 0) return;
        __try { oAddGems(pd, delta); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    inline bool _SendRecyclePerStack(const std::vector<GemEntry>& gems) {
        if (!oRecycleMiningGemstone) return false;
        int n = 0;
        for (auto& g : gems) {
            PlayerData_InventoryKey ik{ g.blockType,
                                        (InventoryItemType)CONSUMABLE_ITEM_TYPE };
            Seh_RecycleMiningGemstone(ik, (short)g.qty);
            ++n;
        }
        if (g_debug)
            std::cout << XS("[GemSeller] per-stack RecycleMiningGemstone x") << n << "\n";
        return n > 0;
    }

    static inline void* _Seh_ObjectNew(void* klass) {
        if (!il2cpp_object_new || !klass) return nullptr;
        __try { return il2cpp_object_new(klass); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    }
    static inline void _Seh_InvokeCtor(void* methodPtr, void* thisPtr) {
        if (!methodPtr || !thisPtr) return;
        __try { ((void(__fastcall*)(void*))methodPtr)(thisPtr); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static inline void _Seh_InvokeAddIK(void* methodPtr, void* thisPtr,
                                        PlayerData_InventoryKey ik) {
        if (!methodPtr || !thisPtr) return;
        __try { ((void(__fastcall*)(void*, PlayerData_InventoryKey))methodPtr)(thisPtr, ik); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static inline void _Seh_InvokeAddInt(void* methodPtr, void* thisPtr, int v) {
        if (!methodPtr || !thisPtr) return;
        __try { ((void(__fastcall*)(void*, int))methodPtr)(thisPtr, v); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static inline void _Seh_InvokeRecycleAll(void* listIK, void* listInt) {
        if (!oRecycleAllMiningGemstone || !listIK || !listInt) return;
        __try { oRecycleAllMiningGemstone(listIK, listInt); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    static inline void* _MethodInfoFnPtr(const void* methodInfo) {
        if (!methodInfo) return nullptr;
        __try { return *(void**)methodInfo; }
        __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    }

    inline bool _TryBatchRecycle(const std::vector<GemEntry>& gems) {
        if (!oRecycleAllMiningGemstone ||
            !p_il2cpp_class_get_method_from_name ||
            !il2cpp_object_new ||
            !il2cpp_method_get_param ||
            !il2cpp_class_from_il2cpp_type) {
            return false;
        }
        void* omKlass = FindClassDynamic("OutgoingMessages");
        if (!omKlass) return false;
        const void* ramgMethod = p_il2cpp_class_get_method_from_name(
            omKlass, "RecycleAllMiningGemstone", 2);
        if (!ramgMethod) return false;
        void* type0 = il2cpp_method_get_param(ramgMethod, 0);
        void* type1 = il2cpp_method_get_param(ramgMethod, 1);
        if (!type0 || !type1) return false;
        void* listIkKlass  = il2cpp_class_from_il2cpp_type(type0);
        void* listIntKlass = il2cpp_class_from_il2cpp_type(type1);
        if (!listIkKlass || !listIntKlass) return false;
        const void* ikCtorMI  = p_il2cpp_class_get_method_from_name(listIkKlass,  ".ctor", 0);
        const void* intCtorMI = p_il2cpp_class_get_method_from_name(listIntKlass, ".ctor", 0);
        const void* ikAddMI   = p_il2cpp_class_get_method_from_name(listIkKlass,  "Add",   1);
        const void* intAddMI  = p_il2cpp_class_get_method_from_name(listIntKlass, "Add",   1);
        void* ikCtorPtr  = _MethodInfoFnPtr(ikCtorMI);
        void* intCtorPtr = _MethodInfoFnPtr(intCtorMI);
        void* ikAddPtr   = _MethodInfoFnPtr(ikAddMI);
        void* intAddPtr  = _MethodInfoFnPtr(intAddMI);
        if (!ikCtorPtr || !intCtorPtr || !ikAddPtr || !intAddPtr) return false;
        void* listIk  = _Seh_ObjectNew(listIkKlass);
        void* listInt = _Seh_ObjectNew(listIntKlass);
        if (!listIk || !listInt) return false;
        _Seh_InvokeCtor(ikCtorPtr,  listIk);
        _Seh_InvokeCtor(intCtorPtr, listInt);
        for (auto& g : gems) {
            PlayerData_InventoryKey ik{ g.blockType,
                                        (InventoryItemType)CONSUMABLE_ITEM_TYPE };
            _Seh_InvokeAddIK (ikAddPtr,  listIk,  ik);
            _Seh_InvokeAddInt(intAddPtr, listInt, g.qty);
        }
        _Seh_InvokeRecycleAll(listIk, listInt);
        return true;
    }

    inline void SendRecycleAll(const std::vector<GemEntry>& gems) {
        if (gems.empty()) return;

        int expectedDelta = 0;
        for (auto& g : gems) {
            if ((unsigned)g.typeIdx < 5 && (unsigned)g.sizeIdx < 5)
                expectedDelta += g.qty * kGemPrices[g.typeIdx][g.sizeIdx];
        }

        std::vector<std::pair<int,int>> bt_qty;
        bt_qty.reserve(gems.size());
        for (auto& g : gems) bt_qty.emplace_back(g.blockType, g.qty);

        ReplyDispatch::NoteSendRA(bt_qty, expectedDelta, kRecycleAllWireId);

        std::string keysCsv, amtsCsv;
        for (auto& g : gems) {
            if (!keysCsv.empty()) { keysCsv += ","; amtsCsv += ","; }
            keysCsv += std::to_string(PackIK(g.blockType, CONSUMABLE_ITEM_TYPE));
            amtsCsv += std::to_string(g.qty);
        }
        nlohmann::json j;
        j["ID"]   = kRecycleAllWireId;
        j["UsUz"] = keysCsv;
        j["iGON"] = amtsCsv;

        QueueRawBsonSend(j);

        ++g_stat_recycles;
        std::cout << XS("[GemSeller] lUre RecycleAll queued — ") << gems.size()
                  << XS(" stack(s), expected +") << expectedDelta
                  << XS(" gems — awaiting reply.\n");
    }

    inline int GemValueForBlock(int bt) {
        for (int t = 0; t < 5; ++t) {
            for (int s = 0; s < 5; ++s) {
                if (kGems[t][s] == bt) return kGemPrices[t][s];
            }
        }
        return 0;
    }

    static inline int _SehGetCountForBlock(int bt) {
        return InventoryCountForBlock(bt, CONSUMABLE_ITEM_TYPE);
    }

    static inline void _SehProbeCounts(int bt, int& outIt, int& outCnt) {
        outIt = -1; outCnt = 0;
        if (!g_LocalPlayer || !oGetCountByKey) return;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return;
        for (int it : { 7, 0, 2, 5, 9 }) {
            PlayerData_InventoryKey k{ (World::BlockType)bt, (InventoryItemType)it };
            short q = 0;
            __try { q = oGetCountByKey(pd, k); }
            __except (EXCEPTION_EXECUTE_HANDLER) { q = 0; }
            if (q > 0) { outIt = it; outCnt = q; return; }
        }
    }
    static inline bool _SehRemoveItems(void* pd, int bt, int it, int qty) {
        if (!oRemoveItemsFromInventory || !pd) return false;
        PlayerData_InventoryKey ik{ (World::BlockType)bt, (InventoryItemType)it };
        bool ok = true;
        __try { oRemoveItemsFromInventory(pd, ik, qty); }
        __except (EXCEPTION_EXECUTE_HANDLER) { ok = false; }
        return ok;
    }
    static inline void _SehRefreshInvUI() {

        void* invCtrl = Safe_GetInventoryControl_FromPlayer();
        if (invCtrl) g_InventoryControl = invCtrl;
        if (!invCtrl) return;

        if (oRefreshInventoryCache) {
            __try { oRefreshInventoryCache(invCtrl); }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
        if (oForceRefreshInventory) {
            __try { oForceRefreshInventory(invCtrl); }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
    }

    inline void OnRAReplyDelivered(const nlohmann::json& ) {

        std::vector<int> blocks, qtys;
        int totalDelta = 0;
        if (!ReplyDispatch::TakeRABatch(blocks, qtys, totalDelta)) return;
        if (blocks.empty() || !g_LocalPlayer) return;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + 0x58);
        if (!pd) return;

        _SafeAddGems(pd, totalDelta);

        for (size_t i = 0; i < blocks.size() && i < qtys.size(); ++i) {
            if (blocks[i] <= 0 || qtys[i] <= 0) continue;
            int probedIt = -1, probedCnt = 0;
            _SehProbeCounts(blocks[i], probedIt, probedCnt);
            int removeIt = probedIt >= 0 ? probedIt : CONSUMABLE_ITEM_TYPE;
            _SehRemoveItems(pd, blocks[i], removeIt, qtys[i]);
        }
        _SehRefreshInvUI();
        std::cout << XS("[ReplyMirror] RA confirmed — drained ") << blocks.size()
                  << XS(" gem stack(s), +") << totalDelta << XS(" gems\n");
    }

    inline void StopWalk() {
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
    }

    inline bool StartWalk() {
        if (!g_LocalPlayer || !g_WorldController) return false;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return false;
        void* tr = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!tr) return false;
        Vector3 pos = PathRenderer::oGetPosition(tr);
        float tileW = PathRenderer::g_tileSize.x > 0
            ? PathRenderer::g_tileSize.x : 0.32f;

        Vector2i startTile = PathRenderer::WorldToTile(
            pos.x, pos.y + tileW * 0.5f, tileW);
        Vector2i goalTile;
        if (!GetExchangerTile(goalTile)) return false;

        void* worldObj = g_off_WCWorld
            ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld)
            : nullptr;
        if (!worldObj) return false;

        PathRenderer::PathResult res = PathRenderer::BuildPath(
            g_WorldController, worldObj, startTile, goalTile);
        if (res.status != PathRenderer::PathStatus::Success) {
            std::cout << XS("[GemSeller] BuildPath FAIL status=")
                      << PathRenderer::PathStatusText(res.status)
                      << XS(" start=(") << startTile.x << "," << startTile.y << ")"
                      << XS(" goal=(") << goalTile.x << "," << goalTile.y << ")\n";
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
        PathRenderer::g_autoMove      = true;
        PathFinder_DisableGravity();
        g_walking = true;
        g_walkStartedAt = std::chrono::steady_clock::now();
        return true;
    }

    inline void SellAllNow() {
        if (!InReach()) {
            snprintf(g_status, sizeof(g_status),
                "GemSeller: not in reach (3 tiles). Walk to (%.2f, %.2f).",
                kExchangerWorldX, kExchangerWorldY);
            return;
        }
        auto gems = CollectSellable();
        if (gems.empty()) {
            snprintf(g_status, sizeof(g_status),
                "GemSeller: no selected gems in inventory.");
            return;
        }
        SendRecycleAll(gems);
        snprintf(g_status, sizeof(g_status),
            "GemSeller: RecycleAllMiningGem fired (%d gem types).",
            (int)gems.size());
    }

    inline bool AutoLoopTick() {
        if (!g_autoSell) {
            AutoMine::g_botBusy.store(false, std::memory_order_release);
            return false;
        }
        if (!HasGemsToSell()) {
            if (g_walking) StopWalk();
            AutoMine::g_botBusy.store(false, std::memory_order_release);
            return false;
        }
        if (InReach()) {

            AutoMine::g_botBusy.store(true, std::memory_order_release);
            StopWalk();
            auto now = std::chrono::steady_clock::now();
            auto sinceLast = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_lastSellAttempt).count();
            if (sinceLast < g_sellCadenceMs) return true;
            g_lastSellAttempt = now;
            auto gems = CollectSellable();
            if (gems.empty()) return false;
            SendRecycleAll(gems);
            return true;
        }

        AutoMine::g_botBusy.store(false, std::memory_order_release);
        auto now = std::chrono::steady_clock::now();
        if (!g_walking) {

            auto sinceAbandon = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_walkAbandonedAt).count();
            if (sinceAbandon < WALK_ABANDON_COOLDOWN_MS) return false;
            if (!StartWalk()) return false;
            return true;
        }
        auto walkMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_walkStartedAt).count();
        if (walkMs > BANK_WALK_TIMEOUT_MS) {
            std::cout << XS("[GemSeller] walk timeout — abandoning (cooldown ")
                      << WALK_ABANDON_COOLDOWN_MS / 1000 << XS("s)\n");
            StopWalk();
            g_walkAbandonedAt = now;
            return false;
        }
        return true;
    }

    inline void RenderUI() {
        ImGui::PushID("GemSeller");
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.6f, 1.0f, 1.0f, 1.0f), "Auto-Sell Gems");
        ImGui::Checkbox("Enable Auto-Sell##gsauto", &g_autoSell);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(
                "Runs as part of AM Auto-Loop in PIXELMINES — after deposit,\n"
                "before portal entry. Pathfinds to gem exchanger\n"
                "(9.28, 10.56) and fires RecycleAllMiningGem with every\n"
                "selected gem type/size present in inventory.");
        ImGui::SliderInt("Sell cadence (ms)##gscad",
                         &g_sellCadenceMs, 500, 5000, "%d ms");

        if (ImGui::TreeNode("Gems to sell##gs")) {
            ImGui::TextDisabled("Per-cell: pick exactly which type+size to sell.");
            char buf[64];
            for (int t = 0; t < 5; ++t) {
                ImGui::Text("%-9s", kGemTypeNames[t]);
                for (int s = 0; s < 5; ++s) {
                    ImGui::SameLine();
                    snprintf(buf, sizeof(buf), "%s##gsm%d_%d", kGemSizeNames[s], t, s);
                    ImGui::Checkbox(buf, &g_sellMatrix[t][s]);
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

}

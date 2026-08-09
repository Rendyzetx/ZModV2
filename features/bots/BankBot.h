#pragma once

#include "main.h"
#include "PathFinding.h"

#include "diagnostics/Diagnostics.h"
#include "imgui/imgui.h"
#include "XorStr.h"
#include <chrono>
#include <vector>
#include <unordered_set>
#include <climits>

namespace BankBot {

    static constexpr int kNuggets[5]   = { 4154, 4155, 4156, 4157, 4162 };
    static constexpr const char* kNuggetNames[5] = {
        "Bronze", "Silver", "Gold", "Platinum", "Dark"
    };

    static constexpr int kKeys[5]      = { 3969, 3970, 3971, 3972, 4170 };
    static constexpr const char* kKeyNames[5] = {
        "Bronze", "Silver", "Gold", "Platinum", "Darkstone"
    };
    static constexpr int kMaterials[9] = {
        4132,
        4131,
        4133,
        4134,
        4135,
        4136,
        4137,
        4139,
        4138,
    };
    static constexpr const char* kMaterialNames[9] = {
        "Rusty Nail", "Old Miner's Shoe", "Piece of Rope", "Tin Can",
        "Matchbox", "Miner's Bucket", "Gold Tooth", "Pocket Watch", "Silver Coin"
    };

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

    static constexpr int SAFEBOX_BLOCK_TYPE = 3576;
    static constexpr int CONSUMABLE_ITEM_TYPE = 7;

    static constexpr int BANK_REACH_TILES   = 0;

    inline bool g_selNuggets[5]   = {};
    inline bool g_selKeys[5]      = {};
    inline bool g_selMaterials[9] = {};

    inline bool g_depositMatrix[5][5] = {};

    static constexpr float kBankWorldX = 8.00f;
    static constexpr float kBankWorldY = 10.56f;

    inline Vector2i g_bankTile    = { 0, 0 };
    inline bool     g_bankTileSet = false;

    inline bool GetBankTile(Vector2i& outTile) {
        float tileW = PathRenderer::g_tileSize.x;
        if (tileW <= 0.001f) return false;
        if (!PathRenderer::g_originCached) return false;
        outTile = PathRenderer::WorldToTile(kBankWorldX, kBankWorldY, tileW);
        g_bankTile = outTile;
        g_bankTileSet = true;
        return true;
    }

    struct BankSlot {
        int ik     = 0;
        int amount = 0;
        int marker = 0;
    };
    inline std::vector<BankSlot> g_bankSlots;

    inline int g_bankMaxSlots = 50;
    inline std::chrono::steady_clock::time_point g_bankLastReply =
        std::chrono::steady_clock::now() - std::chrono::seconds(60);
    inline std::chrono::steady_clock::time_point g_lastGBankSent =
        std::chrono::steady_clock::now() - std::chrono::seconds(60);
    inline bool g_bankCacheValid = false;

    inline char g_bankStatus[256] = "Bank: idle.";
    inline int  g_stat_deposits = 0;

    inline bool g_bankDebug = false;

    typedef void (__fastcall* tSendGetBankItems)(Vector2i mp);
    typedef void (__fastcall* tSendAddItemToBank)(Vector2i mp,
        PlayerData_InventoryKey ik, void* itemData, int amount);
    typedef void (__fastcall* tSendTakeItemsFromBank)(Vector2i mp,
        PlayerData_InventoryKey ik, int amount, int slotIndex);

    typedef void (__fastcall* tSendModifyBank)(Vector2i mp,
        PlayerData_InventoryKey ik, int amount, int slotIndex);
    typedef void* (__fastcall* tGetInventoryDataPD)(void* playerData,
        PlayerData_InventoryKey ik);
    typedef int   (__fastcall* tGetBlockTypeInvItemType)(int blockType);

    inline tSendGetBankItems        oSendGetBankItems        = nullptr;
    inline tSendAddItemToBank       oSendAddItemToBank       = nullptr;
    inline tSendTakeItemsFromBank   oSendTakeItemsFromBank   = nullptr;
    inline tSendModifyBank          oSendModifyBank          = nullptr;
    inline tGetInventoryDataPD      oGetInventoryDataPD      = nullptr;
    inline tGetBlockTypeInvItemType oGetBlockTypeInvItemType = nullptr;
    inline bool g_bankResolved = false;

    inline void Resolve() {
        if (g_bankResolved) return;
        oSendGetBankItems = (tSendGetBankItems)IL2CPP::Class::Utils::GetMethodPointer(
            "OutgoingMessages", "SendGetBankItemsMessage", 1);
        oSendAddItemToBank = (tSendAddItemToBank)IL2CPP::Class::Utils::GetMethodPointer(
            "OutgoingMessages", "SendAddItemToBankMessage", 4);
        oSendTakeItemsFromBank = (tSendTakeItemsFromBank)IL2CPP::Class::Utils::GetMethodPointer(
            "OutgoingMessages", "SendTakeItemsFromBankMessage", 4);
        oSendModifyBank = (tSendModifyBank)IL2CPP::Class::Utils::GetMethodPointer(
            "OutgoingMessages", "SendModifyBankItemsAmountMessage", 4);
        oGetInventoryDataPD = (tGetInventoryDataPD)IL2CPP::Class::Utils::GetMethodPointer(
            "PlayerData", "GetInventoryData", 1);
        oGetBlockTypeInvItemType = (tGetBlockTypeInvItemType)IL2CPP::Class::Utils::GetMethodPointer(
            "ConfigData", "GetBlockTypeInventoryItemType", 1);
        g_bankResolved = true;
    }

    inline int PackIK(int blockType, int itemType) {
        return (itemType << 24) | (blockType & 0xFFFFFF);
    }
    inline int IK_BlockType(int ik) { return ik & 0xFFFFFF; }
    inline int IK_ItemType (int ik) { return (ik >> 24) & 0xFF; }

    inline int Cheb(Vector2i a, Vector2i b) {
        int dx = a.x - b.x; if (dx < 0) dx = -dx;
        int dy = a.y - b.y; if (dy < 0) dy = -dy;
        return dx > dy ? dx : dy;
    }
    inline bool InBankReach() {
        if (!g_LocalPlayer) return false;
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition) return false;
        Vector2i bt;
        if (!GetBankTile(bt)) return false;
        void* tr = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!tr) return false;
        Vector3 pos = PathRenderer::oGetPosition(tr);
        float tileW = (PathRenderer::g_tileSize.x > 0.001f)
            ? PathRenderer::g_tileSize.x : 0.32f;
        Vector2i pt = PathRenderer::WorldToTile(pos.x, pos.y + tileW * 0.5f, tileW);
        return Cheb(pt, bt) <= BANK_REACH_TILES;
    }

    inline void _SafeGetBankItems(Vector2i mp) {
        if (!oSendGetBankItems) return;
        __try { oSendGetBankItems(mp); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline void _SafeAddBank(Vector2i mp, PlayerData_InventoryKey ik,
                             void* data, int amount) {
        if (!oSendAddItemToBank) return;
        __try { oSendAddItemToBank(mp, ik, data, amount); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline void _SafeTakeBank(Vector2i mp, PlayerData_InventoryKey ik,
                              int amount, int slotIdx) {
        if (!oSendTakeItemsFromBank) return;
        __try { oSendTakeItemsFromBank(mp, ik, amount, slotIdx); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline void _SafeModifyBank(Vector2i mp, PlayerData_InventoryKey ik,
                                int amount, int slotIdx) {
        if (!oSendModifyBank) return;
        __try { oSendModifyBank(mp, ik, amount, slotIdx); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline void* _SafeGetInvData(void* pd, PlayerData_InventoryKey ik) {
        if (!oGetInventoryDataPD) return nullptr;
        __try { return oGetInventoryDataPD(pd, ik); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    }
    inline int _SafeBlockItemType(int bt) {
        if (!oGetBlockTypeInvItemType) return CONSUMABLE_ITEM_TYPE;
        __try { return oGetBlockTypeInvItemType(bt); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return CONSUMABLE_ITEM_TYPE; }
    }

    inline long long g_lastGBankReplyMs = 0;
    inline long long g_maxGBankReplyMs  = 0;
    inline int       g_gbankSendCount   = 0;
    inline int       g_gbankReplyCount  = 0;

    inline void RequestBankState() {
        Resolve();
        Vector2i bt;
        if (!GetBankTile(bt)) return;
        _SafeGetBankItems(bt);
        g_lastGBankSent = std::chrono::steady_clock::now();
        ++g_gbankSendCount;
        Diagnostics::Ev("bankbot_gbank_sent", {
            {"tile",  { bt.x, bt.y }},
            {"count", g_gbankSendCount}
        });
        if (g_bankDebug)
            std::cout << XS("[BankBot] GBank @ (") << bt.x << "," << bt.y << ")\n";
    }

    inline void DepositBlock(int blockType, int amount) {
        Resolve();
        Vector2i bt_;
        if (!GetBankTile(bt_)) return;
        if (!g_LocalPlayer) return;
        if (amount <= 0) return;
        int itemType = _SafeBlockItemType(blockType);
        if (itemType <= 0) itemType = CONSUMABLE_ITEM_TYPE;
        PlayerData_InventoryKey ik{ (World::BlockType)blockType,
                                    (InventoryItemType)itemType };
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return;

        void* data = _SafeGetInvData(pd, ik);
        _SafeAddBank(bt_, ik, data, amount);
        ++g_stat_deposits;
        if (g_bankDebug)
            std::cout << XS("[BankBot] ABank bt=") << blockType << XS(" it=") << itemType
                      << XS(" amt=") << amount << XS(" data=") << data << "\n";
    }

    inline int RemainingCapacityForIK(int ik) {
        if (!g_bankCacheValid) return 999;
        int cap = 0;
        bool sawSlot = false;
        for (auto& s : g_bankSlots) {
            if (s.ik == ik) {
                cap += (999 - s.amount);
                sawSlot = true;
            }
        }
        if (!sawSlot) {

            int free = g_bankMaxSlots - (int)g_bankSlots.size();
            if (free > 0) cap += free * 999;
        } else {

            int free = g_bankMaxSlots - (int)g_bankSlots.size();
            if (free > 0) cap += free * 999;
        }
        return cap < 0 ? 0 : cap;
    }

    inline void DepositBlockSplit(int blockType, int amountAvailable) {
        Resolve();
        Vector2i btBank;
        if (!GetBankTile(btBank) || amountAvailable <= 0) return;
        int itemType = _SafeBlockItemType(blockType);
        if (itemType <= 0) itemType = CONSUMABLE_ITEM_TYPE;
        int ik = PackIK(blockType, itemType);

        if (!g_LocalPlayer) return;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return;
        PlayerData_InventoryKey nativeIK{ (World::BlockType)blockType,
                                          (InventoryItemType)itemType };
        void* itemData = _SafeGetInvData(pd, nativeIK);

        int remaining = amountAvailable;

        for (size_t i = 0; i < g_bankSlots.size() && remaining > 0; ++i) {
            BankSlot& slot = g_bankSlots[i];
            if (slot.ik != ik) continue;
            if (slot.amount >= 999) continue;
            int room = 999 - slot.amount;
            int put = (remaining < room) ? remaining : room;
            int newTotal = slot.amount + put;
            if (g_bankDebug)
                std::cout << XS("[BankBot] MBank slot=") << i
                          << XS(" ik=") << ik << " " << slot.amount
                          << XS(" -> ") << newTotal << XS(" (added ") << put << ")\n";
            _SafeModifyBank(btBank, nativeIK, newTotal, (int)i);

            slot.amount = newTotal;
            remaining -= put;
            ++g_stat_deposits;
        }

        if (remaining > 0) {
            int freeSlots = g_bankMaxSlots - (int)g_bankSlots.size();
            if (freeSlots <= 0) {
                snprintf(g_bankStatus, sizeof(g_bankStatus),
                    "Bank: full — no free slots for bt=%d (kept %d in inv).",
                    blockType, remaining);
                return;
            }

            int put = (remaining < 999) ? remaining : 999;
            if (g_bankDebug)
                std::cout << XS("[BankBot] ABank ik=") << ik
                          << XS(" amt=") << put << XS(" (new slot)\n");
            _SafeAddBank(btBank, nativeIK, itemData, put);
            ++g_stat_deposits;

            BankSlot newSlot;
            newSlot.ik = ik;
            newSlot.amount = put;
            newSlot.marker = 0;
            g_bankSlots.push_back(newSlot);
            remaining -= put;
        }

        snprintf(g_bankStatus, sizeof(g_bankStatus),
            "Bank: deposited %d of %d for bt=%d (left in inv: %d).",
            (amountAvailable - remaining), amountAvailable, blockType, remaining);
    }

    inline bool IsBankReplyId(const std::string& id) {

        if (id == "rabg" || id == "cWxO") return true;
        if (id == "GLtr" || id == "HbKe" || id == "cZlN") return true;

        if (id == "GBank" || id == "ABank" || id == "MBank") return true;
        return false;
    }
    inline const nlohmann::json* GetBankReplyPayload(const nlohmann::json& j) {

        static const char* kKeys[] = { "yNqC", "BR" };
        for (const char* k : kKeys) {
            if (j.contains(k)) return &j[k];
        }
        return nullptr;
    }

    inline std::atomic<int> g_parseBankReplyForensicCount{ 0 };

    inline void ParseBankReply(const nlohmann::json& bankReply) {
        const bool forensic = g_parseBankReplyForensicCount.fetch_add(1) < 3;

        if (!bankReply.is_object()) {
            if (forensic) std::cout << XS("[BankBot] PBR: top-level not object\n");
            return;
        }
        const nlohmann::json* payloadPtr = GetBankReplyPayload(bankReply);
        if (!payloadPtr) {
            if (forensic) {
                std::cout << XS("[BankBot] PBR: no yNqC/BR field. keys=");
                for (auto it = bankReply.begin(); it != bankReply.end(); ++it)
                    std::cout << " " << it.key();
                std::cout << "\n";
            }
            return;
        }
        const auto& br = *payloadPtr;

        std::vector<uint8_t> bytes;
        if (br.is_binary()) {
            const auto& bin = br.get_binary();
            bytes.assign(bin.begin(), bin.end());
            if (forensic)
                std::cout << XS("[BankBot] PBR: shape=binary len=") << bytes.size() << "\n";
        } else if (br.is_object() && br.contains("bytes") && br["bytes"].is_array()) {
            bytes.reserve(br["bytes"].size());
            for (auto& b : br["bytes"]) {
                if (!b.is_number_integer()) {
                    if (forensic)
                        std::cout << XS("[BankBot] PBR: object[bytes] non-int element, "
                                     "type=") << b.type_name() << "\n";
                    return;
                }
                bytes.push_back((uint8_t)(int)b.get<long long>());
            }
            if (forensic)
                std::cout << XS("[BankBot] PBR: shape=object{bytes} len=") << bytes.size() << "\n";
        } else if (br.is_array()) {

            bytes.reserve(br.size());
            for (auto& b : br) {
                if (!b.is_number_integer()) {
                    if (forensic)
                        std::cout << XS("[BankBot] PBR: array non-int element, "
                                     "type=") << b.type_name() << "\n";
                    return;
                }
                bytes.push_back((uint8_t)(int)b.get<long long>());
            }
            if (forensic)
                std::cout << XS("[BankBot] PBR: shape=array len=") << bytes.size() << "\n";
        } else {

            std::cout << XS("[BankBot] PBR: BR shape unknown! type=") << (int)br.type()
                      << " (" << br.type_name() << ") "
                      << XS("is_string=") << br.is_string()
                      << XS(" is_object=") << br.is_object()
                      << XS(" is_array=") << br.is_array()
                      << XS(" is_binary=") << br.is_binary() << "\n";
            return;
        }

        if (g_bankDebug) {
            std::cout << XS("[BankBot] BR ") << bytes.size() << XS(" bytes:");
            for (size_t i = 0; i < bytes.size() && i < 80; ++i)
                std::cout << " " << (int)bytes[i];
            std::cout << "\n";
        }

        nlohmann::json inner;
        try {
            inner = nlohmann::json::from_bson(bytes);
        } catch (const std::exception& e) {

            std::cout << XS("[BankBot] PBR: from_bson FAILED (")
                      << bytes.size() << XS(" bytes): ") << e.what() << "\n";

            std::cout << XS("[BankBot] PBR: head:");
            for (size_t i = 0; i < bytes.size() && i < 64; ++i)
                std::cout << " " << (int)bytes[i];
            std::cout << "\n";
            return;
        } catch (...) {
            std::cout << XS("[BankBot] PBR: from_bson FAILED (unknown exception, ")
                      << bytes.size() << XS(" bytes)\n");
            return;
        }
        if (forensic) {
            std::cout << XS("[BankBot] PBR: inner type=") << inner.type_name()
                      << XS(" hasBankI=") << (inner.is_object() && inner.contains("BankI"))
                      << "\n";
        }

        std::vector<BankSlot> next;
        if (inner.is_object() && inner.contains("BankI")) {
            const auto& arr = inner["BankI"];

            auto pushTriplets = [&](const std::vector<long long>& vals) {
                for (size_t i = 0; i + 2 < vals.size(); i += 3) {
                    BankSlot s;
                    s.ik     = (int)vals[i];
                    s.amount = (int)vals[i+1];
                    s.marker = (int)vals[i+2];
                    next.push_back(s);
                }
            };
            std::vector<long long> vals;
            if (arr.is_array()) {
                for (auto& v : arr)
                    if (v.is_number_integer()) vals.push_back(v.get<long long>());
            } else if (arr.is_object()) {

                std::vector<std::pair<int,long long>> tmp;
                for (auto it = arr.begin(); it != arr.end(); ++it) {
                    if (!it.value().is_number_integer()) continue;
                    int idx = atoi(it.key().c_str());
                    tmp.push_back({ idx, it.value().get<long long>() });
                }
                std::sort(tmp.begin(), tmp.end(),
                    [](auto& a, auto& b){ return a.first < b.first; });
                for (auto& p : tmp) vals.push_back(p.second);
            }
            pushTriplets(vals);
        }

        g_bankSlots = std::move(next);
        g_bankCacheValid = true;
        auto replyTime = std::chrono::steady_clock::now();

        if (g_lastGBankSent > g_bankLastReply) {
            g_lastGBankReplyMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                replyTime - g_lastGBankSent).count();
            if (g_lastGBankReplyMs > g_maxGBankReplyMs)
                g_maxGBankReplyMs = g_lastGBankReplyMs;
        }
        g_bankLastReply = replyTime;
        ++g_gbankReplyCount;
        Diagnostics::Ev("bankbot_reply_received", {
            {"slots",     (int)g_bankSlots.size()},
            {"latencyMs", g_lastGBankReplyMs},
            {"sends",     g_gbankSendCount},
            {"replies",   g_gbankReplyCount}
        });
        if ((int)g_bankSlots.size() > g_bankMaxSlots)
            g_bankMaxSlots = (int)g_bankSlots.size();

        if (g_bankDebug) {
            std::cout << XS("[BankBot] cache ") << g_bankSlots.size() << "/"
                      << g_bankMaxSlots << " {";
            for (size_t i = 0; i < g_bankSlots.size(); ++i) {
                if (i) std::cout << ",";
                std::cout << XS(" ik=") << g_bankSlots[i].ik
                          << XS(" x") << g_bankSlots[i].amount;
            }
            std::cout << " }\n";
        }
    }

    inline std::atomic<int> g_onIncomingForensicCount{ 0 };

    inline void OnIncomingPacket(const nlohmann::json& j) {
        std::string id = j.value("ID", "");
        if (!IsBankReplyId(id)) return;

        bool success    = j.value("S", false);
        bool hasPayload = (GetBankReplyPayload(j) != nullptr);

        Diagnostics::Ev("bankbot_incoming", {
            {"id",         id},
            {"success",    success},
            {"hasPayload", hasPayload}
        });

        if (g_onIncomingForensicCount.fetch_add(1) < 5 || g_bankDebug) {
            std::cout << XS("[BankBot] incoming ") << id
                      << XS(" S=") << success
                      << XS(" hasPayload=") << hasPayload << "\n";
        }
        if (!success) return;
        ParseBankReply(j);

        bool isFetch    = (id == "GLtr"  || id == "GBank" || id == "rabg" || id == "cWxO");
        bool isDeposit  = (id == "HbKe"  || id == "ABank");
        bool isModify   = (id == "cZlN"  || id == "MBank");
        (void)isFetch;
        if (isDeposit) {
            snprintf(g_bankStatus, sizeof(g_bankStatus),
                "Bank: deposit confirmed (slots %d).", (int)g_bankSlots.size());
        } else if (isModify) {
            snprintf(g_bankStatus, sizeof(g_bankStatus),
                "Bank: modify confirmed (slots %d).", (int)g_bankSlots.size());
        } else {
            snprintf(g_bankStatus, sizeof(g_bankStatus),
                "Bank: state refreshed (slots %d).", (int)g_bankSlots.size());
        }
    }

    inline std::vector<int> CollectSelectedBlockTypes() {
        std::vector<int> out;
        for (int i = 0; i < 5; ++i) if (g_selNuggets[i])  out.push_back(kNuggets[i]);
        for (int i = 0; i < 5; ++i) if (g_selKeys[i])     out.push_back(kKeys[i]);
        for (int i = 0; i < 9; ++i) if (g_selMaterials[i])out.push_back(kMaterials[i]);

        for (int t = 0; t < 5; ++t)
            for (int s = 0; s < 5; ++s) if (g_depositMatrix[t][s])
                out.push_back(kGems[t][s]);
        return out;
    }

    inline int InventoryCountForBlock(int blockType) {
        if (!g_LocalPlayer || !oGetCountByKey) return 0;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return 0;

        for (int it : { 7, 0, 2 }) {
            PlayerData_InventoryKey k{ (World::BlockType)blockType,
                                       (InventoryItemType)it };
            short qty = 0;
            __try { qty = oGetCountByKey(pd, k); }
            __except (EXCEPTION_EXECUTE_HANDLER) { qty = 0; }
            if (qty > 0) return qty;
        }
        return 0;
    }

    inline int CountSelectedItemsInInventory() {
        int total = 0;
        for (int bt : CollectSelectedBlockTypes())
            total += InventoryCountForBlock(bt);
        return total;
    }

    inline void DepositAllSelectedNow() {
        Resolve();
        Vector2i bt;
        if (!GetBankTile(bt)) {
            snprintf(g_bankStatus, sizeof(g_bankStatus),
                "Bank: tile coords pending (origin/tileW not cached yet).");
            return;
        }
        if (!InBankReach()) {
            snprintf(g_bankStatus, sizeof(g_bankStatus),
                "Bank: not in reach (3 tiles). Walk to tile (%d,%d) "
                "world (%.2f,%.2f).",
                bt.x, bt.y, kBankWorldX, kBankWorldY);
            return;
        }

        RequestBankState();

        int deposited = 0;
        for (int bt : CollectSelectedBlockTypes()) {
            int qty = InventoryCountForBlock(bt);
            if (qty <= 0) continue;
            DepositBlockSplit(bt, qty);
            deposited++;
        }
        if (deposited == 0)
            snprintf(g_bankStatus, sizeof(g_bankStatus),
                "Bank: nothing selected/found to deposit.");
        else
            snprintf(g_bankStatus, sizeof(g_bankStatus),
                "Bank: fired %d deposit(s).", deposited);
    }

    inline void UpdatePicker() {}

    typedef void (__fastcall* tInitBankFromRawData)(void* self, Il2CppArray* rawBytes);
    inline tInitBankFromRawData g_orig_InitBankFromRawData = nullptr;

    inline void* g_safeBoxUI_self = nullptr;
    inline bool  g_safeBoxHookInstalled = false;
    inline std::atomic<int> g_safeBoxHookFireCount{ 0 };

    static constexpr uintptr_t kOff_SafeBox_iks         = 0x68;
    static constexpr uintptr_t kOff_SafeBox_amounts     = 0x70;
    static constexpr uintptr_t kOff_SafeBox_dataClasses = 0x78;

    inline void _SafeCallOrigInitBank(void* self, Il2CppArray* rawBytes) {
        if (!g_orig_InitBankFromRawData) return;
        __try { g_orig_InitBankFromRawData(self, rawBytes); }
        __except (EXCEPTION_EXECUTE_HANDLER) {  }
    }

    inline int _SafeSnapshotSafeBoxArrays(void* self,
                                          int* outIK_block,  
                                          int* outIK_item,   
                                          int* outAmt) {     
        if (!self) return 0;
        int nOut = 0;
        __try {
            Il2CppArray* iksArr  = *(Il2CppArray**)((uintptr_t)self + kOff_SafeBox_iks);
            Il2CppArray* amtsArr = *(Il2CppArray**)((uintptr_t)self + kOff_SafeBox_amounts);
            int nIks  = iksArr  ? (int)iksArr->max_length  : 0;
            int nAmts = amtsArr ? (int)amtsArr->max_length : 0;
            int nSlots = nIks < nAmts ? nIks : nAmts;
            if (nSlots > 64) nSlots = 64;
            if (nSlots > 0 && iksArr && amtsArr) {
                PlayerData_InventoryKey* ikArr = (PlayerData_InventoryKey*)iksArr->items;
                short* amtArr = (short*)amtsArr->items;
                for (int i = 0; i < nSlots; ++i) {
                    outIK_block[i] = ikArr[i].blockType;
                    outIK_item[i]  = ikArr[i].itemType;
                    outAmt[i]      = (int)amtArr[i];
                }
                nOut = nSlots;
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return -1;
        }
        return nOut;
    }

    inline void __fastcall hk_InitBankFromRawData(void* self, Il2CppArray* rawBytes) {
        const int n = g_safeBoxHookFireCount.fetch_add(1) + 1;
        g_safeBoxUI_self = self;

        const size_t rawLen = (rawBytes ? rawBytes->max_length : 0);

        _SafeCallOrigInitBank(self, rawBytes);

        int ikBlock[64]{};
        int ikItem[64]{};
        int amt[64]{};
        int nSlots = _SafeSnapshotSafeBoxArrays(self, ikBlock, ikItem, amt);
        if (nSlots < 0) {
            std::cout << "[BankBot/UI] hk_InitBankFromRawData SNAPSHOT AV "
                         "(self=" << self << ") — keeping previous cache.\n";
            return;
        }

        std::vector<BankSlot> next;
        next.reserve((size_t)nSlots);
        for (int i = 0; i < nSlots; ++i) {
            BankSlot s;
            s.ik     = PackIK(ikBlock[i], ikItem[i]);
            s.amount = amt[i];
            s.marker = 0;
            next.push_back(s);
        }

        g_bankSlots = std::move(next);
        g_bankCacheValid = true;

        auto replyTime = std::chrono::steady_clock::now();
        if (g_lastGBankSent > g_bankLastReply) {
            g_lastGBankReplyMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                replyTime - g_lastGBankSent).count();
            if (g_lastGBankReplyMs > g_maxGBankReplyMs)
                g_maxGBankReplyMs = g_lastGBankReplyMs;
        }
        g_bankLastReply = replyTime;
        ++g_gbankReplyCount;

        if ((int)g_bankSlots.size() > g_bankMaxSlots)
            g_bankMaxSlots = (int)g_bankSlots.size();

        Diagnostics::Ev("bankbot_ui_init_bank", {
            {"fire",      n},
            {"slots",     (int)g_bankSlots.size()},
            {"rawLen",    (int)rawLen},
            {"latencyMs", g_lastGBankReplyMs}
        });

        if (n <= 3 || g_bankDebug) {
            std::cout << "[BankBot/UI] InitBankFromRawData #" << n
                      << " self=" << self
                      << " rawLen=" << rawLen
                      << " slots=" << g_bankSlots.size()
                      << " latencyMs=" << g_lastGBankReplyMs << "\n";
            for (size_t i = 0; i < g_bankSlots.size(); ++i) {
                std::cout << "[BankBot/UI]   slot[" << i << "] ik=" << g_bankSlots[i].ik
                          << " (bt=" << IK_BlockType(g_bankSlots[i].ik)
                          << " it="  << IK_ItemType(g_bankSlots[i].ik)
                          << ") amt=" << g_bankSlots[i].amount << "\n";
            }
        } else {
            std::cout << "[BankBot/UI] InitBankFromRawData #" << n
                      << " slots=" << g_bankSlots.size()
                      << " latencyMs=" << g_lastGBankReplyMs << "\n";
        }
    }

    inline void InstallSafeBoxUIHook() {
        if (g_safeBoxHookInstalled) return;
        void* fn = GetMethodPtr("ItemSafeBoxUI", "InitBankFromRawData", 1);
        if (!fn) {
            std::cout << "[BankBot/UI] InstallSafeBoxUIHook: "
                         "ItemSafeBoxUI::InitBankFromRawData NOT FOUND in manifest "
                         "— bank cache will rely on legacy wire-ID parser "
                         "(broken under AC rotation). Add manifest entry "
                         "\"ItemSafeBoxUI$$InitBankFromRawData\" with RVA 0xEC4470.\n";
            return;
        }
        DoHook(fn, &hk_InitBankFromRawData,
               (void**)&g_orig_InitBankFromRawData,
               "ItemSafeBoxUI::InitBankFromRawData");
        g_safeBoxHookInstalled = true;
        std::cout << "[BankBot/UI] InitBankFromRawData hook installed @ " << fn
                  << " — rotation-immune cache path active.\n";
    }

    inline int g_useButtonValue = 2;
    inline int g_useButtonAltJump = 3;
    inline bool g_sendButtonUp = true;

    typedef void (__fastcall* tTryToToolWrenchBlock)(void* wcSelf, Vector2i mapPoint);
    typedef void (__fastcall* tItemSafeBoxClosePopup)(void* self);

    typedef void (__fastcall* tSetItemSafeBoxUIValues)(Vector2i mapPoint);

    typedef void (__fastcall* tPlayerOnButtonDown)(void* playerSelf, int button);
    typedef void (__fastcall* tPlayerOnButtonUp)  (void* playerSelf, int button);

    inline tTryToToolWrenchBlock   oTryToToolWrenchBlock   = nullptr;
    inline tItemSafeBoxClosePopup  oItemSafeBoxClosePopup  = nullptr;
    inline tSetItemSafeBoxUIValues oSetItemSafeBoxUIValues = nullptr;
    inline tPlayerOnButtonDown     oPlayerOnButtonDown     = nullptr;
    inline tPlayerOnButtonUp       oPlayerOnButtonUp       = nullptr;
    inline bool g_safeBoxUIControlsResolved = false;

    inline void ResolveSafeBoxUIControls() {
        if (g_safeBoxUIControlsResolved) return;
        oTryToToolWrenchBlock   = (tTryToToolWrenchBlock)IL2CPP::Class::Utils::
            GetMethodPointer("WorldController", "TryToToolWrenchBlock", 1);
        oItemSafeBoxClosePopup  = (tItemSafeBoxClosePopup)IL2CPP::Class::Utils::
            GetMethodPointer("ItemSafeBoxUI", "ClosePopup", 0);
        oSetItemSafeBoxUIValues = (tSetItemSafeBoxUIValues)IL2CPP::Class::Utils::
            GetMethodPointer("ItemSafeBoxUI", "SetItemSafeBoxUIValues", 1);
        oPlayerOnButtonDown     = (tPlayerOnButtonDown)IL2CPP::Class::Utils::
            GetMethodPointer("Player", "OnButtonDown", 1);
        oPlayerOnButtonUp       = (tPlayerOnButtonUp)IL2CPP::Class::Utils::
            GetMethodPointer("Player", "OnButtonUp", 1);
        g_safeBoxUIControlsResolved = true;
        std::cout << "[BankBot/UI] ResolveSafeBoxUIControls: "
                     "WC.TryToToolWrenchBlock=" << (oTryToToolWrenchBlock ? "OK" : "MISSING")
                  << " ClosePopup="             << (oItemSafeBoxClosePopup ? "OK" : "MISSING")
                  << " SetValues="              << (oSetItemSafeBoxUIValues ? "OK" : "MISSING")
                  << " Player.OnButtonDown="    << (oPlayerOnButtonDown ? "OK" : "MISSING")
                  << " Player.OnButtonUp="      << (oPlayerOnButtonUp ? "OK" : "MISSING")
                  << "\n";
    }

    inline void _SafeCallTryToolWrench(void* wc, Vector2i mp) {
        __try { oTryToToolWrenchBlock(wc, mp); }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            std::cout << "[BankBot/UI] WorldController.TryToToolWrenchBlock AV\n";
        }
    }
    inline void _SafeCallSetValues(Vector2i mp) {
        __try { oSetItemSafeBoxUIValues(mp); }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            std::cout << "[BankBot/UI] SetItemSafeBoxUIValues AV\n";
        }
    }
    inline void _SafeCallClosePopup(void* self) {
        __try { oItemSafeBoxClosePopup(self); }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            std::cout << "[BankBot/UI] ClosePopup AV\n";
        }
    }

    typedef void (__fastcall* tRootUI_OnOrOffMenu)(void* rootUI, void* typeObj);
    typedef void (__fastcall* tBaseMenuUI_ShowMenu)(void* self, bool doInstant);
    typedef void* (__fastcall* tRootUI_GetMenufromCache)(void* rootUI, void* typeObj);
    typedef void (__fastcall* tTryToSecureForEditing)(Vector2i mp);
    inline tRootUI_OnOrOffMenu oRootUI_OnOrOffMenu           = nullptr;
    inline tRootUI_OnOrOffMenu oRootUI_OnOrOffMenuWhenPossible = nullptr;
    inline tBaseMenuUI_ShowMenu oBaseMenuUI_ShowMenu          = nullptr;
    inline tRootUI_GetMenufromCache oRootUI_GetMenufromCache  = nullptr;
    inline tTryToSecureForEditing   oTryToSecureForEditing    = nullptr;
    inline void* g_safeBoxTypeObject = nullptr;
    inline void* g_rootUIField       = nullptr;
    inline bool  g_onOrOffMenuPathResolved = false;

    inline bool g_useWhenPossibleVariant = true;

    inline bool g_skipSetValuesSeed = false;

    inline void ResolveOnOrOffMenuPath() {
        if (g_onOrOffMenuPathResolved) return;

        oRootUI_OnOrOffMenu = (tRootUI_OnOrOffMenu)IL2CPP::Class::Utils::
            GetMethodPointer("RootUI", "OnOrOffMenu", 1);
        oRootUI_OnOrOffMenuWhenPossible = (tRootUI_OnOrOffMenu)IL2CPP::Class::Utils::
            GetMethodPointer("RootUI", "OnOrOffMenuWhenPossible", 1);

        oBaseMenuUI_ShowMenu = (tBaseMenuUI_ShowMenu)IL2CPP::Class::Utils::
            GetMethodPointer("BaseMenuUI", "ShowMenu", 1);
        oRootUI_GetMenufromCache = (tRootUI_GetMenufromCache)IL2CPP::Class::Utils::
            GetMethodPointer("RootUI", "GetMenufromCache", 1);
        oTryToSecureForEditing = (tTryToSecureForEditing)IL2CPP::Class::Utils::
            GetMethodPointer("OutgoingMessages", "TryToSecureWorldItemForEditing", 1);
        if (!oBaseMenuUI_ShowMenu) {
            oBaseMenuUI_ShowMenu = (tBaseMenuUI_ShowMenu)IL2CPP::Class::Utils::
                GetMethodPointer("ItemSafeBoxUI", "ShowMenu", 1);
            if (oBaseMenuUI_ShowMenu) {
                std::cout << "[BankBot/UI]   resolved ShowMenu via "
                             "ItemSafeBoxUI inheritance fallback @ "
                          << (void*)oBaseMenuUI_ShowMenu << "\n";
            }
        }

        typedef void* (*tClassGetTypeFn)(void* klass);
        typedef void* (*tTypeGetObjectFn)(void* type);
        static tClassGetTypeFn  pClassGetTypeLocal  = nullptr;
        static tTypeGetObjectFn pTypeGetObjectLocal = nullptr;
        if (!pClassGetTypeLocal)
            pClassGetTypeLocal  = Manifest::GetApi<tClassGetTypeFn>("il2cpp_class_get_type");
        if (!pTypeGetObjectLocal)
            pTypeGetObjectLocal = Manifest::GetApi<tTypeGetObjectFn>("il2cpp_type_get_object");

        void* cItemSafeBoxUI = FindClassDynamic("ItemSafeBoxUI");
        std::cout << "[BankBot/UI]   diag: FindClassDynamic(ItemSafeBoxUI)="
                  << cItemSafeBoxUI
                  << " pClassGetType="  << (void*)pClassGetTypeLocal
                  << " pTypeGetObject=" << (void*)pTypeGetObjectLocal
                  << "\n";
        if (cItemSafeBoxUI && pClassGetTypeLocal && pTypeGetObjectLocal) {
            void* typeRaw = nullptr;
            __try { typeRaw = pClassGetTypeLocal(cItemSafeBoxUI); }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                std::cout << "[BankBot/UI]   diag: pClassGetType AV\n";
            }
            std::cout << "[BankBot/UI]   diag: pClassGetType -> typeRaw="
                      << typeRaw << "\n";
            if (typeRaw) {
                __try { g_safeBoxTypeObject = pTypeGetObjectLocal(typeRaw); }
                __except (EXCEPTION_EXECUTE_HANDLER) {
                    std::cout << "[BankBot/UI]   diag: pTypeGetObject AV\n";
                }
                std::cout << "[BankBot/UI]   diag: pTypeGetObject -> typeObject="
                          << g_safeBoxTypeObject << "\n";
            }
        }

        void* ctrlHelperCls = FindClassDynamic("ControllerHelper");
        if (ctrlHelperCls && il2cpp_class_get_field_from_name) {
            g_rootUIField = il2cpp_class_get_field_from_name(ctrlHelperCls, "rootUI");
        }

        g_onOrOffMenuPathResolved = true;
        std::cout << "[BankBot/UI] ResolveOnOrOffMenuPath: "
                     "RootUI.OnOrOffMenu="              << (oRootUI_OnOrOffMenu ? "OK" : "MISSING")
                  << " WhenPossible="                  << (oRootUI_OnOrOffMenuWhenPossible ? "OK" : "MISSING")
                  << " BaseMenuUI.ShowMenu="           << (oBaseMenuUI_ShowMenu ? "OK" : "MISSING")
                  << " ItemSafeBoxUI.Type object="     << (g_safeBoxTypeObject ? "OK" : "MISSING")
                  << " ControllerHelper.rootUI field=" << (g_rootUIField ? "OK" : "MISSING")
                  << "\n";
    }

    inline void* GetRootUI() {
        if (!g_rootUIField || !il2cpp_field_static_get_value) return nullptr;
        void* instance = nullptr;
        il2cpp_field_static_get_value(g_rootUIField, &instance);
        return instance;
    }

    inline void _SafeCallOnOrOffMenu(void* rootUI, void* typeObj) {
        __try { oRootUI_OnOrOffMenu(rootUI, typeObj); }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            std::cout << "[BankBot/UI] RootUI.OnOrOffMenu AV\n";
        }
    }
    inline void _SafeCallOnOrOffMenuWhenPossible(void* rootUI, void* typeObj) {
        __try { oRootUI_OnOrOffMenuWhenPossible(rootUI, typeObj); }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            std::cout << "[BankBot/UI] RootUI.OnOrOffMenuWhenPossible AV\n";
        }
    }
    inline void _SafeCallShowMenu(void* self, bool doInstant) {
        __try { oBaseMenuUI_ShowMenu(self, doInstant); }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            std::cout << "[BankBot/UI] BaseMenuUI.ShowMenu AV\n";
        }
    }

    static constexpr int kUnityKeyCodeE = 101;
    static constexpr int kSimulatePressFrames = 3;
    inline std::atomic<int> g_simulateKeyFramesLeft{ 0 };

    typedef bool (__fastcall* tInputGetKey)(int keyCode);
    inline tInputGetKey g_orig_Input_GetKey      = nullptr;
    inline tInputGetKey g_orig_Input_GetKeyDown  = nullptr;
    inline bool g_inputHooksInstalled = false;

    inline bool __fastcall hk_Input_GetKey(int keyCode) {
        if (keyCode == kUnityKeyCodeE && g_simulateKeyFramesLeft.load() > 0) {
            return true;
        }
        return g_orig_Input_GetKey ? g_orig_Input_GetKey(keyCode) : false;
    }
    inline bool __fastcall hk_Input_GetKeyDown(int keyCode) {
        if (keyCode == kUnityKeyCodeE && g_simulateKeyFramesLeft.load() > 0) {

            int prev = g_simulateKeyFramesLeft.fetch_sub(1);
            return prev == kSimulatePressFrames;
        }
        return g_orig_Input_GetKeyDown ? g_orig_Input_GetKeyDown(keyCode) : false;
    }

    inline void InstallInputHooks() {
        if (g_inputHooksInstalled) return;

        void* fnGetKey     = IL2CPP::Class::Utils::GetMethodPointer(
            "UnityEngine.Input", "GetKey", 1);
        void* fnGetKeyDown = IL2CPP::Class::Utils::GetMethodPointer(
            "UnityEngine.Input", "GetKeyDown", 1);
        if (!fnGetKey && !fnGetKeyDown) {
            std::cout << "[BankBot/UI] InstallInputHooks: UnityEngine.Input "
                         "GetKey/GetKeyDown unresolved in manifest. Add "
                         "{ \"name\": \"Input\", \"namespace\": \"UnityEngine\" } "
                         "to dumper allowlist + method_glob \"*\". RVAs: "
                         "GetKey 0x313FEA0, GetKeyDown 0x313FF40 (sanity-check).\n";
            return;
        }
        if (fnGetKey) {
            DoHook(fnGetKey,     &hk_Input_GetKey,
                   (void**)&g_orig_Input_GetKey,
                   "UnityEngine.Input::GetKey");
        }
        if (fnGetKeyDown) {
            DoHook(fnGetKeyDown, &hk_Input_GetKeyDown,
                   (void**)&g_orig_Input_GetKeyDown,
                   "UnityEngine.Input::GetKeyDown");
        }
        g_inputHooksInstalled = (fnGetKey && fnGetKeyDown);
        std::cout << "[BankBot/UI] InstallInputHooks: GetKey="
                  << (fnGetKey ? "HOOKED" : "miss")
                  << " GetKeyDown="
                  << (fnGetKeyDown ? "HOOKED" : "miss") << "\n";
    }

    inline bool OpenSafeBoxUI() {
        InstallInputHooks();
        if (!g_inputHooksInstalled) {
            std::cout << "[BankBot/UI] OpenSafeBoxUI: input hooks not installed\n";
            return false;
        }
        g_simulateKeyFramesLeft.store(kSimulatePressFrames);
        std::cout << "[BankBot/UI] OpenSafeBoxUI -> simulating E keypress "
                     "via Input.GetKey/GetKeyDown hook for "
                  << kSimulatePressFrames << " frames\n";
        return true;
    }

    inline bool CloseSafeBoxUI() {
        ResolveSafeBoxUIControls();
        if (!oItemSafeBoxClosePopup) {
            std::cout << "[BankBot/UI] CloseSafeBoxUI: ClosePopup unresolved "
                         "— add manifest entry "
                         "\"ItemSafeBoxUI$$ClosePopup\" RVA 0xEC2980 argc 0.\n";
            return false;
        }
        if (!g_safeBoxUI_self) {
            std::cout << "[BankBot/UI] CloseSafeBoxUI: no captured self "
                         "(UI never opened this session)\n";
            return false;
        }
        std::cout << "[BankBot/UI] CloseSafeBoxUI -> ClosePopup(self="
                  << g_safeBoxUI_self << ")\n";
        _SafeCallClosePopup(g_safeBoxUI_self);
        return true;
    }

    inline bool g_autoDeposit = true;
    inline int  g_depositCadenceMs = 800;
    inline bool g_bankWalking = false;
    inline std::chrono::steady_clock::time_point g_bankWalkStartedAt;
    inline std::chrono::steady_clock::time_point g_lastDepositAttempt =
        std::chrono::steady_clock::now() - std::chrono::seconds(60);

    inline std::chrono::steady_clock::time_point g_bankWalkAbandonedAt =
        std::chrono::steady_clock::now() - std::chrono::seconds(120);
    static constexpr int BANK_WALK_TIMEOUT_MS = 20000;
    static constexpr int BANK_WALK_ABANDON_COOLDOWN_MS = 30000;
    static constexpr int GBANK_REPLY_TIMEOUT_MS = 3000;

    inline bool g_bankUIOpened = false;
    inline std::chrono::steady_clock::time_point g_bankUIOpenedAt;
    static constexpr int BANK_OPEN_TIMEOUT_MS = 5000;

    static constexpr int BANK_REACH_GRACE_MS  = 200;
    static constexpr int BANK_GBANK_SPAM_MS   = 1000;
    inline std::chrono::steady_clock::time_point g_bankReachStartedAt =
        std::chrono::steady_clock::now() - std::chrono::seconds(60);
    inline bool g_bankInReach = false;

    inline bool HasItemsToDeposit() {
        for (int bt : CollectSelectedBlockTypes()) {
            if (InventoryCountForBlock(bt) > 0) return true;
        }
        return false;
    }

    inline void StopBankWalk() {
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
        g_bankWalking = false;
    }

    inline bool StartBankWalk() {
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
        if (!GetBankTile(goalTile)) return false;

        void* worldObj = g_off_WCWorld
            ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld)
            : nullptr;
        if (!worldObj) return false;

        PathRenderer::PathResult res = PathRenderer::BuildPath(
            g_WorldController, worldObj, startTile, goalTile);
        if (res.status != PathRenderer::PathStatus::Success) {
            std::cout << XS("[BankBot] BuildPath FAIL status=")
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
        g_bankWalking = true;
        g_bankWalkStartedAt = std::chrono::steady_clock::now();
        return true;
    }

    inline bool AutoLoopTick() {
        if (!g_autoDeposit) {
            AutoMine::g_botBusy.store(false, std::memory_order_release);
            return false;
        }
        Resolve();

        if (!HasItemsToDeposit()) {
            if (g_bankWalking) StopBankWalk();
            if (g_bankUIOpened) {
                std::cout << "[BankBot] no more items — CloseSafeBoxUI\n";
                CloseSafeBoxUI();
                g_bankUIOpened   = false;
                g_bankCacheValid = false;
                g_bankSlots.clear();
            }
            AutoMine::g_botBusy.store(false, std::memory_order_release);
            return false;
        }

        auto now = std::chrono::steady_clock::now();

        bool inReach = InBankReach();
        if (inReach && !g_bankInReach) {
            g_bankReachStartedAt = now;
        }
        g_bankInReach = inReach;

        if (!inReach) {

            AutoMine::g_botBusy.store(false, std::memory_order_release);

            if (g_bankUIOpened) {
                std::cout << "[BankBot] left reach — CloseSafeBoxUI\n";
                CloseSafeBoxUI();
                g_bankUIOpened   = false;
                g_bankCacheValid = false;
                g_bankSlots.clear();
            }
            if (!g_bankWalking) {
                auto sinceAbandon = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - g_bankWalkAbandonedAt).count();
                if (sinceAbandon < BANK_WALK_ABANDON_COOLDOWN_MS) return false;
                if (!StartBankWalk()) return false;
                return true;
            }
            auto walkMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_bankWalkStartedAt).count();
            if (walkMs > BANK_WALK_TIMEOUT_MS) {
                std::cout << XS("[BankBot] walk timeout — abandoning (cooldown ")
                          << BANK_WALK_ABANDON_COOLDOWN_MS / 1000 << XS("s)\n");
                StopBankWalk();
                g_bankWalkAbandonedAt = now;
                return false;
            }
            return true;
        }

        AutoMine::g_botBusy.store(true, std::memory_order_release);
        StopBankWalk();

        if (!g_bankUIOpened) {
            auto sinceReach = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_bankReachStartedAt).count();
            if (sinceReach < BANK_REACH_GRACE_MS) return true;
            std::cout << "[BankBot] STAGE-OPEN: firing OpenSafeBoxUI\n";
            if (!OpenSafeBoxUI()) {
                std::cout << "[BankBot] OpenSafeBoxUI failed — retry next tick\n";
                return false;
            }
            g_bankUIOpened   = true;
            g_bankUIOpenedAt = now;
            g_bankCacheValid = false;
            g_bankSlots.clear();
            return true;
        }

        if (!g_bankCacheValid) {
            auto sinceOpen = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_bankUIOpenedAt).count();
            if (sinceOpen > BANK_OPEN_TIMEOUT_MS) {
                std::cout << "[BankBot] open timeout (" << sinceOpen
                          << "ms) — cache never populated. CloseSafeBoxUI + retry.\n";
                CloseSafeBoxUI();
                g_bankUIOpened = false;
                return false;
            }
            return true;
        }

        auto sinceLast = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_lastDepositAttempt).count();
        if (sinceLast < g_depositCadenceMs) return true;

        g_lastDepositAttempt = now;
        for (int bt : CollectSelectedBlockTypes()) {
            int qty = InventoryCountForBlock(bt);
            if (qty <= 0) continue;
            Diagnostics::Ev("bankbot_deposit_fire", {
                {"blockType", bt},
                {"qty",       qty},
                {"slots",     (int)g_bankSlots.size()}
            });
            DepositBlockSplit(bt, qty);
            break;
        }
        return true;
    }

    inline void RenderUI() {
        ImGui::PushID("BankBot");
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.6f, 1.0f, 1.0f, 1.0f), "Auto-Deposit");
        ImGui::Checkbox("Enable Auto-Deposit##bbauto", &g_autoDeposit);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(
                "Runs as part of AM Auto-Loop in PIXELMINES.\n"
                "Pathfinds to banker (8.00, 10.56), deposits selected items,\n"
                "then continues to portal entry.");
        ImGui::SliderInt("Deposit cadence (ms)##bbcad",
                         &g_depositCadenceMs, 200, 3000, "%d ms");

        if (ImGui::TreeNode("Items to deposit##bb")) {

            char buf[64];
            ImGui::TextDisabled("Nuggets:");
            for (int i = 0; i < 5; ++i) {
                if (i) ImGui::SameLine();
                snprintf(buf, sizeof(buf), "%s##nug%d", kNuggetNames[i], i);
                ImGui::Checkbox(buf, &g_selNuggets[i]);
            }
            ImGui::TextDisabled("Mine Keys:");

            for (int i = 0; i < 5; ++i) {
                if (i != 0 && i != 4) ImGui::SameLine();
                snprintf(buf, sizeof(buf), "%s##key%d", kKeyNames[i], i);
                ImGui::Checkbox(buf, &g_selKeys[i]);
            }
            ImGui::TextDisabled("Mining Ingredients:");
            for (int i = 0; i < 9; ++i) {
                if (i % 4 != 0) ImGui::SameLine();
                snprintf(buf, sizeof(buf), "%s##mat%d", kMaterialNames[i], i);
                ImGui::Checkbox(buf, &g_selMaterials[i]);
            }
            ImGui::TextDisabled("Gemstones (per-cell type+size):");
            for (int t = 0; t < 5; ++t) {
                ImGui::Text("%-9s", kGemTypeNames[t]);
                for (int s = 0; s < 5; ++s) {
                    ImGui::SameLine();
                    snprintf(buf, sizeof(buf), "%s##gemm%d_%d", kGemSizeNames[s], t, s);
                    ImGui::Checkbox(buf, &g_depositMatrix[t][s]);
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

}

inline void BankBot_ProcessIncomingPacket(const nlohmann::json& j) {
    BankBot::OnIncomingPacket(j);
}

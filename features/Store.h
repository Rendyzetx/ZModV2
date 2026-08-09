

#pragma once

#include "main.h"
#include "ui/ZmodUI.h"
#include "ui_bindings.h"

#include "StoreCatalog.h"

#include <chrono>
#include <cstring>
#include <string>
#include <iostream>

namespace Store {

typedef void(__fastcall* tBuyItemPack)(Il2CppString* itemPackId, void* methodInfo);
inline tBuyItemPack oBuyItemPack = nullptr;

inline void Resolve() {
    if (oBuyItemPack) return;
    void* p = GetMethodPtrFromCandidates({
        {"OutgoingMessages", "BuyItemPack"},
        {"NetworkClient",    "BuyItemPack"}
    });
    if (p) {
        oBuyItemPack = (tBuyItemPack)p;
        std::cout << "[Store] OutgoingMessages::BuyItemPack @ " << p << "\n";
    } else {
        std::cout << "[Store] failed to resolve OutgoingMessages::BuyItemPack\n";
    }
}

struct InvStats {
    int used  = 0;
    int max   = 0;
    bool ok   = false;
};

static inline InvStats Seh_ReadInventory() {
    InvStats out;
    void* invCtl = g_InventoryControl;
    void* localP = g_LocalPlayer;
    if (!invCtl || !localP || !oGetPlayerData) return out;

    void* pd = nullptr;
    __try { pd = oGetPlayerData(localP); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return out; }
    if (!pd) return out;

    short maxSlots = 0;
    Il2CppArray* items = nullptr;
    __try {
        maxSlots = *(short*)((uintptr_t)pd + 0x37C);
        items    = *(Il2CppArray**)((uintptr_t)invCtl + 0x188);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return out; }

    out.max = (int)maxSlots;
    if (items) out.used = (int)items->max_length;
    out.ok = true;
    return out;
}

static inline void Seh_FireBuy(const char* packId) {
    if (!oBuyItemPack || !oIl2CppStringNew || !packId || !*packId) return;
    Il2CppString* s = nullptr;
    __try { s = oIl2CppStringNew(packId); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return; }
    if (!s) return;
    __try { oBuyItemPack(s, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

static inline const char* CurrentPackId(const ZmodUI::State& st) {
    int idx = st.store_selected_pack;
    if (idx < 0) {

        return (st.store_custom_pack_id[0] != '\0') ? st.store_custom_pack_id : nullptr;
    }
    if (idx >= 0 && idx < kPackCount) return kPackCatalog[idx].id;
    return nullptr;
}

inline void Tick(ZmodUI::State& st) {

    InvStats inv = Seh_ReadInventory();
    if (inv.ok) {
        st.store_inventory_used = inv.used;
        st.store_inventory_max  = inv.max;
    }

    if (!st.store_auto_buy) return;
    if (!oBuyItemPack)      return;

    if (st.store_bought_count >= st.store_target_count) {
        st.store_auto_buy = false;
        std::cout << "[Store] target reached: " << st.store_bought_count
                  << " packs bought.\n";
        return;
    }

    if (st.store_stop_when_full && inv.ok && inv.max > 0 && inv.used >= inv.max) {
        st.store_auto_buy = false;
        std::cout << "[Store] backpack full (" << inv.used << "/" << inv.max
                  << "). Bought " << st.store_bought_count << " this run.\n";
        return;
    }

    const char* packId = CurrentPackId(st);
    if (!packId) {

        st.store_auto_buy = false;
        return;
    }

    static auto last = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
    float delay = st.store_buy_delay_ms;
    if (delay < 100.f)  delay = 100.f;
    if (delay > 2000.f) delay = 2000.f;
    if (elapsedMs < (long long)delay) return;
    last = now;

    Seh_FireBuy(packId);
    st.store_bought_count++;
}

inline void TickFromHook() { Tick(ZmodBindings::g_uiState); }

}

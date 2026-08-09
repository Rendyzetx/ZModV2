

#include "../Internal.h"
#include "../widgets/Primitives.h"
#include "../Tokens.h"
#include "../Fonts.h"
#include "StoreCatalog.h"

#include <cstring>
#include <cstdio>
#include <cctype>

namespace ZmodUI {

static bool MatchFilter(const char* hay, const char* needle) {
    if (!needle || !*needle) return true;
    if (!hay) return false;
    int hlen = (int)std::strlen(hay);
    int nlen = (int)std::strlen(needle);
    if (nlen > hlen) return false;
    for (int i = 0; i <= hlen - nlen; ++i) {
        int j = 0;
        for (; j < nlen; ++j) {
            char a = (char)std::tolower((unsigned char)hay[i + j]);
            char b = (char)std::tolower((unsigned char)needle[j]);
            if (a != b) break;
        }
        if (j == nlen) return true;
    }
    return false;
}

static const char* FormatGems(int gems) {
    static thread_local char buf[24];
    if (gems >= 9999999) {
        std::strncpy(buf, "Cosmetic-only", sizeof(buf));
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    }
    char raw[16];
    ImFormatString(raw, sizeof(raw), "%d", gems);
    int len = (int)std::strlen(raw);
    int outIdx = 0;
    for (int i = 0; i < len && outIdx < (int)sizeof(buf) - 2; ++i) {
        if (i > 0 && (len - i) % 3 == 0) buf[outIdx++] = ',';
        buf[outIdx++] = raw[i];
    }
    buf[outIdx] = '\0';
    return buf;
}

void RailStore(State& s) {
    PageHead("Store", "Pack auto-buyer");

    Ctx& c = GetCtx();
    {
        ImFont* fe = Tokens::Type::Eyebrow();
        const char* lab = s.store_auto_buy ? "Buying" : "Idle";
        int tone = s.store_auto_buy ? 0 : 2;
        ImVec2 ts = Draw::TextSize(fe, lab);
        float w = ts.x + 20.f;
        float h = ts.y + 8.f;
        float y = Layout::ContentToScreenY(c.cursor.y) - 18.f - h;
        ImVec2 a = ImVec2(c.content_x0 + 70.f, y);
        ImVec2 b = ImVec2(a.x + w, a.y + h);
        ImU32 bg, fg;
        if (tone == 0) { bg = IM_COL32(52,199,89,38); fg = IM_COL32(31,138,58,255); }
        else           { bg = IM_COL32(142,142,147,38); fg = IM_COL32(99,99,102,255); }
        Draw::RectFilledRounded(c.dl, a, b, bg, 999.f);
        Draw::TextL(c.dl, fe, ImVec2(a.x + 10, a.y + 4), fg, lab);
    }

    GroupTitle("Selected pack");
    BeginGroup();

        if (s.store_category_idx < 0 || s.store_category_idx >= Store::kCategoryCount)
            s.store_category_idx = 0;
        ComboRow("Category", Store::kCategories, Store::kCategoryCount,
                 &s.store_category_idx);

        InputRow("Filter", s.store_pack_filter, sizeof(s.store_pack_filter),
                 "Type to narrow the catalogue...");

        const int kMaxVisible = 400;
        static const char* names[kMaxVisible + 1];
        static int   visIdxToCatalog[kMaxVisible + 1];
        int n = 0;
        names[n] = "Custom ID...";
        visIdxToCatalog[n] = -1;
        n++;
        const char* activeCat = Store::kCategories[s.store_category_idx];
        bool allCats = (std::strcmp(activeCat, "All") == 0);
        for (int i = 0; i < Store::kPackCount && n < kMaxVisible + 1; ++i) {
            const Store::PackEntry& p = Store::kPackCatalog[i];
            if (!allCats && std::strcmp(p.category, activeCat) != 0) continue;
            if (!MatchFilter(p.label, s.store_pack_filter)) continue;
            names[n] = p.label;
            visIdxToCatalog[n] = i;
            n++;
        }

        int comboIdx = 0;
        for (int i = 0; i < n; ++i) {
            if (visIdxToCatalog[i] == s.store_selected_pack) { comboIdx = i; break; }
        }

        if (ComboRow("Pack", names, n, &comboIdx)) {
            s.store_selected_pack = visIdxToCatalog[comboIdx];
        }

        if (s.store_selected_pack < 0) {
            InputRow("Custom ID", s.store_custom_pack_id, sizeof(s.store_custom_pack_id),
                     "exact pack id (e.g. StarterPack)");
        } else if (s.store_selected_pack >= 0 && s.store_selected_pack < Store::kPackCount) {
            const Store::PackEntry& p = Store::kPackCatalog[s.store_selected_pack];
            StatLabel("ID",       p.id);
            StatLabel("Category", p.category);
            if (p.priceInGems >= 9999999) {
                StatLabel("Price", "Cosmetic-only");
            } else {
                char priceBuf[32];
                ImFormatString(priceBuf, sizeof(priceBuf), "%s gems",
                               FormatGems(p.priceInGems));
                StatLabel("Price", priceBuf);
            }
        }
    EndGroup();

    if (s.store_selected_pack < 0) {
        HelpText("Type any pack id the server recognises. "
                 "Unknown ids are silently rejected by the server.");
    }

    GroupTitle("Auto-Buy");
    BeginGroup();
        SliderRow("Target count",  "packs", &s.store_target_count, 1, 9999);
        SliderRow("Delay between buys", "ms", &s.store_buy_delay_ms, 100.f, 2000.f);
        ToggleRow("magnet-green",  "Stop when backpack is full",
                  "Halt the loop the moment used slots hits the cap.",
                  &s.store_stop_when_full);
        ToggleRow("treasure-orange", "Auto-Buy",
                  "Start the loop. Fires one pack purchase per delay tick.",
                  &s.store_auto_buy);
    EndGroup();

    GroupTitle("Status");
    BeginGroup();
        char usedMaxBuf[48];
        if (s.store_inventory_max > 0) {
            ImFormatString(usedMaxBuf, sizeof(usedMaxBuf), "%d / %d slots",
                           s.store_inventory_used, s.store_inventory_max);
        } else {
            std::strncpy(usedMaxBuf, "(not in-world)", sizeof(usedMaxBuf));
            usedMaxBuf[sizeof(usedMaxBuf) - 1] = '\0';
        }
        StatLabel("Backpack",   usedMaxBuf);

        char boughtBuf[48];
        ImFormatString(boughtBuf, sizeof(boughtBuf), "%d / %d",
                       s.store_bought_count, s.store_target_count);
        StatLabel("Bought this run", boughtBuf);

        if (s.store_selected_pack >= 0 && s.store_selected_pack < Store::kPackCount) {
            const Store::PackEntry& p = Store::kPackCatalog[s.store_selected_pack];
            int remaining = s.store_target_count - s.store_bought_count;
            if (remaining < 0) remaining = 0;
            long long projected = (long long)remaining * (long long)p.priceInGems;
            char costBuf[64];
            if (p.priceInGems >= 9999999) {
                std::strncpy(costBuf, "Cosmetic-only (no gem cost)", sizeof(costBuf));
                costBuf[sizeof(costBuf) - 1] = '\0';
            } else {

                char gemsBuf[32];
                std::strncpy(gemsBuf, FormatGems((int)projected), sizeof(gemsBuf));
                gemsBuf[sizeof(gemsBuf) - 1] = '\0';
                ImFormatString(costBuf, sizeof(costBuf), "~%s gems", gemsBuf);
            }
            StatLabel("Projected cost", costBuf);
        }

        if (Button("Reset counter")) {
            Store::ResetCounter(s);
        }
    EndGroup();

    HelpText("Auto-Buy fires OutgoingMessages.BuyItemPack on the chosen "
             "pack id at the cadence you set. Server-side wallet, "
             "stock, and cooldown rules still apply \xE2\x80\x94 if you "
             "can't afford it or the pack is on cooldown the server just "
             "drops the request and the loop continues anyway. Loop stops "
             "automatically at target count or when the backpack hits its "
             "slot cap. \"Cosmetic-only\" packs (price 9,999,999) are "
             "intentionally unbuyable through normal channels but the id "
             "is listed here for completeness.");
}

}

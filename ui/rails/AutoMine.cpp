#include "../Internal.h"
#include "../widgets/Primitives.h"
#include "../Tokens.h"
#include "../Fonts.h"
#include <cstring>

namespace ZmodUI {

static void HeadWithPill(const char* title, bool on) {
    Ctx& c = GetCtx();
    PageHead(title, "Mining bot");

}

void RailAutoMine(State& s) {
    PageHead("Auto Mine", "Mining bot");

    Ctx& c = GetCtx();
    {
        float pad = 6.f;
        ImFont* fe = Tokens::Type::Eyebrow();
        const char* lab = s.am_enabled ? "On" : "Off";
        int tone = s.am_enabled ? 0 : 2;
        ImVec2 ts = Draw::TextSize(fe, lab);
        float w = ts.x + 20.f;
        float h = ts.y + 8.f;
        float y = Layout::ContentToScreenY(c.cursor.y) - 18.f - h;
        ImVec2 a = ImVec2(c.content_x0 + 92.f, y);
        ImVec2 b = ImVec2(a.x + w, a.y + h);
        ImU32 bg, fg;
        if (tone == 0) { bg = IM_COL32(52,199,89,38); fg = IM_COL32(31,138,58,255); }
        else           { bg = IM_COL32(255,59,48,38); fg = IM_COL32(196,39,30,255); }
        Draw::RectFilledRounded(c.dl, a, b, bg, 999.f);
        Draw::TextL(c.dl, fe, ImVec2(a.x + 10, a.y + 4), fg, lab);
    }

    GroupTitle("General");
    BeginGroup();
        ToggleRow("pickaxe-indigo", "Enable", nullptr, &s.am_enabled);
        ToggleRow("recipe", "Auto-repair pickaxe", nullptr, &s.am_auto_repair);
        SliderRow("Durability threshold", "dura",
                  &s.am_repair_threshold, 0, 200);

    EndGroup();

    GroupTitle("Pre-Mine HB");
    BeginGroup();
        ToggleRow("hammer-orange", "Enable pre-mine",
                  "Each HB tick targets the closest in-path breakable tile in the NxN window.",
                  &s.am_premine_hb);

        ToggleRow("rock-gray", "Include hard solid",
                  "Also pre-mine bedrock-tier blocks. Usually wasted hits.",
                  &s.am_premine_include_solid);
    EndGroup();

    GroupTitle("Levels");
    BeginGroup();
        Ctx& cc = GetCtx();

        float save_y = cc.cursor.y;
        float y0 = Layout::ContentToScreenY(cc.cursor.y) + 12.f;
        float x = cc.content_x0 + 14.f;
        ImFont* fb = Tokens::Type::Body();
        for (int i = 0; i < 5; ++i) {
            char L[4]; ImFormatString(L, sizeof(L), "L%d", i+1);
            float w = 38.f, h = (fb ? fb->FontSize : 13) + 12.f;
            ImVec2 a = ImVec2(x, y0);
            ImVec2 b = ImVec2(x + w, y0 + h);
            char idb[16]; ImFormatString(idb, sizeof(idb), "lvl_%d", i);
            Layout::Interact it = Layout::RegisterHit(idb, a, b);
            if (it.clicked) s.am_levels[i] = !s.am_levels[i];
            ImU32 bg = s.am_levels[i] ? Tokens::Color::Green() : Tokens::Color::Bg();
            ImU32 fg = s.am_levels[i] ? IM_COL32_WHITE : Tokens::Color::Text();
            ImU32 border = s.am_levels[i] ? 0 : Tokens::Color::Line();
            if (it.hovered && !s.am_levels[i]) bg = Tokens::Color::BgHover();
            Draw::RectFilledRounded(cc.dl, a, b, bg, 999.f);
            if (border) Draw::RectStrokeRounded(cc.dl, a, b, border, 999.f, 1.f);
            ImVec2 ts = Draw::TextSize(fb, L);
            Draw::TextL(cc.dl, fb, ImVec2(a.x + (w - ts.x)*0.5f, a.y + 6.f), fg, L);
            x += w + 6.f;
        }
        cc.cursor.y += 12.f + (fb ? fb->FontSize : 13) + 12.f + 12.f;
    EndGroup();

    GroupTitle("Idle protection");
    BeginGroup();
        ToggleRow("pulse",     "Anti-Sleep", nullptr, &s.am_anti_sleep);
        ToggleRow("sleep-pink","Anti-AFK",   nullptr, &s.am_anti_afk);
    EndGroup();

    GroupTitle("Modules");
    BeginGroup();
        ToggleRow("magnet-green", "Auto Combine Nuggets", "Stack low-tier nuggets automatically.", &s.am_auto_combine);
        DrawerRow("arrow",      "Pathfinder", "Auto move, draw route, tuning, weights.", "mine-pathfinder");
        DrawerRow("portal",     "Targets",    "What to break and collect.",              "mine-targets");
        DrawerRow("combat-red", "Combat",     "HitAI, range, cadence.",                  "mine-combat");
        DrawerRow("database-indigo", "BankBot", "Auto-deposit hauls to bank world.",     "mine-bankbot");
        DrawerRow("treasure-orange", "AutoSell", "Sell hauls at vendor on cycle.",       "mine-autosell");
        DrawerRow("reload-teal", "Rejoin",     "Auto-rejoin world on disconnect.",       "mine-rejoin");
    EndGroup();
}

}

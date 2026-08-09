#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Icons.h"
#include "../Fonts.h"
#include "../Tween.h"
#include "../widgets/Primitives.h"
#include "../widgets/Row.h"
#include <cstring>
#include <cstdio>

namespace ZmodUI {

struct NavItem { const char* key; const char* label; const char* icon_tone; };

static const struct NavGroup {
    const char* title;
    NavItem items[6];
    int count;
} kGroups[] = {
    { "ZMod", {
        { "dashboard", "Dashboard",         "dashboard" },
        { "combat",    "Combat & Survival", "shield-red" },
        { "movement",  "Movement",          "arrow-green" },
        { "visuals",   "Visuals",           "eye-orange" },
        { "network",   "Network",           "wifi-teal" },
        { "misc",      "Misc",              "dots-brown" },
    }, 6 },
    { "Automation", {
        { "auto-mine",   "Auto Mine",   "pickaxe-indigo" },
        { "auto-fish",   "Auto Fish",   "fish-teal" },
        { "auto-fossil", "Auto Fossil", "spark" },
        { "auto-nether", "Auto Nether", "flame-red" },
        { "auto-pnb",    "Place & Break","arrow-green" },
        { "store",       "Store",       "treasure-orange" },
    }, 6 },
    { "App", {
        { "proxies",  "Proxies",  "wifi-teal" },
        { "settings", "Settings", "settings-gray" },
#ifndef ZMOD_V3
        { "debug",    "Debug",    "spark" },
#endif
        {}, {}, {},
#ifdef ZMOD_V3
    }, 2 },
#else
    }, 3 },
#endif
};

void SidebarRender(State& s) {
    Ctx& c = GetCtx();
    float W = Tokens::Size::SidebarW;
    float TBH = Tokens::Size::TitleBarH;
    ImVec2 a = ImVec2(c.win_pos.x, c.win_pos.y + TBH);
    ImVec2 b = ImVec2(a.x + W, c.win_pos.y + c.win_size.y);

    if (Theme::ShouldUseBackdropBlur()) {

        ImU32 colTint = IM_COL32(0, 0, 0, 28);
        c.dl->AddRectFilled(a, b, colTint,
                            Tokens::Size::WindowRadius, ImDrawFlags_RoundCornersBottomLeft);
    } else {
        c.dl->AddRectFilled(a, b, Tokens::Color::BgSidebar(),
                            Tokens::Size::WindowRadius, ImDrawFlags_RoundCornersBottomLeft);
    }
    c.dl->AddLine(ImVec2(b.x, a.y), ImVec2(b.x, b.y), Tokens::Color::Line(), 1.f);

    c.dl->PushClipRect(a, b, true);
    float y = a.y + 16.f;
    ImFont* ft = Tokens::Type::Eyebrow();
    ImFont* fl = Tokens::Type::Body();

    for (const NavGroup& g : kGroups) {
        Draw::TextL(c.dl, ft, ImVec2(a.x + 20.f, y), Tokens::Color::TextTert(), g.title);
        y += (ft ? ft->FontSize : 11) + 8.f;
        for (int i = 0; i < g.count; ++i) {
            const NavItem& it = g.items[i];
            ImVec2 ra = ImVec2(a.x + 12.f, y);
            ImVec2 rb = ImVec2(b.x - 12.f, y + 28.f);
            char idb[40]; ImFormatString(idb, sizeof(idb), "sb_%s", it.key);
            Layout::Interact h = Layout::RegisterHit(idb, ra, rb);
            bool active = (std::strcmp(c.active_rail, it.key) == 0);
            if (h.clicked) { SetActiveRail(it.key); CloseDrawer(); }

            ImGuiID aid = ImGui::GetID(idb);
            float at = Tween::ToFloat(aid, active ? 1.f : 0.f, 10.f);
            if (h.hovered && !active) Draw::RectFilledRounded(c.dl, ra, rb, Tokens::Color::BgHover(), 7.f);
            if (at > 0.01f) {
                ImU32 col = Tokens::Color::Accent();
                col = (col & 0x00FFFFFF) | (((ImU32)(at * 255)) << 24);
                Draw::RectFilledRounded(c.dl, ra, rb, col, 7.f);
            }

            WRow::IconTone tone = WRow::ResolveIcon(it.icon_tone);
            float sz = Tokens::Size::SidebarIconTile;
            ImVec2 tp = ImVec2(ra.x + 8.f, ra.y + (28.f - sz)*0.5f);
            ImU32 bg = tone.bg, fg = tone.fg;
            if (active) {
                bg = IM_COL32(255,255,255,56);
                fg = IM_COL32_WHITE;
            }
            Draw::IconTile(c.dl, tp, sz, bg, fg, tone.icon_id, 6.f, tone.atlas_block_id);

            ImU32 text_col = active ? IM_COL32_WHITE : Tokens::Color::Text();
            Draw::TextL(c.dl, fl, ImVec2(tp.x + sz + 10.f, ra.y + (28.f - (fl?fl->FontSize:13))*0.5f), text_col, it.label);
            y += 28.f + 2.f;
        }
        y += 18.f;
    }
    c.dl->PopClipRect();
    (void)s;
}

}

#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include "../Tween.h"
#include <cstring>
#include <cstdio>
#include <cmath>

namespace ZmodUI {

namespace {
    struct Entry { const char* label; const char* rail; const char* drawer_key; };
    static const Entry kRegistry[] = {

        {"Dashboard",            "dashboard",  nullptr},
        {"Combat & Survival",    "combat",     nullptr},
        {"Movement",             "movement",   nullptr},
        {"Visuals",              "visuals",    nullptr},
        {"Network",              "network",    nullptr},
        {"Misc",                 "misc",       nullptr},
        {"Auto Mine",            "auto-mine",  nullptr},
        {"Auto Fish",            "auto-fish",  nullptr},
        {"Auto Nether",          "auto-nether",nullptr},
        {"Store",                "store",      nullptr},
        {"Auto-Buy Packs",       "store",      nullptr},
        {"Settings",             "settings",   nullptr},

        {"Streamer Mode",        "dashboard",  nullptr},

        {"God Mode",             "combat",     nullptr},
        {"Anti-Trap",            "combat",     nullptr},
        {"Anti-Bounce",          "combat",     nullptr},
        {"Anti-Fan",             "combat",     nullptr},
        {"Anti-Deflector",       "combat",     nullptr},
        {"Anti Block-Death",     "combat",     nullptr},
        {"Unlimited Oxygen",     "combat",     nullptr},
        {"Anti-AFK",             "combat",     nullptr},
        {"AI Aimbot",            "combat",     nullptr},
        {"Place Block on Player","combat",     nullptr},

        {"Player Speed",         "movement",   nullptr},
        {"Jump Override",        "movement",   nullptr},
        {"Gravity Override",     "movement",   nullptr},
        {"Infinite Jetpack",     "movement",   nullptr},
        {"Keys to Fly",          "movement",   "keys-to-fly"},
        {"Auto Navigate",        "movement",   nullptr},
        {"Path Settings",        "movement",   "pathfinder"},

        {"Show Mini-Map",        "visuals",    nullptr},
        {"Free Cam",             "visuals",    nullptr},
        {"Enable ESP",           "visuals",    nullptr},
        {"Show ESP Info",        "visuals",    nullptr},
        {"Anti-Darkness",        "visuals",    nullptr},
        {"Force Weather",        "visuals",    nullptr},
        {"Force Lighting",       "visuals",    nullptr},
        {"Background",           "visuals",    nullptr},

        {"Capture Incoming",     "network",    nullptr},
        {"Capture Outgoing",     "network",    nullptr},
        {"Auto-scroll",          "network",    nullptr},
        {"Filter direction",     "network",    nullptr},
        {"Filter by packet ID",  "network",    nullptr},
        {"Open logger",          "network",    "logger"},
        {"Packet Repeater",      "network",    "repeater"},
        {"Ignore List",          "network",    "ignore"},

        {"Unlock All Recipes",   "misc",       nullptr},
        {"Anti-Collect",         "misc",       nullptr},
        {"Auto-Collect",         "misc",       nullptr},
        {"Uncap FPS",            "misc",       "uncap-fps"},

        {"Auto Mine Enable",     "auto-mine",  nullptr},
        {"Auto-repair pickaxe",  "auto-mine",  nullptr},
        {"Anti-Sleep",           "auto-mine",  nullptr},
        {"Auto Combine Nuggets", "auto-mine",  nullptr},
        {"Pathfinder",           "auto-mine",  "mine-pathfinder"},
        {"Targets",              "auto-mine",  "mine-targets"},
        {"Combat",               "auto-mine",  "mine-combat"},
        {"BankBot",              "auto-mine",  "mine-bankbot"},
        {"AutoSell",             "auto-mine",  "mine-autosell"},
        {"Rejoin",               "auto-mine",  "mine-rejoin"},

        {"Enable Fish Bot",      "auto-fish",  nullptr},
        {"Auto Land",            "auto-fish",  nullptr},
        {"Auto Recast",          "auto-fish",  nullptr},

        {"Auto Nether Enable",   "auto-nether",nullptr},
        {"Nether Pathfinder",    "auto-nether","nether-pathfinder"},

        {"Default",              "settings",   nullptr},
        {"Oceanic",              "settings",   nullptr},
        {"Crimson",              "settings",   nullptr},
        {"Emerald Power",        "settings",   nullptr},
        {"Cyber Sunset",         "settings",   nullptr},
        {"Daylight",             "settings",   nullptr},
        {"Grayscale",            "settings",   nullptr},
        {"Pastel Dream",         "settings",   nullptr},
        {"Custom Colors",        "settings",   "custom-colors"},
        {"Randomize Device ID",  "settings",   nullptr},
        {"Reload Game",          "settings",   nullptr},
        {"Force Logout",         "settings",   nullptr},
    };
    static const int kCount = (int)(sizeof(kRegistry) / sizeof(kRegistry[0]));

    static bool Contains(const char* hay, const char* needle) {
        if (!needle || !*needle) return true;
        for (const char* h = hay; *h; ++h) {
            const char* a = h;
            const char* b = needle;
            while (*a && *b && ((*a | 32) == (*b | 32))) { ++a; ++b; }
            if (!*b) return true;
        }
        return false;
    }
}

void PaletteInit() {
    Ctx& c = GetCtx();
    c.palette_open = false;
    c.palette_query[0] = 0;
    c.palette_sel = 0;
}

const PaletteEntry* PaletteRegistry(int* count) {
    *count = kCount;
    return reinterpret_cast<const PaletteEntry*>(&kRegistry[0]);
}

void PaletteRender(State& s) {
    Ctx& c = GetCtx();
    float target = c.palette_open ? 1.f : 0.f;
    c.palette_t = Tween::SmoothStep(c.palette_t, target, 14.f);
    if (c.palette_t < 0.005f && !c.palette_open) return;

    float t = Tokens::Motion::MacEase(c.palette_t);

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImVec2 wa = c.win_pos;
    ImVec2 wb = ImVec2(c.win_pos.x + c.win_size.x, c.win_pos.y + c.win_size.y);
    ImU32 scrim = IM_COL32(0,0,0, (int)(80 * t));
    dl->AddRectFilled(wa, wb, scrim);

    float W = 540.f;
    float H = 410.f;
    float topMargin = c.win_size.y * 0.14f;
    ImVec2 a = ImVec2(c.win_pos.x + (c.win_size.x - W)*0.5f, c.win_pos.y + topMargin);
    a.y -= 12.f * (1.f - t);
    ImVec2 b = ImVec2(a.x + W, a.y + H);

    Draw::DropShadow(dl, a, b, 12.f, IM_COL32(0,0,0,140), 3);
    Draw::RectFilledRounded(dl, a, b, Tokens::Color::BgCard(), 12.f);
    Draw::RectStrokeRounded(dl, a, b, Tokens::Color::Line(), 12.f, 1.f);

    float input_h = 48.f;
    ImVec2 ia = a;
    ImVec2 ib = ImVec2(b.x, a.y + input_h);
    dl->AddLine(ImVec2(ia.x + 4, ib.y), ImVec2(ib.x - 4, ib.y), Tokens::Color::Line(), 1.f);

    ImFont* fb = Tokens::Type::H3();

    if (c.palette_open) {
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < io.InputQueueCharacters.Size; ++i) {
            ImWchar w = io.InputQueueCharacters[i];
            if (w >= 0x20 && w < 0x7F) {
                int len = (int)std::strlen(c.palette_query);
                if (len + 1 < (int)sizeof(c.palette_query)) {
                    c.palette_query[len] = (char)w;
                    c.palette_query[len+1] = 0;
                }
            }
        }
        io.InputQueueCharacters.resize(0);
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
            int len = (int)std::strlen(c.palette_query);
            if (len > 0) c.palette_query[len-1] = 0;
        }
    }
    const char* query = c.palette_query;
    bool empty = (query[0] == 0);
    Draw::TextL(dl, fb, ImVec2(ia.x + 18, ia.y + 14),
                empty ? Tokens::Color::TextTert() : Tokens::Color::Text(),
                empty ? "Search settings..." : query);

    if (!empty) {
        float ct = (float)ImGui::GetTime();
        float blink = 0.5f + 0.5f * std::sin(ct * 6.f);
        ImVec2 ts = Draw::TextSize(fb, query);
        float cx = ia.x + 18 + ts.x + 2.f;
        ImU32 cc = (Tokens::Color::Accent() & 0x00FFFFFF) | (((ImU32)(blink * 255)) << 24);
        dl->AddLine(ImVec2(cx, ia.y + 14), ImVec2(cx, ia.y + 14 + 18), cc, 1.5f);
    }

    ImVec2 ra = ImVec2(a.x, ib.y);
    ImVec2 rb = ImVec2(b.x, b.y);
    dl->PushClipRect(ra, rb, true);
    ImFont* fl = Tokens::Type::Body();
    ImFont* fs = Tokens::Type::BodySec();
    float row_h = 32.f;
    float y = ra.y;
    int shown = 0;
    int hits[64]; int hit_count = 0;
    for (int i = 0; i < kCount && hit_count < 64; ++i) {
        if (Contains(kRegistry[i].label, query)) hits[hit_count++] = i;
    }
    if (c.palette_sel >= hit_count) c.palette_sel = 0;

    if (c.palette_open) {
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))    c.palette_sel = (c.palette_sel + 1) % (hit_count > 0 ? hit_count : 1);
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))      c.palette_sel = (c.palette_sel - 1 + (hit_count > 0 ? hit_count : 1)) % (hit_count > 0 ? hit_count : 1);
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))       ClosePalette();
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) && hit_count > 0) {
            const Entry& e = kRegistry[hits[c.palette_sel]];
            SetActiveRail(e.rail);
            if (e.drawer_key) OpenDrawer(e.drawer_key);
            ClosePalette();
        }
    }

    for (int k = 0; k < hit_count && y < rb.y - row_h; ++k, ++shown) {
        const Entry& e = kRegistry[hits[k]];
        ImVec2 la = ImVec2(ra.x, y);
        ImVec2 lb = ImVec2(rb.x, y + row_h);
        char idb[40]; ImFormatString(idb, sizeof(idb), "plt_%d", hits[k]);
        Layout::Interact it = Layout::RegisterHit(idb, la, lb);
        if (it.hovered || k == c.palette_sel)
            Draw::RectFilledRounded(dl, la, lb, Tokens::Color::BgHover(), 0.f);
        if (it.clicked) {
            SetActiveRail(e.rail);
            if (e.drawer_key) OpenDrawer(e.drawer_key);
            ClosePalette();
        }
        Draw::TextL(dl, fl, ImVec2(la.x + 18, la.y + 8), Tokens::Color::Text(), e.label);
        ImVec2 crumb_ts = Draw::TextSize(fs, e.rail);
        Draw::TextL(dl, fs, ImVec2(lb.x - 18 - crumb_ts.x, la.y + 10), Tokens::Color::TextSec(), e.rail);
        dl->AddLine(ImVec2(la.x + 4, lb.y), ImVec2(lb.x - 4, lb.y), Tokens::Color::LineSoft(), 1.f);
        y += row_h;
    }
    dl->PopClipRect();

    if (c.palette_open && ImGui::GetFrameCount() > c.palette_open_frame
        && ImGui::IsMouseClicked(0)) {
        if (!ImGui::IsMouseHoveringRect(a, b)) ClosePalette();
    }
    (void)s;
}

}

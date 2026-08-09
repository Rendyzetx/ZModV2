#include "Drawer.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include "../widgets/Row.h"
#include "../Tween.h"
#include <cstring>
#include <cstdio>

namespace ZmodUI {

void DrawerInit() {
    Ctx& c = GetCtx();
    c.drawer_open = false;
    c.drawer_t = 0.f;
    c.drawer_key[0] = 0;
}

void OpenDrawerImpl(const char* key) {
    Ctx& c = GetCtx();
    std::strncpy(c.drawer_key, key ? key : "", sizeof(c.drawer_key)-1);
    c.drawer_open = true;
    c.drawer_scroll_y = 0.f;
}

void CloseDrawerImpl() {
    Ctx& c = GetCtx();
    c.drawer_open = false;
}

void DrawerRenderAll(State& s) {
    Ctx& c = GetCtx();
    float target = c.drawer_open ? 1.f : 0.f;
    float speed = 1000.f / Tokens::Motion::DrawerMs * 4.f;
    c.drawer_t = Tween::SmoothStep(c.drawer_t, target, speed);
    if (c.drawer_t < 0.001f && !c.drawer_open) return;

    float W = Tokens::Size::DrawerW;
    float TBH = Tokens::Size::TitleBarH;
    float SSH = Tokens::Size::StatusStripH;

    float t = c.drawer_t;

    ImVec2 a = ImVec2(c.win_pos.x + c.win_size.x - W * t,
                      c.win_pos.y + TBH);
    ImVec2 b = ImVec2(a.x + W, c.win_pos.y + c.win_size.y - SSH);

    for (int i = 0; i < 3; ++i) {
        ImU32 col = IM_COL32(0,0,0, (int)(40 * t / (1 + i*1.5f)));
        c.dl->AddRectFilled(ImVec2(a.x - (i+1)*6.f, a.y), ImVec2(a.x, b.y), col, 0.f);
    }

    c.dl->AddRectFilled(a, b, Tokens::Color::BgPanelInner(), 0.f);
    c.dl->AddLine(a, ImVec2(a.x, b.y), Tokens::Color::Line(), 1.f);

    float HH = 50.f;
    ImVec2 ha = a;
    ImVec2 hb = ImVec2(b.x, a.y + HH);
    c.dl->AddRectFilled(ha, hb, Tokens::Color::BgPanelInner(), 0.f);
    c.dl->AddLine(ImVec2(ha.x, hb.y), ImVec2(hb.x, hb.y), Tokens::Color::Line(), 1.f);

    ImVec2 ba_ = ImVec2(ha.x + 14, ha.y + (HH - 26)*0.5f);
    ImVec2 bb_ = ImVec2(ba_.x + 26, ba_.y + 26);
    Layout::Interact it = Layout::RegisterHit("drawer_back", ba_, bb_);
    if (it.hovered) Draw::RectFilledRounded(c.dl, ba_, bb_, Tokens::Color::BgHover(), 6.f);

    ImVec2 chc = ImVec2((ba_.x+bb_.x)*0.5f, (ba_.y+bb_.y)*0.5f);
    c.dl->AddLine(ImVec2(chc.x + 4, chc.y - 6), ImVec2(chc.x - 4, chc.y), Tokens::Color::Accent(), 1.8f);
    c.dl->AddLine(ImVec2(chc.x - 4, chc.y),     ImVec2(chc.x + 4, chc.y + 6), Tokens::Color::Accent(), 1.8f);
    if (it.clicked || (ImGui::IsKeyPressed(ImGuiKey_Escape) && c.drawer_open)) {
        c.drawer_back_request = true;
    }

    const char* title = DrawerBody::TitleFor(c.drawer_key);
    ImFont* fh = Tokens::Type::H3();
    Draw::TextL(c.dl, fh, ImVec2(bb_.x + 12, ha.y + (HH - (fh?fh->FontSize:15))*0.5f),
                Tokens::Color::Text(), title);

    ImVec2 body_a = ImVec2(a.x, hb.y);
    ImVec2 body_b = b;
    c.dl->PushClipRect(body_a, body_b, true);

    float save_x0 = c.content_x0, save_x1 = c.content_x1;
    float save_y0 = c.view_y0,   save_y1 = c.view_y1;
    float save_scroll = c.scroll_y;
    ImVec2 save_cursor = c.cursor;

    c.content_x0 = body_a.x + 18.f;
    c.content_x1 = body_b.x - 18.f;
    c.view_y0    = body_a.y;
    c.view_y1    = body_b.y;

    if (ImGui::IsMouseHoveringRect(body_a, body_b)) {
        float mw = ImGui::GetIO().MouseWheel;
        if (mw != 0.f) {
            c.drawer_scroll_y -= mw * 40.f;
            c.drawer_scroll_show_t = 1.f;
        }
    }
    if (c.drawer_scroll_y < 0)                       c.drawer_scroll_y = 0;
    if (c.drawer_scroll_y > c.drawer_max_scroll)     c.drawer_scroll_y = c.drawer_max_scroll;
    c.scroll_y = c.drawer_scroll_y;
    c.cursor = ImVec2(0, 18.f);

    DrawerBody::Render(c.drawer_key, s);

    float content_end = c.cursor.y;
    float view_h = body_b.y - body_a.y;
    float max_scroll = content_end - view_h;
    if (max_scroll < 0) max_scroll = 0;
    c.drawer_max_scroll = max_scroll;
    c.drawer_content_h  = content_end;

    c.drawer_scroll_show_t -= ImGui::GetIO().DeltaTime * 1.2f;
    if (c.drawer_scroll_show_t < 0.f) c.drawer_scroll_show_t = 0.f;
    if (max_scroll > 0.f && c.drawer_scroll_show_t > 0.01f) {
        float thumb_ratio = view_h / content_end;
        if (thumb_ratio > 1.f) thumb_ratio = 1.f;
        float thumb_h = view_h * thumb_ratio;
        if (thumb_h < 30.f) thumb_h = 30.f;
        float thumb_y = body_a.y + (c.drawer_scroll_y / max_scroll) * (view_h - thumb_h);
        float thumb_x = body_b.x - 8.f;
        ImU32 base = Theme::IsDarkMode() ? IM_COL32(255,255,255,90) : IM_COL32(0,0,0,90);
        ImU32 col  = (base & 0x00FFFFFF) | (((ImU32)(((base >> 24) & 0xFF) * c.drawer_scroll_show_t)) << 24);
        Draw::RectFilledRounded(c.dl, ImVec2(thumb_x, thumb_y),
                                ImVec2(thumb_x + 4.f, thumb_y + thumb_h), col, 2.f);
    }

    c.content_x0 = save_x0; c.content_x1 = save_x1;
    c.view_y0 = save_y0;   c.view_y1 = save_y1;
    c.scroll_y = save_scroll;
    c.cursor = save_cursor;

    c.dl->PopClipRect();

    if (c.drawer_back_request) {
        c.drawer_back_request = false;
        c.drawer_open = false;
    }
}

namespace DrawerBody {

const char* TitleFor(const char* key) {
    if (!key || !*key) return "Details";
    if (!std::strcmp(key, "pathfinder"))         return "Path settings";
    if (!std::strcmp(key, "keys-to-fly"))        return "Keys to Fly";
    if (!std::strcmp(key, "uncap-fps"))          return "Uncap FPS";
    if (!std::strcmp(key, "mine-pathfinder"))    return "Pathfinder";
    if (!std::strcmp(key, "mine-targets"))       return "Targets";
    if (!std::strcmp(key, "mine-combat"))        return "Combat";
    if (!std::strcmp(key, "mine-bankbot"))       return "BankBot";
    if (!std::strcmp(key, "mine-autosell"))      return "AutoSell";
    if (!std::strcmp(key, "mine-rejoin"))        return "Rejoin";
    if (!std::strcmp(key, "esp-row"))            return "ESP Type";
    if (!std::strcmp(key, "logger"))             return "Packet logger";
    if (!std::strcmp(key, "repeater"))           return "Packet repeater";
    if (!std::strcmp(key, "ignore"))             return "Ignore list";
    if (!std::strcmp(key, "custom-colors"))      return "Custom colors";
    if (!std::strcmp(key, "nether-pathfinder"))  return "Nether Pathfinder";
    if (!std::strcmp(key, "dbg-inv-pick"))       return "Select inventory item";
    return "Details";
}

void Render(const char* key, State& s) {
    if (!key || !*key) return;
    if (!std::strcmp(key, "pathfinder"))         { Pathfinder(s); return; }
    if (!std::strcmp(key, "keys-to-fly"))        { KeysToFly(s); return; }
    if (!std::strcmp(key, "uncap-fps"))          { UncapFps(s); return; }
    if (!std::strcmp(key, "mine-pathfinder"))    { MinePathfinder(s); return; }
    if (!std::strcmp(key, "mine-targets"))       { MineTargets(s); return; }
    if (!std::strcmp(key, "mine-combat"))        { MineCombat(s); return; }
    if (!std::strcmp(key, "mine-bankbot"))       { MineBankBot(s); return; }
    if (!std::strcmp(key, "mine-autosell"))      { MineAutoSell(s); return; }
    if (!std::strcmp(key, "mine-rejoin"))        { MineRejoin(s); return; }
    if (!std::strcmp(key, "esp-row"))            { EspRow(s); return; }
    if (!std::strcmp(key, "logger"))             { Logger(s); return; }
    if (!std::strcmp(key, "repeater"))           { Repeater(s); return; }
    if (!std::strcmp(key, "ignore"))             { Ignore(s); return; }
    if (!std::strcmp(key, "custom-colors"))      { CustomColors(s); return; }
    if (!std::strcmp(key, "nether-pathfinder"))  { NetherPathfinder(s); return; }
    if (!std::strcmp(key, "dbg-inv-pick"))       { DbgInvPicker(s); return; }
}

}
}

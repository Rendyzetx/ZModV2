#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Icons.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include <cstdio>

namespace ZmodUI {

void TitleBarRender(State& s) {
    Ctx& c = GetCtx();
    float H = Tokens::Size::TitleBarH;
    ImVec2 a = c.win_pos;
    ImVec2 b = ImVec2(c.win_pos.x + c.win_size.x, c.win_pos.y + H);

    if (Theme::ShouldUseBackdropBlur()) {

        ImU32 bandTint = IM_COL32(0, 0, 0, 38);
        c.dl->AddRectFilled(a, b, bandTint,
                            Tokens::Size::WindowRadius, ImDrawFlags_RoundCornersTop);
    } else {
        c.dl->AddRectFilled(a, b, Tokens::Color::BgSidebar(),
                            Tokens::Size::WindowRadius, ImDrawFlags_RoundCornersTop);
    }
    c.dl->AddLine(ImVec2(a.x, b.y), ImVec2(b.x, b.y), Tokens::Color::Line(), 1.f);

    ImVec2 drag_a = ImVec2(a.x + 80.f, a.y);
    ImVec2 drag_b = ImVec2(b.x - 120.f, b.y);
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mp = io.MousePos;
    bool over_drag = (mp.x >= drag_a.x && mp.x <= drag_b.x &&
                      mp.y >= drag_a.y && mp.y <= drag_b.y);

    if (!c.dragging && !c.boot_active && over_drag &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemActive()) {
        c.dragging = true;
        c.drag_grab_offset = ImVec2(mp.x - c.win_pos.x, mp.y - c.win_pos.y);

        if (c.win_user_pos.x <= -1e8f) c.win_user_pos = c.win_pos;
    }

    if (c.dragging) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            c.win_user_pos = ImVec2(mp.x - c.drag_grab_offset.x,
                                    mp.y - c.drag_grab_offset.y);
        } else {
            c.dragging = false;
        }
    }

    if (over_drag && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        c.win_user_pos = ImVec2(-1e9f, -1e9f);
        c.dragging = false;
    }

    float light_y = a.y + H * 0.5f;
    ImU32 lights[3] = {
        IM_COL32(255, 95, 87, 255),
        IM_COL32(254, 188, 46, 255),
        IM_COL32(40, 200, 64, 255),
    };
    for (int i = 0; i < 3; ++i) {
        ImVec2 lc = ImVec2(a.x + 14 + 12 + i * 20.f, light_y);
        c.dl->AddCircleFilled(lc, 6.f, lights[i], 16);
        c.dl->AddCircle(lc, 6.f, IM_COL32(0,0,0,30), 16, 0.5f);
    }

    ImFont* fh = Tokens::Type::H3();
    ImVec2 ts = Draw::TextSize(fh, "ZMod");
    Draw::TextL(c.dl, fh, ImVec2(a.x + (c.win_size.x - ts.x) * 0.5f, a.y + (H - ts.y) * 0.5f),
                Tokens::Color::Text(), "ZMod");

    ImFont* fb = Tokens::Type::BodySec();
    float xR = b.x - 14.f;
    {
        ImVec2 sa = ImVec2(xR - 22.f, a.y + (H - 22.f)*0.5f);
        ImVec2 sb = ImVec2(xR, sa.y + 22.f);
        char idb[] = "tb_search";
        Layout::Interact it = Layout::RegisterHit(idb, sa, sb);
        if (it.hovered) Draw::RectFilledRounded(c.dl, sa, sb, Tokens::Color::BgHover(), 6.f);
        Draw::IconVec(c.dl, ImVec2((sa.x+sb.x)*0.5f, (sa.y+sb.y)*0.5f), 14.f, Tokens::Color::TextSec(), Icons::SearchGlass);
        if (it.clicked) OpenPalette();
        xR -= (22.f + 12.f);
    }
    {
        ImVec2 ts2 = Draw::TextSize(fb, "Ready");
        Draw::TextL(c.dl, fb, ImVec2(xR - ts2.x, a.y + (H - ts2.y)*0.5f), Tokens::Color::TextSec(), "Ready");
        xR -= (ts2.x + 8.f);
    }
    {
        ImVec2 pip = ImVec2(xR - 6.f, a.y + H*0.5f);
        c.dl->AddCircleFilled(pip, 8.f, (Tokens::Color::Green() & 0x00FFFFFF) | (50 << 24), 16);
        c.dl->AddCircleFilled(pip, 3.5f, Tokens::Color::Green(), 16);
    }
    (void)s;
}

}

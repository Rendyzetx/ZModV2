#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../widgets/Primitives.h"
#include <cmath>

namespace ZmodUI {

void ResizeGripRender(State& s) {
    Ctx& c = GetCtx();
    float sz = 18.f;
    ImVec2 a = ImVec2(c.win_pos.x + c.win_size.x - sz, c.win_pos.y + c.win_size.y - sz);
    ImVec2 b = ImVec2(a.x + sz, a.y + sz);
    Layout::Interact it = Layout::RegisterHit("resize_grip", a, b);
    if (it.held) {
        if (!c.resizing) {
            c.resizing = true;
            c.resize_drag_origin = ImGui::GetIO().MousePos;
        }

        ImVec2 m = ImGui::GetIO().MousePos;
        float dx = m.x - c.win_pos.x;
        float dy = m.y - c.win_pos.y;

        float w_from_x = dx;
        float w_from_y = dy * Tokens::Size::WindowAspect;
        float new_w = (w_from_x + w_from_y) * 0.5f;
        if (new_w < Tokens::Size::WindowMinW) new_w = Tokens::Size::WindowMinW;
        c.win_user_w = new_w;
    } else {
        c.resizing = false;
    }

    ImU32 col = it.hovered ? Tokens::Color::Text() : Tokens::Color::TextTert();
    c.dl->AddLine(ImVec2(b.x - 4, b.y - 12), ImVec2(b.x - 12, b.y - 4), col, 1.5f);
    c.dl->AddLine(ImVec2(b.x - 4, b.y - 8),  ImVec2(b.x - 8,  b.y - 4), col, 1.5f);
    (void)s;
}

}

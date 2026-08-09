#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include "../Tween.h"
#include <cstring>

namespace ZmodUI {

void ToastRender(State& s) {
    Ctx& c = GetCtx();
    if (s.toast_request.pending) {
        std::strncpy(c.toast_msg, s.toast_request.msg, sizeof(c.toast_msg)-1);
        c.toast_dur = s.toast_request.duration_sec;
        c.toast_age = 0.f;
        s.toast_request.pending = false;
    }

    if (c.toast_dur <= 0.f && c.toast_t < 0.01f) return;

    float dt = ImGui::GetIO().DeltaTime;
    c.toast_age += dt;
    bool active = (c.toast_age < c.toast_dur);
    c.toast_t = Tween::SmoothStep(c.toast_t, active ? 1.f : 0.f, 8.f);
    if (c.toast_t < 0.01f && !active) { c.toast_dur = 0.f; return; }

    float t = Tokens::Motion::MacEase(c.toast_t);
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImFont* fb = Tokens::Type::Body();
    ImVec2 ts = Draw::TextSize(fb, c.toast_msg);
    float pad = 14.f;
    float W = ts.x + 18.f + 14.f + pad*2;
    float H = ts.y + 20.f;
    float yOff = (1.f - t) * 20.f;
    ImVec2 a = ImVec2(c.win_pos.x + c.win_size.x - 36.f - W,
                      c.win_pos.y + c.win_size.y - Tokens::Size::StatusStripH - 28.f - H + yOff);
    ImVec2 b = ImVec2(a.x + W, a.y + H);
    ImU32 alpha = (ImU32)(t * 255);
    ImU32 base_a = (Tokens::Color::BgCard() & 0x00FFFFFF) | (alpha << 24);
    ImU32 line_a = (Tokens::Color::Line() & 0x00FFFFFF) | (alpha << 24);
    Draw::DropShadow(dl, a, b, 10.f, IM_COL32(0,0,0,(int)(t*110)), 3);
    Draw::RectFilledRounded(dl, a, b, base_a, 10.f);
    Draw::RectStrokeRounded(dl, a, b, line_a, 10.f, 1.f);

    ImVec2 ic = ImVec2(a.x + pad + 9, a.y + H*0.5f);
    ImU32 green_a = (Tokens::Color::Green() & 0x00FFFFFF) | (alpha << 24);
    dl->AddCircleFilled(ic, 9.f, green_a, 18);
    Prim::DrawCheckmark(dl, ic, 9.f, IM_COL32(255,255,255,alpha));
    ImU32 text_a = (Tokens::Color::Text() & 0x00FFFFFF) | (alpha << 24);
    Draw::TextL(dl, fb, ImVec2(a.x + pad + 26, a.y + (H - ts.y)*0.5f), text_a, c.toast_msg);
}

}

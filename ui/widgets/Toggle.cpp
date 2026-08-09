#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Tween.h"

namespace ZmodUI {

static bool DrawToggle(ImVec2 pos, bool* value, ImGuiID id_seed) {
    Ctx& c = GetCtx();
    float W = Tokens::Size::SwitchW;
    float H = Tokens::Size::SwitchH;
    float K = Tokens::Size::SwitchKnob;

    ImVec2 a = pos;
    ImVec2 b = ImVec2(pos.x + W, pos.y + H);
    char id_buf[32]; ImFormatString(id_buf, sizeof(id_buf), "tg##%u", (unsigned)id_seed);
    Layout::Interact it = Layout::RegisterHit(id_buf, a, b);
    bool changed = false;
    if (it.clicked) { *value = !*value; changed = true; }

    ImGuiID kid = ImGui::GetID(id_buf);
    float t = Tween::ToFloat(kid, *value ? 1.f : 0.f, 12.f);

    ImU32 off_col = Theme::IsDarkMode() ? IM_COL32(72,72,74,255) : IM_COL32(209,209,214,255);
    ImU32 on_col  = Tokens::Color::Green();
    auto lerpU = [](ImU32 a, ImU32 b, float t) {
        int ar = a & 0xFF, ag = (a >> 8) & 0xFF, ab = (a >> 16) & 0xFF, aa = (a >> 24) & 0xFF;
        int br = b & 0xFF, bg = (b >> 8) & 0xFF, bb = (b >> 16) & 0xFF, ba = (b >> 24) & 0xFF;
        int rr = ar + (int)((br - ar) * t);
        int gg = ag + (int)((bg - ag) * t);
        int bbv = ab + (int)((bb - ab) * t);
        int aav = aa + (int)((ba - aa) * t);
        return IM_COL32(rr, gg, bbv, aav);
    };
    ImU32 track = lerpU(off_col, on_col, t);
    Draw::RectFilledRounded(c.dl, a, b, track, H*0.5f);

    float kx = a.x + 2.f + (W - K - 4.f) * t;
    float ky = a.y + (H - K) * 0.5f;
    ImVec2 kc = ImVec2(kx + K*0.5f, ky + K*0.5f);
    Draw::Knob(c.dl, kc, K*0.5f, IM_COL32_WHITE);
    return changed;
}

bool ToggleRow(const char* icon, const char* label, const char* desc, bool* value) {
    WRow::Opts o;
    o.icon_id = icon;
    o.label = label;
    o.desc = desc;
    o.no_icon = (icon == nullptr);
    WRow::RowResult r = WRow::Begin(o);
    ImVec2 p = ImVec2(r.end_b.x - Tokens::Size::SwitchW, (r.a.y + r.b.y)*0.5f - Tokens::Size::SwitchH*0.5f);
    ImGuiID seed = ImGui::GetID(label ? label : "tg");
    return DrawToggle(p, value, seed);
}

bool ToggleRow(const char* label, bool* value) {
    WRow::Opts o;
    o.no_icon = true;
    o.label = label;
    WRow::RowResult r = WRow::Begin(o);
    ImVec2 p = ImVec2(r.end_b.x - Tokens::Size::SwitchW, (r.a.y + r.b.y)*0.5f - Tokens::Size::SwitchH*0.5f);
    ImGuiID seed = ImGui::GetID(label ? label : "tg");
    return DrawToggle(p, value, seed);
}

}

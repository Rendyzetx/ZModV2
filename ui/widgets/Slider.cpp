#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Tween.h"
#include <cmath>

namespace ZmodUI {

static bool DrawSlider(ImVec2 pos, float* value, float vmin, float vmax, ImGuiID seed) {
    Ctx& c = GetCtx();
    float W = Tokens::Size::SliderW;
    float TH = Tokens::Size::SliderTrackH;
    float K = Tokens::Size::SliderKnob;

    ImVec2 a = pos;
    ImVec2 b = ImVec2(pos.x + W, pos.y + 24.f);
    char id_buf[32]; ImFormatString(id_buf, sizeof(id_buf), "sl##%u", (unsigned)seed);
    Layout::Interact it = Layout::RegisterHit(id_buf, a, b);

    bool changed = false;
    if (it.held) {
        float px = (ImGui::GetIO().MousePos.x - a.x) / W;
        if (px < 0) px = 0; if (px > 1) px = 1;
        float nv = vmin + px * (vmax - vmin);
        if (nv != *value) { *value = nv; changed = true; }
    }

    float t = (*value - vmin) / (vmax - vmin);
    if (t < 0) t = 0; if (t > 1) t = 1;

    ImU32 track = Theme::IsDarkMode() ? IM_COL32(72,72,74,255) : IM_COL32(209,209,214,255);
    ImU32 fill = Tokens::Color::Accent();

    ImVec2 ta = ImVec2(a.x, a.y + 12.f - TH*0.5f);
    ImVec2 tb = ImVec2(a.x + W, a.y + 12.f + TH*0.5f);
    Draw::RectFilledRounded(c.dl, ta, tb, track, TH*0.5f);
    ImVec2 fb = ImVec2(a.x + W * t, tb.y);
    Draw::RectFilledRounded(c.dl, ta, fb, fill, TH*0.5f);

    ImGuiID kid = ImGui::GetID(id_buf);
    float scale = Tween::ToFloat(kid, it.held ? 1.08f : 1.f, 14.f);
    ImVec2 kc = ImVec2(a.x + W*t, a.y + 12.f);
    Draw::Knob(c.dl, kc, K*0.5f*scale, IM_COL32_WHITE);
    return changed;
}

bool SliderRow(const char* label, const char* units, float* v, float vmin, float vmax) {
    WRow::Opts o;
    o.no_icon = true;
    o.label = label;
    char buf[64];
    if (units && *units) ImFormatString(buf, sizeof(buf), "%.2f %s", *v, units);
    else                 ImFormatString(buf, sizeof(buf), "%.2f", *v);
    o.desc = buf;
    WRow::RowResult r = WRow::Begin(o);
    ImVec2 p = ImVec2(r.end_b.x - Tokens::Size::SliderW, (r.a.y + r.b.y)*0.5f - 12.f);
    return DrawSlider(p, v, vmin, vmax, ImGui::GetID(label));
}

bool SliderRow(const char* label, const char* units, int* v, int vmin, int vmax) {
    WRow::Opts o;
    o.no_icon = true;
    o.label = label;
    char buf[64];
    if (units && *units) ImFormatString(buf, sizeof(buf), "%d %s", *v, units);
    else                 ImFormatString(buf, sizeof(buf), "%d", *v);
    o.desc = buf;
    WRow::RowResult r = WRow::Begin(o);
    ImVec2 p = ImVec2(r.end_b.x - Tokens::Size::SliderW, (r.a.y + r.b.y)*0.5f - 12.f);
    float fv = (float)*v;
    bool changed = DrawSlider(p, &fv, (float)vmin, (float)vmax, ImGui::GetID(label));
    if (changed) {
        int nv = (int)std::round(fv);
        if (nv < vmin) nv = vmin; if (nv > vmax) nv = vmax;
        *v = nv;
    }
    return changed;
}

}

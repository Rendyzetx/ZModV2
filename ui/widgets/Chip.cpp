#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include <cstdio>

namespace ZmodUI {

bool ChipRow(const char* const* labels, bool* values, int count) {
    Ctx& c = GetCtx();
    bool any_change = false;
    float pad_v = 12.f, pad_h = 14.f, gap = 6.f;
    ImFont* fb = Tokens::Type::BodySec();
    float h_chip = (fb ? fb->FontSize : 11.5f) + Tokens::Size::ChipPadV*2 + 2.f;
    float y = Layout::ContentToScreenY(c.cursor.y) + pad_v;
    float x0 = c.content_x0 + pad_h;
    float x  = x0;
    float row_h = pad_v;

    for (int i = 0; i < count; ++i) {
        const char* L = labels[i] ? labels[i] : "";
        ImVec2 ts = Draw::TextSize(fb, L);
        float w = ts.x + Tokens::Size::ChipPadH*2 + 2.f;
        if (x + w > c.content_x1 - pad_h) {
            x = x0;
            y += h_chip + gap;
            row_h += h_chip + gap;
        }
        ImVec2 a = ImVec2(x, y);
        ImVec2 b = ImVec2(x + w, y + h_chip);
        char id_buf[32]; ImFormatString(id_buf, sizeof(id_buf), "chip##%d_%p", i, (void*)L);
        Layout::Interact it = Layout::RegisterHit(id_buf, a, b);
        if (it.clicked) { values[i] = !values[i]; any_change = true; }

        ImU32 bg = values[i] ? Tokens::Color::Accent() : Tokens::Color::Bg();
        ImU32 fg = values[i] ? IM_COL32_WHITE : Tokens::Color::Text();
        ImU32 border = values[i] ? 0 : Tokens::Color::Line();
        if (it.hovered && !values[i]) bg = Tokens::Color::BgHover();

        Draw::RectFilledRounded(c.dl, a, b, bg, 999.f);
        if (border) Draw::RectStrokeRounded(c.dl, a, b, border, 999.f, 1.f);
        Draw::TextL(c.dl, fb, ImVec2(a.x + Tokens::Size::ChipPadH+1, a.y + Tokens::Size::ChipPadV+1), fg, L);
        x += w + gap;
    }
    row_h += h_chip + pad_v;
    c.cursor.y += row_h;
    return any_change;
}

bool ChipLevel(const char* label, bool* value, bool ) {
    Ctx& c = GetCtx();
    float pad_v = 12.f, pad_h = 14.f;
    ImFont* fb = Tokens::Type::Body();
    float h_chip = (fb ? fb->FontSize : 13) + 12.f;
    float y = Layout::ContentToScreenY(c.cursor.y) + pad_v;
    float x = c.content_x0 + pad_h;
    ImVec2 ts = Draw::TextSize(fb, label);
    float w = ts.x + 24.f;
    if (w < 38.f) w = 38.f;
    ImVec2 a = ImVec2(x, y);
    ImVec2 b = ImVec2(x + w, y + h_chip);
    char id_buf[32]; ImFormatString(id_buf, sizeof(id_buf), "chiplvl##%s", label);
    Layout::Interact it = Layout::RegisterHit(id_buf, a, b);
    bool changed = false;
    if (it.clicked) { *value = !*value; changed = true; }
    ImU32 bg = *value ? Tokens::Color::Green() : Tokens::Color::Bg();
    ImU32 fg = *value ? IM_COL32_WHITE : Tokens::Color::Text();
    ImU32 border = *value ? 0 : Tokens::Color::Line();
    if (it.hovered && !*value) bg = Tokens::Color::BgHover();
    Draw::RectFilledRounded(c.dl, a, b, bg, 999.f);
    if (border) Draw::RectStrokeRounded(c.dl, a, b, border, 999.f, 1.f);
    Draw::TextL(c.dl, fb, ImVec2(a.x + (w - ts.x)*0.5f, a.y + 6.f), fg, label);
    c.cursor.y += h_chip + pad_v;
    return changed;
}

}

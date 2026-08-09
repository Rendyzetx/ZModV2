#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../Tween.h"
#include <cstdio>

namespace ZmodUI {

struct PresetSwatch { ImU32 a, b, c; };

static const PresetSwatch kSwatches[Theme::PresetCount] = {
     { IM_COL32(36,40,48,200),  IM_COL32(64,156,255,255),  IM_COL32(245,247,252,255) },
     { IM_COL32(28,28,30,255),  IM_COL32(64,156,255,255),  IM_COL32(245,245,247,255) },
     { IM_COL32(245,245,247,255),IM_COL32(10,132,255,255), IM_COL32(29,29,31,255)    },
     { IM_COL32(14,28,22,255),  IM_COL32(132,204,22,255),  IM_COL32(232,244,236,255) },
     { IM_COL32(28,18,16,255),  IM_COL32(251,146,60,255),  IM_COL32(252,242,232,255) },
     { IM_COL32(8,18,32,255),   IM_COL32(34,211,238,255),  IM_COL32(232,242,252,255) },
     { IM_COL32(28,18,24,255),  IM_COL32(244,114,182,255), IM_COL32(252,232,240,255) },
     { IM_COL32(18,18,18,255),  IM_COL32(245,245,245,255), IM_COL32(160,160,160,255) },
     { IM_COL32(40,40,46,255),  IM_COL32(120,120,255,255), IM_COL32(220,220,225,255) },
};

void ThemeGrid(int* current_index) {
    Ctx& c = GetCtx();
    float pad = 14.f, gap = 12.f;
    float W = c.content_x1 - c.content_x0 - pad*2;
    int   cols = 3;
    float card_w = (W - gap*(cols-1)) / (float)cols;
    float card_h = 78.f;
    float y0 = Layout::ContentToScreenY(c.cursor.y) + pad;

    int real = (int)Theme::CurrentPreset();
    if (current_index && *current_index != real) *current_index = real;

    for (int i = 0; i < Theme::PresetCount; ++i) {
        int col = i % cols;
        int row = i / cols;
        float x = c.content_x0 + pad + col * (card_w + gap);
        float y = y0 + row * (card_h + gap);
        ImVec2 a = ImVec2(x, y);
        ImVec2 b = ImVec2(x + card_w, y + card_h);
        char idb[32]; ImFormatString(idb, sizeof(idb), "themec_%d", i);
        Layout::Interact it = Layout::RegisterHit(idb, a, b);
        if (it.clicked) {
            if (current_index) *current_index = i;
            Theme::SetPreset((Theme::Preset)i);
        }
        bool active = ((int)Theme::CurrentPreset() == i);
        ImGuiID gid = ImGui::GetID(idb);
        float gt = Tween::ToFloat(gid, active ? 1.f : 0.f, 6.f);
        ImU32 border = active ? Tokens::Color::Accent() : (it.hovered ? Tokens::Color::Accent() : Tokens::Color::Line());

        if (gt > 0.01f) {
            ImU32 glow = (Tokens::Color::Accent() & 0x00FFFFFF) | ((ImU32)(gt * 60) << 24);
            for (int s = 1; s <= 3; ++s) {
                ImVec2 ga = ImVec2(a.x - s*1.5f, a.y - s*1.5f);
                ImVec2 gb = ImVec2(b.x + s*1.5f, b.y + s*1.5f);
                Draw::RectStrokeRounded(c.dl, ga, gb, glow, Tokens::Size::CardRadius + s, 1.f);
            }
        }

        const PresetSwatch& sw = kSwatches[i];

        Draw::RectFilledRounded(c.dl, a, b, sw.a, Tokens::Size::CardRadius);
        Draw::RectStrokeRounded(c.dl, a, b, border, Tokens::Size::CardRadius, active ? 2.f : 1.f);

        ImFont* fb = Tokens::Type::Body();
        Draw::TextL(c.dl, fb, ImVec2(a.x + 12, a.y + 12), sw.c, Theme::PresetName((Theme::Preset)i));

        ImU32 chips[2] = { sw.b, sw.c };
        for (int k = 0; k < 2; ++k) {
            ImVec2 sa = ImVec2(a.x + 12 + k * 22.f, a.y + 42);
            ImVec2 sb = ImVec2(sa.x + 18, sa.y + 18);
            Draw::RectFilledRounded(c.dl, sa, sb, chips[k], 4.f);
            Draw::RectStrokeRounded(c.dl, sa, sb, IM_COL32(255,255,255,40), 4.f, 1.f);
        }
    }
    int rows_used = (Theme::PresetCount + cols - 1) / cols;
    c.cursor.y += pad*2 + card_h * rows_used + gap * (rows_used - 1);
}

void ColorList() {
    Ctx& c = GetCtx();
    ImFont* fb = Tokens::Type::Body();
    ImFont* fs = Tokens::Type::BodySec();
    float row_h = 46.f;

    for (int i = 0; i < Theme::PickerTokenCount; ++i) {
        Theme::PickerToken t = (Theme::PickerToken)i;
        float y = Layout::ContentToScreenY(c.cursor.y);
        ImVec2 a = ImVec2(c.content_x0, y);
        ImVec2 b = ImVec2(c.content_x1, y + row_h);

        float sw_size = 28.f;
        ImVec2 sw_a = ImVec2(b.x - 14 - sw_size, a.y + (row_h - sw_size) * 0.5f);
        ImVec2 sw_b = ImVec2(sw_a.x + sw_size, sw_a.y + sw_size);

        ImVec4 cur4 = Theme::GetPicker(t);
        ImU32  cur  = ImGui::ColorConvertFloat4ToU32(cur4);

        char idsw[40]; ImFormatString(idsw, sizeof(idsw), "ctk_sw_%d", i);
        Layout::Interact sh = Layout::RegisterHit(idsw, sw_a, sw_b);

        bool row_hover = ImGui::IsMouseHoveringRect(a, b);
        if (row_hover) Draw::RectFilledRounded(c.dl, a, b, Tokens::Color::BgHover(), 6.f);

        float ty = a.y + 6.f;
        Draw::TextL(c.dl, fb, ImVec2(a.x + 14, ty), Tokens::Color::Text(), Theme::PickerTokenName(t));
        Draw::TextL(c.dl, fs, ImVec2(a.x + 14, ty + (fb ? fb->FontSize : 13) + 4.f),
                    Tokens::Color::TextSec(), Theme::PickerTokenDesc(t));

        ImU32 swBorder = sh.hovered ? Tokens::Color::Accent() : IM_COL32(255,255,255,60);
        Draw::RectFilledRounded(c.dl, sw_a, sw_b, cur, 6.f);
        Draw::RectStrokeRounded(c.dl, sw_a, sw_b, swBorder, 6.f, sh.hovered ? 2.f : 1.f);

        char hex[16];
        ImFormatString(hex, sizeof(hex), "#%02X%02X%02X",
                       (int)(cur4.x * 255), (int)(cur4.y * 255), (int)(cur4.z * 255));
        ImVec2 hts = Draw::TextSize(fs, hex);
        Draw::TextL(c.dl, fs,
                    ImVec2(sw_a.x - 10.f - hts.x, a.y + (row_h - hts.y) * 0.5f),
                    Tokens::Color::TextSec(), hex);

        char popid[40]; ImFormatString(popid, sizeof(popid), "##theme_cp_%d", i);
        if (sh.clicked) ImGui::OpenPopup(popid);

        ImGuiIO& io = ImGui::GetIO();
        float popW = 280.f;
        float popX = sw_a.x - popW - 8.f;
        if (popX < 8.f) popX = sw_b.x + 8.f;
        if (popX + popW > io.DisplaySize.x - 8.f) popX = io.DisplaySize.x - popW - 8.f;
        ImGui::SetNextWindowPos(ImVec2(popX, sw_b.y + 6.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(popW, 0.f), ImGuiCond_Always);
        ImVec4 work = cur4;
        if (Draw::StyledColorPickerPopup(popid, Theme::PickerTokenName(t), &work, false)) {
            Theme::SetPicker(t, work);
        }

        c.cursor.y += row_h;
    }
}

}

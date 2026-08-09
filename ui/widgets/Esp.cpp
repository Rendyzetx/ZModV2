#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include <cstdio>
#include <cstring>

namespace ZmodUI {

struct EspTypeMeta {
    const char* name;
    bool lines, boxes, health;
};
static const EspTypeMeta kEspTypes[11] = {
    { "Local Player",   false, true,  true  },
    { "Players",        true,  true,  true  },
    { "AI Enemies",     true,  true,  true  },
    { "Gemstones",      true,  true,  false },
    { "Collectables",   true,  true,  false },
    { "Gift Boxes",     true,  true,  false },
    { "Exit Portals",   true,  true,  false },
    { "Light Crystals", true,  true,  false },
    { "Pots",           true,  true,  false },
    { "Treasure",       true,  true,  false },
    { "Nether Boss",    true,  true,  false },
};

static const ImU32 kEspPalette[7] = {
    IM_COL32(10,132,255,255),  IM_COL32(52,199,89,255),
    IM_COL32(255,149,0,255),   IM_COL32(255,59,48,255),
    IM_COL32(90,200,250,255),  IM_COL32(191,90,242,255),
    IM_COL32(255,214,10,255),
};

void EspMatrixInitDefaults(State& s) {
    static bool init = false;
    if (init) return; init = true;

    for (int i = 0; i < 11; ++i) {
        s.esp_color[i] = ImGui::ColorConvertU32ToFloat4(kEspPalette[i % 7]);
    }
}

static void DrawEspCell(ImDrawList* dl, ImVec2 a, bool on, bool disabled, const char* id_str) {
    ImVec2 b = ImVec2(a.x + Tokens::Size::EspCellW, a.y + Tokens::Size::EspCellH);
    ImU32 bg = on ? Tokens::Color::Accent() : 0u;
    ImU32 border = on ? 0 : Tokens::Color::Line();
    if (disabled) { bg = (bg & 0x00FFFFFF) | (60 << 24); border = (Tokens::Color::Line() & 0x00FFFFFF) | (60 << 24); }
    Draw::RectFilledRounded(dl, a, b, bg, 5.f);
    if (border) Draw::RectStrokeRounded(dl, a, b, border, 5.f, 1.f);
    if (on) Prim::DrawCheckmark(dl, ImVec2((a.x+b.x)*0.5f, (a.y+b.y)*0.5f), 12.f, IM_COL32_WHITE);
}

void EspMatrix(State& s) {
    EspMatrixInitDefaults(s);
    Ctx& c = GetCtx();

    float W = c.content_x1 - c.content_x0;
    float col_name = W * 0.30f;
    float col_lines = W * 0.135f, col_boxes = W * 0.135f, col_health = W * 0.135f, col_color = W * 0.135f, col_chev = W * 0.16f;

    ImFont* fh = Tokens::Type::Eyebrow();
    float hdr_h = 28.f;
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 hdrA = ImVec2(c.content_x0, y);
    ImVec2 hdrB = ImVec2(c.content_x1, y + hdr_h);
    Draw::RectFilledRounded(c.dl, hdrA, hdrB, Tokens::Color::Bg(), Tokens::Size::CardRadius);
    Draw::RectStrokeRounded(c.dl, hdrA, hdrB, Tokens::Color::Line(), Tokens::Size::CardRadius, 1.f);
    auto hdr_text = [&](const char* t, float x, float w, bool center) {
        ImVec2 ts = Draw::TextSize(fh, t);
        float tx = center ? (x + (w - ts.x)*0.5f) : (x + 14.f);
        Draw::TextL(c.dl, fh, ImVec2(tx, y + (hdr_h - ts.y)*0.5f), Tokens::Color::TextSec(), t);
    };
    hdr_text("TYPE", c.content_x0, col_name, false);
    hdr_text("LINES", c.content_x0 + col_name, col_lines, true);
    hdr_text("BOXES", c.content_x0 + col_name + col_lines, col_boxes, true);
    hdr_text("HEALTH", c.content_x0 + col_name + col_lines + col_boxes, col_health, true);
    hdr_text("COLOR", c.content_x0 + col_name + col_lines + col_boxes + col_health, col_color, true);
    c.cursor.y += hdr_h;

    float row_h = 38.f;
    for (int i = 0; i < 11; ++i) {
        float ry = Layout::ContentToScreenY(c.cursor.y);
        ImVec2 ra = ImVec2(c.content_x0, ry);
        ImVec2 rb = ImVec2(c.content_x1, ry + row_h);
        char id_buf[32]; ImFormatString(id_buf, sizeof(id_buf), "esp_row_%d", i);
        Layout::Interact rIt = Layout::RegisterHit(id_buf, ra, rb);
        if (rIt.hovered) Draw::RectFilledRounded(c.dl, ra, rb, Tokens::Color::BgHover(), 0.f);
        c.dl->AddLine(ImVec2(ra.x, rb.y), ImVec2(rb.x, rb.y), Tokens::Color::LineSoft(), 1.f);

        ImFont* fb = Tokens::Type::Body();
        Draw::TextL(c.dl, fb, ImVec2(ra.x + 14, ra.y + (row_h - (fb?fb->FontSize:13))*0.5f), Tokens::Color::Text(), kEspTypes[i].name);

        auto cell_center = [&](float xs, float w) {
            return ImVec2(xs + (w - Tokens::Size::EspCellW)*0.5f, ra.y + (row_h - Tokens::Size::EspCellH)*0.5f);
        };

        {
            ImVec2 ca = cell_center(c.content_x0 + col_name, col_lines);
            ImVec2 cb_ = ImVec2(ca.x + Tokens::Size::EspCellW, ca.y + Tokens::Size::EspCellH);
            char idc[32]; ImFormatString(idc, sizeof(idc), "espL_%d", i);
            Layout::Interact it = Layout::RegisterHit(idc, ca, cb_);
            if (it.clicked && kEspTypes[i].lines) s.esp_lines[i] = !s.esp_lines[i];
            DrawEspCell(c.dl, ca, s.esp_lines[i], !kEspTypes[i].lines, idc);
        }

        {
            ImVec2 ca = cell_center(c.content_x0 + col_name + col_lines, col_boxes);
            ImVec2 cb_ = ImVec2(ca.x + Tokens::Size::EspCellW, ca.y + Tokens::Size::EspCellH);
            char idc[32]; ImFormatString(idc, sizeof(idc), "espB_%d", i);
            Layout::Interact it = Layout::RegisterHit(idc, ca, cb_);
            if (it.clicked && kEspTypes[i].boxes) s.esp_boxes[i] = !s.esp_boxes[i];
            DrawEspCell(c.dl, ca, s.esp_boxes[i], !kEspTypes[i].boxes, idc);
        }

        {
            ImVec2 ca = cell_center(c.content_x0 + col_name + col_lines + col_boxes, col_health);
            ImVec2 cb_ = ImVec2(ca.x + Tokens::Size::EspCellW, ca.y + Tokens::Size::EspCellH);
            char idc[32]; ImFormatString(idc, sizeof(idc), "espH_%d", i);
            Layout::Interact it = Layout::RegisterHit(idc, ca, cb_);
            if (it.clicked && kEspTypes[i].health) s.esp_health[i] = !s.esp_health[i];
            DrawEspCell(c.dl, ca, s.esp_health[i], !kEspTypes[i].health, idc);
        }

        {
            float sw_w = Tokens::Size::EspCellW - 8.f;
            float sw_h = Tokens::Size::EspCellH;
            ImVec2 ca = ImVec2(c.content_x0 + col_name + col_lines + col_boxes + col_health + (col_color - sw_w)*0.5f, ra.y + (row_h - sw_h)*0.5f);
            ImVec2 cb_ = ImVec2(ca.x + sw_w, ca.y + sw_h);
            ImU32 col = ImGui::ColorConvertFloat4ToU32(s.esp_color[i]);
            Draw::RectFilledRounded(c.dl, ca, cb_, col, 4.f);
            Draw::RectStrokeRounded(c.dl, ca, cb_, IM_COL32(0,0,0,40), 4.f, 1.f);
        }

        Prim::DrawChevron(c.dl, ImVec2(rb.x - 18.f, (ra.y+rb.y)*0.5f), 10.f, Tokens::Color::TextTert());
        if (rIt.clicked) {
            OpenDrawer("esp-row");
            GetCtx().esp_drawer_idx = i;
        }
        c.cursor.y += row_h;
    }
    c.cursor.y += Tokens::Size::GroupMarginB;
}

int CurrentEspIndex() { return GetCtx().esp_drawer_idx; }

}

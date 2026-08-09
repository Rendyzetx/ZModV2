#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Tween.h"
#include <cstdio>

namespace ZmodUI {

namespace {

    ImGuiID g_open_id   = 0;
    int     g_open_sel  = 0;
    ImVec2  g_open_anchor;
    float   g_open_w = 160.f;
    const char* const* g_open_items = nullptr;
    int g_open_count = 0;
    int* g_open_out_idx = nullptr;
    int g_open_frame = -2;
}

bool ComboRow(const char* label, const char* const* items, int item_count, int* current_idx) {
    WRow::Opts o;
    o.no_icon = false;
    o.icon_id = "blank";
    o.label = label;
    WRow::RowResult r = WRow::Begin(o);
    Ctx& c = GetCtx();

    ImFont* f = Tokens::Type::BodySec();
    const char* cur = (*current_idx >= 0 && *current_idx < item_count) ? items[*current_idx] : "";
    ImVec2 ts = Draw::TextSize(f, cur);
    float W = ts.x + 36.f;
    if (W < 120.f) W = 120.f;
    float H = 22.f;
    ImVec2 a = ImVec2(r.end_b.x - W, (r.a.y + r.b.y)*0.5f - H*0.5f);
    ImVec2 b = ImVec2(r.end_b.x, a.y + H);
    ImGuiID id = ImGui::GetID(label);
    char id_buf[40]; ImFormatString(id_buf, sizeof(id_buf), "cb##%u_%s", (unsigned)id, label ? label : "");
    Layout::Interact it = Layout::RegisterHit(id_buf, a, b);
    Draw::RectFilledRounded(c.dl, a, b, Tokens::Color::BgCard(), 6.f);
    Draw::RectStrokeRounded(c.dl, a, b, Tokens::Color::Line(), 6.f, 1.f);
    Draw::TextL(c.dl, f, ImVec2(a.x + 10.f, a.y + (H - ts.y)*0.5f), Tokens::Color::Text(), cur);

    Prim::DrawChevron(c.dl, ImVec2(b.x - 12.f, (a.y + b.y)*0.5f), 8.f, Tokens::Color::TextSec());

    if (it.clicked) {
        if (g_open_id == id) { g_open_id = 0; }
        else {
            g_open_id = id;
            g_open_sel = *current_idx;
            g_open_anchor = ImVec2(a.x, b.y + 4.f);
            g_open_w = W;
            g_open_items = items;
            g_open_count = item_count;
            g_open_out_idx = current_idx;
            g_open_frame = ImGui::GetFrameCount();
        }
    }

    return false;
}

ImVec2 g_open_rect_a = ImVec2(0,0);
ImVec2 g_open_rect_b = ImVec2(0,0);

bool ComboPopupBlocksPoint(ImVec2 p) {
    if (g_open_id == 0) return false;
    return p.x >= g_open_rect_a.x && p.x <= g_open_rect_b.x
        && p.y >= g_open_rect_a.y && p.y <= g_open_rect_b.y;
}

void DrawComboPopup() {
    if (g_open_id == 0 || !g_open_items) {
        g_open_rect_a = g_open_rect_b = ImVec2(0,0);
        return;
    }
    Ctx& c = GetCtx();

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImFont* f = Tokens::Type::BodySec();
    float item_h = (f ? f->FontSize : 12) + 10.f;
    float h = item_h * g_open_count + 8.f;
    ImVec2 a = g_open_anchor;
    ImVec2 b = ImVec2(a.x + g_open_w, a.y + h);
    g_open_rect_a = a;
    g_open_rect_b = b;
    Draw::DropShadow(dl, a, b, 8.f, IM_COL32(0,0,0,90), 2);
    Draw::RectFilledRounded(dl, a, b, Tokens::Color::BgCard(), 8.f);
    Draw::RectStrokeRounded(dl, a, b, Tokens::Color::Line(), 8.f, 1.f);

    ImVec2 mouse = ImGui::GetMousePos();
    bool   clickedFrame = ImGui::GetFrameCount() > g_open_frame
                          && ImGui::IsMouseClicked(0);

    bool consumedInsidePopup = false;
    for (int i = 0; i < g_open_count; ++i) {
        ImVec2 ia = ImVec2(a.x + 4, a.y + 4 + i*item_h);
        ImVec2 ib = ImVec2(b.x - 4, ia.y + item_h);
        bool hovered = (mouse.x >= ia.x && mouse.x <= ib.x &&
                        mouse.y >= ia.y && mouse.y <= ib.y);
        if (hovered) Draw::RectFilledRounded(dl, ia, ib, Tokens::Color::BgHover(), 4.f);
        ImU32 fg = (i == g_open_sel) ? Tokens::Color::Accent() : Tokens::Color::Text();
        Draw::TextL(dl, f, ImVec2(ia.x + 8, ia.y + 5), fg, g_open_items[i]);
        if (hovered && clickedFrame) {
            *g_open_out_idx = i;
            g_open_sel = i;
            g_open_id = 0;
            g_open_items = nullptr;

            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            consumedInsidePopup = true;
            break;
        }
    }
    if (consumedInsidePopup) {
        g_open_rect_a = g_open_rect_b = ImVec2(0,0);
        return;
    }

    bool insidePopup = (mouse.x >= a.x && mouse.x <= b.x &&
                        mouse.y >= a.y && mouse.y <= b.y);
    if (clickedFrame && !insidePopup) {
        g_open_id = 0;
        g_open_items = nullptr;
        g_open_rect_a = g_open_rect_b = ImVec2(0,0);
    }
    (void)c;
}

}

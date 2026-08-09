#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include <cstdio>

namespace ZmodUI {

static void PnbGridStatus(State& s) {
    Ctx& c = GetCtx();
    const float cell = 22.f, gap = 5.f;
    float gx = c.content_x0 + 14.f;
    float gy = Layout::ContentToScreenY(c.cursor.y);
    float gridW = 5 * cell + 4 * gap;
    float gridH = 5 * cell + 4 * gap;

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            ImVec2 a = ImVec2(gx + col * (cell + gap), gy + row * (cell + gap));
            ImVec2 b = ImVec2(a.x + cell, a.y + cell);
            bool center = (row == 2 && col == 2);
            bool on = s.pnb_offsets[row][col];
            if (!center) {
                char idb[24]; ImFormatString(idb, sizeof(idb), "pnbo_%d_%d", row, col);
                Layout::Interact it = Layout::RegisterHit(idb, a, b);
                if (it.clicked) { s.pnb_offsets[row][col] = !on; on = s.pnb_offsets[row][col]; }
            }
            ImU32 fill = center ? Tokens::Color::Orange()
                       : on     ? Tokens::Color::Accent()
                                : 0u;
            Draw::RectFilledRounded(c.dl, a, b, fill, 5.f);
            if (!center && !on)
                Draw::RectStrokeRounded(c.dl, a, b, Tokens::Color::Line(), 5.f, 1.f);
            ImVec2 ctr = ImVec2((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
            ImU32 dot = (center || on) ? IM_COL32_WHITE : Tokens::Color::TextTert();
            c.dl->AddCircleFilled(ctr, 2.5f, dot);
        }
    }

    float sx = gx + gridW + 24.f;
    ImFont* fb = Tokens::Type::Body();
    ImFont* fs = Tokens::Type::BodySec();

    Draw::TextL(c.dl, fb, ImVec2(sx, gy + 4.f), Tokens::Color::Text(), "Place & Break");

    char anchorStr[48];
    if (s.pnb_has_anchor)
        ImFormatString(anchorStr, sizeof(anchorStr), "anchor (%d, %d)", s.pnb_anchor_x, s.pnb_anchor_y);
    else
        ImFormatString(anchorStr, sizeof(anchorStr), "anchor not saved");
    Draw::TextL(c.dl, fs, ImVec2(sx, gy + 26.f), Tokens::Color::TextSec(), anchorStr);

    char heldStr[48];
    if (s.pnb_sel_block_type > 0)
        ImFormatString(heldStr, sizeof(heldStr), "block #%d selected", s.pnb_sel_block_type);
    else
        ImFormatString(heldStr, sizeof(heldStr), "pick a block below");
    Draw::TextL(c.dl, fs, ImVec2(sx, gy + 44.f),
                s.pnb_sel_block_type > 0 ? Tokens::Color::TextSec() : Tokens::Color::Orange(),
                heldStr);

    char blocks[24]; ImFormatString(blocks, sizeof(blocks), "%d blocks", s.pnb_sel_qty);
    ImVec2 tb = Draw::TextSize(fs, blocks);
    Draw::TextL(c.dl, fs, ImVec2(c.content_x1 - 14.f - tb.x, gy + 4.f),
                Tokens::Color::TextSec(), blocks);

    const char* st = s.pnb_phase_label ? s.pnb_phase_label : "stopped";
    ImVec2 tp = Draw::TextSize(fs, st);
    Draw::TextL(c.dl, fs, ImVec2(c.content_x1 - 14.f - tp.x, gy + 26.f),
                s.pnb_enabled ? Tokens::Color::Green() : Tokens::Color::TextTert(), st);

    c.cursor.y += gridH + 10.f;
}

static int PnbButtonRow(const char* l1, const char* l2, const char* l3) {
    Ctx& c = GetCtx();
    float y  = Layout::ContentToScreenY(c.cursor.y);
    float W  = c.content_x1 - c.content_x0;
    float gap = 8.f;
    float bw = (W - 28.f - gap * 2) / 3.f;
    float bh = 30.f;
    float x0 = c.content_x0 + 14.f;
    ImFont* fb = Tokens::Type::Body();
    const char* labels[3] = { l1, l2, l3 };
    int clicked = 0;
    for (int i = 0; i < 3; ++i) {
        ImVec2 a = ImVec2(x0 + i * (bw + gap), y);
        ImVec2 b = ImVec2(a.x + bw, y + bh);
        char idb[16]; ImFormatString(idb, sizeof(idb), "pnbb_%d", i);
        Layout::Interact it = Layout::RegisterHit(idb, a, b);
        bool destructive = (i == 2);
        ImU32 bg = destructive
            ? (it.held ? IM_COL32(255,59,48,90) : it.hovered ? IM_COL32(255,59,48,45) : Tokens::Color::BgCard())
            : (it.held ? Tokens::Color::BgPressed() : it.hovered ? Tokens::Color::BgHover() : Tokens::Color::BgCard());
        Draw::RectFilledRounded(c.dl, a, b, bg, 6.f);
        Draw::RectStrokeRounded(c.dl, a, b, Tokens::Color::Line(), 6.f, 1.f);
        ImVec2 ts = Draw::TextSize(fb, labels[i]);
        ImU32 tc = destructive ? Tokens::Color::Red() : Tokens::Color::Text();
        Draw::TextL(c.dl, fb, ImVec2(a.x + (bw - ts.x) * 0.5f, a.y + (bh - ts.y) * 0.5f), tc, labels[i]);
        if (it.clicked) clicked = i + 1;
    }
    c.cursor.y += bh + 12.f;
    return clicked;
}

void RailPnb(State& s) {
    PageHead("Place & Break", nullptr);

    GroupTitle("Pattern");
    BeginGroup();
        PnbGridStatus(s);
    EndGroup();

    GroupTitle("Controls");
    BeginGroup();

        static char        s_items[24][80];
        static const char* s_itemPtrs[24];
        int n = s.pnb_inv_count; if (n > 24) n = 24;
        for (int i = 0; i < n; ++i) {
            const char* nm = (s.pnb_inv_name[i][0]) ? s.pnb_inv_name[i] : "Block";
            ImFormatString(s_items[i], sizeof(s_items[i]), "%s   (x%d)", nm, s.pnb_inv_qty[i]);
            s_itemPtrs[i] = s_items[i];
        }
        if (n > 0) ComboRow("Block to farm", s_itemPtrs, n, &s.pnb_sel_idx);
        else       HelpText("No placeable blocks in your inventory.");
        ToggleRow("arrow", "Auto Collect", nullptr, &s.pnb_auto_collect);
        int btn = PnbButtonRow("Save", "Start", "Stop");
        if      (btn == 1 && s.on_pnb_save_anchor) s.on_pnb_save_anchor();
        else if (btn == 2) s.pnb_enabled = true;
        else if (btn == 3) s.pnb_enabled = false;
    EndGroup();
}

}

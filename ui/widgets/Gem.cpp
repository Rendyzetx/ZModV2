#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include <cstdio>

namespace ZmodUI {

void GemMatrix(bool grid[5][5]) {
    Ctx& c = GetCtx();
    const char* sizes[5] = { "Tiny", "Small", "Medium", "Large", "Huge" };
    const char* names[5] = { "Topaz", "Emerald", "Sapphire", "Ruby", "Diamond" };

    ImFont* fh = Tokens::Type::BodySec();
    float row_h = 30.f;
    float W = c.content_x1 - c.content_x0;
    float col_name = 80.f;
    float col_other = (W - col_name - 28.f) / 5.f;

    float y = Layout::ContentToScreenY(c.cursor.y);
    for (int i = 0; i < 5; ++i) {
        ImVec2 ts = Draw::TextSize(fh, sizes[i]);
        float cx = c.content_x0 + 14.f + col_name + col_other*i + (col_other - ts.x)*0.5f;
        Draw::TextL(c.dl, fh, ImVec2(cx, y + 8.f), Tokens::Color::TextSec(), sizes[i]);
    }
    c.cursor.y += row_h;

    for (int g = 0; g < 5; ++g) {
        float ry = Layout::ContentToScreenY(c.cursor.y);
        ImFont* fb = Tokens::Type::BodySec();
        Draw::TextL(c.dl, fb, ImVec2(c.content_x0 + 14.f, ry + 8.f), Tokens::Color::Text(), names[g]);
        for (int sz = 0; sz < 5; ++sz) {
            float cw = 22.f, ch = 22.f;
            float cx = c.content_x0 + 14.f + col_name + col_other*sz + (col_other - cw)*0.5f;
            ImVec2 a = ImVec2(cx, ry + (row_h - ch)*0.5f);
            ImVec2 b = ImVec2(a.x + cw, a.y + ch);
            char idb[32]; ImFormatString(idb, sizeof(idb), "gemm_%d_%d", g, sz);
            Layout::Interact it = Layout::RegisterHit(idb, a, b);
            if (it.clicked) grid[g][sz] = !grid[g][sz];
            ImU32 bg = grid[g][sz] ? Tokens::Color::Accent() : 0u;
            ImU32 border = grid[g][sz] ? 0 : Tokens::Color::Line();
            Draw::RectFilledRounded(c.dl, a, b, bg, 5.f);
            if (border) Draw::RectStrokeRounded(c.dl, a, b, border, 5.f, 1.f);
            if (grid[g][sz]) Prim::DrawCheckmark(c.dl, ImVec2((a.x+b.x)*0.5f, (a.y+b.y)*0.5f), 12.f, IM_COL32_WHITE);
        }
        c.cursor.y += row_h;
    }
    c.cursor.y += Tokens::Size::GroupMarginB;
}

}

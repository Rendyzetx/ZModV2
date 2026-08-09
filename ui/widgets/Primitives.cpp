#include "Primitives.h"
#include "Row.h"
#include "../Internal.h"
#include "../Theme.h"
#include "../Tokens.h"
#include "../Icons.h"
#include "../Fonts.h"
#include "../Blur.h"
#include "../../features/visual/SpriteAtlas.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>

namespace ZmodUI {

namespace Prim {

void DrawCard(ImDrawList* dl, ImVec2 a, ImVec2 b, ImU32 bg, ImU32 line, float radius) {
    dl->AddRectFilled(a, b, bg, radius);
    dl->AddRect(a, b, line, radius, 0, 1.f);
}

void DrawChevron(ImDrawList* dl, ImVec2 c, float size, ImU32 col) {
    float s = size * 0.5f;
    ImVec2 a = ImVec2(c.x - s*0.55f, c.y - s);
    ImVec2 m = ImVec2(c.x + s*0.55f, c.y);
    ImVec2 e = ImVec2(c.x - s*0.55f, c.y + s);
    dl->AddLine(a, m, col, 1.6f);
    dl->AddLine(m, e, col, 1.6f);
}

void DrawCheckmark(ImDrawList* dl, ImVec2 c, float size, ImU32 col) {
    float s = size * 0.5f;
    ImVec2 a = ImVec2(c.x - s*0.7f, c.y);
    ImVec2 m = ImVec2(c.x - s*0.15f, c.y + s*0.55f);
    ImVec2 e = ImVec2(c.x + s*0.7f, c.y - s*0.55f);
    dl->AddLine(a, m, col, 1.8f);
    dl->AddLine(m, e, col, 1.8f);
}

void DrawSpinner(ImDrawList* dl, ImVec2 c, float r, ImU32 col, float t) {
    int seg = 24;
    float angOff = t * 6.28318f * 1.5f;
    for (int i = 0; i < seg; ++i) {
        float a = (float)i / seg * 6.28318f + angOff;
        float fade = (float)i / seg;
        ImU32 cc = (col & 0x00FFFFFF) | (((ImU32)(fade*255)) << 24);
        dl->PathArcTo(c, r, a, a + 6.28318f/seg);
    }
}

}

namespace Draw {

void RectFilledRounded(ImDrawList* dl, ImVec2 a, ImVec2 b, ImU32 col, float radius) {
    dl->AddRectFilled(a, b, col, radius);
}
void RectStrokeRounded(ImDrawList* dl, ImVec2 a, ImVec2 b, ImU32 col, float radius, float thickness) {
    dl->AddRect(a, b, col, radius, 0, thickness);
}

void DropShadow(ImDrawList* dl, ImVec2 a, ImVec2 b, float radius, ImU32 base_col, int layers) {
    int baseAlpha = (int)((base_col >> 24) & 0xFF);
    if (baseAlpha == 0) baseAlpha = 80;
    int N = 12;
    (void)layers;

    const float maxSpread = 18.f;
    const float downBias  = 6.f;
    const float gainK     = 2.8f;
    const float gainNorm  = 0.55f;

    for (int i = 0; i < N; ++i) {
        float t = (float)(i + 1) / (float)N;
        float grow = maxSpread * t;
        float yOff = downBias  * t;
        float falloff = expf(-gainK * t * t);
        int aa = (int)((float)baseAlpha * falloff * gainNorm);
        if (aa < 1)   continue;
        if (aa > 255) aa = 255;
        ImU32 col = (base_col & 0x00FFFFFF) | ((ImU32)aa << 24);
        ImVec2 sa = ImVec2(a.x - grow,       a.y - grow + yOff);
        ImVec2 sb = ImVec2(b.x + grow,       b.y + grow + yOff);
        dl->AddRectFilled(sa, sb, col, radius + grow);
    }
}

void SoftHaloRect(ImDrawList* dl, ImVec2 a, ImVec2 b, float radius,
                  ImU32 glow_col, float spread, int N ) {
    int baseAlpha = (int)((glow_col >> 24) & 0xFF);
    if (baseAlpha == 0) baseAlpha = 180;
    if (N < 4) N = 4;
    if (spread <= 0.f) spread = 20.f;
    const float gainK    = 2.4f;
    const float gainNorm = 0.42f;
    for (int i = 0; i < N; ++i) {
        float t = (float)(i + 1) / (float)N;
        float grow = spread * t;
        float falloff = expf(-gainK * t * t);
        int aa = (int)((float)baseAlpha * falloff * gainNorm);
        if (aa < 1)   continue;
        if (aa > 255) aa = 255;
        ImU32 col = (glow_col & 0x00FFFFFF) | ((ImU32)aa << 24);
        dl->AddRectFilled(ImVec2(a.x - grow, a.y - grow),
                          ImVec2(b.x + grow, b.y + grow),
                          col, radius + grow);
    }
}

void GlassPanel(ImDrawList* dl, ImVec2 a, ImVec2 b, float radius,
                ImU32 tint_col, ImU32 border_col,
                ImDrawFlags corner_flags ) {
    if (Blur::IsReady()) {
        ImGuiIO& io = ImGui::GetIO();
        ImTextureID tex = Blur::GetSrv();
        ImVec2 uv0 = Blur::ScreenToUV(a, io.DisplaySize);
        ImVec2 uv1 = Blur::ScreenToUV(b, io.DisplaySize);

        dl->AddImageRounded(tex, a, b, uv0, uv1, IM_COL32_WHITE,
                            radius, corner_flags);
    } else {

        dl->AddRectFilled(a, b, IM_COL32(20, 22, 28, 255), radius, corner_flags);
    }

    if (((tint_col >> 24) & 0xFF) > 0)
        dl->AddRectFilled(a, b, tint_col, radius, corner_flags);

    ImU32 hi  = IM_COL32(255, 255, 255, 38);
    ImU32 hi2 = IM_COL32(255, 255, 255, 18);

    dl->AddLine(ImVec2(a.x + radius, a.y + 0.5f),
                ImVec2(b.x - radius, a.y + 0.5f), hi,  1.f);

    dl->AddLine(ImVec2(a.x + 0.5f, a.y + radius),
                ImVec2(a.x + 0.5f, b.y - radius), hi2, 1.f);

    ImU32 lo  = IM_COL32(0, 0, 0, 60);
    ImU32 lo2 = IM_COL32(0, 0, 0, 40);
    dl->AddLine(ImVec2(a.x + radius, b.y - 0.5f),
                ImVec2(b.x - radius, b.y - 0.5f), lo,  1.f);
    dl->AddLine(ImVec2(b.x - 0.5f, a.y + radius),
                ImVec2(b.x - 0.5f, b.y - radius), lo2, 1.f);

    if (((border_col >> 24) & 0xFF) > 0)
        dl->AddRect(a, b, border_col, radius, corner_flags, 1.f);
}

void SoftHaloCircle(ImDrawList* dl, ImVec2 center, float inner_r, float outer_r,
                    ImU32 glow_col, int N ) {
    int baseAlpha = (int)((glow_col >> 24) & 0xFF);
    if (baseAlpha == 0) baseAlpha = 220;
    if (N < 6) N = 6;
    if (outer_r <= inner_r) outer_r = inner_r + 6.f;
    const float gainK    = 2.6f;
    const float gainNorm = 0.55f;

    for (int i = N; i >= 1; --i) {
        float t = (float)i / (float)N;
        float r = inner_r + (outer_r - inner_r) * t;
        float falloff = expf(-gainK * t * t);
        int aa = (int)((float)baseAlpha * falloff * gainNorm);
        if (aa < 1)   continue;
        if (aa > 255) aa = 255;
        ImU32 col = (glow_col & 0x00FFFFFF) | ((ImU32)aa << 24);
        dl->AddCircleFilled(center, r, col, 48);
    }
}

void TextL(ImDrawList* dl, ImFont* font, ImVec2 pos, ImU32 col, const char* text) {
    if (!font || !text) return;
    dl->AddText(font, font->FontSize, pos, col, text);
}

ImVec2 TextSize(ImFont* font, const char* text) {
    if (!font || !text) return ImVec2(0,0);
    return font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.f, text);
}

void IconVec(ImDrawList* dl, ImVec2 c, float size, ImU32 col, int icon_id) {
    float s = size * 0.5f;
    float t = 1.5f;
    auto L = [&](ImVec2 a, ImVec2 b) { dl->AddLine(a, b, col, t); };
    auto R = [&](ImVec2 a, ImVec2 b) { dl->AddRect(a, b, col, 1.5f, 0, t); };
    auto C = [&](ImVec2 cc, float r) { dl->AddCircle(cc, r, col, 12, t); };

    using namespace Icons;
    switch (icon_id) {
    case Dashboard: {
        R(ImVec2(c.x - s, c.y - s), ImVec2(c.x - s*0.15f, c.y - s*0.15f));
        R(ImVec2(c.x + s*0.15f, c.y - s), ImVec2(c.x + s, c.y - s*0.15f));
        R(ImVec2(c.x - s, c.y + s*0.15f), ImVec2(c.x - s*0.15f, c.y + s));
        R(ImVec2(c.x + s*0.15f, c.y + s*0.15f), ImVec2(c.x + s, c.y + s));
    } break;
    case Shield: {
        dl->PathLineTo(ImVec2(c.x, c.y - s));
        dl->PathLineTo(ImVec2(c.x - s*0.85f, c.y - s*0.5f));
        dl->PathLineTo(ImVec2(c.x - s*0.85f, c.y + s*0.2f));
        dl->PathLineTo(ImVec2(c.x, c.y + s));
        dl->PathLineTo(ImVec2(c.x + s*0.85f, c.y + s*0.2f));
        dl->PathLineTo(ImVec2(c.x + s*0.85f, c.y - s*0.5f));
        dl->PathLineTo(ImVec2(c.x, c.y - s));
        dl->PathStroke(col, 0, t);
    } break;
    case Arrow: {
        L(ImVec2(c.x - s*0.8f, c.y), ImVec2(c.x, c.y - s));
        L(ImVec2(c.x, c.y - s), ImVec2(c.x + s*0.8f, c.y));
        L(ImVec2(c.x, c.y - s), ImVec2(c.x, c.y + s));
    } break;
    case Eye: {
        dl->PathArcTo(ImVec2(c.x, c.y + s*1.2f), s*1.5f, 4.3f, 5.1f);
        dl->PathStroke(col, 0, t);
        dl->PathArcTo(ImVec2(c.x, c.y - s*1.2f), s*1.5f, 1.2f, 1.95f);
        dl->PathStroke(col, 0, t);
        C(c, s*0.35f);
    } break;
    case Wifi: {
        C(c, s*0.8f);
        L(ImVec2(c.x - s, c.y), ImVec2(c.x + s, c.y));
        dl->AddBezierQuadratic(ImVec2(c.x - s*0.4f, c.y - s), c, ImVec2(c.x + s*0.4f, c.y - s), col, t);
        dl->AddBezierQuadratic(ImVec2(c.x - s*0.4f, c.y + s), c, ImVec2(c.x + s*0.4f, c.y + s), col, t);
    } break;
    case Dots: {
        C(ImVec2(c.x - s*0.55f, c.y - s*0.55f), 1.6f);
        C(ImVec2(c.x + s*0.55f, c.y - s*0.55f), 1.6f);
        C(ImVec2(c.x - s*0.55f, c.y + s*0.55f), 1.6f);
        C(ImVec2(c.x + s*0.55f, c.y + s*0.55f), 1.6f);
    } break;
    case Pickaxe: {
        L(ImVec2(c.x - s, c.y + s), ImVec2(c.x, c.y - s));
        L(ImVec2(c.x, c.y - s), ImVec2(c.x + s, c.y + s));
        L(ImVec2(c.x - s, c.y + s), ImVec2(c.x + s, c.y + s));
    } break;
    case Fish: {
        dl->AddBezierQuadratic(ImVec2(c.x - s, c.y), ImVec2(c.x - s*0.3f, c.y - s*0.6f), ImVec2(c.x + s*0.3f, c.y), col, t);
        dl->AddBezierQuadratic(ImVec2(c.x + s*0.3f, c.y), ImVec2(c.x - s*0.3f, c.y + s*0.6f), ImVec2(c.x - s, c.y), col, t);
        L(ImVec2(c.x + s, c.y - s*0.5f), ImVec2(c.x + s*0.4f, c.y));
        L(ImVec2(c.x + s*0.4f, c.y), ImVec2(c.x + s, c.y + s*0.5f));
    } break;
    case Flame: {
        L(ImVec2(c.x - s, c.y + s), ImVec2(c.x, c.y - s));
        L(ImVec2(c.x, c.y - s), ImVec2(c.x + s, c.y + s));
        L(ImVec2(c.x - s*0.5f, c.y), ImVec2(c.x + s*0.5f, c.y));
    } break;
    case Settings: {
        C(c, s*0.35f);
        for (int i = 0; i < 8; ++i) {
            float a = i * 6.28318f / 8.f;
            ImVec2 p0 = ImVec2(c.x + std::cos(a)*s*0.55f, c.y + std::sin(a)*s*0.55f);
            ImVec2 p1 = ImVec2(c.x + std::cos(a)*s, c.y + std::sin(a)*s);
            L(p0, p1);
        }
    } break;
    case SearchGlass: {
        C(ImVec2(c.x - s*0.15f, c.y - s*0.15f), s*0.55f);
        L(ImVec2(c.x + s*0.35f, c.y + s*0.35f), ImVec2(c.x + s*0.9f, c.y + s*0.9f));
    } break;
    case Chevron: Prim::DrawChevron(dl, c, size*0.9f, col); break;
    case Check:   Prim::DrawCheckmark(dl, c, size*0.9f, col); break;
    case X: {
        L(ImVec2(c.x - s*0.7f, c.y - s*0.7f), ImVec2(c.x + s*0.7f, c.y + s*0.7f));
        L(ImVec2(c.x + s*0.7f, c.y - s*0.7f), ImVec2(c.x - s*0.7f, c.y + s*0.7f));
    } break;
    case Pulse: {
        L(ImVec2(c.x - s, c.y), ImVec2(c.x - s*0.4f, c.y));
        L(ImVec2(c.x - s*0.4f, c.y), ImVec2(c.x - s*0.2f, c.y - s*0.7f));
        L(ImVec2(c.x - s*0.2f, c.y - s*0.7f), ImVec2(c.x + s*0.1f, c.y + s*0.7f));
        L(ImVec2(c.x + s*0.1f, c.y + s*0.7f), ImVec2(c.x + s*0.4f, c.y));
        L(ImVec2(c.x + s*0.4f, c.y), ImVec2(c.x + s, c.y));
    } break;
    case Spark: {
        L(ImVec2(c.x, c.y - s), ImVec2(c.x, c.y + s));
        L(ImVec2(c.x - s, c.y), ImVec2(c.x + s, c.y));
        L(ImVec2(c.x - s*0.7f, c.y - s*0.7f), ImVec2(c.x + s*0.7f, c.y + s*0.7f));
        L(ImVec2(c.x - s*0.7f, c.y + s*0.7f), ImVec2(c.x + s*0.7f, c.y - s*0.7f));
    } break;
    case Diamond: {
        dl->PathLineTo(ImVec2(c.x, c.y - s));
        dl->PathLineTo(ImVec2(c.x + s, c.y));
        dl->PathLineTo(ImVec2(c.x, c.y + s));
        dl->PathLineTo(ImVec2(c.x - s, c.y));
        dl->PathLineTo(ImVec2(c.x, c.y - s));
        dl->PathStroke(col, 0, t);
    } break;
    case GiftBox: {
        R(ImVec2(c.x - s, c.y - s*0.3f), ImVec2(c.x + s, c.y + s));
        L(ImVec2(c.x - s, c.y + s*0.25f), ImVec2(c.x + s, c.y + s*0.25f));
        L(ImVec2(c.x, c.y - s*0.3f), ImVec2(c.x, c.y + s));
    } break;
    case Treasure: {
        R(ImVec2(c.x - s, c.y - s*0.4f), ImVec2(c.x + s, c.y + s));
        L(ImVec2(c.x - s, c.y + s*0.15f), ImVec2(c.x + s, c.y + s*0.15f));
    } break;
    case FloorItem: {
        R(ImVec2(c.x - s, c.y + s*0.55f), ImVec2(c.x + s, c.y + s));
    } break;
    case Portal: {
        C(c, s*0.85f);
        C(c, s*0.4f);
    } break;
    case Reload: {
        dl->PathArcTo(c, s*0.8f, 0.f, 4.5f);
        dl->PathStroke(col, 0, t);
        L(ImVec2(c.x + s*0.6f, c.y - s*0.4f), ImVec2(c.x + s*0.8f, c.y - s*0.05f));
        L(ImVec2(c.x + s*0.6f, c.y - s*0.4f), ImVec2(c.x + s*0.2f, c.y - s*0.4f));
    } break;
    case LogOut: {
        R(ImVec2(c.x - s*0.9f, c.y - s*0.9f), ImVec2(c.x + s*0.1f, c.y + s*0.9f));
        L(ImVec2(c.x - s*0.2f, c.y), ImVec2(c.x + s*0.9f, c.y));
        L(ImVec2(c.x + s*0.55f, c.y - s*0.35f), ImVec2(c.x + s*0.9f, c.y));
        L(ImVec2(c.x + s*0.55f, c.y + s*0.35f), ImVec2(c.x + s*0.9f, c.y));
    } break;
    case Trash: {
        L(ImVec2(c.x - s*0.85f, c.y - s*0.5f), ImVec2(c.x + s*0.85f, c.y - s*0.5f));
        R(ImVec2(c.x - s*0.65f, c.y - s*0.5f), ImVec2(c.x + s*0.65f, c.y + s*0.85f));
        L(ImVec2(c.x - s*0.25f, c.y - s*0.5f), ImVec2(c.x - s*0.25f, c.y - s*0.8f));
        L(ImVec2(c.x + s*0.25f, c.y - s*0.5f), ImVec2(c.x + s*0.25f, c.y - s*0.8f));
    } break;
    case Database: {
        dl->AddEllipse(ImVec2(c.x, c.y - s*0.6f), ImVec2(s*0.85f, s*0.25f), col, 0.f, 24, t);
        L(ImVec2(c.x - s*0.85f, c.y - s*0.6f), ImVec2(c.x - s*0.85f, c.y + s*0.6f));
        L(ImVec2(c.x + s*0.85f, c.y - s*0.6f), ImVec2(c.x + s*0.85f, c.y + s*0.6f));
        dl->AddEllipse(ImVec2(c.x, c.y + s*0.6f), ImVec2(s*0.85f, s*0.25f), col, 0.f, 24, t);
    } break;
    case Combat: {
        L(ImVec2(c.x - s, c.y + s), ImVec2(c.x + s, c.y - s));
        L(ImVec2(c.x - s*0.4f, c.y - s*0.4f), ImVec2(c.x + s*0.4f, c.y + s*0.4f));
    } break;
    case ArrowIn: {
        L(ImVec2(c.x - s, c.y), ImVec2(c.x + s*0.4f, c.y));
        L(ImVec2(c.x + s*0.4f, c.y), ImVec2(c.x + s*0.0f, c.y - s*0.4f));
        L(ImVec2(c.x + s*0.4f, c.y), ImVec2(c.x + s*0.0f, c.y + s*0.4f));
    } break;
    case ArrowOut: {
        L(ImVec2(c.x + s, c.y), ImVec2(c.x - s*0.4f, c.y));
        L(ImVec2(c.x - s*0.4f, c.y), ImVec2(c.x + s*0.0f, c.y - s*0.4f));
        L(ImVec2(c.x - s*0.4f, c.y), ImVec2(c.x + s*0.0f, c.y + s*0.4f));
    } break;
    case Grid: {
        R(ImVec2(c.x - s, c.y - s), ImVec2(c.x + s, c.y + s));
        L(ImVec2(c.x - s, c.y - s*0.25f), ImVec2(c.x + s, c.y - s*0.25f));
        L(ImVec2(c.x - s*0.4f, c.y - s*0.25f), ImVec2(c.x + s*0.4f, c.y + s));
    } break;
    case Magnet: {
        L(ImVec2(c.x - s, c.y), ImVec2(c.x - s, c.y + s));
        L(ImVec2(c.x + s, c.y), ImVec2(c.x + s, c.y + s));
        dl->PathArcTo(c, s, 3.14f, 6.28f);
        dl->PathStroke(col, 0, t);
    } break;
    case Spring: {
        for (int i = 0; i < 5; ++i) {
            float y = c.y - s + (s*2.f / 5.f) * i;
            dl->AddCircle(ImVec2(c.x, y), s*0.25f, col, 8, t);
        }
    } break;
    case Recipe: {
        L(ImVec2(c.x - s*0.4f, c.y), ImVec2(c.x - s*0.05f, c.y + s*0.45f));
        L(ImVec2(c.x - s*0.05f, c.y + s*0.45f), ImVec2(c.x + s*0.7f, c.y - s*0.4f));
    } break;
    default: {
        C(c, s*0.5f);
    } break;
    }
}

void IconTile(ImDrawList* dl, ImVec2 pos, float size, ImU32 bg, ImU32 fg,
              int icon_id, float radius, int atlas_block_id) {
    ImVec2 a = pos;
    ImVec2 b = ImVec2(pos.x + size, pos.y + size);
    dl->AddRectFilled(a, b, bg, radius);

    if (atlas_block_id > 0) {
        ImVec2 uv0, uv1;
        ImTextureID tex = 0;
        if (SpriteAtlas::LookupUV(atlas_block_id, uv0, uv1, tex) && tex) {
            const float inset = size * 0.12f;
            const ImVec2 ia(a.x + inset, a.y + inset);
            const ImVec2 ib(b.x - inset, b.y - inset);

            dl->AddImage(tex, ia, ib, uv0, uv1, IM_COL32_WHITE);
            return;
        }
    }
    IconVec(dl, ImVec2(pos.x + size*0.5f, pos.y + size*0.5f), size*0.6f, fg, icon_id);
}

void Knob(ImDrawList* dl, ImVec2 c, float r, ImU32 fill) {
    ImU32 sh = IM_COL32(0,0,0, 50);
    dl->AddCircleFilled(ImVec2(c.x, c.y + 1.5f), r, sh, 24);
    dl->AddCircleFilled(c, r, fill, 24);
    dl->AddCircle(c, r, IM_COL32(0,0,0,30), 24, 1.f);
}

bool StyledColorPickerPopup(const char* popup_id, const char* header,
                            ImVec4* color, bool show_alpha) {
    if (!popup_id || !color) return false;

    auto u32_to_v4 = [](ImU32 c) -> ImVec4 {
        return ImVec4(((c >>  0) & 0xFF) / 255.f,
                      ((c >>  8) & 0xFF) / 255.f,
                      ((c >> 16) & 0xFF) / 255.f,
                      ((c >> 24) & 0xFF) / 255.f);
    };
    ImVec4 cBgCard  = u32_to_v4(Tokens::Color::BgCard());
    ImVec4 cLine    = u32_to_v4(Tokens::Color::Line());
    ImVec4 cText    = u32_to_v4(Tokens::Color::Text());
    ImVec4 cTextSec = u32_to_v4(Tokens::Color::TextSec());
    ImVec4 cPanel   = u32_to_v4(Tokens::Color::BgPanelInner());
    ImVec4 cHover   = u32_to_v4(Tokens::Color::BgHover());
    ImVec4 cActive  = u32_to_v4(Tokens::Color::BgPressed());
    ImVec4 cAccent  = u32_to_v4(Tokens::Color::Accent());

    ImGui::PushStyleColor(ImGuiCol_WindowBg,         cBgCard);
    ImGui::PushStyleColor(ImGuiCol_PopupBg,          cBgCard);
    ImGui::PushStyleColor(ImGuiCol_Border,           cLine);
    ImGui::PushStyleColor(ImGuiCol_Text,             cText);
    ImGui::PushStyleColor(ImGuiCol_TextDisabled,     cTextSec);
    ImGui::PushStyleColor(ImGuiCol_FrameBg,          cPanel);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,   cHover);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,    cActive);
    ImGui::PushStyleColor(ImGuiCol_Button,           cPanel);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,    cHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,     cActive);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,       cAccent);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, cAccent);
    ImGui::PushStyleColor(ImGuiCol_Separator,        cLine);
    ImGui::PushStyleColor(ImGuiCol_CheckMark,        cAccent);
    ImGui::PushStyleColor(ImGuiCol_Header,           cHover);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,    cHover);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,     cActive);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,  10.f);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding,   10.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,    6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding,     6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.f, 12.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(8.f, 8.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

    bool changed = false;
    if (ImGui::BeginPopup(popup_id)) {
        ImFont* fb = Tokens::Type::Body();
        if (fb) ImGui::PushFont(fb);
        if (header && *header) {
            ImGui::TextUnformatted(header);
            ImGui::Separator();
        }
        ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoSidePreview
                                  | ImGuiColorEditFlags_NoSmallPreview
                                  | ImGuiColorEditFlags_DisplayHex
                                  | ImGuiColorEditFlags_PickerHueBar;
        if (!show_alpha) flags |= ImGuiColorEditFlags_NoAlpha;
        if (ImGui::ColorPicker4("##cp", (float*)color, flags)) changed = true;
        if (ImGui::Button("Done", ImVec2(-1, 0))) ImGui::CloseCurrentPopup();
        if (fb) ImGui::PopFont();
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar(7);
    ImGui::PopStyleColor(18);
    return changed;
}

}

namespace Layout {

void AdvanceY(float h) { GetCtx().cursor.y += h; }
void AdvanceCursor(float w, float h) {
    Ctx& c = GetCtx();
    c.cursor.x += w;
    c.cursor.y += h;
}
ImVec2 Cursor() {
    Ctx& c = GetCtx();
    return ImVec2(c.content_x0, ContentToScreenY(c.cursor.y));
}
float ColumnWidth() {
    Ctx& c = GetCtx();
    return c.content_x1 - c.content_x0;
}

static struct ColStack { float a, b; } g_stack[8];
static int g_stack_n = 0;
void PushColumn(float x0, float x1) {
    Ctx& c = GetCtx();
    if (g_stack_n < 8) { g_stack[g_stack_n++] = { c.content_x0, c.content_x1 }; }
    c.content_x0 = x0; c.content_x1 = x1;
}
void PopColumn() {
    Ctx& c = GetCtx();
    if (g_stack_n > 0) { --g_stack_n; c.content_x0 = g_stack[g_stack_n].a; c.content_x1 = g_stack[g_stack_n].b; }
}

Interact RegisterHit(const char* id_str, ImVec2 a, ImVec2 b) {
    ImGui::SetCursorScreenPos(a);
    ImGui::InvisibleButton(id_str, ImVec2(b.x - a.x, b.y - a.y));
    Interact r;
    r.hovered  = ImGui::IsItemHovered();
    r.clicked  = ImGui::IsItemClicked();
    r.held     = ImGui::IsItemActive();
    r.pressed  = ImGui::IsItemActivated();

    ImVec2 mid((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
    if (ZmodUI::ComboPopupBlocksPoint(mid)) {
        r.hovered = r.clicked = r.held = r.pressed = false;
    }
    return r;
}

}

void BeginGroup() {
    Ctx& c = GetCtx();
    c.in_group = true;
    c.group_start = ImVec2(c.content_x0, Layout::ContentToScreenY(c.cursor.y));
    c.group_row_count = 0;
    c.group_hover_active = false;
    c.group_hover_row_idx = -1;

    c.dl->ChannelsSplit(2);
    c.dl->ChannelsSetCurrent(1);
}

void EndGroup() {
    Ctx& c = GetCtx();
    if (!c.in_group) return;
    ImVec2 a = c.group_start;
    ImVec2 b = ImVec2(c.content_x1, Layout::ContentToScreenY(c.cursor.y));
    float radius = Tokens::Size::CardRadius;
    c.dl->ChannelsSetCurrent(0);
    Draw::RectFilledRounded(c.dl, a, b, Tokens::Color::BgCard(), radius);

    if (c.group_hover_active && c.group_row_count > 0) {
        ImDrawFlags flags = 0;
        if (c.group_hover_row_idx == 0) flags |= ImDrawFlags_RoundCornersTop;
        if (c.group_hover_row_idx == c.group_row_count - 1) flags |= ImDrawFlags_RoundCornersBottom;
        if (flags == 0) flags = ImDrawFlags_RoundCornersNone;
        c.dl->AddRectFilled(c.group_hover_a, c.group_hover_b,
                            Tokens::Color::BgHover(), radius, flags);
    }

    Draw::RectStrokeRounded(c.dl, a, b, Tokens::Color::Line(), radius, 1.f);
    c.dl->ChannelsMerge();
    c.in_group = false;
    c.group_hover_active = false;
    c.cursor.y += Tokens::Size::GroupMarginB;
}

void GroupTitle(const char* text) {
    Ctx& c = GetCtx();
    ImFont* f = Tokens::Type::H3();
    float h = f ? f->FontSize : 15.f;
    ImVec2 p = ImVec2(c.content_x0 + 4.f, Layout::ContentToScreenY(c.cursor.y));
    Draw::TextL(c.dl, f, p, Tokens::Color::Text(), text);
    c.cursor.y += h + 8.f;
}

void SubLabel(const char* text) {
    Ctx& c = GetCtx();
    ImFont* f = Tokens::Type::Eyebrow();
    float h = f ? f->FontSize : 11.f;
    ImVec2 p = ImVec2(c.content_x0 + 14.f, Layout::ContentToScreenY(c.cursor.y) + 10.f);

    char up[256]; int n = 0;
    for (const char* s = text; *s && n < 255; ++s, ++n) {
        char ch = *s;
        if (ch >= 'a' && ch <= 'z') ch -= 32;
        up[n] = ch;
    }
    up[n] = 0;
    Draw::TextL(c.dl, f, p, Tokens::Color::TextTert(), up);
    c.cursor.y += h + 16.f;
}

void PageHead(const char* title, const char* subtitle) {
    Ctx& c = GetCtx();
    ImFont* f1 = Tokens::Type::H1();
    ImFont* f2 = Tokens::Type::BodySec();
    float y = Layout::ContentToScreenY(c.cursor.y);
    Draw::TextL(c.dl, f1, ImVec2(c.content_x0, y), Tokens::Color::Text(), title);
    float advance = (f1 ? f1->FontSize : 28.f) + 4.f;
    if (subtitle && *subtitle) {
        Draw::TextL(c.dl, f2, ImVec2(c.content_x0, y + advance), Tokens::Color::TextSec(), subtitle);
        advance += (f2 ? f2->FontSize : 13.f);
    }
    c.cursor.y += advance + 24.f;
}

void Separator() {
    Ctx& c = GetCtx();
    float y = Layout::ContentToScreenY(c.cursor.y);
    c.dl->AddLine(ImVec2(c.content_x0, y), ImVec2(c.content_x1, y), Tokens::Color::LineSoft(), 1.f);
    c.cursor.y += 1.f;
}

void Spacing(float px) {
    GetCtx().cursor.y += px;
}

void Pill(const char* text, int tone) {
    Ctx& c = GetCtx();
    ImFont* f = Tokens::Type::Eyebrow();
    if (!f) f = Tokens::Type::BodySec();
    ImVec2 ts = Draw::TextSize(f, text);
    float padH = 10.f, padV = 3.f;
    float w = ts.x + padH*2;
    float h = ts.y + padV*2;
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 a = ImVec2(c.content_x0, y);
    ImVec2 b = ImVec2(a.x + w, a.y + h);
    ImU32 bg, fg;
    switch (tone) {
        case 0: bg = IM_COL32(52,199,89,38); fg = IM_COL32(31,138,58,255); break;
        case 1: bg = IM_COL32(255,149,0,38); fg = IM_COL32(176,96,0,255); break;
        case 2: bg = IM_COL32(255,59,48,38); fg = IM_COL32(196,39,30,255); break;
        default: bg = Tokens::Color::BgHover(); fg = Tokens::Color::TextSec(); break;
    }
    if (Theme::IsDarkMode()) {
        switch (tone) {
            case 0: fg = IM_COL32(95,213,122,255); break;
            case 1: fg = IM_COL32(255,174,60,255); break;
            case 2: fg = IM_COL32(255,105,97,255); break;
        }
    }
    Draw::RectFilledRounded(c.dl, a, b, bg, 999.f);
    Draw::TextL(c.dl, f, ImVec2(a.x + padH, a.y + padV), fg, text);
}

void StatusPip(int tone) {
    Ctx& c = GetCtx();
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 cc = ImVec2(c.content_x0 + 4, y + 4);
    ImU32 col = Tokens::Color::Green();
    if (tone == 1) col = Tokens::Color::Orange();
    else if (tone == 2) col = Tokens::Color::Red();

    c.dl->AddCircleFilled(cc, 8.f, (col & 0x00FFFFFF) | (60 << 24), 16);
    c.dl->AddCircleFilled(cc, 3.5f, col, 16);
}

void StatLabel(const char* k, const char* v) {
    Ctx& c = GetCtx();
    ImFont* fb = Tokens::Type::Body();
    float row_h = Tokens::Size::RowMinH;
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 a = ImVec2(c.content_x0, y);
    ImVec2 b = ImVec2(c.content_x1, y + row_h);

    if (c.in_group && c.group_row_count > 0) {
        c.dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, a.y), Tokens::Color::LineSoft(), 1.f);
    }
    Draw::TextL(c.dl, fb, ImVec2(a.x + Tokens::Size::RowPadH, a.y + (row_h - (fb ? fb->FontSize : 13))/2 ), Tokens::Color::Text(), k);
    ImVec2 vs = Draw::TextSize(fb, v);
    Draw::TextL(c.dl, fb, ImVec2(b.x - Tokens::Size::RowPadH - vs.x, a.y + (row_h - (fb ? fb->FontSize : 13))/2 ), Tokens::Color::TextSec(), v);
    if (c.in_group) c.group_row_count++;
    c.cursor.y += row_h;
}

void QuadCard(const char* eyebrow, const char* value, const char* trend, int trend_tone) {

    Ctx& c = GetCtx();
    float y = Layout::ContentToScreenY(c.cursor.y);
    float h = 70.f;
    ImVec2 a = ImVec2(c.content_x0, y);
    ImVec2 b = ImVec2(c.content_x1, y + h);
    Draw::RectFilledRounded(c.dl, a, b, Tokens::Color::BgCard(), Tokens::Size::CardRadius);
    Draw::RectStrokeRounded(c.dl, a, b, Tokens::Color::Line(), Tokens::Size::CardRadius, 1.f);
    ImFont* eb = Tokens::Type::Eyebrow();
    ImFont* h2 = Tokens::Type::H2();
    ImFont* bs = Tokens::Type::BodySec();
    Draw::TextL(c.dl, eb, ImVec2(a.x + 16, a.y + 14), Tokens::Color::TextSec(), eyebrow);
    Draw::TextL(c.dl, h2, ImVec2(a.x + 16, a.y + 26), Tokens::Color::Text(), value);
    ImU32 tcol = Tokens::Color::Green();
    if (trend_tone == 1) tcol = Tokens::Color::Orange();
    else if (trend_tone == 2) tcol = Tokens::Color::Red();
    Draw::TextL(c.dl, bs, ImVec2(a.x + 16, a.y + 52), tcol, trend ? trend : "");
    c.cursor.y += h + 10.f;
}

void HelpText(const char* text) {
    Ctx& c = GetCtx();
    ImFont* f = Tokens::Type::BodySec();
    Draw::TextL(c.dl, f, ImVec2(c.content_x0 + 4, Layout::ContentToScreenY(c.cursor.y)), Tokens::Color::TextTert(), text);
    c.cursor.y += (f ? f->FontSize : 12) + 16.f;
}

}

#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"

namespace ZmodUI {

enum BtnKind { BK_Default = 0, BK_Primary = 1, BK_Destructive = 2 };

static bool DrawBtn(ImVec2 pos, float w, float h, const char* label, BtnKind kind, ImGuiID seed) {
    Ctx& c = GetCtx();
    ImVec2 a = pos;
    ImVec2 b = ImVec2(pos.x + w, pos.y + h);
    char id_buf[40]; ImFormatString(id_buf, sizeof(id_buf), "btn##%u_%s", (unsigned)seed, label ? label : "");
    Layout::Interact it = Layout::RegisterHit(id_buf, a, b);
    ImU32 bg, fg, border;
    switch (kind) {
        case BK_Primary: {
            bg = Tokens::Color::Accent();
            fg = IM_COL32_WHITE;
            border = 0;
            auto tweak = [](ImU32 c, int mul) {
                int r = (int)((c >> 0)  & 0xFF) * mul / 100;
                int g = (int)((c >> 8)  & 0xFF) * mul / 100;
                int b = (int)((c >> 16) & 0xFF) * mul / 100;
                if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
                return IM_COL32(r, g, b, 255);
            };
            if (it.held)         bg = tweak(bg, 92);
            else if (it.hovered) bg = tweak(bg, 106);
        } break;
        case BK_Destructive:
            bg = it.held ? IM_COL32(255,59,48,40) : (it.hovered ? IM_COL32(255,59,48,20) : Tokens::Color::BgCard());
            fg = Tokens::Color::Red();
            border = Tokens::Color::Line();
            break;
        default:
            bg = it.held ? Tokens::Color::BgPressed() : (it.hovered ? Tokens::Color::BgHover() : Tokens::Color::BgCard());
            fg = Tokens::Color::Text();
            border = Tokens::Color::Line();
            break;
    }
    Draw::RectFilledRounded(c.dl, a, b, bg, 6.f);
    if (border) Draw::RectStrokeRounded(c.dl, a, b, border, 6.f, 1.f);
    ImFont* f = Tokens::Type::Body();
    ImVec2 ts = Draw::TextSize(f, label);
    Draw::TextL(c.dl, f, ImVec2(a.x + (w - ts.x)*0.5f, a.y + (h - ts.y)*0.5f), fg, label);
    return it.clicked;
}

static bool BtnRow(const char* label, BtnKind kind) {
    Ctx& c = GetCtx();
    ImFont* f = Tokens::Type::Body();
    ImVec2 ts = Draw::TextSize(f, label);
    float w = ts.x + 24.f;
    float h = (f ? f->FontSize : 13) + 10.f;
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 p = ImVec2(c.content_x0, y);
    bool clicked = DrawBtn(p, w, h, label, kind, ImGui::GetID(label));
    c.cursor.y += h + 6.f;
    return clicked;
}

bool Button(const char* label)            { return BtnRow(label, BK_Default); }
bool PrimaryButton(const char* label)     { return BtnRow(label, BK_Primary); }
bool DestructiveButton(const char* label) { return BtnRow(label, BK_Destructive); }

bool ButtonFullWidth(const char* label, float height_px) {
    Ctx& c = GetCtx();
    float y = Layout::ContentToScreenY(c.cursor.y);
    float w = c.content_x1 - c.content_x0 - 28.f;
    ImVec2 p = ImVec2(c.content_x0 + 14.f, y);
    bool clicked = DrawBtn(p, w, height_px, label, BK_Primary, ImGui::GetID(label));
    c.cursor.y += height_px + 14.f;
    return clicked;
}

bool DrawerRow(const char* icon, const char* label, const char* desc, const char* drawer_key) {
    WRow::Opts o;
    o.icon_id = icon;
    o.label = label;
    o.desc = desc;
    o.no_icon = (icon == nullptr);
    o.no_hover = false;
    o.is_drawer = true;
    WRow::RowResult r = WRow::Begin(o);
    if (r.clicked) {
        OpenDrawer(drawer_key);
        return true;
    }
    return false;
}

bool ButtonInRow(const WRow::RowResult& r, const char* label, BtnKind kind) {
    ImFont* f = Tokens::Type::Body();
    ImVec2 ts = Draw::TextSize(f, label);
    float w = ts.x + 24.f;
    float h = (f ? f->FontSize : 13) + 10.f;
    ImVec2 p = ImVec2(r.end_b.x - w, (r.a.y + r.b.y)*0.5f - h*0.5f);
    return DrawBtn(p, w, h, label, kind, ImGui::GetID(label));
}

}

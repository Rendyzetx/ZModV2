#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../Icons.h"
#include "../IconAtlasMap.h"
#include "../Tween.h"
#include <cstdio>
#include <cstring>
#include <string_view>

namespace ZmodUI {
namespace WRow {

IconTone ResolveIcon(const char* icon_id) {
    IconTone r;
    r.bg = Tokens::Color::Accent();
    r.fg = IM_COL32_WHITE;
    r.icon_id = Icons::None;
    r.atlas_block_id = 0;
    if (!icon_id || !*icon_id) return r;

    r.atlas_block_id = IconAtlasMap::Lookup(std::string_view(icon_id));

    char name[32] = {};
    char tone[16] = {};
    const char* dash = std::strchr(icon_id, '-');
    if (dash) {
        size_t nlen = (size_t)(dash - icon_id);
        if (nlen >= sizeof(name)) nlen = sizeof(name)-1;
        std::memcpy(name, icon_id, nlen);
        std::strncpy(tone, dash+1, sizeof(tone)-1);
    } else {
        std::strncpy(name, icon_id, sizeof(name)-1);
    }

    if      (!std::strcmp(name, "dashboard"))   r.icon_id = Icons::Dashboard;
    else if (!std::strcmp(name, "shield"))      r.icon_id = Icons::Shield;
    else if (!std::strcmp(name, "arrow"))       r.icon_id = Icons::Arrow;
    else if (!std::strcmp(name, "eye"))         r.icon_id = Icons::Eye;
    else if (!std::strcmp(name, "wifi"))        r.icon_id = Icons::Wifi;
    else if (!std::strcmp(name, "dots"))        r.icon_id = Icons::Dots;
    else if (!std::strcmp(name, "pickaxe"))     r.icon_id = Icons::Pickaxe;
    else if (!std::strcmp(name, "fish"))        r.icon_id = Icons::Fish;
    else if (!std::strcmp(name, "flame"))       r.icon_id = Icons::Flame;
    else if (!std::strcmp(name, "settings"))    r.icon_id = Icons::Settings;
    else if (!std::strcmp(name, "search"))      r.icon_id = Icons::SearchGlass;
    else if (!std::strcmp(name, "check"))       r.icon_id = Icons::Check;
    else if (!std::strcmp(name, "x"))           r.icon_id = Icons::X;
    else if (!std::strcmp(name, "pulse"))       r.icon_id = Icons::Pulse;
    else if (!std::strcmp(name, "spark"))       r.icon_id = Icons::Spark;
    else if (!std::strcmp(name, "diamond"))     r.icon_id = Icons::Diamond;
    else if (!std::strcmp(name, "gift"))        r.icon_id = Icons::GiftBox;
    else if (!std::strcmp(name, "treasure"))    r.icon_id = Icons::Treasure;
    else if (!std::strcmp(name, "floor"))       r.icon_id = Icons::FloorItem;
    else if (!std::strcmp(name, "portal"))      r.icon_id = Icons::Portal;
    else if (!std::strcmp(name, "reload"))      r.icon_id = Icons::Reload;
    else if (!std::strcmp(name, "logout"))      r.icon_id = Icons::LogOut;
    else if (!std::strcmp(name, "trash"))       r.icon_id = Icons::Trash;
    else if (!std::strcmp(name, "database"))    r.icon_id = Icons::Database;
    else if (!std::strcmp(name, "combat"))      r.icon_id = Icons::Combat;
    else if (!std::strcmp(name, "in"))          r.icon_id = Icons::ArrowIn;
    else if (!std::strcmp(name, "out"))         r.icon_id = Icons::ArrowOut;
    else if (!std::strcmp(name, "grid"))        r.icon_id = Icons::Grid;
    else if (!std::strcmp(name, "magnet"))      r.icon_id = Icons::Magnet;
    else if (!std::strcmp(name, "spring"))      r.icon_id = Icons::Spring;
    else if (!std::strcmp(name, "recipe"))      r.icon_id = Icons::Recipe;
    else if (!std::strcmp(name, "chevron"))     r.icon_id = Icons::Chevron;
    else                                        r.icon_id = Icons::None;

    if (!std::strcmp(tone, "red"))         r.bg = Tokens::Color::Red();
    else if (!std::strcmp(tone, "green"))  r.bg = Tokens::Color::Green();
    else if (!std::strcmp(tone, "orange")) r.bg = Tokens::Color::Orange();
    else if (!std::strcmp(tone, "gray"))   r.bg = IM_COL32(142,142,147,255);
    else if (!std::strcmp(tone, "purple")) r.bg = IM_COL32(175,82,222,255);
    else if (!std::strcmp(tone, "pink"))   r.bg = IM_COL32(255,45,146,255);
    else if (!std::strcmp(tone, "teal"))   r.bg = IM_COL32(90,200,250,255);
    else if (!std::strcmp(tone, "indigo")) r.bg = IM_COL32(94,92,230,255);
    else if (!std::strcmp(tone, "yellow")) { r.bg = Tokens::Color::Yellow(); r.fg = IM_COL32(29,29,31,255); }
    else if (!std::strcmp(tone, "brown"))  r.bg = IM_COL32(162,132,94,255);
    else if (!std::strcmp(tone, "blank"))  { r.bg = 0; r.fg = 0; }
    return r;
}

RowResult Begin(const Opts& o) {
    Ctx& c = GetCtx();
    float row_h = o.row_h > 0 ? o.row_h : Tokens::Size::RowMinH;

    if (o.desc && o.desc[0]) {
        ImFont* fl = Tokens::Type::Body();
        ImFont* fd = Tokens::Type::BodySec();
        float text_h = (fl ? fl->FontSize : 13) + (fd ? fd->FontSize : 11.5f) + 6.f;
        float min_h = text_h + Tokens::Size::RowPadV*2;
        if (min_h > row_h) row_h = min_h;
    }
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 a = ImVec2(c.content_x0, y);
    ImVec2 b = ImVec2(c.content_x1, y + row_h);

    if (c.in_group && c.group_row_count > 0) {
        c.dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, a.y), Tokens::Color::LineSoft(), 1.f);
    }

    RowResult r;
    r.a = a; r.b = b;
    r.hovered = false; r.clicked = false;

    if (!o.no_hover || o.is_drawer) {
        Layout::Interact it = Layout::RegisterHit(o.label ? o.label : "row", a, b);
        r.hovered = it.hovered;
        r.clicked = it.clicked;
    }

    if (r.hovered) {
        if (c.in_group) {
            c.group_hover_active = true;
            c.group_hover_a = a;
            c.group_hover_b = b;
            c.group_hover_row_idx = c.group_row_count;
        } else {
            Draw::RectFilledRounded(c.dl, a, b, Tokens::Color::BgHover(), 0.f);
        }
    }

    if (c.in_group) c.group_row_count++;

    float text_left = a.x + Tokens::Size::RowPadH;
    if (!o.no_icon) {
        IconTone tone = ResolveIcon(o.icon_id);
        float sz = Tokens::Size::IconTileSize;
        ImVec2 tp = ImVec2(a.x + Tokens::Size::RowPadH, a.y + (row_h - sz) * 0.5f);
        if ((tone.bg & 0xFF000000) != 0) {
            Draw::IconTile(c.dl, tp, sz, tone.bg, tone.fg, tone.icon_id, 6.f, tone.atlas_block_id);
        }
        text_left = tp.x + sz + 12.f;
    }

    ImFont* fl = Tokens::Type::Body();
    ImFont* fd = Tokens::Type::BodySec();
    float text_y;
    if (o.desc && o.desc[0]) {
        text_y = a.y + (row_h - ((fl?fl->FontSize:13)+(fd?fd->FontSize:11.5f)+4.f)) * 0.5f;
        Draw::TextL(c.dl, fl, ImVec2(text_left, text_y), Tokens::Color::Text(), o.label ? o.label : "");
        Draw::TextL(c.dl, fd, ImVec2(text_left, text_y + (fl?fl->FontSize:13) + 2.f), Tokens::Color::TextSec(), o.desc);
    } else {
        text_y = a.y + (row_h - (fl?fl->FontSize:13)) * 0.5f;
        Draw::TextL(c.dl, fl, ImVec2(text_left, text_y), Tokens::Color::Text(), o.label ? o.label : "");
    }

    float end_pad = Tokens::Size::RowPadH;
    r.end_b = ImVec2(b.x - end_pad, a.y + row_h * 0.5f + Tokens::Size::SwitchH*0.5f);
    r.end_a = ImVec2(b.x - end_pad - Tokens::Size::SwitchW, a.y + row_h*0.5f - Tokens::Size::SwitchH*0.5f);

    if (o.is_drawer) {
        ImVec2 chev_c = ImVec2(b.x - end_pad - 8.f, a.y + row_h*0.5f);
        Prim::DrawChevron(c.dl, chev_c, 12.f, Tokens::Color::TextTert());
        r.end_a.x -= 24.f;
        r.end_b.x -= 24.f;
    }

    c.cursor.y += row_h;
    return r;
}

ImVec2 EndSlot(const RowResult& r, float width, float height) {
    ImVec2 cen = ImVec2((r.end_a.x + r.end_b.x) * 0.5f, (r.end_a.y + r.end_b.y) * 0.5f);
    return ImVec2(r.end_b.x - width, cen.y - height*0.5f);
}

}
}

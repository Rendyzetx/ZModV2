#include "../Internal.h"
#include "../Theme.h"
#include "../Tokens.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include "../widgets/Row.h"
#include "../../core/ProxyPool.h"
#include <vector>
#include <mutex>

namespace ZmodUI {

static bool SegmentRow(const char* idp, const char* label, int* mode) {
    Ctx& c = GetCtx();
    float row_h = Tokens::Size::RowMinH;
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 a = ImVec2(c.content_x0, y);
    ImVec2 b = ImVec2(c.content_x1, y + row_h);
    if (c.in_group && c.group_row_count > 0)
        c.dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, a.y), Tokens::Color::LineSoft(), 1.f);

    ImFont* fb = Tokens::Type::Body();
    float fbh = fb ? fb->FontSize : 13.f;
    Draw::TextL(c.dl, fb, ImVec2(a.x + Tokens::Size::RowPadH, a.y + (row_h - fbh) * 0.5f),
                Tokens::Color::Text(), label);

    const char* opts[2] = { "Static", "Rotating" };
    float segH = 24.f, segW = 78.f, totalW = segW * 2;
    float x0 = b.x - Tokens::Size::RowPadH - totalW;
    float sy = a.y + (row_h - segH) * 0.5f;
    ImVec2 ca = ImVec2(x0, sy), cb = ImVec2(x0 + totalW, sy + segH);
    Draw::RectFilledRounded(c.dl, ca, cb, Tokens::Color::BgCard(), 6.f);
    Draw::RectStrokeRounded(c.dl, ca, cb, Tokens::Color::Line(), 6.f, 1.f);

    bool changed = false;
    ImFont* fs = Tokens::Type::BodySec();
    for (int i = 0; i < 2; ++i) {
        ImVec2 sa = ImVec2(x0 + i * segW, sy);
        ImVec2 sb = ImVec2(x0 + (i + 1) * segW, sy + segH);
        char idb[40]; ImFormatString(idb, sizeof(idb), "%s_seg%d", idp, i);
        Layout::Interact it = Layout::RegisterHit(idb, sa, sb);
        bool active = (*mode == i);
        if (active)          Draw::RectFilledRounded(c.dl, sa, sb, Tokens::Color::Accent(), 6.f);
        else if (it.hovered) Draw::RectFilledRounded(c.dl, sa, sb, Tokens::Color::BgHover(), 6.f);
        ImVec2 ts = Draw::TextSize(fs, opts[i]);
        ImU32 tc = active ? IM_COL32_WHITE : Tokens::Color::TextSec();
        Draw::TextL(c.dl, fs, ImVec2(sa.x + (segW - ts.x) * 0.5f, sa.y + (segH - ts.y) * 0.5f), tc, opts[i]);
        if (it.clicked && *mode != i) { *mode = i; changed = true; }
    }
    if (c.in_group) c.group_row_count++;
    c.cursor.y += row_h;
    return changed;
}

static bool ProxyEntryRow(const char* idp, int idx, const ProxyPool::Entry& e) {
    Ctx& c = GetCtx();
    float row_h = 32.f;
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 a = ImVec2(c.content_x0, y);
    ImVec2 b = ImVec2(c.content_x1, y + row_h);
    if (c.in_group && c.group_row_count > 0)
        c.dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, a.y), Tokens::Color::LineSoft(), 1.f);
    float cy = a.y + row_h * 0.5f;

    ImU32 dot = (e.status == 1) ? Tokens::Color::Green()
              : (e.status == 2) ? Tokens::Color::Red()
                                : Tokens::Color::TextTert();
    float dx = a.x + Tokens::Size::RowPadH + 3.f;
    c.dl->AddCircleFilled(ImVec2(dx, cy), 3.5f, dot, 16);

    float bs = 16.f;
    ImVec2 xa = ImVec2(b.x - Tokens::Size::RowPadH - bs, cy - bs * 0.5f);
    ImVec2 xb = ImVec2(b.x - Tokens::Size::RowPadH, cy + bs * 0.5f);
    char idb[40]; ImFormatString(idb, sizeof(idb), "%s_x%d", idp, idx);
    Layout::Interact it = Layout::RegisterHit(idb, xa, xb);
    ImU32 xcol = it.hovered ? Tokens::Color::Red() : Tokens::Color::TextTert();
    c.dl->AddLine(ImVec2(xa.x + 4, xa.y + 4), ImVec2(xb.x - 4, xb.y - 4), xcol, 1.6f);
    c.dl->AddLine(ImVec2(xb.x - 4, xa.y + 4), ImVec2(xa.x + 4, xb.y - 4), xcol, 1.6f);

    ImFont* fs = Tokens::Type::BodySec();
    float fsh = fs ? fs->FontSize : 12.f;
    char lat[16];
    if (e.status == 1 && e.latencyMs >= 0) ImFormatString(lat, sizeof(lat), "%dms", e.latencyMs);
    else if (e.status == 2)                ImFormatString(lat, sizeof(lat), "dead");
    else                                   ImFormatString(lat, sizeof(lat), "--");
    ImVec2 lts = Draw::TextSize(fs, lat);
    float latRight = xa.x - 10.f;
    ImU32 latCol = (e.status == 1) ? Tokens::Color::Green()
                 : (e.status == 2) ? Tokens::Color::Red()
                                   : Tokens::Color::TextTert();
    Draw::TextL(c.dl, fs, ImVec2(latRight - lts.x, cy - fsh * 0.5f), latCol, lat);

    ImFont* fm = Tokens::Type::Mono(); if (!fm) fm = fs;
    float fmh = fm ? fm->FontSize : 12.f;
    float tx = dx + 12.f;
    c.dl->PushClipRect(ImVec2(tx, a.y), ImVec2(latRight - lts.x - 8.f, b.y), true);
    Draw::TextL(c.dl, fm, ImVec2(tx, cy - fmh * 0.5f), Tokens::Color::Text(), e.display.c_str());
    c.dl->PopClipRect();

    if (c.in_group) c.group_row_count++;
    c.cursor.y += row_h;
    return it.clicked;
}

static void EmptyRow(const char* text) {
    Ctx& c = GetCtx();
    float row_h = 34.f;
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImFont* fs = Tokens::Type::BodySec();
    float fsh = fs ? fs->FontSize : 12.f;
    ImVec2 ts = Draw::TextSize(fs, text);
    Draw::TextL(c.dl, fs, ImVec2((c.content_x0 + c.content_x1) * 0.5f - ts.x * 0.5f, y + (row_h - fsh) * 0.5f),
                Tokens::Color::TextTert(), text);
    c.cursor.y += row_h;
}

static int ProxyButtons(const char* idp) {
    Ctx& c = GetCtx();
    float y = Layout::ContentToScreenY(c.cursor.y);
    float W = c.content_x1 - c.content_x0;
    float gap = 8.f, x0 = c.content_x0 + 14.f;
    float bw = (W - 28.f - gap * 2) / 3.f, bh = 30.f;
    ImFont* fb = Tokens::Type::Body();
    const char* labels[3] = { "Import", "Check", "Clear" };
    int clicked = 0;
    for (int i = 0; i < 3; ++i) {
        ImVec2 a = ImVec2(x0 + i * (bw + gap), y);
        ImVec2 b = ImVec2(a.x + bw, y + bh);
        char idb[24]; ImFormatString(idb, sizeof(idb), "%s_b%d", idp, i);
        Layout::Interact it = Layout::RegisterHit(idb, a, b);
        ImU32 bg, fg, border = Tokens::Color::Line();
        if (i == 0) {
            bg = Tokens::Color::Accent();
            if (it.hovered && !it.held) bg = (bg & 0x00FFFFFF) | (0xFFu << 24);
            fg = IM_COL32_WHITE; border = 0;
        } else if (i == 2) {
            bg = it.held ? IM_COL32(255,59,48,90) : it.hovered ? IM_COL32(255,59,48,45) : Tokens::Color::BgCard();
            fg = Tokens::Color::Red();
        } else {
            bg = it.held ? Tokens::Color::BgPressed() : it.hovered ? Tokens::Color::BgHover() : Tokens::Color::BgCard();
            fg = Tokens::Color::Text();
        }
        Draw::RectFilledRounded(c.dl, a, b, bg, 6.f);
        if (border) Draw::RectStrokeRounded(c.dl, a, b, border, 6.f, 1.f);
        ImVec2 ts = Draw::TextSize(fb, labels[i]);
        Draw::TextL(c.dl, fb, ImVec2(a.x + (bw - ts.x) * 0.5f, a.y + (bh - ts.y) * 0.5f), fg, labels[i]);
        if (it.clicked) clicked = i + 1;
    }
    c.cursor.y += bh + 12.f;
    return clicked;
}

static void ProxySection(State& s, bool isGame) {
    std::vector<ProxyPool::Entry>& vec = isGame ? ProxyPool::g_game : ProxyPool::g_auth;
    char* importBuf = isGame ? s.proxy_game_import : s.proxy_auth_import;
    int   importCap = isGame ? (int)sizeof(s.proxy_game_import) : (int)sizeof(s.proxy_auth_import);
    const char* idp = isGame ? "gp" : "ap";

    std::vector<ProxyPool::Entry> snap;
    int alive = 0, dead = 0;
    {
        std::lock_guard<std::mutex> lk(ProxyPool::g_mtx);
        snap = vec;
        for (const ProxyPool::Entry& e : snap) { if (e.status == 1) ++alive; else if (e.status == 2) ++dead; }
    }
    char cnt[80];
    ImFormatString(cnt, sizeof(cnt), "%d saved   %d alive   %d dead", (int)snap.size(), alive, dead);
    StatLabel("Saved", cnt);

    if (snap.empty()) {
        EmptyRow("No proxies saved");
    } else {
        int removeIdx = -1;
        for (size_t i = 0; i < snap.size(); ++i)
            if (ProxyEntryRow(idp, (int)i, snap[i])) removeIdx = (int)i;
        if (removeIdx >= 0) {
            {
                std::lock_guard<std::mutex> lk(ProxyPool::g_mtx);
                if (removeIdx < (int)vec.size()) vec.erase(vec.begin() + removeIdx);
            }
            ProxyPool::SaveToDisk();
            s.proxy_pool_dirty = true;
        }
    }

    TextareaFullWidth(importBuf, importCap, "socks5://user:pass@host:1080   (one per line)", 78.f);

    int btn = ProxyButtons(idp);
    if (btn == 1) {
        int added = isGame ? ProxyPool::ImportGame(importBuf) : ProxyPool::ImportAuth(importBuf);
        if (added > 0) { importBuf[0] = 0; ProxyPool::SaveToDisk(); s.proxy_pool_dirty = true; }
    } else if (btn == 2) {
        s.proxy_check_req = isGame ? 1 : 2;
    } else if (btn == 3) {
        if (isGame) ProxyPool::ClearGame(); else ProxyPool::ClearAuth();
        ProxyPool::SaveToDisk();
        s.proxy_pool_dirty = true;
    }
}

void RailProxies(State& s) {
    PageHead("Proxies", "Hide your IP \xE2\x80\x94 route game and auth traffic through SOCKS5 / HTTP proxies.");

    GroupTitle("Game Proxies");
    BeginGroup();
        ToggleRow("wifi-teal", "Tunnel game traffic",
                  "Route the game's own connections through a proxy.", &s.proxy_tunnel_game);
        if (SegmentRow("gm", "Rotation", &ProxyPool::g_gameMode)) ProxyPool::SaveToDisk();
        ProxySection(s, true);
    EndGroup();

    GroupTitle("Auth Proxies");
    BeginGroup();
        ToggleRow("share-blue", "Route auth through proxy",
                  "Send login/auth requests through a proxy (HTTP only).", &s.proxy_enabled);
        if (SegmentRow("am", "Rotation", &ProxyPool::g_authMode)) ProxyPool::SaveToDisk();
        ProxySection(s, false);
    EndGroup();

    HelpText("Format: scheme://user:pass@host:port  (socks5:// or http://). "
             "Static keeps one IP; Rotating cycles to a new proxy each reconnect.");
}

}

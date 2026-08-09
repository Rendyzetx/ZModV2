#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include <cstdio>
#include <cstring>

#ifndef ZMOD_GIT_SHA
#define ZMOD_GIT_SHA "dev"
#endif

namespace ZmodUI {

static const char* RailDisplay(const char* key, State& s, bool* running) {
    *running = true;
    if (!std::strcmp(key, "auto-mine"))   { *running = s.am_enabled;  return "Auto Mine"; }
    if (!std::strcmp(key, "auto-fish"))   { *running = s.af_enabled;  return "Auto Fish"; }
    if (!std::strcmp(key, "auto-fossil")) { *running = s.ff_enabled;  return "Auto Fossil"; }
    if (!std::strcmp(key, "auto-nether")) { *running = s.an_enabled;  return "Auto Nether"; }
    *running = false;
    return "Idle";
}

void StatusStripRender(State& s) {
    Ctx& c = GetCtx();
    float H = Tokens::Size::StatusStripH;
    ImVec2 a = ImVec2(c.win_pos.x + Tokens::Size::SidebarW, c.win_pos.y + c.win_size.y - H);
    ImVec2 b = ImVec2(c.win_pos.x + c.win_size.x, c.win_pos.y + c.win_size.y);

    if (Theme::ShouldUseBackdropBlur()) {

        ImU32 stripTint = IM_COL32(0, 0, 0, 32);
        c.dl->AddRectFilled(a, b, stripTint, Tokens::Size::WindowRadius,
                            ImDrawFlags_RoundCornersBottomRight);
    } else {

        ImU32 bg = (Tokens::Color::Bg() & 0x00FFFFFF) | (220 << 24);
        c.dl->AddRectFilled(a, b, bg, Tokens::Size::WindowRadius,
                            ImDrawFlags_RoundCornersBottomRight);
    }
    c.dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, a.y), Tokens::Color::Line(), 1.f);

    ImFont* fb = Tokens::Type::BodySec();
    float y = a.y + (H - (fb ? fb->FontSize : 11.5f)) * 0.5f;
    float x = a.x + 22.f;

    c.dl->AddCircleFilled(ImVec2(a.x + 16.f, a.y + H*0.5f), 3.5f, Tokens::Color::Green(), 16);

    bool running = false;
    const char* botName = RailDisplay(c.active_rail, s, &running);

    char tmp[64];
    auto put = [&](const char* bold, const char* tail) {
        ImVec2 ts = Draw::TextSize(fb, bold);
        Draw::TextL(c.dl, fb, ImVec2(x, y), Tokens::Color::Text(), bold);
        x += ts.x + 4.f;
        if (tail && *tail) {
            ImVec2 ts2 = Draw::TextSize(fb, tail);
            Draw::TextL(c.dl, fb, ImVec2(x, y), Tokens::Color::TextSec(), tail);
            x += ts2.x;
        }
        x += 22.f;
    };

    ImFormatString(tmp, sizeof(tmp), "%s", botName);
    put(tmp, running ? " running" : "");

    put(ZMOD_GIT_SHA, " build");

    ImFormatString(tmp, sizeof(tmp), "%d", s.fps);
    put(tmp, " FPS");

    if (s.ping_ms > 0)  ImFormatString(tmp, sizeof(tmp), "%d", s.ping_ms);
    else                ImFormatString(tmp, sizeof(tmp), "%s", "\xE2\x80\x94");
    put(tmp, " ms");

    float xR = b.x - 22.f;
    {
        const char* t = "INSERT for menu";
        ImVec2 ts = Draw::TextSize(fb, t);
        Draw::TextL(c.dl, fb, ImVec2(xR - ts.x, y), Tokens::Color::TextSec(), t);
        xR -= (ts.x + 18.f);
    }
    {
        long sec = s.uptime_sec;
        int hh = (int)(sec / 3600), mm = (int)((sec % 3600) / 60), ss = (int)(sec % 60);
        char up[32]; ImFormatString(up, sizeof(up), "%02d:%02d:%02d", hh, mm, ss);
        ImVec2 ts2 = Draw::TextSize(fb, " uptime");
        ImVec2 ts1 = Draw::TextSize(fb, up);
        Draw::TextL(c.dl, fb, ImVec2(xR - ts2.x - ts1.x, y), Tokens::Color::Text(), up);
        Draw::TextL(c.dl, fb, ImVec2(xR - ts2.x, y), Tokens::Color::TextSec(), " uptime");
    }
}

}

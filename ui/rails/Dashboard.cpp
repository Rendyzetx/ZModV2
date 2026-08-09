#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../widgets/Primitives.h"
#include "../widgets/Row.h"
#include "../Fonts.h"
#include <cstdio>

namespace ZmodUI {

void RailDashboard(State& s) {
    Ctx& c = GetCtx();
    PageHead("Dashboard", nullptr);

    float y = Layout::ContentToScreenY(c.cursor.y);
    float W = c.content_x1 - c.content_x0;
    float H = 96.f;
    ImVec2 a = ImVec2(c.content_x0, y);
    ImVec2 b = ImVec2(c.content_x1, y + H);
    Draw::RectFilledRounded(c.dl, a, b, Tokens::Color::BgCard(), 14.f);
    Draw::RectStrokeRounded(c.dl, a, b, Tokens::Color::Line(), 14.f, 1.f);
    ImFont* fh = Tokens::Type::H2();
    ImFont* fb = Tokens::Type::BodySec();
    char hi[128]; ImFormatString(hi, sizeof(hi), "Welcome back, %s.", s.username);
    Draw::TextL(c.dl, fh, ImVec2(a.x + 24, a.y + 22), Tokens::Color::Text(), hi);
    char lic[128]; ImFormatString(lic, sizeof(lic), "License time remaining: %s", s.license);
    Draw::TextL(c.dl, fb, ImVec2(a.x + 24, a.y + 22 + 30), Tokens::Color::TextSec(), lic);
    long sec = s.uptime_sec;
    int hh = (int)(sec / 3600), mm = (int)((sec % 3600) / 60), ss = (int)(sec % 60);
    char meta[128]; ImFormatString(meta, sizeof(meta), "v%s \xC2\xB7 %02d:%02d:%02d runtime", s.version, hh, mm, ss);
    Draw::TextL(c.dl, fb, ImVec2(a.x + 24, a.y + 22 + 30 + 18), Tokens::Color::TextSec(), meta);

    ImFont* fe = Tokens::Type::Eyebrow();
    const char* pill = "Active";
    ImVec2 pts = Draw::TextSize(fe, pill);
    ImU32 pbg = IM_COL32(52,199,89,38);
    ImU32 pfg = IM_COL32(31,138,58,255);
    float pw = pts.x + 20.f;
    float ph = pts.y + 8.f;
    ImVec2 pa = ImVec2(b.x - 24 - pw, a.y + (H - ph)*0.5f);
    ImVec2 pb = ImVec2(pa.x + pw, pa.y + ph);
    Draw::RectFilledRounded(c.dl, pa, pb, pbg, 999.f);
    Draw::TextL(c.dl, fe, ImVec2(pa.x + 10, pa.y + 4), pfg, pill);
    c.cursor.y += H + 18.f;

    float gap = 10.f;
    float card_w = (W - gap*3) / 4.f;
    float yq = Layout::ContentToScreenY(c.cursor.y);
    auto quad_card = [&](int idx, const char* eb, const char* val, const char* trend, int tone) {
        float xq = c.content_x0 + idx * (card_w + gap);
        ImVec2 qa = ImVec2(xq, yq);
        ImVec2 qb = ImVec2(xq + card_w, yq + 80.f);
        Draw::RectFilledRounded(c.dl, qa, qb, Tokens::Color::BgCard(), 10.f);
        Draw::RectStrokeRounded(c.dl, qa, qb, Tokens::Color::Line(), 10.f, 1.f);
        ImFont* feb_ = Tokens::Type::Eyebrow();
        ImFont* fv  = Tokens::Type::H2();
        ImFont* fbs = Tokens::Type::BodySec();
        Draw::TextL(c.dl, feb_, ImVec2(qa.x + 16, qa.y + 14), Tokens::Color::TextSec(), eb);
        Draw::TextL(c.dl, fv,  ImVec2(qa.x + 16, qa.y + 30), Tokens::Color::Text(), val);
        ImU32 tcol = tone == 1 ? Tokens::Color::Orange() : (tone == 2 ? Tokens::Color::Red() : Tokens::Color::Green());
        Draw::TextL(c.dl, fbs, ImVec2(qa.x + 16, qa.y + 58), tcol, trend);
    };
    char vb[32];
    ImFormatString(vb, sizeof(vb), "%d", s.fps);
    quad_card(0, "FPS", vb, "stable", 0);
    ImFormatString(vb, sizeof(vb), "%d ms", s.ping_ms);
    quad_card(1, "PING", vb, "good", 0);
    ImFormatString(vb, sizeof(vb), "%d%%", s.cpu_pct);
    quad_card(2, "CPU", vb, "low", 0);
    ImFormatString(vb, sizeof(vb), "%d MB", s.ram_mb);
    quad_card(3, "RAM", vb, "steady", 0);
    c.cursor.y += 80.f + 18.f;

    GroupTitle("Streamer Mode");
    BeginGroup();
        ToggleRow("spark-purple", "Streamer Mode", "Hides yours and other players' names. Local visual only.", &s.streamer_mode);
    EndGroup();

    GroupTitle("Account");
    BeginGroup();
        StatLabel("Username", s.username);
        char lv[16]; ImFormatString(lv, sizeof(lv), "%d", s.level);
        StatLabel("Level", lv);
        char bc[32]; ImFormatString(bc, sizeof(bc), "%ld", s.bytecoins);
        StatLabel("Bytecoins", bc);
        char gm[32]; ImFormatString(gm, sizeof(gm), "%ld", s.gems);
        StatLabel("Gems", gm);
    EndGroup();

    GroupTitle("Quick exit");
    BeginGroup();
        WRow::Opts panicO;
        panicO.icon_id = "logout-red";
        panicO.label   = "Panic";
        panicO.desc    = "Click here or press END to unload the cheat instantly.";
        WRow::RowResult pr = WRow::Begin(panicO);

        ImFont* fpb = Tokens::Type::Body();
        const char* PL = "Unload";
        ImVec2 pts2 = Draw::TextSize(fpb, PL);
        float pbw = pts2.x + 24.f;
        float pbh = (fpb ? fpb->FontSize : 13) + 10.f;
        ImVec2 pba = ImVec2(pr.end_b.x - pbw, (pr.a.y + pr.b.y)*0.5f - pbh*0.5f);
        ImVec2 pbb = ImVec2(pba.x + pbw, pba.y + pbh);
        Layout::Interact pbi = Layout::RegisterHit("panic_btn", pba, pbb);
        ImU32 pbg2 = pbi.held    ? IM_COL32(255,59,48,80)
                  : pbi.hovered ? IM_COL32(255,59,48,40)
                                : Tokens::Color::BgCard();
        Draw::RectFilledRounded(c.dl, pba, pbb, pbg2, 6.f);
        Draw::RectStrokeRounded(c.dl, pba, pbb, Tokens::Color::Line(), 6.f, 1.f);
        Draw::TextL(c.dl, fpb,
                    ImVec2(pba.x + (pbw - pts2.x)*0.5f, pba.y + (pbh - pts2.y)*0.5f),
                    Tokens::Color::Red(), PL);
        if (pbi.clicked && s.on_panic) s.on_panic();
    EndGroup();
}

}

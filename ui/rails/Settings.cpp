#include "../Internal.h"
#include "../Theme.h"
#include "../Tokens.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include "../widgets/Row.h"

namespace ZmodUI {

void RailSettings(State& s) {
    PageHead("Settings", "Appearance, identity, app controls.");

    GroupTitle("Appearance");
    BeginGroup();
        ThemeGrid(&s.theme_index);
        DrawerRow("diamond", "Custom Colors", "Edit every UI color.", "custom-colors");
    EndGroup();

    GroupTitle("Device ID");
    BeginGroup();
        Ctx& c = GetCtx();
        WRow::Opts o1; o1.icon_id = "database-gray"; o1.label = "Active ID";
        char descBuf[64]; ImFormatString(descBuf, sizeof(descBuf), "ID %s", s.hwid_short);
        o1.desc = descBuf;
        WRow::Begin(o1);
        WRow::Opts o2; o2.icon_id = "reload-orange"; o2.label = "Randomize Device ID"; o2.no_hover = false;
        WRow::RowResult r2 = WRow::Begin(o2);

        ImFont* fb = Tokens::Type::Body();
        const char* lab = "Randomize";
        ImVec2 ts = Draw::TextSize(fb, lab);
        float bw = ts.x + 24.f;
        float bh = (fb ? fb->FontSize : 13) + 10.f;
        ImVec2 ba = ImVec2(r2.end_b.x - bw, (r2.a.y + r2.b.y)*0.5f - bh*0.5f);
        ImVec2 bb = ImVec2(ba.x + bw, ba.y + bh);
        Layout::Interact bi = Layout::RegisterHit("rand_btn", ba, bb);
        ImU32 bbg = bi.held ? Tokens::Color::BgPressed() : (bi.hovered ? Tokens::Color::BgHover() : Tokens::Color::BgCard());
        Draw::RectFilledRounded(c.dl, ba, bb, bbg, 6.f);
        Draw::RectStrokeRounded(c.dl, ba, bb, Tokens::Color::Line(), 6.f, 1.f);
        Draw::TextL(c.dl, fb, ImVec2(ba.x + (bw - ts.x)*0.5f, ba.y + (bh - ts.y)*0.5f), Tokens::Color::Text(), lab);
        if (bi.clicked && s.on_randomize_hwid) s.on_randomize_hwid();
    EndGroup();

    GroupTitle("Game Controls");
    BeginGroup();

        WRow::Opts o3; o3.icon_id = "reload"; o3.label = "Reload Game";
        WRow::RowResult r3 = WRow::Begin(o3);
        Ctx& cc = GetCtx();
        ImFont* fb2 = Tokens::Type::Body();
        const char* L1 = "Reload";
        ImVec2 ts1 = Draw::TextSize(fb2, L1);
        float bw1 = ts1.x + 24.f, bh1 = (fb2 ? fb2->FontSize : 13) + 10.f;
        ImVec2 ba1 = ImVec2(r3.end_b.x - bw1, (r3.a.y + r3.b.y)*0.5f - bh1*0.5f);
        ImVec2 bb1 = ImVec2(ba1.x + bw1, ba1.y + bh1);
        Layout::Interact bi1 = Layout::RegisterHit("rel_btn", ba1, bb1);
        ImU32 bbg1 = bi1.held ? Tokens::Color::BgPressed() : (bi1.hovered ? Tokens::Color::BgHover() : Tokens::Color::BgCard());
        Draw::RectFilledRounded(cc.dl, ba1, bb1, bbg1, 6.f);
        Draw::RectStrokeRounded(cc.dl, ba1, bb1, Tokens::Color::Line(), 6.f, 1.f);
        Draw::TextL(cc.dl, fb2, ImVec2(ba1.x + (bw1 - ts1.x)*0.5f, ba1.y + (bh1 - ts1.y)*0.5f), Tokens::Color::Text(), L1);
        if (bi1.clicked && s.on_reload_game) s.on_reload_game();

        WRow::Opts o4; o4.icon_id = "logout-red"; o4.label = "Force Logout";
        WRow::RowResult r4 = WRow::Begin(o4);
        const char* L2 = "Log out";
        ImVec2 ts2 = Draw::TextSize(fb2, L2);
        float bw2 = ts2.x + 24.f, bh2 = (fb2 ? fb2->FontSize : 13) + 10.f;
        ImVec2 ba2 = ImVec2(r4.end_b.x - bw2, (r4.a.y + r4.b.y)*0.5f - bh2*0.5f);
        ImVec2 bb2 = ImVec2(ba2.x + bw2, ba2.y + bh2);
        Layout::Interact bi2 = Layout::RegisterHit("lo_btn", ba2, bb2);
        ImU32 bbg2 = bi2.held ? IM_COL32(255,59,48,40) : (bi2.hovered ? IM_COL32(255,59,48,20) : Tokens::Color::BgCard());
        Draw::RectFilledRounded(cc.dl, ba2, bb2, bbg2, 6.f);
        Draw::RectStrokeRounded(cc.dl, ba2, bb2, Tokens::Color::Line(), 6.f, 1.f);
        Draw::TextL(cc.dl, fb2, ImVec2(ba2.x + (bw2 - ts2.x)*0.5f, ba2.y + (bh2 - ts2.y)*0.5f), Tokens::Color::Red(), L2);
        if (bi2.clicked && s.on_force_logout) s.on_force_logout();
    EndGroup();

    GroupTitle("Config Sharing");
    BeginGroup();

        WRow::Opts oCS; oCS.icon_id = "diamond-gray"; oCS.label = "Last Saved"; oCS.desc = s.config_last_save_text;
        WRow::Begin(oCS);

        WRow::Opts oCE; oCE.icon_id = "share-blue"; oCE.label = "Export Config";
        oCE.desc = "Pick a .json path; share that file in Discord etc.";
        WRow::RowResult rCE = WRow::Begin(oCE);
        ImFont* fb3 = Tokens::Type::Body();
        const char* LE = "Export\xE2\x80\xA6";
        ImVec2 tsE = Draw::TextSize(fb3, LE);
        float bwE = tsE.x + 24.f, bhE = (fb3 ? fb3->FontSize : 13) + 10.f;
        ImVec2 baE = ImVec2(rCE.end_b.x - bwE, (rCE.a.y + rCE.b.y)*0.5f - bhE*0.5f);
        ImVec2 bbE = ImVec2(baE.x + bwE, baE.y + bhE);
        Layout::Interact biE = Layout::RegisterHit("cfg_export_btn", baE, bbE);
        ImU32 bbgE = biE.held ? Tokens::Color::BgPressed() : (biE.hovered ? Tokens::Color::BgHover() : Tokens::Color::BgCard());
        Draw::RectFilledRounded(cc.dl, baE, bbE, bbgE, 6.f);
        Draw::RectStrokeRounded(cc.dl, baE, bbE, Tokens::Color::Line(), 6.f, 1.f);
        Draw::TextL(cc.dl, fb3, ImVec2(baE.x + (bwE - tsE.x)*0.5f, baE.y + (bhE - tsE.y)*0.5f), Tokens::Color::Text(), LE);
        if (biE.clicked && s.on_export_config) s.on_export_config();

        WRow::Opts oCI; oCI.icon_id = "download-green"; oCI.label = "Import Config";
        oCI.desc = "Replace current settings with a shared .json file.";
        WRow::RowResult rCI = WRow::Begin(oCI);
        const char* LI = "Import\xE2\x80\xA6";
        ImVec2 tsI = Draw::TextSize(fb3, LI);
        float bwI = tsI.x + 24.f, bhI = (fb3 ? fb3->FontSize : 13) + 10.f;
        ImVec2 baI = ImVec2(rCI.end_b.x - bwI, (rCI.a.y + rCI.b.y)*0.5f - bhI*0.5f);
        ImVec2 bbI = ImVec2(baI.x + bwI, baI.y + bhI);
        Layout::Interact biI = Layout::RegisterHit("cfg_import_btn", baI, bbI);
        ImU32 bbgI = biI.held ? Tokens::Color::BgPressed() : (biI.hovered ? Tokens::Color::BgHover() : Tokens::Color::BgCard());
        Draw::RectFilledRounded(cc.dl, baI, bbI, bbgI, 6.f);
        Draw::RectStrokeRounded(cc.dl, baI, bbI, Tokens::Color::Line(), 6.f, 1.f);
        Draw::TextL(cc.dl, fb3, ImVec2(baI.x + (bwI - tsI.x)*0.5f, baI.y + (bhI - tsI.y)*0.5f), Tokens::Color::Text(), LI);
        if (biI.clicked && s.on_import_config) s.on_import_config();
    EndGroup();

    HelpText("Settings auto-save every 2 seconds. Use Export to share configs.");
}

}

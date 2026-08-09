#include "Drawer.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

namespace ZmodUI {

extern int CurrentEspIndex();

namespace DrawerBody {

static bool FloatInputRow(const char* label, float* v, float vmin, float vmax,
                          const char* placeholder = "0.0") {
    static thread_local char buf[32];
    static thread_local float lastVal = std::numeric_limits<float>::quiet_NaN();
    if (*v != lastVal) {
        std::snprintf(buf, sizeof(buf), "%g", *v);
        lastVal = *v;
    }
    bool edited = InputRow(label, buf, (int)sizeof(buf), placeholder);
    if (edited) {
        float parsed = (float)std::atof(buf);
        if (parsed < vmin) parsed = vmin;
        if (parsed > vmax) parsed = vmax;
        *v = parsed;
        lastVal = parsed;
    }
    return edited;
}

void Pathfinder(State& s) {
    BeginGroup();
        ToggleRow("Show path overlay", &s.pf_show_path);
        ToggleRow("Auto move", &s.pf_auto_move);
        static const char* mm_items[2] = { "Smooth", "Fast" };
        ComboRow("Movement method", mm_items, 2, &s.pf_move_method);
        if (s.pf_move_method == 0) {

            SliderRow("Move speed", "units / sec", &s.pf_move_speed, 0.5f, 30.0f);
            if (s.pf_move_speed > 4.0f)
                HelpText("\xE2\x9A\xA0  >4 may trip fly-detect kick. \xE2\x89\xA4\x34 is the safe baseline.");
            else
                HelpText("\xE2\x89\xA4\x34 units/sec = safe range (matches base run speed).");
        } else {

            SliderRow("Delay between hops", "ms", &s.pf_tp_interval_ms, 0, 1000);
            HelpText("Time between tile teleports. 0 = hop every frame "
                     "(fastest, risks server speedhack detection). "
                     "16-17 ms ≈ vanilla 60Hz rate.");
            SliderRow("Jitter band", "ms", &s.pf_tp_jitter_ms, 0, 100);
            HelpText("Random \xC2\xB1 noise added to each hop delay. Defeats "
                     "the 'metronome' anti-cheat fingerprint (perfectly-"
                     "even intervals are a known cheat signature). 20 ms = "
                     "safe + still feels fast.");
        }

        ToggleRow("Gravity pulse mode", &s.pf_grav_pulse);
        HelpText("Pulse the pathfinder's zero-gravity override on/off "
                 "instead of holding it constant. Default: 1500 ms on, "
                 "100 ms off. Defeats anti-cheat fingerprinting of "
                 "'gravity is always exactly 0'.");
        if (s.pf_grav_pulse) {
            SliderRow("  Pulse ON ms",  "ms", &s.pf_grav_pulse_on_ms,  50, 5000);
            SliderRow("  Pulse OFF ms", "ms", &s.pf_grav_pulse_off_ms,  0, 1000);
        }
        SliderRow("Stop range", "units", &s.pf_stop_range, 0.05f, 1.5f);
    EndGroup();

    GroupTitle("Airborne");
    BeginGroup();
        ToggleRow("Limit airborne tiles", &s.pf_airborne_check);
        SliderRow("Max airborne tiles", "tiles", &s.pf_max_airborne, 1, 20);
    EndGroup();

    GroupTitle("Player body fit");
    BeginGroup();
        SliderRow("Body offset", "\xC3\x97 tile", &s.pf_body_offset, 0.f, 1.f);
    EndGroup();

    GroupTitle("Risk");
    BeginGroup();
        ToggleRow("warn", "Walk on insta-kill tiles", "Lets the planner route through deadly blocks.", &s.pf_walk_instakill);

    EndGroup();

    GroupTitle("Visuals");
    BeginGroup();
        ToggleRow("Glow halo", &s.pf_glow_halo);
        ToggleRow("Flow particles", &s.pf_flow_particles);
        ToggleRow("Pulsing target", &s.pf_pulsing_target);
    EndGroup();

    GroupTitle("Debug");
    BeginGroup();
        ToggleRow("grid",        "Block overlay",
                  "Tint walkable / blocked tiles for path debugging.",
                  &s.pf_block_overlay);
        ToggleRow("pulse",       "Stats HUD",
                  "Corner overlay: last build status, tile count, time, "
                  "start/goal tile.",
                  &s.pf_dbg_stats_hud);
        ToggleRow("eye-orange",  "Tile Inspector",
                  "Alt + hover any in-world tile to see its full pathfinder "
                  "decision chain (block id, every predicate, FINAL passable).",
                  &s.pf_dbg_inspect);
        ToggleRow("spark",       "Verbose log",
                  "Console-log every BuildPath call (status, tiles, "
                  "start/goal, microseconds). Loud \xE2\x80\x94 toggle off "
                  "outside of active debugging.",
                  &s.pf_dbg_verbose_log);
    EndGroup();
}

void KeysToFly(State& s) {
    BeginGroup();
        ToggleRow("Enable Keys to Fly", &s.keys_to_fly);
        SliderRow("Fly speed", "\xC3\x97 base speed", &s.fly_speed, 1.f, 12.f);
    EndGroup();
}

void UncapFps(State& s) {
    BeginGroup();
        ToggleRow("Enable FPS Uncap", &s.fps_uncap);
        SliderRow("Target FPS", "fps", &s.target_fps, 60, 360);
    EndGroup();
}

void MinePathfinder(State& s) {
    BeginGroup();
        ToggleRow("Auto move", &s.am_pf_auto_move);
        ToggleRow("Draw route", &s.am_pf_draw_route);
        static const char* mm_items[2] = { "Smooth", "Fast" };
        ComboRow("Movement method", mm_items, 2, &s.am_pf_move_method);
        if (s.am_pf_move_method == 0) {

            SliderRow("Move speed", "units / sec", &s.am_pf_move_speed, 0.5f, 30.0f);
            if (s.am_pf_move_speed > 4.0f)
                HelpText("\xE2\x9A\xA0  >4 may trip fly-detect kick. \xE2\x89\xA4\x34 is the safe baseline.");
            else
                HelpText("\xE2\x89\xA4\x34 units/sec = safe range (matches base run speed).");
        } else {

            int interval = (int)s.am_pf_portal_interval_ms;
            SliderRow("Delay between hops", "ms", &interval, 0, 1000);
            s.am_pf_portal_interval_ms = (float)interval;
            HelpText("Time between tile teleports. 0 = hop every frame.");
        }
    EndGroup();
    GroupTitle("Path tuning");
    BeginGroup();
        SliderRow("Stop range", "units", &s.am_pf_stop_range, 0.05f, 1.5f);
        SliderRow("Body offset", "\xC3\x97 tile", &s.am_pf_body_offset, 0.f, 1.f);

    EndGroup();
    GroupTitle("Mining weights");
    BeginGroup();
        ToggleRow("Prefer staircase pattern", &s.am_pf_prefer_staircase);
        SliderRow("Up-break penalty", "", &s.am_pf_up_penalty, 0, 200);
        SliderRow("Unsupported up-break extra", "", &s.am_pf_up_extra, 0, 400);
    EndGroup();
    GroupTitle("Airborne Check");
    BeginGroup();
        ToggleRow("Airborne check", &s.am_pf_airborne_check);
        SliderRow("Max airborne tiles", "tiles", &s.am_pf_max_airborne, 1, 20);
    EndGroup();
}

void MineTargets(State& s) {
    BeginGroup();
        ToggleRow("Gemstones",          &s.am_t_gemstones);
        ToggleRow("Nuggets",            &s.am_t_nuggets);
        ToggleRow("Light crystals",     &s.am_t_light_crystals);
        ToggleRow("Pots",               &s.am_t_pots);
        ToggleRow("Floor collectables", &s.am_t_floor_collectables);
        ToggleRow("AI enemies",         &s.am_t_ai_enemies);
        ToggleRow("Exit portal (last)", &s.am_t_exit_portal);
    EndGroup();
}

void MineCombat(State& s) {
    BeginGroup();
        ToggleRow("Combat (HitAI)", &s.am_c_hit_ai);
        SliderRow("Combat range",  "tiles", &s.am_c_range, 1, 8);
        SliderRow("Combat cadence","s",     &s.am_c_cadence, 0.05f, 2.f);
        SliderRow("Hit cadence",   "s",     &s.am_c_hit_cadence, 0.05f, 1.f);
    EndGroup();
}

void MineBankBot(State& s) {
    BeginGroup();
        ToggleRow("Auto-Deposit", &s.am_bb_auto_deposit);
        SliderRow("Delay", "ms", &s.am_bb_delay_ms, 100, 5000);
    EndGroup();
    GroupTitle("Items to deposit");

    ImGui::PushID("bb_nuggets");
    BeginGroup();
        SubLabel("Nuggets");
        const char* nug[5] = {"Bronze","Silver","Gold","Platinum","Dark"};
        for (int i = 0; i < 5; ++i) ToggleRow(nug[i], &s.am_bb_nuggets[i]);
    EndGroup();
    ImGui::PopID();
    ImGui::PushID("bb_keys");
    BeginGroup();
        SubLabel("Keys");
        const char* keys[5] = {"Bronze","Silver","Gold","Platinum","Darkstone"};
        for (int i = 0; i < 5; ++i) ToggleRow(keys[i], &s.am_bb_keys[i]);
    EndGroup();
    ImGui::PopID();
    ImGui::PushID("bb_materials");
    BeginGroup();
        SubLabel("Mining Ingredients");
        const char* mats[9] = {
            "Rusty Nail","Old Miner\xE2\x80\x99s Shoe","Piece of Rope","Tin Can",
            "Matchbox","Miner\xE2\x80\x99s Bucket","Gold Tooth","Pocket Watch","Silver Coin"
        };
        for (int i = 0; i < 9; ++i) ToggleRow(mats[i], &s.am_bb_materials[i]);
    EndGroup();
    ImGui::PopID();
    GroupTitle("Gemstones");
    BeginGroup();
        GemMatrix(s.am_bb_gems);
    EndGroup();
}

void MineAutoSell(State& s) {
    BeginGroup();
        ToggleRow("Auto sell gems", &s.am_as_enabled);
    EndGroup();
    GroupTitle("Gemstones");
    BeginGroup();
        GemMatrix(s.am_as_gems);
    EndGroup();
}

void MineRejoin(State& s) {
    BeginGroup();
        ToggleRow("Auto-Rejoin on disconnect", &s.am_rj_enabled);
        SliderRow("Delay", "ms", &s.am_rj_delay_ms, 100, 10000);
    EndGroup();
}

void EspRow(State& s) {
    int idx = CurrentEspIndex();
    if (idx < 0 || idx > 9) idx = 0;

    GroupTitle("Snaplines");

    ImGui::PushID("esprow_snaplines");
    BeginGroup();
        ToggleRow("Enable lines", &s.esp_lines[idx]);
        static const char* origin_items[3] = {"Local Player","Top of screen","Bottom of screen"};
        static int origin = 0;
        ComboRow("Origin", origin_items, 3, &origin);
        static float thick = 2.0f;
        SliderRow("Thickness", "px", &thick, 0.5f, 6.f);

        Ctx& c = GetCtx();
        ImFont* fb = Tokens::Type::Body();
        float y = Layout::ContentToScreenY(c.cursor.y);
        ImVec2 a = ImVec2(c.content_x0, y);
        ImVec2 b = ImVec2(c.content_x1, y + Tokens::Size::RowMinH);
        Draw::TextL(c.dl, fb, ImVec2(a.x + Tokens::Size::RowPadH + 26 + 12, a.y + (Tokens::Size::RowMinH - (fb?fb->FontSize:13))*0.5f), Tokens::Color::Text(), "Color");
        float sw_w = 28, sw_h = 22;
        ImVec2 sa = ImVec2(b.x - Tokens::Size::RowPadH - sw_w, a.y + (Tokens::Size::RowMinH - sw_h)*0.5f);
        ImVec2 sb_ = ImVec2(sa.x + sw_w, sa.y + sw_h);
        char swid[40]; ImFormatString(swid, sizeof(swid), "esp_color_sw_%d", idx);
        Layout::Interact swh = Layout::RegisterHit(swid, sa, sb_);
        ImU32 col = ImGui::ColorConvertFloat4ToU32(s.esp_color[idx]);
        Draw::RectFilledRounded(c.dl, sa, sb_, col, 4.f);
        ImU32 swBorder = swh.hovered ? Tokens::Color::Accent() : IM_COL32(0,0,0,80);
        Draw::RectStrokeRounded(c.dl, sa, sb_, swBorder, 4.f, swh.hovered ? 2.f : 1.f);
        c.dl->AddLine(ImVec2(a.x, b.y), ImVec2(b.x, b.y), Tokens::Color::LineSoft(), 1.f);

        char hex[16];
        ImVec4 c4 = s.esp_color[idx];
        ImFormatString(hex, sizeof(hex), "#%02X%02X%02X",
                       (int)(c4.x * 255), (int)(c4.y * 255), (int)(c4.z * 255));
        ImVec2 hts = Draw::TextSize(fb, hex);
        Draw::TextL(c.dl, fb,
                    ImVec2(sa.x - 10.f - hts.x, a.y + (Tokens::Size::RowMinH - hts.y) * 0.5f),
                    Tokens::Color::TextSec(), hex);

        char popid[40]; ImFormatString(popid, sizeof(popid), "##esp_cp_%d", idx);
        if (swh.clicked) ImGui::OpenPopup(popid);

        ImGuiIO& io = ImGui::GetIO();
        float popW = 280.f;
        float popX = sa.x - popW - 8.f;
        if (popX < 8.f) popX = sb_.x + 8.f;
        if (popX + popW > io.DisplaySize.x - 8.f) popX = io.DisplaySize.x - popW - 8.f;
        ImGui::SetNextWindowPos(ImVec2(popX, sb_.y + 6.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(popW, 0.f), ImGuiCond_Always);
        ImVec4 work = s.esp_color[idx];
        if (Draw::StyledColorPickerPopup(popid, "ESP Color", &work, true)) {
            s.esp_color[idx] = work;
        }
        c.cursor.y += Tokens::Size::RowMinH;
    EndGroup();
    ImGui::PopID();

    GroupTitle("Boxes");
    ImGui::PushID("esprow_boxes");
    BeginGroup();
        ToggleRow("Enable boxes", &s.esp_boxes[idx]);
        static const char* box_type_items[2] = {"Corner","Full"};
        static int box_type = 0;
        ComboRow("Box type", box_type_items, 2, &box_type);
        static float box_thick = 2.0f;
        SliderRow("Thickness", "px", &box_thick, 0.5f, 6.f);
        static bool fill = false;
        ToggleRow("Fill", &fill);
    EndGroup();
    ImGui::PopID();

    GroupTitle("Health");
    ImGui::PushID("esprow_health");
    BeginGroup();
        ToggleRow("Enable health", &s.esp_health[idx]);
        static const char* health_style_items[4] = {"Side bar","Bottom bar","Text only","Side bar + text"};
        static int hstyle = 0;
        ComboRow("Style", health_style_items, 4, &hstyle);
    EndGroup();
    ImGui::PopID();
}

void Logger(State& s) {
    if (DestructiveButton("Clear log")) {
        if (s.on_clear_log) s.on_clear_log();
    }
    Spacing(8.f);

    Ctx& c = GetCtx();
    float W = c.content_x1 - c.content_x0;
    float col_t = 70, col_d = 36, col_id = W - col_t - col_d - 70 - 14, col_s = 70;
    float y = Layout::ContentToScreenY(c.cursor.y);
    ImVec2 a = ImVec2(c.content_x0, y);
    ImVec2 b = ImVec2(c.content_x1, y + 30);
    Draw::RectFilledRounded(c.dl, a, b, Tokens::Color::Bg(), 6.f);
    ImFont* fe = Tokens::Type::Eyebrow();
    auto hdr = [&](const char* t, float x, float w) {
        Draw::TextL(c.dl, fe, ImVec2(x + 10, a.y + 8), Tokens::Color::TextSec(), t);
    };
    hdr("TIME", a.x, col_t);
    hdr("DIR",  a.x + col_t, col_d);
    hdr("ID",   a.x + col_t + col_d, col_id);
    hdr("SIZE", a.x + col_t + col_d + col_id, col_s);
    c.cursor.y += 30;

    ImFont* fm = Tokens::Type::Mono();
    int n = s.packet_count;
    if (n <= 0) n = 0;
    const char* fallback_ids[5] = { "SendBson","TileUpdate","PlayerMove","MiningGem","SendBlockMessage" };
    for (int i = 0; i < (n > 0 ? n : 20); ++i) {
        float ry = Layout::ContentToScreenY(c.cursor.y);
        ImVec2 ra = ImVec2(c.content_x0, ry);
        ImVec2 rb = ImVec2(c.content_x1, ry + 22);
        c.dl->AddLine(ImVec2(ra.x, rb.y), ImVec2(rb.x, rb.y), Tokens::Color::LineSoft(), 1.f);
        char idb[16]; ImFormatString(idb, sizeof(idb), "pkt_%d", i);
        Layout::Interact it = Layout::RegisterHit(idb, ra, rb);
        if (it.hovered) Draw::RectFilledRounded(c.dl, ra, rb, Tokens::Color::BgHover(), 0.f);
        if (it.clicked) s.selected_packet_index = i;
        bool out = (n > 0) ? s.packets[i].outgoing : (i % 3 != 0);
        ImU32 col = out ? Tokens::Color::Accent() : Tokens::Color::Green();
        char tbuf[16] = "00:00:00";
        const char* id = (n > 0) ? s.packets[i].id : fallback_ids[i % 5];
        int sz = (n > 0) ? s.packets[i].size_bytes : (24 + (i*13)%80);
        char sbuf[16]; ImFormatString(sbuf, sizeof(sbuf), "%d B", sz);
        Draw::TextL(c.dl, fm, ImVec2(ra.x + 10, ra.y + 4), col, tbuf);
        Draw::TextL(c.dl, fm, ImVec2(ra.x + col_t + 10, ra.y + 4), col, out ? "OUT" : "IN");
        Draw::TextL(c.dl, fm, ImVec2(ra.x + col_t + col_d + 10, ra.y + 4), col, id);
        Draw::TextL(c.dl, fm, ImVec2(ra.x + col_t + col_d + col_id + 10, ra.y + 4), col, sbuf);
        c.cursor.y += 22;
    }
}

void Repeater(State& s) {
    HelpText("Edit and resend a packet. Select a packet from the logger first.");
    TextareaFullWidth(s.net_repeater_buf, sizeof(s.net_repeater_buf), "Edit and resend a packet…", 120.f);
    if (ButtonFullWidth("Send packet", 32.f)) {
        if (s.on_send_packet) s.on_send_packet();
    }
}

void Ignore(State& s) {
    HelpText("Comma-separated packet IDs to ignore.");
    TextareaFullWidth(s.net_ignore_buf, sizeof(s.net_ignore_buf), "PlayerMove, TileUpdate", 120.f);
    if (ButtonFullWidth("Apply", 32.f)) {
        if (s.on_apply_ignore_list) s.on_apply_ignore_list();
    }
}

void CustomColors(State& s) {
    BeginGroup();

        static const char* presets[Theme::PresetCount] = {
            "Liquid Glass","Dark","Light","Forest","Sunset","Ocean","Rose","Mono","Custom"
        };

        int active = (int)Theme::CurrentPreset();
        if (s.theme_index != active) s.theme_index = active;
        if (ComboRow("Seed from", presets, Theme::PresetCount, &s.theme_index)) {
            Theme::ResetCustomToPreset((Theme::Preset)s.theme_index);
            s.theme_index = (int)Theme::CurrentPreset();
        }
        if (Button("Reset to Dark")) {
            Theme::SetPreset(Theme::Dark);
            s.theme_index = (int)Theme::Dark;
        }
    EndGroup();
    GroupTitle("Edit colors");
    BeginGroup();
        ColorList();
    EndGroup();
}

void NetherPathfinder(State& s) {
    GroupTitle("Movement");
    BeginGroup();
        static const char* mm_items[2] = { "Smooth", "Fast" };
        ComboRow("Movement method", mm_items, 2, &s.an_pf_move_method);
        if (s.an_pf_move_method == 0) {

            SliderRow("Move speed", "units / sec", &s.an_pf_move_speed, 0.5f, 30.0f);
            if (s.an_pf_move_speed > 4.0f)
                HelpText("\xE2\x9A\xA0  >4 may trip fly-detect kick. \xE2\x89\xA4\x34 is the safe baseline.");
            else
                HelpText("\xE2\x89\xA4\x34 units/sec = safe range (matches base run speed).");
        } else {

            int interval = (int)s.an_pf_portal_interval_ms;
            SliderRow("Delay between hops", "ms", &interval, 0, 1000);
            s.an_pf_portal_interval_ms = (float)interval;
            HelpText("Time between tile teleports. 0 = hop every frame.");
        }
        SliderRow("Flight speed", "x", &s.an_flight_speed, 0.5f, 2.5f);
        SliderRow("Stop range", "units", &s.an_pf_stop_range, 0.05f, 1.5f);
        SliderRow("Body offset", "\xC3\x97 tile", &s.an_pf_body_offset, 0.f, 1.f);
    EndGroup();
    GroupTitle("Airborne");
    BeginGroup();
        ToggleRow("Airborne check", &s.an_pf_airborne_check);
        SliderRow("Max airborne tiles", "tiles", &s.an_pf_max_airborne, 1, 20);
    EndGroup();

    GroupTitle("Risk");
    BeginGroup();
        ToggleRow("Walk on insta-kill tiles", &s.an_pf_walk_instakill);
    EndGroup();
    GroupTitle("Visuals");
    BeginGroup();
        ToggleRow("Glow halo", &s.an_pf_glow_halo);
        ToggleRow("Flow particles", &s.an_pf_flow_particles);
        ToggleRow("Pulsing target", &s.an_pf_pulsing_target);
    EndGroup();
}

void DbgInvPicker(State& s) {
    HelpText("Tap a row to copy {blockType, itemType} into the selected IK. "
             "The picker writes to ConvertItems or Repair based on which "
             "row in the Debug rail opened the drawer.");

    BeginGroup();
        if (ButtonFullWidth("Refresh inventory snapshot", 30.f)) {
            if (s.on_dbg_refresh_inventory) s.on_dbg_refresh_inventory();
        }
    EndGroup();

    Ctx& c = GetCtx();
    if (s.dbg_inv_count <= 0) {
        GroupTitle("Empty");
        BeginGroup();
            HelpText("No inventory items snapshot. Refresh after entering a world.");
        EndGroup();
        return;
    }

    char title[48];
    ImFormatString(title, sizeof(title), "Items (%d)", s.dbg_inv_count);
    GroupTitle(title);
    BeginGroup();
        ImFont* fb = Tokens::Type::Body();
        ImFont* fs = Tokens::Type::BodySec();
        for (int i = 0; i < s.dbg_inv_count; ++i) {
            float y = Layout::ContentToScreenY(c.cursor.y);
            float h = 30.f;
            ImVec2 ra = ImVec2(c.content_x0, y);
            ImVec2 rb = ImVec2(c.content_x1, y + h);
            char idb[32];
            ImFormatString(idb, sizeof(idb), "dbg_inv_%d", i);
            Layout::Interact hit = Layout::RegisterHit(idb, ra, rb);
            if (hit.hovered) {
                Draw::RectFilledRounded(c.dl, ra, rb, Tokens::Color::BgHover(), 6.f);
            }
            if (hit.clicked) {
                if (s.on_dbg_pick_inv) s.on_dbg_pick_inv(i);
                CloseDrawer();
            }

            char left[48];
            ImFormatString(left, sizeof(left), "bt=%-5d it=%d",
                           s.dbg_inv_block[i], s.dbg_inv_item[i]);
            Draw::TextL(c.dl, fb, ImVec2(ra.x + 14, ra.y + 8),
                        Tokens::Color::Text(), left);

            char right[24];
            ImFormatString(right, sizeof(right), "x%d", s.dbg_inv_qty[i]);
            ImVec2 ts = Draw::TextSize(fs, right);
            Draw::TextL(c.dl, fs, ImVec2(rb.x - 18.f - ts.x, ra.y + 9),
                        Tokens::Color::TextSec(), right);
            c.cursor.y += h + 2.f;
        }
    EndGroup();
}

}
}

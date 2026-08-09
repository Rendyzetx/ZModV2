#include "../Internal.h"
#include <cstdio>

namespace ZmodUI {

void RailAutoFish(State& s) {
    PageHead("Auto Fish", "Fishing bot with humanized reaction timing.");

    GroupTitle("General");
    BeginGroup();
        ToggleRow("fish-teal", "Enable Fish Bot", nullptr, &s.af_enabled);
        ToggleRow("arrow",      "Auto Land",  nullptr, &s.af_auto_land);
        ToggleRow("reload",     "Auto Recast",nullptr, &s.af_auto_recast);
        ToggleRow("pulse",      "Anti-Sleep", nullptr, &s.af_anti_sleep);
        ToggleRow("sleep-pink", "Anti-AFK",   nullptr, &s.af_anti_afk);
    EndGroup();

    GroupTitle("Timing");
    BeginGroup();
        SliderRow("Recast min",        "s",  &s.af_recast_min, 0.5f, 8.f);
        SliderRow("Recast max",        "s",  &s.af_recast_max, 0.5f, 8.f);
        SliderRow("Hook reaction min", "ms", &s.af_hook_min, 50.f, 1000.f);
        SliderRow("Hook reaction max", "ms", &s.af_hook_max, 50.f, 1000.f);
        SliderRow("Minigame box speed", "x", &s.af_speed_mult, 0.5f, 5.0f);
    EndGroup();

    GroupTitle("Size filter");
    BeginGroup();
        ToggleRow("fish-teal", "Enable size filter",
                  "Only fish whose size is selected below are kept. When a "
                  "de-selected fish hooks, the minigame is skipped and AutoFish "
                  "recasts. Watch the readout to calibrate.",
                  &s.af_size_filter);

        static const char* kSizeNames[5] = { "Tiny", "Small", "Medium", "Large", "Huge" };
        Ctx& cc = GetCtx();
        float y0 = Layout::ContentToScreenY(cc.cursor.y) + 12.f;
        float x  = cc.content_x0 + 14.f;
        ImFont* fb = Tokens::Type::Body();
        for (int i = 0; i < 5; ++i) {
            const char* L = kSizeNames[i];
            ImVec2 ts = Draw::TextSize(fb, L);
            float w = ts.x + 20.f, h = (fb ? fb->FontSize : 13) + 12.f;
            ImVec2 a = ImVec2(x, y0);
            ImVec2 b = ImVec2(x + w, y0 + h);
            char idb[16]; ImFormatString(idb, sizeof(idb), "fsz_%d", i);
            Layout::Interact it = Layout::RegisterHit(idb, a, b);
            if (it.clicked) s.af_keep_size[i] = !s.af_keep_size[i];
            bool on = s.af_keep_size[i];
            ImU32 bg = on ? Tokens::Color::Green() : Tokens::Color::Bg();
            ImU32 fg = on ? IM_COL32_WHITE : Tokens::Color::Text();
            ImU32 border = on ? 0 : Tokens::Color::Line();
            if (it.hovered && !on) bg = Tokens::Color::BgHover();
            Draw::RectFilledRounded(cc.dl, a, b, bg, 999.f);
            if (border) Draw::RectStrokeRounded(cc.dl, a, b, border, 999.f, 1.f);

            if (s.af_last_fish_bucket == i)
                Draw::RectStrokeRounded(cc.dl, a, b, Tokens::Color::Accent(), 999.f, 2.f);
            Draw::TextL(cc.dl, fb, ImVec2(a.x + (w - ts.x)*0.5f, a.y + 6.f), fg, L);
            x += w + 6.f;
        }
        cc.cursor.y += 12.f + (fb ? fb->FontSize : 13) + 12.f + 12.f;

        char rb[48];
        const char* nm = (s.af_last_fish_bucket >= 0 && s.af_last_fish_bucket < 5)
                         ? kSizeNames[s.af_last_fish_bucket] : "-";
        std::snprintf(rb, sizeof(rb), "%s  (%.2fx)", nm, s.af_last_fish_size);
        StatLabel("Last fish", rb);
    EndGroup();

    GroupTitle("Spot & Portal");
    BeginGroup();
        ToggleRow("arrow", "Return to spot",
                  "Walk back to the cast tile if knocked away before recasting.",
                  &s.af_return_to_spot);
        SliderRow("Max drift", "tiles", &s.af_max_drift_tiles, 1.f, 10.f);
        if (Button("Clear spot cache") && s.on_clear_spot_cache)
            s.on_clear_spot_cache();
    EndGroup();

}

}

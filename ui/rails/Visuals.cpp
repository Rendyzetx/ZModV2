#include "../Internal.h"

namespace ZmodUI {

void RailVisuals(State& s) {
    PageHead("Visuals", "ESP overlays, camera, world appearance.");

    GroupTitle("Camera");
    BeginGroup();
        ToggleRow("camera-orange", "Show Mini-Map", "Top-right map overlay.", &s.show_minimap);
        ToggleRow("camera-purple", "Free Cam", "Detach view from player.", &s.free_cam);
        ToggleRow("zoom-teal",     "Extra Zoom",   "Scale camera ortho size past the in-game Max.", &s.extra_zoom);
        if (s.extra_zoom)
            SliderRow("Zoom Multiplier", "x", &s.extra_zoom_mul, 1.0f, 15.0f);
    EndGroup();

    GroupTitle("Inspector");
    BeginGroup();
        ToggleRow("eye-teal", "Block Tooltip",
                  "Hold Alt to inspect any tile in-world. Shows the same info card the minimap shows on hover.",
                  &s.block_tooltip);
    EndGroup();

    GroupTitle("ESP");
    BeginGroup();
        ToggleRow("eye-orange", "Enable ESP", "Master switch for all overlays.", &s.esp_master);
        ToggleRow("pulse-teal",  "Show ESP Info", "AI count, scan rate.", &s.esp_show_info);
    EndGroup();

    GroupTitle("Types");
    EspMatrix(s);

    GroupTitle("Gemstone Types");
    BeginGroup();
        static const char* gem_names[5] = {"Diamonds","Rubies","Emeralds","Sapphires","Topazes"};
        ChipRow(gem_names, s.gem_chip, 5);
    EndGroup();

    GroupTitle("Crystal Types");
    BeginGroup();
        static const char* cry_names[7] = {
            "Orb Lighting Mining","Light Crystal Small","Light Crystal Medium","Light Crystal Large",
            "Time Crystal Small","Time Crystal Medium","Time Crystal Large"
        };
        ChipRow(cry_names, s.crystal_chip, 7);
    EndGroup();

    GroupTitle("World");
    BeginGroup();
        ToggleRow("spark-yellow", "Anti-Darkness", "Cancel dark world lighting.", &s.anti_darkness);
        ToggleRow("rain-purple",  "Force Weather", nullptr, &s.force_weather);
        static const char* weather[14] = {
            "None","Heavy Rain","Pixel Trail","Sand Storm","Light Rain","Light Snow","Snow Storm",
            "Deep Nether","Halloween","Halloween Tower","Hearts","Mining","Aurora Borealis","Armageddon"
        };
        ComboRow("Weather", weather, 14, &s.weather_idx);
        ToggleRow("spark-yellow", "Force Lighting", nullptr, &s.force_lighting);
        static const char* lighting[5] = {"None","Dark","Mining","Lesser Dark","Great Dark"};
        ComboRow("Lighting", lighting, 5, &s.lighting_idx);
        static const char* bg[12] = {"Forest","Night","Space","Desert","Ice","Star","Candy","Halloween Tower","Nether","City","Blue Sky","Jet Race"};
        ComboRow("Background", bg, 12, &s.background_idx);

        if (ButtonFullWidth("Apply", 32.f)) {
            if (s.on_apply_background) s.on_apply_background();
        }
    EndGroup();
}

}

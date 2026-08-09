#include "../Internal.h"

namespace ZmodUI {

void RailMisc(State& s) {
    PageHead("Misc", "Quality-of-life toggles and overrides.");

    GroupTitle("Items");
    BeginGroup();
        ToggleRow("recipe-green",  "Unlock All Recipes", "Reveal every craft recipe locally.", &s.misc_unlock_recipes);
        ToggleRow("x-indigo",      "Anti-Collect", "Skip auto-pickup on touch.", &s.misc_anti_collect);
        ToggleRow("magnet-green",  "Auto-Collect", "Magnet nearby drops to you.", &s.misc_auto_collect);
        ToggleRow("gift-pink",     "Auto Gift Box", "Auto-open gift boxes as you step onto them. Server enforces the on-tile check, no client-side range.", &s.auto_gift_box);
    EndGroup();

    GroupTitle("Performance");
    BeginGroup();
        DrawerRow("spark", "Uncap FPS", "Target higher refresh rate.", "uncap-fps");
    EndGroup();
}

}

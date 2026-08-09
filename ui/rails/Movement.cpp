#include "../Internal.h"

namespace ZmodUI {

void RailMovement(State& s) {
    PageHead("Movement", "Speed, pathfinding, manual fly control.");

    GroupTitle("Speed");
    BeginGroup();
        SliderRow("Player Speed", "\xC3\x97 base velocity", &s.player_speed, 1.f, 9.f);
    EndGroup();

    GroupTitle("Movement Overrides");
    BeginGroup();
        ToggleRow("arrow", "Enable Jump Override", nullptr, &s.jump_override);
        static const char* jump_items[5] = {"Normal","Double","Long Jump","Parachute","Triple"};
        ComboRow("Jump Mode", jump_items, 5, &s.jump_mode);
        ToggleRow("arrow", "Enable Gravity Override", nullptr, &s.grav_override);
        static const char* grav_items[3] = {"Normal","Low","High"};
        ComboRow("Gravity Mode", grav_items, 3, &s.grav_mode);
    EndGroup();

    GroupTitle("Fly");
    BeginGroup();
        ToggleRow("jetpack-green", "Infinite Jetpack", "Jetpack fuel never burns.", &s.infinite_jetpack);
        DrawerRow("arrow-teal", "Keys to Fly", "Hold a key to fly.", "keys-to-fly");
    EndGroup();

    GroupTitle("Pathfinder");
    BeginGroup();
        ToggleRow("arrow", "Auto Navigate", "Hold Alt and right-click a tile to set destination.", &s.auto_navigate);
        DrawerRow("settings-gray", "Path Settings", "Speed, range, visuals, airborne, body offset.", "pathfinder");
    EndGroup();

    GroupTitle("Status");
    BeginGroup();
        StatLabel("Navigation", "Idle");
    EndGroup();
}

}

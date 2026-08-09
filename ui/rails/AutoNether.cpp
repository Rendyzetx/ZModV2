#include "../Internal.h"

namespace ZmodUI {

void RailAutoNether(State& s) {
    PageHead("Auto Nether", "Nether run bot with target queue.");

    GroupTitle("General");
    BeginGroup();
        ToggleRow("flame-red", "Enable",             nullptr, &s.an_enabled);
        ToggleRow("arrow",     "Auto move",          nullptr, &s.an_auto_move);
        ToggleRow("arrow",     "Draw route",         nullptr, &s.an_draw_route);

        ToggleRow("portal-red","Prioritise closest", "Engage nearest target first.", &s.an_prioritise_closest);
        ToggleRow("pulse",     "Anti-Sleep",         nullptr, &s.an_anti_sleep);
        ToggleRow("sleep-pink","Anti-AFK",           nullptr, &s.an_anti_afk);
    EndGroup();

    GroupTitle("Targets");
    BeginGroup();
        ToggleRow("combat-red",      "AI enemies",         nullptr, &s.an_t_ai);
        ToggleRow("treasure-orange", "Treasure chests",    nullptr, &s.an_t_treasure);
        ToggleRow("gift-pink",       "Gift boxes",         nullptr, &s.an_t_gifts);
        ToggleRow("floor",           "Floor collectables", nullptr, &s.an_t_collectables);
        ToggleRow("portal-purple",   "Exit portal",        nullptr, &s.an_t_exit);
    EndGroup();

    GroupTitle("Combat");
    BeginGroup();
        SliderRow("Melee cooldown",  "s",     &s.an_melee_cooldown, 0.05f, 2.f);
        SliderRow("Max melee range", "tiles", &s.an_max_melee_range, 1.f, 10.f);
    EndGroup();

    GroupTitle("Auto-enter");
    BeginGroup();
        ToggleRow("flame-red", "Deep loop",
                  "Auto-enter DEEPNETHER with Blood Scrolls, complete runs for embers, repeat.",
                  &s.an_deep_loop);
        ToggleRow("portal-purple", "Network join (fallback)",
                  "If using the Blood Scroll doesn't enter, join via packets instead.",
                  &s.an_deep_net_join);
        ToggleRow("flame-red", "Loop levels 1-5",
                  "Auto-enter the selected levels with Red Scrolls, clear + walk to the exit, "
                  "then re-enter the next. Keep \"Exit portal\" on so it can leave.",
                  &s.an_reg_loop);
        static const char* an_lvl_names[5] = { "Lvl 1", "Lvl 2", "Lvl 3", "Lvl 4", "Lvl 5" };
        ChipRow(an_lvl_names, s.an_reg_levels, 5);

        char st[48];
        ImFormatString(st, sizeof(st), "%d", s.an_deep_scrolls);                       StatLabel("Blood Scrolls", st);
        ImFormatString(st, sizeof(st), "%d", s.an_deep_runs);                          StatLabel("Deep runs",     st);
        ImFormatString(st, sizeof(st), "%d", s.an_reg_scrolls);                        StatLabel("Red Scrolls",   st);
        ImFormatString(st, sizeof(st), "%d (lvl %d)", s.an_reg_runs, s.an_reg_level);  StatLabel("Level runs",    st);
        if (s.an_deep_msg) HelpText(s.an_deep_msg);
        if (s.an_reg_msg)  HelpText(s.an_reg_msg);
    EndGroup();

    GroupTitle("Pathfinder");
    BeginGroup();
        DrawerRow("arrow", "Pathfinder", "Move tuning, body fit, visuals.", "nether-pathfinder");
    EndGroup();
}

}

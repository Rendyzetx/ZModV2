#include "../Internal.h"

namespace ZmodUI {

void RailCombat(State& s) {
    PageHead("Combat & Survival", "Damage avoidance, anti-trap, AFK protection.");

    GroupTitle("Defense");
    BeginGroup();
        ToggleRow("shield", "God Mode", "Absorbs incoming damage.", &s.god_mode);
        ToggleRow("bandage-orange", "Anti-Trap", "Ignore deflectors, wind, mushrooms.", &s.anti_trap);
        ToggleRow("poison-green",   "Anti-Poison", "Block poison status effects.", &s.anti_poison);
        ToggleRow("spring-gray", "Anti-Bounce", "Cancel spring blocks.", &s.anti_bounce);
        ToggleRow("fan-teal", "Anti-Fan", nullptr, &s.anti_fan);
        ToggleRow("diamond-indigo", "Anti-Deflector", nullptr, &s.anti_deflector);

    EndGroup();

    GroupTitle("Survival");
    BeginGroup();
        ToggleRow("database-green", "Anti Block-Death", nullptr, &s.anti_block_death);
        ToggleRow("lung-teal", "Unlimited Oxygen", nullptr, &s.unlimited_oxygen);
        ToggleRow("sleep-pink", "Anti-AFK", "Keeps you active.", &s.anti_afk);
    EndGroup();

    GroupTitle("Offense");
    BeginGroup();
        ToggleRow("aim", "AI Aimbot", "Auto-aim at hostile AI.", &s.ai_aimbot);
        ToggleRow("block-red", "Place Block on Player", "Drop block at target's feet.", &s.block_on_player);
    EndGroup();

}

}

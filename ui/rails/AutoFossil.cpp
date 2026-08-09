#include "../Internal.h"
#include <cstdio>

namespace ZmodUI {

void RailAutoFossil(State& s) {
    PageHead("Auto Fossil", "Auto-digs and solves the Archaeology fossil puzzle on a loop.");

    GroupTitle("General");
    BeginGroup();
        ToggleRow("spark-yellow", "Enable Auto Fossil",
                  "Digs a fossil from your inventory, solves the sliding puzzle, and "
                  "starts the next one. No need to open the puzzle UI.",
                  &s.ff_enabled);
    EndGroup();

    GroupTitle("Status");
    BeginGroup();
        char buf[32];
        StatLabel("State", s.ff_status);
        std::snprintf(buf, sizeof(buf), "%d", s.ff_solved); StatLabel("Completed", buf);
        std::snprintf(buf, sizeof(buf), "%d", s.ff_failed); StatLabel("Failed", buf);
        std::snprintf(buf, sizeof(buf), "%d", s.ff_moves);  StatLabel("Moves sent", buf);
    EndGroup();
}

}

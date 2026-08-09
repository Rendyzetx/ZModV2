#include "../Internal.h"

namespace ZmodUI {

void RailNetwork(State& s) {
    PageHead("Network", "Packet capture, repeat, ignore list.");

    GroupTitle("Capture");
    BeginGroup();
        ToggleRow("in",  "Capture Incoming", nullptr, &s.net_capture_in);
        ToggleRow("out", "Capture Outgoing", nullptr, &s.net_capture_out);
        ToggleRow("grid","Auto-scroll",      nullptr, &s.net_auto_scroll);
        static const char* dir_items[3] = {"All","Outgoing","Incoming"};
        ComboRow("Filter direction", dir_items, 3, &s.net_direction_filter);
        InputRow("Filter by packet ID", s.net_id_filter, sizeof(s.net_id_filter), "e.g. SendBson");
        DrawerRow("grid", "Open logger", "1,421 in \xC2\xB7 873 out", "logger");
    EndGroup();

    GroupTitle("Tools");
    BeginGroup();
        DrawerRow("out-orange",   "Packet Repeater", "Replay edited packets.", "repeater");
        DrawerRow("x-gray",       "Ignore List", "Packet IDs to skip when capturing.", "ignore");
    EndGroup();
}

}



#pragma once

namespace ZmodUI {
namespace Icons {

enum Id {
    None = 0,
    Dashboard, Shield, Arrow, Eye, Wifi, Dots, Pickaxe, Fish, Flame, Settings,
    SearchGlass, Chevron, Check, X,
    TrafficRed, TrafficYellow, TrafficGreen,
    Pulse, Spark, Streamer, Bolt, Bandage, Mushroom, Spring, Fan, Diamond,
    Lung, Sleep, Aim, Block,
    JumpUp, GravDown, Jetpack, KeyArrow, PathTri, PathCross,
    Camera, FreeCam, Crystal, Mining, AntiCollect, Magnet, Recipe,
    Cog, IdCard, Reload, LogOut, Refresh,
    ChestBank, Coin, RouteUp, ArrowIn, ArrowOut, Grid, Funnel,
    GiftBox, Treasure, FloorItem, Portal, Cancel,
    Trash, Database, Combat,
    Count
};

inline const char* Codepoint(Id id) {
    switch (id) {
        case Dashboard:    return "\xee\x8e\xa1";
        case Shield:       return "\xee\x8e\xa2";
        case Arrow:        return "\xee\x8e\xa3";
        case Eye:          return "\xee\x8e\xa4";
        case Wifi:         return "\xee\x8e\xa5";
        case Dots:         return "\xee\x8e\xa6";
        case Pickaxe:      return "\xee\x8e\xa7";
        case Fish:         return "\xee\x8e\xa8";
        case Flame:        return "\xee\x8e\xa9";
        case Settings:     return "\xee\x8e\xaa";
        case SearchGlass:  return "\xee\x8e\xab";
        case Chevron:      return "\xee\x8e\xac";
        case Check:        return "\xee\x8e\xad";
        case X:            return "\xee\x8e\xae";
        default:           return "";
    }
}

}
}



#pragma once

#include <string_view>

namespace ZmodUI {
namespace IconAtlasMap {

inline int Lookup(std::string_view icon_id) {
    if (icon_id.empty()) return 0;

    if (icon_id == "aim")              return 1765;
    if (icon_id == "arrow")            return 2008;
    if (icon_id == "arrow-green")      return 4288;
    if (icon_id == "bandage-orange")   return 1409;
    if (icon_id == "combat-red")       return 1131;
    if (icon_id == "dashboard")        return 1543;
    if (icon_id == "database-green")   return 1;
    if (icon_id == "database-indigo")  return 3588;
    if (icon_id == "diamond-indigo")   return 954;
    if (icon_id == "dots-brown")       return 1450;
    if (icon_id == "eye-orange")       return 2139;
    if (icon_id == "fan-teal")         return 120;
    if (icon_id == "fish-teal")        return 2499;
    if (icon_id == "flame-red")        return 1420;
    if (icon_id == "jetpack-green")    return 881;
    if (icon_id == "lung-teal")        return 18;
    if (icon_id == "magnet-green")     return 4162;
    if (icon_id == "pickaxe-indigo")   return 4098;
    if (icon_id == "poison-green")     return 957;
    if (icon_id == "portal")           return 3967;
    if (icon_id == "rain-purple")      return 523;
    if (icon_id == "recipe")           return 4093;
    if (icon_id == "reload-teal")      return 2983;
    if (icon_id == "settings-gray")    return 4274;
    if (icon_id == "shield")           return 2182;
    if (icon_id == "shield-red")       return 1131;
    if (icon_id == "spark-yellow")     return 524;
    if (icon_id == "spring-gray")      return 7;
    if (icon_id == "treasure-orange")  return 144;
    if (icon_id == "warn")             return 453;
    if (icon_id == "wifi-teal")        return 1980;
    return 0;
}

}
}

#pragma once

#include "imgui/imgui.h"

enum class ThemeStyle {
    Default,
    Oceanic,
    Crimson,
    EmeraldPower,
    CyberSunset,
    Daylight,
    Grayscale,
    PastelDream,
    Custom
};

inline ThemeStyle currentStyle = ThemeStyle::Default;

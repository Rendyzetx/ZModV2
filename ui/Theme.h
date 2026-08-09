
#pragma once
#include "imgui.h"

namespace ZmodUI {
namespace Theme {

enum Preset {
    LiquidGlass = 0, Dark, Light, Forest, Sunset, Ocean, Rose, Mono, Custom,
    PresetCount
};

void   SetPreset(Preset p);
Preset CurrentPreset();
const char* PresetName(Preset p);

bool   IsDarkMode();

bool   ShouldUseBackdropBlur();

enum PickerToken {
    Background = 0,
    Surface,
    TextPrimary,
    TextSubtle,
    Accent,
    Success,
    Danger,
    Warning,
    PickerTokenCount
};
void   SetPicker(PickerToken t, ImVec4 color);
ImVec4 GetPicker(PickerToken t);
const char* PickerTokenName(PickerToken t);
const char* PickerTokenDesc(PickerToken t);
void   ResetCustomToPreset(Preset src);

enum CustomToken {
    Bg = 0, BgSidebar, BgCard, BgHover, BgActive, BgPressed, BgPanelInner,
    Line, LineSoft,
    Text, TextSec, TextTert,
    AccentTok, AccentSoft,
    Green, Red, Orange, Yellow,
    CustomTokenCount
};
ImU32  Resolve(CustomToken t);
void   PollOsTheme();
void   Init();
float  ThemeFadeAlpha();

}
}

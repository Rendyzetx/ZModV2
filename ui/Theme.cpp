#include "Theme.h"
#include "Tokens.h"
#include <windows.h>
#include <cstring>

namespace ZmodUI {
namespace Theme {

namespace {

struct Palette {
    ImVec4 c[CustomTokenCount];
};

Preset  g_preset       = LiquidGlass;
bool    g_initial_set  = false;
float   g_fade_t       = 1.f;
Palette g_custom;
bool    g_custom_init  = false;
ImVec4  g_picker[PickerTokenCount];
bool    g_picker_init  = false;

static ImVec4 v(int r, int g, int b, int a = 255) {
    return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

static ImVec4 mix(ImVec4 a, ImVec4 b, float t) {
    return ImVec4(a.x + (b.x - a.x) * t,
                  a.y + (b.y - a.y) * t,
                  a.z + (b.z - a.z) * t,
                  a.w + (b.w - a.w) * t);
}
static ImVec4 lighten(ImVec4 c, float t) { return mix(c, ImVec4(1,1,1,c.w), t); }
static ImVec4 darken(ImVec4 c, float t)  { return mix(c, ImVec4(0,0,0,c.w), t); }
static bool   isDarkColor(ImVec4 c) { return (c.x + c.y + c.z) / 3.f < 0.5f; }

Palette PaletteDark() {
    Palette p;
    p.c[Bg]           = v(28,28,30);
    p.c[BgSidebar]    = v(37,37,39);
    p.c[BgCard]       = v(44,44,46);
    p.c[BgHover]      = v(52,52,58);
    p.c[BgActive]     = v(58,58,64);
    p.c[BgPressed]    = v(68,68,74);
    p.c[BgPanelInner] = v(38,38,41);
    p.c[Line]         = v(58,58,60);
    p.c[LineSoft]     = v(47,47,49);
    p.c[Text]         = v(245,245,247);
    p.c[TextSec]      = v(152,152,158);
    p.c[TextTert]     = v(108,108,112);
    p.c[AccentTok]    = v(64,156,255);
    p.c[AccentSoft]   = v(28,52,88);
    p.c[Green]        = v(48,209,88);
    p.c[Red]          = v(255,69,58);
    p.c[Orange]       = v(255,159,10);
    p.c[Yellow]       = v(255,214,10);
    return p;
}

Palette PaletteLight() {
    Palette p;
    p.c[Bg]           = v(245,245,247);
    p.c[BgSidebar]    = v(236,236,239);
    p.c[BgCard]       = v(255,255,255);
    p.c[BgHover]      = v(235,235,239);
    p.c[BgActive]     = v(228,228,234);
    p.c[BgPressed]    = v(216,216,223);
    p.c[BgPanelInner] = v(249,249,251);
    p.c[Line]         = v(216,216,222);
    p.c[LineSoft]     = v(232,232,237);
    p.c[Text]         = v(29,29,31);
    p.c[TextSec]      = v(110,110,115);
    p.c[TextTert]     = v(142,142,147);
    p.c[AccentTok]    = v(10,132,255);
    p.c[AccentSoft]   = v(227,240,255);
    p.c[Green]        = v(52,199,89);
    p.c[Red]          = v(255,59,48);
    p.c[Orange]       = v(255,149,0);
    p.c[Yellow]       = v(255,204,0);
    return p;
}

Palette PaletteLiquidGlass() {
    Palette p;

    p.c[Bg]           = ImVec4(10  / 255.f, 12  / 255.f, 16  / 255.f, 0.28f);
    p.c[BgSidebar]    = ImVec4(18  / 255.f, 20  / 255.f, 26  / 255.f, 0.38f);
    p.c[BgPanelInner] = ImVec4(14  / 255.f, 16  / 255.f, 22  / 255.f, 0.30f);

    p.c[BgCard]       = ImVec4(26  / 255.f, 28  / 255.f, 34  / 255.f, 0.55f);
    p.c[BgHover]      = ImVec4(48  / 255.f, 52  / 255.f, 62  / 255.f, 0.55f);
    p.c[BgActive]     = ImVec4(64  / 255.f, 68  / 255.f, 80  / 255.f, 0.60f);
    p.c[BgPressed]    = ImVec4(80  / 255.f, 84  / 255.f, 96  / 255.f, 0.65f);

    p.c[Line]         = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.16f);
    p.c[LineSoft]     = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.08f);

    p.c[Text]         = v(245, 247, 252);
    p.c[TextSec]      = v(192, 198, 215);
    p.c[TextTert]     = v(140, 148, 168);

    p.c[AccentTok]    = v(64, 156, 255);
    p.c[AccentSoft]   = ImVec4(64 / 255.f, 156 / 255.f, 255 / 255.f, 0.22f);
    p.c[Green]        = v(48, 209, 88);
    p.c[Red]          = v(255, 69, 58);
    p.c[Orange]       = v(255, 159, 10);
    p.c[Yellow]       = v(255, 214, 10);
    return p;
}

Palette PaletteForest() {
    Palette p;
    p.c[Bg]           = v(14,28,22);
    p.c[BgSidebar]    = v(18,36,28);
    p.c[BgCard]       = v(24,46,36);
    p.c[BgHover]      = v(32,58,46);
    p.c[BgActive]     = v(40,72,56);
    p.c[BgPressed]    = v(52,88,68);
    p.c[BgPanelInner] = v(20,40,32);
    p.c[Line]         = v(40,68,54);
    p.c[LineSoft]     = v(28,52,42);
    p.c[Text]         = v(232,244,236);
    p.c[TextSec]      = v(148,180,160);
    p.c[TextTert]     = v(108,140,120);
    p.c[AccentTok]    = v(132,204,22);
    p.c[AccentSoft]   = v(40,72,16);
    p.c[Green]        = v(74,222,128);
    p.c[Red]          = v(248,113,113);
    p.c[Orange]       = v(251,146,60);
    p.c[Yellow]       = v(250,204,21);
    return p;
}

Palette PaletteSunset() {
    Palette p;
    p.c[Bg]           = v(28,18,16);
    p.c[BgSidebar]    = v(38,24,20);
    p.c[BgCard]       = v(48,32,26);
    p.c[BgHover]      = v(62,40,32);
    p.c[BgActive]     = v(76,50,38);
    p.c[BgPressed]    = v(92,60,46);
    p.c[BgPanelInner] = v(34,22,18);
    p.c[Line]         = v(70,46,36);
    p.c[LineSoft]     = v(52,34,28);
    p.c[Text]         = v(252,242,232);
    p.c[TextSec]      = v(196,170,148);
    p.c[TextTert]     = v(144,124,108);
    p.c[AccentTok]    = v(251,146,60);
    p.c[AccentSoft]   = v(80,40,18);
    p.c[Green]        = v(163,230,53);
    p.c[Red]          = v(248,113,113);
    p.c[Orange]       = v(251,146,60);
    p.c[Yellow]       = v(250,204,21);
    return p;
}

Palette PaletteOcean() {
    Palette p;
    p.c[Bg]           = v(8,18,32);
    p.c[BgSidebar]    = v(12,24,42);
    p.c[BgCard]       = v(18,34,56);
    p.c[BgHover]      = v(24,44,72);
    p.c[BgActive]     = v(32,56,90);
    p.c[BgPressed]    = v(42,70,110);
    p.c[BgPanelInner] = v(12,26,46);
    p.c[Line]         = v(36,58,90);
    p.c[LineSoft]     = v(22,40,68);
    p.c[Text]         = v(232,242,252);
    p.c[TextSec]      = v(140,170,210);
    p.c[TextTert]     = v(100,130,170);
    p.c[AccentTok]    = v(34,211,238);
    p.c[AccentSoft]   = v(20,52,80);
    p.c[Green]        = v(74,222,128);
    p.c[Red]          = v(244,63,94);
    p.c[Orange]       = v(251,146,60);
    p.c[Yellow]       = v(250,204,21);
    return p;
}

Palette PaletteRose() {
    Palette p;
    p.c[Bg]           = v(28,18,24);
    p.c[BgSidebar]    = v(38,24,32);
    p.c[BgCard]       = v(48,30,40);
    p.c[BgHover]      = v(62,40,52);
    p.c[BgActive]     = v(76,50,64);
    p.c[BgPressed]    = v(92,60,78);
    p.c[BgPanelInner] = v(34,22,28);
    p.c[Line]         = v(70,46,58);
    p.c[LineSoft]     = v(52,34,44);
    p.c[Text]         = v(252,232,240);
    p.c[TextSec]      = v(200,160,180);
    p.c[TextTert]     = v(150,120,135);
    p.c[AccentTok]    = v(244,114,182);
    p.c[AccentSoft]   = v(80,30,52);
    p.c[Green]        = v(74,222,128);
    p.c[Red]          = v(244,63,94);
    p.c[Orange]       = v(251,146,60);
    p.c[Yellow]       = v(250,204,21);
    return p;
}

Palette PaletteMono() {
    Palette p;
    p.c[Bg]           = v(18,18,18);
    p.c[BgSidebar]    = v(26,26,26);
    p.c[BgCard]       = v(34,34,34);
    p.c[BgHover]      = v(46,46,46);
    p.c[BgActive]     = v(58,58,58);
    p.c[BgPressed]    = v(72,72,72);
    p.c[BgPanelInner] = v(22,22,22);
    p.c[Line]         = v(56,56,56);
    p.c[LineSoft]     = v(40,40,40);
    p.c[Text]         = v(245,245,245);
    p.c[TextSec]      = v(160,160,160);
    p.c[TextTert]     = v(115,115,115);
    p.c[AccentTok]    = v(245,245,245);
    p.c[AccentSoft]   = v(60,60,60);
    p.c[Green]        = v(200,200,200);
    p.c[Red]          = v(225,225,225);
    p.c[Orange]       = v(210,210,210);
    p.c[Yellow]       = v(220,220,220);
    return p;
}

Palette FixedPalette(Preset p) {
    switch (p) {
        case Dark:        return PaletteDark();
        case Light:       return PaletteLight();
        case LiquidGlass: return PaletteLiquidGlass();
        case Forest:      return PaletteForest();
        case Sunset:      return PaletteSunset();
        case Ocean:       return PaletteOcean();
        case Rose:        return PaletteRose();
        case Mono:        return PaletteMono();
        case Custom:      return g_custom;
        default:          return PaletteDark();
    }
}

Palette Derive(const ImVec4 picker[PickerTokenCount]) {
    Palette p;
    ImVec4 bg     = picker[Background];
    ImVec4 surf   = picker[Surface];
    ImVec4 txt    = picker[TextPrimary];
    ImVec4 sub    = picker[TextSubtle];
    ImVec4 ac     = picker[Accent];
    ImVec4 grn    = picker[Success];
    ImVec4 red    = picker[Danger];
    ImVec4 warn   = picker[Warning];
    bool dark = isDarkColor(bg);
    float k = dark ? 0.08f : 0.04f;

    p.c[Bg]           = bg;
    p.c[BgSidebar]    = dark ? lighten(bg, 0.04f) : darken(bg, 0.03f);
    p.c[BgPanelInner] = dark ? lighten(bg, 0.02f) : lighten(bg, 0.02f);
    p.c[BgCard]       = surf;
    p.c[BgHover]      = dark ? lighten(surf, k)      : darken(surf, k);
    p.c[BgActive]     = dark ? lighten(surf, k*2)    : darken(surf, k*2);
    p.c[BgPressed]    = dark ? lighten(surf, k*3)    : darken(surf, k*3);
    p.c[Line]         = dark ? lighten(surf, 0.12f)  : darken(surf, 0.10f);
    p.c[LineSoft]     = dark ? lighten(surf, 0.06f)  : darken(surf, 0.05f);
    p.c[Text]         = txt;
    p.c[TextSec]      = sub;
    p.c[TextTert]     = dark ? darken(sub, 0.25f) : lighten(sub, 0.25f);
    p.c[AccentTok]    = ac;
    p.c[AccentSoft]   = dark ? darken(ac, 0.65f) : lighten(ac, 0.75f);
    p.c[Green]        = grn;
    p.c[Red]          = red;
    p.c[Orange]       = warn;
    p.c[Yellow]       = lighten(warn, 0.15f);
    return p;
}

void SeedPickerFromPreset(Preset src) {
    Palette p = FixedPalette(src);
    g_picker[Background]  = p.c[Bg];
    g_picker[Surface]     = p.c[BgCard];
    g_picker[TextPrimary] = p.c[Text];
    g_picker[TextSubtle]  = p.c[TextSec];
    g_picker[Accent]      = p.c[AccentTok];
    g_picker[Success]     = p.c[Green];
    g_picker[Danger]      = p.c[Red];
    g_picker[Warning]     = p.c[Orange];
    g_picker_init = true;
}

}

void Init() {
    g_custom = PaletteDark();
    g_custom_init = true;
    SeedPickerFromPreset(Dark);
    PollOsTheme();
}

void SetPreset(Preset p) {
    if (g_preset != p) { g_preset = p; g_fade_t = 0.f; }
    if (p != Custom) SeedPickerFromPreset(p);
}
Preset CurrentPreset() { return g_preset; }

const char* PresetName(Preset p) {

    static const char* names[PresetCount] = {
        "Liquid Glass","Dark","Light","Forest","Sunset","Ocean","Rose","Mono","Custom"
    };
    if (p < 0 || p >= PresetCount) return "";
    return names[p];
}

bool ShouldUseBackdropBlur() {
    return g_preset == LiquidGlass;
}

bool IsDarkMode() {
    Palette p = FixedPalette(g_preset);
    return isDarkColor(p.c[Bg]);
}

void PollOsTheme() {
    if (g_initial_set) return;
    SeedPickerFromPreset(g_preset);
    g_initial_set = true;
}

void SetPicker(PickerToken t, ImVec4 color) {
    if (!g_picker_init) SeedPickerFromPreset(Dark);
    if (t < 0 || t >= PickerTokenCount) return;
    g_picker[t] = color;
    g_custom = Derive(g_picker);
    if (g_preset != Custom) { g_preset = Custom; g_fade_t = 0.f; }
}
ImVec4 GetPicker(PickerToken t) {
    if (!g_picker_init) SeedPickerFromPreset(g_preset);
    if (t < 0 || t >= PickerTokenCount) return ImVec4(0,0,0,1);
    return g_picker[t];
}

const char* PickerTokenName(PickerToken t) {
    static const char* names[PickerTokenCount] = {
        "Background","Surface","Text","Subtext",
        "Accent","Success","Danger","Warning"
    };
    if (t < 0 || t >= PickerTokenCount) return "";
    return names[t];
}
const char* PickerTokenDesc(PickerToken t) {
    static const char* d[PickerTokenCount] = {
        "Window background + sidebar",
        "Cards, hover, borders",
        "Headings and body text",
        "Labels and captions",
        "Buttons, toggles, focus",
        "Online / OK indicators",
        "Errors and offline",
        "Warnings and alerts"
    };
    if (t < 0 || t >= PickerTokenCount) return "";
    return d[t];
}

void ResetCustomToPreset(Preset src) {
    SeedPickerFromPreset(src);
    g_custom = FixedPalette(src);
    g_preset = Custom;
    g_fade_t = 0.f;
}

ImU32 Resolve(CustomToken t) {
    if (!g_custom_init) Init();
    Palette p = FixedPalette(g_preset);
    return ImGui::ColorConvertFloat4ToU32(p.c[t]);
}

float ThemeFadeAlpha() {
    float dt = ImGui::GetIO().DeltaTime;
    if (g_fade_t < 1.f) {
        g_fade_t += dt * 1000.f / Tokens::Motion::PaneFadeMs;
        if (g_fade_t > 1.f) g_fade_t = 1.f;
    }
    return g_fade_t;
}

}
}

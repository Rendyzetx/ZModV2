#include "Tokens.h"
#include "Theme.h"
#include "Fonts.h"
#include <cmath>

namespace ZmodUI {
namespace Tokens {

namespace Color {
    ImU32 Bg()           { return Theme::Resolve(Theme::Bg); }
    ImU32 BgSidebar()    { return Theme::Resolve(Theme::BgSidebar); }
    ImU32 BgCard()       { return Theme::Resolve(Theme::BgCard); }
    ImU32 BgHover()      { return Theme::Resolve(Theme::BgHover); }
    ImU32 BgActive()     { return Theme::Resolve(Theme::BgActive); }
    ImU32 BgPressed()    { return Theme::Resolve(Theme::BgPressed); }
    ImU32 BgPanelInner() { return Theme::Resolve(Theme::BgPanelInner); }
    ImU32 Line()         { return Theme::Resolve(Theme::Line); }
    ImU32 LineSoft()     { return Theme::Resolve(Theme::LineSoft); }
    ImU32 Text()         { return Theme::Resolve(Theme::Text); }
    ImU32 TextSec()      { return Theme::Resolve(Theme::TextSec); }
    ImU32 TextTert()     { return Theme::Resolve(Theme::TextTert); }
    ImU32 Accent()       { return Theme::Resolve(Theme::AccentTok); }
    ImU32 AccentSoft()   { return Theme::Resolve(Theme::AccentSoft); }
    ImU32 Green()        { return Theme::Resolve(Theme::Green); }
    ImU32 Red()          { return Theme::Resolve(Theme::Red); }
    ImU32 Orange()       { return Theme::Resolve(Theme::Orange); }
    ImU32 Yellow()       { return Theme::Resolve(Theme::Yellow); }
    ImU32 ShadowSoft()   { return IM_COL32(0, 0, 0, Theme::IsDarkMode() ? 100 : 28); }
}

namespace Type {
    ImFont* H1()      { return Fonts::Get(Fonts::F_H1); }
    ImFont* H2()      { return Fonts::Get(Fonts::F_H2); }
    ImFont* H3()      { return Fonts::Get(Fonts::F_H3); }
    ImFont* Body()    { return Fonts::Get(Fonts::F_Body); }
    ImFont* BodySec() { return Fonts::Get(Fonts::F_BodySec); }
    ImFont* Mono()    { return Fonts::Get(Fonts::F_Mono); }
    ImFont* Eyebrow() { return Fonts::Get(Fonts::F_Eyebrow); }
    ImFont* Icon()    { return Fonts::Get(Fonts::F_Icon); }
}

namespace Motion {
    static float clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }

    float MacEase(float t) {
        t = clamp01(t);
        float x = t;
        for (int i = 0; i < 6; ++i) {
            float bx = 3*0.32f*(1-x)*(1-x)*x + 3*0.0f*(1-x)*x*x + x*x*x;
            float dbx = 3*0.32f*(1-3*x+2*x*x) + 3*0.0f*(2*x - 3*x*x) + 3*x*x;
            float d = bx - t;
            if (std::fabs(dbx) < 1e-6f) break;
            x -= d / dbx;
            x = clamp01(x);
        }
        float p = x;
        float y = 3*0.72f*(1-p)*(1-p)*p + 3*1.0f*(1-p)*p*p + p*p*p;
        return clamp01(y);
    }

    float EaseOut(float t) {
        t = clamp01(t);
        float u = 1.f - t;
        return 1.f - u*u*u;
    }
}

}
}

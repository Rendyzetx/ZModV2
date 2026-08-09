

#pragma once
#include "imgui.h"

namespace ZmodUI {
namespace Tokens {

namespace Color {
    ImU32 Bg();              ImU32 BgSidebar();      ImU32 BgCard();
    ImU32 BgHover();         ImU32 BgActive();       ImU32 BgPressed();
    ImU32 BgPanelInner();
    ImU32 Line();            ImU32 LineSoft();
    ImU32 Text();            ImU32 TextSec();        ImU32 TextTert();
    ImU32 Accent();          ImU32 AccentSoft();
    ImU32 Green();           ImU32 Red();
    ImU32 Orange();          ImU32 Yellow();
    ImU32 ShadowSoft();
}

namespace Size {
    inline constexpr float WindowAspect    = 1080.f / 680.f;
    inline constexpr float WindowMinW      = 720.f;
    inline constexpr float WindowDefaultW  = 1080.f;
    inline constexpr float TitleBarH       = 38.f;
    inline constexpr float SidebarW        = 220.f;
    inline constexpr float StatusStripH    = 38.f;
    inline constexpr float DrawerW         = 380.f;
    inline constexpr float RowMinH         = 44.f;
    inline constexpr float RowPadV         = 11.f;
    inline constexpr float RowPadH         = 14.f;
    inline constexpr float GroupMarginB    = 22.f;
    inline constexpr float CardRadius      = 10.f;
    inline constexpr float WindowRadius    = 12.f;
    inline constexpr float IconTileSize    = 26.f;
    inline constexpr float SidebarIconTile = 22.f;
    inline constexpr float SwitchW         = 38.f;
    inline constexpr float SwitchH         = 22.f;
    inline constexpr float SwitchKnob      = 18.f;
    inline constexpr float SliderW         = 180.f;
    inline constexpr float SliderTrackH    = 4.f;
    inline constexpr float SliderKnob      = 20.f;
    inline constexpr float ChipPadV        = 4.f;
    inline constexpr float ChipPadH        = 10.f;
    inline constexpr float ChipRadius      = 999.f;
    inline constexpr float EspCellW        = 32.f;
    inline constexpr float EspCellH        = 22.f;
    inline constexpr float ColorSwatchW    = 22.f;
    inline constexpr float ColorSwatchH    = 14.f;
}

namespace Type {
    ImFont* H1();
    ImFont* H2();
    ImFont* H3();
    ImFont* Body();
    ImFont* BodySec();
    ImFont* Mono();
    ImFont* Eyebrow();
    ImFont* Icon();
}

namespace Motion {
    inline constexpr float SwitchMs    = 220.f;
    inline constexpr float SliderMs    = 120.f;
    inline constexpr float DrawerMs    = 360.f;
    inline constexpr float PaneFadeMs  = 320.f;
    inline constexpr float HoverMs     = 160.f;
    inline constexpr float ToastMs     = 280.f;
    inline constexpr float BootTotalMs = 2400.f;

    float MacEase(float t);
    float EaseOut(float t);
}

}
}

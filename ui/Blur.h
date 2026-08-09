

#pragma once

#include <d3d11.h>
#include <dxgiformat.h>
#include "imgui/imgui.h"

namespace ZmodUI {
namespace Blur {

bool IsReady();

bool EnsureInit(ID3D11Device* dev, ID3D11DeviceContext* ctx,
                int backbuffer_w, int backbuffer_h,
                DXGI_FORMAT backbuffer_format);

void OnResize(int new_w, int new_h);

void Shutdown();

void Capture(ID3D11Texture2D* backbufferTex);

ImTextureID GetSrv();

inline ImVec2 ScreenToUV(ImVec2 screen_pos, ImVec2 display_size) {
    if (display_size.x <= 0.f || display_size.y <= 0.f) return ImVec2(0.f, 0.f);
    return ImVec2(screen_pos.x / display_size.x,
                  screen_pos.y / display_size.y);
}

}
}

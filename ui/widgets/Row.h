

#pragma once
#include "imgui.h"

namespace ZmodUI {
namespace WRow {

struct Opts {
    const char* icon_id   = nullptr;
    const char* label     = nullptr;
    const char* desc      = nullptr;
    bool        no_icon   = false;
    bool        no_hover  = true;
    bool        is_drawer = false;
    float       row_h     = 0.f;
};

struct RowResult {
    ImVec2 a, b;
    ImVec2 end_a, end_b;
    bool   hovered;
    bool   clicked;
};

RowResult Begin(const Opts& o);

ImVec2 EndSlot(const RowResult& r, float width, float height);

struct IconTone { ImU32 bg, fg; int icon_id; int atlas_block_id; };
IconTone ResolveIcon(const char* icon_id);

}
}

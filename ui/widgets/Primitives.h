
#pragma once
#include "imgui.h"

namespace ZmodUI {
namespace Prim {

void DrawCard(ImDrawList* dl, ImVec2 a, ImVec2 b, ImU32 bg, ImU32 line, float radius);
void DrawChevron(ImDrawList* dl, ImVec2 center, float size, ImU32 col);
void DrawCheckmark(ImDrawList* dl, ImVec2 center, float size, ImU32 col);
void DrawSpinner(ImDrawList* dl, ImVec2 center, float radius, ImU32 col, float t);

}
}

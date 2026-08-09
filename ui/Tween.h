

#pragma once
#include "imgui.h"

namespace ZmodUI {
namespace Tween {

float ToFloat(ImGuiID id, float target, float speed = 8.f);
float MacEase(float t);
float EaseOut(float t);

void  Forget(ImGuiID id);

float SmoothStep(float current, float target, float speed);

}
}

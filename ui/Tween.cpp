#include "Tween.h"
#include "Tokens.h"
#include <unordered_map>
#include <cmath>

namespace ZmodUI {
namespace Tween {

namespace {
    std::unordered_map<ImGuiID, float>& Store() {
        static std::unordered_map<ImGuiID, float> s;
        return s;
    }
}

float SmoothStep(float current, float target, float speed) {
    float dt = ImGui::GetIO().DeltaTime;
    if (dt <= 0.f) return current;
    float k = 1.f - std::exp(-speed * dt);
    return current + (target - current) * k;
}

float ToFloat(ImGuiID id, float target, float speed) {
    auto& s = Store();
    auto it = s.find(id);
    float cur = (it == s.end()) ? target : it->second;
    float nxt = SmoothStep(cur, target, speed);
    if (std::fabs(nxt - target) < 1e-4f) nxt = target;
    s[id] = nxt;
    return nxt;
}

void Forget(ImGuiID id) {
    auto& s = Store();
    s.erase(id);
}

float MacEase(float t) { return Tokens::Motion::MacEase(t); }
float EaseOut(float t) { return Tokens::Motion::EaseOut(t); }

}
}

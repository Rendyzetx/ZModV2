
#pragma once
#include "imgui.h"

namespace ZmodUI {
namespace Fonts {

enum Slot {
    F_H1 = 0,
    F_H2,
    F_H3,
    F_Body,
    F_BodySec,
    F_Mono,
    F_Eyebrow,
    F_Icon,
    F_Count
};

void    Init();
void    Shutdown();
ImFont* Get(Slot s);
bool    HasIconFont();

}
}



#pragma once
#include "ZmodUI.h"

namespace ZmodBindings {

extern ZmodUI::State g_uiState;

void Init();
void SyncFromBacking();
void ApplyToBacking();

}

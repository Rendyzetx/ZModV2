#pragma once
#include "imgui.h"
#include "../ZmodUI.h"

namespace ZmodUI {
namespace DrawerBody {

void Render(const char* key, State& s);

void Pathfinder(State& s);
void KeysToFly(State& s);
void UncapFps(State& s);
void MinePathfinder(State& s);
void MineTargets(State& s);
void MineCombat(State& s);
void MineBankBot(State& s);
void MineAutoSell(State& s);
void MineRejoin(State& s);
void EspRow(State& s);
void Logger(State& s);
void Repeater(State& s);
void Ignore(State& s);
void CustomColors(State& s);
void NetherPathfinder(State& s);
void DbgInvPicker(State& s);

const char* TitleFor(const char* key);

}
}

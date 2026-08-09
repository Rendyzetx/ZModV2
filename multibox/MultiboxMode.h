

#pragma once

#include <Windows.h>
#include <atomic>
#include <iostream>

#include "DllAuth.h"
#include "../il2cpp/Manifest.h"
#include "XorStr.h"

namespace MultiboxMode {

typedef void (*t_UI_LogOut)();
typedef void (*t_SL_ReloadGame)();

inline t_UI_LogOut     g_UI_LogOut     = nullptr;
inline t_SL_ReloadGame g_SL_ReloadGame = nullptr;

inline std::atomic<bool> g_resolved{ false };

inline bool ResolveMethods() {
    if (g_resolved.load()) return true;

    if (!g_UI_LogOut)
        g_UI_LogOut = (t_UI_LogOut)Manifest::GetMethod("UserIdent", "LogOut");
    if (!g_SL_ReloadGame)
        g_SL_ReloadGame = (t_SL_ReloadGame)Manifest::GetMethod("SceneLoader", "ReloadGame");

    if (!g_UI_LogOut || !g_SL_ReloadGame) {
        auto* uiCls = IL2CPP::Class::Find("UserIdent");
        auto* slCls = IL2CPP::Class::Find("SceneLoader");
        if (uiCls && !g_UI_LogOut)
            g_UI_LogOut = (t_UI_LogOut)IL2CPP::Class::Utils::GetMethodPointer(uiCls, "LogOut", 0);
        if (slCls && !g_SL_ReloadGame)
            g_SL_ReloadGame = (t_SL_ReloadGame)IL2CPP::Class::Utils::GetMethodPointer(slCls, "ReloadGame", 0);
    }

    if (!g_UI_LogOut && !g_SL_ReloadGame) {
        std::cout << XS("[Multibox] Resolve: zero UserIdent / SceneLoader "
                     "methods found (manifest + runtime both empty)") << std::endl;
        return false;
    }
    g_resolved.store(true);
    std::cout << XS("[Multibox] Resolved (LogOut=") << (void*)g_UI_LogOut
              << XS(", ReloadGame=") << (void*)g_SL_ReloadGame
              << XS(")") << std::endl;
    return true;
}

inline void InstallHooks() {

}

inline bool LogoutCurrent() {

    ResolveMethods();

    if (g_UI_LogOut) g_UI_LogOut();
    if (g_SL_ReloadGame) { g_SL_ReloadGame(); return true; }
    if (oReloadGame)     { oReloadGame();    return true; }
    return false;
}

}


#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_4.h>
#include <dwmapi.h>
#include <dcomp.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dcomp.lib")
#include <iostream>
#include <string>
#include <vector> 
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "IL2CPP_Resolver/kiero/kiero.h"
#include "IL2CPP_Resolver/kiero/minhook/include/MinHook.h"

#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <wininet.h>
#include <Lmcons.h>
#include <chrono>
#include "includes.h" 
#include "Styles.h"  
#include "main.h"    
#include <sstream>     
#include <iomanip>     
#include <cctype>    
#include <fstream>
#include <regex>
#include <Wincrypt.h>
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Winhttp.lib") 
#include <unordered_set>
#include <set>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

#include <algorithm>
#include <cmath>
#include <cstdio>
#define NOMINMAX
#include <chrono>
#include "MineScheduler.h"
#include "SpriteAtlas.h"
#include "minimap.h"

std::set<void*> countedPlayers;

enum class NotificationType {
    Info,
    Success,
    Warning,
    Error
};

struct Notification {
    int id;
    std::string title;
    std::string message;
    std::chrono::steady_clock::time_point creation_time;
    std::chrono::duration<float> duration;
    NotificationType type;
};

void ShowNotification(const std::string& title,
                      const std::string& message,
                      NotificationType   type     = NotificationType::Info,
                      float              duration = 5.0f);

#include "Commands.h"

#include "Store.h"

typedef HRESULT(__stdcall* PresentFnPtr)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WndProcFnPtr)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

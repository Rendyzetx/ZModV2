

#define IL2CPP_ASSERT(x) ((void)0)

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <shlobj.h>
#include <sstream>
#include <chrono>

#include "includes.h"
#include <Lmcons.h>
#include <gdiplus.h>
#include "Styles.h"
#include "main.h"
#include "esp.h"
#include <algorithm>

#include "MultiboxMode.h"
#include "MultiboxProfile.h"
#define NOMINMAX
#pragma comment(lib, "gdiplus.lib")
#include <shellapi.h>
#include "IL2CPP_Resolver/IL2CPP_Resolver.hpp"
#include "functions.h"
#include "PathFinding.h"
#include "AutoNether.h"
#include "Pnb.h"
#include "AutoMine.h"

#include "BankBot.h"
#include "Config.h"

#include "XorStr.h"
#include "DllAuth.h"
#include "ACTkBypass.h"
#include "Sentry.h"
#include "Stealth.h"
#include "DllAuthClient.h"
#include "KickLogger.h"
#include "CrashLogger.h"
#ifdef ZMOD_V3

#include "LogStream.h"
#endif
#include "BootState.h"
#include "Manifest.h"
#include "ui_bindings.h"
#include "AuthBanner.h"
#include "Blur.h"

inline void OpenURL(const char* url)
{
    ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

static char key_input[64] = "";
static std::string login_message = "";

bool show_imgui_menu = true;

#include "Theme.h"

#include "ui/Theme.h"

inline void SeedCustomThemeFrom(ThemeStyle preset) {

    switch (preset) {
    case ThemeStyle::Default:      ApplyCustomStyleDefault(); break;
    case ThemeStyle::Oceanic:      ApplyCustomStyle1();        break;
    case ThemeStyle::Crimson:      ApplyCustomStyle2();        break;
    case ThemeStyle::EmeraldPower: ApplyEmeraldPowerStyle();   break;
    case ThemeStyle::CyberSunset:  ApplyCyberSunsetStyle();    break;
    case ThemeStyle::Daylight:     ApplyDaylightStyle();       break;
    case ThemeStyle::Grayscale:    ApplyGrayscaleStyle();      break;
    case ThemeStyle::PastelDream:  ApplyPastelDreamStyle();    break;
    default:                       ApplyCustomStyleDefault();  break;
    }
    InitCustomThemeFromCurrent();
}

std::string username_str = "";
std::string windows_username_str = "User";
HWND window_handle = nullptr;
WNDPROC original_wnd_proc = nullptr;
ID3D11Device* d3d_device = nullptr;
ID3D11DeviceContext* d3d_context = nullptr;
ID3D11RenderTargetView* main_render_target_view = nullptr;
bool imgui_initialized = false;
typedef HRESULT(__stdcall* Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
Present original_present = nullptr;

typedef HRESULT(__stdcall* tPresent1Fwd)     (IDXGISwapChain1*, UINT, UINT, const DXGI_PRESENT_PARAMETERS*);
typedef HRESULT(__stdcall* tResizeBuffersFwd)(IDXGISwapChain*,  UINT, UINT, UINT, DXGI_FORMAT, UINT);
typedef void   (__stdcall* tExecuteCmdsFwd)  (ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
inline tPresent1Fwd      g_originalPresent1    = nullptr;
inline tResizeBuffersFwd g_originalResize      = nullptr;
inline tExecuteCmdsFwd   g_originalExecuteCmds = nullptr;
inline ID3D12Device*       g_d3d12Device     = nullptr;
inline ID3D12CommandQueue* g_d3d12Queue      = nullptr;

inline bool                g_d3d11Native    = false;
inline IDXGISwapChain*     g_swapChainBase  = nullptr;
inline ID3D11On12Device*   g_d3d11on12       = nullptr;
inline IDXGISwapChain3*    g_swapChain3      = nullptr;
inline UINT                g_backBufferCount = 0;
struct WrappedBackBuffer {
    ID3D12Resource*         native  = nullptr;
    ID3D11Resource*         wrapped = nullptr;
    ID3D11RenderTargetView* rtv     = nullptr;
};
inline std::vector<WrappedBackBuffer> g_backBuffers;
inline volatile LONG g_present1Hits = 0;
inline volatile LONG g_resizeHits   = 0;
inline volatile LONG g_execHits     = 0;

typedef void(__fastcall* tSetTargetFrameRate)(int value);
typedef void(__fastcall* tSetVSyncCount)(int value);
inline tSetTargetFrameRate o_SetTargetFrameRate = nullptr;
inline tSetVSyncCount      o_SetVSyncCount      = nullptr;
bool g_should_exit_thread = false;
bool g_kiero_initialized_successfully = false;
uintptr_t g_game_module_base_for_sigmahook = 0;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::string ExtractJsonValue(const std::string& json, const std::string& key) {
    std::string search_key = "\"" + key + "\"";
    size_t p = json.find(search_key);
    if (p == std::string::npos) return "";
    size_t colon = json.find(':', p);
    if (colon == std::string::npos) return "";
    size_t firstQuote = json.find('\"', colon + 1);
    if (firstQuote == std::string::npos) return "";
    size_t secondQuote = json.find('\"', firstQuote + 1);
    if (secondQuote == std::string::npos) return "";
    return json.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

std::string GetWindowsUsername() {
    char username_buffer[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username_buffer, &username_len)) {
        return std::string(username_buffer);
    }
    return "Valued User";
}

std::string GetHWID() {
    DWORD serialNumber = 0;
    if (GetVolumeInformationA("C:\\", nullptr, 0, &serialNumber, nullptr, nullptr, nullptr, 0))
        return std::to_string(serialNumber);
    return "HWID_ERROR";
}

ImVec4 GetRainbowColor(float speed = 1.0f) {
    float t = ImGui::GetTime() * speed;
    float r = 0.5f + 0.5f * sin(t);
    float g = 0.5f + 0.5f * sin(t + 2.094395f);
    float b = 0.5f + 0.5f * sin(t + 4.188790f);
    return ImVec4(r, g, b, 1.0f);
}

inline std::vector<Notification> g_notifications;
inline std::mutex g_notification_mutex;
inline int g_notification_counter = 0;

void ShowNotification(const std::string& title, const std::string& message, NotificationType type, float duration_seconds) {
    std::lock_guard<std::mutex> lock(g_notification_mutex);
    g_notifications.push_back({
        g_notification_counter++,
        title,
        message,
        std::chrono::steady_clock::now(),
        std::chrono::duration<float>(duration_seconds),
        type
        });
}

static inline ImU32 NotifFade(ImU32 col, float t) {
    if (t >= 1.f) return col;
    if (t <= 0.f) return col & 0x00FFFFFF;
    int a = (int)(((col >> 24) & 0xFF) * t);
    if (a < 0) a = 0; if (a > 255) a = 255;
    return (col & 0x00FFFFFF) | ((ImU32)a << 24);
}

static inline ImU32 NotifMix(ImU32 a, ImU32 b, float t) {
    auto ch = [&](int sh) {
        int x = (a >> sh) & 0xFF, y = (b >> sh) & 0xFF;
        int v = (int)(x + (y - x) * t);
        if (v < 0) v = 0; if (v > 255) v = 255;
        return (ImU32)v << sh;
    };
    return ch(0) | ch(8) | ch(16) | ch(24);
}

static inline void NotifSanitize(const std::string& in, std::string& out) {
    out.clear();
    out.reserve(in.size());
    for (size_t i = 0; i < in.size();) {
        unsigned char c0 = (unsigned char)in[i];
        if (c0 == 0xE2 && i + 2 < in.size() &&
            (unsigned char)in[i+1] == 0x80 &&
            (unsigned char)in[i+2] == 0xA6) {
            out.append("...");
            i += 3;
        } else {
            out.push_back(in[i]);
            ++i;
        }
    }
}

enum class NotifIcon { Info, Success, Warning, Error };

static inline void DrawNotifGlyph(ImDrawList* dl, ImVec2 c, float r,
                                   NotifIcon kind, ImU32 col) {
    const float th = 2.2f;
    switch (kind) {
        case NotifIcon::Success: {

            dl->PathClear();
            dl->PathLineTo(ImVec2(c.x - r*0.45f, c.y + r*0.05f));
            dl->PathLineTo(ImVec2(c.x - r*0.10f, c.y + r*0.40f));
            dl->PathLineTo(ImVec2(c.x + r*0.50f, c.y - r*0.30f));
            dl->PathStroke(col, 0, th);
        } break;
        case NotifIcon::Warning: {

            dl->AddLine(ImVec2(c.x, c.y - r*0.50f),
                        ImVec2(c.x, c.y + r*0.18f), col, th);
            dl->AddCircleFilled(ImVec2(c.x, c.y + r*0.45f), 1.7f, col);
        } break;
        case NotifIcon::Error: {

            dl->AddLine(ImVec2(c.x - r*0.42f, c.y - r*0.42f),
                        ImVec2(c.x + r*0.42f, c.y + r*0.42f), col, th);
            dl->AddLine(ImVec2(c.x + r*0.42f, c.y - r*0.42f),
                        ImVec2(c.x - r*0.42f, c.y + r*0.42f), col, th);
        } break;
        case NotifIcon::Info:
        default: {

            dl->AddCircleFilled(ImVec2(c.x, c.y - r*0.45f), 1.7f, col);
            dl->AddLine(ImVec2(c.x, c.y - r*0.10f),
                        ImVec2(c.x, c.y + r*0.50f), col, th);
        } break;
    }
}

void RenderNotifications() {
    using ZmodUI::Theme::Resolve;
    namespace TT = ZmodUI::Theme;

    const float CARD_W        = 360.0f;
    const float MARGIN        = 16.0f;
    const float PAD_X         = 16.0f;
    const float PAD_Y         = 14.0f;
    const float ICON_SIZE     = 34.0f;
    const float ACCENT_W      =  3.0f;
    const float CORNER        = 14.0f;
    const float FADE_IN       = 0.30f;
    const float FADE_OUT      = 0.45f;
    const int   SHADOW_LAYERS = 7;
    const float PROGRESS_H    = 3.0f;

    ImDrawList* dl   = ImGui::GetForegroundDrawList();
    ImVec2      disp = ImGui::GetIO().DisplaySize;
    float       y    = MARGIN;

    std::lock_guard<std::mutex> lk(g_notification_mutex);

    g_notifications.erase(std::remove_if(g_notifications.begin(), g_notifications.end(),
        [&](const Notification& n) {
            auto e = std::chrono::duration_cast<std::chrono::duration<float>>(
                       std::chrono::steady_clock::now() - n.creation_time).count();
            return e > n.duration.count() + FADE_OUT;
        }), g_notifications.end());

    ImU32 cBg      = Resolve(TT::BgCard);
    ImU32 cLine    = Resolve(TT::Line);
    ImU32 cLineSft = Resolve(TT::LineSoft);
    ImU32 cText    = Resolve(TT::Text);
    ImU32 cTextSec = Resolve(TT::TextSec);
    bool  glass    = ZmodUI::Theme::ShouldUseBackdropBlur();

    ImFont* fDef       = ImGui::GetFont();
    const float fDefH  = fDef ? fDef->FontSize : 13.f;
    const float TITLE_SIZE = fDefH * 1.15f;
    const float BODY_SIZE  = fDefH * 0.94f;

    for (auto& n : g_notifications) {
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(
                         std::chrono::steady_clock::now() - n.creation_time).count();
        float dur = n.duration.count();

        float t = 1.0f;
        if (elapsed < FADE_IN)             { float p = elapsed / FADE_IN;        t = 1.f - (1.f-p)*(1.f-p)*(1.f-p); }
        else if (elapsed > dur - FADE_OUT) { float p = (dur - elapsed) / FADE_OUT; t = p*p; }
        if (t < 0.f) t = 0.f; if (t > 1.f) t = 1.f;

        ImU32 accent;
        NotifIcon iconKind;
        switch (n.type) {
            case NotificationType::Success: accent = Resolve(TT::Green);     iconKind = NotifIcon::Success; break;
            case NotificationType::Warning: accent = Resolve(TT::Orange);    iconKind = NotifIcon::Warning; break;
            case NotificationType::Error:   accent = Resolve(TT::Red);       iconKind = NotifIcon::Error;   break;
            default:                        accent = Resolve(TT::AccentTok); iconKind = NotifIcon::Info;    break;
        }

        std::string title, body;
        NotifSanitize(n.title,   title);
        NotifSanitize(n.message, body);

        float bodyWrap = CARD_W - (PAD_X * 2.f) - ICON_SIZE - 12.f - ACCENT_W;
        ImVec2 bodySz;
        {
            float prev = fDef ? fDef->Scale : 1.f;
            if (fDef) fDef->Scale = BODY_SIZE / fDefH;
            bodySz = ImGui::CalcTextSize(body.c_str(), nullptr, false, bodyWrap);
            if (fDef) fDef->Scale = prev;
        }
        float titleH = TITLE_SIZE;
        float bodyH  = bodySz.y;
        float contentH = titleH + 5.f + bodyH;
        if (contentH < ICON_SIZE) contentH = ICON_SIZE;
        float cardH    = PAD_Y * 2.f + contentH + PROGRESS_H + 4.f;

        float onX  = disp.x - CARD_W - MARGIN;
        float offX = disp.x + 16.f;
        float x    = onX + (offX - onX) * (1.f - t);
        float scale = 0.95f + 0.05f * t;
        float cx = x + CARD_W * 0.5f;
        float cy = y + cardH * 0.5f;
        ImVec2 a(cx - CARD_W * 0.5f * scale, cy - cardH * 0.5f * scale);
        ImVec2 b(cx + CARD_W * 0.5f * scale, cy + cardH * 0.5f * scale);

        for (int i = 1; i <= SHADOW_LAYERS; ++i) {
            float spread = (float)i * 2.2f;
            float falloff = 1.f - (float)i / (SHADOW_LAYERS + 1.5f);
            int   alpha  = (int)(38.f * falloff * falloff * t);
            if (alpha <= 0) continue;
            dl->AddRectFilled(ImVec2(a.x - spread, a.y - spread * 0.4f + 3.f),
                              ImVec2(b.x + spread, b.y + spread * 1.4f + 3.f),
                              IM_COL32(0, 0, 0, alpha), CORNER + spread);
        }

        (void)glass;
        dl->AddRectFilled(a, b, NotifFade(cBg, t), CORNER);

        dl->AddLine(ImVec2(a.x + CORNER * 0.6f, a.y + 0.5f),
                    ImVec2(b.x - CORNER * 0.6f, a.y + 0.5f),
                    NotifFade(IM_COL32(255, 255, 255, 18), t), 1.f);
        dl->AddRect(a, b, NotifFade(cLineSft, t), CORNER, 0, 1.0f);

        {
            ImU32 tintTop = NotifFade((accent & 0x00FFFFFF) | (38u << 24), t);
            ImU32 tintBot = NotifFade((accent & 0x00FFFFFF) | (0u  << 24), t);
            dl->AddRectFilledMultiColor(a, ImVec2(b.x, a.y + 6.f),
                                        tintTop, tintTop, tintBot, tintBot);
        }

        dl->AddRectFilled(ImVec2(a.x, a.y),
                          ImVec2(a.x + ACCENT_W, b.y),
                          NotifFade(accent, t),
                          CORNER, ImDrawFlags_RoundCornersLeft);

        ImVec2 icoCenter(a.x + ACCENT_W + PAD_X + ICON_SIZE * 0.5f,
                         a.y + PAD_Y + ICON_SIZE * 0.5f);
        float  icoR     = ICON_SIZE * 0.5f;
        int aR = (accent >> 0)  & 0xFF;
        int aG = (accent >> 8)  & 0xFF;
        int aB = (accent >> 16) & 0xFF;

        {
            const int N = 10;
            float r0 = icoR + 1.f;
            float r1 = icoR + 8.f;
            for (int i = 0; i < N; ++i) {
                float f = (float)i / (float)(N - 1);
                float r = r0 + (r1 - r0) * f;
                int   al = (int)(70.f * (1.f - f) * (1.f - f) * t);
                if (al <= 0) continue;
                dl->AddCircleFilled(icoCenter, r,
                                    IM_COL32(aR, aG, aB, al), 24);
            }
        }
        dl->AddCircleFilled(icoCenter, icoR, NotifFade(accent, t), 24);

        dl->AddCircle(icoCenter, icoR - 0.5f,
                      IM_COL32(255, 255, 255, (int)(36.f * t)), 24, 1.f);
        DrawNotifGlyph(dl, icoCenter, icoR,
                       iconKind,
                       NotifFade(IM_COL32_WHITE, t));

        float textX = icoCenter.x + icoR + 12.f;
        float textY = a.y + PAD_Y;
        dl->AddText(fDef, TITLE_SIZE,
                    ImVec2(textX, textY),
                    NotifFade(cText, t), title.c_str());
        dl->AddText(fDef, BODY_SIZE,
                    ImVec2(textX, textY + titleH + 4.f),
                    NotifFade(cTextSec, t), body.c_str(), nullptr, bodyWrap);

        if (elapsed < dur - FADE_OUT) {
            float p = 1.f - (elapsed / (dur - FADE_OUT));
            if (p < 0.f) p = 0.f;
            float by = b.y - PROGRESS_H - 6.f;
            ImVec2 bA(a.x + ACCENT_W + 8.f, by);
            ImVec2 bB(b.x - 12.f, by + PROGRESS_H);
            ImU32 trackCol = IM_COL32(aR, aG, aB, (int)(40.f * t));
            dl->AddRectFilled(bA, bB, trackCol, PROGRESS_H * 0.5f);
            ImVec2 fillEnd(bA.x + (bB.x - bA.x) * p, bB.y);
            dl->AddRectFilled(bA, fillEnd, NotifFade(accent, t), PROGRESS_H * 0.5f);
        }

        y += cardH + MARGIN;
        (void)cLine;
    }
}

bool NotifyingCheckbox(const char* label, bool* v) {  
    bool value_changed = ImGui::Checkbox(label, v);
    if (value_changed) {
      
        std::string title = label;
        size_t hash_pos = title.find("##");
        if (hash_pos != std::string::npos) {
            title = title.substr(0, hash_pos);
        }
        std::string message = (*v) ? "Enabled" : "Disabled";
        NotificationType type = (*v) ? NotificationType::Success : NotificationType::Warning;
        ShowNotification(title, message, type, 2.0f);
    }
    return value_changed;
}

ImVec4 GenerateColorFromUsername(const std::string& username) {

    uint32_t hash = 0x811c9dc5;
    for (char c : username) {
        hash ^= (uint32_t)c;
        hash *= 0x01000193;
    }

    float hue = (hash % 360) / 360.0f;
    float saturation = 0.7f;
    float value = 0.9f;

    ImVec4 color;
    ImGui::ColorConvertHSVtoRGB(hue, saturation, value, color.x, color.y, color.z);
    color.w = 1.0f;
    return color;
}

inline std::chrono::steady_clock::time_point g_sessionStartTime = std::chrono::steady_clock::now();

inline void EnsureZmodUIInit() {
    static bool s_zmodui_init = false;
    if (s_zmodui_init) return;
    ZmodUI::Init();
    ZmodBindings::Init();
    ZmodUI::StartBoot();
    s_zmodui_init = true;
}

void RenderApplicationMenu() {

    __try {
        ZmodBindings::SyncFromBacking();
        ZmodUI::Render(ZmodBindings::g_uiState);
        ZmodBindings::ApplyToBacking();
    } __except (EXCEPTION_EXECUTE_HANDLER) {

        static bool s_menuFaultLogged = false;
        DWORD code = GetExceptionCode();
        if (!s_menuFaultLogged) {
            s_menuFaultLogged = true;
            char path[512];
            const char* appd = getenv("APPDATA");
            snprintf(path, sizeof(path), "%s\\ZMod\\menu_fault.log", appd ? appd : ".");
            FILE* f = fopen(path, "a");
            if (f) { fprintf(f, "[menu] render fault 0x%08lX (SyncFromBacking/Render/ApplyToBacking)\n",
                             (unsigned long)code); fclose(f); }
        }
    }
    return;

#if 0

    switch (currentStyle) {
     
    case ThemeStyle::Default:
        ApplyCustomStyleDefault();
        break;
    case ThemeStyle::Oceanic:
        ApplyCustomStyle1();
        break;
    case ThemeStyle::Crimson:
        ApplyCustomStyle2();
        break;

    case ThemeStyle::EmeraldPower:
        ApplyEmeraldPowerStyle();
        break;
    case ThemeStyle::CyberSunset:
        ApplyCyberSunsetStyle();
        break;
    case ThemeStyle::Daylight:
        ApplyDaylightStyle();
        break;
    case ThemeStyle::Grayscale:
        ApplyGrayscaleStyle();
        break;
    case ThemeStyle::PastelDream:
        ApplyPastelDreamStyle();
        break;
    case ThemeStyle::Custom:
        ApplyCustomUserStyle();
        break;
    }

    ImVec2 mds = ImGui::GetIO().DisplaySize;
    if (mds.x > 0 && mds.y > 0) {
        ImGui::SetNextWindowPos(ImVec2(mds.x * 0.5f, mds.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }
    ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Appearing);

    ImGui::Begin("ZMod", &show_imgui_menu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("Home")) {
            ImGui::Text("Welcome, ");
            ImGui::SameLine();
            ImGui::TextColored(GetRainbowColor(0.8f), "%s", windows_username_str.c_str());
            ImGui::SameLine();
            ImGui::Text("!");
            ImGui::Spacing();
            ImGui::Text("Device ID (HWID): %s", GetHWID().c_str());
            ImGui::SeparatorText("System Info");
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Resolution: %d x %d", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

            if (MineScheduler::IsSessionActive()) {
                ImGui::SeparatorText("Mine Scheduler");
                ImGui::TextWrapped("Status: %s", MineScheduler::g_status.c_str());
                ImGui::Text("Tour remaining: %d",
                            (int)MineScheduler::SequenceSize());
            }

            ImGui::SeparatorText("Streamer Mode");
            ImGui::Checkbox("Enable Streamer Mode", &g_streamerMode);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Visual-only nametag override:\n"
                    "  - You appear as rainbow \"ZMod\"\n"
                    "  - Other players appear as \"Anon_N\"\n"
                    "Does not modify packets or server state.\n"
                    "Real names restore on world rejoin after disabling.");
            }

            ImGui::SeparatorText("Security & Exit");

           

          

            ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), "Panic Key: ");
            ImGui::SameLine();
            ImGui::Text("Press ");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.f, 1.f, 0.4f, 1.f), "[ END ]");
            ImGui::SameLine();
            ImGui::Text(" to instantly unload the cheat");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Features")) {

            ImGui::TextDisabled("Config: %s (auto-save)",
                Config::GetConfigPath().string().c_str());
            ImGui::Separator();

            if (ImGui::BeginTabBar("FeatureTabs")) {
                if (ImGui::BeginTabItem("Main")) {
                    ImGui::Text("Main Features"); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
                    NotifyingCheckbox("GodMode", &GodModeEnabled); ImGui::SameLine(); NotifyingCheckbox("Anti-Trap", &g_antiTrap);

                    NotifyingCheckbox("AntiBounce", &AntiBounce);  ImGui::SameLine(); NotifyingCheckbox("AntiFan", &antifan); ImGui::SameLine(); NotifyingCheckbox("AntiDeflector", &g_antiDeflector);
                    if (AntiBounce) {
                        ImGui::Indent(20.0f);
                        NotifyingCheckbox("Ignore Mushrooms", &AntiBounceIgnoreMushrooms);
                        ImGui::SameLine(); ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(?)");
                        if (ImGui::IsItemHovered()) { ImGui::BeginTooltip(); ImGui::Text("Mushroom blocks (75/757/1382/1626/1627/2998) keep their bounce property.\nAll other bouncy/hot blocks still neutralized."); ImGui::EndTooltip(); }
                        ImGui::Unindent(20.0f);
                    }
                    NotifyingCheckbox("InfiniteJetpack", &InfiniteJetpack);
                    ImGui::SameLine(); ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(?)");
                    if (ImGui::IsItemHovered()) { ImGui::BeginTooltip(); ImGui::Text("Zeros ConfigData.rocketFuelConsumptionSpeed (+60FPS) every tick.\nFuel never burns while jetpack equipped. Toggle OFF restores vanilla 1.15 / 1.29."); ImGui::EndTooltip(); }

                    ImGui::SliderFloat("PlayerSpeed", &runSpeedMultiplier, 1.0f, 9.0f);
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::SeparatorText("Movement Overrides");
                    ImGui::Spacing();

                    ImGui::Columns(2, "movement_columns", false);

                    NotifyingCheckbox("Enable Jump Override", &EnableJumpModeOverride);
                    ImGui::BeginDisabled(!EnableJumpModeOverride);
                    {
                        static const char* visibleJumpModes = "Normal\0Double\0LongJump\0Parachute\0Triple\0\0";
                        static const int enumJumpModeMap[] = { 0, 1, 2, 3, 6 };
                        int selectedIndex = 0;
                        for (int i = 0; i < IM_ARRAYSIZE(enumJumpModeMap); i++) {
                            if (static_cast<int>(SelectedJumpMode) == enumJumpModeMap[i]) {
                                selectedIndex = i;
                                break;
                            }
                        }
                        ImGui::Text("Jump Mode:");
                        ImGui::PushItemWidth(-1);
                        if (ImGui::Combo("##JumpModeCombo", &selectedIndex, visibleJumpModes)) {
                            SelectedJumpMode = static_cast<PlayerJumpMode>(enumJumpModeMap[selectedIndex]);
                        }
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndDisabled();

                    ImGui::NextColumn();

                    NotifyingCheckbox("Enable Gravity Override", &EnableGravityOverride);
                    ImGui::BeginDisabled(!EnableGravityOverride);
                    {
                        const char* gravityNames[] = { "Normal", "Low", "High" };
                        static int selectedGravityIndex = 0;
                        ImGui::Text("Gravity Mode:");
                        ImGui::PushItemWidth(-1);
                        if (ImGui::Combo("##GravityModeCombo", &selectedGravityIndex, gravityNames, IM_ARRAYSIZE(gravityNames))) {
                            ChangeGravity(static_cast<GravityMode>(selectedGravityIndex));
                        }
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndDisabled();

                    ImGui::Columns(1);
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Misc")) {
                    ImGui::Text("Misc Tab"); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                    ImGui::Columns(2, "misc_tab_columns", false);

                    NotifyingCheckbox("AntiBlockDeath", &noBlockKill);
                    NotifyingCheckbox("AntiDarkness", &AntiDarkness);
                    NotifyingCheckbox("MiniMap", &Minimap::showMinimap);
                    NotifyingCheckbox("FreeCam", &FreecamEnabled);
                    NotifyingCheckbox("UnlockAllRecipes", &unlockAllRecipes);

                    ImGui::NextColumn();

                    NotifyingCheckbox("UnlimitedOxygen", &unlimitedOxygenEnabled);
                    NotifyingCheckbox("PlaceBlockOnPlayer", &g_BlockOnPlayer);
                    NotifyingCheckbox("AiAimbot", &g_AIAimBot);
                    NotifyingCheckbox("AntiCollect", &g_AntiCollect);
                    NotifyingCheckbox("AutoCollect", &g_autoCollect);
                    NotifyingCheckbox("Keys to Fly", &g_features.b_keysToFlyEnabled);
                    if (g_features.b_keysToFlyEnabled)
                    {
                        ImGui::SliderFloat("Fly Speed", &g_features.f_flySpeed, 4.0f, 10.0f, "%.1f");
                    }
                    NotifyingCheckbox("Anti-AFK", &g_features.b_patchAfkTimers);

                    NotifyingCheckbox("Uncap FPS", &g_uncapFps);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Disables Unity's 60 fps cap by setting "
                                          "Application.targetFrameRate and QualitySettings.vSyncCount=0.\n"
                                          "Re-applied each frame so the game's own setter is overridden.");
                    if (g_uncapFps) {
                        ImGui::SetNextItemWidth(150.0f);
                        if (ImGui::SliderInt("Target FPS", &g_targetFps, 60, 360))
                            g_lastAppliedFps = -2;
                    }

                    ImGui::Columns(1);
                    ImGui::Spacing();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Pathfinder")) {
                    ImGui::Text("Pathfinder"); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                    ImGui::TextColored(ImVec4(0.47f, 0.78f, 1.0f, 1.0f), "Auto Navigate");
                    ImGui::TextWrapped("Hold Alt to highlight a tile. Alt + Right Click to set destination. Escape to cancel.");
                    ImGui::Spacing();
                    NotifyingCheckbox("Enable Auto Navigate", &PathRenderer::g_navEnabled);
                    ImGui::BeginDisabled(!PathRenderer::g_navEnabled);
                    {
                        ImGui::Indent(12.f);
                        NotifyingCheckbox("Show Path Overlay##PF",  &PathRenderer::g_showPath);
                        NotifyingCheckbox("Auto Move##PF",          &PathRenderer::g_autoMove);

                        {
                            const char* options[] = { "Smooth", "Teleport" };
                            int current = (PathRenderer::g_moveMethod == PathRenderer::MoveMethod::Portal) ? 1 : 0;
                            if (ImGui::Combo("Movement Method##PF", &current, options, IM_ARRAYSIZE(options))) {
                                PathRenderer::g_moveMethod = (current == 1)
                                    ? PathRenderer::MoveMethod::Portal
                                    : PathRenderer::MoveMethod::InputDrive;
                            }
                            if (PathRenderer::g_moveMethod == PathRenderer::MoveMethod::Portal) {
                                ImGui::SliderFloat("Portal Interval (ms)##PF",
                                    &PathRenderer::g_portalIntervalMs, 50.f, 500.f, "%.0f");
                                ImGui::TextColored(ImVec4(1.f, 0.85f, 0.3f, 1.f),
                                    "Sends OutgoingMessages.SendPlayerActivateInPortal per node.");
                            }
                        }
                        ImGui::Spacing();

                        ImGui::SliderFloat("Move Speed##PF", &PathRenderer::g_moveSpeed, 0.5f, 15.0f, "%.1f u/s");
                        if (PathRenderer::g_moveSpeed > 4.0f) {
                            ImGui::TextColored(ImVec4(1.f, 0.6f, 0.3f, 1.f),
                                "WARNING: >4 u/s may trip fly-detect kick. \xE2\x89\xA4\x34 is the safe baseline.");
                        } else {
                            ImGui::TextColored(ImVec4(0.55f, 0.78f, 0.55f, 1.f),
                                "\xE2\x89\xA4\x34 units/sec = safe range (matches base run speed).");
                        }
                        ImGui::SliderFloat("Stop Range##PF", &PathRenderer::g_stopRange, 0.0f, 1.0f,  "%.2f u");
                        ImGui::Spacing();

                        ImGui::TextColored(ImVec4(0.9f,0.75f,0.3f,1.f), "Airborne Constraint");
                        NotifyingCheckbox("Airborne Check##PF", &PathRenderer::g_airborneCheck);
                        ImGui::TextWrapped("Limits consecutive air tiles in path. A* finds detours over solid ground when over budget.");
                        ImGui::BeginDisabled(!PathRenderer::g_airborneCheck);
                        ImGui::SliderInt("Max Airborne Tiles##PF", &PathRenderer::g_maxAirborne, 1, 20);
                        ImGui::EndDisabled();
                        ImGui::Spacing();

                        ImGui::TextColored(ImVec4(0.6f,0.9f,1.0f,1.f), "Player Body Fit");
                        ImGui::SliderFloat("Body-Centre Offset##PF", &PathRenderer::g_bodyOffsetFrac, 0.0f, 1.0f, "%.2f x tile");
                        ImGui::TextWrapped("Where in the player body the move-reference sits. Lower = sits higher in tile.");
                        ImGui::Spacing();

                        NotifyingCheckbox("Walk on Insta-Kill Tiles##PF", &PathRenderer::g_walkInstakill);
                        ImGui::TextWrapped("Allow A* to route through insta-kill tiles. Off by default (treats them as solid).");
                        ImGui::Spacing();

                        ImGui::TextColored(ImVec4(0.9f,0.5f,1.0f,1.f), "Path Visuals");
                        NotifyingCheckbox("Glow Halo##PF",        &PathRenderer::g_pathGlow);
                        NotifyingCheckbox("Flow Particles##PF",   &PathRenderer::g_pathFlow);
                        NotifyingCheckbox("Pulsing Target##PF",   &PathRenderer::g_pathPulse);
                        ImGui::Spacing();

                        if (PathRenderer::g_isNavigating && PathRenderer::g_hasTarget) {
                            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f),
                                "Navigating -> (%.2f, %.2f)",
                                PathRenderer::g_navTarget.x, PathRenderer::g_navTarget.y);
                            if (ImGui::Button("Cancel Navigation"))
                                PathRenderer::g_isNavigating = PathRenderer::g_isTeleporting = false;
                        } else if (PathRenderer::g_hasTarget) {
                            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
                                "Target set (%.2f, %.2f) — arrived",
                                PathRenderer::g_navTarget.x, PathRenderer::g_navTarget.y);
                        } else {
                            ImGui::TextDisabled("No target set");
                        }
                        ImGui::Unindent(12.f);
                    }
                    ImGui::EndDisabled();

                    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                    ImGui::TextColored(ImVec4(1.f,0.5f,0.2f,1.f), "Diagnostics");
                    NotifyingCheckbox("Debug Block Overlay##PF", &PathRenderer::g_debugBlockOverlay);
                    ImGui::TextWrapped("Colours tiles by type: Green=air, Red=solid, Yellow=one-way, Grey=passable non-air, White=read error. Block IDs shown when zoomed in.");

                    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("ESP")) {
                    NotifyingCheckbox("Enable ESP", &ESP::bMasterSwitch);
                    ImGui::BeginDisabled(!ESP::bMasterSwitch);

                    const char* lineOrigins[] = { "Local Player", "Top", "Bottom" };
                    const char* healthStyles[] = { "Side Bar", "Bottom Bar", "Text Only", "Side Bar + Text" };

                    auto ColorPickerInline = [](const char* label, ImVec4* col) {
                        ImGui::SameLine();
                        ImGui::ColorEdit4(label, (float*)col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                        };

                    auto SnapSettings = [&](const char* prefix, bool& enable, int& origin, float& thickness, ImVec4& color) {
                        NotifyingCheckbox((std::string("Snaplines##") + prefix).c_str(), &enable);
                        ImGui::BeginDisabled(!enable);
                        {

                            if (origin < 0) origin = 0;
                            if (origin >= IM_ARRAYSIZE(lineOrigins)) origin = IM_ARRAYSIZE(lineOrigins) - 1;
                            ImGui::SetNextItemWidth(120);
                            ImGui::Combo((std::string("Origin##") + prefix).c_str(), &origin, lineOrigins, IM_ARRAYSIZE(lineOrigins));
                            ImGui::SliderFloat((std::string("Line Thickness##") + prefix).c_str(), &thickness, 1.f, 5.f);
                            ColorPickerInline((std::string("Color##") + prefix).c_str(), &color);
                        }
                        ImGui::EndDisabled();
                        ImGui::Spacing();
                        };

                    auto BoxSettings = [&](const char* prefix, bool& enable, int& boxType, float& thickness, bool& fill, ImVec4& col, ImVec4& fillCol) {
                        NotifyingCheckbox((std::string("Boxes##") + prefix).c_str(), &enable);
                        ImGui::BeginDisabled(!enable);
                        {
                            int typeIndex = boxType - 1;
                            ImGui::SetNextItemWidth(120);
                            ImGui::Combo((std::string("Box Type##") + prefix).c_str(), &typeIndex, "Corner\0Full\0\0");
                            boxType = typeIndex + 1;
                            ImGui::SliderFloat((std::string("Box Thickness##") + prefix).c_str(), &thickness, 1.f, 5.f);
                            if (boxType == 2) {
                                NotifyingCheckbox((std::string("Fill##") + prefix).c_str(), &fill);
                                ColorPickerInline((std::string("Fill Color##") + prefix).c_str(), &fillCol);
                            }
                            ColorPickerInline((std::string("Box Color##") + prefix).c_str(), &col);
                        }
                        ImGui::EndDisabled();
                        ImGui::Spacing();
                        };

                    auto HealthSettings = [&](const char* prefix, bool& enable, int& style) {
                        NotifyingCheckbox((std::string("Health##") + prefix).c_str(), &enable);
                        ImGui::BeginDisabled(!enable);
                        {
                            ImGui::SetNextItemWidth(120);
                            ImGui::Combo((std::string("Health Style##") + prefix).c_str(), &style, healthStyles, IM_ARRAYSIZE(healthStyles));
                        }
                        ImGui::EndDisabled();
                        ImGui::Spacing();
                        };

                    ImGui::BeginTabBar("ESP_MainTabs", ImGuiTabBarFlags_Reorderable);

                    if (ImGui::BeginTabItem("LocalPlayer")) {
                        ImGui::Spacing();
                        NotifyingCheckbox("Boxes##LocalPlayer", &ESP::bEnableLocalPlayerBoxes);
                        ImGui::BeginDisabled(!ESP::bEnableLocalPlayerBoxes);
                        {
                            int typeIndex = ESP::localPlayerBoxType - 1;
                            ImGui::SetNextItemWidth(120);
                            ImGui::Combo("Box Type##LocalPlayer", &typeIndex, "Corner\0Full\0\0");
                            ESP::localPlayerBoxType = typeIndex + 1;
                            ImGui::SliderFloat("Box Thickness##LocalPlayer", &ESP::localPlayerBoxThickness, 1.f, 5.f);
                            if (ESP::localPlayerBoxType == 2) {
                                NotifyingCheckbox("Fill##LocalPlayer", &ESP::bLocalPlayerBoxFill);
                                ColorPickerInline("Fill Color##LocalPlayer", &ESP::localPlayerBoxFillColor);
                            }
                        }
                        ImGui::EndDisabled();
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        HealthSettings("LocalPlayer", ESP::bEnableLocalPlayerHealthESP, ESP::localPlayerHealthStyle);
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Players")) {
                        ImGui::Spacing();
                        SnapSettings("Players", ESP::bEnableSnaplines, ESP::lineOrigin, ESP::lineThickness, ESP::lineColor);
                        BoxSettings("Players", ESP::bEnablePlayerBoxes, ESP::boxType, ESP::boxThickness, ESP::bBoxFill, ESP::boxColor, ESP::boxFillColor);
                        HealthSettings("Players", ESP::bEnablePlayerHealthESP, ESP::PlayerHealthStyle);
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("AI")) {
                        ImGui::Spacing();
                        SnapSettings("AI", ESP::bEnableAILines, ESP::AILineOrigin, ESP::ailineThickness, ESP::AIlineColor);
                        BoxSettings("AI", ESP::bEnableAIBoxes, ESP::AIBoxType, ESP::AIBoxThickness, ESP::bAIBoxFill, ESP::AIBoxColor, ESP::AIBoxFillColor);
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        ImGui::Checkbox("Debug Info Overlay", &ESP::bEnableDebugInfo);
                        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Shows AI count, world scan counts\nand offset diagnostics on screen.");
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("World")) {
                        ImGui::BeginTabBar("ESP_WorldItemTabs", ImGuiTabBarFlags_Reorderable);

                        if (ImGui::BeginTabItem("Gemstones")) {
                            ImGui::Spacing();
                            SnapSettings("Gems", ESP::bEnableGemstoneLines, ESP::GemstoneLineOrigin, ESP::gemSnapThickness, ESP::gemSnapColor);
                            BoxSettings("Gems", ESP::bEnableGemstoneBoxes, ESP::gemBoxType, ESP::gemBoxThickness, ESP::bGemBoxFill, ESP::gemBoxColor, ESP::gemBoxFillColor);

                            ImGui::Separator();
                            ImGui::Text("Show Gemstones:");

                            NotifyingCheckbox("Diamonds", &ESP::g_enabledGemstones[World::BlockType::MiningGemStoneDiamond]); ImGui::SameLine();
                            NotifyingCheckbox("Rubies", &ESP::g_enabledGemstones[World::BlockType::MiningGemStoneRuby]); ImGui::SameLine();
                            NotifyingCheckbox("Emeralds", &ESP::g_enabledGemstones[World::BlockType::MiningGemStoneEmerald]);
                            NotifyingCheckbox("Sapphires", &ESP::g_enabledGemstones[World::BlockType::MiningGemStoneSapphire]); ImGui::SameLine();
               
                            NotifyingCheckbox("Topazes", &ESP::g_enabledGemstones[World::BlockType::MiningGemStoneTopaz]);

                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Collectables")) {
                            ImGui::Spacing();
                            SnapSettings("Collectables", ESP::bEnableCollectableLines, ESP::collectableLineOrigin, ESP::collectableLineThickness, ESP::collectableLineColor);
                            BoxSettings("Collectables", ESP::bEnableCollectableBoxes, ESP::collectableBoxType, ESP::collectableBoxThickness, ESP::bCollectableBoxFill, ESP::collectableBoxColor, ESP::collectableBoxFillColor);
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("GiftBoxes")) {
                            ImGui::Spacing();
                            SnapSettings("GiftBoxes", ESP::bEnableGiftBoxLines, ESP::giftBoxLineOrigin, ESP::giftBoxLineThickness, ESP::giftBoxLineColor);
                            BoxSettings("GiftBoxes", ESP::bEnableGiftBoxBoxes, ESP::giftBoxBoxType, ESP::giftBoxBoxThickness, ESP::bGiftBoxBoxFill, ESP::giftBoxBoxColor, ESP::giftBoxBoxFillColor);
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("ExitPortals")) {
                            ImGui::Spacing();
                            SnapSettings("ExitPortals", ESP::bEnableExitPortalLines, ESP::exitPortalLineOrigin, ESP::exitPortalLineThickness, ESP::exitPortalLineColor);
                            BoxSettings("ExitPortals", ESP::bEnableExitPortalBoxes, ESP::exitPortalBoxType, ESP::exitPortalBoxThickness, ESP::bExitPortalBoxFill, ESP::exitPortalBoxColor, ESP::exitPortalBoxFillColor);
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Light Crystals")) {
                            ImGui::Spacing();
                            SnapSettings("Light Crystals", ESP::bEnableLightCrystalLines, ESP::LightCrystalLineOrigin, ESP::lightCrystalLineThickness, ESP::lightCrystalLineColor);
                            BoxSettings("Light Crystals", ESP::bEnableLightCrystalBoxes, ESP::lightCrystalBoxType, ESP::lightCrystalBoxThickness, ESP::bLightCrystalBoxFill, ESP::lightCrystalBoxColor, ESP::lightCrystalBoxFillColor);

                            ImGui::Separator();
                            ImGui::Text("Show Light Crystals:");
                            NotifyingCheckbox("Orb Lighting Mining", &ESP::g_enabledLightCrystals[World::BlockType::OrbLightingMining]); ImGui::SameLine();
                            NotifyingCheckbox("Light Crystal Small", &ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalSmall]); ImGui::SameLine();
                            NotifyingCheckbox("Light Crystal Medium", &ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalMedium]);
                            NotifyingCheckbox("Light Crystal Large", &ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalLarge]); ImGui::SameLine();
                            NotifyingCheckbox("Time Crystal Small", &ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalSmall]); ImGui::SameLine();
                            NotifyingCheckbox("Time Crystal Medium", &ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalMedium]);
                            NotifyingCheckbox("Time Crystal Large", &ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalLarge]);

                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Pots")) {
                            ImGui::Spacing();
                            SnapSettings("Pots", ESP::bEnablePotLines, ESP::PotLineOrigin, ESP::potLineThickness, ESP::potLineColor);
                            BoxSettings("Pots", ESP::bEnablePotBoxes, ESP::potBoxType, ESP::potBoxThickness, ESP::bPotBoxFill, ESP::potBoxColor, ESP::potBoxFillColor);
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Treasure")) {
                            ImGui::Spacing();
                            SnapSettings("Treasure", ESP::bEnableTreasureLines, ESP::TreasureLineOrigin, ESP::treasureLineThickness, ESP::treasureLineColor);
                            BoxSettings("Treasure", ESP::bEnableTreasureBoxes, ESP::treasureBoxType, ESP::treasureBoxThickness, ESP::bTreasureBoxFill, ESP::treasureBoxColor, ESP::treasureBoxFillColor);
                            ImGui::EndTabItem();
                        }

                        ImGui::EndTabBar();
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                
                if (ImGui::BeginTabItem("World"))
                {
                    ImGui::SeparatorText("World Effects");

                    NotifyingCheckbox("Force Weather", &g_forceWeather);

                    if (g_forceWeather) {
                        const char* weatherTypesStr = "None\0Heavy Rain\0Pixel Trail\0Sand Storm\0Light Rain\0Light Snow\0Snow Storm\0Deep Nether\0Halloween\0Halloween Tower\0Hearts\0Mining\0Aurora Borealis\0Armageddon\0\0";
                        int weatherIndex = static_cast<int>(g_forcedWeatherType);

                        ImGui::Text("Weather Type:");
                        ImGui::PushItemWidth(-1);
                        if (ImGui::Combo("##WeatherCombo", &weatherIndex, weatherTypesStr)) {
                            g_forcedWeatherType = static_cast<World::WeatherType>(weatherIndex);

                            if (g_WorldController && oChangeWeather) {
                                oChangeWeather(g_WorldController, (int)g_forcedWeatherType, nullptr);
                            }
                        }
                        ImGui::PopItemWidth();
                    }

                    ImGui::Spacing();

                    NotifyingCheckbox("Force Lighting", &g_forceLighting);

                    if (g_forceLighting) {
                        const char* lightingTypesStr = "None\0Dark\0Mining\0Lesser Dark\0Great Dark\0\0";
                        int lightingIndex = static_cast<int>(g_forcedLightingType);

                        ImGui::Text("Lighting Type:");
                        ImGui::PushItemWidth(-1);
                        if (ImGui::Combo("##LightingCombo", &lightingIndex, lightingTypesStr)) {
                            g_forcedLightingType = static_cast<World::LightingType>(lightingIndex);

                            if (g_WorldController && oChangeLighting) {
                                oChangeLighting(g_WorldController, (int)g_forcedLightingType, nullptr);
                            }
                        }
                        ImGui::PopItemWidth();
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::SeparatorText("Background Changer");
                    {

                        const char* backgroundTypesStr = "Forest\0Night\0Space\0Desert\0Ice\0Star\0Candy\0Halloween Tower\0Nether\0City\0Blue Sky\0Jet Race\0\0";

                        const int enumBgMap[] = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12 };

                        static int selectedBgEnumValue = 0;

                        int comboDisplayIndex = 0;

                        for (int i = 0; i < IM_ARRAYSIZE(enumBgMap); i++) {
                            if (selectedBgEnumValue == enumBgMap[i]) {
                                comboDisplayIndex = i;
                                break;
                            }
                        }

                        ImGui::Text("Background:");
                        ImGui::PushItemWidth(-1);

                        if (ImGui::Combo("##BackgroundCombo", &comboDisplayIndex, backgroundTypesStr)) {

                            selectedBgEnumValue = enumBgMap[comboDisplayIndex];
                        }
                        ImGui::PopItemWidth();

                        if (ImGui::Button("Set Background", ImVec2(-1, 0))) {
                            if (g_WorldController && oChangeBackground) {

                                oChangeBackground(g_WorldController, selectedBgEnumValue, nullptr);
                            }
                        }
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Auto"))
                {
              
                    if (ImGui::CollapsingHeader("Auto Fish Bot"))
                    {

                        ImGui::Checkbox("Enable Fish Bot", &FishBot::g_enabled);
                        ImGui::Spacing();

                        ImGui::BeginDisabled(!FishBot::g_enabled);
                        {
                            ImGui::Checkbox("Auto Land",    &FishBot::g_autoLand);
                            ImGui::SameLine();
                            ImGui::Checkbox("Auto Recast",  &FishBot::g_autoRecast);
                            ImGui::SameLine();
                            ImGui::Checkbox("Anti-Sleep",   &FishBot::g_antiSleep);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Pins playerChangeToSleepSeconds to INT_MAX every tick\nso the server cannot force your character to sleep.");
                            ImGui::SameLine();
                            ImGui::Checkbox("Anti-AFK",     &FishBot::g_antiAfk);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Taps jump at ~8 s idle; forces recast at 10 s idle.\nPrevents server AFK detection between casts.");

                            ImGui::Separator();

                            ImGui::SliderFloat("Recast Min (s)", &FishBot::g_recastDelayMinSec, 0.2f, 5.0f, "%.1f s");
                            ImGui::SliderFloat("Recast Max (s)", &FishBot::g_recastDelayMaxSec,
                                FishBot::g_recastDelayMinSec, 10.0f, "%.1f s");

                            ImGui::SliderFloat("Hook React Min (ms)", &FishBot::g_hookReactionMinMs, 50.0f, 500.0f, "%.0f ms");
                            ImGui::SliderFloat("Hook React Max (ms)", &FishBot::g_hookReactionMaxMs,
                                FishBot::g_hookReactionMinMs, 1200.0f, "%.0f ms");

                            ImGui::Separator();

                            ImGui::Checkbox("Legit Mode (pause mid-reel)", &FishBot::g_legitMode);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Periodically pauses steering to look more human.\n"
                                                  "Note: may be MORE suspicious, not less.");
                            if (FishBot::g_legitMode) {
                                ImGui::Indent();
                                ImGui::SliderFloat("Pause Delay Min (s)",    &FishBot::g_legitPauseDelayMin, 2.0f, 20.0f, "%.1f s");
                                ImGui::SliderFloat("Pause Delay Max (s)",    &FishBot::g_legitPauseDelayMax,
                                    FishBot::g_legitPauseDelayMin, 30.0f, "%.1f s");
                                ImGui::SliderFloat("Pause Duration Min (s)", &FishBot::g_legitPauseDurMin,   0.2f, 3.0f, "%.1f s");
                                ImGui::SliderFloat("Pause Duration Max (s)", &FishBot::g_legitPauseDurMax,
                                    FishBot::g_legitPauseDurMin, 6.0f, "%.1f s");
                                ImGui::Unindent();
                            }
                        }
                        ImGui::EndDisabled();

                        ImGui::Separator();
                        if (FishBot::g_activeLure != World::BlockType::None)
                            ImGui::Text("Active lure: %d", (int)FishBot::g_activeLure);
                        else
                            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "No lure — cast once manually or use inventory scan.");
                        if (FishBot::g_lastBaitPos.x != -1)
                            ImGui::Text("Bait tile: (%d, %d)", FishBot::g_lastBaitPos.x, FishBot::g_lastBaitPos.y);
                        else
                            ImGui::TextDisabled("Bait tile: auto-probe (x±1, y-1)");

                        if (FishBot::g_enabled)
                        {
                            ImGui::Separator();
                            auto now = std::chrono::steady_clock::now();
                            auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - FishBot::g_stats_startTime);
                            long long ts = dur.count();
                            ImGui::Text("Runtime: %02lld:%02lld:%02lld", ts / 3600, (ts % 3600) / 60, ts % 60);

                            int total = FishBot::g_stats_caught + FishBot::g_stats_missed + FishBot::g_stats_skipped;
                            ImGui::Text("Caught: %d  Missed: %d  Skipped: %d",
                                FishBot::g_stats_caught, FishBot::g_stats_missed, FishBot::g_stats_skipped);
                            if (total > 0) {
                                float rate = 100.0f * FishBot::g_stats_caught / (float)total;
                                ImGui::Text("Catch rate: %.1f%%", rate);
                            }
                            if (ts > 0) {
                                float fph = (float)FishBot::g_stats_caught / ((float)ts / 3600.0f);
                                ImGui::Text("Fish/hour: %.0f", fph);
                            }

                            if (ImGui::TreeNode("Field Offset Diagnostics")) {
                                ImGui::Text("fishingState:   0x%llX", (unsigned long long)g_off_Player_fishingState);
                                ImGui::Text("fishingLeft:    0x%llX", (unsigned long long)g_off_Player_fishingLeftButton);
                                ImGui::Text("fishingRight:   0x%llX", (unsigned long long)g_off_Player_fishingRightButton);
                                ImGui::Text("jumpButtonDown: 0x%llX", (unsigned long long)g_off_Player_jumpButtonDown);
                                ImGui::Text("leftButton:     0x%llX", (unsigned long long)g_off_Player_leftButton);
                                ImGui::Text("isReadyToLand:  0x%llX", (unsigned long long)g_off_Gauge_isReadyToLand);
                                ImGui::Text("fishTargetPoint:0x%llX", (unsigned long long)g_off_Gauge_fishTargetPoint);
                                ImGui::TreePop();
                            }
                        }
                    }

                    if (ImGui::CollapsingHeader("Auto Nether Bot"))
                        AutoNether::RenderTab();

                    if (ImGui::CollapsingHeader("Auto Mine Bot"))
                        AutoMine::RenderTab();

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Utilities"))
        {
            if (ImGui::BeginTabBar("UtilitiesSubTabs"))
            {
             
                if (ImGui::BeginTabItem("Game Controls"))
                {
                    ImGui::SeparatorText("Client Management");
                    ImGui::TextWrapped("General game client operations.");
                    ImGui::Spacing();

                    if (ImGui::Button("Reload Game", ImVec2(-1, 35))) {
                        if (oReloadGame) {
                            oReloadGame();
                            ShowNotification("Game Control", "Reloading the game client.", NotificationType::Info);
                        }
                    }

                    if (ImGui::Button("Force Logout (return to login screen)",
                        ImVec2(-1, 35)))
                    {
                        if (MultiboxMode::LogoutCurrent()) {
                            ShowNotification("Game Control",
                                "Logged out — game will reload to login screen.",
                                NotificationType::Info, 4.0f);
                        } else {
                            ShowNotification("Logout Failed",
                                "Game function pointers not resolved. Inject "
                                "again after entering a world.",
                                NotificationType::Error, 5.0f);
                        }
                    }

                    ImGui::Spacing();
                    ImGui::SeparatorText("HWID Spoof");
                    ImGui::TextWrapped(
                        "Pixel Worlds reads SystemInfo.deviceUniqueIdentifier "
                        "and sends it to the server as `sdid`. Randomize to "
                        "swap in a fake GUID — click Force Logout above to "
                        "apply (the game caches the value at login).");

                    if (MultiboxProfile::g_userHwidOverride.load()) {
                        ImGui::PushStyleColor(ImGuiCol_Text,
                            ImVec4(0.40f, 0.85f, 0.45f, 1.0f));
                        ImGui::TextWrapped("Active: %s",
                            MultiboxProfile::g_userHwidValue.c_str());
                        ImGui::PopStyleColor();
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Text,
                            ImVec4(0.65f, 0.65f, 0.65f, 1.0f));
                        ImGui::TextUnformatted("Active: (real device id)");
                        ImGui::PopStyleColor();
                    }

                    if (ImGui::Button("Randomize HWID", ImVec2(-1, 35))) {
                        std::string newId = MultiboxProfile::RandomizeHwid();
                        ShowNotification("Game Control",
                            "HWID randomized to " + newId.substr(0, 13) +
                            "... — click Force Logout to apply.",
                            NotificationType::Info, 6.0f);
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Networking")) {
            if (ImGui::BeginTabBar("NetworkingTabs")) {

                if (ImGui::BeginTabItem("Logger")) {

                    NotifyingCheckbox("Capture Incoming", &g_CaptureIncoming); ImGui::SameLine();
                    NotifyingCheckbox("Capture Outgoing", &g_CaptureOutgoing); ImGui::SameLine();
                    NotifyingCheckbox("Auto-scroll", &g_AutoScroll); ImGui::SameLine();
                    if (ImGui::Button("Clear Log")) {
                        std::lock_guard<std::mutex> lock(g_PacketLogMutex);
                        g_PacketLog.clear();
                        g_selectedPackets.clear();
                        g_anchorPacketIndex = -1;
                        g_packetRepeaterContent[0] = '\0';
                    }

                    const char* directions[] = { "All", "Outgoing", "Incoming" };
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
                    ImGui::InputTextWithHint("##Filter", "Filter by Packet ID...", g_packetFilterText, sizeof(g_packetFilterText));
                    ImGui::PopItemWidth(); ImGui::SameLine();
                    ImGui::PushItemWidth(-1);
                    ImGui::Combo("##Direction", &g_directionFilter, directions, IM_ARRAYSIZE(directions));
                    ImGui::PopItemWidth();

                    ImGui::Columns(2, "LoggerSplitter", true);

                    ImGui::BeginChild("PacketListChild");
                    if (ImGui::BeginTable("PacketTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit)) {
                        ImGui::TableSetupColumn("Time",      ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Direction", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("ID",        ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Size",      ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        std::vector<int> visibleIndices;
                        {
                            std::lock_guard<std::mutex> lock(g_PacketLogMutex);
                            for (int i = 0; i < (int)g_PacketLog.size(); ++i) {
                                const auto& p = g_PacketLog[i];
                                if (g_directionFilter != 0 && (int)p.direction + 1 != g_directionFilter) continue;
                                if (g_packetFilterText[0] != '\0' && p.idString.find(g_packetFilterText) == std::string::npos) continue;
                                visibleIndices.push_back(i);
                            }
                        }

                        const bool kCtrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                        const bool kShift = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;
                        const bool kA     = (GetAsyncKeyState('A')         & 0x8000) != 0;
                        const bool kC     = (GetAsyncKeyState('C')         & 0x8000) != 0;

                        static bool prevCtrlA = false, prevCtrlC = false;
                        const bool edgeCtrlA = kCtrl && kA && !prevCtrlA;
                        const bool edgeCtrlC = kCtrl && kC && !prevCtrlC;
                        prevCtrlA = kCtrl && kA;
                        prevCtrlC = kCtrl && kC;

                        if (edgeCtrlA) {
                            g_selectedPackets.clear();
                            for (int idx : visibleIndices) g_selectedPackets.insert(idx);
                        }

                        if (edgeCtrlC && !g_selectedPackets.empty()) {
                            std::string clip;
                            std::lock_guard<std::mutex> lock(g_PacketLogMutex);
                            for (int idx : g_selectedPackets) {
                                if (idx >= (int)g_PacketLog.size()) continue;
                                const auto& p = g_PacketLog[idx];
                                clip += "[" + FormatTimestamp(p.timestamp) + "] ";
                                clip += (p.direction == PacketDirection::Incoming ? "Incoming" : "Outgoing");
                                clip += " | ID: " + p.idString;
                                clip += " | " + std::to_string(p.fullPacketJson.size()) + " B\n";
                                clip += p.fullPacketJson + "\n\n";
                            }
                            ImGui::SetClipboardText(clip.c_str());
                        }

                        {
                            std::lock_guard<std::mutex> lock(g_PacketLogMutex);
                            for (int vi = 0; vi < (int)visibleIndices.size(); ++vi) {
                                int i = visibleIndices[vi];
                                const auto& packet = g_PacketLog[i];

                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();

                                bool isSelected = g_selectedPackets.count(i) > 0;
                                std::string label = FormatTimestamp(packet.timestamp) + "##" + std::to_string(i);

                                if (ImGui::Selectable(label.c_str(), isSelected,
                                    ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {

                                    bool ctrl  = kCtrl;
                                    bool shift = kShift;

                                    if (ctrl && !shift) {

                                        if (isSelected) g_selectedPackets.erase(i);
                                        else            g_selectedPackets.insert(i);
                                        g_anchorPacketIndex = i;
                                    } else if (shift && g_anchorPacketIndex != -1) {

                                        if (!ctrl) g_selectedPackets.clear();

                                        int anchorVi = 0;
                                        for (int k = 0; k < (int)visibleIndices.size(); ++k)
                                            if (visibleIndices[k] == g_anchorPacketIndex) { anchorVi = k; break; }
                                        int lo = anchorVi < vi ? anchorVi : vi;
                                        int hi = anchorVi > vi ? anchorVi : vi;
                                        for (int k = lo; k <= hi; ++k)
                                            g_selectedPackets.insert(visibleIndices[k]);
                                    } else {

                                        g_selectedPackets.clear();
                                        g_selectedPackets.insert(i);
                                        g_anchorPacketIndex = i;
                                    }

                                    strncpy_s(g_packetRepeaterContent, packet.fullPacketJson.c_str(),
                                              sizeof(g_packetRepeaterContent) - 1);
                                }

                                ImGui::TableNextColumn(); ImGui::TextUnformatted(packet.direction == PacketDirection::Incoming ? "Incoming" : "Outgoing");
                                ImGui::TableNextColumn(); ImGui::TextUnformatted(packet.idString.c_str());
                                ImGui::TableNextColumn(); ImGui::Text("%zu B", packet.fullPacketJson.length());
                            }
                        }

                        if (g_AutoScroll && g_newPacketReceived) {
                            ImGui::SetScrollHereY(1.0f);
                            g_newPacketReceived = false;
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndChild();

                    ImGui::NextColumn();

                    {
                        int displayIdx = g_selectedPackets.empty() ? -1 : *g_selectedPackets.begin();
                        ImGui::Text("Selected Packet JSON: (%zu selected)", g_selectedPackets.size());
                        ImGui::BeginChild("PacketDetailsChild", ImVec2(0, 0), true);
                        if (displayIdx != -1) {
                            std::lock_guard<std::mutex> lock(g_PacketLogMutex);
                            if (displayIdx < (int)g_PacketLog.size())
                                ImGui::TextUnformatted(g_PacketLog[displayIdx].fullPacketJson.c_str());
                        }
                        ImGui::EndChild();
                    }
                    ImGui::Columns(1);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Repeater")) {
                    ImGui::TextWrapped("Edit and resend a packet. Select a packet from the logger to use it as a template.");
                    ImGui::InputTextMultiline("##RepeaterContent", g_packetRepeaterContent, sizeof(g_packetRepeaterContent), ImVec2(-1, ImGui::GetContentRegionAvail().y - 70));

                    if (ImGui::Button("Send Packet", ImVec2(-1, 30))) {
                        std::string packetContent = std::string(g_packetRepeaterContent);
                        std::thread([packetContent]() {
                            Il2cppThreadScope _gc;
                            RepeatPacket(packetContent);
                        }).detach();
                    }

                    ImGui::Text("Status: %s", g_networkStatus.c_str());
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Settings")) {
                    ImGui::TextWrapped("Ignored Packet IDs (comma-separated). Packets with these IDs will not be logged. Click 'Apply' to save changes.");
                    ImGui::InputTextMultiline("##IgnoreList", g_packetIgnoreListBuffer, sizeof(g_packetIgnoreListBuffer), ImVec2(-1, ImGui::GetContentRegionAvail().y - 40));

                    if (ImGui::Button("Apply Ignore List", ImVec2(-1, 30))) {
                        std::lock_guard<std::mutex> lock(g_PacketLogMutex);
                        g_packetIdIgnoreList.clear();
                        std::string s(g_packetIgnoreListBuffer);
                        std::stringstream ss(s);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                            item.erase(0, item.find_first_not_of(" \n\r\t"));
                            item.erase(item.find_last_not_of(" \n\r\t") + 1);
                            if (!item.empty()) {
                                g_packetIdIgnoreList.push_back(item);
                            }
                        }
                    }
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Settings")) {
          
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

            ImGui::BeginChild("SessionStatsPanel", ImVec2(0, 75), true);
            {
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - g_sessionStartTime);
                int hours = (int)elapsed.count() / 3600;
                int minutes = ((int)elapsed.count() % 3600) / 60;
                int seconds = (int)elapsed.count() % 60;
                char timeBuffer[32];
                sprintf_s(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", hours, minutes, seconds);

                ImGui::Text("Session Duration");
                ImGui::Separator();
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(timeBuffer).x) * 0.5f);
                ImGui::Text(timeBuffer);
                ImGui::ProgressBar((float)seconds / 59.0f, ImVec2(-1.0f, 4.0f), "");
            }
            ImGui::EndChild();

          

            ImGui::Text("Select a Theme");
            ImGui::Separator();

            float fullWidth = ImGui::GetContentRegionAvail().x;
            float cardWidth = (fullWidth - ImGui::GetStyle().ItemSpacing.x * 3) / 4.0f;
            float cardHeight = 69.0f; 

            ImGui::BeginChild("ThemeSelector", ImVec2(0, 230), false);

            auto ThemeCard = [&](const char* id, const char* title, ThemeStyle styleType,
                ImVec4 c1, ImVec4 c2, ImVec4 c3, ImVec4 borderColor) {
                    bool isSelected = (currentStyle == styleType);
                    if (isSelected) ImGui::PushStyleColor(ImGuiCol_Border, borderColor);

                    ImGui::BeginChild(id, ImVec2(cardWidth, cardHeight), true);
                    ImGui::Text(title);
                    ImGui::ColorButton((std::string("##") + id + "1").c_str(), c1, ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20)); ImGui::SameLine();
                    ImGui::ColorButton((std::string("##") + id + "2").c_str(), c2, ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20)); ImGui::SameLine();
                    ImGui::ColorButton((std::string("##") + id + "3").c_str(), c3, ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20));
                    ImGui::EndChild();

                    if (ImGui::IsItemClicked()) currentStyle = styleType;
                    if (isSelected) ImGui::PopStyleColor();
                };

            ThemeCard("DefaultCard", "Default", ThemeStyle::Default,
                ImVec4(0.055f, 0.063f, 0.094f, 1.0f),
                ImVec4(0.082f, 0.102f, 0.149f, 1.0f),
                ImVec4(0.000f, 0.784f, 0.961f, 1.0f),
                ImVec4(0.000f, 0.784f, 0.961f, 1.0f));
            ImGui::SameLine();
            ThemeCard("OceanicCard", "Oceanic", ThemeStyle::Oceanic,
                ImVec4(0.07f, 0.12f, 0.18f, 1.0f),
                ImVec4(0.15f, 0.30f, 0.55f, 1.0f),
                ImVec4(0.30f, 0.55f, 0.90f, 1.0f),
                ImVec4(0.40f, 0.65f, 1.00f, 1.0f));
            ImGui::SameLine();
            ThemeCard("CrimsonCard", "Crimson", ThemeStyle::Crimson,
                ImVec4(0.12f, 0.07f, 0.07f, 1.0f),
                ImVec4(0.45f, 0.15f, 0.15f, 1.0f),
                ImVec4(0.80f, 0.30f, 0.30f, 1.0f),
                ImVec4(0.95f, 0.40f, 0.40f, 1.0f));
            ImGui::SameLine();
            ThemeCard("EmeraldCard", "Emerald Power", ThemeStyle::EmeraldPower,
                ImVec4(0.01f, 0.01f, 0.01f, 1.0f),
                ImVec4(0.10f, 0.10f, 0.10f, 1.0f),
                ImVec4(0.00f, 1.00f, 0.47f, 1.0f),
                ImVec4(0.00f, 1.00f, 0.47f, 1.0f));

            ImGui::Dummy(ImVec2(0, 5));
            ThemeCard("CyberCard", "Cyber Sunset", ThemeStyle::CyberSunset,
                ImVec4(0.05f, 0.03f, 0.10f, 1.0f),
                ImVec4(0.40f, 0.20f, 0.10f, 1.0f),
                ImVec4(1.00f, 0.40f, 0.00f, 1.0f),
                ImVec4(1.00f, 0.55f, 0.20f, 1.0f));
            ImGui::SameLine();
            ThemeCard("DaylightCard", "Daylight", ThemeStyle::Daylight,
                ImVec4(0.88f, 0.88f, 0.88f, 1.0f),
                ImVec4(0.93f, 0.93f, 0.93f, 1.0f),
                ImVec4(0.26f, 0.59f, 0.98f, 1.0f),
                ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
            ImGui::SameLine();
            ThemeCard("GrayscaleCard", "Grayscale", ThemeStyle::Grayscale,
                ImVec4(0.10f, 0.10f, 0.10f, 1.0f),
                ImVec4(0.40f, 0.40f, 0.40f, 1.0f),
                ImVec4(0.80f, 0.80f, 0.80f, 1.0f),
                ImVec4(0.80f, 0.80f, 0.80f, 1.0f));
            ImGui::SameLine();
            ThemeCard("PastelCard", "Pastel Dream", ThemeStyle::PastelDream,
                ImVec4(0.10f, 0.10f, 0.20f, 1.0f),
                ImVec4(0.30f, 0.70f, 0.80f, 1.0f),
                ImVec4(0.60f, 0.40f, 0.80f, 1.0f),
                ImVec4(0.60f, 0.40f, 0.80f, 1.0f));

            ImGui::Dummy(ImVec2(0, 5));

            if (!g_customThemeInitialized) {
                InitCustomThemeFromCurrent();
            }
            ThemeCard("CustomCard", "Custom", ThemeStyle::Custom,
                g_customThemeColors[ImGuiCol_WindowBg],
                g_customThemeColors[ImGuiCol_FrameBg],
                g_customThemeColors[ImGuiCol_Button],
                g_customThemeColors[ImGuiCol_ButtonHovered]);

            ImGui::EndChild();

            if (currentStyle == ThemeStyle::Custom) {
                ImGui::Separator();
                ImGui::Text("Customize Colors");
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Edit any ImGui color. Changes apply live and persist to config.");
                }

                static int presetIdx = 0;
                const char* presetNames[] = {
                    "Default", "Oceanic", "Crimson", "Emerald Power",
                    "Cyber Sunset", "Daylight", "Grayscale", "Pastel Dream"
                };
                ImGui::SetNextItemWidth(180);
                ImGui::Combo("##CustomSeedPreset", &presetIdx, presetNames, IM_ARRAYSIZE(presetNames));
                ImGui::SameLine();
                if (ImGui::Button("Load preset", ImVec2(110, 0))) {
                    SeedCustomThemeFrom((ThemeStyle)presetIdx);
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset to Default", ImVec2(140, 0))) {
                    SeedCustomThemeFrom(ThemeStyle::Default);
                }

                static char filterBuf[64] = "";
                ImGui::SetNextItemWidth(-1);
                ImGui::InputTextWithHint("##CustomColorFilter", "Filter color name...", filterBuf, sizeof(filterBuf));

                ImGui::BeginChild("CustomColorList", ImVec2(0, 220), true);
                for (int i = 0; i < ImGuiCol_COUNT; ++i) {
                    const char* name = ImGui::GetStyleColorName(i);
                    if (filterBuf[0] != '\0') {

                        std::string lhs = name, rhs = filterBuf;
                        for (auto& c : lhs) c = (char)tolower((unsigned char)c);
                        for (auto& c : rhs) c = (char)tolower((unsigned char)c);
                        if (lhs.find(rhs) == std::string::npos) continue;
                    }
                    ImGui::PushID(i);
                    ImGui::ColorEdit4(name, (float*)&g_customThemeColors[i],
                        ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
                    ImGui::PopID();
                }
                ImGui::EndChild();
            }

            ImGui::PopStyleVar(4);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

#endif
}

class TeeStreambuf : public std::streambuf {
public:
    TeeStreambuf(std::streambuf* a, std::streambuf* b) : m_a(a), m_b(b) {}
protected:

    int_type overflow(int_type c) override {
        if (c == EOF) return !EOF;
        std::lock_guard<std::mutex> lk(SharedMutex());
        if (m_a) (void)m_a->sputc(static_cast<char>(c));
        if (m_b && m_b->sputc(static_cast<char>(c)) == EOF) return EOF;
        return c;
    }
    int sync() override {
        std::lock_guard<std::mutex> lk(SharedMutex());
        if (m_a) (void)m_a->pubsync();
        return (m_b && m_b->pubsync() != 0) ? -1 : 0;
    }
private:
    static std::mutex& SharedMutex() {
        static std::mutex m;
        return m;
    }
    std::streambuf* m_a;
    std::streambuf* m_b;
};

static std::ofstream g_log_file;
static TeeStreambuf* g_cout_tee = nullptr;
static TeeStreambuf* g_cerr_tee = nullptr;
static std::streambuf* g_orig_cout = nullptr;
static std::streambuf* g_orig_cerr = nullptr;

static std::filesystem::path LogFilePath() {
    PWSTR appData = nullptr;
    std::filesystem::path base = "C:\\";
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appData))) {
        base = appData;
        CoTaskMemFree(appData);
    }
    auto dir = base / "ZMod" / "logs";
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    char name[64];
    std::snprintf(name, sizeof(name), "zwspark-%lu-%llu.log",
        (unsigned long)GetCurrentProcessId(),
        (unsigned long long)GetTickCount64());
    return dir / name;
}

void AttachConsoleToDLL() {
#ifdef ZMOD_V3

    LogStream::Init();
#else

    if (AllocConsole()) {
        FILE* f_dummy;
        freopen_s(&f_dummy, "CONOUT$", "w", stdout);
        freopen_s(&f_dummy, "CONOUT$", "w", stderr);
        freopen_s(&f_dummy, "CONIN$", "r", stdin);

        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }

    try {
        auto path = LogFilePath();
        g_log_file.open(path, std::ios::out | std::ios::trunc);
        if (g_log_file) {
            g_orig_cout = std::cout.rdbuf();
            g_orig_cerr = std::cerr.rdbuf();
            g_cout_tee = new TeeStreambuf(g_orig_cout, g_log_file.rdbuf());
            g_cerr_tee = new TeeStreambuf(g_orig_cerr, g_log_file.rdbuf());
            std::cout.rdbuf(g_cout_tee);
            std::cerr.rdbuf(g_cerr_tee);

            std::cout << std::unitbuf;
            std::cerr << std::unitbuf;

            std::cout << "[Log] Tee to " << path.string() << std::endl;
        }
    } catch (...) {

    }

    std::cout << "Console attached successfully." << std::endl;
#endif
}

void InitializeImGui() {
    if (imgui_initialized) return;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigInputTrickleEventQueue = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.MouseDrawCursor = false;

    ImGui_ImplWin32_Init(window_handle);
    ImGui_ImplDX11_Init(d3d_device, d3d_context);

    SpriteAtlas::TryLoad(d3d_device);
    imgui_initialized = true;
}

static bool HandleExtraZoomScroll(UINT uMsg, WPARAM wParam) {
    if (uMsg != WM_MOUSEWHEEL) return false;
    if (!g_extraZoom) return false;

    if (show_imgui_menu && imgui_initialized) {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) return false;
    }

    short delta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (delta == 0) return false;

    const float kStep = 0.25f;
    const float kMax  = 15.0f;
    const float kMin  = 0.3f;

    if (delta < 0) {
        g_extraZoomMul += kStep;
        if (g_extraZoomMul > kMax) g_extraZoomMul = kMax;
    } else {
        g_extraZoomMul -= kStep;
        if (g_extraZoomMul < kMin) g_extraZoomMul = kMin;
    }

    return true;
}

LRESULT __stdcall WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (HandleExtraZoomScroll(uMsg, wParam)) return 0;

    bool imguiHandled = false;
    if (imgui_initialized) {
        imguiHandled = ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) != 0;
    }

    if (imgui_initialized) {
        ImGuiIO& io = ImGui::GetIO();

        if (show_imgui_menu) {
            switch (uMsg) {
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
            case WM_XBUTTONDOWN: case WM_XBUTTONUP: case WM_XBUTTONDBLCLK:
            case WM_MOUSEWHEEL:  case WM_MOUSEHWHEEL:
                if (io.WantCaptureMouse) return TRUE;
                break;
            case WM_KEYDOWN: case WM_KEYUP:
            case WM_SYSKEYDOWN: case WM_SYSKEYUP:
            case WM_CHAR: case WM_DEADCHAR:
            case WM_SYSCHAR: case WM_SYSDEADCHAR:
                if (io.WantCaptureKeyboard) return TRUE;
                break;
            default: break;
            }
        }

        if (imguiHandled) return TRUE;
    }

    return CallWindowProc(original_wnd_proc, hWnd, uMsg, wParam, lParam);
}
void PerformFullCleanup() {

    MH_DisableHook(MH_ALL_HOOKS);

    if (original_wnd_proc && window_handle) {
        SetWindowLongPtr(window_handle, GWLP_WNDPROC, (LONG_PTR)original_wnd_proc);
    }
    original_wnd_proc = nullptr;

    ZmodUI::Blur::Shutdown();

    if (imgui_initialized) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    imgui_initialized = false;

    for (auto& s : g_backBuffers) {
        if (s.rtv)     s.rtv->Release();
        if (s.wrapped) s.wrapped->Release();
        if (s.native)  s.native->Release();
    }
    g_backBuffers.clear();

    if (main_render_target_view) { main_render_target_view->Release(); main_render_target_view = nullptr; }
    if (g_d3d11on12)             { g_d3d11on12->Release();             g_d3d11on12             = nullptr; }
    if (d3d_context)             { d3d_context->Release();             d3d_context             = nullptr; }
    if (d3d_device)              { d3d_device->Release();              d3d_device              = nullptr; }
    if (g_swapChain3)            { g_swapChain3->Release();            g_swapChain3            = nullptr; }
    if (g_d3d12Queue)            { g_d3d12Queue->Release();            g_d3d12Queue            = nullptr; }
    if (g_d3d12Device)           { g_d3d12Device->Release();           g_d3d12Device           = nullptr; }

    if (g_kiero_initialized_successfully) kiero::shutdown();
    g_kiero_initialized_successfully = false;
    original_present      = nullptr;
    g_originalPresent1    = nullptr;
    g_originalResize      = nullptr;
    g_originalExecuteCmds = nullptr;

    MH_Uninitialize();
    g_OverlayHwnd = nullptr;
}

static void ResolveFpsUncapSetters() {
    if (o_SetTargetFrameRate && o_SetVSyncCount) return;
    o_SetTargetFrameRate = (tSetTargetFrameRate)IL2CPP::Class::Utils::GetMethodPointer(
        "UnityEngine.Application",     "set_targetFrameRate", 1);
    o_SetVSyncCount      = (tSetVSyncCount)IL2CPP::Class::Utils::GetMethodPointer(
        "UnityEngine.QualitySettings", "set_vSyncCount",      1);
    if (!o_SetTargetFrameRate)
        std::cout << "[FPS] set_targetFrameRate NOT FOUND" << std::endl;
    if (!o_SetVSyncCount)
        std::cout << "[FPS] set_vSyncCount NOT FOUND" << std::endl;
    if (o_SetTargetFrameRate && o_SetVSyncCount)
        std::cout << "[FPS] FPS uncap setters resolved." << std::endl;
}

static void TickFpsUncap() {
    if (!o_SetTargetFrameRate || !o_SetVSyncCount) return;
    int wanted = g_uncapFps ? g_targetFps : 60;

    bool needApply = g_uncapFps || (wanted != g_lastAppliedFps);
    if (!needApply) return;
    __try {
        o_SetVSyncCount(0);
        o_SetTargetFrameRate(wanted);
        if (wanted != g_lastAppliedFps) {
            std::cout << "[FPS] targetFrameRate -> " << wanted << " (vsync 0)." << std::endl;
        }
        g_lastAppliedFps = wanted;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        std::cout << "[FPS] EXCEPTION applying FPS settings - disabling." << std::endl;
        o_SetTargetFrameRate = nullptr;
        o_SetVSyncCount      = nullptr;
    }
}

static void ReleaseWrappedBackBuffers() {
    for (auto& s : g_backBuffers) {
        if (s.rtv)     { s.rtv->Release();     s.rtv     = nullptr; }
        if (s.wrapped) { s.wrapped->Release(); s.wrapped = nullptr; }
        if (s.native)  { s.native->Release();  s.native  = nullptr; }
    }
    g_backBuffers.clear();
}

static bool WrapBackBuffersFromSwapChain() {
    if (!g_swapChain3 || !g_d3d11on12) return false;
    DXGI_SWAP_CHAIN_DESC1 d1 = {};
    g_swapChain3->GetDesc1(&d1);
    g_backBufferCount = d1.BufferCount;
    g_backBuffers.assign(g_backBufferCount, WrappedBackBuffer{});

    for (UINT i = 0; i < g_backBufferCount; ++i) {
        ID3D12Resource* native = nullptr;
        if (FAILED(g_swapChain3->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&native))) {
            std::cout << "[Render] GetBuffer(" << i << ") FAILED" << std::endl;
            continue;
        }
        D3D11_RESOURCE_FLAGS rf = {};
        rf.BindFlags = D3D11_BIND_RENDER_TARGET;
        ID3D11Resource* wrapped = nullptr;
        HRESULT hr = g_d3d11on12->CreateWrappedResource(
            native, &rf,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT,
            __uuidof(ID3D11Resource), (void**)&wrapped);
        if (FAILED(hr)) {
            std::cout << "[Render] CreateWrappedResource(" << i << ") FAILED hr=0x"
                      << std::hex << hr << std::dec << std::endl;
            native->Release();
            continue;
        }
        ID3D11RenderTargetView* rtv = nullptr;
        if (FAILED(d3d_device->CreateRenderTargetView(wrapped, nullptr, &rtv))) {
            std::cout << "[Render] CreateRenderTargetView(" << i << ") FAILED" << std::endl;
            wrapped->Release(); native->Release();
            continue;
        }
        g_backBuffers[i] = { native, wrapped, rtv };
    }
    return true;
}

static bool InitD3D11On12(IDXGISwapChain1* pSwapChain) {
    if (!g_d3d12Queue) {

        return false;
    }
    if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&g_d3d12Device))) {
        std::cout << "[Render] swap->GetDevice(ID3D12Device) FAILED" << std::endl;
        return false;
    }
    if (FAILED(pSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&g_swapChain3))) {
        std::cout << "[Render] QI(IDXGISwapChain3) FAILED" << std::endl;
        return false;
    }

    DXGI_SWAP_CHAIN_DESC desc = {};
    g_swapChain3->GetDesc(&desc);
    window_handle = desc.OutputWindow;

    DXGI_SWAP_CHAIN_DESC1 d1 = {};
    g_swapChain3->GetDesc1(&d1);
    std::cout << "[Render] SwapChain " << d1.Width << "x" << d1.Height
              << " fmt=" << d1.Format
              << " buffers=" << d1.BufferCount
              << " hwnd=" << window_handle << std::endl;

    IUnknown* queueAsUnknown = (IUnknown*)g_d3d12Queue;
    HRESULT hr = D3D11On12CreateDevice(
        g_d3d12Device,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0,
        &queueAsUnknown, 1, 0,
        &d3d_device, &d3d_context, nullptr);
    if (FAILED(hr)) {
        std::cout << "[Render] D3D11On12CreateDevice FAILED hr=0x"
                  << std::hex << hr << std::dec << std::endl;
        return false;
    }
    if (FAILED(d3d_device->QueryInterface(__uuidof(ID3D11On12Device), (void**)&g_d3d11on12))) {
        std::cout << "[Render] QI(ID3D11On12Device) FAILED" << std::endl;
        return false;
    }
    std::cout << "[Render] D3D11on12 device OK. d3d11=" << d3d_device
              << " on12=" << g_d3d11on12 << std::endl;

    if (!WrapBackBuffersFromSwapChain()) return false;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigInputTrickleEventQueue = false;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.MouseDrawCursor = false;
    ImGui_ImplWin32_Init(window_handle);
    ImGui_ImplDX11_Init(d3d_device, d3d_context);
    ApplyCustomStyleDefault();

    original_wnd_proc = (WNDPROC)SetWindowLongPtrA(
        window_handle, GWLP_WNDPROC, (LONG_PTR)WndProcHook);
    std::cout << "[Render] WndProc subclassed on game window. originalWndProc="
              << original_wnd_proc << std::endl;

    g_OverlayHwnd = window_handle;

    imgui_initialized = true;
    std::cout << "[Render] ImGui initialised against D3D11on12." << std::endl;
    return true;
}

static void RenderFrame() {
    if (!imgui_initialized || !g_swapChain3) return;

    if (GetAsyncKeyState(VK_INSERT) & 1) {
        show_imgui_menu = !show_imgui_menu;
        std::cout << "[Render] INSERT -> menu=" << (show_imgui_menu ? "OPEN" : "CLOSED") << std::endl;
    }
    if (GetAsyncKeyState(VK_END) & 1) {
        g_should_exit_thread = true;
    }

    __try {
        g_features.Run();
    } __except (EXCEPTION_EXECUTE_HANDLER) {}

    TickFpsUncap();

    EnsureZmodUIInit();

    SpriteAtlas::TryLoad(d3d_device);

    {
        UINT bidx = g_swapChain3->GetCurrentBackBufferIndex();
        if (bidx < g_backBuffers.size()) {
            auto& bbslot = g_backBuffers[bidx];
            if (bbslot.wrapped) {
                DXGI_SWAP_CHAIN_DESC1 d1{};
                if (SUCCEEDED(g_swapChain3->GetDesc1(&d1))) {
                    ID3D11Texture2D* bbTex = nullptr;
                    if (SUCCEEDED(bbslot.wrapped->QueryInterface(
                            __uuidof(ID3D11Texture2D), (void**)&bbTex)) && bbTex) {
                        if (ZmodUI::Blur::EnsureInit(d3d_device, d3d_context,
                                                     (int)d1.Width, (int)d1.Height, d1.Format)) {
                            g_d3d11on12->AcquireWrappedResources(&bbslot.wrapped, 1);
                            __try { ZmodUI::Blur::Capture(bbTex); }
                            __except (EXCEPTION_EXECUTE_HANDLER) {}
                            g_d3d11on12->ReleaseWrappedResources(&bbslot.wrapped, 1);
                        }
                        bbTex->Release();
                    }
                }
            }
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        auto stage = BootState::g_stage.load(std::memory_order_acquire);
        if (stage == BootState::Stage::Failed) {
            AuthBanner::Render();
        } else if (stage == BootState::Stage::FeaturesReady) {
            __try {
                PathRenderer::Render();
                AutoNether::Render();
                Pnb::Tick(0.f);
                AutoMine::Render();
                BankBot::UpdatePicker();
                Config::Tick();
                RenderNotifications();
                Commands::RenderPopups();
                Commands::Tick();
                ESP::RenderESP();
                MineDebug::Render();
            } __except (EXCEPTION_EXECUTE_HANDLER) {}
        }

    }

    if (show_imgui_menu) RenderApplicationMenu();
    if (Minimap::showMinimap) Minimap::RenderMinimap();

    ImGui::Render();

    UINT idx = g_swapChain3->GetCurrentBackBufferIndex();
    if (idx >= g_backBuffers.size()) return;
    auto& slot = g_backBuffers[idx];
    if (!slot.wrapped || !slot.rtv) return;

    g_d3d11on12->AcquireWrappedResources(&slot.wrapped, 1);
    __try {
        d3d_context->OMSetRenderTargets(1, &slot.rtv, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    } __except (EXCEPTION_EXECUTE_HANDLER) {

    }
    g_d3d11on12->ReleaseWrappedResources(&slot.wrapped, 1);
    d3d_context->Flush();
}

static bool InitD3D11Direct(IDXGISwapChain* pSwapChain) {
    if (!pSwapChain) return false;

    DXGI_SWAP_CHAIN_DESC desc = {};
    if (FAILED(pSwapChain->GetDesc(&desc))) return false;
    window_handle = desc.OutputWindow;

    if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device),
        reinterpret_cast<void**>(&d3d_device))))
    {
        std::cout << "[Render] D3D11 direct: GetDevice failed" << std::endl;
        return false;
    }
    d3d_device->GetImmediateContext(&d3d_context);
    if (!d3d_context) {
        std::cout << "[Render] D3D11 direct: GetImmediateContext returned null" << std::endl;
        return false;
    }

    ID3D11Texture2D* backBuf = nullptr;
    if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&backBuf))))
    {
        std::cout << "[Render] D3D11 direct: GetBuffer failed" << std::endl;
        return false;
    }
    HRESULT hr = d3d_device->CreateRenderTargetView(backBuf, nullptr,
                                                    &main_render_target_view);
    backBuf->Release();
    if (FAILED(hr)) {
        std::cout << "[Render] D3D11 direct: CreateRenderTargetView failed" << std::endl;
        return false;
    }

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigInputTrickleEventQueue = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.MouseDrawCursor = false;
    ImGui_ImplWin32_Init(window_handle);
    ImGui_ImplDX11_Init(d3d_device, d3d_context);

    extern LRESULT __stdcall WndProcHook(HWND, UINT, WPARAM, LPARAM);
    original_wnd_proc = (WNDPROC)SetWindowLongPtrW(window_handle, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(WndProcHook));

    g_swapChainBase  = pSwapChain;
    g_d3d11Native    = true;
    imgui_initialized = true;
    std::cout << "[Render] D3D11 direct path active. SwapChain=" << pSwapChain
              << " device=" << d3d_device << std::endl;
    return true;
}

static void RenderFrame_DX11() {
    if (!imgui_initialized || !main_render_target_view) return;

    if (GetAsyncKeyState(VK_INSERT) & 1) {
        show_imgui_menu = !show_imgui_menu;
        std::cout << "[Render] INSERT -> menu=" << (show_imgui_menu ? "OPEN" : "CLOSED") << std::endl;
    }
    if (GetAsyncKeyState(VK_END) & 1) {
        g_should_exit_thread = true;
    }

    __try { g_features.Run(); } __except (EXCEPTION_EXECUTE_HANDLER) {}
    TickFpsUncap();

    EnsureZmodUIInit();

    SpriteAtlas::TryLoad(d3d_device);

    if (g_swapChainBase) {
        ID3D11Texture2D* bbTex = nullptr;
        if (SUCCEEDED(g_swapChainBase->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                                 (void**)&bbTex)) && bbTex) {
            D3D11_TEXTURE2D_DESC td{}; bbTex->GetDesc(&td);
            if (ZmodUI::Blur::EnsureInit(d3d_device, d3d_context,
                                         (int)td.Width, (int)td.Height, td.Format)) {
                __try { ZmodUI::Blur::Capture(bbTex); }
                __except (EXCEPTION_EXECUTE_HANDLER) {}
            }
            bbTex->Release();
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        auto stage = BootState::g_stage.load(std::memory_order_acquire);
        if (stage == BootState::Stage::Failed) {
            AuthBanner::Render();
        } else if (stage == BootState::Stage::FeaturesReady) {
            __try {
                PathRenderer::Render();
                AutoNether::Render();
                Pnb::Tick(0.f);
                AutoMine::Render();
                BankBot::UpdatePicker();
                Config::Tick();
                RenderNotifications();
                Commands::RenderPopups();
                Commands::Tick();
                ESP::RenderESP();
                MineDebug::Render();
            } __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
    }

    if (show_imgui_menu) RenderApplicationMenu();
    if (Minimap::showMinimap) Minimap::RenderMinimap();

    ImGui::Render();

    __try {
        d3d_context->OMSetRenderTargets(1, &main_render_target_view, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
}

HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {

    const bool isTestPresent = (Flags & DXGI_PRESENT_TEST) != 0;

    if (!imgui_initialized && !g_d3d12Queue && !isTestPresent) {
        InitD3D11Direct(pSwapChain);
    }
    if (g_d3d11Native && imgui_initialized && !isTestPresent) {
        RenderFrame_DX11();
    }
    return original_present(pSwapChain, SyncInterval, Flags);
}

HRESULT __stdcall Present1Hook(IDXGISwapChain1* pSwap, UINT SyncInterval, UINT Flags,
                               const DXGI_PRESENT_PARAMETERS* pParams) {
    if (InterlockedIncrement(&g_present1Hits) == 1)
        std::cout << "[Render] >>> Present1 (vt 22) FIRED <<<" << std::endl;

    const bool isTestPresent = (Flags & DXGI_PRESENT_TEST) != 0;

    if (!imgui_initialized && g_d3d12Queue && !isTestPresent) {
        InitD3D11On12(pSwap);
    }
    if (!isTestPresent && imgui_initialized) {
        RenderFrame();
    }

    return g_originalPresent1(pSwap, SyncInterval, Flags, pParams);
}

HRESULT __stdcall ResizeBuffersHook(IDXGISwapChain* pSwap, UINT cnt,
                                    UINT W, UINT H, DXGI_FORMAT F, UINT flags) {
    InterlockedIncrement(&g_resizeHits);
    std::cout << "[Render] ResizeBuffers " << W << "x" << H << std::endl;

    ZmodUI::Blur::OnResize((int)W, (int)H);

    ReleaseWrappedBackBuffers();

    if (g_d3d11Native && main_render_target_view) {
        main_render_target_view->Release();
        main_render_target_view = nullptr;
    }

    HRESULT hr = g_originalResize(pSwap, cnt, W, H, F, flags);

    if (g_swapChain3 && g_d3d11on12) {
        WrapBackBuffersFromSwapChain();
    }
    if (g_d3d11Native && d3d_device) {
        ID3D11Texture2D* backBuf = nullptr;
        if (SUCCEEDED(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&backBuf))) && backBuf)
        {
            d3d_device->CreateRenderTargetView(backBuf, nullptr,
                                                &main_render_target_view);
            backBuf->Release();
        }
    }
    return hr;
}

void __stdcall ExecuteCommandListsHook(ID3D12CommandQueue* q, UINT n,
                                       ID3D12CommandList* const* lists) {
    if (InterlockedIncrement(&g_execHits) == 1)
        std::cout << "[Render] >>> ExecuteCommandLists FIRED <<< (D3D12 native)" << std::endl;

    if (!g_d3d12Queue && q) {
        D3D12_COMMAND_QUEUE_DESC desc = q->GetDesc();
        if (desc.Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
            g_d3d12Queue = q;
            g_d3d12Queue->AddRef();
            std::cout << "[Render] Captured D3D12 DIRECT queue: " << q << std::endl;
        }
    }
    g_originalExecuteCmds(q, n, lists);
}

static void* FindPresent1ViaDummy() {
    HMODULE d3d11 = GetModuleHandleA("d3d11.dll");
    if (!d3d11) {
        d3d11 = LoadLibraryA("d3d11.dll");
        if (!d3d11) return nullptr;
    }
    auto pCreate = (decltype(&D3D11CreateDevice))GetProcAddress(d3d11, "D3D11CreateDevice");
    if (!pCreate) return nullptr;

    ID3D11Device* dev = nullptr;
    ID3D11DeviceContext* ctx = nullptr;
    D3D_FEATURE_LEVEL fl;
    if (FAILED(pCreate(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                       nullptr, 0, D3D11_SDK_VERSION, &dev, &fl, &ctx))) {
        return nullptr;
    }
    IDXGIDevice* dxgiDev = nullptr;
    IDXGIAdapter* adp = nullptr;
    IDXGIFactory2* fac2 = nullptr;
    if (FAILED(dev->QueryInterface(__uuidof(IDXGIDevice),  (void**)&dxgiDev)) ||
        FAILED(dxgiDev->GetAdapter(&adp)) ||
        FAILED(adp->GetParent(__uuidof(IDXGIFactory2), (void**)&fac2))) {
        if (adp)     adp->Release();
        if (dxgiDev) dxgiDev->Release();
        ctx->Release(); dev->Release();
        return nullptr;
    }
    HWND dummy = CreateWindowExA(0, "STATIC", "RFDummy", WS_POPUP, 0, 0, 100, 100,
                                 nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width = 100; scd.Height = 100;
    scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    IDXGISwapChain1* sc1 = nullptr;
    void* p1 = nullptr;
    if (SUCCEEDED(fac2->CreateSwapChainForHwnd(dev, dummy, &scd, nullptr, nullptr, &sc1)) && sc1) {
        p1 = (*(void***)sc1)[22];
        sc1->Release();
    }
    if (dummy) DestroyWindow(dummy);
    fac2->Release(); adp->Release(); dxgiDev->Release();
    ctx->Release(); dev->Release();
    return p1;
}

static void* FindExecuteCommandListsViaDummy() {
    HMODULE d3d12 = GetModuleHandleA("d3d12.dll");
    if (!d3d12) return nullptr;
    typedef HRESULT(__stdcall * D3D12CreateDeviceFn)(IUnknown*, D3D_FEATURE_LEVEL, REFIID, void**);
    auto pCreate = (D3D12CreateDeviceFn)GetProcAddress(d3d12, "D3D12CreateDevice");
    if (!pCreate) return nullptr;

    ID3D12Device* dev = nullptr;
    if (FAILED(pCreate(nullptr, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&dev)))
        return nullptr;

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = 0;
    desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ID3D12CommandQueue* q = nullptr;
    if (FAILED(dev->CreateCommandQueue(&desc, __uuidof(ID3D12CommandQueue), (void**)&q))) {
        dev->Release();
        return nullptr;
    }
    void* exec = (*(void***)q)[10];
    q->Release();
    dev->Release();
    return exec;
}
void printRainbowAscii() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    const WORD colors[] = { 11, 3, 9, 11, 3, 9 };
    const std::string ascii =
        " _____ __  __           _ \n"
        "|__  /|  \\/  | ___   __| |\n"
        "  / / | |\\/| |/ _ \\ / _` |\n"
        " / /_ | |  | | (_) | (_| |\n"
        "/____||_|  |_|\\___/ \\__,_|\n";

    std::stringstream ss(ascii);
    std::string line;
    int colorIndex = 0;

    while (std::getline(ss, line)) {
        SetConsoleTextAttribute(hConsole, colors[colorIndex % 6]);
        std::cout << line << std::endl;
        colorIndex++;
    }
    SetConsoleTextAttribute(hConsole, 7);
}

int main() {
 return 0;
}

DWORD WINAPI MainThread(LPVOID hInst) {

    AttachConsoleToDLL();

    CrashLogger::Install();
    BootState::Set(BootState::Stage::Init, 0);

    if (MH_Initialize() != MH_OK) {
        BootState::Fail("boot: MinHook init failed");
        FreeLibraryAndExitThread((HMODULE)hInst, 1);
        return 1;
    }
    std::cout << "[Main] MinHook Initialized." << std::endl;

    Sentry::Start();

    windows_username_str = GetWindowsUsername();
    std::cout << "[+] Welcome, " << windows_username_str << "!" << std::endl;
    printRainbowAscii();

    HMODULE gameAsm = nullptr;
    {
        std::cout << "[Main] Waiting for GameAssembly.dll..." << std::endl;
        for (int attempt = 0; attempt < 60 && !gameAsm; ++attempt) {
            gameAsm = GetModuleHandleA("GameAssembly.dll");
            if (!gameAsm) Sleep(1000);
        }
        if (gameAsm) {
            moduleBase = (uintptr_t)gameAsm;
        } else {
            std::cout << "[Main] GameAssembly.dll not loaded after 60s — feature wiring will fail." << std::endl;
        }
    }
    BootState::Set(BootState::Stage::Init, 15);

    BootState::Set(BootState::Stage::AuthPending, 25);
    bool bootHardFailed = false;
    {
        auto r = DllAuthClient::Authenticate();
        if (r == DllAuthClient::Result::Permanent) {
            std::cout << XS("[DllAuth] FATAL: ") << DllAuthClient::g_lastErrorMsg
                      << XS(" — features disabled; banner will show.") << std::endl;
            BootState::Fail(DllAuthClient::g_lastErrorMsg.empty()
                ? std::string("Authentication failed")
                : DllAuthClient::g_lastErrorMsg);
            bootHardFailed = true;
        } else if (r == DllAuthClient::Result::Transient) {

            std::cout << XS("[DllAuth] WARN (transient): ") << DllAuthClient::g_lastErrorMsg
                      << std::endl;
            DllAuthClient::g_authFatal.store(true, std::memory_order_release);
            BootState::Fail(DllAuthClient::g_lastErrorMsg.empty()
                ? std::string("Auth transient failure — retry the loader.")
                : DllAuthClient::g_lastErrorMsg);
            bootHardFailed = true;
        }

    }
    if (!bootHardFailed) {
        BootState::Set(BootState::Stage::AuthOk, 45);

        BootState::Set(BootState::Stage::ManifestPending, 55);
        bool il2ok = false;
        try {
            std::cout << "[Main] Fetching manifest from worker..." << std::endl;
            il2ok = Manifest::Init((HMODULE)hInst);
            std::cout << "[Main] Manifest loaded = " << (il2ok ? "true" : "false") << std::endl;
        } catch (...) {
            std::cout << "[Main] EXCEPTION inside Manifest::Init" << std::endl;
            BootState::Fail("manifest: exception during init");
        }
        if (!il2ok) {

            DllAuthClient::g_authFatal.store(true, std::memory_order_release);
            bootHardFailed = true;
        } else {
            BootState::Set(BootState::Stage::ManifestOk, 80);

            if (gameAsm) {
                ACTkBypass::Install();
            }
        }
    }

    if (bootHardFailed) {
        std::cout << XS("[Main] boot failed — skipping feature wiring; render-only mode.") << std::endl;
    } else try {

        InitializeIl2CppHelpers();
        std::cout << "[Main] IL2CPP helpers wired from manifest. "
                  << "string_new=" << (void*)oIl2CppStringNew
                  << " array_new=" << (void*)il2cpp_array_new
                  << std::endl;

        std::cout << "[Main] Multibox: InstallHook()..." << std::endl;
        MultiboxProfile::InstallHook();
        std::cout << "[Main] Multibox: InstallHook done." << std::endl;

        std::cout << "[Main] SetupHooks()..." << std::endl;
        SetupHooks();
        ESP::SetupFunctionPointers();
        ResolveFpsUncapSetters();

        Commands::SetupHooks();

        KickLogger::Install();

        AutoMine::InstallMineResultsHook();

        BankBot::InstallSafeBoxUIHook();

        BankBot::InstallInputHooks();

        MultiboxMode::ResolveMethods();
        MultiboxMode::InstallHooks();
        MultiboxProfile::InstallHook();

        ReplyDispatch::SetHandlers(
            &AutoMine::OnMPRReplyDelivered,
            &AutoMine::AutoLoop::OnCIReplyDelivered,
            &GemSeller::OnRAReplyDelivered
        );
        std::cout << "[Main] ReplyDispatch handlers registered.\n";

        std::cout << "[Main] SetupHooks() done." << std::endl;

        if (Config::Load())
            std::cout << "[Config] Loaded " << Config::GetConfigPath().string() << std::endl;
        else
            std::cout << "[Config] No prior config; using defaults." << std::endl;

        PathRenderer::Initialize();

        std::cout << "[Main] Game hooks ready." << std::endl;
    } catch (...) {
        std::cout << "[Main] EXCEPTION during game hook init!" << std::endl;
    }
    if (!bootHardFailed) {
        BootState::Set(BootState::Stage::FeaturesReady, 100);
    }

    auto kStatus = kiero::init(kiero::RenderType::D3D11);
    if (kStatus == kiero::Status::Success) {
        g_kiero_initialized_successfully = true;
        if (kiero::bind(8, (void**)&original_present, PresentHook) != kiero::Status::Success)
            std::cout << "[Render] kiero::bind(8 Present) FAILED" << std::endl;
        else
            std::cout << "[Render] Present (vt 8) bound." << std::endl;
        if (kiero::bind(13, (void**)&g_originalResize, ResizeBuffersHook) != kiero::Status::Success)
            std::cout << "[Render] kiero::bind(13 ResizeBuffers) FAILED" << std::endl;
        else
            std::cout << "[Render] ResizeBuffers (vt 13) bound." << std::endl;
    } else {
        std::cout << "[Render] kiero::init(D3D11) FAILED status=" << (int)kStatus
                  << " - render hooks for vt8/vt13 skipped." << std::endl;
    }

    if (void* p1 = FindPresent1ViaDummy()) {
        if (MH_CreateHook(p1, &Present1Hook, (void**)&g_originalPresent1) == MH_OK &&
            MH_EnableHook(p1) == MH_OK) {
            std::cout << "[Render] Present1 (vt 22) bound at " << p1 << std::endl;
        } else {
            std::cout << "[Render] Present1 MH_CreateHook/Enable FAILED" << std::endl;
        }
    } else {
        std::cout << "[Render] FindPresent1ViaDummy returned NULL." << std::endl;
    }

    if (void* exec = FindExecuteCommandListsViaDummy()) {
        if (MH_CreateHook(exec, &ExecuteCommandListsHook, (void**)&g_originalExecuteCmds) == MH_OK &&
            MH_EnableHook(exec) == MH_OK) {
            std::cout << "[Render] ExecuteCommandLists (vt 10) bound at " << exec << std::endl;
        } else {
            std::cout << "[Render] ExecuteCommandLists MH_CreateHook/Enable FAILED" << std::endl;
        }
    } else {
        std::cout << "[Render] FindExecuteCommandListsViaDummy returned NULL "
                     "(d3d12.dll not loaded yet?)." << std::endl;
    }

    std::cout << "[Main] Render hooks installed. Press INSERT in-game for menu, END to unload."
              << std::endl;

#ifdef USE_SENTRY
    SelfCRC::Initialize((HMODULE)hInst);
    VEHAntiDebug::Install();
    Stealth::Apply((HMODULE)hInst);
#endif

    while (!g_should_exit_thread) Sleep(100);

    PerformFullCleanup();
    std::cout << "[Main] Exiting." << std::endl;
    FreeLibraryAndExitThread((HMODULE)hInst, 0);
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
     
        DisableThreadLibraryCalls(hInst);
        if (HANDLE hThread = CreateThread(nullptr, 0, MainThread, hInst, 0, nullptr)) {
         
            CloseHandle(hThread);
        }
        else {
         
            return FALSE;
        }
        break;
    }

    return TRUE;
}

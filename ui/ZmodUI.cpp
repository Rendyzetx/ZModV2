

#include "ZmodUI.h"
#include "Internal.h"
#include "Tokens.h"
#include "Theme.h"
#include "Fonts.h"
#include "Tween.h"
#include "widgets/Primitives.h"
#include <cstring>
#include <cstdio>

namespace ZmodUI {

namespace { Ctx g_ctx; }
Ctx& GetCtx() { return g_ctx; }

void SetActiveRail(const char* key) {
    Ctx& c = GetCtx();
    if (!key || !*key) return;
    if (std::strcmp(c.active_rail, key) == 0) return;
    std::strncpy(c.last_rail, c.active_rail, sizeof(c.last_rail)-1);
    std::strncpy(c.active_rail, key, sizeof(c.active_rail)-1);
    c.rail_just_changed = true;
    c.rail_fade_t = 0.f;
    c.scroll_y = 0.f;
}
const char* ActiveRail() { return GetCtx().active_rail; }

void Init() {
    Theme::Init();
    Fonts::Init();
    DrawerInit();
    PaletteInit();
}

void Shutdown() {
    Fonts::Shutdown();
}

void StartBoot() {
    BootStart();
}

void Toast(State& s, const char* msg, float dur) {
    if (!msg) return;
    std::strncpy(s.toast_request.msg, msg, sizeof(s.toast_request.msg)-1);
    s.toast_request.duration_sec = dur;
    s.toast_request.pending = true;
}

bool IsPaletteOpen() { return GetCtx().palette_open; }
void OpenPalette()   { GetCtx().palette_open = true; GetCtx().palette_open_frame = ImGui::GetFrameCount(); }
void ClosePalette()  { GetCtx().palette_open = false; GetCtx().palette_query[0] = 0; GetCtx().palette_sel = 0; }

void OpenDrawer(const char* key)  { OpenDrawerImpl(key); }
void CloseDrawer()                { CloseDrawerImpl(); }
bool IsDrawerOpen(const char* key) {
    Ctx& c = GetCtx();
    if (!c.drawer_open) return false;
    if (!key || !*key)  return true;
    return std::strcmp(c.drawer_key, key) == 0;
}

static void RenderActiveRail(State& s) {
    Ctx& c = GetCtx();
    if (!std::strcmp(c.active_rail, "dashboard"))   RailDashboard(s);
    else if (!std::strcmp(c.active_rail, "combat")) RailCombat(s);
    else if (!std::strcmp(c.active_rail, "movement"))   RailMovement(s);
    else if (!std::strcmp(c.active_rail, "visuals"))    RailVisuals(s);
    else if (!std::strcmp(c.active_rail, "network"))    RailNetwork(s);
    else if (!std::strcmp(c.active_rail, "misc"))       RailMisc(s);
    else if (!std::strcmp(c.active_rail, "auto-mine"))  RailAutoMine(s);
    else if (!std::strcmp(c.active_rail, "auto-fish"))  RailAutoFish(s);
    else if (!std::strcmp(c.active_rail, "auto-fossil"))RailAutoFossil(s);
    else if (!std::strcmp(c.active_rail, "auto-nether"))RailAutoNether(s);
    else if (!std::strcmp(c.active_rail, "auto-pnb"))   RailPnb(s);
    else if (!std::strcmp(c.active_rail, "proxies"))    RailProxies(s);
    else if (!std::strcmp(c.active_rail, "store"))      RailStore(s);
    else if (!std::strcmp(c.active_rail, "settings"))   RailSettings(s);
#ifndef ZMOD_V3
    else if (!std::strcmp(c.active_rail, "debug"))      RailDebug(s);
#endif
}

static void RenderWindowFrame() {
    Ctx& c = GetCtx();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    ImVec2 a = c.win_pos;
    ImVec2 b = ImVec2(a.x + c.win_size.x, a.y + c.win_size.y);
    float r = Tokens::Size::WindowRadius;

    Draw::DropShadow(dl, a, b, r, IM_COL32(0, 0, 0, 80), 3);

    if (Theme::ShouldUseBackdropBlur()) {

        Draw::GlassPanel(dl, a, b, r,
                         Tokens::Color::Bg(),
                         Tokens::Color::Line());
    } else {

        dl->AddRectFilled(a, b, Tokens::Color::Bg(), r);
        dl->AddRect      (a, b, Tokens::Color::Line(), r, 0, 1.f);
    }
}

void Render(State& s) {
    Theme::PollOsTheme();
    if (!s.show) return;

    ImGuiIO& io = ImGui::GetIO();
    Ctx& c = GetCtx();
    c.dpi = io.DisplayFramebufferScale.x > 0 ? io.DisplayFramebufferScale.x : 1.f;

    if (c.win_user_w <= 0.f) c.win_user_w = Tokens::Size::WindowDefaultW;
    float w = c.win_user_w;
    float h = w / Tokens::Size::WindowAspect;
    if (w > io.DisplaySize.x) { w = io.DisplaySize.x; h = w / Tokens::Size::WindowAspect; }
    if (h > io.DisplaySize.y) { h = io.DisplaySize.y; w = h * Tokens::Size::WindowAspect; }

    float boot_scale = 1.f;
    float boot_slide = 0.f;
    if (c.boot_active) {
        float p = c.boot_ms / 600.f;
        if (p < 0.f) p = 0.f; if (p > 1.f) p = 1.f;
        p = Tokens::Motion::MacEase(p);
        boot_scale = 0.88f + 0.12f * p;
        boot_slide = (1.f - p) * 14.f;
    }
    w *= boot_scale;
    h *= boot_scale;
    c.win_size = ImVec2(w, h);

    bool have_user_pos = (c.win_user_pos.x > -1e8f);
    if (have_user_pos && !c.boot_active) {
        float minX = -w * 0.6f;
        float maxX = io.DisplaySize.x - w * 0.4f;
        float minY = 0.f;
        float maxY = io.DisplaySize.y - Tokens::Size::TitleBarH;
        if (c.win_user_pos.x < minX) c.win_user_pos.x = minX;
        if (c.win_user_pos.x > maxX) c.win_user_pos.x = maxX;
        if (c.win_user_pos.y < minY) c.win_user_pos.y = minY;
        if (c.win_user_pos.y > maxY) c.win_user_pos.y = maxY;
        c.win_pos = c.win_user_pos;
    } else {
        c.win_pos = ImVec2((io.DisplaySize.x - w) * 0.5f,
                           (io.DisplaySize.y - h) * 0.5f + boot_slide);
    }

    ImGui::SetNextWindowPos(c.win_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(c.win_size, ImGuiCond_Always);
    ImGuiWindowFlags wf = ImGuiWindowFlags_NoTitleBar
                       | ImGuiWindowFlags_NoResize
                       | ImGuiWindowFlags_NoScrollbar
                       | ImGuiWindowFlags_NoMove
                       | ImGuiWindowFlags_NoCollapse
                       | ImGuiWindowFlags_NoBackground
                       | ImGuiWindowFlags_NoBringToFrontOnFocus
                       | ImGuiWindowFlags_NoSavedSettings
                       | ImGuiWindowFlags_NoNavFocus;
    ImGui::Begin("##ZmodUI_Host", nullptr, wf);
    c.dl = ImGui::GetWindowDrawList();

    RenderWindowFrame();

    TitleBarRender(s);
    SidebarRender(s);
    StatusStripRender(s);
    ResizeGripRender(s);

    float content_x0 = c.win_pos.x + Tokens::Size::SidebarW + 32.f;
    float content_x1 = c.win_pos.x + c.win_size.x - 32.f;
    if (c.drawer_open || c.drawer_t > 0.01f) {
        content_x1 = c.win_pos.x + c.win_size.x - Tokens::Size::DrawerW;

        float t = c.drawer_t;
        content_x1 = c.win_pos.x + c.win_size.x - (Tokens::Size::DrawerW * t) - 32.f * (1.f - t);
    }
    c.content_x0 = content_x0;
    c.content_x1 = content_x1;
    c.view_y0    = c.win_pos.y + Tokens::Size::TitleBarH + 4.f;
    c.view_y1    = c.win_pos.y + c.win_size.y - Tokens::Size::StatusStripH;

    ImVec2 main_a = ImVec2(c.win_pos.x + Tokens::Size::SidebarW, c.view_y0);
    ImVec2 main_b = ImVec2(content_x1 + 32.f, c.view_y1);
    c.dl->PushClipRect(main_a, main_b, true);

    if (ImGui::IsMouseHoveringRect(main_a, main_b) && !c.palette_open) {
        if (io.MouseWheel != 0.f) {
            c.scroll_y -= io.MouseWheel * 40.f;
            c.main_scroll_show_t = 1.f;
        }
    }
    if (c.scroll_y < 0)                    c.scroll_y = 0;
    if (c.scroll_y > c.main_max_scroll)    c.scroll_y = c.main_max_scroll;
    c.cursor = ImVec2(0.f, 22.f);

    if (c.rail_just_changed) c.rail_just_changed = false;
    if (c.rail_fade_t < 1.f) {
        c.rail_fade_t += io.DeltaTime * 1000.f / Tokens::Motion::PaneFadeMs;
        if (c.rail_fade_t > 1.f) c.rail_fade_t = 1.f;
    }

    RenderActiveRail(s);

    float content_end = c.cursor.y + 60.f;
    float view_h = c.view_y1 - c.view_y0;
    float max_scroll = content_end - view_h;
    if (max_scroll < 0) max_scroll = 0;
    c.main_max_scroll = max_scroll;
    c.main_content_h  = content_end;

    c.main_scroll_show_t -= io.DeltaTime * 1.2f;
    if (c.main_scroll_show_t < 0.f) c.main_scroll_show_t = 0.f;
    if (max_scroll > 0.f && c.main_scroll_show_t > 0.01f) {
        float thumb_ratio = view_h / content_end;
        if (thumb_ratio > 1.f) thumb_ratio = 1.f;
        float thumb_h = view_h * thumb_ratio;
        if (thumb_h < 30.f) thumb_h = 30.f;
        float thumb_y = c.view_y0 + (c.scroll_y / max_scroll) * (view_h - thumb_h);
        float thumb_x = main_b.x - 8.f;
        ImU32 base = Theme::IsDarkMode() ? IM_COL32(255,255,255,90) : IM_COL32(0,0,0,90);
        ImU32 col  = (base & 0x00FFFFFF) | (((ImU32)(((base >> 24) & 0xFF) * c.main_scroll_show_t)) << 24);
        Draw::RectFilledRounded(c.dl, ImVec2(thumb_x, thumb_y),
                                ImVec2(thumb_x + 4.f, thumb_y + thumb_h), col, 2.f);
    }

    c.dl->PopClipRect();

    DrawerRenderAll(s);

    DrawComboPopup();

    ToastRender(s);
    BootRender(s);
    PaletteRender(s);

    if (ImGui::IsKeyPressed(ImGuiKey_K) && (io.KeyCtrl || io.KeySuper)) {
        if (c.palette_open) ClosePalette();
        else                OpenPalette();
    }

    ImGui::End();
}

}

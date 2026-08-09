

#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "ZmodUI.h"
#include "Tokens.h"
#include "Tween.h"
#include <cstdio>

namespace ZmodUI {

struct Ctx {
    ImDrawList* dl       = nullptr;
    ImVec2      win_pos  = {};
    ImVec2      win_size = {};
    float       dpi      = 1.f;

    ImVec2      cursor   = {};
    float       content_x0 = 0.f, content_x1 = 0.f;
    float       content_y0 = 0.f;
    float       scroll_y = 0.f;
    float       view_y0  = 0.f, view_y1 = 0.f;

    bool        in_group = false;
    ImVec2      group_start = {};
    int         group_row_count = 0;
    bool        group_hover_active = false;
    ImVec2      group_hover_a = {};
    ImVec2      group_hover_b = {};
    int         group_hover_row_idx = -1;

    char        drawer_key[32] = "";
    char        pending_drawer[32] = "";
    bool        drawer_open = false;
    float       drawer_t = 0.f;
    bool        drawer_back_request = false;
    float       drawer_scroll_y = 0.f;
    float       drawer_max_scroll = 0.f;
    float       drawer_content_h  = 0.f;
    float       drawer_scroll_show_t = 0.f;

    float       main_max_scroll  = 0.f;
    float       main_content_h   = 0.f;
    float       main_scroll_show_t = 0.f;

    bool        palette_open = false;
    char        palette_query[128] = "";
    int         palette_sel = 0;
    float       palette_t = 0.f;
    int         palette_open_frame = -2;
    int         esp_drawer_idx = 0;

    char        toast_msg[128] = "";
    float       toast_dur = 0.f;
    float       toast_age = 0.f;
    float       toast_t  = 0.f;

    bool        boot_active = false;
    float       boot_ms = 0.f;
    int         boot_status_idx = -1;

    char        active_rail[32] = "dashboard";
    bool        rail_just_changed = false;
    float       rail_fade_t = 1.f;
    char        last_rail[32] = "dashboard";

    bool        resizing = false;
    ImVec2      resize_drag_origin = {};
    float       win_user_w = 1080.f;

    ImVec2      win_user_pos      = ImVec2(-1e9f, -1e9f);
    bool        dragging          = false;
    ImVec2      drag_grab_offset  = {};

    ImGuiID     hot_id = 0;
};

Ctx& GetCtx();

namespace Layout {

    void  AdvanceY(float h);
    void  AdvanceCursor(float w, float h);

    ImVec2 Cursor();
    float  ColumnWidth();
    void   PushColumn(float x0, float x1);
    void   PopColumn();

    inline float ContentToScreenY(float y) {
        Ctx& c = GetCtx();
        return c.view_y0 + (y - c.scroll_y);
    }

    struct Interact {
        bool hovered, clicked, held, pressed;
    };
    Interact RegisterHit(const char* id, ImVec2 a, ImVec2 b);
}

namespace Draw {
    void RectFilledRounded(ImDrawList* dl, ImVec2 a, ImVec2 b, ImU32 col, float radius);
    void RectStrokeRounded(ImDrawList* dl, ImVec2 a, ImVec2 b, ImU32 col, float radius, float thickness = 1.f);
    void DropShadow(ImDrawList* dl, ImVec2 a, ImVec2 b, float radius, ImU32 base_col, int layers = 3);

    void SoftHaloRect(ImDrawList* dl, ImVec2 a, ImVec2 b, float radius,
                      ImU32 glow_col, float spread, int N = 14);
    void SoftHaloCircle(ImDrawList* dl, ImVec2 center, float inner_r, float outer_r,
                        ImU32 glow_col, int N = 16);

    void GlassPanel(ImDrawList* dl, ImVec2 a, ImVec2 b, float radius,
                    ImU32 tint_col, ImU32 border_col,
                    ImDrawFlags corner_flags = 0);

    bool StyledColorPickerPopup(const char* popup_id, const char* header,
                                ImVec4* color, bool show_alpha = false);
    void TextL(ImDrawList* dl, ImFont* font, ImVec2 pos, ImU32 col, const char* text);
    ImVec2 TextSize(ImFont* font, const char* text);

    void IconVec(ImDrawList* dl, ImVec2 center, float size, ImU32 col, int icon_id);

    void IconTile(ImDrawList* dl, ImVec2 pos, float size, ImU32 bg, ImU32 fg,
                  int icon_id, float radius = 6.f, int atlas_block_id = 0);

    void Knob(ImDrawList* dl, ImVec2 center, float radius, ImU32 fill);

    void PushClipText(ImDrawList* dl, ImVec2 a, ImVec2 b);
    void PopClipText(ImDrawList* dl);
}

void SetActiveRail(const char* key);
const char* ActiveRail();

void DrawerInit();
void DrawerRenderAll(State& s);
void OpenDrawerImpl(const char* key);
void CloseDrawerImpl();

void DrawComboPopup();

bool ComboPopupBlocksPoint(ImVec2 p);

void PaletteInit();
void PaletteRender(State& s);
struct PaletteEntry { const char* label; const char* rail; const char* drawer_key; };
const PaletteEntry* PaletteRegistry(int* count);

void ToastRender(State& s);

void BootStart();
void BootRender(State& s);

void TitleBarRender(State& s);
void SidebarRender(State& s);
void StatusStripRender(State& s);
void ResizeGripRender(State& s);

void RailDashboard(State& s);
void RailCombat(State& s);
void RailMovement(State& s);
void RailVisuals(State& s);
void RailNetwork(State& s);
void RailMisc(State& s);
void RailAutoMine(State& s);
void RailAutoFish(State& s);
void RailAutoFossil(State& s);
void RailAutoNether(State& s);
void RailPnb(State& s);
void RailProxies(State& s);
void RailStore(State& s);
void RailSettings(State& s);
void RailDebug(State& s);

}



#pragma once
#include "imgui.h"
#include <string>

namespace ZmodUI {

struct State {
    bool show          = true;
    bool boot_complete = false;

    char  username[64]   = "Player";
    char  hwid_short[16] = "3F1E\xE2\x80\xA6" "A902";
    char  license[32]    = "Lifetime License";
    char  version[16]    = "dev";
    int   level          = 1;
    long  bytecoins      = 0;
    long  gems           = 0;
    int   fps            = 0;
    int   ping_ms        = 0;
    int   cpu_pct        = 0;
    int   ram_mb         = 0;
    long  uptime_sec     = 0;
    bool  streamer_mode  = false;

    bool  proxy_enabled  = false;
    char  proxy_host[64]  = "";
    int   proxy_port      = 8080;
    char  proxy_port_buf[8] = "8080";
    bool  proxy_use_auth  = false;
    char  proxy_user[64]  = "";
    char  proxy_pass[64]  = "";
    int   proxy_scheme    = 0;
    bool  proxy_tunnel_game = false;
    char  proxy_game_import[4096] = "";
    char  proxy_auth_import[4096] = "";
    int   proxy_check_req  = 0;
    bool  proxy_pool_dirty = false;

    bool god_mode = false, anti_trap = false;
    bool anti_bounce = false, ignore_mushrooms = false;
    bool anti_fan = false, anti_deflector = false;
    bool anti_block_death = false, unlimited_oxygen = false, anti_afk = false;
    bool ai_aimbot = false, block_on_player = false;
    bool anti_poison = false;

    bool door_walk_through = false;

    bool gm_master_swap = false;

    bool damage_debug_log = false;

    bool  actk_bypass = true;

    bool  player_runspeed_hack = false;
    float player_runspeed_mul  = 1.0f;

    float player_speed = 1.0f;
    bool  jump_override = false; int jump_mode = 0;
    bool  grav_override = false; int grav_mode = 0;
    bool  infinite_jetpack = false;
    bool  keys_to_fly = false;   float fly_speed = 7.0f;
    bool  auto_navigate = false;
    bool  pf_show_path = true, pf_auto_move = false;
    float pf_move_speed = 2.5f;
    float pf_stop_range = 0.30f;
    bool  pf_airborne_check = true; int pf_max_airborne = 6;
    float pf_body_offset = 0.50f;
    bool  pf_walk_instakill = false;

    bool  pf_strict_traps = false;

    int   pf_hazard_buffer = 0;
    bool  pf_glow_halo = true, pf_flow_particles = true, pf_pulsing_target = true;
    bool  pf_block_overlay = false;

    bool  pf_dbg_stats_hud   = false;
    bool  pf_dbg_inspect     = false;
    bool  pf_dbg_verbose_log = false;

    int   pf_move_method        = 0;
    float pf_portal_interval_ms = 100.f;

    int   pf_tp_interval_ms     = 100;

    int   pf_tp_jitter_ms       = 20;

    bool  pf_grav_pulse         = false;
    int   pf_grav_pulse_on_ms   = 1500;
    int   pf_grav_pulse_off_ms  = 100;

    bool  pf_tp_throttle_packets = false;
    bool  pf_tp_zero_velocity    = false;
    bool  pf_tp_force_grounded   = false;
    bool  pf_tp_suppress_gravity = false;

    bool show_minimap = true, free_cam = false;
    bool extra_zoom = false; float extra_zoom_mul = 3.0f;
    bool block_tooltip = false;
    bool esp_master = false,  esp_show_info = false;
    bool   esp_lines[11]   = {};
    bool   esp_boxes[11]   = {};
    bool   esp_health[11]  = {};
    ImVec4 esp_color[11]   = {};
    bool gem_chip[5]       = {};
    bool crystal_chip[7]   = {};
    bool anti_darkness = false;
    bool force_weather = false; int weather_idx = 0;
    bool force_lighting = false; int lighting_idx = 0;
    int  background_idx = 0;

    bool  am_enabled = false;
    bool  am_auto_repair = true;
    int   am_repair_threshold = 50;

    bool  am_premine_hb            = false;
    int   am_premine_radius        = 2;
    bool  am_premine_include_solid = false;
    float am_speed = 2.5f;
    bool  am_levels[5]    = {};
    bool  am_anti_sleep = true, am_anti_afk = true;
    bool  am_auto_combine = false;
    bool  am_pf_auto_move = false, am_pf_draw_route = true;
    float am_pf_move_speed = 2.5f;
    float am_pf_stop_range = 0.30f, am_pf_body_offset = 0.50f;
    bool  am_pf_prefer_staircase = true; int am_pf_up_penalty = 30, am_pf_up_extra = 80;
    bool  am_pf_airborne_check = true;   int am_pf_max_airborne = 6;
    int   am_pf_move_method        = 0;
    float am_pf_portal_interval_ms = 100.f;

    bool  am_pf_tp_throttle_packets = false;
    bool  am_pf_tp_zero_velocity    = false;
    bool  am_pf_tp_force_grounded   = false;
    bool  am_pf_tp_suppress_gravity = false;
    bool  am_t_gemstones = true, am_t_nuggets = true, am_t_light_crystals = true;
    bool  am_t_pots = true, am_t_floor_collectables = true, am_t_ai_enemies = true, am_t_exit_portal = true;
    bool  am_c_hit_ai = true; int am_c_range = 3;
    float am_c_cadence = 0.30f; float am_c_hit_cadence = 0.20f;
    bool  am_bb_auto_deposit = false; int am_bb_delay_ms = 850;
    bool  am_bb_nuggets[5]   = { true, true, true, true, true };
    bool  am_bb_keys[5]      = { true, true, true, true, true };
    bool  am_bb_materials[9] = { true, true, true, true, true, true, true, true, true };
    bool  am_bb_gems[5][5]   = {};
    bool  am_as_enabled = false;
    bool  am_as_gems[5][5]   = {};
    bool  am_rj_enabled = false; int am_rj_delay_ms = 2500;

    bool  af_enabled = false;
    bool  af_auto_land = true, af_auto_recast = true, af_anti_sleep = true, af_anti_afk = true;
    float af_recast_min = 1.5f, af_recast_max = 4.0f;
    float af_hook_min = 180.f,  af_hook_max = 420.f;
    bool  af_legit_mode = false;
    float af_pause_delay_min = 6.0f, af_pause_delay_max = 15.0f;
    float af_pause_dur_min = 0.8f, af_pause_dur_max = 2.2f;
    bool  af_verbose_minigame = false;

    float af_speed_mult = 1.0f;

    bool  af_return_to_spot  = true;
    float af_max_drift_tiles = 3.0f;

    bool  af_size_filter   = false;
    bool  af_keep_size[5]  = { true, true, true, true, true };
    float af_last_fish_size = 0.f;
    int   af_last_fish_bucket = -1;

    bool  ff_enabled = false;
    bool  ff_repeat = true;
    bool  ff_raw_moves = true;
    int   ff_target = 0;
    char  ff_target_buf[16] = "0";
    float ff_move_min = 160.f, ff_move_max = 420.f;
    bool  ff_verbose = false;

    int   ff_solved = 0, ff_failed = 0, ff_moves = 0;
    const char* ff_status = "Idle";

    const char* ff_diag_id  = "-";
    const char* ff_diag_raw = "-";
    int         ff_diag_solve = -1;
    const char* ff_diag_start = "-";
    const char* ff_diag_inst  = "-";
    const char* ff_diag_run   = "-";
    const char* ff_diag_via   = "-";

    bool  an_enabled = false;
    bool  an_auto_move = true, an_draw_route = true;
    float an_flight_speed = 1.0f;
    bool  an_t_ai = true, an_t_treasure = true, an_t_gifts = true;
    bool  an_t_collectables = true, an_t_exit = true;
    bool  an_anti_sleep = true, an_anti_afk = true;
    bool  an_prioritise_closest = true;
    float an_melee_cooldown = 0.40f;
    float an_max_melee_range = 4.0f;
    float an_stop_distance = 0.50f;
    float an_min_enemy_hp = 0.f;
    float an_pf_move_speed = 2.5f, an_pf_stop_range = 0.30f, an_pf_body_offset = 0.50f;
    bool  an_pf_airborne_check = true; int an_pf_max_airborne = 6;
    bool  an_pf_chamfer = true; float an_pf_chamfer_amount = 0.20f;
    bool  an_pf_walk_instakill = false;
    bool  an_pf_glow_halo = true, an_pf_flow_particles = true, an_pf_pulsing_target = true;
    int   an_pf_move_method        = 0;
    float an_pf_portal_interval_ms = 100.f;

    bool  an_pf_tp_throttle_packets = false;
    bool  an_pf_tp_zero_velocity    = false;
    bool  an_pf_tp_force_grounded   = false;
    bool  an_pf_tp_suppress_gravity = false;
    bool  an_deep_loop    = false;
    bool  an_deep_net_join = false;
    int   an_deep_scrolls = 0;
    int   an_deep_runs    = 0;
    const char* an_deep_msg = nullptr;

    bool  an_reg_loop     = false;
    bool  an_reg_levels[5] = {};
    int   an_reg_level    = 0;
    int   an_reg_runs     = 0;
    int   an_reg_scrolls  = 0;
    const char* an_reg_msg = nullptr;
    const char* an_fault_msg = nullptr;

    bool  pnb_enabled       = false;
    bool  pnb_offsets[5][5] = {};
    bool  pnb_auto_collect  = false;
    bool  pnb_has_anchor    = false;
    int   pnb_anchor_x = 0, pnb_anchor_y = 0;
    int   pnb_sel_block_type = 0;
    int   pnb_sel_qty        = 0;
    int   pnb_inv_block[24] = {};
    int   pnb_inv_qty[24]   = {};
    char  pnb_inv_name[24][64] = {};
    int   pnb_inv_count     = 0;
    int   pnb_sel_idx       = 0;
    const char* pnb_phase_label = "idle";
    void (*on_pnb_save_anchor)() = nullptr;

    bool  net_capture_in = true, net_capture_out = true, net_auto_scroll = true;
    int   net_direction_filter = 0;
    char  net_id_filter[64] = "";
    char  net_repeater_buf[8192] = "";
    char  net_ignore_buf[1024] = "PlayerMove, TileUpdate";

    bool  misc_unlock_recipes = false, misc_anti_collect = false, misc_auto_collect = false;
    bool  auto_gift_box = false;
    bool  fps_uncap = true; int target_fps = 240;

    bool  store_auto_buy        = false;
    int   store_target_count    = 50;
    int   store_bought_count    = 0;
    int   store_selected_pack   = 0;
    float store_buy_delay_ms    = 500.f;
    bool  store_stop_when_full  = true;
    int   store_inventory_used  = 0;
    int   store_inventory_max   = 0;
    char  store_custom_pack_id[64] = "";
    char  store_pack_filter[64]   = "";
    int   store_category_idx      = 0;

    char  dbg_tile_x_buf[16] = "0";
    char  dbg_tile_y_buf[16] = "0";
    int   dbg_ik_block       = 4154;
    int   dbg_ik_item        = 7;
    int   dbg_ik_qty         = 0;
    int   dbg_repair_block   = 4087;
    int   dbg_repair_item    = 5;
    int   dbg_repair_qty     = 0;

    char  dbg_ik_block_buf[16]     = "4154";
    char  dbg_ik_item_buf[8]       = "7";
    char  dbg_repair_block_buf[16] = "4087";
    char  dbg_repair_item_buf[8]   = "5";
    int   dbg_ai_id          = 0;
    int   dbg_collect_id     = 0;
    char  dbg_log[8192]      = "Debug log (last 16 actions):\n";

    static const int kDbgInvCap = 256;
    int   dbg_inv_block[256] = {};
    int   dbg_inv_item[256]  = {};
    int   dbg_inv_qty[256]   = {};
    int   dbg_inv_count      = 0;
    int   dbg_inv_picker_target = 0;

    int   theme_index = 0;

    struct ToastReq { char msg[128]; float duration_sec; bool pending; };
    ToastReq toast_request = {};

    void (*on_reload_game)()       = nullptr;
    void (*on_force_logout)()      = nullptr;
    void (*on_randomize_hwid)()    = nullptr;

    void (*on_export_config)()     = nullptr;
    void (*on_import_config)()     = nullptr;
    void (*on_clear_spot_cache)()  = nullptr;

    char config_last_save_text[64] = "Never saved";
    void (*on_apply_background)()  = nullptr;
    void (*on_send_packet)()       = nullptr;
    void (*on_apply_ignore_list)() = nullptr;
    void (*on_clear_log)()         = nullptr;
    void (*on_panic)()             = nullptr;

    void (*on_dbg_hit_block)()        = nullptr;
    void (*on_dbg_convert_items)()    = nullptr;
    void (*on_dbg_pickaxe_repair)()   = nullptr;
    void (*on_dbg_hit_ai)()           = nullptr;
    void (*on_dbg_collect_collectable)() = nullptr;
    void (*on_dbg_fill_player_pos)()  = nullptr;
    void (*on_dbg_inventory_refresh_legacy)() = nullptr;
    void (*on_dbg_agi_helper_dummy)() = nullptr;
    void (*on_dbg_agi_helper_real)()  = nullptr;
    void (*on_dbg_get_inventory_data)() = nullptr;
    void (*on_dbg_agi_raw_discovered)() = nullptr;
    void (*on_dbg_agi_raw_figh)()       = nullptr;
    void (*on_dbg_clear_log)()        = nullptr;
    void (*on_dbg_refresh_inventory)() = nullptr;
    void (*on_dbg_pick_inv)(int idx)   = nullptr;
    void (*on_dbg_recycle_all_gems)()  = nullptr;
    void (*on_dbg_equipped_info)()     = nullptr;
    void (*on_dump_world_blocks)()     = nullptr;
    void (*on_dump_block_predicates)() = nullptr;

    struct PacketEntry {
        long long timestamp_ns;
        bool      outgoing;
        char      id[32];
        int       size_bytes;
        const char* full_json;
    };
    const PacketEntry* packets = nullptr;
    int   packet_count = 0;
    int   selected_packet_index = -1;
};

void Init();
void Shutdown();
void Render(State& s);
void StartBoot();
void Toast(State& s, const char* msg, float duration_sec = 1.8f);

bool IsPaletteOpen();
void OpenPalette();
void ClosePalette();

void BeginGroup();
void EndGroup();
void GroupTitle(const char* text);
void SubLabel(const char* text);
void PageHead(const char* title, const char* subtitle = nullptr);
void Separator();
void Spacing(float px = 8.f);

bool ToggleRow(const char* icon, const char* label, const char* desc, bool* value);
bool ToggleRow(const char* label, bool* value);
bool SliderRow(const char* label, const char* units, float* v, float vmin, float vmax);
bool SliderRow(const char* label, const char* units, int*   v, int   vmin, int   vmax);
bool ComboRow(const char* label, const char* const* items, int item_count, int* current_idx);
bool ChipRow(const char* const* labels, bool* values, int count);
bool ChipLevel(const char* label, bool* value, bool gated);
bool InputRow(const char* label, char* buf, int buf_size, const char* placeholder = nullptr);
bool TextareaFullWidth(char* buf, int buf_size, const char* placeholder = nullptr, float height_px = -1.f);

bool Button(const char* label);
bool PrimaryButton(const char* label);
bool DestructiveButton(const char* label);
bool ButtonFullWidth(const char* label, float height_px = 30.f);
bool DrawerRow(const char* icon, const char* label, const char* desc, const char* drawer_key);

void Pill(const char* text, int tone);
void StatusPip(int tone);
void StatLabel(const char* k, const char* v);
void QuadCard(const char* eyebrow, const char* value, const char* trend, int trend_tone);
void HelpText(const char* text);

void EspMatrix(State& s);
void GemMatrix(bool grid[5][5]);
void ThemeGrid(int* current_index);
void ColorList();

void OpenDrawer(const char* key);
void CloseDrawer();
bool IsDrawerOpen(const char* key);

}

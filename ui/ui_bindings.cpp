

#include "ui_bindings.h"

#include "includes.h"

#include "core/Theme.h"
#include "core/functions.h"
#include "core/Config.h"
#include "core/ProxyConfig.h"
#include "core/ProxyPool.h"
#include "build_info.h"
#include "diagnostics/Diagnostics.h"
#include "features/mining/AutoMine.h"
#include "features/mining/PathFinding.h"
#include "features/bots/AutoNether.h"
#include "features/bots/BankBot.h"
#include "features/bots/GemSeller.h"
#include "features/visual/esp.h"
#include "multibox/MultiboxMode.h"
#include "multibox/MultiboxProfile.h"

extern bool show_imgui_menu;
extern std::string windows_username_str;
extern std::string GetHWID();

#include "imgui/imgui.h"
#include <chrono>
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <string>
#include <sstream>
#include <thread>
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

namespace ZmodBindings {

ZmodUI::State g_uiState;

static std::chrono::steady_clock::time_point g_session_start =
    std::chrono::steady_clock::now();

static struct CpuSampler {
    ULONGLONG lastSystem  = 0;
    ULONGLONG lastProcess = 0;
    int       cores       = 0;
    int       last_pct    = 0;
    std::chrono::steady_clock::time_point last_sample = {};
} g_cpu;

static int SampleCpuPercent() {
    auto now = std::chrono::steady_clock::now();
    if (g_cpu.last_sample.time_since_epoch().count() != 0) {
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_cpu.last_sample);
        if (dt.count() < 700) return g_cpu.last_pct;
    }
    if (g_cpu.cores == 0) {
        SYSTEM_INFO si; GetSystemInfo(&si);
        g_cpu.cores = si.dwNumberOfProcessors > 0 ? si.dwNumberOfProcessors : 1;
    }
    FILETIME ftSysIdle, ftSysKernel, ftSysUser;
    FILETIME ftProcCreate, ftProcExit, ftProcKernel, ftProcUser;
    if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser)) return g_cpu.last_pct;
    if (!GetProcessTimes(GetCurrentProcess(), &ftProcCreate, &ftProcExit, &ftProcKernel, &ftProcUser)) return g_cpu.last_pct;
    auto toU64 = [](const FILETIME& f){ return ((ULONGLONG)f.dwHighDateTime << 32) | f.dwLowDateTime; };
    ULONGLONG sys  = toU64(ftSysKernel) + toU64(ftSysUser);
    ULONGLONG proc = toU64(ftProcKernel) + toU64(ftProcUser);
    if (g_cpu.lastSystem && g_cpu.lastProcess) {
        ULONGLONG sysD  = sys  - g_cpu.lastSystem;
        ULONGLONG procD = proc - g_cpu.lastProcess;
        if (sysD > 0) {
            int pct = (int)((procD * 100ULL) / sysD);
            if (pct < 0) pct = 0; if (pct > 100) pct = 100;
            g_cpu.last_pct = pct;
        }
    }
    g_cpu.lastSystem  = sys;
    g_cpu.lastProcess = proc;
    g_cpu.last_sample = now;
    return g_cpu.last_pct;
}

static int SampleRamMb() {
    PROCESS_MEMORY_COUNTERS pmc{};
    pmc.cb = sizeof(pmc);
    if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) return 0;
    return (int)(pmc.WorkingSetSize / (1024ULL * 1024ULL));
}

static void SafeReadPlayerCurrency(void* pd, int& byteCoins, int& gems) {
    byteCoins = 0; gems = 0;
    if (!pd) return;
    if (oGetByteCoinAmount) {
        __try { byteCoins = oGetByteCoinAmount(pd); }
        __except (EXCEPTION_EXECUTE_HANDLER) { byteCoins = 0; }
    }
    if (oGetGemAmount) {
        __try { gems = oGetGemAmount(pd); }
        __except (EXCEPTION_EXECUTE_HANDLER) { gems = 0; }
    }
}

static int SafeReadPingMs() {
    return PingProbe::g_lastPingMs.load();
}

static constexpr float kSpeedSafeMax  = 4.0f;
static constexpr float kSpeedHardMax  = 30.0f;
static inline float ClampSpeed(float v) {
    if (v < 0.5f)         return 0.5f;
    if (v > kSpeedHardMax) return kSpeedHardMax;
    return v;
}

static void TruncateHwid(char out[16], const std::string& full) {
    if (full.size() <= 9) {

        std::strncpy(out, full.c_str(), 15);
        out[15] = 0;
        return;
    }

    std::snprintf(out, 16, "%c%c%c%c\xE2\x80\xA6%c%c%c%c",
                  full[0], full[1], full[2], full[3],
                  full[full.size() - 4], full[full.size() - 3],
                  full[full.size() - 2], full[full.size() - 1]);
}

static void ProjectCrossProduct(const bool types[5], const bool sizes[5],
                                bool out[5][5]) {
    for (int t = 0; t < 5; ++t)
        for (int s = 0; s < 5; ++s)
            out[t][s] = types[t] && sizes[s];
}

static void CollapseMatrix(const bool grid[5][5],
                           bool types[5], bool sizes[5]) {
    for (int t = 0; t < 5; ++t) {
        bool any = false;
        for (int s = 0; s < 5; ++s) if (grid[t][s]) { any = true; break; }
        types[t] = any;
    }
    for (int s = 0; s < 5; ++s) {
        bool any = false;
        for (int t = 0; t < 5; ++t) if (grid[t][s]) { any = true; break; }
        sizes[s] = any;
    }
}

static void CbReloadGame() {
    if (oReloadGame) {
        try { oReloadGame(); } catch (...) {}
    }
}

static void CbForceLogout() {
    try { MultiboxMode::LogoutCurrent(); } catch (...) {}
}

static void CbRandomizeHwid() {
    try { MultiboxProfile::RandomizeHwid(); } catch (...) {}
}

static void CbApplyBackground() {
    if (!g_WorldController || !oChangeBackground) return;
    int displayIdx = g_uiState.background_idx;
    int enumValue  = (displayIdx < 7) ? displayIdx : displayIdx + 1;
    try { oChangeBackground(g_WorldController, enumValue, nullptr); } catch (...) {}
}

static void CbSendPacket() {

    std::string content = g_uiState.net_repeater_buf;
    std::thread([content]() {
        Il2cppThreadScope _gc;
        try { RepeatPacket(content); } catch (...) {}
    }).detach();
}

static void CbApplyIgnoreList() {
    std::string buf = g_uiState.net_ignore_buf;
    g_packetIdIgnoreList.clear();
    std::stringstream ss(buf);
    std::string item;
    while (std::getline(ss, item, ',')) {

        while (!item.empty() && (item.front() == ' ' || item.front() == '\t' ||
                                  item.front() == '\n' || item.front() == '\r')) item.erase(item.begin());
        while (!item.empty() && (item.back()  == ' ' || item.back()  == '\t' ||
                                  item.back()  == '\n' || item.back()  == '\r')) item.pop_back();
        if (!item.empty()) g_packetIdIgnoreList.push_back(item);
    }
}

static void CbClearLog() {
    std::lock_guard<std::mutex> lock(g_PacketLogMutex);
    g_PacketLog.clear();
    g_selectedPackets.clear();
}

static void DbgLog(const char* fmt, ...) {
    char line[256];
    va_list ap;
    va_start(ap, fmt);
    int n = std::vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    if (n <= 0) return;

    std::cout << "[DbgLog] " << line << "\n";

    char ts[16];
    auto now  = std::chrono::system_clock::now();
    auto tt   = std::chrono::system_clock::to_time_t(now);
    std::tm lt{};
    localtime_s(&lt, &tt);
    std::snprintf(ts, sizeof(ts), "%02d:%02d:%02d ",
                  lt.tm_hour, lt.tm_min, lt.tm_sec);

    char& buf0 = g_uiState.dbg_log[0];
    size_t cap = sizeof(g_uiState.dbg_log);
    size_t cur = std::strlen(&buf0);
    size_t add = std::strlen(ts) + (size_t)n + 2;
    if (cur + add >= cap) {

        size_t drop = cap / 4;
        if (drop > cur) drop = cur;
        std::memmove(&buf0, &buf0 + drop, cur - drop + 1);
        cur -= drop;
    }
    std::snprintf(&buf0 + cur, cap - cur, "%s%s\n", ts, line);
}

static int DbgParseInt(const char* buf) {
    if (!buf || !*buf) return 0;
    return std::atoi(buf);
}

static void CbDbgFillPlayerPos() {
    Vector2i mp;
    if (!GetPlayerMapPoint(mp)) {
        DbgLog("[fill_pos] player not in a world (skipped)");
        return;
    }
    std::snprintf(g_uiState.dbg_tile_x_buf, sizeof(g_uiState.dbg_tile_x_buf), "%d", mp.x);
    std::snprintf(g_uiState.dbg_tile_y_buf, sizeof(g_uiState.dbg_tile_y_buf), "%d", mp.y);
    DbgLog("[fill_pos] -> tile (%d, %d)", mp.x, mp.y);
}

static void CbDbgHitBlock() {
    if (!oSendHitBlockMessage) {
        DbgLog("[HB] oSendHitBlockMessage not resolved (skipped)");
        return;
    }
    Vector2i mp{ DbgParseInt(g_uiState.dbg_tile_x_buf),
                 DbgParseInt(g_uiState.dbg_tile_y_buf) };
    __try {
        oSendHitBlockMessage(mp, GetDateTimeTicksGlobal(), false);
        DbgLog("[HB] fired tile (%d, %d)", mp.x, mp.y);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgLog("[HB] exception 0x%08x firing tile (%d, %d)",
               (unsigned)GetExceptionCode(), mp.x, mp.y);
    }
}

static void CbDbgConvertItems() {
    if (!oConvertItems) {
        DbgLog("[CI] oConvertItems not resolved (skipped)");
        return;
    }
    PlayerData_InventoryKey ik{ g_uiState.dbg_ik_block, g_uiState.dbg_ik_item };
    ReplyDispatch::NoteSendCI(ik.blockType, ik.itemType);
    bool queued = false;
    __try {
        oConvertItems(ik);
        queued = true;
        DbgLog("[CI] sent ik={bt=%d, it=%d} — packet queued",
               g_uiState.dbg_ik_block, g_uiState.dbg_ik_item);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgLog("[CI] exception 0x%08x firing ik={%d,%d}",
               (unsigned)GetExceptionCode(),
               g_uiState.dbg_ik_block, g_uiState.dbg_ik_item);
    }

    (void)queued;
}

static void CbDbgPickaxeRepair() {
    if (!oMiningPickaxeRepairing) {
        DbgLog("[MPR] oMiningPickaxeRepairing not resolved (skipped)");
        return;
    }
    PlayerData_InventoryKey ik{ g_uiState.dbg_repair_block, g_uiState.dbg_repair_item };
    ReplyDispatch::NoteSendMPR(ik.blockType);
    bool queued = false;
    __try {
        oMiningPickaxeRepairing(ik);
        queued = true;
        DbgLog("[MPR] sent ik={bt=%d, it=%d} — packet queued",
               g_uiState.dbg_repair_block, g_uiState.dbg_repair_item);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgLog("[MPR] exception 0x%08x firing ik={%d,%d}",
               (unsigned)GetExceptionCode(),
               g_uiState.dbg_repair_block, g_uiState.dbg_repair_item);
    }

    (void)queued;
}

static void CbDbgHitAI() {
    if (!oSendHitAIEnemyMessage) {
        DbgLog("[HAI] oSendHitAIEnemyMessage not resolved (skipped)");
        return;
    }
    Vector2i mp{ DbgParseInt(g_uiState.dbg_tile_x_buf),
                 DbgParseInt(g_uiState.dbg_tile_y_buf) };
    __try {
        oSendHitAIEnemyMessage(mp, g_uiState.dbg_ai_id, -1);
        DbgLog("[HAI] fired tile=(%d,%d) aiId=%d",
               mp.x, mp.y, g_uiState.dbg_ai_id);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgLog("[HAI] exception 0x%08x firing aiId=%d",
               (unsigned)GetExceptionCode(), g_uiState.dbg_ai_id);
    }
}

static void CbDbgCollect() {
    if (!oSendCollectCollectableMessage) {
        DbgLog("[CC] oSendCollectCollectableMessage not resolved (skipped)");
        return;
    }
    int id = g_uiState.dbg_collect_id;
    __try {
        oSendCollectCollectableMessage(id);
        DbgLog("[CC] fired id=%d", id);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgLog("[CC] exception 0x%08x firing id=%d",
               (unsigned)GetExceptionCode(), id);
    }
}

static void CbDbgInventoryRefreshLegacy() {
    if (!oSimpleBSON_Load || !oAddOneMessageToList || !il2cpp_array_new || !byteClass) {
        DbgLog("[AGI legacy] dependency missing (skipped)");
        return;
    }
    nlohmann::json j;
    j["ID"] = "AGI";
    j["PT"] = 0;
    SendAutoMineworldPacket(j);
    DbgLog("[AGI legacy] dispatched raw BSON via SendAutoMineworldPacket");
}

typedef void (__fastcall* tSendAGI_Helper)(Vector2i pos, void* wib, int tool);
static tSendAGI_Helper g_oSendAGI_Helper = nullptr;

static void _ResolveAGIHelper() {
    if (g_oSendAGI_Helper) return;

    g_oSendAGI_Helper = (tSendAGI_Helper)::Manifest::GetMethod(
        "OutgoingMessages", "SendAdjustGiftBoxAndInventoryMessage");
}

static unsigned long _SEH_FireAGI_Helper(Vector2i pos, void* wib, int tool) {
    __try { g_oSendAGI_Helper(pos, wib, tool); return 0; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return GetExceptionCode(); }
}

static void CbDbgAGIDummy() {
    _ResolveAGIHelper();
    if (!g_oSendAGI_Helper) {
        DbgLog("[AGI helper dummy] SendAdjust...Message unresolved");
        return;
    }
    Vector2i pos{0, 0};
    unsigned long ec = _SEH_FireAGI_Helper(pos, nullptr, 0);
    if (ec) {
        DbgLog("[AGI helper dummy] SEH 0x%08lX (likely null-wib deref in serializer)", ec);
        return;
    }
    DbgLog("[AGI helper dummy] sent (pos=(0,0) wib=null tool=NoTool)");
}

static void CbDbgAGIReal() {
    _ResolveAGIHelper();
    if (!g_oSendAGI_Helper) {
        DbgLog("[AGI helper real] SendAdjust...Message unresolved");
        return;
    }
    Vector2i pmp{0, 0};
    if (!::GetPlayerMapPoint(pmp)) {
        DbgLog("[AGI helper real] GetPlayerMapPoint failed");
        return;
    }
    unsigned long ec = _SEH_FireAGI_Helper(pmp, nullptr, 0);
    if (ec) {
        DbgLog("[AGI helper real] SEH 0x%08lX (likely null-wib deref in serializer)", ec);
        return;
    }
    DbgLog("[AGI helper real] sent (pos=(%d,%d) wib=null tool=NoTool)",
           pmp.x, pmp.y);
}

static void CbDbgAGIRawFigh() {
    nlohmann::json j;
    j["ID"] = "figh";
    j["PT"] = 0;
    SendAutoMineworldPacket(j);
    DbgLog("[AGI raw-figh] dispatched raw BSON {ID='figh', PT=0}");
}

static void CbDbgAGIRawDiscovered() {
    if (!g_haveAgiCipher.load(std::memory_order_acquire)) {
        DbgLog("[AGI raw-disc] no cipher captured yet — trigger natural AGI "
               "first (deposit to gift box / open safe box)");
        return;
    }
    std::string id;
    {
        std::lock_guard<std::mutex> lk(g_agiCipherMutex);
        id = g_agiCipherId;
    }
    if (id.empty()) {
        DbgLog("[AGI raw-disc] cipher cache empty (race?)");
        return;
    }
    nlohmann::json j;
    j["ID"] = id;
    j["PT"] = 0;
    SendAutoMineworldPacket(j);
    DbgLog("[AGI raw-disc] dispatched raw BSON {ID='%s', PT=0}", id.c_str());
}

typedef void (__fastcall* tSendGetInventoryDataMessage)(PlayerData_InventoryKey ik);
static tSendGetInventoryDataMessage g_oSendGetInventoryDataMessage = nullptr;

static void _ResolveGetInventoryData() {
    if (g_oSendGetInventoryDataMessage) return;
    g_oSendGetInventoryDataMessage = (tSendGetInventoryDataMessage)::Manifest::GetMethod(
        "OutgoingMessages", "SendGetInventoryDataMessage");
}

static unsigned long _SEH_FireGetInventoryData(PlayerData_InventoryKey ik) {
    __try { g_oSendGetInventoryDataMessage(ik); return 0; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return GetExceptionCode(); }
}

static void CbDbgGetInventoryData() {
    _ResolveGetInventoryData();
    if (!g_oSendGetInventoryDataMessage) {
        DbgLog("[GetInv] SendGetInventoryDataMessage unresolved");
        return;
    }
    PlayerData_InventoryKey ik{ g_uiState.dbg_ik_block, g_uiState.dbg_ik_item };
    unsigned long ec = _SEH_FireGetInventoryData(ik);
    if (ec) {
        DbgLog("[GetInv] SEH 0x%08lX (ik bt=%d it=%d)", ec,
               g_uiState.dbg_ik_block, g_uiState.dbg_ik_item);
        return;
    }
    DbgLog("[GetInv] sent (ik bt=%d it=%d)",
           g_uiState.dbg_ik_block, g_uiState.dbg_ik_item);
}

static int Seh_GetGemAmount(void* pd) {
    if (!oGetGemAmount || !pd) return 0;
    __try { return oGetGemAmount(pd); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

static void CbDbgRecycleAllGems() {
    if (!g_LocalPlayer) {
        DbgLog("[RA] g_LocalPlayer NULL (skipped)");
        return;
    }
    void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
    if (!pd) {
        DbgLog("[RA] PlayerData NULL (skipped)");
        return;
    }
    int preGems = Seh_GetGemAmount(pd);

    std::vector<GemSeller::GemEntry> gems;
    int totalStacks = 0;
    int totalQty    = 0;
    for (int t = 0; t < 5; ++t) {
        for (int s = 0; s < 5; ++s) {
            int bt  = GemSeller::kGems[t][s];
            int qty = GemSeller::InventoryCountForBlock(bt, GemSeller::CONSUMABLE_ITEM_TYPE);
            if (qty > 0) {
                gems.push_back({ bt, qty, t, s });
                ++totalStacks;
                totalQty += qty;
            }
        }
    }
    if (gems.empty()) {
        DbgLog("[RA] no gemstones to sell");
        return;
    }
    DbgLog("[RA] sending: %d stacks (%d total gems), pre-balance=%d",
           totalStacks, totalQty, preGems);
    GemSeller::SendRecycleAll(gems);
    DbgLog("[RA] queued — awaiting server reply for state mirror");
}

static void CbDbgClearLog() {
    std::snprintf(g_uiState.dbg_log, sizeof(g_uiState.dbg_log),
                  "Debug log (last 16 actions):\n");
}

static int DbgSnapshotInventory(void* pd,
                                int out_block[256],
                                int out_item[256],
                                int out_qty[256],
                                int cap)
{

    if (!pd || !oGetCountByKey) {
        DbgLog("[inv-snap] missing pd=%p oGetCountByKey=%p",
               pd, (void*)oGetCountByKey);
        return 0;
    }

    static const int kCommonItemTypes[] = { 7, 0, 5, 2, 9 };
    constexpr int MAX_BT = 6500;
    int n = 0;
    for (int bt = 1; bt <= MAX_BT && n < cap; ++bt) {
        for (int it : kCommonItemTypes) {
            PlayerData_InventoryKey k{ (World::BlockType)bt,
                                       (InventoryItemType)it };
            short qty = 0;
            __try { qty = oGetCountByKey(pd, k); }
            __except (EXCEPTION_EXECUTE_HANDLER) { qty = 0; }
            if (qty <= 0) continue;
            out_block[n] = bt;
            out_item[n]  = it;
            out_qty[n]   = (int)qty;
            ++n;

            break;
        }
    }
    DbgLog("[inv-snap] brute-force probe found %d items (scanned bt 1..%d)",
           n, MAX_BT);
    return n;
}

static void CbDbgRefreshInventory() {
    g_uiState.dbg_inv_count = 0;
    DbgLog("[inv-refresh] start: g_LocalPlayer=%p oGetInv=%p oGetCountByKey=%p "
           "Player_myPlayerData_off=0x%X",
           g_LocalPlayer,
           (void*)oGetInventoryAsOrderedByInventoryItemType,
           (void*)oGetCountByKey,
           (unsigned)Offsets::Player_myPlayerData);
    if (!g_LocalPlayer) {
        DbgLog("[inv-refresh] g_LocalPlayer NULL (skipped)");
        return;
    }
    if (!oGetInventoryAsOrderedByInventoryItemType) {
        DbgLog("[inv-refresh] oGetInventoryAsOrderedByInventoryItemType NULL — "
               "method missing in manifest");
        return;
    }
    if (!oGetCountByKey) {
        DbgLog("[inv-refresh] oGetCountByKey NULL — method missing in manifest");
        return;
    }
    void* pd = nullptr;
    __try {
        pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgLog("[inv-refresh] PlayerData read AV @ Player+0x%X",
               (unsigned)Offsets::Player_myPlayerData);
        return;
    }
    DbgLog("[inv-refresh] pd=%p", pd);
    if (!pd) {
        DbgLog("[inv-refresh] PlayerData NULL (skipped)");
        return;
    }
    int n = DbgSnapshotInventory(pd,
                                  g_uiState.dbg_inv_block,
                                  g_uiState.dbg_inv_item,
                                  g_uiState.dbg_inv_qty,
                                  ZmodUI::State::kDbgInvCap);
    g_uiState.dbg_inv_count = n;
    DbgLog("[inv-refresh] DbgSnapshotInventory -> %d items", n);
}

static void CbDbgPickInv(int idx) {
    if (idx < 0 || idx >= g_uiState.dbg_inv_count) {
        DbgLog("[inv-pick] out-of-range idx=%d count=%d",
               idx, g_uiState.dbg_inv_count);
        return;
    }
    int bt = g_uiState.dbg_inv_block[idx];
    int it = g_uiState.dbg_inv_item[idx];
    int qt = g_uiState.dbg_inv_qty[idx];
    if (g_uiState.dbg_inv_picker_target == 1) {
        g_uiState.dbg_repair_block = bt;
        g_uiState.dbg_repair_item  = it;
        g_uiState.dbg_repair_qty   = qt;
        DbgLog("[inv-pick] -> Repair IK {bt=%d, it=%d, qty=%d}", bt, it, qt);
    } else {
        g_uiState.dbg_ik_block = bt;
        g_uiState.dbg_ik_item  = it;
        g_uiState.dbg_ik_qty   = qt;
        DbgLog("[inv-pick] -> CI IK {bt=%d, it=%d, qty=%d}", bt, it, qt);
    }
}

}
extern bool g_should_exit_thread;
namespace ZmodBindings {
static void CbPanic() {
    ::g_should_exit_thread = true;
}

void Init() {
    g_session_start = std::chrono::steady_clock::now();

    AutoMine::AutoLoop::g_combineCadenceMs = 500;
    GemSeller::g_sellCadenceMs              = 200;

    std::strncpy(g_uiState.license, "Local Build",  sizeof(g_uiState.license) - 1);
    std::snprintf(g_uiState.version, sizeof(g_uiState.version), "%s", ZMOD_VERSION);

    ProxyCfg::LoadFromDisk();
    g_uiState.proxy_enabled  = ProxyCfg::g_enabled;
    std::strncpy(g_uiState.proxy_host, ProxyCfg::g_host.c_str(), sizeof(g_uiState.proxy_host) - 1);
    g_uiState.proxy_port     = ProxyCfg::g_port;
    std::snprintf(g_uiState.proxy_port_buf, sizeof(g_uiState.proxy_port_buf), "%d", ProxyCfg::g_port);
    g_uiState.proxy_use_auth = ProxyCfg::g_useAuth;
    std::strncpy(g_uiState.proxy_user, ProxyCfg::g_user.c_str(), sizeof(g_uiState.proxy_user) - 1);
    std::strncpy(g_uiState.proxy_pass, ProxyCfg::g_pass.c_str(), sizeof(g_uiState.proxy_pass) - 1);
    g_uiState.proxy_scheme      = ProxyCfg::g_scheme;
    g_uiState.proxy_tunnel_game = ProxyCfg::g_tunnelGame;
    ProxyPool::LoadFromDisk();

    g_uiState.on_reload_game       = &CbReloadGame;
    g_uiState.on_force_logout      = &CbForceLogout;
    g_uiState.on_randomize_hwid    = &CbRandomizeHwid;

    g_uiState.on_export_config     = +[]() {
        std::filesystem::path p = Config::PickSavePath(g_OverlayHwnd);
        if (p.empty()) return;
        if (Config::SaveAs(p)) {
            std::cout << "[Config] Exported to " << p.string() << "\n";
        }
    };
    g_uiState.on_import_config     = +[]() {
        std::filesystem::path p = Config::PickOpenPath(g_OverlayHwnd);
        if (p.empty()) return;
        if (Config::LoadFrom(p)) {
            std::cout << "[Config] Imported from " << p.string() << "\n";

        }
    };
    g_uiState.on_clear_spot_cache  = +[]() { FishBot::ClearSpotCache(); };
    g_uiState.on_pnb_save_anchor   = +[]() { Pnb::SavePosition(); };
    g_uiState.on_apply_background  = &CbApplyBackground;
    g_uiState.on_send_packet       = &CbSendPacket;
    g_uiState.on_apply_ignore_list = &CbApplyIgnoreList;
    g_uiState.on_clear_log         = &CbClearLog;
    g_uiState.on_panic             = &CbPanic;

    g_uiState.on_dbg_hit_block               = &CbDbgHitBlock;
    g_uiState.on_dbg_convert_items           = &CbDbgConvertItems;
    g_uiState.on_dbg_pickaxe_repair          = &CbDbgPickaxeRepair;
    g_uiState.on_dbg_hit_ai                  = &CbDbgHitAI;
    g_uiState.on_dbg_collect_collectable     = &CbDbgCollect;
    g_uiState.on_dbg_fill_player_pos         = &CbDbgFillPlayerPos;
    g_uiState.on_dbg_inventory_refresh_legacy = &CbDbgInventoryRefreshLegacy;
    g_uiState.on_dbg_agi_helper_dummy        = &CbDbgAGIDummy;
    g_uiState.on_dbg_agi_helper_real         = &CbDbgAGIReal;
    g_uiState.on_dbg_get_inventory_data      = &CbDbgGetInventoryData;
    g_uiState.on_dbg_agi_raw_discovered      = &CbDbgAGIRawDiscovered;
    g_uiState.on_dbg_agi_raw_figh            = &CbDbgAGIRawFigh;
    g_uiState.on_dbg_clear_log               = &CbDbgClearLog;
    g_uiState.on_dbg_refresh_inventory       = &CbDbgRefreshInventory;
    g_uiState.on_dbg_pick_inv                = &CbDbgPickInv;
    g_uiState.on_dbg_recycle_all_gems        = &CbDbgRecycleAllGems;
    g_uiState.on_dbg_equipped_info           = +[]() { AutoMine::DumpEquippedItemInfo(); };
    g_uiState.on_dump_world_blocks           = +[]() {

        std::string res = DumpWorldBlocksToJson();
        std::cout << "[BlockDump] " << res << "\n";
    };
    g_uiState.on_dump_block_predicates       = +[]() {

        std::string res = DumpBlockPredicatesToJson();
        std::cout << "[PredicateProbe] " << res << "\n";
    };
}

static void FillUsername(char dst[64]) {
    const std::string& src = windows_username_str;
    if (g_streamerMode) {
        std::strncpy(dst, "********", 63);
        dst[63] = 0;
        return;
    }
    std::strncpy(dst, src.c_str(), 63);
    dst[63] = 0;
}

namespace {
    struct PacketCache {
        std::vector<ZmodUI::State::PacketEntry> entries;
        std::vector<std::string>                ids;
        std::vector<std::string>                jsons;
    };
    PacketCache g_pktCache;
}
static void RebuildPacketCache(ZmodUI::State& s) {
    std::lock_guard<std::mutex> lock(g_PacketLogMutex);
    int n = (int)g_PacketLog.size();

    int start = (n > 500) ? (n - 500) : 0;
    int count = n - start;
    g_pktCache.entries.resize(count);
    g_pktCache.ids.resize(count);
    g_pktCache.jsons.resize(count);
    for (int i = 0; i < count; ++i) {
        const auto& src = g_PacketLog[start + i];
        ZmodUI::State::PacketEntry& dst = g_pktCache.entries[i];
        g_pktCache.ids[i]   = src.idString;
        g_pktCache.jsons[i] = src.fullPacketJson;
        dst.timestamp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                               src.timestamp.time_since_epoch()).count();
        dst.outgoing  = (src.direction == PacketDirection::Outgoing);
        std::strncpy(dst.id, g_pktCache.ids[i].c_str(), sizeof(dst.id) - 1);
        dst.id[sizeof(dst.id) - 1] = 0;
        dst.size_bytes = (int)g_pktCache.jsons[i].size();
        dst.full_json  = g_pktCache.jsons[i].c_str();
    }
    s.packets       = g_pktCache.entries.empty() ? nullptr : g_pktCache.entries.data();
    s.packet_count  = count;
}

void SyncFromBacking() {
    ZmodUI::State& s = g_uiState;

    s.show = show_imgui_menu;

    s.streamer_mode = g_streamerMode;
    FillUsername(s.username);
    TruncateHwid(s.hwid_short, GetHWID());
    s.level    = GetLocalPlayerLevel();
    s.fps      = (int)ImGui::GetIO().Framerate;
    s.cpu_pct  = SampleCpuPercent();
    s.ram_mb   = SampleRamMb();
    TryResolveNetworkClient();
    s.ping_ms  = SafeReadPingMs();

    if (g_LocalPlayer) {
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (pd) {
            int bc = 0, gm = 0;
            SafeReadPlayerCurrency(pd, bc, gm);
            s.bytecoins = (long)bc;
            s.gems      = (long)gm;
        }
    }
    s.uptime_sec = (long)std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::steady_clock::now() - g_session_start).count();

    {
        using namespace std::chrono;
        if (Config::g_lastSaveTime.time_since_epoch().count() == 0) {
            std::snprintf(s.config_last_save_text,
                          sizeof(s.config_last_save_text),
                          "Never saved");
        } else {
            long secs = (long)duration_cast<seconds>(
                steady_clock::now() - Config::g_lastSaveTime).count();
            const char* okTag = Config::g_lastSaveOk ? "" : " (FAILED)";
            if (secs < 5)        std::snprintf(s.config_last_save_text, sizeof(s.config_last_save_text), "Just now%s",        okTag);
            else if (secs < 60)  std::snprintf(s.config_last_save_text, sizeof(s.config_last_save_text), "%lds ago%s",        secs, okTag);
            else if (secs < 3600) std::snprintf(s.config_last_save_text, sizeof(s.config_last_save_text), "%ldm ago%s",       secs / 60, okTag);
            else                 std::snprintf(s.config_last_save_text, sizeof(s.config_last_save_text), "%ldh ago%s",        secs / 3600, okTag);
        }
    }

    s.god_mode         = GodModeEnabled;
    s.anti_trap        = g_antiTrap;
    s.door_walk_through= Door::g_walkThroughAll;
    s.gm_master_swap   = g_gmMasterSwap;
    s.anti_bounce      = AntiBounce;
    s.ignore_mushrooms = AntiBounceIgnoreMushrooms;
    s.anti_fan         = antifan;
    s.anti_deflector   = g_antiDeflector;
    s.anti_block_death = noBlockKill;
    s.unlimited_oxygen = unlimitedOxygenEnabled;
    s.anti_afk         = g_features.b_patchAfkTimers;
    s.ai_aimbot        = g_AIAimBot;
    s.block_on_player  = g_BlockOnPlayer;
    s.anti_poison      = g_antiPoison;
    s.damage_debug_log = g_debugDamageLog;
    s.actk_bypass            = g_actkBypass;
    s.player_runspeed_hack   = g_playerRunSpeedHack;
    s.player_runspeed_mul    = g_playerRunSpeedMul;

    s.player_speed       = runSpeedMultiplier;
    s.jump_override      = EnableJumpModeOverride;
    s.jump_mode          = (int)SelectedJumpMode;
    s.grav_override      = EnableGravityOverride;

    s.infinite_jetpack   = InfiniteJetpack;
    s.keys_to_fly        = g_features.b_keysToFlyEnabled;
    s.fly_speed          = g_features.f_flySpeed;
    s.auto_navigate      = PathRenderer::g_navEnabled;
    s.pf_show_path       = PathRenderer::g_showPath;
    s.pf_auto_move       = PathRenderer::g_autoMove;
    s.pf_move_speed      = ClampSpeed(PathRenderer::g_moveSpeed);
    s.pf_stop_range      = PathRenderer::g_stopRange;
    s.pf_airborne_check  = PathRenderer::g_airborneCheck;
    s.pf_max_airborne    = PathRenderer::g_maxAirborne;
    s.pf_body_offset     = PathRenderer::g_bodyOffsetFrac;
    s.pf_walk_instakill  = PathRenderer::g_walkInstakill;
    s.pf_strict_traps    = PathRenderer::g_pfStrictTraps;
    s.pf_hazard_buffer   = PathRenderer::g_pfHazardBufferRadius;
    s.pf_glow_halo       = PathRenderer::g_pathGlow;
    s.pf_flow_particles  = PathRenderer::g_pathFlow;
    s.pf_pulsing_target  = PathRenderer::g_pathPulse;
    s.pf_block_overlay   = PathRenderer::g_debugBlockOverlay;
    s.pf_dbg_stats_hud   = PathRenderer::g_dbgStatsHud;
    s.pf_dbg_inspect     = PathRenderer::g_dbgInspect;
    s.pf_dbg_verbose_log = PathRenderer::g_dbgVerboseLog;
    s.pf_move_method        = (PathRenderer::g_moveMethod == PathRenderer::MoveMethod::Teleport) ? 1 : 0;
    s.pf_portal_interval_ms = PathRenderer::g_portalIntervalMs;
    {
        int ms = (int)(PathRenderer::g_tp_tickIntervalMs + 0.5f);
        if (ms < 0)    ms = 0;
        if (ms > 1000) ms = 1000;
        s.pf_tp_interval_ms = ms;
    }
    s.pf_tp_throttle_packets = PathRenderer::g_pf_tp_throttleMovePackets;
    s.pf_tp_zero_velocity    = PathRenderer::g_pf_tp_zeroVelocity;
    s.pf_tp_force_grounded   = PathRenderer::g_pf_tp_forceGrounded;
    s.pf_tp_suppress_gravity = PathRenderer::g_pf_tp_suppressGravity;
    s.pf_tp_jitter_ms        = PathRenderer::g_tp_tickJitterMs;
    s.pf_grav_pulse          = g_pfGravPulse;
    s.pf_grav_pulse_on_ms    = g_pfGravPulseOnMs;
    s.pf_grav_pulse_off_ms   = g_pfGravPulseOffMs;

    s.show_minimap   = Minimap::showMinimap;
    s.free_cam       = FreecamEnabled;
    s.extra_zoom     = g_extraZoom;
    s.extra_zoom_mul = g_extraZoomMul;
    s.block_tooltip  = Minimap::g_blockTooltip;
    s.esp_master     = ESP::bMasterSwitch;
    s.esp_show_info  = ESP::bEnableDebugInfo;

    s.esp_boxes[0]  = ESP::bEnableLocalPlayerBoxes;
    s.esp_health[0] = ESP::bEnableLocalPlayerHealthESP;
    s.esp_color[0]  = ESP::localPlayerBoxFillColor;

    s.esp_lines[1]  = ESP::bEnableSnaplines;
    s.esp_boxes[1]  = ESP::bEnablePlayerBoxes;
    s.esp_health[1] = ESP::bEnablePlayerHealthESP;
    s.esp_color[1]  = ESP::boxColor;

    s.esp_lines[2]  = ESP::bEnableAILines;
    s.esp_boxes[2]  = ESP::bEnableAIBoxes;
    s.esp_color[2]  = ESP::AIBoxColor;

    s.esp_lines[3]  = ESP::bEnableGemstoneLines;
    s.esp_boxes[3]  = ESP::bEnableGemstoneBoxes;
    s.esp_color[3]  = ESP::gemBoxColor;

    s.esp_lines[4]  = ESP::bEnableCollectableLines;
    s.esp_boxes[4]  = ESP::bEnableCollectableBoxes;
    s.esp_color[4]  = ESP::collectableBoxColor;

    s.esp_lines[5]  = ESP::bEnableGiftBoxLines;
    s.esp_boxes[5]  = ESP::bEnableGiftBoxBoxes;
    s.esp_color[5]  = ESP::giftBoxBoxColor;

    s.esp_lines[6]  = ESP::bEnableExitPortalLines;
    s.esp_boxes[6]  = ESP::bEnableExitPortalBoxes;
    s.esp_color[6]  = ESP::exitPortalBoxColor;

    s.esp_lines[7]  = ESP::bEnableLightCrystalLines;
    s.esp_boxes[7]  = ESP::bEnableLightCrystalBoxes;
    s.esp_color[7]  = ESP::lightCrystalBoxColor;

    s.esp_lines[8]  = ESP::bEnablePotLines;
    s.esp_boxes[8]  = ESP::bEnablePotBoxes;
    s.esp_color[8]  = ESP::potBoxColor;

    s.esp_lines[9]  = ESP::bEnableTreasureLines;
    s.esp_boxes[9]  = ESP::bEnableTreasureBoxes;
    s.esp_color[9]  = ESP::treasureBoxColor;

    s.esp_lines[10] = ESP::bEnableNetherBossLines;
    s.esp_boxes[10] = ESP::bEnableNetherBossBoxes;
    s.esp_color[10] = ESP::NetherBossColor;

    s.gem_chip[0] = ESP::g_enabledGemstones[World::BlockType::MiningGemStoneDiamond];
    s.gem_chip[1] = ESP::g_enabledGemstones[World::BlockType::MiningGemStoneRuby];
    s.gem_chip[2] = ESP::g_enabledGemstones[World::BlockType::MiningGemStoneEmerald];
    s.gem_chip[3] = ESP::g_enabledGemstones[World::BlockType::MiningGemStoneSapphire];
    s.gem_chip[4] = ESP::g_enabledGemstones[World::BlockType::MiningGemStoneTopaz];

    s.crystal_chip[0] = ESP::g_enabledLightCrystals[World::BlockType::OrbLightingMining];
    s.crystal_chip[1] = ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalSmall];
    s.crystal_chip[2] = ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalMedium];
    s.crystal_chip[3] = ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalLarge];
    s.crystal_chip[4] = ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalSmall];
    s.crystal_chip[5] = ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalMedium];
    s.crystal_chip[6] = ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalLarge];

    s.anti_darkness  = AntiDarkness;
    s.force_weather  = g_forceWeather;
    s.weather_idx    = (int)g_forcedWeatherType;
    s.force_lighting = g_forceLighting;
    s.lighting_idx   = (int)g_forcedLightingType;

    s.am_enabled       = AutoMine::g_enabled || AutoMine::AutoLoop::g_enabled;
    s.am_auto_repair   = AutoMine::g_autoRepairPickaxe;
    s.am_repair_threshold = AutoMine::g_repairThreshold;
    s.am_premine_hb            = AutoMine::g_preMineHB;
    s.am_premine_radius        = AutoMine::g_preMineRadius;
    s.am_premine_include_solid = (AutoMine::g_preMineIncludeHardSolid != 0);
    s.am_speed         = AutoMine::g_flightScale;
    for (int i = 0; i < 5; ++i) s.am_levels[i] = AutoMine::AutoLoop::g_levelEnabled[i];
    s.am_anti_sleep    = AutoMine::g_antiSleep;
    s.am_anti_afk      = AutoMine::g_antiAfk;
    s.am_auto_combine  = AutoMine::AutoLoop::g_autoCombine;
    s.am_pf_auto_move  = AutoMine::g_autoMove;
    s.am_pf_draw_route = AutoMine::g_showPath;
    s.am_pf_move_speed = ClampSpeed(AutoMine::g_moveSpeed);
    s.am_pf_stop_range = AutoMine::g_pfStopRange;
    s.am_pf_body_offset = AutoMine::g_bodyOffsetFrac;
    s.am_pf_prefer_staircase = AutoMine::g_weights.preferStaircase;
    s.am_pf_up_penalty = AutoMine::g_weights.upPenaltySolid;
    s.am_pf_up_extra   = AutoMine::g_weights.unsupportedExtra;
    s.am_pf_airborne_check = AutoMine::g_weights.airborneCheck;
    s.am_pf_max_airborne   = AutoMine::g_weights.maxAirborne;
    s.am_pf_move_method        = (AutoMine::g_moveMethod == PathRenderer::MoveMethod::Teleport) ? 1 : 0;
    s.am_pf_portal_interval_ms = AutoMine::g_portalIntervalMs;
    s.am_pf_tp_throttle_packets = PathRenderer::g_am_tp_throttleMovePackets;
    s.am_pf_tp_zero_velocity    = PathRenderer::g_am_tp_zeroVelocity;
    s.am_pf_tp_force_grounded   = PathRenderer::g_am_tp_forceGrounded;
    s.am_pf_tp_suppress_gravity = PathRenderer::g_am_tp_suppressGravity;
    s.am_t_gemstones        = AutoMine::g_targetGemstones;
    s.am_t_nuggets          = AutoMine::g_targetNuggets;
    s.am_t_light_crystals   = AutoMine::g_targetCrystals;
    s.am_t_pots             = AutoMine::g_targetPots;
    s.am_t_floor_collectables = AutoMine::g_targetCollect;
    s.am_t_ai_enemies       = AutoMine::g_targetAI;
    s.am_t_exit_portal      = AutoMine::g_targetExit;
    s.am_c_hit_ai           = AutoMine::g_combatEnabled;
    s.am_c_range            = AutoMine::g_combatTileRange;
    s.am_c_cadence          = AutoMine::g_combatCadence;
    s.am_c_hit_cadence      = AutoMine::g_hitInterval;
    s.am_bb_auto_deposit    = BankBot::g_autoDeposit;
    s.am_bb_delay_ms        = BankBot::g_depositCadenceMs;
    for (int i = 0; i < 5; ++i) s.am_bb_nuggets[i]   = BankBot::g_selNuggets[i];
    for (int i = 0; i < 5; ++i) s.am_bb_keys[i]      = BankBot::g_selKeys[i];
    for (int i = 0; i < 9; ++i) s.am_bb_materials[i] = BankBot::g_selMaterials[i];
    for (int t = 0; t < 5; ++t) for (int sz = 0; sz < 5; ++sz)
        s.am_bb_gems[t][sz] = BankBot::g_depositMatrix[t][sz];
    s.am_as_enabled         = GemSeller::g_autoSell;
    for (int t = 0; t < 5; ++t) for (int sz = 0; sz < 5; ++sz)
        s.am_as_gems[t][sz] = GemSeller::g_sellMatrix[t][sz];
    s.am_rj_enabled         = AutoMine::AutoLoop::g_autoRejoin;
    s.am_rj_delay_ms        = AutoMine::AutoLoop::g_rejoinDelayMs;

    s.af_enabled      = FishBot::g_enabled;
    s.af_auto_land    = FishBot::g_autoLand;
    s.af_size_filter      = FishBot::g_sizeFilterEnabled;
    for (int i = 0; i < 5; ++i) s.af_keep_size[i] = FishBot::g_keepSize[i];
    s.af_last_fish_size   = FishBot::g_lastFishSize;
    s.af_last_fish_bucket = FishBot::g_lastFishBucket;
    s.af_auto_recast  = FishBot::g_autoRecast;
    s.af_anti_sleep   = FishBot::g_antiSleep;
    s.af_anti_afk     = FishBot::g_antiAfk;
    s.af_recast_min   = FishBot::g_recastDelayMinSec;
    s.af_recast_max   = FishBot::g_recastDelayMaxSec;
    s.af_hook_min     = FishBot::g_hookReactionMinMs;
    s.af_hook_max     = FishBot::g_hookReactionMaxMs;
    s.af_legit_mode   = FishBot::g_legitMode;
    s.af_pause_delay_min = FishBot::g_legitPauseDelayMin;
    s.af_pause_delay_max = FishBot::g_legitPauseDelayMax;
    s.af_pause_dur_min   = FishBot::g_legitPauseDurMin;
    s.af_pause_dur_max   = FishBot::g_legitPauseDurMax;
    s.af_verbose_minigame = FishBot::g_verboseMinigame;
    s.af_speed_mult       = FishBot::g_speedMult;
    s.af_return_to_spot   = FishBot::g_returnToSpot;
    s.af_max_drift_tiles  = FishBot::g_maxDriftTiles;

    s.ff_enabled   = AutoFossil::g_enabled;
    s.ff_repeat    = AutoFossil::g_repeat;
    s.ff_raw_moves = AutoFossil::g_useRawMoves;
    s.ff_move_min  = AutoFossil::g_moveDelayMinMs;
    s.ff_move_max  = AutoFossil::g_moveDelayMaxMs;
    s.ff_verbose   = AutoFossil::g_verbose;
    s.ff_solved    = AutoFossil::g_stats_solved;
    s.ff_failed    = AutoFossil::g_stats_failed;
    s.ff_moves     = AutoFossil::g_stats_moves;
    s.ff_status    = AutoFossil::PhaseLabel();
    s.ff_diag_id   = AutoFossil::g_diagId;
    s.ff_diag_raw  = AutoFossil::g_diagRaw;
    s.ff_diag_solve = AutoFossil::g_diagSolveLen;
    s.ff_diag_start = AutoFossil::g_diagStart;
    s.ff_diag_via   = AutoFossil::g_diagVia;
    s.ff_diag_inst  = AutoFossil::g_cachedArchUI ? "cached" : "null";
    s.ff_diag_run   = !AutoFossil::g_field_archGameIsRunning ? "no-field"
                      : (AutoFossil::ArchGameIsRunning() ? "yes" : "no");

    s.an_enabled         = AutoNether::g_enabled;
    s.an_auto_move       = AutoNether::g_autoMove;
    s.an_draw_route      = AutoNether::g_showPath;
    s.an_flight_speed    = AutoNether::g_flightScale;
    s.an_t_ai            = AutoNether::g_targetEnemies;
    s.an_t_treasure      = AutoNether::g_targetTreasure;
    s.an_t_gifts         = AutoNether::g_targetGifts;
    s.an_t_collectables  = AutoNether::g_targetCollectables;
    s.an_t_exit          = AutoNether::g_targetFloor;
    s.an_anti_sleep      = AutoNether::g_antiSleep;
    s.an_anti_afk        = AutoNether::g_antiAfk;
    s.an_prioritise_closest = AutoNether::g_priorityClosest;
    s.an_melee_cooldown  = AutoNether::g_hitInterval;
    s.an_max_melee_range = AutoNether::g_maxHitRange;
    s.an_stop_distance   = AutoNether::g_stopRange;
    s.an_min_enemy_hp    = AutoNether::g_minMonsterHp;
    s.an_pf_move_speed   = ClampSpeed(AutoNether::g_moveSpeed);
    s.an_pf_stop_range   = AutoNether::g_pfStopRange;
    s.an_pf_body_offset  = AutoNether::g_bodyOffsetFrac;
    s.an_pf_airborne_check = AutoNether::g_pathOpts.airborneCheck;
    s.an_pf_max_airborne = AutoNether::g_pathOpts.maxAirborne;
    s.an_pf_chamfer      = AutoNether::g_pathOpts.chamfer;
    s.an_pf_chamfer_amount = AutoNether::g_pathOpts.chamferAmount;
    s.an_pf_walk_instakill = AutoNether::g_pathOpts.walkInstakill;
    s.an_pf_glow_halo    = AutoNether::g_pathGlow;
    s.an_pf_flow_particles = AutoNether::g_pathFlow;
    s.an_pf_pulsing_target = AutoNether::g_pathPulse;
    s.an_pf_move_method        = (AutoNether::g_moveMethod == PathRenderer::MoveMethod::Teleport) ? 1 : 0;
    s.an_pf_portal_interval_ms = AutoNether::g_portalIntervalMs;
    s.an_pf_tp_throttle_packets = PathRenderer::g_an_tp_throttleMovePackets;
    s.an_pf_tp_zero_velocity    = PathRenderer::g_an_tp_zeroVelocity;
    s.an_pf_tp_force_grounded   = PathRenderer::g_an_tp_forceGrounded;
    s.an_pf_tp_suppress_gravity = PathRenderer::g_an_tp_suppressGravity;
    s.an_deep_loop     = AutoNether::g_deepLoop;
    s.an_deep_net_join = AutoNether::g_deepEntryNetwork;
    { int _bs = AutoNether::BloodScrollCount(); s.an_deep_scrolls = _bs < 0 ? 0 : _bs; }
    s.an_deep_runs     = AutoNether::g_deepRuns;
    s.an_deep_msg      = AutoNether::g_deepMsg[0] ? AutoNether::g_deepMsg : nullptr;
    s.an_fault_msg     = AutoNether::g_hadFault ? AutoNether::g_faultMsg : nullptr;
    s.an_reg_loop      = AutoNether::g_regLoop;
    for (int i = 0; i < 5; ++i) s.an_reg_levels[i] = AutoNether::g_regLevels[i];
    s.an_reg_level     = AutoNether::g_regCurLevel;
    s.an_reg_runs      = AutoNether::g_regRuns;
    s.an_reg_scrolls   = AutoNether::g_regScrolls;
    s.an_reg_msg       = AutoNether::g_regMsg[0] ? AutoNether::g_regMsg : nullptr;

    s.pnb_enabled    = Pnb::g_enabled;
    s.pnb_has_anchor = Pnb::g_hasAnchor;
    s.pnb_anchor_x   = Pnb::g_anchor.x;
    s.pnb_anchor_y   = Pnb::g_anchor.y;
    s.pnb_auto_collect = Pnb::g_autoCollect;
    for (int r = 0; r < 5; ++r)
        for (int col = 0; col < 5; ++col)
            s.pnb_offsets[r][col] = Pnb::g_offsets[r][col];

    s.pnb_inv_count = Pnb::g_invCount; if (s.pnb_inv_count < 0) s.pnb_inv_count = 0; if (s.pnb_inv_count > 24) s.pnb_inv_count = 24;
    for (int i = 0; i < s.pnb_inv_count; ++i) {
        s.pnb_inv_block[i] = Pnb::g_invBlock[i];
        s.pnb_inv_qty[i]   = Pnb::g_invQty[i];
        std::strncpy(s.pnb_inv_name[i], Pnb::g_invName[i], sizeof(s.pnb_inv_name[i]) - 1);
        s.pnb_inv_name[i][sizeof(s.pnb_inv_name[i]) - 1] = 0;
    }
    if (s.pnb_sel_idx < 0) s.pnb_sel_idx = 0;
    if (s.pnb_sel_idx >= s.pnb_inv_count) s.pnb_sel_idx = (s.pnb_inv_count > 0) ? s.pnb_inv_count - 1 : 0;

    s.pnb_sel_block_type = Pnb::g_heldBlock;
    s.pnb_sel_qty        = Pnb::g_heldQty;
    s.pnb_phase_label    = Pnb::g_enabled ? Pnb::PhaseLabel() : "stopped";

    s.net_capture_in   = g_CaptureIncoming;
    s.net_capture_out  = g_CaptureOutgoing;
    s.net_auto_scroll  = g_AutoScroll;
    s.net_direction_filter = g_directionFilter;
    std::strncpy(s.net_id_filter, g_packetFilterText, sizeof(s.net_id_filter) - 1);
    s.net_id_filter[sizeof(s.net_id_filter) - 1] = 0;

    s.misc_unlock_recipes = unlockAllRecipes;
    s.misc_anti_collect   = g_AntiCollect;
    s.misc_auto_collect   = g_autoCollect;
    s.auto_gift_box       = g_autoGiftBox;
    s.fps_uncap           = g_uncapFps;
    s.target_fps          = g_targetFps;

    s.theme_index = (int)currentStyle;

    RebuildPacketCache(s);
}

void ApplyToBacking() {

    ZmodUI::State& s = g_uiState;

    s.gm_master_swap              = false;
    s.door_walk_through           = false;
    s.damage_debug_log            = false;
    s.pf_strict_traps             = false;
    s.pf_hazard_buffer            = 0;
    s.pf_tp_force_grounded        = false;
    s.pf_tp_suppress_gravity      = false;
    s.am_pf_tp_force_grounded     = false;
    s.am_pf_tp_suppress_gravity   = false;
    s.an_pf_tp_force_grounded     = false;
    s.an_pf_tp_suppress_gravity   = false;
    s.an_pf_chamfer               = false;
    s.an_pf_chamfer_amount        = 0.f;

    s.am_premine_radius           = 2;

    show_imgui_menu = s.show;

    g_streamerMode = s.streamer_mode;

    GodModeEnabled              = s.god_mode;
    g_antiTrap                  = s.anti_trap;
    Door::g_walkThroughAll      = s.door_walk_through;
    g_gmMasterSwap              = s.gm_master_swap;
    AntiBounce                  = s.anti_bounce;
    AntiBounceIgnoreMushrooms   = s.ignore_mushrooms;
    antifan                     = s.anti_fan;
    g_antiDeflector             = s.anti_deflector;
    noBlockKill                 = s.anti_block_death;
    unlimitedOxygenEnabled      = s.unlimited_oxygen;
    g_features.b_patchAfkTimers = s.anti_afk;
    g_AIAimBot                  = s.ai_aimbot;
    g_BlockOnPlayer             = s.block_on_player;
    g_antiPoison                = s.anti_poison;
    g_debugDamageLog            = s.damage_debug_log;

    s.actk_bypass               = true;
    g_actkBypass                = true;
    g_playerRunSpeedHack        = s.player_runspeed_hack;
    g_playerRunSpeedMul         = s.player_runspeed_mul;

    runSpeedMultiplier          = s.player_speed;
    EnableJumpModeOverride      = s.jump_override;
    SelectedJumpMode            = (PlayerJumpMode)s.jump_mode;
    EnableGravityOverride       = s.grav_override;

    if (s.grav_override) {
        ChangeGravity(static_cast<::GravityMode>(s.grav_mode));
    }
    InfiniteJetpack             = s.infinite_jetpack;
    g_features.b_keysToFlyEnabled = s.keys_to_fly;
    g_features.f_flySpeed       = s.fly_speed;
    PathRenderer::g_navEnabled  = s.auto_navigate;
    PathRenderer::g_showPath    = s.pf_show_path;
    PathRenderer::g_autoMove    = s.pf_auto_move;
    PathRenderer::g_walkInstakill  = s.pf_walk_instakill;
    PathRenderer::g_pfStrictTraps  = s.pf_strict_traps;
    {
        int r = s.pf_hazard_buffer;
        if (r < 1) r = 1;
        if (r > 3) r = 3;
        PathRenderer::g_pfHazardBufferRadius = r;
    }
    PathRenderer::g_pathGlow    = s.pf_glow_halo;
    PathRenderer::g_pathFlow    = s.pf_flow_particles;
    PathRenderer::g_pathPulse   = s.pf_pulsing_target;
    PathRenderer::g_debugBlockOverlay = s.pf_block_overlay;
    PathRenderer::g_dbgStatsHud       = s.pf_dbg_stats_hud;
    PathRenderer::g_dbgInspect        = s.pf_dbg_inspect;
    PathRenderer::g_dbgVerboseLog     = s.pf_dbg_verbose_log;

    if (AutoMine::AutoLoop::g_walking) {
        PathRenderer::g_moveSpeed      = ClampSpeed(AutoMine::g_moveSpeed);
        PathRenderer::g_flightScale    = AutoMine::g_flightScale;
        PathRenderer::g_stopRange      = AutoMine::g_pfStopRange;
        PathRenderer::g_bodyOffsetFrac = AutoMine::g_bodyOffsetFrac;
        PathRenderer::g_airborneCheck  = AutoMine::g_weights.airborneCheck;
        PathRenderer::g_maxAirborne    = AutoMine::g_weights.maxAirborne;
    } else {
        PathRenderer::g_moveSpeed      = ClampSpeed(s.pf_move_speed);
        PathRenderer::g_flightScale    = 1.0f;
        PathRenderer::g_stopRange      = s.pf_stop_range;
        PathRenderer::g_bodyOffsetFrac = s.pf_body_offset;
        PathRenderer::g_airborneCheck  = s.pf_airborne_check;
        PathRenderer::g_maxAirborne    = s.pf_max_airborne;
    }

    PathRenderer::g_moveMethod  = (s.pf_move_method == 1)
                                  ? PathRenderer::MoveMethod::Teleport
                                  : PathRenderer::MoveMethod::InputDrive;

    if (PathRenderer::g_moveMethod == PathRenderer::MoveMethod::Teleport) {
        int ms = s.pf_tp_interval_ms;
        if (ms < 0)    ms = 0;
        if (ms > 1000) ms = 1000;
        PathRenderer::g_tp_tickIntervalMs = (float)ms;
    } else {
        PathRenderer::g_portalIntervalMs = s.pf_portal_interval_ms;
    }

    {
        bool fast = (PathRenderer::g_moveMethod == PathRenderer::MoveMethod::Teleport);
        if (fast) { s.pf_tp_throttle_packets = true; s.pf_tp_zero_velocity = true; }
        PathRenderer::g_pf_tp_throttleMovePackets = s.pf_tp_throttle_packets;
        PathRenderer::g_pf_tp_zeroVelocity        = s.pf_tp_zero_velocity;
    }
    PathRenderer::g_pf_tp_forceGrounded       = s.pf_tp_force_grounded;
    PathRenderer::g_pf_tp_suppressGravity     = s.pf_tp_suppress_gravity;
    {
        int j = s.pf_tp_jitter_ms;
        if (j < 0)   j = 0;
        if (j > 200) j = 200;
        PathRenderer::g_tp_tickJitterMs = j;
    }
    g_pfGravPulse      = s.pf_grav_pulse;
    g_pfGravPulseOnMs  = (s.pf_grav_pulse_on_ms  < 50)   ? 50   : s.pf_grav_pulse_on_ms;
    g_pfGravPulseOffMs = (s.pf_grav_pulse_off_ms < 0)    ? 0    : s.pf_grav_pulse_off_ms;

    Minimap::showMinimap   = s.show_minimap;
    FreecamEnabled         = s.free_cam;
    g_extraZoom            = s.extra_zoom;
    g_extraZoomMul         = s.extra_zoom_mul;
    Minimap::g_blockTooltip = s.block_tooltip;
    ESP::bMasterSwitch     = s.esp_master;
    ESP::bEnableDebugInfo  = s.esp_show_info;

    ESP::bEnableLocalPlayerBoxes       = s.esp_boxes[0];
    ESP::bEnableLocalPlayerHealthESP   = s.esp_health[0];
    ESP::localPlayerBoxFillColor       = s.esp_color[0];

    ESP::bEnableSnaplines              = s.esp_lines[1];
    ESP::bEnablePlayerBoxes            = s.esp_boxes[1];
    ESP::bEnablePlayerHealthESP        = s.esp_health[1];
    ESP::boxColor                      = s.esp_color[1];
    ESP::lineColor                     = s.esp_color[1];

    ESP::bEnableAILines                = s.esp_lines[2];
    ESP::bEnableAIBoxes                = s.esp_boxes[2];
    ESP::AIBoxColor                    = s.esp_color[2];
    ESP::AIlineColor                   = s.esp_color[2];

    ESP::bEnableGemstoneLines          = s.esp_lines[3];
    ESP::bEnableGemstoneBoxes          = s.esp_boxes[3];
    ESP::gemBoxColor                   = s.esp_color[3];
    ESP::gemSnapColor                  = s.esp_color[3];

    ESP::bEnableCollectableLines       = s.esp_lines[4];
    ESP::bEnableCollectableBoxes       = s.esp_boxes[4];
    ESP::collectableBoxColor           = s.esp_color[4];
    ESP::collectableLineColor          = s.esp_color[4];

    ESP::bEnableGiftBoxLines           = s.esp_lines[5];
    ESP::bEnableGiftBoxBoxes           = s.esp_boxes[5];
    ESP::giftBoxBoxColor               = s.esp_color[5];
    ESP::giftBoxLineColor              = s.esp_color[5];

    ESP::bEnableExitPortalLines        = s.esp_lines[6];
    ESP::bEnableExitPortalBoxes        = s.esp_boxes[6];
    ESP::exitPortalBoxColor            = s.esp_color[6];
    ESP::exitPortalLineColor           = s.esp_color[6];

    ESP::bEnableLightCrystalLines      = s.esp_lines[7];
    ESP::bEnableLightCrystalBoxes      = s.esp_boxes[7];
    ESP::lightCrystalBoxColor          = s.esp_color[7];
    ESP::lightCrystalLineColor         = s.esp_color[7];

    ESP::bEnablePotLines               = s.esp_lines[8];
    ESP::bEnablePotBoxes               = s.esp_boxes[8];
    ESP::potBoxColor                   = s.esp_color[8];
    ESP::potLineColor                  = s.esp_color[8];

    ESP::bEnableTreasureLines          = s.esp_lines[9];
    ESP::bEnableTreasureBoxes          = s.esp_boxes[9];
    ESP::treasureBoxColor              = s.esp_color[9];
    ESP::treasureLineColor             = s.esp_color[9];

    ESP::bEnableNetherBossLines        = s.esp_lines[10];
    ESP::bEnableNetherBossBoxes        = s.esp_boxes[10];
    ESP::NetherBossColor               = s.esp_color[10];

    ESP::g_enabledGemstones[World::BlockType::MiningGemStoneDiamond]  = s.gem_chip[0];
    ESP::g_enabledGemstones[World::BlockType::MiningGemStoneRuby]     = s.gem_chip[1];
    ESP::g_enabledGemstones[World::BlockType::MiningGemStoneEmerald]  = s.gem_chip[2];
    ESP::g_enabledGemstones[World::BlockType::MiningGemStoneSapphire] = s.gem_chip[3];
    ESP::g_enabledGemstones[World::BlockType::MiningGemStoneTopaz]    = s.gem_chip[4];

    ESP::g_enabledLightCrystals[World::BlockType::OrbLightingMining]         = s.crystal_chip[0];
    ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalSmall]   = s.crystal_chip[1];
    ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalMedium]  = s.crystal_chip[2];
    ESP::g_enabledLightCrystals[World::BlockType::MiningLightCrystalLarge]   = s.crystal_chip[3];
    ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalSmall]    = s.crystal_chip[4];
    ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalMedium]   = s.crystal_chip[5];
    ESP::g_enabledLightCrystals[World::BlockType::MiningTimeCrystalLarge]    = s.crystal_chip[6];

    AntiDarkness         = s.anti_darkness;
    g_forceWeather       = s.force_weather;
    g_forcedWeatherType  = (World::WeatherType)s.weather_idx;
    g_forceLighting      = s.force_lighting;
    g_forcedLightingType = (World::LightingType)s.lighting_idx;

    AutoMine::g_enabled              = s.am_enabled;
    AutoMine::AutoLoop::g_enabled    = s.am_enabled;

    AutoMine::AutoLoop::g_autoEnter  = s.am_enabled;
    AutoMine::g_pathGlow             = s.am_enabled;
    AutoMine::g_pathFlow             = s.am_enabled;

    AutoMine::g_autoRepairPickaxe = s.am_auto_repair;
    AutoMine::g_repairThreshold   = s.am_repair_threshold;
    AutoMine::g_preMineHB              = s.am_premine_hb;
    AutoMine::g_preMineRadius          = s.am_premine_radius;
    AutoMine::g_preMineIncludeHardSolid = s.am_premine_include_solid ? 1 : 0;

    AutoMine::g_flightScale       = 1.0f;
    for (int i = 0; i < 5; ++i) AutoMine::AutoLoop::g_levelEnabled[i] = s.am_levels[i];
    AutoMine::g_antiSleep         = s.am_anti_sleep;
    AutoMine::g_antiAfk           = s.am_anti_afk;
    AutoMine::AutoLoop::g_autoCombine = s.am_auto_combine;
    AutoMine::g_autoMove          = s.am_pf_auto_move;
    AutoMine::g_showPath          = s.am_pf_draw_route;
    AutoMine::g_moveSpeed         = ClampSpeed(s.am_pf_move_speed);
    AutoMine::g_pfStopRange       = s.am_pf_stop_range;
    AutoMine::g_bodyOffsetFrac    = s.am_pf_body_offset;
    AutoMine::g_weights.preferStaircase  = s.am_pf_prefer_staircase;
    AutoMine::g_weights.upPenaltySolid   = s.am_pf_up_penalty;
    AutoMine::g_weights.unsupportedExtra = s.am_pf_up_extra;
    AutoMine::g_weights.airborneCheck    = s.am_pf_airborne_check;
    AutoMine::g_weights.maxAirborne      = s.am_pf_max_airborne;
    AutoMine::g_moveMethod        = (s.am_pf_move_method == 1)
                                    ? PathRenderer::MoveMethod::Teleport
                                    : PathRenderer::MoveMethod::InputDrive;

    AutoMine::g_portalIntervalMs  = s.am_pf_portal_interval_ms;

    {
        bool fast = (s.am_pf_move_method == 1);
        if (fast) { s.am_pf_tp_throttle_packets = true; s.am_pf_tp_zero_velocity = true; }
        PathRenderer::g_am_tp_throttleMovePackets = s.am_pf_tp_throttle_packets;
        PathRenderer::g_am_tp_zeroVelocity        = s.am_pf_tp_zero_velocity;
    }
    PathRenderer::g_am_tp_forceGrounded       = s.am_pf_tp_force_grounded;
    PathRenderer::g_am_tp_suppressGravity     = s.am_pf_tp_suppress_gravity;
    AutoMine::g_targetGemstones   = s.am_t_gemstones;
    AutoMine::g_targetNuggets     = s.am_t_nuggets;
    AutoMine::g_targetCrystals    = s.am_t_light_crystals;
    AutoMine::g_targetPots        = s.am_t_pots;
    AutoMine::g_targetCollect     = s.am_t_floor_collectables;
    AutoMine::g_targetAI          = s.am_t_ai_enemies;
    AutoMine::g_targetExit        = s.am_t_exit_portal;
    AutoMine::g_combatEnabled     = s.am_c_hit_ai;
    AutoMine::g_combatTileRange   = s.am_c_range;
    AutoMine::g_combatCadence     = s.am_c_cadence;
    AutoMine::g_hitInterval       = s.am_c_hit_cadence;
    BankBot::g_autoDeposit        = s.am_bb_auto_deposit;
    BankBot::g_depositCadenceMs   = s.am_bb_delay_ms;
    for (int i = 0; i < 5; ++i) BankBot::g_selNuggets[i]   = s.am_bb_nuggets[i];
    for (int i = 0; i < 5; ++i) BankBot::g_selKeys[i]      = s.am_bb_keys[i];
    for (int i = 0; i < 9; ++i) BankBot::g_selMaterials[i] = s.am_bb_materials[i];

    for (int t = 0; t < 5; ++t) for (int sz = 0; sz < 5; ++sz)
        BankBot::g_depositMatrix[t][sz] = s.am_bb_gems[t][sz];
    GemSeller::g_autoSell         = s.am_as_enabled;
    for (int t = 0; t < 5; ++t) for (int sz = 0; sz < 5; ++sz)
        GemSeller::g_sellMatrix[t][sz] = s.am_as_gems[t][sz];
    AutoMine::AutoLoop::g_autoRejoin    = s.am_rj_enabled;
    AutoMine::AutoLoop::g_rejoinDelayMs = s.am_rj_delay_ms;

    FishBot::g_enabled              = s.af_enabled;
    FishBot::g_autoLand             = s.af_auto_land;
    FishBot::g_sizeFilterEnabled    = s.af_size_filter;
    for (int i = 0; i < 5; ++i) FishBot::g_keepSize[i] = s.af_keep_size[i];
    FishBot::g_autoRecast           = s.af_auto_recast;
    FishBot::g_antiSleep            = s.af_anti_sleep;
    FishBot::g_antiAfk              = s.af_anti_afk;
    FishBot::g_recastDelayMinSec    = s.af_recast_min;
    FishBot::g_recastDelayMaxSec    = s.af_recast_max;
    FishBot::g_hookReactionMinMs    = s.af_hook_min;
    FishBot::g_hookReactionMaxMs    = s.af_hook_max;
    FishBot::g_legitMode            = s.af_legit_mode;
    FishBot::g_legitPauseDelayMin   = s.af_pause_delay_min;
    FishBot::g_legitPauseDelayMax   = s.af_pause_delay_max;
    FishBot::g_legitPauseDurMin     = s.af_pause_dur_min;
    FishBot::g_legitPauseDurMax     = s.af_pause_dur_max;
    FishBot::g_verboseMinigame      = s.af_verbose_minigame;
    FishBot::g_speedMult            = s.af_speed_mult;
    FishBot::g_returnToSpot         = s.af_return_to_spot;
    FishBot::g_maxDriftTiles        = s.af_max_drift_tiles;

    AutoFossil::g_enabled        = s.ff_enabled;
    AutoFossil::g_repeat         = true;
    AutoFossil::g_useRawMoves    = true;
    AutoFossil::g_targetCount    = 0;
    AutoFossil::g_moveDelayMinMs = s.ff_move_min;
    AutoFossil::g_moveDelayMaxMs = s.ff_move_max;
    AutoFossil::g_verbose        = false;

    AutoNether::g_enabled            = s.an_enabled;
    AutoNether::g_autoMove           = s.an_auto_move;
    AutoNether::g_showPath           = s.an_draw_route;
    {
        float f = s.an_flight_speed;
        if (f < 0.5f) f = 0.5f;
        if (f > 2.5f) f = 2.5f;
        AutoNether::g_flightScale = f;
    }
    AutoNether::g_targetEnemies      = s.an_t_ai;
    AutoNether::g_targetTreasure     = s.an_t_treasure;
    AutoNether::g_targetGifts        = s.an_t_gifts;
    AutoNether::g_targetCollectables = s.an_t_collectables;
    AutoNether::g_targetFloor        = s.an_t_exit;
    AutoNether::g_antiSleep          = s.an_anti_sleep;
    AutoNether::g_antiAfk            = s.an_anti_afk;
    AutoNether::g_priorityClosest    = s.an_prioritise_closest;
    AutoNether::g_hitInterval        = s.an_melee_cooldown;
    AutoNether::g_maxHitRange        = s.an_max_melee_range;
    AutoNether::g_stopRange          = s.an_stop_distance;
    AutoNether::g_minMonsterHp       = s.an_min_enemy_hp;
    AutoNether::g_moveSpeed          = ClampSpeed(s.an_pf_move_speed);
    AutoNether::g_pfStopRange        = s.an_pf_stop_range;
    AutoNether::g_bodyOffsetFrac     = s.an_pf_body_offset;
    AutoNether::g_pathOpts.airborneCheck = s.an_pf_airborne_check;
    AutoNether::g_pathOpts.maxAirborne   = s.an_pf_max_airborne;
    AutoNether::g_pathOpts.chamfer       = s.an_pf_chamfer;
    AutoNether::g_pathOpts.chamferAmount = s.an_pf_chamfer_amount;
    AutoNether::g_pathOpts.walkInstakill = s.an_pf_walk_instakill;
    AutoNether::g_pathGlow           = s.an_pf_glow_halo;
    AutoNether::g_pathFlow           = s.an_pf_flow_particles;
    AutoNether::g_pathPulse          = s.an_pf_pulsing_target;
    AutoNether::g_moveMethod         = (s.an_pf_move_method == 1)
                                       ? PathRenderer::MoveMethod::Teleport
                                       : PathRenderer::MoveMethod::InputDrive;
    AutoNether::g_portalIntervalMs   = s.an_pf_portal_interval_ms;

    {
        bool fast = (s.an_pf_move_method == 1);
        if (fast) { s.an_pf_tp_throttle_packets = true; s.an_pf_tp_zero_velocity = true; }
        PathRenderer::g_an_tp_throttleMovePackets = s.an_pf_tp_throttle_packets;
        PathRenderer::g_an_tp_zeroVelocity        = s.an_pf_tp_zero_velocity;
    }
    PathRenderer::g_an_tp_forceGrounded       = s.an_pf_tp_force_grounded;
    PathRenderer::g_an_tp_suppressGravity     = s.an_pf_tp_suppress_gravity;
    AutoNether::g_deepLoop = s.an_deep_loop;
    AutoNether::g_deepEntryNetwork = s.an_deep_net_join;
    AutoNether::g_regLoop = s.an_reg_loop;
    for (int i = 0; i < 5; ++i) AutoNether::g_regLevels[i] = s.an_reg_levels[i];

    Pnb::g_enabled      = s.pnb_enabled;
    for (int r = 0; r < 5; ++r)
        for (int col = 0; col < 5; ++col)
            Pnb::g_offsets[r][col] = s.pnb_offsets[r][col];
    Pnb::g_autoCollect  = s.pnb_auto_collect;

    g_CaptureIncoming  = s.net_capture_in;
    g_CaptureOutgoing  = s.net_capture_out;
    g_AutoScroll       = s.net_auto_scroll;
    g_directionFilter  = s.net_direction_filter;
    std::strncpy(g_packetFilterText, s.net_id_filter, sizeof(g_packetFilterText) - 1);
    g_packetFilterText[sizeof(g_packetFilterText) - 1] = 0;

    std::strncpy(g_packetRepeaterContent, s.net_repeater_buf,
                 sizeof(g_packetRepeaterContent) - 1);
    g_packetRepeaterContent[sizeof(g_packetRepeaterContent) - 1] = 0;
    std::strncpy(g_packetIgnoreListBuffer, s.net_ignore_buf,
                 sizeof(g_packetIgnoreListBuffer) - 1);
    g_packetIgnoreListBuffer[sizeof(g_packetIgnoreListBuffer) - 1] = 0;

    {
        int port = std::atoi(s.proxy_port_buf);
        if (port < 0) port = 0; if (port > 65535) port = 65535;
        s.proxy_port = port;
        bool changed =
            ProxyCfg::g_enabled != s.proxy_enabled ||
            ProxyCfg::g_useAuth != s.proxy_use_auth ||
            ProxyCfg::g_port    != port ||
            ProxyCfg::g_host    != s.proxy_host ||
            ProxyCfg::g_user    != s.proxy_user ||
            ProxyCfg::g_pass    != s.proxy_pass ||
            ProxyCfg::g_scheme  != s.proxy_scheme ||
            ProxyCfg::g_tunnelGame != s.proxy_tunnel_game;
        if (changed) {
            ProxyCfg::g_enabled = s.proxy_enabled;
            ProxyCfg::g_useAuth = s.proxy_use_auth;
            ProxyCfg::g_port    = port;
            ProxyCfg::g_host    = s.proxy_host;
            ProxyCfg::g_user    = s.proxy_user;
            ProxyCfg::g_pass    = s.proxy_pass;
            ProxyCfg::g_scheme  = s.proxy_scheme;
            ProxyCfg::g_tunnelGame = s.proxy_tunnel_game;
            ProxyCfg::SaveToDisk();
            ProxyTunnel::g_verbose = true;
            ProxyTunnel::ApplyConfig();
        }
    }

    if (s.proxy_check_req) {
        if (s.proxy_check_req == 1) ProxyTunnel::CheckAllGame();
        else                        ProxyTunnel::CheckAllAuth();
        s.proxy_check_req = 0;
    }
    if (s.proxy_pool_dirty) {
        ProxyTunnel::ApplyConfig();
        s.proxy_pool_dirty = false;
    }

    unlockAllRecipes = s.misc_unlock_recipes;
    g_AntiCollect    = s.misc_anti_collect;
    g_autoCollect    = s.misc_auto_collect;
    g_autoGiftBox    = s.auto_gift_box;
    g_uncapFps       = s.fps_uncap;
    g_targetFps      = s.target_fps;

    currentStyle = (ThemeStyle)s.theme_index;
}

}

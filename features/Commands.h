

#pragma once

#include "main.h"
#include "functions.h"
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <deque>
#include <unordered_map>

#include "ui/Theme.h"

namespace ESP        { extern bool bMasterSwitch; }
namespace GemSeller  { extern bool g_autoSell; }
namespace MultiboxMode {
    typedef void (*t_UI_LogOut)();
    extern t_UI_LogOut g_UI_LogOut;
}

namespace Commands {

typedef void(__fastcall* tSubmitWorldChat)(Il2CppString* msg, void* methodInfo);
inline tSubmitWorldChat   oSubmitWorldChatMessage = nullptr;

typedef void(__fastcall* tSubmitClanChat)(Il2CppString* msg, void* methodInfo);
inline tSubmitClanChat    oSubmitClanChatMessage = nullptr;

typedef void(__fastcall* tSubmitGlobalChat)(void* chatMessage, void* methodInfo);
inline tSubmitGlobalChat  oSubmitGlobalChatMessage = nullptr;

typedef void(__fastcall* tSubmitPrivateChat)(Il2CppString* playerId,
                                             Il2CppString* msg,
                                             void* methodInfo);
inline tSubmitPrivateChat oSubmitPrivateChatMessage = nullptr;

typedef void(__fastcall* tSubmitPetChat)(int petId, Il2CppString* msg, void* methodInfo);
inline tSubmitPetChat     oSubmitPetCommandChatMessage = nullptr;

typedef bool(__fastcall* tChatUISubmit)(void* thisPtr, Il2CppString* text, void* methodInfo);
inline tChatUISubmit oChatUISubmit = nullptr;

typedef void(__fastcall* tSendRespawn)(int64_t clientTimestampTicks,
                                       bool toWorldEntrance,
                                       void* methodInfo);

typedef void(__fastcall* tWCSetBlock)(void* wc, int blockType, int x, int y, void* methodInfo);
inline tWCSetBlock oWCSetBlock = nullptr;
typedef bool(__fastcall* tWorldSetBlock)(void* world, int blockType,
                                         Vector2i pos,
                                         Il2CppString* ownerId,
                                         Il2CppString* ownerName,
                                         bool checkValidityInServer,
                                         void* methodInfo);
inline tWorldSetBlock oWorldSetBlock = nullptr;
inline tSendRespawn oSendRespawnCmd = nullptr;

inline std::mutex g_popupMutex;
inline bool        g_showGemsPopup  = false;
inline std::string g_gemsResultText;
inline bool        g_showInfoPopup  = false;
inline std::string g_infoTitle;
inline std::string g_infoBody;
inline bool        g_showMathPopup  = false;
inline std::string g_mathExpr;
inline std::string g_mathResult;
inline bool        g_showError      = false;
inline std::string g_errorText;

inline void SetError(const std::string& msg) {
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_errorText = msg;
    g_showError = true;
}

inline void Toast(const std::string& title, const std::string& msg,
                  NotificationType type = NotificationType::Info) {
    ShowNotification(title, msg, type, 4.0f);
}

inline std::string ReadIl2Str(const Il2CppString* s) {
    if (!s || IsBadReadPtr((void*)s, 0x18)) return "";
    int len = s->length;
    if (len <= 0 || len > 8192) return "";
    std::string out; out.reserve(len);
    for (int i = 0; i < len; ++i) {
        wchar_t c = s->chars[i];
        if (c == 0) break;

        if (c >= 32 && c < 127) out.push_back((char)c);
        else                    out.push_back('?');
    }
    return out;
}

inline std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return s;
}

inline std::string StripTags(const std::string& s) {
    std::string out; out.reserve(s.size());
    bool inTag = false;
    for (char c : s) {
        if (c == '<') { inTag = true;  continue; }
        if (c == '>') { inTag = false; continue; }
        if (!inTag) out.push_back(c);
    }
    return out;
}

static inline void* Seh_DerefPtr(void* base, uintptr_t off) {
    if (!base) return nullptr;
    __try { return *(void**)((uintptr_t)base + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}
static inline short Seh_DerefShort(void* base, uintptr_t off) {
    if (!base) return 0;
    __try { return *(short*)((uintptr_t)base + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

inline std::string ReadPlayerNickname(void* playerInst) {
    if (!playerInst || !g_off_PlayerNameTMP) return "";
    void* tmp = Seh_DerefPtr(playerInst, g_off_PlayerNameTMP);
    if (!tmp || IsBadReadPtr(tmp, 0xF0)) return "";
    Il2CppString* str = (Il2CppString*)Seh_DerefPtr(tmp, 0xE0);
    return ReadIl2Str(str);
}

inline std::string ReadPlayerId(void* playerInst) {
    if (!playerInst) return "";
    void* pd = Seh_DerefPtr(playerInst, Offsets::Player_myPlayerData);
    if (!pd) return "";
    Il2CppString* idStr = (Il2CppString*)Seh_DerefPtr(pd, Offsets::PlayerData_playerId);
    return ReadIl2Str(idStr);
}

inline int ReadPlayerCountryCode(void* playerInst) {
    if (!playerInst) return 0;
    void* pd = Seh_DerefPtr(playerInst, Offsets::Player_myPlayerData);
    if (!pd) return 0;
    return (int)Seh_DerefShort(pd, 0x32);
}

inline void* FindPlayerByName(const std::string& nameQuery) {
    std::string q = ToLower(nameQuery);
    std::vector<void*> snap;
    {
        std::lock_guard<std::mutex> lk(g_PlayerInstancesMutex);
        snap = g_PlayerInstances;
    }
    for (void* p : snap) {
        if (!p) continue;
        std::string raw = ReadPlayerNickname(p);
        if (raw.empty()) continue;
        std::string nick = ToLower(StripTags(raw));
        if (nick == q || nick.find(q) != std::string::npos) return p;
    }
    return nullptr;
}

inline Il2CppString* MakeIl2Str(const std::string& s) {
    if (!oIl2CppStringNew) return nullptr;
    return (Il2CppString*)oIl2CppStringNew(s.c_str());
}

static inline void Seh_JoinDynamicWorld(Il2CppString* world, Il2CppString* entry) {
    if (!oJoinDynamicWorld) return;
    __try { oJoinDynamicWorld(world, entry, false, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SendRespawnTicks(int64_t ticks) {
    if (!oSendRespawnCmd) return;
    __try { oSendRespawnCmd(ticks, false, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SubmitWorld(Il2CppString* msg) {
    if (!oSubmitWorldChatMessage) return;
    __try { oSubmitWorldChatMessage(msg, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SubmitPrivate(Il2CppString* id, Il2CppString* msg) {
    if (!oSubmitPrivateChatMessage) return;
    __try { oSubmitPrivateChatMessage(id, msg, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

inline void EchoLocal(const std::string& msg) {
    Toast("/command", msg, NotificationType::Info);
}

inline std::vector<std::string> Tokenize(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '"') { inQuotes = !inQuotes; continue; }
        if (!inQuotes && std::isspace((unsigned char)c)) {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
            continue;
        }
        cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

inline std::string RestAfter(const std::string& line, int skipTokens) {
    std::istringstream iss(line);
    std::string tok;
    int remaining = skipTokens;
    while (remaining-- > 0 && iss >> tok) {}
    std::string rest;
    std::getline(iss, rest);
    while (!rest.empty() && std::isspace((unsigned char)rest.front())) rest.erase(rest.begin());
    return rest;
}

struct MathParser {
    const std::string& s;
    size_t i = 0;
    bool   error = false;
    MathParser(const std::string& src) : s(src) {}
    void skip() { while (i < s.size() && std::isspace((unsigned char)s[i])) ++i; }
    double parse() { double v = parseAddSub(); skip(); if (i != s.size()) error = true; return v; }
    double parseAddSub() {
        double v = parseMulDiv();
        while (true) {
            skip();
            if (i >= s.size()) break;
            char c = s[i];
            if (c != '+' && c != '-') break;
            ++i;
            double rhs = parseMulDiv();
            v = (c == '+') ? v + rhs : v - rhs;
        }
        return v;
    }
    double parseMulDiv() {
        double v = parseUnary();
        while (true) {
            skip();
            if (i >= s.size()) break;
            char c = s[i];
            if (c != '*' && c != '/' && c != '%') break;
            ++i;
            double rhs = parseUnary();
            if (c == '*') v *= rhs;
            else if (c == '/') v = (rhs == 0.0) ? 0.0 : v / rhs;
            else               v = (rhs == 0.0) ? 0.0 : std::fmod(v, rhs);
        }
        return v;
    }
    double parseUnary() {
        skip();
        if (i < s.size() && s[i] == '-') { ++i; return -parseUnary(); }
        if (i < s.size() && s[i] == '+') { ++i; return  parseUnary(); }
        return parseAtom();
    }
    double parseAtom() {
        skip();
        if (i < s.size() && s[i] == '(') {
            ++i;
            double v = parseAddSub();
            skip();
            if (i < s.size() && s[i] == ')') ++i;
            else error = true;
            return v;
        }
        size_t start = i;
        while (i < s.size() &&
               (std::isdigit((unsigned char)s[i]) || s[i] == '.')) ++i;
        if (start == i) { error = true; return 0.0; }
        try { return std::stod(s.substr(start, i - start)); }
        catch (...) { error = true; return 0.0; }
    }
};

typedef int(__fastcall* tCfgGetGemstoneRecycleValue)(int blockType, void* methodInfo);
typedef int(__fastcall* tCfgGetFishRecycleValue)(int blockType, void* methodInfo);
typedef int(__fastcall* tCfgGetTreasurePouchReward)(int blockType, void* methodInfo);
typedef bool(__fastcall* tCfgIsConsumableTreasurePouch)(int blockType, void* methodInfo);
inline tCfgGetGemstoneRecycleValue    oCfgGetGemstoneRecycleValue    = nullptr;
inline tCfgGetFishRecycleValue        oCfgGetFishRecycleValue        = nullptr;
inline tCfgGetTreasurePouchReward     oCfgGetTreasurePouchReward     = nullptr;
inline tCfgIsConsumableTreasurePouch  oCfgIsConsumableTreasurePouch  = nullptr;

typedef Il2CppArray*(__fastcall* tPD_GetOrderedInventory)(void* pd);
inline tPD_GetOrderedInventory oPD_GetOrderedInventory = nullptr;

typedef short(__fastcall* tPD_GetCount)(void* pd, PlayerData_InventoryKey ik);
inline tPD_GetCount oPD_GetCount = nullptr;

static inline Il2CppArray* Seh_GetOrderedInventory(void* pd) {
    if (!oPD_GetOrderedInventory) return nullptr;
    __try { return oPD_GetOrderedInventory(pd); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}
static inline short Seh_PDGetCount(void* pd, PlayerData_InventoryKey ik) {
    if (!oPD_GetCount) return 0;
    __try { return oPD_GetCount(pd, ik); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}
static inline int Seh_GemstoneRecycle(int bt) {
    if (!oCfgGetGemstoneRecycleValue) return 0;
    __try { return oCfgGetGemstoneRecycleValue(bt, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}
static inline bool Seh_IsConsumablePouch(int bt) {
    if (!oCfgIsConsumableTreasurePouch) return false;
    __try { return oCfgIsConsumableTreasurePouch(bt, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}
static inline int Seh_PouchReward(int bt) {
    if (!oCfgGetTreasurePouchReward) return 0;
    __try { return oCfgGetTreasurePouchReward(bt, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}
static inline int Seh_FishRecycle(int bt) {
    if (!oCfgGetFishRecycleValue) return 0;
    __try { return oCfgGetFishRecycleValue(bt, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

inline void RunGems() {
    if (!g_LocalPlayer) { SetError("LocalPlayer not yet available."); return; }
    void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
    if (!pd)            { SetError("PlayerData not available.");      return; }
    if (!oPD_GetOrderedInventory) {
        SetError("PlayerData::GetInventoryAsOrderedByInventoryItemType missing.");
        return;
    }

    Il2CppArray* arr = Seh_GetOrderedInventory(pd);
    if (!arr) { SetError("Inventory walk failed."); return; }

    long long fishGems     = 0;
    long long gemstoneGems = 0;
    long long pouchGems    = 0;
    long long rawGems      = oGetGemAmount ? oGetGemAmount(pd) : 0;
    int       fishCount    = 0;
    int       stoneCount   = 0;
    int       pouchCount   = 0;

    size_t count = arr->max_length;
    PlayerData_InventoryKey* items = (PlayerData_InventoryKey*)arr->items;
    for (size_t i = 0; i < count; ++i) {
        PlayerData_InventoryKey ik = items[i];
        short amount = Seh_PDGetCount(pd, ik);
        if (amount <= 0) continue;

        int bt = ik.blockType;

        if (IsGemstone((World::BlockType)bt)) {
            int per = Seh_GemstoneRecycle(bt);
            gemstoneGems += (long long)per * amount;
            stoneCount   += amount;
            continue;
        }

        if (Seh_IsConsumablePouch(bt)) {
            int per = Seh_PouchReward(bt);
            pouchGems  += (long long)per * amount;
            pouchCount += amount;
            continue;
        }

        int fishPer = Seh_FishRecycle(bt);
        if (fishPer > 0) {
            fishGems  += (long long)fishPer * amount;
            fishCount += amount;
        }
    }

    long long total = fishGems + gemstoneGems + pouchGems + rawGems;
    char buf[1024];
    snprintf(buf, sizeof(buf),
        "Gem balance: %lld\n"
        "Fish (recycle):  %lld   (%d fish)\n"
        "Gemstones:       %lld   (%d stones)\n"
        "Gem pouches:     %lld   (%d pouches)\n"
        "---------------------------------\n"
        "Total estimated: %lld",
        rawGems, fishGems, fishCount, gemstoneGems, stoneCount,
        pouchGems, pouchCount, total);

    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_gemsResultText = buf;
    g_showGemsPopup  = true;
}

inline void CmdTp(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        EchoLocal("Usage: /tp <x> <y>");
        EchoLocal("  Warps the player to tile (x,y) in-world via "
                  "Player.WarpPlayer. Local-coord set + tp=true flag on "
                  "next GnkD packet. No cross-world support — use /warp.");
        return;
    }
    int x, y;
    try { x = std::stoi(args[1]); y = std::stoi(args[2]); }
    catch (...) { EchoLocal("/tp: bad number"); return; }
    if (!Player_WarpToTile(x, y)) {
        EchoLocal("/tp: WarpPlayer not resolved or call failed");
        return;
    }
    char buf[96];
    std::snprintf(buf, sizeof(buf), "Warped to (%d, %d)", x, y);
    Toast("/tp", buf, NotificationType::Success);
}

inline void CmdTele(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        EchoLocal("Usage: /tele <x> <y>   (also /silentwarp)");
        EchoLocal("  Silent teleport via WP-packet injection. Synthesizes "
                  "the inbound 'WP' BSON the server normally sends and "
                  "feeds it to HandleMessages in-process. Zero outbound "
                  "packets \xE2\x80\x94 server never knows. Unlimited range.");
        EchoLocal("  Risk: untested at scale. Test in a throwaway world.");
        return;
    }
    int x, y;
    try { x = std::stoi(args[1]); y = std::stoi(args[2]); }
    catch (...) { EchoLocal("/tele: bad number"); return; }

    DWORD now = GetTickCount();
    DWORD since = now - Teleport::g_lastWarpMs;
    if (Teleport::g_lastWarpMs != 0 && since < 500) {
        EchoLocal("/tele: rate-limited, last warp was " +
                  std::to_string(since) + "ms ago");
        return;
    }

    uint64_t hitsBefore = Teleport::g_warpHookHits.load();
    if (!Teleport::SilentWarp(x, y)) {
        EchoLocal("/tele: SilentWarp failed (see console for diag)");
        Toast("/tele", "Failed (check console)", NotificationType::Error);
        return;
    }

    uint64_t hitsAfter = Teleport::g_warpHookHits.load();
    bool reached = (hitsAfter > hitsBefore);
    char buf[160];
    std::snprintf(buf, sizeof(buf),
                  "Silent-warped to (%d, %d) [count=%llu, hook=%s]",
                  x, y,
                  (unsigned long long)Teleport::g_silentWarpCount.load(),
                  reached ? "FIRED" : "missed");
    Toast("/tele", buf,
          reached ? NotificationType::Success : NotificationType::Warning);
    if (!reached) {
        EchoLocal("/tele: dispatcher didn't route our packet to "
                  "HandleWarpPlayerMessage \xE2\x80\x94 'WP' ID may be wrong.");
        EchoLocal("  Walk through a real portal next; the [WarpHook] line "
                  "in console will show the correct field shape.");
    }
}

inline void CmdTeleProbe(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        EchoLocal("Usage: /teleprobe <x> <y>");
        EchoLocal("  Dumps the JSON+BSON shape SilentWarp would send for "
                  "(x,y), without actually sending. Compare against "
                  "[WarpHook] payload= lines (visible when you walk "
                  "through a real portal) to verify the field shape.");
        return;
    }
    int x, y;
    try { x = std::stoi(args[1]); y = std::stoi(args[2]); }
    catch (...) { EchoLocal("/teleprobe: bad number"); return; }
    std::string p = Teleport::ProbePayload(x, y);
    EchoLocal("/teleprobe payload: " + p);
    std::cout << "[Teleport] /teleprobe (" << x << "," << y << ") "
              << "would send: " << p << "\n";
    Toast("/teleprobe",
          "Payload logged to console (count=" +
              std::to_string(Teleport::g_warpHookHits.load()) +
              " WP hooks fired so far)",
          NotificationType::Info);
}

inline void CmdCheckpoint(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        EchoLocal("Usage: /cp <x> <y>   (also /checkpoint)");
        EchoLocal("  Claims tile (x,y) as your respawn checkpoint via "
                  "OutgoingMessages.SendCheckpoint. Future deaths respawn "
                  "there. Client-authoritative coord — server doesn't "
                  "validate the tile is in a valid block.");
        return;
    }
    int x, y;
    try { x = std::stoi(args[1]); y = std::stoi(args[2]); }
    catch (...) { EchoLocal("/cp: bad number"); return; }
    if (!OutMsg_SendCheckpoint(x, y)) {
        EchoLocal("/cp: SendCheckpoint not resolved or call failed");
        return;
    }
    char buf[96];
    std::snprintf(buf, sizeof(buf), "Checkpoint set @ (%d, %d)", x, y);
    Toast("/cp", buf, NotificationType::Success);
}

inline void CmdResurrect(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        EchoLocal("Usage: /rez <x> <y>   (also /resurrect)");
        EchoLocal("  Sends a SendResurrect packet with arbitrary coords. "
                  "Only valid AFTER a death — pair with a death trigger "
                  "(e.g. walk on instakill with godmode off). Server "
                  "accepts the tile as-is.");
        return;
    }
    int x, y;
    try { x = std::stoi(args[1]); y = std::stoi(args[2]); }
    catch (...) { EchoLocal("/rez: bad number"); return; }
    if (!OutMsg_SendResurrect(x, y)) {
        EchoLocal("/rez: SendResurrect not resolved or call failed");
        return;
    }
    char buf[96];
    std::snprintf(buf, sizeof(buf), "Resurrect @ (%d, %d)", x, y);
    Toast("/rez", buf, NotificationType::Success);
}

inline void CmdWarp(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        EchoLocal("Usage: /warp <world> [portal_id]");
        EchoLocal("  portal_id is the destination spawn point — e.g. "
                  "\"default\", \"exit\", \"from_mine\", \"1\", or any "
                  "named entry the target world exposes.");
        return;
    }
    std::string world = args[1];
    std::transform(world.begin(), world.end(), world.begin(), ::toupper);

    std::string entry = args.size() >= 3 ? args[2] : "default";
    if (!oJoinDynamicWorld || !oIl2CppStringNew) {
        EchoLocal("Warp helper not wired."); return;
    }
    Seh_JoinDynamicWorld(MakeIl2Str(world), MakeIl2Str(entry));
    std::string toastBody = "Warping to " + world;
    if (entry != "default") toastBody += " @ " + entry;
    Toast("/warp", toastBody, NotificationType::Success);
}

inline void CmdRespawn(const std::vector<std::string>&) {
    if (!oSendRespawnCmd) { EchoLocal("Respawn helper not wired."); return; }
    Seh_SendRespawnTicks(GetDateTimeTicksGlobal());
    Toast("/respawn", "Respawn sent.", NotificationType::Success);
}

inline void CmdMath(const std::string& fullLine) {
    std::string expr = RestAfter(fullLine, 1);
    if (expr.empty()) { EchoLocal("Usage: /math <expression>"); return; }
    MathParser p(expr);
    double v = p.parse();
    char buf[64];
    if (p.error) snprintf(buf, sizeof(buf), "parse error");
    else         snprintf(buf, sizeof(buf), "%.10g", v);

    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_mathExpr     = expr;
    g_mathResult   = buf;
    g_showMathPopup = true;
}

inline void CmdFlag(const std::vector<std::string>& args) {
    if (args.size() < 2) { EchoLocal("Usage: /flag <username>"); return; }
    void* target = FindPlayerByName(args[1]);
    if (!target) { EchoLocal("Player not found: " + args[1]); return; }
    std::string disp = ReadPlayerNickname(target);
    if (disp.empty()) { EchoLocal("Couldn't read target's nametag."); return; }
    if (!oSubmitWorldChatMessage || !oIl2CppStringNew) {
        EchoLocal("Chat sender not wired."); return;
    }

    std::string cleaned = StripTags(disp);
    Il2CppString* s = MakeIl2Str(cleaned);
    if (s) Seh_SubmitWorld(s);
}

inline void CmdMsg(const std::string& fullLine, const std::vector<std::string>& args) {
    if (args.size() < 3) { EchoLocal("Usage: /msg <username> <message>"); return; }
    void* target = FindPlayerByName(args[1]);
    if (!target) { EchoLocal("Player not found: " + args[1]); return; }
    std::string id = ReadPlayerId(target);
    if (id.empty()) { EchoLocal("Couldn't read target's player ID."); return; }
    std::string body = RestAfter(fullLine, 2);
    if (body.empty()) { EchoLocal("Empty message."); return; }
    if (!oSubmitPrivateChatMessage || !oIl2CppStringNew) {
        EchoLocal("Private chat helper not wired."); return;
    }
    Seh_SubmitPrivate(MakeIl2Str(id), MakeIl2Str(body));
    Toast("/msg", "Sent to " + args[1], NotificationType::Success);
}

inline void CmdGlobal(const std::string& fullLine) {
    std::string body = RestAfter(fullLine, 1);
    if (body.empty()) { EchoLocal("Usage: /gm <message>"); return; }

    if (!oSubmitWorldChatMessage || !oIl2CppStringNew) {
        EchoLocal("Chat sender not wired."); return;
    }
    Il2CppString* s = MakeIl2Str(body);
    if (s) Seh_SubmitWorld(s);
    Toast("/gm", "Sent (as world chat — global channel needs the GameplayUI helper).",
          NotificationType::Info);
}

inline void CmdInfo(const std::vector<std::string>& args) {
    if (args.size() < 2) { EchoLocal("Usage: /info <username>"); return; }
    void* target = FindPlayerByName(args[1]);
    if (!target) { EchoLocal("Player not found: " + args[1]); return; }
    std::string disp = ReadPlayerNickname(target);
    std::string id   = ReadPlayerId(target);
    int country      = ReadPlayerCountryCode(target);
    void* pos        = (target && oGetTransform) ? oGetTransform(target) : nullptr;
    Vector3 wp = pos ? oGetPosition(pos) : Vector3{0,0,0};

    char buf[1024];
    snprintf(buf, sizeof(buf),
        "Display:      %s\n"
        "Stripped:     %s\n"
        "Player ID:    %s\n"
        "Country code: %d\n"
        "World pos:    (%.2f, %.2f)\n"
        "Local? :       %s",
        disp.c_str(), StripTags(disp).c_str(), id.c_str(),
        country, wp.x, wp.y,
        (target == g_LocalPlayer) ? "yes" : "no");

    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "/info " + args[1];
    g_infoBody  = buf;
    g_showInfoPopup = true;
}

static inline Vector2i Seh_W2M(void* worldController, Vector3 v) {
    Vector2i r{0,0};
    if (!worldController || !oConvertWorldPointToMapPoint) return r;
    __try { r = oConvertWorldPointToMapPoint(worldController, v); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
    return r;
}
static inline Vector2i Seh_ReadV2i(void* base, uintptr_t off) {
    Vector2i r{0,0};
    if (!base) return r;
    __try { r = *(Vector2i*)((uintptr_t)base + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
    return r;
}
static inline int Seh_ReadInt(void* base, uintptr_t off) {
    if (!base) return 0;
    __try { return *(int*)((uintptr_t)base + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}
static inline int Seh_XPLevel(int xp) {
    if (!oGetXPLevel) return 0;
    __try { return oGetXPLevel(xp); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

struct PosResult { bool ok; Vector3 v; };
static inline PosResult Seh_GetPlayerPos(void* p) {
    PosResult r{false, {0,0,0}};
    if (!p || !oGetTransform || !oGetPosition) return r;
    __try {
        void* t = oGetTransform(p);
        if (t) { r.v = oGetPosition(t); r.ok = true; }
    } __except (EXCEPTION_EXECUTE_HANDLER) { r.ok = false; }
    return r;
}
static inline void Seh_SetPlayerPos(void* p, Vector3 pos) {
    if (!p || !oGetTransform || !oSetPosition) return;
    __try {
        void* t = oGetTransform(p);
        if (t) oSetPosition(t, pos);
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
}

static inline Vector3 TileCenter(Vector2i tile) {
    Vector3 r{0,0,0};
    if (!g_WorldController || !oConvertMapPointToWorldPoint_V3) return r;
    __try { r = oConvertMapPointToWorldPoint_V3(g_WorldController, tile); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
    return r;
}

inline std::mutex g_stateMutex;
inline std::unordered_map<std::string, Vector2i> g_homes;
inline std::deque<Vector3>                       g_backStack;
inline std::unordered_map<std::string, std::vector<std::string>> g_macros;
inline std::string                               g_lastPmFromId;
inline std::string                               g_lastPmFromName;
inline int                                       g_scanBlockId = -1;
inline std::chrono::steady_clock::time_point     g_scanUntil{};

inline std::string                               g_announceText;
inline std::chrono::steady_clock::time_point     g_announceUntil{};

struct DelayedCmd { std::chrono::steady_clock::time_point fireAt; std::string line; };
inline std::deque<DelayedCmd>                    g_delayedCmds;

inline bool                                      g_showHelp = false;

inline bool TryDispatch(const std::string& line);

inline void CmdToggleGod(const std::vector<std::string>&) {
    GodModeEnabled = !GodModeEnabled;
    Toast("/god", GodModeEnabled ? "ON" : "OFF",
          GodModeEnabled ? NotificationType::Success : NotificationType::Info);
}
inline void CmdToggleFly(const std::vector<std::string>&) {
    g_features.b_keysToFlyEnabled = !g_features.b_keysToFlyEnabled;
    Toast("/fly", g_features.b_keysToFlyEnabled ? "ON" : "OFF",
          g_features.b_keysToFlyEnabled ? NotificationType::Success : NotificationType::Info);
}
inline void CmdToggleFreecam(const std::vector<std::string>&) {
    FreecamEnabled = !FreecamEnabled;
    Toast("/freecam", FreecamEnabled ? "ON" : "OFF",
          FreecamEnabled ? NotificationType::Success : NotificationType::Info);
}
inline void CmdToggleAimbot(const std::vector<std::string>&) {
    g_AIAimBot = !g_AIAimBot;
    Toast("/aimbot", g_AIAimBot ? "ON" : "OFF",
          g_AIAimBot ? NotificationType::Success : NotificationType::Info);
}
inline void CmdSpeed(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        char b[64]; snprintf(b, sizeof(b), "current %.2fx", runSpeedMultiplier);
        EchoLocal(std::string("Usage: /speed <0.5-9>  — ") + b);
        return;
    }
    try {
        float v = std::stof(args[1]);
        if (v < 0.5f) v = 0.5f; if (v > 9.0f) v = 9.0f;
        runSpeedMultiplier = v;
        char b[64]; snprintf(b, sizeof(b), "%.2fx", v);
        Toast("/speed", b, NotificationType::Success);
    } catch (...) { EchoLocal("Bad number for /speed."); }
}
inline void CmdZoom(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        char b[80];
        snprintf(b, sizeof(b), "ExtraZoom=%s  mul=%.2f",
                 g_extraZoom ? "ON" : "OFF", g_extraZoomMul);
        EchoLocal(b);
        return;
    }
    try {
        float v = std::stof(args[1]);
        if (v <= 1.01f) { g_extraZoom = false; Toast("/zoom", "OFF"); return; }
        if (v > 20.f) v = 20.f;
        g_extraZoom    = true;
        g_extraZoomMul = v;
        char b[40]; snprintf(b, sizeof(b), "%.1fx", v);
        Toast("/zoom", b, NotificationType::Success);
    } catch (...) { EchoLocal("Bad number for /zoom."); }
}
inline void CmdToggleESP(const std::vector<std::string>&) {
    ESP::bMasterSwitch = !ESP::bMasterSwitch;
    Toast("/esp", ESP::bMasterSwitch ? "ON" : "OFF",
          ESP::bMasterSwitch ? NotificationType::Success : NotificationType::Info);
}
inline void CmdRainbow(const std::vector<std::string>& args) {
    bool on = !g_rainbowNameEnabled;
    if (args.size() >= 2) {
        std::string a = ToLower(args[1]);
        on = (a == "on" || a == "1" || a == "true");
    }
    g_rainbowNameEnabled = on;
    Toast("/rainbow", on ? "ON" : "OFF",
          on ? NotificationType::Success : NotificationType::Info);
}

inline void CmdPos(const std::vector<std::string>&) {
    if (!g_LocalPlayer) { EchoLocal("LocalPlayer not yet available."); return; }
    auto r = Seh_GetPlayerPos(g_LocalPlayer);
    if (!r.ok) { EchoLocal("Couldn't read player position."); return; }
    Vector2i tile = Seh_W2M(g_WorldController, r.v);
    char b[160];
    snprintf(b, sizeof(b), "world (%.2f, %.2f)  tile (%d, %d)",
             r.v.x, r.v.y, tile.x, tile.y);
    Toast("/pos", b, NotificationType::Info);
}

inline void CmdWorldInfo(const std::vector<std::string>&) {
    std::string name = GetCachedWorldName();
    char b[256];
    if (g_WorldController) {
        void* world = Seh_DerefPtr(g_WorldController, g_off_WCWorld);
        if (world) {
            Vector2i sz = Seh_ReadV2i(world, g_off_WorldSize);
            snprintf(b, sizeof(b), "World: %s\nSize: %dx%d",
                     name.empty() ? "?" : name.c_str(), sz.x, sz.y);
        } else { snprintf(b, sizeof(b), "World: %s (no world ptr)", name.c_str()); }
    } else { snprintf(b, sizeof(b), "Not in a world."); }
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "/world";
    g_infoBody  = b;
    g_showInfoPopup = true;
}

inline void CmdPlayers(const std::vector<std::string>&) {
    std::vector<void*> snap;
    {
        std::lock_guard<std::mutex> lk(g_PlayerInstancesMutex);
        snap = g_PlayerInstances;
    }
    std::string out;
    int idx = 0;
    for (void* p : snap) {
        if (!p) continue;
        std::string n  = StripTags(ReadPlayerNickname(p));
        int         cc = ReadPlayerCountryCode(p);
        auto pos = Seh_GetPlayerPos(p);
        char line[256];
        snprintf(line, sizeof(line),
                 "%2d. %-22s  cc=%-3d  %s%s\n",
                 idx++, n.empty() ? "?" : n.c_str(), cc,
                 (p == g_LocalPlayer) ? "(you) " : "",
                 pos.ok ? "" : "(no pos)");
        out += line;
    }
    if (out.empty()) out = "No players visible.";
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "/players";
    g_infoBody  = out;
    g_showInfoPopup = true;
}

inline void CmdAI(const std::vector<std::string>&) {
    std::vector<void*> snap;
    {
        std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
        snap = g_AIInstances;
    }
    std::string out;
    int idx = 0;
    for (void* ai : snap) {
        if (!ai) continue;
        void* base    = Seh_DerefPtr(ai, g_off_AIBase);
        int enemyType = Seh_ReadInt(base, Offsets::AIBase_enemyType);
        int hp        = Seh_ReadInt(base, Offsets::AIBase_health);
        char line[160];
        snprintf(line, sizeof(line), "%2d. type=%d  hp=%d\n", idx++, enemyType, hp);
        out += line;
    }
    if (out.empty()) out = "No AI enemies visible.";
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "/ai";
    g_infoBody  = out;
    g_showInfoPopup = true;
}

inline void CmdLevel(const std::vector<std::string>&) {
    if (!g_LocalPlayer) { EchoLocal("LocalPlayer not yet available."); return; }
    void* pd = Seh_DerefPtr(g_LocalPlayer, Offsets::Player_myPlayerData);
    if (!pd) { EchoLocal("PlayerData not available."); return; }
    int xp = Seh_ReadInt(pd, Offsets::PlayerData_xpAmount);
    int lvl = Seh_XPLevel(xp);
    long long gems = oGetGemAmount ? oGetGemAmount(pd) : 0;
    long long bc   = oGetByteCoinAmount ? oGetByteCoinAmount(pd) : 0;
    char b[256];
    snprintf(b, sizeof(b),
             "Level: %d\nXP:    %d\nGems:  %lld\nBytes: %lld", lvl, xp, gems, bc);
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "/lvl";
    g_infoBody  = b;
    g_showInfoPopup = true;
}

inline void CmdFPS(const std::vector<std::string>&) {
    ImGuiIO& io = ImGui::GetIO();
    char b[96];
    snprintf(b, sizeof(b), "%.1f fps  (%.2f ms)", io.Framerate, 1000.f / io.Framerate);
    Toast("/fps", b, NotificationType::Info);
}

inline void CmdPortals(const std::vector<std::string>&) {
    std::vector<Vector2i> snap;
    {
        std::lock_guard<std::mutex> lk(g_exitPortalLocationsMutex);
        snap = g_exitPortalLocations;
    }
    std::string out;
    int idx = 0;
    for (const auto& p : snap) {
        char line[80];
        snprintf(line, sizeof(line), "%2d. (%d, %d)\n", idx++, p.x, p.y);
        out += line;
    }
    if (out.empty()) out = "No exit portals tracked.";
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "/portals";
    g_infoBody  = out;
    g_showInfoPopup = true;
}

inline void CmdGifts(const std::vector<std::string>&) {
    std::vector<Vector2i> snap;
    {
        std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);
        snap = g_giftBoxLocations;
    }
    std::string out;
    int idx = 0;
    for (const auto& p : snap) {
        char line[80];
        snprintf(line, sizeof(line), "%2d. (%d, %d)\n", idx++, p.x, p.y);
        out += line;
    }
    if (out.empty()) out = "No gift boxes tracked.";
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "/gifts";
    g_infoBody  = out;
    g_showInfoPopup = true;
}

inline void CmdInv(const std::vector<std::string>&) {

    RunGems();
}

inline void CmdDist(const std::vector<std::string>& args) {
    if (args.size() < 2) { EchoLocal("Usage: /dist <username>"); return; }
    void* tgt = FindPlayerByName(args[1]);
    if (!tgt) { EchoLocal("Player not found: " + args[1]); return; }
    auto a = Seh_GetPlayerPos(g_LocalPlayer);
    auto b = Seh_GetPlayerPos(tgt);
    if (!a.ok || !b.ok) { EchoLocal("Position read failed."); return; }
    float dx = a.v.x - b.v.x, dy = a.v.y - b.v.y;
    float dist = sqrtf(dx*dx + dy*dy);
    char buf[96];
    snprintf(buf, sizeof(buf), "%s is %.2f units (%.1f tiles) away",
             StripTags(ReadPlayerNickname(tgt)).c_str(), dist, dist / 0.32f);
    Toast("/dist", buf, NotificationType::Info);
}

inline void CmdDump(const std::vector<std::string>& args) {
    if (args.size() < 2) { EchoLocal("Usage: /dump <username>"); return; }
    void* tgt = FindPlayerByName(args[1]);
    if (!tgt) { EchoLocal("Player not found: " + args[1]); return; }
    std::string disp  = ReadPlayerNickname(tgt);
    std::string id    = ReadPlayerId(tgt);
    int cc            = ReadPlayerCountryCode(tgt);
    auto pos          = Seh_GetPlayerPos(tgt);
    void* pd          = Seh_DerefPtr(tgt, Offsets::Player_myPlayerData);
    int xp = pd ? Seh_ReadInt(pd, Offsets::PlayerData_xpAmount) : 0;
    int lvl = Seh_XPLevel(xp);
    long long gems = (pd && oGetGemAmount) ? oGetGemAmount(pd) : 0;
    long long bc   = (pd && oGetByteCoinAmount) ? oGetByteCoinAmount(pd) : 0;
    char b[1024];
    snprintf(b, sizeof(b),
        "Display:      %s\n"
        "Stripped:     %s\n"
        "PlayerID:     %s\n"
        "Country:      %d\n"
        "Level:        %d  (xp %d)\n"
        "Gems / Bytes: %lld / %lld\n"
        "World pos:    (%.2f, %.2f)\n"
        "Is local:     %s",
        disp.c_str(), StripTags(disp).c_str(), id.c_str(), cc,
        lvl, xp, gems, bc, pos.v.x, pos.v.y,
        (tgt == g_LocalPlayer) ? "yes" : "no");
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "/dump " + args[1];
    g_infoBody  = b;
    g_showInfoPopup = true;
}

inline void PushBack(Vector3 p) {
    std::lock_guard<std::mutex> lk(g_stateMutex);
    g_backStack.push_back(p);
    if (g_backStack.size() > 32) g_backStack.pop_front();
}

static inline bool ParseInt(const std::string& s, int& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (!end || *end != '\0') return false;
    out = (int)v;
    return true;
}
static inline bool ParseFloat(const std::string& s, float& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    double v = std::strtod(s.c_str(), &end);
    if (!end || *end != '\0') return false;
    out = (float)v;
    return true;
}

inline void CmdTP(const std::vector<std::string>& args) {
    if (args.size() < 3) { EchoLocal("Usage: /tp <x> <y>"); return; }
    if (!g_LocalPlayer) { EchoLocal("No local player."); return; }
    int x = 0, y = 0;
    if (!ParseInt(args[1], x) || !ParseInt(args[2], y)) { EchoLocal("Bad coords."); return; }
    auto cur = Seh_GetPlayerPos(g_LocalPlayer);
    if (cur.ok) PushBack(cur.v);
    Vector3 target = TileCenter({ x, y });
    if (target.x == 0 && target.y == 0) {
        target.x = x * 0.32f;
        target.y = y * 0.32f;
    }
    Seh_SetPlayerPos(g_LocalPlayer, target);
    char b[80]; snprintf(b, sizeof(b), "(%d, %d)", x, y);
    Toast("/tp", b, NotificationType::Success);
}

inline void CmdSpawn(const std::vector<std::string>&) {
    if (!g_WorldController) { EchoLocal("Not in a world."); return; }
    void* world = Seh_DerefPtr(g_WorldController, g_off_WCWorld);
    if (!world) { EchoLocal("World ptr null."); return; }
    Vector2i spawnTile = Seh_ReadV2i(world, 0xF0);
    if (spawnTile.x == 0 && spawnTile.y == 0) {
        EchoLocal("Spawn tile unavailable.");
        return;
    }
    CmdTP({"/tp", std::to_string(spawnTile.x), std::to_string(spawnTile.y)});
}

inline void CmdBack(const std::vector<std::string>&) {
    Vector3 prev;
    {
        std::lock_guard<std::mutex> lk(g_stateMutex);
        if (g_backStack.empty()) { EchoLocal("Back stack empty."); return; }
        prev = g_backStack.back();
        g_backStack.pop_back();
    }
    if (!g_LocalPlayer) return;
    Seh_SetPlayerPos(g_LocalPlayer, prev);
    Toast("/back", "Restored previous position.", NotificationType::Success);
}

inline void CmdSetHome(const std::vector<std::string>& args) {
    if (args.size() < 2) { EchoLocal("Usage: /sethome <name>"); return; }
    if (!g_LocalPlayer || !g_WorldController) { EchoLocal("No world."); return; }
    auto p = Seh_GetPlayerPos(g_LocalPlayer);
    if (!p.ok) { EchoLocal("Couldn't read pos."); return; }
    Vector2i tile = Seh_W2M(g_WorldController, p.v);
    {
        std::lock_guard<std::mutex> lk(g_stateMutex);
        g_homes[ToLower(args[1])] = tile;
    }
    char b[80]; snprintf(b, sizeof(b), "%s = (%d, %d)", args[1].c_str(), tile.x, tile.y);
    Toast("/sethome", b, NotificationType::Success);
}

inline void CmdHome(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::string list;
        {
            std::lock_guard<std::mutex> lk(g_stateMutex);
            for (auto& kv : g_homes) {
                char l[80]; snprintf(l, sizeof(l), "%s -> (%d, %d)\n",
                                     kv.first.c_str(), kv.second.x, kv.second.y);
                list += l;
            }
        }
        if (list.empty()) list = "No homes saved.\nUsage: /sethome <name>";
        std::lock_guard<std::mutex> lk(g_popupMutex);
        g_infoTitle = "/home";
        g_infoBody  = list;
        g_showInfoPopup = true;
        return;
    }
    Vector2i tile{0,0}; bool found = false;
    {
        std::lock_guard<std::mutex> lk(g_stateMutex);
        auto it = g_homes.find(ToLower(args[1]));
        if (it != g_homes.end()) { tile = it->second; found = true; }
    }
    if (!found) { EchoLocal("No home named '" + args[1] + "'."); return; }
    CmdTP({"/tp", std::to_string(tile.x), std::to_string(tile.y)});
}

inline void CmdExit(const std::vector<std::string>&) {
    if (!g_LocalPlayer) { EchoLocal("No local player."); return; }
    std::vector<Vector2i> snap;
    {
        std::lock_guard<std::mutex> lk(g_exitPortalLocationsMutex);
        snap = g_exitPortalLocations;
    }
    if (snap.empty()) { EchoLocal("No exit portal tracked."); return; }
    auto cur = Seh_GetPlayerPos(g_LocalPlayer);
    if (!cur.ok) return;

    Vector2i best = snap[0];
    float bestD = 1e9f;
    for (const auto& p : snap) {
        Vector3 wp = TileCenter(p);
        float dx = wp.x - cur.v.x, dy = wp.y - cur.v.y;
        float d  = dx*dx + dy*dy;
        if (d < bestD) { bestD = d; best = p; }
    }
    CmdTP({"/tp", std::to_string(best.x), std::to_string(best.y)});
}

inline void CmdRecycle(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        EchoLocal("Usage: /recycle gems|fish");
        return;
    }
    std::string what = ToLower(args[1]);
    if (what == "gems" || what == "gemstones") {

        GemSeller::g_autoSell = true;
        Toast("/recycle", "Gemstone recycler armed.", NotificationType::Success);
        return;
    }
    if (what == "fish") {
        EchoLocal("Fish recycle not wired yet (game has no batched outgoing helper).");
        return;
    }
    EchoLocal("Unknown recycle target: " + what);
}

typedef void(__fastcall* tSendDropItem)(Vector2i mp, PlayerData_InventoryKey ik,
                                        short amount, void* invData, void* methodInfo);
inline tSendDropItem oSendDropItem = nullptr;

static inline void Seh_SendDrop(Vector2i mp, PlayerData_InventoryKey ik, short amount) {
    if (!oSendDropItem) return;
    __try { oSendDropItem(mp, ik, amount, nullptr, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

inline void CmdDrop(const std::vector<std::string>& args) {
    if (args.size() < 3) { EchoLocal("Usage: /drop <blockId> <amount>"); return; }
    if (!oSendDropItem) { EchoLocal("Drop helper not wired."); return; }
    if (!g_LocalPlayer) return;
    int bt = 0, am = 0;
    if (!ParseInt(args[1], bt) || !ParseInt(args[2], am)) { EchoLocal("Bad numbers."); return; }
    if (am <= 0 || am > 32767) { EchoLocal("Amount out of range."); return; }
    Vector2i tile{0,0};
    auto p = Seh_GetPlayerPos(g_LocalPlayer);
    if (p.ok) tile = Seh_W2M(g_WorldController, p.v);
    PlayerData_InventoryKey ik{ bt, (int)InventoryItemType::Block };
    Seh_SendDrop(tile, ik, (short)am);
    char b[80]; snprintf(b, sizeof(b), "blockId %d x%d", bt, am);
    Toast("/drop", b, NotificationType::Info);
}

struct DropSlotSnap { bool ok; PlayerData_InventoryKey ik; int amount; };
static inline DropSlotSnap Seh_ReadActiveSlot(void* ic) {
    DropSlotSnap r{ false, {0,0}, 0 };
    if (!ic) return r;
    __try {
        int slot = *(int*)((uintptr_t)ic + 0x17C);
        if (slot < 0) return r;
        Il2CppArray* items  = *(Il2CppArray**)((uintptr_t)ic + 0x188);
        Il2CppArray* counts = *(Il2CppArray**)((uintptr_t)ic + 0x190);
        if (!items || !counts) return r;
        if (slot >= (int)items->max_length) return r;
        r.ik     = ((PlayerData_InventoryKey*)items->items)[slot];
        r.amount = (int)((short*)counts->items)[slot];
        r.ok     = (r.amount > 0);
    } __except (EXCEPTION_EXECUTE_HANDLER) { r.ok = false; }
    return r;
}
static inline void Seh_InvokeActualDrop(void* ic, PlayerData_InventoryKey ik, int amt) {
    if (!oInvActualDrop) return;
    __try { oInvActualDrop(ic, nullptr, ik, amt, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

inline void CmdDropStack(const std::vector<std::string>&) {
    if (!g_InventoryControl) { EchoLocal("InventoryControl not ready — tap-and-hold an inventory slot first."); return; }
    if (!oInvActualDrop)     { EchoLocal("ActualDropButtonAction not wired."); return; }
    DropSlotSnap s = Seh_ReadActiveSlot(g_InventoryControl);
    if (!s.ok) { EchoLocal("No slot selected. Tap-and-hold an item, then /dropstack."); return; }
    Seh_InvokeActualDrop(g_InventoryControl, s.ik, s.amount);
    char b[96];
    snprintf(b, sizeof(b), "Dropped %d (blockType %d, itemType %d)",
             s.amount, s.ik.blockType, s.ik.itemType);
    Toast("/dropstack", b, NotificationType::Success);
}

inline void CmdSell(const std::vector<std::string>&) {

    GemSeller::g_autoSell = !GemSeller::g_autoSell;
    Toast("/sell", GemSeller::g_autoSell ? "auto-sell ON" : "auto-sell OFF",
          GemSeller::g_autoSell ? NotificationType::Success : NotificationType::Info);
}

inline void CmdReply(const std::string& fullLine, const std::vector<std::string>& args) {
    if (args.size() < 2) { EchoLocal("Usage: /r <message>"); return; }
    std::string id;
    { std::lock_guard<std::mutex> lk(g_stateMutex); id = g_lastPmFromId; }
    if (id.empty()) { EchoLocal("No recent PM sender to reply to."); return; }
    std::string body = RestAfter(fullLine, 1);
    if (body.empty()) return;
    if (!oSubmitPrivateChatMessage) { EchoLocal("Private chat helper missing."); return; }
    Seh_SubmitPrivate(MakeIl2Str(id), MakeIl2Str(body));
    Toast("/r", "Reply sent.", NotificationType::Success);
}

inline void CmdRepeat(const std::string& fullLine, const std::vector<std::string>& args) {
    if (args.size() < 3) { EchoLocal("Usage: /repeat <count> <command>"); return; }
    int n = 0;
    try { n = std::stoi(args[1]); } catch (...) { EchoLocal("Bad count."); return; }
    if (n <= 0 || n > 32) { EchoLocal("Count must be 1..32."); return; }
    std::string inner = RestAfter(fullLine, 2);

    if (!inner.empty() && inner[0] != '/') inner = "/" + inner;
    for (int i = 0; i < n; ++i) TryDispatch(inner);
}

inline void CmdDelay(const std::string& fullLine, const std::vector<std::string>& args) {
    if (args.size() < 3) { EchoLocal("Usage: /delay <ms> <command>"); return; }
    int ms = 0;
    try { ms = std::stoi(args[1]); } catch (...) { EchoLocal("Bad ms."); return; }
    std::string inner = RestAfter(fullLine, 2);
    if (!inner.empty() && inner[0] != '/') inner = "/" + inner;
    DelayedCmd d;
    d.fireAt = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    d.line   = inner;
    std::lock_guard<std::mutex> lk(g_stateMutex);
    g_delayedCmds.push_back(d);
}

inline void CmdSay(const std::string& fullLine) {
    std::string body = RestAfter(fullLine, 1);
    if (body.empty()) { EchoLocal("Usage: /say <text>"); return; }
    if (!oSubmitWorldChatMessage) return;
    Seh_SubmitWorld(MakeIl2Str(body));
}

inline void CmdSpam(const std::string& fullLine, const std::vector<std::string>& args) {
    if (args.size() < 3) { EchoLocal("Usage: /spam <count> <text>"); return; }
    int n = 0;
    try { n = std::stoi(args[1]); } catch (...) { EchoLocal("Bad count."); return; }
    if (n <= 0 || n > 8) { EchoLocal("Spam count capped 1..8 to avoid kicks."); return; }
    std::string body = RestAfter(fullLine, 2);
    if (body.empty()) return;
    auto now = std::chrono::steady_clock::now();
    for (int i = 0; i < n; ++i) {
        DelayedCmd d;

        d.fireAt = now + std::chrono::milliseconds(700LL * i);
        d.line   = "/say " + body;
        std::lock_guard<std::mutex> lk(g_stateMutex);
        g_delayedCmds.push_back(d);
    }
    Toast("/spam", "Queued " + std::to_string(n) + " messages.", NotificationType::Info);
}

inline void CmdScan(const std::vector<std::string>& args) {
    if (args.size() < 2) { EchoLocal("Usage: /scan <blockId>"); g_scanBlockId = -1; return; }
    int bid = 0;
    try { bid = std::stoi(args[1]); } catch (...) { EchoLocal("Bad blockId."); return; }
    std::lock_guard<std::mutex> lk(g_stateMutex);
    g_scanBlockId = bid;
    g_scanUntil   = std::chrono::steady_clock::now() + std::chrono::seconds(15);
    Toast("/scan", "Highlighting blockId " + std::to_string(bid) + " for 15s.",
          NotificationType::Info);
}

inline void CmdCls(const std::vector<std::string>&) {
    std::lock_guard<std::mutex> lk(g_PacketLogMutex);
    g_PacketLog.clear();
    Toast("/cls", "Packet log cleared.", NotificationType::Info);
}

static inline void Seh_Reload() {
    if (!oReloadGame) return;
    __try { oReloadGame(); } __except (EXCEPTION_EXECUTE_HANDLER) {}
}
inline void CmdReload(const std::vector<std::string>&) {
    if (!oReloadGame) { EchoLocal("Reload helper not wired."); return; }
    Toast("/reload", "Reloading game...", NotificationType::Info);
    Seh_Reload();
}

static inline void Seh_LogOut() {
    if (!MultiboxMode::g_UI_LogOut) return;
    __try { MultiboxMode::g_UI_LogOut(); } __except (EXCEPTION_EXECUTE_HANDLER) {}
}
inline void CmdLogout(const std::vector<std::string>&) {
    if (!MultiboxMode::g_UI_LogOut) {
        EchoLocal("Logout helper not wired."); return;
    }
    Toast("/logout", "Logging out...", NotificationType::Info);
    Seh_LogOut();
}

inline void CmdRejoin(const std::vector<std::string>&) {
    std::string world = GetCachedWorldName();
    if (world.empty()) { EchoLocal("Not in a world."); return; }
    Toast("/rejoin", "Re-entering " + world, NotificationType::Info);

    CmdWarp({"/warp", "PWE"});

    DelayedCmd d;
    d.fireAt = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    d.line   = "/warp " + world;
    std::lock_guard<std::mutex> lk(g_stateMutex);
    g_delayedCmds.push_back(d);
}

inline void CmdMacro(const std::string& fullLine, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        EchoLocal("Usage: /macro save <name> <cmd1>;<cmd2>;...  |  /macro run <name>  |  /macro list");
        return;
    }
    std::string sub = ToLower(args[1]);
    if (sub == "list") {
        std::string out;
        {
            std::lock_guard<std::mutex> lk(g_stateMutex);
            for (auto& kv : g_macros) {
                out += kv.first + " (" + std::to_string(kv.second.size()) + " cmds)\n";
            }
        }
        if (out.empty()) out = "No macros saved.";
        std::lock_guard<std::mutex> lk(g_popupMutex);
        g_infoTitle = "/macro list";
        g_infoBody  = out;
        g_showInfoPopup = true;
        return;
    }
    if (sub == "save") {
        if (args.size() < 4) { EchoLocal("/macro save <name> <cmds joined by ;>"); return; }
        std::string body = RestAfter(fullLine, 3);
        std::vector<std::string> seq;
        std::string cur;
        for (char c : body) {
            if (c == ';') { if (!cur.empty()) { seq.push_back(cur); cur.clear(); } }
            else cur.push_back(c);
        }
        if (!cur.empty()) seq.push_back(cur);
        for (auto& s : seq) {
            while (!s.empty() && std::isspace((unsigned char)s.front())) s.erase(s.begin());
            while (!s.empty() && std::isspace((unsigned char)s.back()))  s.pop_back();
            if (!s.empty() && s[0] != '/') s.insert(0, "/");
        }
        std::lock_guard<std::mutex> lk(g_stateMutex);
        g_macros[ToLower(args[2])] = seq;
        Toast("/macro", "Saved '" + args[2] + "' (" + std::to_string(seq.size()) + " cmds).",
              NotificationType::Success);
        return;
    }
    if (sub == "run") {
        if (args.size() < 3) { EchoLocal("/macro run <name>"); return; }
        std::vector<std::string> seq;
        {
            std::lock_guard<std::mutex> lk(g_stateMutex);
            auto it = g_macros.find(ToLower(args[2]));
            if (it == g_macros.end()) { EchoLocal("No macro: " + args[2]); return; }
            seq = it->second;
        }
        for (auto& c : seq) TryDispatch(c);
        return;
    }
    EchoLocal("Unknown /macro sub-command: " + sub);
}

inline void CmdAnnounce(const std::string& fullLine) {
    std::string body = RestAfter(fullLine, 1);
    if (body.empty()) { EchoLocal("Usage: /announce <text>"); return; }
    std::lock_guard<std::mutex> lk(g_stateMutex);
    g_announceText  = body;
    g_announceUntil = std::chrono::steady_clock::now() + std::chrono::seconds(4);
}

inline const char* kHelpText =
"== ZMod /commands ==\n"
"Toggles:   /god  /fly  /freecam  /aimbot  /esp  /rainbow [on|off]\n"
"           /speed <n>  /zoom <x>\n"
"Info:      /pos  /coords  /world  /players  /ai  /portals  /gifts\n"
"           /lvl  /fps  /inv  /dist <user>  /dump <user>  /info <user>\n"
"           /flag <user>\n"
"Teleport:  /tp <x> <y>  /spawn  /back  /sethome <name>  /home [name]\n"
"           /exit  (nearest exit portal)\n"
"Warp:      /warp <world> [entry]  /w <world>  /pwe\n"
"Inventory: /gems  /recycle gems|fish  /sell  /drop <id> <amt>\n"
"Chat:      /msg <user> <text>  /r <text>  /gm <text>  /say <text>\n"
"           /spam <count> <text>  (max 8)\n"
"Macros:    /repeat <count> <cmd>  /delay <ms> <cmd>\n"
"           /macro save <name> <cmd1>;<cmd2>;...  /macro run <name>  /macro list\n"
"Power:     /reload  /logout  /rejoin  /respawn\n"
"Diag:      /math <expr>  /scan <blockId>  /cls\n"
"Fun:       /announce <text>\n";

inline void CmdHelp(const std::vector<std::string>&) {
    std::lock_guard<std::mutex> lk(g_popupMutex);
    g_infoTitle = "ZMod Commands";
    g_infoBody  = kHelpText;
    g_showInfoPopup = true;
}

inline void Tick() {
    auto now = std::chrono::steady_clock::now();
    std::vector<std::string> due;
    {
        std::lock_guard<std::mutex> lk(g_stateMutex);
        for (auto it = g_delayedCmds.begin(); it != g_delayedCmds.end();) {
            if (it->fireAt <= now) { due.push_back(it->line); it = g_delayedCmds.erase(it); }
            else ++it;
        }
        if (g_scanBlockId >= 0 && now > g_scanUntil) g_scanBlockId = -1;
        if (!g_announceText.empty() && now > g_announceUntil) g_announceText.clear();
    }
    for (auto& s : due) TryDispatch(s);
}

static inline Vector2i Seh_ReadPlayerTile() {
    if (!g_LocalPlayer) return { 0, 0 };
    Vector2i p{0,0};
    __try {
        p = *(Vector2i*)((uintptr_t)g_LocalPlayer + g_off_Player_currentPlayerMapPoint);
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    return p;
}

static inline void Seh_SetBlockBoth(void* wc, void* world, int bt, Vector2i pos,
                                    Il2CppString* a, Il2CppString* b) {
    if (oWCSetBlock) {
        __try { oWCSetBlock(wc, bt, pos.x, pos.y, nullptr); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    if (oWorldSetBlock) {
        __try { oWorldSetBlock(world, bt, pos, a, b, false, nullptr); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
}

static inline void* Seh_GetWorldPtr() {
    if (!g_WorldController) return nullptr;
    __try { return *(void**)((uintptr_t)g_WorldController + g_off_WCWorld); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

inline void SpawnBlockHere(int blockType, const char* tag, const char* msg) {
    if (!g_LocalPlayer || !g_WorldController) {
        Toast(tag, "Player not available.", NotificationType::Error);
        return;
    }
    if (!oWCSetBlock || !oWorldSetBlock) {
        Toast(tag, "Block setter not wired (rebuild after Setup).",
              NotificationType::Error);
        return;
    }
    Vector2i pos = Seh_ReadPlayerTile();
    if (pos.x == 0 && pos.y == 0) {
        Toast(tag, "Player tile unreadable.", NotificationType::Error);
        return;
    }
    void* world = Seh_GetWorldPtr();
    if (!world) {
        Toast(tag, "World pointer unavailable.", NotificationType::Error);
        return;
    }
    Il2CppString* empty = MakeIl2Str("");
    Seh_SetBlockBoth(g_WorldController, world, blockType, pos, empty, empty);
    Toast(tag, msg, NotificationType::Success);
}

inline void CmdPWE      (const std::vector<std::string>&) {
    SpawnBlockHere(1605, "/pwe",     "PWE spawned (view-only — server rejects transactions).");
}
inline void CmdBank     (const std::vector<std::string>&) {
    SpawnBlockHere(3576, "/bank",    "Bank spawned (view-only — server rejects transactions).");
}
inline void CmdFavHolo  (const std::vector<std::string>&) {
    SpawnBlockHere(2145, "/favholo", "Favourite Worlds hologram spawned.");
}
inline void CmdFishGemUI(const std::vector<std::string>&) {
    SpawnBlockHere(2504, "/fgemui",  "Fishing Recycler spawned.");
}
inline void CmdMineGemUI(const std::vector<std::string>&) {
    SpawnBlockHere(1922, "/mgemui",  "Mining Gemstone Recycler spawned.");
}
inline void CmdMineWheel(const std::vector<std::string>&) {
    SpawnBlockHere(4169, "/mwheel",  "Mining Wheel of Fortune spawned.");
}

static inline int Seh_LocalPlayerClanNameLen() {
    if (!g_LocalPlayer) return -1;
    void* pd = nullptr;
    __try { pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    if (!pd) return -1;
    Il2CppString* clanName = nullptr;
    __try { clanName = *(Il2CppString**)((uintptr_t)pd + 0x348); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    if (!clanName) return 0;
    int n = 0;
    __try { n = clanName->length; }
    __except (EXCEPTION_EXECUTE_HANDLER) { n = 0; }
    return n;
}

inline void CmdClanTotem(const std::vector<std::string>&) {

    int clanLen = Seh_LocalPlayerClanNameLen();
    if (clanLen == 0) {
        Toast("/ctotem", "You are not in a clan.", NotificationType::Warning);
        return;
    }
    SpawnBlockHere(3466, "/ctotem", "Clan Totem spawned.");
}
inline void CmdDquest   (const std::vector<std::string>&) {
    Toast("/dquest",
          "Direct UI open pending — no matching world block to spawn.",
          NotificationType::Warning);
}
inline void CmdModTools (const std::vector<std::string>&) {
    Toast("/modtools",
          "Direct UI open pending — admin-only, no spawnable block.",
          NotificationType::Warning);
}

inline bool TryDispatch(const std::string& line) {
    if (line.empty() || line[0] != '/') return false;
    auto args = Tokenize(line);
    if (args.empty()) return false;
    std::string cmd = ToLower(args[0]);

    if (cmd == "/warp" || cmd == "/w") { CmdWarp(args);    return true; }
    if (cmd == "/pwe")                  { CmdPWE(args);     return true; }
    if (cmd == "/mw" || cmd == "/mineworld") { CmdWarp({"/warp","MINEWORLD"}); return true; }
    if (cmd == "/tp")                   { CmdTp(args);            return true; }
    if (cmd == "/tele" || cmd == "/silentwarp") { CmdTele(args);   return true; }
    if (cmd == "/teleprobe")            { CmdTeleProbe(args);     return true; }
    if (cmd == "/cp" || cmd == "/checkpoint") { CmdCheckpoint(args); return true; }
    if (cmd == "/rez" || cmd == "/resurrect") { CmdResurrect(args);  return true; }
    if (cmd == "/dumpblocks" || cmd == "/blockdump") {

        std::string res = DumpWorldBlocksToJson();
        EchoLocal(res);
        Toast("/dumpblocks", res, NotificationType::Success);
        return true;
    }
    if (cmd == "/probepreds" || cmd == "/blockpreds") {

        std::string res = DumpBlockPredicatesToJson();
        EchoLocal(res);
        Toast("/probepreds", res, NotificationType::Success);
        return true;
    }
    if (cmd == "/respawn")              { CmdRespawn(args); return true; }
    if (cmd == "/gems")                 { RunGems();        return true; }
    if (cmd == "/math" || cmd == "/calc" || cmd == "/eval")
                                        { CmdMath(line);    return true; }
    if (cmd == "/flag")                 { CmdFlag(args);    return true; }
    if (cmd == "/msg")                  { CmdMsg(line,args);return true; }
    if (cmd == "/gm")                   { CmdGlobal(line);  return true; }
    if (cmd == "/info" || cmd == "/who"){ CmdInfo(args);    return true; }

    if (cmd == "/god")                  { CmdToggleGod(args);     return true; }
    if (cmd == "/fly")                  { CmdToggleFly(args);     return true; }
    if (cmd == "/freecam")              { CmdToggleFreecam(args); return true; }
    if (cmd == "/aimbot")               { CmdToggleAimbot(args);  return true; }
    if (cmd == "/door" || cmd == "/doors") {
        Door::g_walkThroughAll = !Door::g_walkThroughAll;
        const char* state = Door::g_walkThroughAll ? "ON (walk through all)" : "off (access-gated)";
        EchoLocal(std::string("Doors: ") + state);
        Toast("/door", state, NotificationType::Success);
        return true;
    }
    if (cmd == "/esp")                  { CmdToggleESP(args);     return true; }
    if (cmd == "/rainbow")              { CmdRainbow(args);       return true; }
    if (cmd == "/speed")                { CmdSpeed(args);         return true; }
    if (cmd == "/zoom")                 { CmdZoom(args);          return true; }

    if (cmd == "/help" || cmd == "/?")  { CmdHelp(args);     return true; }
    if (cmd == "/pos" || cmd == "/coords") { CmdPos(args);   return true; }
    if (cmd == "/world")                { CmdWorldInfo(args);return true; }
    if (cmd == "/players")              { CmdPlayers(args);  return true; }
    if (cmd == "/ai" || cmd == "/mobs") { CmdAI(args);       return true; }
    if (cmd == "/lvl" || cmd == "/level"){ CmdLevel(args);   return true; }
    if (cmd == "/fps")                  { CmdFPS(args);      return true; }
    if (cmd == "/portals")              { CmdPortals(args);  return true; }
    if (cmd == "/gifts")                { CmdGifts(args);    return true; }
    if (cmd == "/inv")                  { CmdInv(args);      return true; }
    if (cmd == "/dist")                 { CmdDist(args);     return true; }
    if (cmd == "/dump")                 { CmdDump(args);     return true; }

    if (cmd == "/tp")                   { CmdTP(args);       return true; }
    if (cmd == "/spawn")                { CmdSpawn(args);    return true; }
    if (cmd == "/back")                 { CmdBack(args);     return true; }
    if (cmd == "/sethome")              { CmdSetHome(args);  return true; }
    if (cmd == "/home")                 { CmdHome(args);     return true; }
    if (cmd == "/exit")                 { CmdExit(args);     return true; }

    if (cmd == "/recycle")              { CmdRecycle(args);  return true; }
    if (cmd == "/sell")                 { CmdSell(args);     return true; }
    if (cmd == "/drop")                 { CmdDrop(args);     return true; }
    if (cmd == "/dropstack")            { CmdDropStack(args); return true; }

    if (cmd == "/r")                    { CmdReply(line,args); return true; }
    if (cmd == "/repeat")               { CmdRepeat(line,args);return true; }
    if (cmd == "/delay")                { CmdDelay(line,args); return true; }
    if (cmd == "/say")                  { CmdSay(line);        return true; }
    if (cmd == "/spam")                 { CmdSpam(line,args);  return true; }

    if (cmd == "/reload")               { CmdReload(args);  return true; }
    if (cmd == "/logout")               { CmdLogout(args);  return true; }
    if (cmd == "/rejoin")               { CmdRejoin(args);  return true; }

    if (cmd == "/scan")                 { CmdScan(args);    return true; }
    if (cmd == "/cls")                  { CmdCls(args);     return true; }
    if (cmd == "/macro")                { CmdMacro(line,args); return true; }
    if (cmd == "/announce")             { CmdAnnounce(line); return true; }

    if (cmd == "/pwe")                  { CmdPWE(args);        return true; }
    if (cmd == "/bank")                 { CmdBank(args);       return true; }
    if (cmd == "/favholo")              { CmdFavHolo(args);    return true; }
    if (cmd == "/fgemui")               { CmdFishGemUI(args);  return true; }
    if (cmd == "/mgemui")               { CmdMineGemUI(args);  return true; }
    if (cmd == "/mwheel")               { CmdMineWheel(args);  return true; }
    if (cmd == "/ctotem")               { CmdClanTotem(args);  return true; }
    if (cmd == "/dquest")               { CmdDquest(args);     return true; }
    if (cmd == "/modtools")             { CmdModTools(args);   return true; }

    return false;
}

inline void __fastcall hkSubmitWorldChatMessage(Il2CppString* msg, void* methodInfo) {
    std::string copy = ReadIl2Str(msg);
    if (!copy.empty() && copy[0] == '/' && TryDispatch(copy)) return;
    if (oSubmitWorldChatMessage) oSubmitWorldChatMessage(msg, methodInfo);
}
inline void __fastcall hkSubmitClanChatMessage(Il2CppString* msg, void* methodInfo) {
    std::string copy = ReadIl2Str(msg);
    if (!copy.empty() && copy[0] == '/' && TryDispatch(copy)) return;
    if (oSubmitClanChatMessage) oSubmitClanChatMessage(msg, methodInfo);
}
inline void __fastcall hkSubmitPrivateChatMessage(Il2CppString* playerId,
                                                  Il2CppString* msg,
                                                  void* methodInfo) {
    std::string copy = ReadIl2Str(msg);
    if (!copy.empty() && copy[0] == '/' && TryDispatch(copy)) return;
    if (oSubmitPrivateChatMessage) oSubmitPrivateChatMessage(playerId, msg, methodInfo);
}
inline void __fastcall hkSubmitPetCommandChatMessage(int petId,
                                                     Il2CppString* msg,
                                                     void* methodInfo) {
    std::string copy = ReadIl2Str(msg);
    if (!copy.empty() && copy[0] == '/' && TryDispatch(copy)) return;
    if (oSubmitPetCommandChatMessage) oSubmitPetCommandChatMessage(petId, msg, methodInfo);
}

inline bool __fastcall hkChatUISubmit(void* thisPtr, Il2CppString* text, void* methodInfo) {
    std::string copy = ReadIl2Str(text);
    if (!copy.empty() && copy[0] == '/' && TryDispatch(copy)) {
        return false;
    }
    if (oChatUISubmit) return oChatUISubmit(thisPtr, text, methodInfo);
    return true;
}

static inline Il2CppString* Seh_ReadChatMessageText(void* chatMsg) {
    if (!chatMsg) return nullptr;
    __try { return *(Il2CppString**)((uintptr_t)chatMsg + 0x30); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

inline void __fastcall hkSubmitGlobalChatMessage(void* chatMsg, void* methodInfo) {
    Il2CppString* textObj = Seh_ReadChatMessageText(chatMsg);
    if (textObj) {
        std::string copy = ReadIl2Str(textObj);
        if (!copy.empty() && copy[0] == '/' && TryDispatch(copy)) return;
    }
    if (oSubmitGlobalChatMessage) oSubmitGlobalChatMessage(chatMsg, methodInfo);
}

inline void SetupHooks() {

    {
        void* asmCs = Manifest::detail::SafeImageByName("Assembly-CSharp.dll");
        void* cChatUI = asmCs
            ? Manifest::detail::SafeClassFromName(asmCs, "", "ChatUI")
            : nullptr;
        if (!cChatUI) cChatUI = DropStackInjector::FindClassAnywhere("", "ChatUI");
        void* submitPtr = cChatUI
            ? DropStackInjector::ResolveMethod(cChatUI, "Submit", 1)
            : nullptr;
        if (submitPtr) {
            DoHook(submitPtr, &hkChatUISubmit, (void**)&oChatUISubmit,
                   "ChatUI::Submit (chokepoint)");
        } else {
            std::cout << "[Commands] ChatUI::Submit not resolved — falling back "
                         "to the per-channel Submit*ChatMessage hooks.\n";
        }
    }

    DoHook(GetMethodPtr("OutgoingMessages", "SubmitWorldChatMessage"),
           &hkSubmitWorldChatMessage,
           (void**)&oSubmitWorldChatMessage,
           "OutgoingMessages::SubmitWorldChatMessage");
    DoHook(GetMethodPtr("OutgoingMessages", "SubmitClanChatMessage"),
           &hkSubmitClanChatMessage,
           (void**)&oSubmitClanChatMessage,
           "OutgoingMessages::SubmitClanChatMessage");
    DoHook(GetMethodPtr("OutgoingMessages", "SubmitGlobalChatMessage"),
           &hkSubmitGlobalChatMessage,
           (void**)&oSubmitGlobalChatMessage,
           "OutgoingMessages::SubmitGlobalChatMessage");
    DoHook(GetMethodPtr("OutgoingMessages", "SubmitPrivateChatMessage"),
           &hkSubmitPrivateChatMessage,
           (void**)&oSubmitPrivateChatMessage,
           "OutgoingMessages::SubmitPrivateChatMessage");
    DoHook(GetMethodPtr("OutgoingMessages", "SubmitPetCommandChatMessage"),
           &hkSubmitPetCommandChatMessage,
           (void**)&oSubmitPetCommandChatMessage,
           "OutgoingMessages::SubmitPetCommandChatMessage");
    DoFuncPtr(GetMethodPtr("OutgoingMessages", "SendRespawn"),
              oSendRespawnCmd,
              "OutgoingMessages::SendRespawn");

    DoFuncPtr(GetMethodPtr("ConfigData", "GetGemstoneRecycleValueForMiningGemstoneRecycler"),
              oCfgGetGemstoneRecycleValue,
              "ConfigData::GetGemstoneRecycleValueForMiningGemstoneRecycler");
    DoFuncPtr(GetMethodPtr("ConfigData", "GetFishRecycleValueForFishRecycler"),
              oCfgGetFishRecycleValue,
              "ConfigData::GetFishRecycleValueForFishRecycler");
    DoFuncPtr(GetMethodPtr("ConfigData", "GetTreasurePouchRewardAmount"),
              oCfgGetTreasurePouchReward,
              "ConfigData::GetTreasurePouchRewardAmount");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsConsumableTreasurePouch"),
              oCfgIsConsumableTreasurePouch,
              "ConfigData::IsConsumableTreasurePouch");
    DoFuncPtr(GetMethodPtr("PlayerData", "GetInventoryAsOrderedByInventoryItemType"),
              oPD_GetOrderedInventory,
              "PlayerData::GetInventoryAsOrderedByInventoryItemType");
    DoFuncPtr(GetMethodPtr("PlayerData", "GetCount"),
              oPD_GetCount,
              "PlayerData::GetCount");

    DoFuncPtr(GetMethodPtr("OutgoingMessages", "SendDropItemMessage"),
              oSendDropItem,
              "OutgoingMessages::SendDropItemMessage");

    DoFuncPtr(GetMethodPtr("WorldController", "SetBlock"),
              oWCSetBlock,
              "WorldController::SetBlock");
    DoFuncPtr(GetMethodPtr("World", "SetBlock"),
              oWorldSetBlock,
              "World::SetBlock");
}

inline void RenderPopups() {
    bool showGems, showInfo, showMath, showErr;
    std::string gemsText, infoTitle, infoBody, mathExpr, mathResult, errText;
    {
        std::lock_guard<std::mutex> lk(g_popupMutex);
        showGems = g_showGemsPopup; gemsText = g_gemsResultText;
        showInfo = g_showInfoPopup; infoTitle = g_infoTitle; infoBody = g_infoBody;
        showMath = g_showMathPopup; mathExpr = g_mathExpr; mathResult = g_mathResult;
        showErr  = g_showError;     errText  = g_errorText;
    }

    using ZmodUI::Theme::Resolve;
    namespace TT = ZmodUI::Theme;
    ImU32 cBg      = Resolve(TT::BgCard);
    ImU32 cBgHover = Resolve(TT::BgHover);
    ImU32 cLine    = Resolve(TT::Line);
    ImU32 cText    = Resolve(TT::Text);
    ImU32 cTextSec = Resolve(TT::TextSec);
    ImU32 cAccent  = Resolve(TT::AccentTok);
    ImVec4 vBg      = ImGui::ColorConvertU32ToFloat4(cBg);
    ImVec4 vBgHover = ImGui::ColorConvertU32ToFloat4(cBgHover);
    ImVec4 vLine    = ImGui::ColorConvertU32ToFloat4(cLine);
    ImVec4 vText    = ImGui::ColorConvertU32ToFloat4(cText);
    ImVec4 vTextSec = ImGui::ColorConvertU32ToFloat4(cTextSec);
    ImVec4 vAccent  = ImGui::ColorConvertU32ToFloat4(cAccent);

    auto clamp01 = [](float v) -> float {
        return v < 0.f ? 0.f : (v > 1.f ? 1.f : v);
    };
    ImVec4 vAccentHover(clamp01(vAccent.x + 0.07f),
                       clamp01(vAccent.y + 0.07f),
                       clamp01(vAccent.z + 0.07f), 1.0f);
    ImVec4 vAccentActive(clamp01(vAccent.x - 0.06f),
                        clamp01(vAccent.y - 0.06f),
                        clamp01(vAccent.z - 0.06f), 1.0f);
    ImVec4 vAccentFill(vAccent.x, vAccent.y, vAccent.z, 1.0f);

    auto renderModal = [&](const char* id, const char* title, const std::string& body,
                           bool* visible) {
        if (!*visible) return;
        if (!ImGui::IsPopupOpen(id)) ImGui::OpenPopup(id);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 18.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 8.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 10.0f));

        ImGui::PushStyleColor(ImGuiCol_WindowBg,     vBg);
        ImGui::PushStyleColor(ImGuiCol_PopupBg,      vBg);
        ImGui::PushStyleColor(ImGuiCol_Border,       vLine);
        ImGui::PushStyleColor(ImGuiCol_Text,         vText);
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, vTextSec);
        ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0,0,0,0.45f));

        ImGui::PushStyleColor(ImGuiCol_Button,        vAccentFill);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, vAccentHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  vAccentActive);

        ImGui::SetNextWindowSize(ImVec2(460, 0), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal(id, nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoSavedSettings |
                                   ImGuiWindowFlags_NoTitleBar)) {

            {
                ImVec2 a = ImGui::GetWindowPos();
                ImVec2 sz = ImGui::GetWindowSize();
                ImVec2 b{a.x + sz.x, a.y + sz.y};
                ImDrawList* bg = ImGui::GetBackgroundDrawList();
                for (int i = 1; i <= 6; ++i) {
                    float spread = (float)i * 2.4f;
                    int   alpha  = (int)(36.f * (1.f - (float)i / 7.0f));
                    bg->AddRectFilled(ImVec2(a.x - spread, a.y - spread*0.5f + 3.f),
                                      ImVec2(b.x + spread, b.y + spread*1.3f + 3.f),
                                      IM_COL32(0,0,0,alpha), 14.0f + spread);
                }
            }

            ImGui::PushStyleColor(ImGuiCol_Text, vText);
            ImGui::SetWindowFontScale(1.18f);
            ImGui::TextUnformatted(title);
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            {
                ImVec2 p = ImGui::GetCursorScreenPos();
                float w = ImGui::GetContentRegionAvail().x;
                ImGui::GetWindowDrawList()->AddLine(
                    ImVec2(p.x, p.y + 6.f),
                    ImVec2(p.x + w, p.y + 6.f),
                    cLine, 1.0f);
                ImGui::Dummy(ImVec2(0.f, 14.0f));
            }

            ImGui::PushStyleColor(ImGuiCol_Text, vTextSec);
            ImGui::TextUnformatted(body.c_str());
            ImGui::PopStyleColor();

            ImGui::Dummy(ImVec2(0.f, 6.0f));

            {
                ImVec2 p = ImGui::GetCursorScreenPos();
                float w = ImGui::GetContentRegionAvail().x;
                ImGui::GetWindowDrawList()->AddLine(
                    ImVec2(p.x, p.y),
                    ImVec2(p.x + w, p.y),
                    cLine, 1.0f);
                ImGui::Dummy(ImVec2(0.f, 8.0f));
            }

            const float btnW = 120.f;
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - btnW - 20.f);
            if (ImGui::Button("Close", ImVec2(btnW, 0))) {
                ImGui::CloseCurrentPopup();
                *visible = false;
            }
            ImGui::EndPopup();
        }

        ImGui::PopStyleColor(9);
        ImGui::PopStyleVar(6);
    };

    if (showGems) renderModal("/gems##cmd", "Inventory gem worth", gemsText, &g_showGemsPopup);
    if (showInfo) renderModal("/info##cmd", infoTitle.c_str(), infoBody, &g_showInfoPopup);
    if (showMath) renderModal("/math##cmd", ("= " + mathResult).c_str(),
                              mathExpr + "\n= " + mathResult, &g_showMathPopup);
    if (showErr)  renderModal("/cmd-err##cmd", "Command error", errText, &g_showError);

    std::string announce;
    {
        std::lock_guard<std::mutex> lk(g_stateMutex);
        announce = g_announceText;
    }
    if (!announce.empty()) {
        ImVec2 disp = ImGui::GetIO().DisplaySize;
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const char* s = announce.c_str();

        ImVec2 sz = ImGui::CalcTextSize(s);
        const float k = 2.2f;
        ImVec2 sized(sz.x * k, sz.y * k);
        ImVec2 anchor(disp.x * 0.5f - sized.x * 0.5f, disp.y * 0.20f);

        for (int dx = -2; dx <= 2; ++dx) {
            for (int dy = -2; dy <= 2; ++dy) {
                if (dx == 0 && dy == 0) continue;
                dl->AddText(ImGui::GetFont(),
                            ImGui::GetFontSize() * k,
                            ImVec2(anchor.x + dx, anchor.y + dy),
                            IM_COL32(0, 0, 0, 220), s);
            }
        }
        dl->AddText(ImGui::GetFont(),
                    ImGui::GetFontSize() * k,
                    anchor,
                    IM_COL32(255, 220, 80, 255), s);
    }
}

inline int GetActiveScanBlockId() {
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lk(g_stateMutex);
    if (g_scanBlockId < 0)         return -1;
    if (now > g_scanUntil)         return -1;
    return g_scanBlockId;
}

}

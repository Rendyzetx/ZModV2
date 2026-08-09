#pragma once

#include <Windows.h>
#include <vector>
#include <map>
#include <string>
#include "imgui/imgui.h"
#include "PathFinding.h"

#include "ui/Theme.h"

struct Vector2i_Minimap { int x, y; };
struct Vector3_Minimap { float x, y, z; };

extern uintptr_t gameAssemblyBase;
extern std::vector<void*> g_PlayerInstances;
extern void* g_LocalPlayer;
extern void* g_WorldController;

struct ID3D11Device;
extern ID3D11Device* d3d_device;

extern uintptr_t g_off_WCWorld;
extern uintptr_t g_off_WorldBlockLayer;
extern uintptr_t g_off_WorldSize;

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }

inline std::map<int, ImVec4> GetBlockColors_Minimap() {
    static std::map<int, ImVec4> blockColors = {
 
        {1,   ImVec4(128 / 255.0f, 90 / 255.0f, 51 / 255.0f, 1.0f)},
        {2,   ImVec4(89 / 255.0f, 89 / 255.0f, 89 / 255.0f, 1.0f)},
        {3,   ImVec4(54 / 255.0f, 54 / 255.0f, 54 / 255.0f, 1.0f)},
        {4,   ImVec4(153 / 255.0f, 153 / 255.0f, 153 / 255.0f, 1.0f)},
        {5,   ImVec4(242 / 255.0f, 222 / 255.0f, 166 / 255.0f, 1.0f)},
        {7,   ImVec4(255 / 255.0f, 51 / 255.0f, 0 / 255.0f, 1.0f)},
        {10,  ImVec4(0 / 255.0f, 179 / 255.0f, 25 / 255.0f, 1.0f)},
        {11,  ImVec4(255 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f)},
        {14,  ImVec4(102 / 255.0f, 102 / 255.0f, 115 / 255.0f, 1.0f)},
        {15,  ImVec4(166 / 255.0f, 115 / 255.0f, 64 / 255.0f, 1.0f)},
        {16,  ImVec4(153 / 255.0f, 204 / 255.0f, 250 / 255.0f, 1.0f)},
        {17,  ImVec4(26 / 255.0f, 102 / 255.0f, 204 / 255.0f, 1.0f)},
        {18,  ImVec4(128 / 255.0f, 128 / 255.0f, 128 / 255.0f, 1.0f)},
        {19,  ImVec4(179 / 255.0f, 51 / 255.0f, 51 / 255.0f, 1.0f)},
        {20,  ImVec4(204 / 255.0f, 204 / 255.0f, 51 / 255.0f, 1.0f)},
        {22,  ImVec4(102 / 255.0f, 64 / 255.0f, 26 / 255.0f, 1.0f)},
        {23,  ImVec4(64 / 255.0f, 38 / 255.0f, 13 / 255.0f, 1.0f)},
        {101, ImVec4(230 / 255.0f, 26 / 255.0f, 26 / 255.0f, 1.0f)},
        {102, ImVec4(51 / 255.0f, 204 / 255.0f, 255 / 255.0f, 1.0f)},
        {3966, ImVec4(255 / 255.0f, 60 / 255.0f, 60 / 255.0f, 1.0f)},
        {3965, ImVec4(140 / 255.0f, 220 / 255.0f, 255 / 255.0f, 1.0f)},
        {4103, ImVec4(140 / 255.0f, 255 / 255.0f, 140 / 255.0f, 1.0f)},
        {134, ImVec4(255 / 255.0f, 230 / 255.0f, 51 / 255.0f, 1.0f)},
        {142, ImVec4(255 / 255.0f, 212 / 255.0f, 0 / 255.0f, 1.0f) },
        {238, ImVec4(255 / 255.0f, 212 / 255.0f, 0 / 255.0f, 1.0f)},
        {468, ImVec4(51 / 255.0f, 51 / 255.0f, 51 / 255.0f, 1.0f)},
        {1133, ImVec4(175 / 255.0f, 230 / 255.0f, 255 / 255.0f, 1.0f)},
        {4002, ImVec4(0 / 255.0f, 122 / 255.0f, 255 / 255.0f, 1.0f)},
        
    };
    return blockColors;
}

namespace Minimap
{

    inline bool showMinimap = false;
    inline float minimapZoomAmount = 8.0f;

    inline bool g_blockTooltip = false;

    inline float g_tooltipFontScale = 0.85f;
    inline ImVec4 localPlayerColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    inline ImVec4 otherPlayerColor = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
    inline ImVec4 defaultBlockColor = ImVec4(0.5f, 0.0f, 0.5f, 1.0f);

    struct TileSnapshot {
        int  bgID                = 0;
        void* worldItem          = nullptr;
        const char* className    = nullptr;

        bool base_valid          = false;
        int  itemId              = 0;
        int  blockType           = 0;
        int  direction           = 0;
        bool animOn              = false;
        bool altSprite           = false;
        bool damageNow           = false;

        bool has_portal          = false;
        bool portal_isLocked     = false;
        char portal_name[96]               = {0};
        char portal_entryPointID[96]       = {0};
        char portal_password[96]           = {0};
        char portal_targetWorldID[96]      = {0};
        char portal_targetEntryPointID[96] = {0};

        bool has_door            = false;
        bool door_isLocked       = false;

        bool has_sign            = false;
        char sign_text[256]      = {0};

        bool has_lock                   = false;
        char lock_ownerId[64]           = {0};
        char lock_ownerName[64]         = {0};
        int  lock_accessCount           = 0;
        int  lock_minorAccessCount      = 0;
        bool lock_isOpen                = false;
        bool lock_punchingAllowed       = false;
        bool lock_isBattleOn            = false;

        char lock_creationTime[40]      = {0};
        char lock_lastActivatedTime[40] = {0};
    };

    static inline int Seh_ReadBgID(uintptr_t worldObject, int x, int y) {
        __try {
            uintptr_t bgLayer = *(uintptr_t*)(worldObject + 0x38);
            if (!bgLayer) return 0;
            void** cols = (void**)(bgLayer + 0x20);
            if (!cols || !cols[x]) return 0;
            char* row = (char*)((uintptr_t)cols[x] + 0x20);
            return *(int*)(row + y * 0x20);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    }

    static inline int Seh_ReadFgID(uintptr_t worldObject, int x, int y) {
        __try {
            uintptr_t fgLayer = *(uintptr_t*)(worldObject + 0x40);
            if (!fgLayer) return 0;
            void** cols = (void**)(fgLayer + 0x20);
            if (!cols || !cols[x]) return 0;
            char* row = (char*)((uintptr_t)cols[x] + 0x20);
            return *(int*)(row + y * 0x28);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    }

    static inline void* Seh_ReadWorldItemPtr(uintptr_t worldObject, int x, int y) {
        __try {
            uintptr_t itLayer = *(uintptr_t*)(worldObject + 0x60);
            if (!itLayer) return nullptr;
            void** cols = (void**)(itLayer + 0x20);
            if (!cols || !cols[x]) return nullptr;
            void** arr = (void**)((uintptr_t)cols[x] + 0x20);
            return arr[y];
        } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    }

    static inline const char* Seh_GetClassNameOf(void* obj) {
        __try {
            if (!obj) return nullptr;
            void* klass = *(void**)obj;
            if (!klass || !il2cpp_class_get_name) return nullptr;
            return il2cpp_class_get_name(klass);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    }

    static inline void Seh_ReadIl2cppStrField(uintptr_t addr, char* out, int outSz) {
        if (outSz > 0) out[0] = '\0';
        if (outSz < 2) return;
        __try {
            void* sp = *(void**)addr;
            if (!sp) return;
            int len = *(int*)((uintptr_t)sp + 0x10);
            if (len <= 0) return;
            int max = outSz - 1;
            if (len > max) len = max;
            const wchar_t* w = (const wchar_t*)((uintptr_t)sp + 0x14);
            for (int i = 0; i < len; ++i) {
                wchar_t c = w[i];
                out[i] = (c >= 32 && c < 127) ? (char)c : '?';
            }
            out[len] = '\0';
        } __except (EXCEPTION_EXECUTE_HANDLER) { if (outSz > 0) out[0] = '\0'; }
    }

    static inline int Seh_ProbeDoorWorldAccess(void* world, int tx, int ty, void* pd) {
        if (!::Door::oWorldRightAccess || !world || !pd) return -1;
        __try {
            return ::Door::oWorldRightAccess(world, { tx, ty }, pd, nullptr) ? 1 : 0;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    }
    static inline int Seh_ProbeDoorWcGoDoor(void* wc, int tx, int ty, void* pd) {
        if (!::Door::oWcRightGo || !wc || !pd) return -1;
        __try {
            return ::Door::oWcRightGo(wc, { tx, ty }, pd, nullptr) ? 1 : 0;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    }
    static inline int Seh_ProbeDoorWcRight(void* wc, int tx, int ty) {
        if (!::Door::oWcRightCollider || !wc) return -1;
        __try {
            return ::Door::oWcRightCollider(wc, { tx, ty }, nullptr) ? 1 : 0;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    }

    static inline void Seh_FillItemSnapshot(TileSnapshot& s) {
        if (!s.worldItem) return;
        __try {
            uintptr_t p = (uintptr_t)s.worldItem;
            s.itemId    = *(int*)(p + 0x10);
            s.blockType = *(int*)(p + 0x14);
            s.direction = *(int*)(p + 0x18);
            s.animOn    = *(bool*)(p + 0x1C);
            s.altSprite = *(bool*)(p + 0x1D);
            s.damageNow = *(bool*)(p + 0x1E);
            s.base_valid = true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { s.base_valid = false; }
    }

    static inline void Seh_FillPortal(TileSnapshot& s) {
        if (!s.worldItem) return;
        __try {
            uintptr_t p = (uintptr_t)s.worldItem;
            s.portal_isLocked = *(bool*)(p + 0x40);
            s.has_portal = true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { s.has_portal = false; }
        if (!s.has_portal) return;
        uintptr_t p = (uintptr_t)s.worldItem;
        Seh_ReadIl2cppStrField(p + 0x20, s.portal_targetWorldID,      sizeof(s.portal_targetWorldID));
        Seh_ReadIl2cppStrField(p + 0x28, s.portal_targetEntryPointID, sizeof(s.portal_targetEntryPointID));
        Seh_ReadIl2cppStrField(p + 0x30, s.portal_name,               sizeof(s.portal_name));
        Seh_ReadIl2cppStrField(p + 0x38, s.portal_entryPointID,       sizeof(s.portal_entryPointID));
        Seh_ReadIl2cppStrField(p + 0x48, s.portal_password,           sizeof(s.portal_password));
    }

    static inline void Seh_FillDoor(TileSnapshot& s) {
        if (!s.worldItem) return;
        __try {
            s.door_isLocked = *(bool*)((uintptr_t)s.worldItem + 0x20);
            s.has_door = true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { s.has_door = false; }
    }

    static inline void Seh_FillSign(TileSnapshot& s) {
        if (!s.worldItem) return;
        s.has_sign = true;
        Seh_ReadIl2cppStrField((uintptr_t)s.worldItem + 0x20, s.sign_text, sizeof(s.sign_text));
    }

    static inline void Seh_FormatDateTime(uint64_t raw, char* out, size_t outSz) {
        if (outSz < 2) return;
        out[0] = '\0';
        __try {
            const uint64_t kindMask  = 0xC000000000000000ULL;
            const uint64_t ticks     = raw & ~kindMask;
            const uint64_t ticksUnix = 621355968000000000ULL;
            if (ticks < ticksUnix) {
                std::snprintf(out, outSz, "(pre-1970)");
                return;
            }
            uint64_t unixTicks   = ticks - ticksUnix;
            uint64_t unixMillis  = unixTicks / 10000;
            time_t   secs        = (time_t)(unixMillis / 1000);
            int      ms          = (int)(unixMillis % 1000);
            tm       tm_buf      = {};
            if (gmtime_s(&tm_buf, &secs) != 0) {
                std::snprintf(out, outSz, "(invalid)");
                return;
            }
            std::snprintf(out, outSz,
                "%04d-%02d-%02d %02d:%02d:%02d.%03d UTC",
                tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
                tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec, ms);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            if (outSz > 0) std::snprintf(out, outSz, "(read fault)");
        }
    }

    static inline int Seh_ReadListCount(uintptr_t listPtrField) {
        __try {
            void* list = *(void**)listPtrField;
            if (!list) return 0;
            return *(int*)((uintptr_t)list + 0x18);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    }

    static inline void Seh_FillLock(TileSnapshot& s) {
        if (!s.worldItem) return;
        uintptr_t p = (uintptr_t)s.worldItem;

        bool gotScalars = false;
        uint64_t tCreated = 0, tActivated = 0;
        __try {
            s.lock_isOpen          = *(bool*)(p + 0x40);
            s.lock_punchingAllowed = *(bool*)(p + 0x41);
            tCreated               = *(uint64_t*)(p + 0x48);
            tActivated             = *(uint64_t*)(p + 0x50);
            s.lock_isBattleOn      = *(bool*)(p + 0x58);
            s.lock_accessCount      = Seh_ReadListCount(p + 0x30);
            s.lock_minorAccessCount = Seh_ReadListCount(p + 0x38);
            gotScalars = true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
        if (!gotScalars) return;
        s.has_lock = true;

        Seh_ReadIl2cppStrField(p + 0x20, s.lock_ownerId,   sizeof(s.lock_ownerId));
        Seh_ReadIl2cppStrField(p + 0x28, s.lock_ownerName, sizeof(s.lock_ownerName));

        Seh_FormatDateTime(tCreated,   s.lock_creationTime,      sizeof(s.lock_creationTime));
        Seh_FormatDateTime(tActivated, s.lock_lastActivatedTime, sizeof(s.lock_lastActivatedTime));
    }

    enum class WxKind { None, Rain, Snow, Sand, Ember, Aurora, Hearts, Pixel };
    struct WxStyle { ImU32 bg; ImU32 fg; WxKind kind; int density; };

    inline WxStyle GetWxStyle(World::WeatherType w) {

        switch (w) {
            case World::WeatherType::HeavyRain:
                return { IM_COL32( 25,  35,  55, 255), IM_COL32(120, 170, 255, 220), WxKind::Rain,   140 };
            case World::WeatherType::LightRain:
                return { IM_COL32( 45,  60,  85, 255), IM_COL32(140, 190, 255, 200), WxKind::Rain,    60 };
            case World::WeatherType::SnowStorm:
                return { IM_COL32( 70,  90, 125, 255), IM_COL32(240, 245, 255, 230), WxKind::Snow,   120 };
            case World::WeatherType::LightSnow:
                return { IM_COL32(110, 130, 160, 255), IM_COL32(245, 250, 255, 230), WxKind::Snow,    50 };
            case World::WeatherType::SandStorm:
                return { IM_COL32(120, 100,  60, 255), IM_COL32(220, 195, 130, 220), WxKind::Sand,   110 };
            case World::WeatherType::DeepNether:
                return { IM_COL32( 40,   5,   5, 255), IM_COL32(255, 120,  60, 230), WxKind::Ember,   70 };
            case World::WeatherType::Armageddon:
                return { IM_COL32( 70,  10,  10, 255), IM_COL32(255,  80,  40, 230), WxKind::Ember,  120 };
            case World::WeatherType::Halloween:
            case World::WeatherType::HalloweenTower:
                return { IM_COL32( 55,  25,   5, 255), IM_COL32(255, 140,  40, 220), WxKind::Ember,   60 };
            case World::WeatherType::Mining:
                return { IM_COL32( 45,  35,  20, 255), IM_COL32(255, 200, 100, 200), WxKind::Ember,   40 };
            case World::WeatherType::AuroraBorealis:
                return { IM_COL32( 12,  25,  35, 255), IM_COL32( 80, 230, 160, 200), WxKind::Aurora,   0 };
            case World::WeatherType::Hearts:
                return { IM_COL32( 90,  30,  60, 255), IM_COL32(255, 130, 180, 230), WxKind::Hearts,  40 };
            case World::WeatherType::PixelTrail:
                return { IM_COL32( 30,  35,  55, 255), IM_COL32(255, 255, 255, 220), WxKind::Pixel,   80 };
            case World::WeatherType::WeatherNone:
            default:

                return { IM_COL32( 35,  48,  72, 255), IM_COL32(255, 255, 255,   0), WxKind::None,    0 };
        }
    }

    struct WxParticle { float x, y, vx, vy; };
    inline WxParticle g_wxPool[256] = {};
    inline bool       g_wxInit      = false;

    inline void DrawWeatherBackdrop(ImDrawList* dl, ImVec2 a, ImVec2 b,
                                    float dtSec, World::WeatherType w) {
        WxStyle s = GetWxStyle(w);

        dl->AddRectFilled(a, b, s.bg, 8.0f);

        if (s.kind == WxKind::None || s.density <= 0) return;

        const int N = (s.density < 256) ? s.density : 256;
        const float width  = b.x - a.x;
        const float height = b.y - a.y;
        if (width < 1.f || height < 1.f) return;

        auto rfrac = []() -> float { return (rand() % 10000) / 10000.f; };
        if (!g_wxInit) {
            for (int i = 0; i < 256; ++i) {
                g_wxPool[i].x = rfrac() * width;
                g_wxPool[i].y = rfrac() * height;
            }
            g_wxInit = true;
        }

        dl->PushClipRect(a, b, true);

        switch (s.kind) {
            case WxKind::Rain: {
                const float speed = 220.f;
                for (int i = 0; i < N; ++i) {
                    auto& p = g_wxPool[i];
                    p.y += speed * dtSec;
                    if (p.y > height) { p.y = -4.f; p.x = rfrac() * width; }
                    if (p.x < 0 || p.x > width)
                        { p.x = rfrac() * width; p.y = rfrac() * height; }
                    dl->AddLine(ImVec2(a.x + p.x, a.y + p.y),
                                ImVec2(a.x + p.x - 1.f, a.y + p.y + 4.f),
                                s.fg, 1.0f);
                }
                break;
            }
            case WxKind::Snow: {
                const float speed = 38.f;
                for (int i = 0; i < N; ++i) {
                    auto& p = g_wxPool[i];
                    p.y += speed * dtSec;
                    p.x += sinf((p.y + i) * 0.05f) * 18.f * dtSec;
                    if (p.y > height) { p.y = -2.f; p.x = rfrac() * width; }
                    dl->AddCircleFilled(ImVec2(a.x + p.x, a.y + p.y), 1.4f, s.fg, 4);
                }
                break;
            }
            case WxKind::Sand: {
                const float speed = 170.f;
                for (int i = 0; i < N; ++i) {
                    auto& p = g_wxPool[i];
                    p.x += speed * dtSec;
                    p.y += sinf((p.x + i) * 0.04f) * 8.f * dtSec;
                    if (p.x > width) { p.x = -2.f; p.y = rfrac() * height; }
                    dl->AddLine(ImVec2(a.x + p.x, a.y + p.y),
                                ImVec2(a.x + p.x + 3.f, a.y + p.y),
                                s.fg, 1.0f);
                }
                break;
            }
            case WxKind::Ember: {
                const float speed = -52.f;
                for (int i = 0; i < N; ++i) {
                    auto& p = g_wxPool[i];
                    p.y += speed * dtSec;
                    p.x += sinf((p.y + i) * 0.06f) * 12.f * dtSec;
                    if (p.y < -2.f) { p.y = height + 2.f; p.x = rfrac() * width; }
                    dl->AddCircleFilled(ImVec2(a.x + p.x, a.y + p.y), 1.6f, s.fg, 6);
                }
                break;
            }
            case WxKind::Hearts: {
                const float speed = -30.f;
                for (int i = 0; i < N; ++i) {
                    auto& p = g_wxPool[i];
                    p.y += speed * dtSec;
                    if (p.y < -4.f) { p.y = height + 2.f; p.x = rfrac() * width; }

                    dl->AddCircleFilled(ImVec2(a.x + p.x - 1.5f, a.y + p.y), 1.6f, s.fg, 6);
                    dl->AddCircleFilled(ImVec2(a.x + p.x + 1.5f, a.y + p.y), 1.6f, s.fg, 6);
                }
                break;
            }
            case WxKind::Pixel: {

                const float speed = 90.f;
                for (int i = 0; i < N; ++i) {
                    auto& p = g_wxPool[i];
                    p.x += speed * dtSec;
                    if (p.x > width) { p.x = -2.f; p.y = rfrac() * height; }
                    float h = fmodf((p.x + i * 7.f) / width, 1.f);
                    float r = 0.5f + 0.5f * cosf(h * 6.283f);
                    float g = 0.5f + 0.5f * cosf(h * 6.283f + 2.094f);
                    float c = 0.5f + 0.5f * cosf(h * 6.283f + 4.188f);
                    ImU32 col = IM_COL32((int)(r*255), (int)(g*255), (int)(c*255), 220);
                    dl->AddRectFilled(ImVec2(a.x + p.x, a.y + p.y),
                                      ImVec2(a.x + p.x + 2.f, a.y + p.y + 2.f), col);
                }
                break;
            }
            case WxKind::Aurora: {

                float t = (float)(GetTickCount64() % 100000) / 1000.f;
                for (int band = 0; band < 2; ++band) {
                    float baseY = height * (0.18f + band * 0.10f);
                    for (int x = 0; x < (int)width; x += 3) {
                        float ph = (float)x * 0.045f + t * (band ? 1.2f : 1.5f);
                        float yy = baseY + sinf(ph) * 8.f + cosf(ph * 0.5f) * 6.f;
                        ImU32 col = IM_COL32(60 + band*30, 220 - band*40, 180 - band*20, 90);
                        dl->AddRectFilled(ImVec2(a.x + x, a.y + yy),
                                          ImVec2(a.x + x + 3, a.y + yy + 9), col);
                    }
                }
                break;
            }
            default: break;
        }

        dl->PopClipRect();
    }

    inline const char* BlockShortName(int id) {
        switch (id) {
            case 0:     return "(empty)";
            case 1:     return "Soil";
            case 2:     return "CaveWall";
            case 3:     return "Bedrock";
            case 4:     return "Granite";
            case 5:     return "Sand";
            case 7:     return "Lava";
            case 10:    return "Grass";
            case 11:    return "Rose";
            case 14:    return "MetalPlate";
            case 15:    return "WoodenPlatform";
            case 16:    return "Ice";
            case 17:    return "Water";
            case 18:    return "GreyBrick";
            case 19:    return "RedBrick";
            case 20:    return "YellowBrick";
            case 22:    return "WoodBlock";
            case 23:    return "WoodenBackground";
            case 101:   return "SpikeTrap";
            case 102:   return "EntrancePortal";
            case 109:   return "SpikeTrap";
            case 134:   return "SmallChest";
            case 142:   return "GoldBlock";
            case 238:   return "GoldBlock";
            case 329:   return "Portal";
            case 468:   return "CoalBlock";
            case 966:   return "GiftBox";
            case 1133:  return "MiningGemStoneDiamond";
            case 1392:  return "NetherGiftBox";
            case 1400:  return "NetherTreasure";
            case 1419:  return "NetherExit";
            case 1536:  return "ScifiBackground3";
            case 1663:  return "CrackedPottery";
            case 3965:  return "MineworldPortal";
            case 3966:  return "PortalMineExit";
            case 3973:  return "OrbLightingMining";
            case 3974:  return "MiningLightCrystalSmall";
            case 3975:  return "MiningLightCrystalMedium";
            case 3976:  return "MiningLightCrystalLarge";
            case 3977:  return "MiningTimeCrystalSmall";
            case 3978:  return "MiningTimeCrystalMedium";
            case 3979:  return "MiningTimeCrystalLarge";
            case 3985:  return "MiningNuggetBronze";
            case 3986:  return "MiningNuggetSilver";
            case 3987:  return "MiningNuggetGold";
            case 3988:  return "MiningNuggetPlatinum";
            case 3995:  return "MiningGemStoneDiamond";
            case 3996:  return "MiningGemStoneEmerald";
            case 3999:  return "MiningGemStoneRuby";
            case 4000:  return "MiningGemStoneSapphire";
            case 4002:  return "MiningGemStoneTopaz";
            case 4103:  return "PortalMiningEntry";
            case 4551:  return "DeathSpikeTrap";
            case 8014:  return "OrbNetherBackground";
            case 8040:  return "DeepNetherExit";
            case 15773: return "MiningGemStoneAmethyst";
            case 17565: return "LabGiftBox";
            default:    return "Block";
        }
    }

    inline void RenderTileTooltip(uintptr_t worldObject, int tileX, int tileY,
                                  int fgID, ImVec2 anchor,
                                  const char* windowId = "##BlockTooltip") {
        TileSnapshot snap;
        snap.bgID      = Seh_ReadBgID(worldObject, tileX, tileY);
        snap.worldItem = Seh_ReadWorldItemPtr(worldObject, tileX, tileY);
        snap.className = Seh_GetClassNameOf(snap.worldItem);
        Seh_FillItemSnapshot(snap);
        if (snap.className) {

            if (std::strstr(snap.className, "Portal") != nullptr) {
                Seh_FillPortal(snap);
            } else if (std::strstr(snap.className, "Lock") != nullptr) {

                Seh_FillLock(snap);
            } else if (std::strstr(snap.className, "Door") != nullptr) {
                Seh_FillDoor(snap);
            } else if (std::strstr(snap.className, "Sign") != nullptr) {
                Seh_FillSign(snap);
            }
        }

        const char* pfTag = "?";
        ImU32 pfCol = IM_COL32(200, 200, 200, 255);
        if (fgID == 0) {
            pfTag = "Air"; pfCol = IM_COL32(140, 200, 140, 255);
        } else if (PathRenderer::oCfgIsInstakill && PathRenderer::oCfgIsInstakill(fgID)) {
            pfTag = "Insta-kill"; pfCol = IM_COL32(255, 90, 90, 255);
        } else if (PathRenderer::PF_IsTilePassable_Tile(
                       fgID, tileX, tileY,  false,  false,
                       PathRenderer::g_walkInstakill))
        {

            pfTag = "Passable"; pfCol = IM_COL32(160, 200, 255, 255);
        } else {
            pfTag = "Solid"; pfCol = IM_COL32(220, 180, 90, 255);
        }

        if (snap.has_door && fgID > 0 && ::g_WorldController &&
            g_off_WCWorld && g_LocalPlayer)
        {
            void* world = *(void**)((uintptr_t)::g_WorldController + g_off_WCWorld);
            void* pd    = *(void**)((uintptr_t)g_LocalPlayer + 0x58);
            int r1 = Seh_ProbeDoorWorldAccess(world,                   tileX, tileY, pd);
            int r2 = Seh_ProbeDoorWcGoDoor(::g_WorldController,        tileX, tileY, pd);
            int r3 = Seh_ProbeDoorWcRight(::g_WorldController,         tileX, tileY);
            static int lastTileX = -99999, lastTileY = -99999;
            if (lastTileX != tileX || lastTileY != tileY) {
                lastTileX = tileX; lastTileY = tileY;
                std::cout << "[DoorRPC] tile=(" << tileX << "," << tileY
                          << ") bt=" << fgID
                          << " isLocked=" << (snap.door_isLocked ? "true" : "false")
                          << " WorldAccess="    << (r1 < 0 ? "n/a" : r1 ? "true" : "false")
                          << " WcGoDoor="       << (r2 < 0 ? "n/a" : r2 ? "true" : "false")
                          << " WcGoForColl="    << (r3 < 0 ? "n/a" : r3 ? "true" : "false")
                          << " world=" << world << " pd=" << pd << "\n";
            }
        }

        ImGui::SetNextWindowPos(anchor);
        ImGui::SetNextWindowBgAlpha(0.96f);

        ImGui::PushStyleColor(ImGuiCol_Text,         IM_COL32(235, 235, 235, 255));
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, IM_COL32(170, 170, 170, 255));
        ImGui::PushStyleColor(ImGuiCol_WindowBg,     IM_COL32( 12,  14,  18, 245));
        ImGui::PushStyleColor(ImGuiCol_Border,       IM_COL32( 60,  68,  80, 255));
        ImGui::PushStyleVar  (ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleVar  (ImGuiStyleVar_WindowRounding,   6.0f);
        ImGui::PushStyleVar  (ImGuiStyleVar_WindowPadding,    ImVec2(10.0f, 8.0f));

        ImGui::Begin(windowId, nullptr,
                     ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_Tooltip);

        ImGui::SetWindowFontScale(g_tooltipFontScale);

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                           "Tile: %d, %d", tileX, tileY);
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.30f, 1.0f),
                           "Foreground:"); ImGui::SameLine();
        ImGui::Text("%s [%d]", BlockShortName(fgID), fgID);
        ImGui::TextColored(ImVec4(0.55f, 0.78f, 0.95f, 1.0f),
                           "Background:"); ImGui::SameLine();
        ImGui::Text("%s [%d]", BlockShortName(snap.bgID), snap.bgID);
        ImGui::PushStyleColor(ImGuiCol_Text, pfCol);
        ImGui::Text("Path: %s", pfTag);
        ImGui::PopStyleColor();

        if (snap.className && snap.worldItem && snap.base_valid) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.70f, 1.0f), "ITEM DATA");

            const ImVec4 keyCol = ImVec4(0.55f, 0.85f, 0.55f, 1.0f);
            auto kv     = [&](const char* k, const char* v) {
                ImGui::TextColored(keyCol, "%s:", k); ImGui::SameLine();
                ImGui::Text("%s", v);
            };
            auto kvBool = [&](const char* k, bool b)    { kv(k, b ? "true" : "false"); };
            auto kvInt  = [&](const char* k, int v)     {
                char buf[24]; snprintf(buf, sizeof(buf), "%d", v); kv(k, buf);
            };
            auto kvStr  = [&](const char* k, const char* s) {
                kv(k, (s && *s) ? s : "(empty)");
            };

            kvBool("animOn",        snap.animOn);
            kvBool("anotherSprite", snap.altSprite);
            kvInt ("blockType",     snap.blockType);
            kv    ("class",         snap.className);
            kvBool("damageNow",     snap.damageNow);
            kvInt ("direction",     snap.direction);
            kvInt ("itemId",        snap.itemId);

            if (snap.has_portal) {
                kvStr ("entryPointID",       snap.portal_entryPointID);
                kvBool("isLocked",           snap.portal_isLocked);
                kvStr ("name",               snap.portal_name);
                kvStr ("password",           snap.portal_password);
                kvStr ("targetEntryPointID", snap.portal_targetEntryPointID);
                kvStr ("targetWorldID",      snap.portal_targetWorldID);
            } else if (snap.has_lock) {

                kvStr ("creationTime",     snap.lock_creationTime);
                kvBool("isBattleOn",       snap.lock_isBattleOn);
                kvBool("isOpen",           snap.lock_isOpen);
                kvStr ("lastActivatedTime", snap.lock_lastActivatedTime);
                kvStr ("playerWhoOwnsLockId",   snap.lock_ownerId);
                kvStr ("playerWhoOwnsLockName", snap.lock_ownerName);
                char accBuf[32];
                std::snprintf(accBuf, sizeof(accBuf), "[%d]", snap.lock_accessCount);
                kv    ("playersWhoHaveAccessToLock", accBuf);
                std::snprintf(accBuf, sizeof(accBuf), "[%d]", snap.lock_minorAccessCount);
                kv    ("playersWhoHaveMinorAccessToLock", accBuf);
                kvBool("punchingAllowed",  snap.lock_punchingAllowed);
            } else if (snap.has_door) {
                kvBool("isLocked", snap.door_isLocked);
            } else if (snap.has_sign) {
                kvStr("text", snap.sign_text);
            }
        }

        ImGui::End();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(4);
    }

    inline void RenderMinimap() {

        static bool s_loggedNoWC   = false;
        static bool s_loggedNoLP   = false;
        static bool s_loggedBadWO  = false;
        static bool s_loggedBadSz  = false;
        static bool s_loggedBadBA  = false;
        static bool s_loggedReady  = false;

        if (!showMinimap) return;

        if (g_isTransitioning.load(std::memory_order_acquire)) return;

        if (!g_WorldController) {
            if (!s_loggedNoWC) { std::cout << "[Minimap] g_WorldController null - waiting for world load." << std::endl; s_loggedNoWC = true; }
            return;
        }
        s_loggedNoWC = false;
        if (!g_LocalPlayer) {
            if (!s_loggedNoLP) { std::cout << "[Minimap] g_LocalPlayer null - waiting for player load." << std::endl; s_loggedNoLP = true; }
            return;
        }
        s_loggedNoLP = false;

        uintptr_t worldObject = *(uintptr_t*)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (!worldObject) {
            if (!s_loggedBadWO) {
                std::cout << "[Minimap] worldObject null at WC+0x"
                          << std::hex << g_off_WCWorld << std::dec
                          << " (waiting for world load)." << std::endl;
                s_loggedBadWO = true;
            }
            return;
        }
        s_loggedBadWO = false;

        Vector2i_Minimap worldSize = *(Vector2i_Minimap*)(worldObject + g_off_WorldSize);

        if (worldSize.x == 0 && worldSize.y == 0) return;
        if (worldSize.x <= 1 || worldSize.x > 1000 || worldSize.y <= 1 || worldSize.y > 1000) {
            if (!s_loggedBadSz) {
                std::cout << "[Minimap] bad worldSize (" << worldSize.x << "," << worldSize.y
                          << ") at world+0x" << std::hex << g_off_WorldSize << std::dec
                          << ". World struct offset may have drifted - run with "
                             "g_verboseInit=true to see the resolver output." << std::endl;
                s_loggedBadSz = true;
            }
            return;
        }
        s_loggedBadSz = false;

        uintptr_t pBlockArrayObject = *(uintptr_t*)(worldObject + g_off_WorldBlockLayer);
        if (!pBlockArrayObject) {
            if (!s_loggedBadBA) {
                std::cout << "[Minimap] worldBlockLayer null at world+0x"
                          << std::hex << g_off_WorldBlockLayer << std::dec << "." << std::endl;
                s_loggedBadBA = true;
            }
            return;
        }
        s_loggedBadBA = false;

        uintptr_t* pRows = (uintptr_t*)(pBlockArrayObject + 0x20);
        if (!pRows) return;

        if (!s_loggedReady) {
            std::cout << "[Minimap] Ready. World " << worldSize.x << "x" << worldSize.y
                      << " blockLayer=" << (void*)pBlockArrayObject << std::endl;
            s_loggedReady = true;
        }

        ImGui::SetNextWindowSize(ImVec2(480.f, 360.f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(200.f, 180.f),
                                            ImVec2(FLT_MAX, FLT_MAX));

        ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_FirstUseEver);

        using ZmodUI::Theme::Resolve;
        ImU32 bgU      = Resolve(ZmodUI::Theme::Bg);
        ImU32 cardU    = Resolve(ZmodUI::Theme::BgCard);
        ImU32 lineU    = Resolve(ZmodUI::Theme::Line);
        ImU32 textU    = Resolve(ZmodUI::Theme::Text);
        ImU32 textSecU = Resolve(ZmodUI::Theme::TextSec);
        ImVec4 bgV      = ImGui::ColorConvertU32ToFloat4(bgU);
        ImVec4 cardV    = ImGui::ColorConvertU32ToFloat4(cardU);
        ImVec4 lineV    = ImGui::ColorConvertU32ToFloat4(lineU);
        ImVec4 textV    = ImGui::ColorConvertU32ToFloat4(textU);
        ImVec4 textSecV = ImGui::ColorConvertU32ToFloat4(textSecU);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg,      bgV);
        ImGui::PushStyleColor(ImGuiCol_Border,        lineV);
        ImGui::PushStyleColor(ImGuiCol_TitleBg,       cardV);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, cardV);
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, cardV);
        ImGui::PushStyleColor(ImGuiCol_Text,          textV);
        ImGui::PushStyleColor(ImGuiCol_TextDisabled,  textSecV);
        ImGui::PushStyleColor(ImGuiCol_Button,        cardV);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(cardV.x+0.06f, cardV.y+0.06f, cardV.z+0.06f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(cardV.x+0.10f, cardV.y+0.10f, cardV.z+0.10f, 1.f));

        ImGui::Begin("ZMod Minimap", &showMinimap,
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoCollapse);

        {
            auto weatherName = [](World::WeatherType t) -> const char* {
                switch (t) {
                    case World::WeatherType::WeatherNone:     return "Clear";
                    case World::WeatherType::HeavyRain:       return "Heavy Rain";
                    case World::WeatherType::PixelTrail:      return "Pixel Trail";
                    case World::WeatherType::SandStorm:       return "Sand Storm";
                    case World::WeatherType::LightRain:       return "Light Rain";
                    case World::WeatherType::LightSnow:       return "Light Snow";
                    case World::WeatherType::SnowStorm:       return "Snow Storm";
                    case World::WeatherType::DeepNether:      return "Deep Nether";
                    case World::WeatherType::Halloween:       return "Halloween";
                    case World::WeatherType::HalloweenTower:  return "Halloween Tower";
                    case World::WeatherType::Hearts:          return "Hearts";
                    case World::WeatherType::Mining:          return "Mining";
                    case World::WeatherType::AuroraBorealis:  return "Aurora";
                    case World::WeatherType::Armageddon:      return "Armageddon";
                    default:                                  return "?";
                }
            };
            std::string wname = GetCachedWorldName();
            ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.30f, 1.f), "%s",
                               wname.empty() ? "(unknown world)" : wname.c_str());
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.65f, 0.65f, 0.70f, 1.f),
                               "  %dx%d", worldSize.x, worldSize.y);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.55f, 0.78f, 0.95f, 1.f),
                               "  · Weather: %s", weatherName(g_currentWeatherType));
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x < 16.f) avail.x = 16.f;
        if (avail.y < 16.f) avail.y = 16.f;
        avail.y -= 8.f;
        if (avail.y < 16.f) avail.y = 16.f;

        float pxPerTileW = avail.x / (float)worldSize.x;
        float pxPerTileH = avail.y / (float)worldSize.y;
        minimapZoomAmount = (pxPerTileW < pxPerTileH) ? pxPerTileW : pxPerTileH;
        if (minimapZoomAmount < 1.f) minimapZoomAmount = 1.f;

        ImVec2 mapSize((float)worldSize.x * minimapZoomAmount,
                       (float)worldSize.y * minimapZoomAmount);
        float xPad = (avail.x - mapSize.x) * 0.5f;
        if (xPad < 0.f) xPad = 0.f;
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        canvas_pos.x += xPad;
        auto blockColors = GetBlockColors_Minimap();

        {
            ImGuiIO& io = ImGui::GetIO();
            float dt = io.DeltaTime;
            if (dt <= 0.f || dt > 0.25f) dt = 1.f / 60.f;
            DrawWeatherBackdrop(drawList, canvas_pos, canvas_pos + mapSize,
                                dt, g_currentWeatherType);
        }
        drawList->AddRect(canvas_pos, canvas_pos + mapSize, lineU, 8.0f, 0, 1.f);

        Vector2i_Minimap hoveredMapPoint = { -1, -1 };
        int hoveredBlockID = -1;

        std::vector<Vector2i_Minimap> exitPortalTiles;

        uintptr_t pBgArray = *(uintptr_t*)(worldObject + 0x38);
        uintptr_t* pBgRows = pBgArray ? (uintptr_t*)(pBgArray + 0x20) : nullptr;

        SpriteAtlas::TryLoad(::d3d_device);

        for (int x = 0; x < worldSize.x; x++) {
            uintptr_t pRowObject = pRows[x];
            if (!pRowObject) continue;
            char* pRowData = (char*)(pRowObject + 0x20);
            char* pBgRowData = nullptr;
            if (pBgRows && pBgRows[x])
                pBgRowData = (char*)(pBgRows[x] + 0x20);

            for (int y = 0; y < worldSize.y; y++) {
                uintptr_t structAddress = (uintptr_t)(pRowData + (y * 0x28));
                int blockType = *(int*)structAddress;
                int bgID = 0;
                if (pBgRowData) bgID = *(int*)(pBgRowData + (y * 0x20));

                ImVec2 rect_min = canvas_pos + ImVec2(x * minimapZoomAmount, (worldSize.y - 1 - y) * minimapZoomAmount);
                ImVec2 rect_max = rect_min + ImVec2(minimapZoomAmount, minimapZoomAmount);

                if (bgID > 0) {
                    ImTextureID bgTex; ImVec2 bgUV0, bgUV1;
                    if (SpriteAtlas::LookupUV(bgID, bgUV0, bgUV1, bgTex)) {
                        drawList->AddImage(bgTex, rect_min, rect_max, bgUV0, bgUV1);
                    }
                }

                if (blockType > 0) {
                    ImTextureID fgTex; ImVec2 fgUV0, fgUV1;
                    if (SpriteAtlas::LookupUV(blockType, fgUV0, fgUV1, fgTex)) {
                        drawList->AddImage(fgTex, rect_min, rect_max, fgUV0, fgUV1);
                    } else {

                        bool passable = PathRenderer::PF_IsTilePassable_Tile(
                                            blockType, x, y,
                                             false,
                                             false,
                                            PathRenderer::g_walkInstakill);
                        if (passable) {
                            drawList->AddRectFilled(rect_min, rect_max,
                                                    IM_COL32(0, 0, 0, 255));
                        } else {
                            auto it = blockColors.find(blockType);
                            ImVec4 color = (it != blockColors.end()) ? it->second : defaultBlockColor;
                            drawList->AddRectFilled(rect_min, rect_max, ImGui::GetColorU32(color));
                        }
                    }

                    if (IsExitPortal((World::BlockType)blockType))
                        exitPortalTiles.push_back({ x, y });
                }

                if (ImGui::IsMouseHoveringRect(rect_min, rect_max)) {
                    hoveredMapPoint = { x, y };
                    hoveredBlockID = blockType;
                }
            }
        }

        for (const auto& ep : exitPortalTiles) {
            ImVec2 c = canvas_pos + ImVec2(
                ep.x * minimapZoomAmount + minimapZoomAmount * 0.5f,
                (worldSize.y - 1 - ep.y) * minimapZoomAmount + minimapZoomAmount * 0.5f);
            float r = minimapZoomAmount * 1.4f;
            drawList->AddCircle(c, r, IM_COL32(255, 60, 60, 255), 0, 2.0f);
            drawList->AddCircle(c, r + 1.5f, IM_COL32(0, 0, 0, 200), 0, 1.0f);
            ImVec2 ts = ImGui::CalcTextSize("EXIT");
            ImVec2 tp = ImVec2(c.x - ts.x * 0.5f, c.y + r + 1.0f);
            drawList->AddRectFilled(ImVec2(tp.x - 2, tp.y - 1),
                                    ImVec2(tp.x + ts.x + 2, tp.y + ts.y + 1),
                                    IM_COL32(0, 0, 0, 200));
            drawList->AddText(tp, IM_COL32(255, 80, 80, 255), "EXIT");
        }

        static double s_clickFlashAt = -10.0;
        static int    s_clickFlashTx = -1;
        static int    s_clickFlashTy = -1;
        static bool   s_clickFlashOk = false;
        bool overCanvas =
            ImGui::IsMouseHoveringRect(canvas_pos, canvas_pos + mapSize);
        if (overCanvas && hoveredMapPoint.x >= 0 && hoveredMapPoint.y >= 0) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                s_clickFlashOk = PathRenderer::NavigateToTile(
                    { hoveredMapPoint.x, hoveredMapPoint.y });
                s_clickFlashAt = ImGui::GetTime();
                s_clickFlashTx = hoveredMapPoint.x;
                s_clickFlashTy = hoveredMapPoint.y;
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {

                PathRenderer::g_isNavigating  = false;
                PathRenderer::g_isTeleporting = false;
                std::lock_guard<std::mutex> lk(PathRenderer::g_pathMutex);
                PathRenderer::g_pathTiles.clear();
                PathRenderer::g_pathOffsets.clear();
            }
        }

        {
            double now = ImGui::GetTime();
            double age = now - s_clickFlashAt;
            const double kFlashDur = 0.6;
            if (age >= 0.0 && age <= kFlashDur && s_clickFlashTx >= 0) {
                float t = 1.f - (float)(age / kFlashDur);
                int alpha = (int)(220.f * t);
                ImU32 col = s_clickFlashOk
                    ? IM_COL32( 80, 220, 120, alpha)
                    : IM_COL32(255,  90,  90, alpha);
                ImVec2 fmin = canvas_pos + ImVec2(
                    s_clickFlashTx * minimapZoomAmount,
                    (worldSize.y - 1 - s_clickFlashTy) * minimapZoomAmount);
                ImVec2 fmax = fmin + ImVec2(minimapZoomAmount, minimapZoomAmount);

                float grow = (1.f - t) * minimapZoomAmount * 1.2f;
                drawList->AddRect(
                    ImVec2(fmin.x - grow, fmin.y - grow),
                    ImVec2(fmax.x + grow, fmax.y + grow),
                    col, 2.f, 0, 2.0f);
                drawList->AddRect(fmin, fmax, col, 1.f, 0, 1.5f);
            }
        }

        if (hoveredMapPoint.x != -1) {
            RenderTileTooltip(worldObject,
                              hoveredMapPoint.x, hoveredMapPoint.y,
                              hoveredBlockID,
                              ImGui::GetMousePos() + ImVec2(15, 15));
        }

        {
            std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
            for (const auto& fc : g_floorCollectables) {
                if (!IsNugget(fc.blockType)) continue;
                if (fc.mapPoint.x < 0 || fc.mapPoint.x >= worldSize.x) continue;
                if (fc.mapPoint.y < 0 || fc.mapPoint.y >= worldSize.y) continue;
                ImVec2 rmin = canvas_pos + ImVec2(
                    fc.mapPoint.x * minimapZoomAmount,
                    (worldSize.y - 1 - fc.mapPoint.y) * minimapZoomAmount);
                ImVec2 rmax = rmin + ImVec2(minimapZoomAmount, minimapZoomAmount);
                drawList->AddRectFilled(rmin, rmax, IM_COL32(255, 220, 0, 255));
                drawList->AddRect      (rmin, rmax, IM_COL32(80, 60, 0, 255));
            }
        }

        MineScheduler::RenderOnMinimap(drawList, canvas_pos,
                                       worldSize.x, worldSize.y,
                                       minimapZoomAmount);

        for (void* playerInstance : g_PlayerInstances) {
            if (!playerInstance) continue;
            Vector2i_Minimap playerMapPoint = *(Vector2i_Minimap*)((uintptr_t)playerInstance + 0x678);
            float centerX = canvas_pos.x + (playerMapPoint.x * minimapZoomAmount) + (minimapZoomAmount / 2.0f);
            float centerY = canvas_pos.y + ((worldSize.y - 1 - playerMapPoint.y) * minimapZoomAmount) + (minimapZoomAmount / 2.0f);
            bool isLocal = (playerInstance == g_LocalPlayer);
            drawList->AddCircleFilled(ImVec2(centerX, centerY), isLocal ? 4.5f : 3.5f, ImGui::GetColorU32(isLocal ? localPlayerColor : otherPlayerColor));
            if (isLocal) drawList->AddCircle(ImVec2(centerX, centerY), 4.5f, IM_COL32_WHITE);
        }

        if (g_LocalPlayer && g_WorldController &&
            PathRenderer::oGetTransform && PathRenderer::oGetPosition) {
            void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
            if (lt) {
                Vector3 wp = PathRenderer::oGetPosition(lt);

                const float tileW = (PathRenderer::g_tileSize.x > 0.001f)
                                      ? PathRenderer::g_tileSize.x : 0.32f;
                float fx = wp.x / tileW;
                float fy = wp.y / tileW;
                if (fx >= 0.f && fx <= (float)worldSize.x &&
                    fy >= 0.f && fy <= (float)worldSize.y) {
                    float cx = canvas_pos.x + fx * minimapZoomAmount;

                    float cy = canvas_pos.y +
                               ((float)(worldSize.y - 1) - fy + 0.5f)
                                 * minimapZoomAmount;
                    drawList->AddCircleFilled(ImVec2(cx, cy), 5.0f,
                                              IM_COL32(40, 220, 255, 255));
                    drawList->AddCircle      (ImVec2(cx, cy), 5.0f,
                                              IM_COL32(255, 255, 255, 255), 0, 1.5f);
                    drawList->AddLine(ImVec2(cx - 7.f, cy), ImVec2(cx + 7.f, cy),
                                      IM_COL32(255, 255, 255, 200), 1.0f);
                    drawList->AddLine(ImVec2(cx, cy - 7.f), ImVec2(cx, cy + 7.f),
                                      IM_COL32(255, 255, 255, 200), 1.0f);
                }
            }
        }

        ImGui::Dummy(mapSize);

        ImGui::End();

        ImGui::PopStyleColor(10);
        ImGui::PopStyleVar(4);
    }
}
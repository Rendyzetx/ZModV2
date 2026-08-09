#include "Fonts.h"
#include <cstdio>
#include <cstdint>
#include <cstring>
#ifdef _WIN32
  #include <windows.h>
#endif
#ifdef ZMOD_FREETYPE
  #include "misc/freetype/imgui_freetype.h"
#endif

namespace ZmodUI {
namespace Fonts {

namespace {
    ImFont* g_fonts[F_Count] = {};
    bool    g_icon_loaded = false;

    static const char* kSearchPaths[] = {
        "ui/assets/",
        "../ui/assets/",
        "../../ui/assets/",
        "assets/",
        nullptr,
    };
    static char     g_asset_dir[1024] = "";
    static HMODULE  g_self_module     = nullptr;

    static bool FileExists(const char* path) {
        if (!path || !*path) return false;
        FILE* f = std::fopen(path, "rb");
        if (!f) return false;
        std::fclose(f);
        return true;
    }

    static void ResolveSelfModule() {
        if (g_self_module) return;
#ifdef _WIN32
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCSTR>(&ResolveSelfModule),
                           &g_self_module);
#endif
    }

    static void ResolveAssetDir() {
        if (g_asset_dir[0]) return;
#ifdef _WIN32
        ResolveSelfModule();
        char exe[MAX_PATH];
        DWORD n = GetModuleFileNameA(g_self_module, exe, MAX_PATH);
        if (n > 0 && n < MAX_PATH) {
            for (DWORD i = n; i > 0; --i) {
                if (exe[i-1] == '\\' || exe[i-1] == '/') { exe[i] = 0; break; }
            }
            for (int i = 0; kSearchPaths[i]; ++i) {
                char probe[1024];
                std::snprintf(probe, sizeof(probe), "%s%sInter-Regular.ttf", exe, kSearchPaths[i]);
                if (FileExists(probe)) {
                    std::snprintf(g_asset_dir, sizeof(g_asset_dir), "%s%s", exe, kSearchPaths[i]);
                    return;
                }
            }
        }
#endif
        for (int i = 0; kSearchPaths[i]; ++i) {
            char probe[1024];
            std::snprintf(probe, sizeof(probe), "%sInter-Regular.ttf", kSearchPaths[i]);
            if (FileExists(probe)) {
                std::snprintf(g_asset_dir, sizeof(g_asset_dir), "%s", kSearchPaths[i]);
                return;
            }
        }
    }

    static ImFontConfig MakeConfig(float size, ImFontConfig* extra) {
        ImFontConfig cfg;
        if (extra) cfg = *extra;
        cfg.SizePixels  = size;
#ifdef ZMOD_FREETYPE
        cfg.OversampleH = 1;
        cfg.OversampleV = 1;
        cfg.PixelSnapH  = false;
        cfg.RasterizerMultiply = 1.0f;
        cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting;
#else
        cfg.OversampleH = 3;
        cfg.OversampleV = 1;
        cfg.PixelSnapH  = false;
        cfg.RasterizerMultiply = 1.10f;
#endif
        return cfg;
    }

    static bool TryLoadResource(ImGuiIO& io, Slot slot, const char* res_name,
                                float size, const ImWchar* range = nullptr,
                                ImFontConfig* extra = nullptr) {
#ifdef _WIN32
        ResolveSelfModule();
        if (!g_self_module) return false;
        HRSRC hr = FindResourceA(g_self_module, res_name, RT_RCDATA);
        if (!hr) return false;
        DWORD sz = SizeofResource(g_self_module, hr);
        HGLOBAL hg = LoadResource(g_self_module, hr);
        if (!hg || sz == 0) return false;
        void* data = LockResource(hg);
        if (!data) return false;

        ImFontConfig cfg = MakeConfig(size, extra);

        cfg.FontDataOwnedByAtlas = false;
        ImFont* f = io.Fonts->AddFontFromMemoryTTF(data, (int)sz, size, &cfg, range);
        if (f) { g_fonts[slot] = f; return true; }
#endif
        (void)io; (void)slot; (void)res_name; (void)size; (void)range; (void)extra;
        return false;
    }

    static bool TryLoadFile(ImGuiIO& io, Slot slot, const char* file, float size,
                            const ImWchar* range = nullptr, ImFontConfig* extra = nullptr) {
        if (!g_asset_dir[0]) return false;
        char full[1280];
        std::snprintf(full, sizeof(full), "%s%s", g_asset_dir, file);
        ImFontConfig cfg = MakeConfig(size, extra);
        ImFont* f = io.Fonts->AddFontFromFileTTF(full, size, &cfg, range);
        if (f) { g_fonts[slot] = f; return true; }
        return false;
    }

    static bool TryLoad(ImGuiIO& io, Slot slot, const char* res_name,
                        const char* file, float size,
                        const ImWchar* range = nullptr, ImFontConfig* extra = nullptr) {
        if (TryLoadResource(io, slot, res_name, size, range, extra)) return true;
        return TryLoadFile(io, slot, file, size, range, extra);
    }
}

void Init() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->TexGlyphPadding = 1;
#ifdef ZMOD_FREETYPE
    io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
    io.Fonts->FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LightHinting;
#endif
    ResolveAssetDir();
    static const ImWchar latin[] = { 0x0020, 0x00FF, 0 };

    bool loaded_anything = false;
    loaded_anything |= TryLoad(io, F_H1,      "INTER_SEMIBOLD",  "Inter-SemiBold.ttf", 28.f, latin);
    loaded_anything |= TryLoad(io, F_H2,      "INTER_SEMIBOLD",  "Inter-SemiBold.ttf", 22.f, latin);
    loaded_anything |= TryLoad(io, F_H3,      "INTER_SEMIBOLD",  "Inter-SemiBold.ttf", 15.f, latin);
    loaded_anything |= TryLoad(io, F_Body,    "INTER_MEDIUM",    "Inter-Medium.ttf",   13.f, latin);
    loaded_anything |= TryLoad(io, F_BodySec, "INTER_REGULAR",   "Inter-Regular.ttf",  11.5f, latin);
    loaded_anything |= TryLoad(io, F_Mono,    "JETBRAINS_MONO",  "JetBrainsMono-Regular.ttf", 11.5f, latin);
    loaded_anything |= TryLoad(io, F_Eyebrow, "INTER_SEMIBOLD",  "Inter-SemiBold.ttf", 10.5f, latin);

    static const ImWchar icon_range[] = { 0xE000, 0xF8FF, 0 };
    if (TryLoad(io, F_Icon, "LUCIDE_ICONS", "lucide.ttf", 14.f, icon_range)) g_icon_loaded = true;

    ImFont* def = nullptr;
    if (!loaded_anything) def = io.Fonts->AddFontDefault();
    for (int i = 0; i < F_Count; ++i) {
        if (!g_fonts[i]) g_fonts[i] = def ? def : (io.Fonts->Fonts.Size > 0 ? io.Fonts->Fonts[0] : nullptr);
    }
    io.Fonts->Build();
}

void Shutdown() {
    for (int i = 0; i < F_Count; ++i) g_fonts[i] = nullptr;
    g_icon_loaded = false;
}

ImFont* Get(Slot s) {
    if (s < 0 || s >= F_Count) return nullptr;
    return g_fonts[s];
}

bool HasIconFont() { return g_icon_loaded; }

}
}

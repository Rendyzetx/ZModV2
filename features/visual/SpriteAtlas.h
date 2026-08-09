

#pragma once

#include <d3d11.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <chrono>

#include "imgui/imgui.h"
#include "vendor/json.hpp"
#include "XorStr.h"

namespace SpriteAtlas {

struct Tile { int px, py; };

inline std::unordered_map<int, Tile> g_tiles;
inline int                           g_cell    = 32;
inline int                           g_atlasW  = 0;
inline int                           g_atlasH  = 0;
inline ID3D11ShaderResourceView*     g_srv     = nullptr;
inline bool                          g_loaded  = false;

inline std::chrono::steady_clock::time_point g_lastTry{};
inline int                           g_failCount = 0;

inline HMODULE                       g_selfModule = nullptr;

inline void ResolveSelfModule() {
    if (g_selfModule) return;
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCSTR>(&ResolveSelfModule),
        &g_selfModule);
}

struct EmbeddedBlob { const void* data; DWORD size; };
inline EmbeddedBlob ReadEmbedded(const char* resName) {
    ResolveSelfModule();
    if (!g_selfModule) return { nullptr, 0 };
    HRSRC hr = FindResourceA(g_selfModule, resName, RT_RCDATA);
    if (!hr) return { nullptr, 0 };
    DWORD sz = SizeofResource(g_selfModule, hr);
    if (sz == 0) return { nullptr, 0 };
    HGLOBAL hg = LoadResource(g_selfModule, hr);
    if (!hg) return { nullptr, 0 };
    const void* p = LockResource(hg);
    if (!p) return { nullptr, 0 };
    return { p, sz };
}

inline bool DecodePNG_FromMemory(const void* bytes, size_t len,
                                  std::vector<uint8_t>& out,
                                  int& outW, int& outH) {
    if (!bytes || len < 8) return false;
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool didInit = SUCCEEDED(hrInit);

    Microsoft::WRL::ComPtr<IWICImagingFactory>     factory;
    Microsoft::WRL::ComPtr<IWICStream>             stream;
    Microsoft::WRL::ComPtr<IWICBitmapDecoder>      decoder;
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode>  frame;
    Microsoft::WRL::ComPtr<IWICFormatConverter>    converter;

    bool ok = false;
    do {
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)))) break;
        if (FAILED(factory->CreateStream(&stream))) break;

        if (FAILED(stream->InitializeFromMemory(
                reinterpret_cast<BYTE*>(const_cast<void*>(bytes)),
                (DWORD)len))) break;
        if (FAILED(factory->CreateDecoderFromStream(stream.Get(), nullptr,
                WICDecodeMetadataCacheOnLoad, &decoder))) break;
        if (FAILED(decoder->GetFrame(0, &frame))) break;
        UINT W = 0, H = 0;
        if (FAILED(frame->GetSize(&W, &H))) break;
        if (FAILED(factory->CreateFormatConverter(&converter))) break;
        if (FAILED(converter->Initialize(frame.Get(),
                GUID_WICPixelFormat32bppRGBA,
                WICBitmapDitherTypeNone, nullptr, 0.0,
                WICBitmapPaletteTypeMedianCut))) break;
        outW = (int)W;
        outH = (int)H;
        out.resize((size_t)W * H * 4);
        if (FAILED(converter->CopyPixels(nullptr, W * 4,
                (UINT)out.size(), out.data()))) break;
        ok = true;
    } while (false);

    if (didInit) CoUninitialize();
    return ok;
}

inline bool DecodePNG(const std::filesystem::path& path,
                      std::vector<uint8_t>& out, int& outW, int& outH) {
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool didInit = SUCCEEDED(hrInit);

    Microsoft::WRL::ComPtr<IWICImagingFactory>     factory;
    Microsoft::WRL::ComPtr<IWICBitmapDecoder>      decoder;
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode>  frame;
    Microsoft::WRL::ComPtr<IWICFormatConverter>    converter;

    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
    bool ok = false;
    do {
        if (FAILED(hr)) break;
        if (FAILED(factory->CreateDecoderFromFilename(
                path.wstring().c_str(), nullptr, GENERIC_READ,
                WICDecodeMetadataCacheOnLoad, &decoder))) break;
        if (FAILED(decoder->GetFrame(0, &frame))) break;
        UINT W = 0, H = 0;
        if (FAILED(frame->GetSize(&W, &H))) break;
        if (FAILED(factory->CreateFormatConverter(&converter))) break;
        if (FAILED(converter->Initialize(frame.Get(),
                GUID_WICPixelFormat32bppRGBA,
                WICBitmapDitherTypeNone, nullptr, 0.0,
                WICBitmapPaletteTypeMedianCut))) break;
        outW = (int)W;
        outH = (int)H;
        out.resize((size_t)W * H * 4);
        if (FAILED(converter->CopyPixels(nullptr, W * 4,
                (UINT)out.size(), out.data()))) break;
        ok = true;
    } while (false);

    if (didInit) CoUninitialize();
    return ok;
}

inline std::filesystem::path AssetsDir() {
    char buf[MAX_PATH] = {};
    HMODULE self = nullptr;
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&AssetsDir, &self);
    GetModuleFileNameA(self, buf, MAX_PATH);
    return std::filesystem::path(buf).parent_path() / "tiles";
}

inline bool ParseAtlasJson(const std::string& jsonText) {
    nlohmann::json j;
    try { j = nlohmann::json::parse(jsonText); }
    catch (const std::exception& e) {
        std::cout << XS("[SpriteAtlas] atlas.json parse error: ") << e.what() << "\n";
        return false;
    }
    g_cell = j.value("cell", 32);
    if (!j.contains("tiles") || !j["tiles"].is_object()) {
        std::cout << XS("[SpriteAtlas] atlas.json missing 'tiles' object\n");
        return false;
    }
    g_tiles.clear();
    for (auto it = j["tiles"].begin(); it != j["tiles"].end(); ++it) {
        int id = 0;
        try { id = std::stoi(it.key()); } catch (...) { continue; }
        const auto& xy = it.value();
        if (!xy.is_array() || xy.size() < 2) continue;
        g_tiles[id] = { xy[0].get<int>(), xy[1].get<int>() };
    }
    return true;
}

inline bool TryLoad(ID3D11Device* dev) {
    if (g_loaded) return true;
    if (!dev) return false;

    auto now = std::chrono::steady_clock::now();
    if (g_lastTry.time_since_epoch().count() != 0 &&
        std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastTry).count() < 2000) {
        return false;
    }
    g_lastTry = now;

    auto logFail = [&](const char* what) {

        if ((g_failCount % 5) == 0) {
            std::cout << XS("[SpriteAtlas] ") << what << "\n";
        }
        ++g_failCount;
    };

    std::string jsonText;
    bool jsonFromEmbedded = false;
    {
        EmbeddedBlob jblob = ReadEmbedded("ATLAS_JSON");
        if (jblob.data && jblob.size > 0) {
            jsonText.assign(reinterpret_cast<const char*>(jblob.data), jblob.size);
            jsonFromEmbedded = true;
        } else {
            const auto jsonPath = AssetsDir() / "atlas.json";
            std::ifstream f(jsonPath);
            if (!f) { logFail("atlas.json not embedded AND not found on disk"); return false; }
            std::ostringstream oss; oss << f.rdbuf();
            jsonText = oss.str();
        }
    }
    if (!ParseAtlasJson(jsonText)) {
        ++g_failCount;
        return false;
    }

    std::vector<uint8_t> pixels;
    bool pngFromEmbedded = false;
    {
        EmbeddedBlob pblob = ReadEmbedded("ATLAS_PNG");
        if (pblob.data && pblob.size > 0) {
            if (!DecodePNG_FromMemory(pblob.data, (size_t)pblob.size,
                                       pixels, g_atlasW, g_atlasH)) {
                logFail("ATLAS_PNG embedded resource present but WIC decode failed");
                g_tiles.clear();
                return false;
            }
            pngFromEmbedded = true;
        } else {
            const auto pngPath = AssetsDir() / "atlas.png";
            if (!DecodePNG(pngPath, pixels, g_atlasW, g_atlasH)) {
                logFail("atlas.png not embedded AND not decodable from disk");
                g_tiles.clear();
                return false;
            }
        }
    }

    D3D11_TEXTURE2D_DESC td{};
    td.Width            = (UINT)g_atlasW;
    td.Height           = (UINT)g_atlasH;
    td.MipLevels        = 1;
    td.ArraySize        = 1;
    td.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage            = D3D11_USAGE_DEFAULT;
    td.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem     = pixels.data();
    sd.SysMemPitch = (UINT)g_atlasW * 4;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    if (FAILED(dev->CreateTexture2D(&td, &sd, tex.GetAddressOf()))) {
        std::cout << XS("[SpriteAtlas] CreateTexture2D failed\n");
        g_tiles.clear();
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC sv{};
    sv.Format              = td.Format;
    sv.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
    sv.Texture2D.MipLevels = 1;
    if (FAILED(dev->CreateShaderResourceView(tex.Get(), &sv, &g_srv))) {
        std::cout << XS("[SpriteAtlas] CreateShaderResourceView failed\n");
        g_tiles.clear();
        return false;
    }

    g_loaded = true;
    std::cout << XS("[SpriteAtlas] Loaded ") << g_atlasW << "x" << g_atlasH
              << XS(" px atlas, ") << g_tiles.size() << XS(" tiles, cell=")
              << g_cell
              << XS(" (json=") << (jsonFromEmbedded ? "embedded" : "disk")
              << XS(", png=")  << (pngFromEmbedded  ? "embedded" : "disk")
              << ")\n";
    return true;
}

inline bool LookupUV(int blockId, ImVec2& uv0, ImVec2& uv1, ImTextureID& outTex) {
    if (!g_loaded || !g_srv) return false;
    auto it = g_tiles.find(blockId);
    if (it == g_tiles.end()) return false;
    const float aw = (float)g_atlasW;
    const float ah = (float)g_atlasH;
    uv0 = ImVec2(it->second.px / aw, it->second.py / ah);
    uv1 = ImVec2((it->second.px + g_cell) / aw,
                 (it->second.py + g_cell) / ah);
    outTex = (ImTextureID)(uintptr_t)g_srv;
    return true;
}

inline void Shutdown() {
    if (g_srv) { g_srv->Release(); g_srv = nullptr; }
    g_tiles.clear();
    g_loaded   = false;
    g_failCount = 0;
    g_lastTry  = {};
}

}

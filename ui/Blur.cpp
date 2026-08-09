

#include "Blur.h"
#include <d3dcompiler.h>
#include <iostream>
#include <cstring>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")

namespace ZmodUI {
namespace Blur {

namespace {

static const char* kShaderHLSL = R"HLSL(
Texture2D    g_src     : register(t0);
SamplerState g_samp    : register(s0);

cbuffer BlurCB : register(b0) {
    float2 g_texelSize;    // 1/textureW, 1/textureH
    float2 g_dir;          // (1,0) for horizontal, (0,1) for vertical
    float  g_radius;       // multiplier on texel step
    float3 _pad;
};

struct VS_OUT { float4 pos : SV_POSITION; float2 uv : TEXCOORD; };

VS_OUT VS_Fullscreen(uint vid : SV_VertexID) {
    VS_OUT o;
    // Generate a triangle that covers the screen.  vid 0,1,2 -> (-1,-1),(3,-1),(-1,3).
    float2 ndc;
    ndc.x = (vid == 1) ?  3.0 : -1.0;
    ndc.y = (vid == 2) ?  3.0 : -1.0;
    o.pos = float4(ndc, 0.0, 1.0);
    // UV is (ndc * 0.5 + 0.5), flipped y because D3D UV origin is top-left.
    o.uv  = float2((ndc.x + 1.0) * 0.5, 1.0 - (ndc.y + 1.0) * 0.5);
    return o;
}

float4 PS_Copy(VS_OUT i) : SV_TARGET {
    return g_src.Sample(g_samp, i.uv);
}

// 13-tap Gaussian weights (sigma ~ 3.0).  Sum = 1.
static const float W[7] = {
    0.196381, 0.176900, 0.129856, 0.077842, 0.038317, 0.015386, 0.005052
};

float4 PS_Blur(VS_OUT i) : SV_TARGET {
    float2 step = g_dir * g_texelSize * g_radius;
    float4 acc = g_src.Sample(g_samp, i.uv) * W[0];
    [unroll] for (int k = 1; k < 7; ++k) {
        acc += g_src.Sample(g_samp, i.uv + step * (float)k) * W[k];
        acc += g_src.Sample(g_samp, i.uv - step * (float)k) * W[k];
    }
    return acc;
}
)HLSL";

struct Globals {
    ID3D11Device*        dev = nullptr;
    ID3D11DeviceContext* ctx = nullptr;

    int bb_w = 0, bb_h = 0;
    int rt_w = 0, rt_h = 0;
    DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
    bool ready = false;
    bool captured = false;

    ID3D11VertexShader*  vs       = nullptr;
    ID3D11PixelShader*   ps_copy  = nullptr;
    ID3D11PixelShader*   ps_blur  = nullptr;
    ID3D11SamplerState*  samp     = nullptr;
    ID3D11Buffer*        cb       = nullptr;
    ID3D11RasterizerState* raster = nullptr;
    ID3D11BlendState*    blend_off = nullptr;
    ID3D11DepthStencilState* ds_off = nullptr;

    ID3D11Texture2D*          capture_tex = nullptr;
    ID3D11ShaderResourceView* capture_srv = nullptr;

    ID3D11Texture2D*          blur_a_tex = nullptr;
    ID3D11RenderTargetView*   blur_a_rtv = nullptr;
    ID3D11ShaderResourceView* blur_a_srv = nullptr;

    ID3D11Texture2D*          blur_b_tex = nullptr;
    ID3D11RenderTargetView*   blur_b_rtv = nullptr;
    ID3D11ShaderResourceView* blur_b_srv = nullptr;
};
static Globals g;

struct BlurCB {
    float texelSize[2];
    float dir[2];
    float radius;
    float pad[3];
};

template <typename T>
static void SafeRelease(T*& p) { if (p) { p->Release(); p = nullptr; } }

static void ReleaseTransient() {
    SafeRelease(g.capture_srv);
    SafeRelease(g.capture_tex);
    SafeRelease(g.blur_a_srv);
    SafeRelease(g.blur_a_rtv);
    SafeRelease(g.blur_a_tex);
    SafeRelease(g.blur_b_srv);
    SafeRelease(g.blur_b_rtv);
    SafeRelease(g.blur_b_tex);
    g.ready = false;
    g.captured = false;
}

static void ReleaseShaders() {
    SafeRelease(g.vs);
    SafeRelease(g.ps_copy);
    SafeRelease(g.ps_blur);
    SafeRelease(g.samp);
    SafeRelease(g.cb);
    SafeRelease(g.raster);
    SafeRelease(g.blend_off);
    SafeRelease(g.ds_off);
}

static bool CompileShaders() {
    if (g.vs && g.ps_copy && g.ps_blur) return true;

    ID3DBlob* vsBlob = nullptr; ID3DBlob* err = nullptr;
    HRESULT hr = D3DCompile(kShaderHLSL, std::strlen(kShaderHLSL), "BlurVS",
                            nullptr, nullptr, "VS_Fullscreen", "vs_5_0",
                            0, 0, &vsBlob, &err);
    if (FAILED(hr)) {
        std::cout << "[Blur] VS compile failed: "
                  << (err ? (const char*)err->GetBufferPointer() : "(no log)") << std::endl;
        if (err) err->Release();
        return false;
    }
    if (err) { err->Release(); err = nullptr; }
    hr = g.dev->CreateVertexShader(vsBlob->GetBufferPointer(),
                                   vsBlob->GetBufferSize(), nullptr, &g.vs);
    vsBlob->Release();
    if (FAILED(hr)) { std::cout << "[Blur] CreateVertexShader failed " << std::hex << hr << std::endl; return false; }

    auto compilePS = [&](const char* entry, ID3D11PixelShader** out) -> bool {
        ID3DBlob* psBlob = nullptr; ID3DBlob* perr = nullptr;
        HRESULT h = D3DCompile(kShaderHLSL, std::strlen(kShaderHLSL), "BlurPS",
                               nullptr, nullptr, entry, "ps_5_0",
                               0, 0, &psBlob, &perr);
        if (FAILED(h)) {
            std::cout << "[Blur] PS '" << entry << "' compile failed: "
                      << (perr ? (const char*)perr->GetBufferPointer() : "(no log)") << std::endl;
            if (perr) perr->Release();
            return false;
        }
        if (perr) perr->Release();
        h = g.dev->CreatePixelShader(psBlob->GetBufferPointer(),
                                     psBlob->GetBufferSize(), nullptr, out);
        psBlob->Release();
        return SUCCEEDED(h);
    };
    if (!compilePS("PS_Copy", &g.ps_copy)) return false;
    if (!compilePS("PS_Blur", &g.ps_blur)) return false;

    D3D11_SAMPLER_DESC sd{};
    sd.Filter   = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD   = D3D11_FLOAT32_MAX;
    if (FAILED(g.dev->CreateSamplerState(&sd, &g.samp))) return false;

    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth      = sizeof(BlurCB);
    bd.Usage          = D3D11_USAGE_DYNAMIC;
    bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(g.dev->CreateBuffer(&bd, nullptr, &g.cb))) return false;

    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = TRUE;
    if (FAILED(g.dev->CreateRasterizerState(&rd, &g.raster))) return false;

    D3D11_BLEND_DESC bdo{};
    bdo.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (FAILED(g.dev->CreateBlendState(&bdo, &g.blend_off))) return false;

    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = FALSE;
    dsd.StencilEnable = FALSE;
    if (FAILED(g.dev->CreateDepthStencilState(&dsd, &g.ds_off))) return false;

    return true;
}

static DXGI_FORMAT NormaliseFormat(DXGI_FORMAT f) {
    switch (f) {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return f;
        default: return DXGI_FORMAT_UNKNOWN;
    }
}

static bool CreateScratch(int w, int h, DXGI_FORMAT fmt) {

    D3D11_TEXTURE2D_DESC td{};
    td.Width  = w;  td.Height = h;
    td.MipLevels = 1; td.ArraySize = 1;
    td.Format = fmt;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    if (FAILED(g.dev->CreateTexture2D(&td, nullptr, &g.capture_tex))) return false;
    if (FAILED(g.dev->CreateShaderResourceView(g.capture_tex, nullptr, &g.capture_srv))) return false;

    int hw = w / 2, hh = h / 2;
    if (hw < 1) hw = 1; if (hh < 1) hh = 1;
    g.rt_w = hw; g.rt_h = hh;
    td.Width = hw; td.Height = hh;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

    if (FAILED(g.dev->CreateTexture2D(&td, nullptr, &g.blur_a_tex))) return false;
    if (FAILED(g.dev->CreateRenderTargetView(g.blur_a_tex, nullptr, &g.blur_a_rtv))) return false;
    if (FAILED(g.dev->CreateShaderResourceView(g.blur_a_tex, nullptr, &g.blur_a_srv))) return false;

    if (FAILED(g.dev->CreateTexture2D(&td, nullptr, &g.blur_b_tex))) return false;
    if (FAILED(g.dev->CreateRenderTargetView(g.blur_b_tex, nullptr, &g.blur_b_rtv))) return false;
    if (FAILED(g.dev->CreateShaderResourceView(g.blur_b_tex, nullptr, &g.blur_b_srv))) return false;

    return true;
}

struct StateGuard {
    ID3D11DeviceContext* c;
    ID3D11RenderTargetView* rtv[8] = {};
    ID3D11DepthStencilView* dsv = nullptr;
    UINT vp_count = 8; D3D11_VIEWPORT vp[8] = {};
    ID3D11VertexShader* vs = nullptr;
    ID3D11PixelShader*  ps = nullptr;
    ID3D11InputLayout*  il = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY topo = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ID3D11Buffer*       cb0 = nullptr;
    ID3D11ShaderResourceView* srv0 = nullptr;
    ID3D11SamplerState* samp0 = nullptr;
    ID3D11RasterizerState* raster = nullptr;
    ID3D11BlendState* blend = nullptr; FLOAT blendFactor[4] = {}; UINT blendMask = 0;
    ID3D11DepthStencilState* ds = nullptr; UINT stencilRef = 0;

    explicit StateGuard(ID3D11DeviceContext* ctx) : c(ctx) {
        c->OMGetRenderTargets(8, rtv, &dsv);
        c->RSGetViewports(&vp_count, vp);
        c->VSGetShader(&vs, nullptr, nullptr);
        c->PSGetShader(&ps, nullptr, nullptr);
        c->IAGetInputLayout(&il);
        c->IAGetPrimitiveTopology(&topo);
        c->PSGetConstantBuffers(0, 1, &cb0);
        c->PSGetShaderResources(0, 1, &srv0);
        c->PSGetSamplers(0, 1, &samp0);
        c->RSGetState(&raster);
        c->OMGetBlendState(&blend, blendFactor, &blendMask);
        c->OMGetDepthStencilState(&ds, &stencilRef);
    }
    ~StateGuard() {
        c->OMSetRenderTargets(8, rtv, dsv);
        c->RSSetViewports(vp_count, vp);
        c->VSSetShader(vs, nullptr, 0);
        c->PSSetShader(ps, nullptr, 0);
        c->IASetInputLayout(il);
        c->IASetPrimitiveTopology(topo);
        ID3D11Buffer* cbs[1] = { cb0 };
        c->PSSetConstantBuffers(0, 1, cbs);
        ID3D11ShaderResourceView* srvs[1] = { srv0 };
        c->PSSetShaderResources(0, 1, srvs);
        ID3D11SamplerState* sams[1] = { samp0 };
        c->PSSetSamplers(0, 1, sams);
        c->RSSetState(raster);
        c->OMSetBlendState(blend, blendFactor, blendMask);
        c->OMSetDepthStencilState(ds, stencilRef);
        for (int i = 0; i < 8; ++i) SafeRelease(rtv[i]);
        SafeRelease(dsv);
        SafeRelease(vs);
        SafeRelease(ps);
        SafeRelease(il);
        SafeRelease(cb0);
        SafeRelease(srv0);
        SafeRelease(samp0);
        SafeRelease(raster);
        SafeRelease(blend);
        SafeRelease(ds);
    }
};

static void UpdateCB(float texW, float texH, float dirX, float dirY, float radius) {
    D3D11_MAPPED_SUBRESOURCE m{};
    if (FAILED(g.ctx->Map(g.cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &m))) return;
    BlurCB* d = (BlurCB*)m.pData;
    d->texelSize[0] = 1.f / texW;
    d->texelSize[1] = 1.f / texH;
    d->dir[0] = dirX; d->dir[1] = dirY;
    d->radius = radius;
    d->pad[0] = d->pad[1] = d->pad[2] = 0.f;
    g.ctx->Unmap(g.cb, 0);
}

static void RunPass(ID3D11ShaderResourceView* src,
                    ID3D11RenderTargetView* dst,
                    ID3D11PixelShader* ps,
                    float texW, float texH,
                    float dirX, float dirY,
                    float radius)
{
    g.ctx->OMSetRenderTargets(1, &dst, nullptr);
    D3D11_VIEWPORT vp{};
    vp.Width = texW; vp.Height = texH;
    vp.MinDepth = 0.f; vp.MaxDepth = 1.f;
    g.ctx->RSSetViewports(1, &vp);

    UpdateCB(texW, texH, dirX, dirY, radius);

    g.ctx->IASetInputLayout(nullptr);
    g.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g.ctx->VSSetShader(g.vs, nullptr, 0);
    g.ctx->PSSetShader(ps, nullptr, 0);
    ID3D11Buffer* cbs[1] = { g.cb };
    g.ctx->PSSetConstantBuffers(0, 1, cbs);
    ID3D11ShaderResourceView* srvs[1] = { src };
    g.ctx->PSSetShaderResources(0, 1, srvs);
    ID3D11SamplerState* sams[1] = { g.samp };
    g.ctx->PSSetSamplers(0, 1, sams);
    g.ctx->RSSetState(g.raster);
    FLOAT bf[4] = { 0,0,0,0 };
    g.ctx->OMSetBlendState(g.blend_off, bf, 0xFFFFFFFF);
    g.ctx->OMSetDepthStencilState(g.ds_off, 0);

    g.ctx->Draw(3, 0);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    g.ctx->PSSetShaderResources(0, 1, nullSRV);
}

}

bool IsReady() { return g.ready && g.captured; }

ImTextureID GetSrv() {
    if (!IsReady()) return (ImTextureID)nullptr;
    return (ImTextureID)g.blur_b_srv;
}

bool EnsureInit(ID3D11Device* dev, ID3D11DeviceContext* ctx,
                int backbuffer_w, int backbuffer_h,
                DXGI_FORMAT backbuffer_format) {
    if (!dev || !ctx || backbuffer_w <= 1 || backbuffer_h <= 1) return false;
    DXGI_FORMAT fmt = NormaliseFormat(backbuffer_format);
    if (fmt == DXGI_FORMAT_UNKNOWN) {
        std::cout << "[Blur] unsupported backbuffer format " << (int)backbuffer_format << std::endl;
        return false;
    }
    g.dev = dev; g.ctx = ctx;
    if (!CompileShaders()) return false;

    if (g.ready && g.bb_w == backbuffer_w && g.bb_h == backbuffer_h && g.fmt == fmt)
        return true;

    ReleaseTransient();
    g.bb_w = backbuffer_w; g.bb_h = backbuffer_h; g.fmt = fmt;
    if (!CreateScratch(backbuffer_w, backbuffer_h, fmt)) {
        std::cout << "[Blur] CreateScratch failed for " << backbuffer_w << "x" << backbuffer_h << std::endl;
        ReleaseTransient();
        return false;
    }
    g.ready = true;
    std::cout << "[Blur] ready " << backbuffer_w << "x" << backbuffer_h
              << " half-res " << g.rt_w << "x" << g.rt_h
              << " fmt=" << (int)fmt << std::endl;
    return true;
}

void OnResize(int new_w, int new_h) {
    if (!g.ready) return;
    if (new_w == g.bb_w && new_h == g.bb_h) return;
    ReleaseTransient();
    if (g.dev) CreateScratch(new_w, new_h, g.fmt);
    g.bb_w = new_w; g.bb_h = new_h;
    g.ready = (g.capture_tex != nullptr);
}

void Shutdown() {
    ReleaseTransient();
    ReleaseShaders();
    g.dev = nullptr; g.ctx = nullptr;
    g.bb_w = g.bb_h = g.rt_w = g.rt_h = 0;
    g.fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
}

void Capture(ID3D11Texture2D* backbufferTex) {
    if (!g.ready || !backbufferTex || !g.ctx) return;

    D3D11_TEXTURE2D_DESC td{};
    backbufferTex->GetDesc(&td);
    if ((int)td.Width != g.bb_w || (int)td.Height != g.bb_h) {
        OnResize((int)td.Width, (int)td.Height);
        if (!g.ready) return;
    }

    StateGuard guard(g.ctx);

    g.ctx->CopyResource(g.capture_tex, backbufferTex);

    RunPass(g.capture_srv, g.blur_a_rtv, g.ps_blur,
            (float)g.rt_w, (float)g.rt_h,
            1.f, 0.f, 2.0f );

    RunPass(g.blur_a_srv, g.blur_b_rtv, g.ps_blur,
            (float)g.rt_w, (float)g.rt_h,
            0.f, 1.f, 2.0f);

    g.captured = true;
}

}
}

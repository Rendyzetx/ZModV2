#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include "../Tween.h"
#include "BootState.h"
#include <cstdio>
#include <cmath>

namespace ZmodUI {

namespace {
    static float fclamp(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

    struct StageCopy { const char* a; const char* b; const char* c; };
    constexpr StageCopy kCopy[] = {
         { "Booting up\xE2\x80\xA6",       "Waking up\xE2\x80\xA6",      "Stretching\xE2\x80\xA6"      },
         { "Stretching\xE2\x80\xA6",       "Getting ready\xE2\x80\xA6",  "Tuning in\xE2\x80\xA6"       },
         { "Almost there\xE2\x80\xA6",     "Just a sec\xE2\x80\xA6",     "Hold tight\xE2\x80\xA6"      },
         { "Warming up\xE2\x80\xA6",       "Checking the map\xE2\x80\xA6", "Counting blocks\xE2\x80\xA6" },
         { "Loading modules\xE2\x80\xA6",  "Lighting it up\xE2\x80\xA6", "Putting it together\xE2\x80\xA6" },
         { "Ready.",                       "Ready.",                     "Ready."                      },
    };
    constexpr int kCopyCount = sizeof(kCopy) / sizeof(kCopy[0]);

    const char* PickCopy(int stage_idx, float boot_ms) {
        if (stage_idx < 0 || stage_idx >= kCopyCount)
            return "Loading\xE2\x80\xA6";
        int slot = static_cast<int>(boot_ms / 900.f) % 3;
        switch (slot) {
            case 0:  return kCopy[stage_idx].a;
            case 1:  return kCopy[stage_idx].b;
            default: return kCopy[stage_idx].c;
        }
    }
}

void BootStart() {
    Ctx& c = GetCtx();
    c.boot_active = true;
    c.boot_ms = 0.f;
    c.boot_status_idx = -1;
}

void BootRender(State& s) {
    Ctx& c = GetCtx();
    if (!c.boot_active) return;
    float dt_ms = ImGui::GetIO().DeltaTime * 1000.f;

    auto stage = BootState::g_stage.load(std::memory_order_acquire);
    int  pct_raw = BootState::g_progress_percent.load(std::memory_order_acquire);
    if (pct_raw < 0)   pct_raw = 0;
    if (pct_raw > 100) pct_raw = 100;

    if (stage == BootState::Stage::Failed) {
        c.boot_active = false;
        s.boot_complete = false;
        return;
    }

    if (stage == BootState::Stage::FeaturesReady && c.boot_ms == 0.f) {
        c.boot_active = false;
        s.boot_complete = true;
        return;
    }

    c.boot_ms += dt_ms;
    float t = c.boot_ms;

    static float s_readyStartMs = -1.f;
    if (stage == BootState::Stage::FeaturesReady) {
        if (s_readyStartMs < 0.f) s_readyStartMs = t;
    } else {
        s_readyStartMs = -1.f;
    }

    float overlay_a = 1.f;
    if (s_readyStartMs >= 0.f) {
        float ageMs = t - s_readyStartMs;
        overlay_a = 1.f - fclamp(ageMs / 420.f, 0.f, 1.f);
        if (ageMs >= 420.f) {
            c.boot_active = false;
            s.boot_complete = true;
            s_readyStartMs = -1.f;
            return;
        }
    }

    ImVec2 a = ImVec2(c.win_pos.x, c.win_pos.y + Tokens::Size::TitleBarH);
    ImVec2 b = ImVec2(c.win_pos.x + c.win_size.x, c.win_pos.y + c.win_size.y);

    if (Theme::ShouldUseBackdropBlur()) {

        ImU32 baseTint = Tokens::Color::Bg();
        int baseA = (int)((baseTint >> 24) & 0xFF);
        int outA  = (int)(baseA * overlay_a);
        ImU32 tint = (baseTint & 0x00FFFFFF) | ((ImU32)outA << 24);
        ImU32 border = IM_COL32(0,0,0,0);
        Draw::GlassPanel(c.dl, a, b, Tokens::Size::WindowRadius,
                         tint, border, ImDrawFlags_RoundCornersBottom);
    } else {
        ImU32 bg = Tokens::Color::Bg();
        bg = (bg & 0x00FFFFFF) | (((ImU32)(overlay_a * 255)) << 24);
        c.dl->AddRectFilled(a, b, bg, Tokens::Size::WindowRadius,
                            ImDrawFlags_RoundCornersBottom);
    }

    ImVec2 ctr = ImVec2((a.x+b.x)*0.5f, (a.y+b.y)*0.5f - 28.f);

    float markA = 0.f, markS = 0.88f;
    if (t > 180.f) {
        float k = fclamp((t - 180.f) / 520.f, 0.f, 1.f);
        k = Tokens::Motion::MacEase(k);
        markA = k; markS = 0.88f + 0.12f * k;
    }
    {
        float sz = 64.f * markS;
        ImVec2 ma = ImVec2(ctr.x - sz*0.5f, ctr.y - sz*0.5f - 36.f);
        ImVec2 mb = ImVec2(ma.x + sz, ma.y + sz);
        int alpha_i = (int)(markA * 255 * overlay_a);
        if (alpha_i < 0) alpha_i = 0; if (alpha_i > 255) alpha_i = 255;
        float radius = sz * 0.25f;

        ImVec2 ctrTile = ImVec2((ma.x + mb.x) * 0.5f, (ma.y + mb.y) * 0.5f);

        float breath = 0.85f + 0.15f * sinf(c.boot_ms * 0.005f);
        int   wideA  = (int)(alpha_i * 0.55f * breath);
        if (wideA > 0) {
            Draw::SoftHaloCircle(c.dl, ctrTile,
                                 sz * 0.55f,
                                 sz * 2.10f,
                                 IM_COL32(10, 132, 255, wideA),
                                 18);
        }

        int purpA = (int)(alpha_i * 0.35f * breath);
        if (purpA > 0) {
            Draw::SoftHaloCircle(c.dl,
                                 ImVec2(ctrTile.x + 8.f, ctrTile.y + 4.f),
                                 sz * 0.45f, sz * 1.60f,
                                 IM_COL32(94, 92, 230, purpA),
                                 14);
        }

        int rectA = (int)(alpha_i * 0.65f);
        if (rectA > 0) {
            Draw::SoftHaloRect(c.dl, ma, mb, radius,
                               IM_COL32(40, 110, 244, rectA),
                               20.f, 14);
        }

        ImU32 base = IM_COL32(40, 110, 244, alpha_i);
        Draw::RectFilledRounded(c.dl, ma, mb, base, radius);

        ImU32 hi = IM_COL32(120, 170, 255, (int)(alpha_i * 0.45f));
        Draw::RectFilledRounded(c.dl,
            ImVec2(ma.x + 1, ma.y + 1),
            ImVec2(ma.x + sz*0.55f, ma.y + sz*0.55f), hi, radius);

        ImU32 lo = IM_COL32(94, 92, 230, (int)(alpha_i * 0.6f));
        Draw::RectFilledRounded(c.dl,
            ImVec2(ma.x + sz*0.45f, ma.y + sz*0.45f),
            ImVec2(mb.x - 1, mb.y - 1), lo, radius);

        Draw::RectStrokeRounded(c.dl, ma, mb,
            IM_COL32(255, 255, 255, (int)(alpha_i * 0.15f)),
            radius, 1.f);

        ImFont* fz = Tokens::Type::H1();
        ImVec2 zs = Draw::TextSize(fz, "Z");
        ImU32 zc = IM_COL32(255,255,255, alpha_i);
        Draw::TextL(c.dl, fz, ImVec2(ma.x + (sz - zs.x)*0.5f - 1.f, ma.y + (sz - zs.y)*0.5f - 2.f), zc, "Z");
    }

    if (t > 480.f) {
        float k = fclamp((t - 480.f) / 380.f, 0.f, 1.f);
        ImU32 cc = (Tokens::Color::Text() & 0x00FFFFFF) | (((ImU32)(k*overlay_a*255)) << 24);
        ImFont* fw = Tokens::Type::H3();
        ImVec2 ts = Draw::TextSize(fw, "ZMod");
        Draw::TextL(c.dl, fw, ImVec2(ctr.x - ts.x*0.5f, ctr.y + 36.f - 4.f*(1.f-k)), cc, "ZMod");
    }

    if (t > 620.f) {
        float k = fclamp((t - 620.f) / 380.f, 0.f, 1.f);
        ImU32 cc = (Tokens::Color::TextTert() & 0x00FFFFFF) | (((ImU32)(k*overlay_a*255)) << 24);
        ImFont* fe = Tokens::Type::Eyebrow();
        const char* lab = "LOADING WORKSPACE";
        ImVec2 ts = Draw::TextSize(fe, lab);
        Draw::TextL(c.dl, fe, ImVec2(ctr.x - ts.x*0.5f, ctr.y + 70.f), cc, lab);
    }

    if (t > 700.f) {
        static float s_fillSmoothed = 0.f;
        float target = pct_raw / 100.f;

        float k = fclamp(dt_ms / 80.f, 0.f, 1.f);
        s_fillSmoothed += (target - s_fillSmoothed) * k;

        float kfill = fclamp(s_fillSmoothed, 0.f, 1.f);

        float W = 220.f, H = 3.f;
        ImVec2 ba = ImVec2(ctr.x - W*0.5f, ctr.y + 96.f);
        ImVec2 bb = ImVec2(ba.x + W, ba.y + H);
        ImU32 trk = (Tokens::Color::LineSoft() & 0x00FFFFFF) | (((ImU32)(overlay_a*255)) << 24);
        Draw::RectFilledRounded(c.dl, ba, bb, trk, 2.f);
        ImVec2 fb = ImVec2(ba.x + W * kfill, bb.y);
        c.dl->AddRectFilledMultiColor(ba, fb,
            IM_COL32(10,132,255,(int)(255*overlay_a)),
            IM_COL32(94,92,230,(int)(255*overlay_a)),
            IM_COL32(94,92,230,(int)(255*overlay_a)),
            IM_COL32(10,132,255,(int)(255*overlay_a)));
    }

    if (t > 700.f) {
        int stage_idx = static_cast<int>(stage);
        if (stage_idx < 0 || stage_idx >= kCopyCount) stage_idx = 0;
        const char* msg = PickCopy(stage_idx, t);

        ImFont* fm = Tokens::Type::Mono();
        ImVec2 ts = Draw::TextSize(fm, msg);
        ImU32 cc = (Tokens::Color::TextTert() & 0x00FFFFFF) | (((ImU32)(overlay_a*255)) << 24);
        Draw::TextL(c.dl, fm, ImVec2(ctr.x - ts.x*0.5f, ctr.y + 116.f), cc, msg);
    }
}

}

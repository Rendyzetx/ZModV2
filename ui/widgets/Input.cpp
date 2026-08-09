#include "Row.h"
#include "Primitives.h"
#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include <cstring>
#include <cstdio>
#include <cmath>

namespace ZmodUI {

namespace {
    ImGuiID g_focus_id = 0;

    static void AppendStr(char* buf, int buf_size, const char* add, bool keepNewlines) {
        int len = (int)std::strlen(buf);
        for (const char* p = add; *p; ++p) {
            char ch = *p;
            if (ch == '\r') ch = '\n';
            if (ch == '\n' && !keepNewlines) ch = ' ';
            if (ch != '\n' && (unsigned char)ch < 0x20) continue;
            if (len + 1 >= buf_size) break;
            buf[len++] = ch;
        }
        buf[len] = 0;
    }

    static void HandleTyping(char* buf, int buf_size, bool multiline) {
        ImGuiIO& io = ImGui::GetIO();

        for (int i = 0; i < io.InputQueueCharacters.Size; ++i) {
            ImWchar wc = io.InputQueueCharacters[i];
            if (wc == '\r') wc = '\n';
            if (wc < 0x20 && !(multiline && wc == '\n')) continue;
            int len = (int)std::strlen(buf);
            if (len + 4 >= buf_size) continue;
            char tmp[5] = {};
            int nb = 0;
            if (wc < 0x80) { tmp[nb++] = (char)wc; }
            else if (wc < 0x800) { tmp[nb++] = (char)(0xC0 | (wc >> 6)); tmp[nb++] = (char)(0x80 | (wc & 0x3F)); }
            else { tmp[nb++] = (char)(0xE0 | (wc >> 12)); tmp[nb++] = (char)(0x80 | ((wc >> 6) & 0x3F)); tmp[nb++] = (char)(0x80 | (wc & 0x3F)); }
            std::memcpy(buf + len, tmp, nb);
            buf[len + nb] = 0;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
            int len = (int)std::strlen(buf);
            if (len > 0) {
                while (len > 0 && (buf[len-1] & 0xC0) == 0x80) { buf[--len] = 0; }
                if (len > 0) buf[--len] = 0;
            }
        }
        io.InputQueueCharacters.resize(0);
    }
}

static bool DrawInputBox(ImVec2 pos, float w, float h, char* buf, int buf_size,
                         const char* placeholder, ImGuiID id, bool mono, bool multiline) {
    Ctx& c = GetCtx();
    ImVec2 a = pos;
    ImVec2 b = ImVec2(pos.x + w, pos.y + h);
    char idb[40]; ImFormatString(idb, sizeof(idb), "in##%u", (unsigned)id);
    Layout::Interact it = Layout::RegisterHit(idb, a, b);
    if (it.clicked) g_focus_id = id;
    if (g_focus_id == id && ImGui::IsKeyPressed(ImGuiKey_Escape)) g_focus_id = 0;

    bool focused = (g_focus_id == id);
    Draw::RectFilledRounded(c.dl, a, b, Tokens::Color::BgCard(), 6.f);
    ImU32 line = focused ? Tokens::Color::Accent() : Tokens::Color::Line();
    Draw::RectStrokeRounded(c.dl, a, b, line, 6.f, focused ? 2.f : 1.f);

    bool changed = false;
    if (focused) {
        ImGuiIO& io = ImGui::GetIO();
        size_t before = std::strlen(buf);
        HandleTyping(buf, buf_size, multiline);

        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V, false)) {
            const char* clip = ImGui::GetClipboardText();
            if (clip && *clip) AppendStr(buf, buf_size, clip, multiline);
        }

        if (multiline && (ImGui::IsKeyPressed(ImGuiKey_Enter, false) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false))) {
            int len = (int)std::strlen(buf);
            if (len + 1 < buf_size) { buf[len] = '\n'; buf[len + 1] = 0; }
        }
        changed = (std::strlen(buf) != before);
    }

    ImFont* f = mono ? Tokens::Type::Mono() : Tokens::Type::BodySec();
    float fh = f ? f->FontSize : 13.f;
    bool empty = (buf == nullptr || buf[0] == 0);
    if (empty && placeholder) {
        Draw::TextL(c.dl, f, ImVec2(a.x + 10, a.y + 4), Tokens::Color::TextTert(), placeholder);
    } else if (multiline) {
        c.dl->PushClipRect(a, b, true);
        float lh = fh + 3.f;
        float ty = a.y + 4.f;
        const char* p = buf;
        char lineBuf[512];
        while (p && *p && ty < b.y - 2.f) {
            const char* nl = std::strchr(p, '\n');
            int llen = nl ? (int)(nl - p) : (int)std::strlen(p);
            int cap = (int)sizeof(lineBuf) - 1;
            int n = llen < cap ? llen : cap;
            std::memcpy(lineBuf, p, (size_t)n); lineBuf[n] = 0;
            Draw::TextL(c.dl, f, ImVec2(a.x + 10, ty), Tokens::Color::Text(), lineBuf);
            ty += lh;
            if (!nl) break;
            p = nl + 1;
        }
        c.dl->PopClipRect();
    } else {
        c.dl->PushClipRect(a, b, true);
        Draw::TextL(c.dl, f, ImVec2(a.x + 10, a.y + 4), Tokens::Color::Text(), buf);
        c.dl->PopClipRect();
    }

    if (focused) {
        float ct = (float)ImGui::GetTime();
        float blink = 0.5f + 0.5f * std::sin(ct * 6.f);
        ImU32 cc = Tokens::Color::Accent();
        cc = (cc & 0x00FFFFFF) | (((ImU32)(blink * 255)) << 24);
        if (multiline) {
            const char* lastNl = std::strrchr(buf, '\n');
            const char* lastLine = lastNl ? lastNl + 1 : buf;
            int lines = 1; for (const char* q = buf; *q; ++q) if (*q == '\n') ++lines;
            float lh = fh + 3.f;
            ImVec2 lts = Draw::TextSize(f, lastLine);
            float cx = a.x + 10 + lts.x + 1.f;
            float cy0 = a.y + 4.f + (lines - 1) * lh;
            if (cy0 + fh < b.y - 2.f)
                c.dl->AddLine(ImVec2(cx, cy0), ImVec2(cx, cy0 + fh), cc, 1.5f);
        } else {
            ImVec2 ts = Draw::TextSize(f, buf);
            float cx = a.x + 10 + ts.x + 1.f;
            c.dl->AddLine(ImVec2(cx, a.y + 5.f), ImVec2(cx, a.y + h - 5.f), cc, 1.5f);
        }
    }
    return changed;
}

bool InputRow(const char* label, char* buf, int buf_size, const char* placeholder) {
    WRow::Opts o;
    o.no_icon = false;
    o.icon_id = "blank";
    o.label = label;
    WRow::RowResult r = WRow::Begin(o);
    float W = 160.f, H = 22.f;
    ImVec2 a = ImVec2(r.end_b.x - W, (r.a.y + r.b.y)*0.5f - H*0.5f);
    bool mono = false;
    if (placeholder && std::strstr(placeholder, "SendBson")) mono = true;
    return DrawInputBox(a, W, H, buf, buf_size, placeholder, ImGui::GetID(label), mono, false);
}

bool TextareaFullWidth(char* buf, int buf_size, const char* placeholder, float height_px) {
    Ctx& c = GetCtx();
    if (height_px <= 0) height_px = 120.f;
    float y = Layout::ContentToScreenY(c.cursor.y);
    float w = c.content_x1 - c.content_x0 - 28.f;
    ImVec2 a = ImVec2(c.content_x0 + 14.f, y);

    bool changed = DrawInputBox(a, w, height_px, buf, buf_size, placeholder, ImGui::GetID((void*)buf), true, true);
    c.cursor.y += height_px + 10.f;
    return changed;
}

}

namespace ZmodUI {
namespace Draw {
void PushClipText(ImDrawList* dl, ImVec2 a, ImVec2 b) {
    dl->PushClipRect(a, b, true);
}
void PopClipText(ImDrawList* dl) {
    dl->PopClipRect();
}
}}

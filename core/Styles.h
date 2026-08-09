
#pragma once
#include "imgui/imgui.h"
#include <iostream>

void ApplyCustomStyleDefault() {

    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha                  = 1.0f;
    style.WindowPadding          = ImVec2(10.0f, 10.0f);
    style.WindowRounding         = 8.0f;
    style.WindowBorderSize       = 1.0f;
    style.WindowMinSize          = ImVec2(30.0f, 30.0f);
    style.WindowTitleAlign       = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding          = 6.0f;
    style.ChildBorderSize        = 1.0f;
    style.PopupRounding          = 8.0f;
    style.PopupBorderSize        = 1.0f;
    style.FramePadding           = ImVec2(6.0f, 4.0f);
    style.FrameRounding          = 5.0f;
    style.FrameBorderSize        = 0.0f;
    style.ItemSpacing            = ImVec2(6.0f, 5.0f);
    style.ItemInnerSpacing       = ImVec2(5.0f, 5.0f);
    style.IndentSpacing          = 5.0f;
    style.ColumnsMinSpacing      = 5.0f;
    style.ScrollbarSize          = 12.0f;
    style.ScrollbarRounding      = 6.0f;
    style.GrabMinSize            = 12.0f;
    style.GrabRounding           = 4.0f;
    style.TabRounding            = 5.0f;
    style.TabBorderSize          = 0.0f;
    style.ColorButtonPosition    = ImGuiDir_Left;
    style.ButtonTextAlign        = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign    = ImVec2(0.0f, 0.0f);

    const ImVec4 bg       = ImVec4(0.055f, 0.063f, 0.094f, 1.000f);
    const ImVec4 panel    = ImVec4(0.082f, 0.102f, 0.149f, 1.000f);
    const ImVec4 frame    = ImVec4(0.110f, 0.137f, 0.200f, 1.000f);
    const ImVec4 accent   = ImVec4(0.000f, 0.784f, 0.961f, 1.000f);
    const ImVec4 accentDim= ImVec4(0.000f, 0.784f, 0.961f, 0.550f);
    const ImVec4 accentHov= ImVec4(0.000f, 0.659f, 0.812f, 1.000f);
    const ImVec4 text     = ImVec4(0.894f, 0.918f, 0.957f, 1.000f);
    const ImVec4 textDim  = ImVec4(0.894f, 0.918f, 0.957f, 0.380f);
    const ImVec4 transpar = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);

    style.Colors[ImGuiCol_Text]                 = text;
    style.Colors[ImGuiCol_TextDisabled]         = textDim;
    style.Colors[ImGuiCol_WindowBg]             = bg;
    style.Colors[ImGuiCol_ChildBg]              = transpar;
    style.Colors[ImGuiCol_PopupBg]              = panel;
    style.Colors[ImGuiCol_Border]               = accentDim;
    style.Colors[ImGuiCol_BorderShadow]         = transpar;
    style.Colors[ImGuiCol_FrameBg]              = frame;
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.000f, 0.784f, 0.961f, 0.180f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.000f, 0.784f, 0.961f, 0.320f);
    style.Colors[ImGuiCol_TitleBg]              = bg;
    style.Colors[ImGuiCol_TitleBgActive]        = panel;
    style.Colors[ImGuiCol_TitleBgCollapsed]     = bg;
    style.Colors[ImGuiCol_MenuBarBg]            = transpar;
    style.Colors[ImGuiCol_ScrollbarBg]          = transpar;
    style.Colors[ImGuiCol_ScrollbarGrab]        = frame;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = accentDim;
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = accent;
    style.Colors[ImGuiCol_CheckMark]            = accent;
    style.Colors[ImGuiCol_SliderGrab]           = accent;
    style.Colors[ImGuiCol_SliderGrabActive]     = accentHov;
    style.Colors[ImGuiCol_Button]               = accentDim;
    style.Colors[ImGuiCol_ButtonHovered]        = accent;
    style.Colors[ImGuiCol_ButtonActive]         = accentHov;
    style.Colors[ImGuiCol_Header]               = ImVec4(0.000f, 0.784f, 0.961f, 0.300f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.000f, 0.784f, 0.961f, 0.500f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.000f, 0.784f, 0.961f, 0.750f);
    style.Colors[ImGuiCol_Separator]            = accentDim;
    style.Colors[ImGuiCol_SeparatorHovered]     = accent;
    style.Colors[ImGuiCol_SeparatorActive]      = accent;
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.000f, 0.784f, 0.961f, 0.200f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.000f, 0.784f, 0.961f, 0.600f);
    style.Colors[ImGuiCol_ResizeGripActive]     = accent;
    style.Colors[ImGuiCol_Tab]                  = ImVec4(0.000f, 0.784f, 0.961f, 0.280f);
    style.Colors[ImGuiCol_TabHovered]           = ImVec4(0.000f, 0.784f, 0.961f, 0.700f);
    style.Colors[ImGuiCol_TabActive]            = accent;
    style.Colors[ImGuiCol_TabUnfocused]         = bg;
    style.Colors[ImGuiCol_TabUnfocusedActive]   = frame;
    style.Colors[ImGuiCol_PlotLines]            = accentDim;
    style.Colors[ImGuiCol_PlotLinesHovered]     = accent;
    style.Colors[ImGuiCol_PlotHistogram]        = accentDim;
    style.Colors[ImGuiCol_PlotHistogramHovered] = accent;
}

void ApplyCustomStyle1()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 3.0f;
    style.PopupRounding = 5.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.95f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.50f, 0.60f, 0.58f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.12f, 0.18f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.15f, 0.25f, 0.90f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.13f, 0.22f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.30f, 0.50f, 0.80f, 0.80f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.40f, 0.80f, 0.75f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.55f, 0.90f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.65f, 1.00f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.40f, 0.80f, 0.75f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.55f, 0.90f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.65f, 1.00f, 1.00f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.30f, 0.55f, 0.60f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.45f, 0.75f, 0.80f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.55f, 0.85f, 1.00f);

    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.35f, 0.70f, 0.90f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.55f, 0.90f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.65f, 1.00f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.25f, 0.50f, 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.40f, 0.70f, 1.00f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.30f, 0.55f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.20f, 0.40f, 0.90f);

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.12f, 0.18f, 0.80f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.45f, 0.75f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.55f, 0.90f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.65f, 1.00f, 1.00f);

    colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.55f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.65f, 1.00f, 1.00f);

    colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.90f, 1.00f, 1.00f);

    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.50f, 0.80f, 0.80f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);

    colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.50f, 0.80f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.60f, 0.90f, 0.90f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);

    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.13f, 0.22f, 0.94f);

    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.10f, 0.20f, 0.70f);
}

void ApplyCustomStyle2()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 4.0f;
    style.PopupRounding = 6.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.22f, 0.22f, 0.58f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.07f, 0.07f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.07f, 0.07f, 0.90f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.05f, 0.05f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.50f, 0.20f, 0.20f, 0.80f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    colors[ImGuiCol_Header] = ImVec4(0.55f, 0.18f, 0.18f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.80f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.95f, 0.40f, 0.40f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.55f, 0.15f, 0.15f, 0.80f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.95f, 0.40f, 0.40f, 1.00f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.45f, 0.15f, 0.15f, 0.60f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.75f, 0.30f, 0.30f, 0.80f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.35f, 0.35f, 1.00f);

    colors[ImGuiCol_Tab] = ImVec4(0.50f, 0.15f, 0.15f, 0.90f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.75f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.85f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.40f, 0.10f, 0.10f, 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.55f, 0.20f, 0.20f, 1.00f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.45f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.55f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.35f, 0.10f, 0.10f, 0.90f);

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.07f, 0.07f, 0.80f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.50f, 0.15f, 0.15f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.75f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.85f, 0.35f, 0.35f, 1.00f);

    colors[ImGuiCol_SliderGrab] = ImVec4(0.75f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.85f, 0.35f, 0.35f, 1.00f);

    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.75f, 0.75f, 1.00f);

    colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.20f, 0.20f, 0.80f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.85f, 0.35f, 0.35f, 1.00f);

    colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.20f, 0.20f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.75f, 0.30f, 0.30f, 0.90f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.85f, 0.35f, 0.35f, 1.00f);

    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.05f, 0.05f, 0.94f);

    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.03f, 0.03f, 0.70f);
}

void ApplyEmeraldPowerStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.Alpha = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 10.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(30.0f, 30.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 5.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 10.0f;
    style.PopupBorderSize = 0.0f;
    style.FramePadding = ImVec2(5.0f, 3.5f);
    style.FrameRounding = 5.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(5.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
    style.IndentSpacing = 5.0f;
    style.ColumnsMinSpacing = 5.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 15.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    style.TabBorderSize = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Left;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    ImVec4 emerald = ImVec4(0.20f, 1.00f, 0.30f, 1.00f);
    ImVec4 emerald_hover = ImVec4(0.30f, 1.00f, 0.40f, 1.00f);
    ImVec4 emerald_dark = ImVec4(0.10f, 0.40f, 0.20f, 1.00f);
    ImVec4 bg_dark = ImVec4(0.02f, 0.06f, 0.04f, 0.95f);

    colors[ImGuiCol_Text] = ImVec4(0.85f, 0.95f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.45f, 0.40f, 1.00f);
    colors[ImGuiCol_WindowBg] = bg_dark;
    colors[ImGuiCol_ChildBg] = ImVec4(0.03f, 0.08f, 0.05f, 0.70f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.02f, 0.05f, 0.03f, 0.94f);
    colors[ImGuiCol_Border] = emerald_dark;
    colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.15f, 0.10f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.25f, 0.12f, 0.80f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.10f, 0.35f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBg] = emerald_dark;
    colors[ImGuiCol_TitleBgActive] = emerald;
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.02f, 0.08f, 0.05f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.03f, 0.06f, 0.04f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = emerald_dark;
    colors[ImGuiCol_ScrollbarGrabHovered] = emerald;
    colors[ImGuiCol_ScrollbarGrabActive] = emerald_hover;
    colors[ImGuiCol_CheckMark] = emerald;
    colors[ImGuiCol_SliderGrab] = emerald;
    colors[ImGuiCol_SliderGrabActive] = emerald_hover;
    colors[ImGuiCol_Button] = emerald_dark;
    colors[ImGuiCol_ButtonHovered] = emerald;
    colors[ImGuiCol_ButtonActive] = emerald_hover;
    colors[ImGuiCol_Header] = emerald_dark;
    colors[ImGuiCol_HeaderHovered] = emerald;
    colors[ImGuiCol_HeaderActive] = emerald_hover;
    colors[ImGuiCol_Separator] = emerald_dark;
    colors[ImGuiCol_Tab] = emerald_dark;
    colors[ImGuiCol_TabHovered] = emerald;
    colors[ImGuiCol_TabActive] = emerald_hover;
}

void ApplyCyberSunsetStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.Alpha = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 10.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(30.0f, 30.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 5.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 10.0f;
    style.PopupBorderSize = 0.0f;
    style.FramePadding = ImVec2(5.0f, 3.5f);
    style.FrameRounding = 5.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(5.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
    style.IndentSpacing = 5.0f;
    style.ColumnsMinSpacing = 5.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 15.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    style.TabBorderSize = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Left;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    ImVec4 main_accent = ImVec4(1.00f, 0.40f, 0.00f, 0.85f);
    ImVec4 hover_accent = ImVec4(1.00f, 0.55f, 0.20f, 1.00f);
    ImVec4 active_accent = ImVec4(1.00f, 0.70f, 0.30f, 1.00f);

    colors[ImGuiCol_Text] = ImVec4(1.00f, 0.95f, 0.85f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.45f, 0.40f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.03f, 0.10f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.05f, 0.15f, 0.60f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.04f, 0.02f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.40f, 0.20f, 0.60f, 0.50f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.40f, 0.20f, 0.10f, 0.30f);
    colors[ImGuiCol_FrameBgHovered] = hover_accent;
    colors[ImGuiCol_FrameBgActive] = active_accent;
    colors[ImGuiCol_TitleBg] = main_accent;
    colors[ImGuiCol_TitleBgActive] = active_accent;
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.08f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = main_accent;
    colors[ImGuiCol_ScrollbarGrabHovered] = hover_accent;
    colors[ImGuiCol_ScrollbarGrabActive] = active_accent;
    colors[ImGuiCol_CheckMark] = active_accent;
    colors[ImGuiCol_SliderGrab] = main_accent;
    colors[ImGuiCol_SliderGrabActive] = active_accent;
    colors[ImGuiCol_Button] = main_accent;
    colors[ImGuiCol_ButtonHovered] = hover_accent;
    colors[ImGuiCol_ButtonActive] = active_accent;
    colors[ImGuiCol_Header] = main_accent;
    colors[ImGuiCol_HeaderHovered] = hover_accent;
    colors[ImGuiCol_HeaderActive] = active_accent;
    colors[ImGuiCol_Separator] = main_accent;
    colors[ImGuiCol_Tab] = main_accent;
    colors[ImGuiCol_TabHovered] = hover_accent;
    colors[ImGuiCol_TabActive] = active_accent;
}

void ApplyDaylightStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 8.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.ChildRounding = 5.0f;
    style.PopupRounding = 8.0f;
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.FrameRounding = 5.0f;
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabMinSize = 12.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    style.TabBorderSize = 0.0f;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.93f, 0.93f, 0.94f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.96f, 0.96f, 0.97f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.97f, 0.97f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.90f, 0.90f, 0.92f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.84f, 0.84f, 0.87f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.80f, 0.84f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.89f, 0.89f, 0.91f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.86f, 0.86f, 0.89f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.92f, 0.92f, 0.94f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.74f, 0.74f, 0.78f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.70f, 0.70f, 0.74f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.35f, 0.45f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.60f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.50f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.85f, 0.88f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.75f, 0.80f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.70f, 0.77f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.80f, 0.83f, 0.93f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.73f, 0.77f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.66f, 0.70f, 0.86f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.78f, 0.78f, 0.81f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.70f, 0.70f, 0.74f, 0.4f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.60f, 0.68f, 0.6f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.55f, 0.70f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.82f, 0.84f, 0.92f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.75f, 0.78f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.70f, 0.74f, 0.88f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.86f, 0.86f, 0.88f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.80f, 0.82f, 0.90f, 1.0f);
}

void ApplyGrayscaleStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 8.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.ChildRounding = 5.0f;
    style.PopupRounding = 8.0f;
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.FrameRounding = 5.0f;
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabMinSize = 12.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    style.TabBorderSize = 0.0f;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.8f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.7f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.30f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.6f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 0.50f, 0.9f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
}

void ApplyPastelDreamStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.Alpha = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 10.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(30.0f, 30.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 5.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 10.0f;
    style.PopupBorderSize = 0.0f;
    style.FramePadding = ImVec2(5.0f, 3.5f);
    style.FrameRounding = 5.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(5.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
    style.IndentSpacing = 5.0f;
    style.ColumnsMinSpacing = 5.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 15.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    style.TabBorderSize = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Left;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    ImVec4 lavender = ImVec4(0.60f, 0.40f, 0.80f, 1.00f);
    ImVec4 lavender_hover = ImVec4(0.70f, 0.50f, 0.90f, 1.00f);
    ImVec4 cyan = ImVec4(0.30f, 0.70f, 0.80f, 1.00f);

    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.90f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.45f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.20f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.24f, 0.70f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.15f, 0.94f);
    colors[ImGuiCol_Border] = lavender;
    colors[ImGuiCol_FrameBg] = ImVec4(0.40f, 0.30f, 0.60f, 0.40f);
    colors[ImGuiCol_FrameBgHovered] = lavender_hover;
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.60f, 0.50f, 0.80f, 0.80f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = lavender;
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = lavender;
    colors[ImGuiCol_ScrollbarGrabHovered] = lavender_hover;
    colors[ImGuiCol_ScrollbarGrabActive] = cyan;
    colors[ImGuiCol_CheckMark] = cyan;
    colors[ImGuiCol_SliderGrab] = cyan;
    colors[ImGuiCol_SliderGrabActive] = lavender_hover;
    colors[ImGuiCol_Button] = lavender;
    colors[ImGuiCol_ButtonHovered] = lavender_hover;
    colors[ImGuiCol_ButtonActive] = cyan;
    colors[ImGuiCol_Header] = lavender;
    colors[ImGuiCol_HeaderHovered] = lavender_hover;
    colors[ImGuiCol_HeaderActive] = cyan;
    colors[ImGuiCol_Separator] = lavender;
    colors[ImGuiCol_Tab] = lavender;
    colors[ImGuiCol_TabHovered] = lavender_hover;
    colors[ImGuiCol_TabActive] = cyan;
}

inline ImVec4 g_customThemeColors[ImGuiCol_COUNT];
inline bool   g_customThemeInitialized = false;

inline void InitCustomThemeFromCurrent() {
    ImGuiStyle& s = ImGui::GetStyle();
    for (int i = 0; i < ImGuiCol_COUNT; ++i) g_customThemeColors[i] = s.Colors[i];
    g_customThemeInitialized = true;
}

inline void InitCustomThemeFromDefault() {
    ApplyCustomStyleDefault();
    InitCustomThemeFromCurrent();
}

inline void ApplyCustomUserStyle() {

    ApplyCustomStyleDefault();
    if (!g_customThemeInitialized) {
        InitCustomThemeFromCurrent();
    }
    ImGuiStyle& s = ImGui::GetStyle();
    for (int i = 0; i < ImGuiCol_COUNT; ++i) s.Colors[i] = g_customThemeColors[i];
}

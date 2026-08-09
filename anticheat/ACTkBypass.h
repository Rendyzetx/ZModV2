

#pragma once

#include <iostream>
#include <string>
#include "XorStr.h"
#include "Manifest.h"

namespace ACTkBypass {

extern "C" void __fastcall HookNoop() {}

extern "C" bool __fastcall HookRetFalse() { return false; }

inline int g_noop_count        = 0;
inline int g_ret_false_count   = 0;
inline int g_skipped_missing   = 0;
inline int g_skipped_unknown   = 0;

inline void LogInstalled() {
    std::cout << XS("[ACTk] Install summary: ")
              << g_noop_count       << XS(" noop, ")
              << g_ret_false_count  << XS(" ret_false, ")
              << g_skipped_missing  << XS(" skipped(missing), ")
              << g_skipped_unknown  << XS(" skipped(unknown-action)")
              << std::endl;
}

inline void Install() {

    if (!moduleBase) {
        moduleBase = (uintptr_t)GetModuleHandleA(XS("GameAssembly.dll"));
    }
    if (!moduleBase) {
        std::cout << XS("[ACTk] GameAssembly.dll not loaded — bypass skipped") << std::endl;
        return;
    }

    g_noop_count = g_ret_false_count = g_skipped_missing = g_skipped_unknown = 0;

    const nlohmann::json* ac = Manifest::GetAnticheatSection();
    if (!ac || !ac->contains("actk_bypass") || !(*ac)["actk_bypass"].is_object()) {
        std::cout << XS("[ACTk] No bypass RVAs in manifest — detectors will run normally.")
                  << std::endl;
        return;
    }
    const auto& bypass = (*ac)["actk_bypass"];
    if (!bypass.contains("rvas") || !bypass["rvas"].is_array() || bypass["rvas"].empty()) {
        std::cout << XS("[ACTk] No bypass RVAs in manifest — detectors will run normally.")
                  << std::endl;
        return;
    }

    std::cout << XS("[ACTk] Installing detector neutralizers @ moduleBase=0x")
              << std::hex << moduleBase << std::dec
              << XS(" from manifest (") << bypass["rvas"].size() << XS(" entries)")
              << std::endl;

    for (const auto& entry : bypass["rvas"]) {
        if (!entry.is_object()) {
            ++g_skipped_unknown;
            continue;
        }

        if (entry.value("missing", false)) {
            ++g_skipped_missing;
            continue;
        }

        if (!entry.contains("rva") || !entry["rva"].is_string()) {
            ++g_skipped_unknown;
            continue;
        }
        uintptr_t rva = Manifest::ParseHex(entry["rva"].get<std::string>());
        if (!rva) {
            ++g_skipped_unknown;
            continue;
        }

        std::string action = entry.value("action", std::string());
        if (action == "noop") {
            SIGMAHOOK(rva, &HookNoop, nullptr);
            ++g_noop_count;
        } else if (action == "ret_false") {
            SIGMAHOOK(rva, &HookRetFalse, nullptr);
            ++g_ret_false_count;
        } else if (action == "skip" || action.empty()) {

            ++g_skipped_unknown;
        } else {
            std::string name = entry.value("name", std::string("(unnamed)"));
            std::cout << XS("[ACTk] Unknown action '") << action
                      << XS("' for ") << name << XS(" @ rva=0x")
                      << std::hex << rva << std::dec << XS(" — skipped")
                      << std::endl;
            ++g_skipped_unknown;
        }
    }

    LogInstalled();
}

}

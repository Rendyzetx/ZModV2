

#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <chrono>
#include <ctime>
#include <algorithm>

#include "imgui/imgui.h"
#include "json.hpp"
#include "AccountCrypto.h"
#include "DllAuth.h"
#include "MultiboxMode.h"

void ShowNotification(const std::string& title, const std::string& message,
                      NotificationType type, float duration_seconds);

namespace ZModAccounts {

enum class TokenStatus { Unknown, Valid, Expired, Banned };

struct AccountEntry {
    std::string uuid;
    std::string alias;
    std::string username;
    std::string password;
    std::string fakeHwid;
    TokenStatus status      = TokenStatus::Unknown;
    bool        isBanned    = false;
    std::string lastUsedIso;
    std::string createdAtIso;
    int         version     = 1;
};

inline std::vector<AccountEntry> g_accounts;
inline std::mutex                g_mutex;
inline int                       g_selectedIndex = -1;
inline bool                      g_initialized   = false;

inline int g_pendingDeleteIndex = -1;

inline MultiboxMode::CapturedCreds g_capturedForPopup;
inline bool                        g_popupOpenRequested = false;

inline bool                        g_logoutFirstPopupOpen = false;
inline std::string                 g_logoutFirstPopupAcct;

inline std::filesystem::path AccountsDir() {
    PWSTR appData = nullptr;
    std::filesystem::path p;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appData))) {
        p = std::filesystem::path(appData) / L"ZMod" / L"Accounts";
        CoTaskMemFree(appData);
    } else {

        p = std::filesystem::path("ZMod") / "Accounts";
    }
    std::error_code ec;
    std::filesystem::create_directories(p, ec);
    return p;
}

inline std::filesystem::path AccountFilePath(const std::string& uuid) {
    return AccountsDir() / (uuid + ".acct");
}

inline std::string NowIsoUtc() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = {};
    gmtime_s(&tm, &t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return std::string(buf);
}

inline const char* StatusToStr(TokenStatus s) {
    switch (s) {
        case TokenStatus::Valid:   return "valid";
        case TokenStatus::Expired: return "expired";
        case TokenStatus::Banned:  return "banned";
        default:                   return "unknown";
    }
}
inline TokenStatus StatusFromStr(const std::string& s) {
    if (s == "valid")   return TokenStatus::Valid;
    if (s == "expired") return TokenStatus::Expired;
    if (s == "banned")  return TokenStatus::Banned;
    return TokenStatus::Unknown;
}

inline std::string SerializeAccount(const AccountEntry& a) {
    nlohmann::json j = {
        {"uuid",        a.uuid},
        {"alias",       a.alias},
        {"username",    a.username},
        {"password",    a.password},
        {"fakeHwid",    a.fakeHwid},
        {"status",      StatusToStr(a.status)},
        {"isBanned",    a.isBanned},
        {"lastUsed",    a.lastUsedIso},
        {"createdAt",   a.createdAtIso},
        {"version",     a.version},
    };
    return j.dump();
}

inline bool DeserializeAccount(const std::string& json, AccountEntry& out) {
    try {
        auto j = nlohmann::json::parse(json);
        out.uuid         = j.value("uuid",      "");
        out.alias        = j.value("alias",     "");
        out.username     = j.value("username",  "");
        out.password     = j.value("password",  "");
        out.fakeHwid     = j.value("fakeHwid",  "");
        out.status       = StatusFromStr(j.value("status", "unknown"));
        out.isBanned     = j.value("isBanned",  false);
        out.lastUsedIso  = j.value("lastUsed",  "");
        out.createdAtIso = j.value("createdAt", "");
        out.version      = j.value("version",   1);
        return !out.uuid.empty();
    } catch (...) {
        return false;
    }
}

inline bool SaveAccountToDisk(const AccountEntry& a) {
    std::string plaintext = SerializeAccount(a);
    auto blob = AccountCrypto::Encrypt(plaintext);
    if (blob.empty()) return false;

    auto finalPath = AccountFilePath(a.uuid);
    auto tmpPath   = finalPath;
    tmpPath += ".tmp";

    {
        std::ofstream out(tmpPath, std::ios::binary | std::ios::trunc);
        if (!out) return false;
        out.write(reinterpret_cast<const char*>(blob.data()),
                  static_cast<std::streamsize>(blob.size()));
        if (!out) return false;
    }

    BOOL ok = MoveFileExW(tmpPath.wstring().c_str(),
                          finalPath.wstring().c_str(),
                          MOVEFILE_REPLACE_EXISTING);
    if (!ok) {
        std::error_code ec;
        std::filesystem::remove(tmpPath, ec);
        return false;
    }
    return true;
}

inline bool LoadAccountFromDisk(const std::filesystem::path& path,
                                AccountEntry& out)
{
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    std::vector<uint8_t> blob((std::istreambuf_iterator<char>(in)),
                              std::istreambuf_iterator<char>());
    std::string plaintext = AccountCrypto::Decrypt(blob);
    if (plaintext.empty()) return false;
    return DeserializeAccount(plaintext, out);
}

inline bool DeleteAccountFromDisk(const std::string& uuid) {
    std::error_code ec;
    return std::filesystem::remove(AccountFilePath(uuid), ec);
}

inline void ReloadAllFromDisk() {
    std::vector<AccountEntry> next;
    auto dir = AccountsDir();
    std::error_code ec;
    if (std::filesystem::exists(dir, ec)) {
        for (auto& entry : std::filesystem::directory_iterator(dir, ec)) {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".acct") continue;
            AccountEntry a;
            if (LoadAccountFromDisk(entry.path(), a)) {
                next.push_back(std::move(a));
            }

        }
    }

    std::sort(next.begin(), next.end(),
        [](const AccountEntry& a, const AccountEntry& b) {
            return a.alias < b.alias;
        });

    std::lock_guard<std::mutex> lk(g_mutex);
    g_accounts = std::move(next);
    if (g_selectedIndex >= static_cast<int>(g_accounts.size()))
        g_selectedIndex = -1;
}

inline void Initialize() {
    if (g_initialized) return;
    g_initialized = true;
    ReloadAllFromDisk();

}

inline bool HasAccountForUsername(const std::string& username) {
    std::lock_guard<std::mutex> lk(g_mutex);
    for (const auto& a : g_accounts) {
        if (a.username == username) return true;
    }
    return false;
}

inline std::string AddAccount(const std::string& username,
                              const std::string& password,
                              const std::string& alias = "")
{
    AccountEntry a;
    a.uuid         = AccountCrypto::NewUuid();
    a.alias        = alias.empty() ? username : alias;
    a.username     = username;
    a.password     = password;
    a.fakeHwid     = AccountCrypto::NewUuid();
    a.status       = TokenStatus::Unknown;
    a.isBanned     = false;
    a.createdAtIso = NowIsoUtc();
    a.lastUsedIso  = "";

    if (!SaveAccountToDisk(a)) return {};
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_accounts.push_back(a);
        std::sort(g_accounts.begin(), g_accounts.end(),
            [](const AccountEntry& x, const AccountEntry& y) {
                return x.alias < y.alias;
            });
    }
    return a.uuid;
}

inline bool RenameAlias(const std::string& uuid, const std::string& newAlias) {
    AccountEntry copy;
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        auto it = std::find_if(g_accounts.begin(), g_accounts.end(),
            [&](const AccountEntry& a) { return a.uuid == uuid; });
        if (it == g_accounts.end()) return false;
        it->alias = newAlias;
        copy = *it;
    }
    return SaveAccountToDisk(copy);
}

inline bool DeleteAccount(const std::string& uuid) {
    bool removed = false;
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        auto it = std::remove_if(g_accounts.begin(), g_accounts.end(),
            [&](const AccountEntry& a) { return a.uuid == uuid; });
        if (it != g_accounts.end()) {
            g_accounts.erase(it, g_accounts.end());
            removed = true;
        }
    }
    if (removed) DeleteAccountFromDisk(uuid);
    return removed;
}

inline bool MarkStatus(const std::string& uuid, TokenStatus s, bool banned) {
    AccountEntry copy;
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        auto it = std::find_if(g_accounts.begin(), g_accounts.end(),
            [&](const AccountEntry& a) { return a.uuid == uuid; });
        if (it == g_accounts.end()) return false;
        it->status   = s;
        it->isBanned = banned;
        copy = *it;
    }
    return SaveAccountToDisk(copy);
}

inline bool MarkUsedNow(const std::string& uuid) {
    AccountEntry copy;
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        auto it = std::find_if(g_accounts.begin(), g_accounts.end(),
            [&](const AccountEntry& a) { return a.uuid == uuid; });
        if (it == g_accounts.end()) return false;
        it->lastUsedIso = NowIsoUtc();
        copy = *it;
    }
    return SaveAccountToDisk(copy);
}

inline bool IsMultiboxLaunch() {
    return DllAuth::g_launchMode == "empty" || DllAuth::g_launchMode == "auto";
}
inline bool IsEmptyMode() { return DllAuth::g_launchMode == "empty"; }
inline bool IsAutoMode()  { return DllAuth::g_launchMode == "auto"; }

namespace ui {

inline ImVec4 StatusColor(TokenStatus s, bool banned) {
    if (banned)                       return ImVec4(0.95f, 0.30f, 0.30f, 1.0f);
    switch (s) {
        case TokenStatus::Valid:   return ImVec4(0.40f, 0.85f, 0.45f, 1.0f);
        case TokenStatus::Expired: return ImVec4(0.95f, 0.55f, 0.20f, 1.0f);
        case TokenStatus::Banned:  return ImVec4(0.95f, 0.30f, 0.30f, 1.0f);
        default:                   return ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
    }
}
inline const char* StatusLabel(TokenStatus s, bool banned) {
    if (banned) return "Banned";
    switch (s) {
        case TokenStatus::Valid:   return "Valid";
        case TokenStatus::Expired: return "Expired";
        case TokenStatus::Banned:  return "Banned";
        default:                   return "Unknown";
    }
}

inline void DrawLeftPane(float width) {
    ImGui::BeginChild("##acct_left", ImVec2(width, 0), true);

    ImGui::TextDisabled("Saved accounts (%zu)", g_accounts.size());
    ImGui::Separator();

    {
        std::lock_guard<std::mutex> lk(g_mutex);
        for (size_t i = 0; i < g_accounts.size(); ++i) {
            const auto& a = g_accounts[i];
            ImGui::PushID(static_cast<int>(i));

            ImVec2 cursor = ImGui::GetCursorScreenPos();
            ImVec4 col    = StatusColor(a.status, a.isBanned);
            ImGui::GetWindowDrawList()->AddCircleFilled(
                ImVec2(cursor.x + 6, cursor.y + ImGui::GetTextLineHeight() * 0.5f),
                4.0f,
                ImGui::ColorConvertFloat4ToU32(col));
            ImGui::Dummy(ImVec2(14, 0));
            ImGui::SameLine();

            bool selected = (g_selectedIndex == static_cast<int>(i));

            if (ImGui::Selectable(a.alias.c_str(), selected, 0))
            {
                g_selectedIndex = static_cast<int>(i);
            }
            ImGui::PopID();
        }
    }

    ImGui::EndChild();
}

inline void DrawRightPane() {
    ImGui::BeginChild("##acct_right", ImVec2(0, 0), true);

    AccountEntry sel;
    bool haveSelection = false;
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        if (g_selectedIndex >= 0 &&
            g_selectedIndex < static_cast<int>(g_accounts.size()))
        {
            sel = g_accounts[g_selectedIndex];
            haveSelection = true;
        }
    }

    ImGui::SeparatorText("Session");
    {
        const float w = ImGui::GetContentRegionAvail().x;
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.30f, 0.10f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.40f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.80f, 0.45f, 0.20f, 1.0f));
        if (ImGui::Button("Force Logout (return to login screen)", ImVec2(w, 0))) {
            if (MultiboxMode::LogoutCurrent()) {
                ShowNotification("Accounts",
                    "Logged out — game will reload to login screen.",
                    NotificationType::Info, 4.0f);
            } else {
                ShowNotification("Logout Failed",
                    "Game function pointers not resolved. Inject "
                    "again after entering a world.",
                    NotificationType::Error, 5.0f);
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::TextDisabled(
            "Pixel Worlds auto-signs you in via Steam. Use this to\n"
            "reach the native login screen. To capture an account:\n"
            "  1. Click Force Logout\n"
            "  2. Open the Debug expander below, flip mode to 'empty'\n"
            "  3. Log into the account through the game's login UI\n"
            "  4. Capture popup will fire on success.");
    }

    ImGui::Spacing();

    ImGui::SeparatorText("Capture");
    {
        bool canCapture = IsMultiboxLaunch();
        ImGui::BeginDisabled(!canCapture);
        if (ImGui::Button("Add Current Account",
            ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {

            MultiboxMode::CapturedCreds c;
            if (MultiboxMode::TakeArmedCapture(c)) {

                g_capturedForPopup = c;
                g_popupOpenRequested = true;
            } else {
                ShowNotification("Accounts",
                    "Log into a Pixel Worlds account first — capture "
                    "will fire automatically once the server confirms.",
                    NotificationType::Info, 5.0f);
            }
        }
        ImGui::EndDisabled();
        if (!canCapture && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip(
                "Available only on instances launched via the loader's\n"
                "Multibox tab. Normal injects do not capture credentials\n"
                "to keep your account info private.");
        }
    }

    ImGui::Spacing();

    ImGui::SeparatorText("Account details");
    if (!haveSelection) {
        ImGui::TextDisabled("No account selected.");
    } else {

        ImGui::TextUnformatted("Status:");
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, StatusColor(sel.status, sel.isBanned));
        ImGui::TextUnformatted(StatusLabel(sel.status, sel.isBanned));
        ImGui::PopStyleColor();

        ImGui::Text("Alias: %s", sel.alias.c_str());
        ImGui::Text("Fake HWID: %s",
            sel.fakeHwid.empty() ? "(none)" : sel.fakeHwid.c_str());
        ImGui::Text("Last used: %s",
            sel.lastUsedIso.empty() ? "never" : sel.lastUsedIso.c_str());
        ImGui::Text("Created: %s", sel.createdAtIso.c_str());

        ImGui::Spacing();

        static bool s_useFakeHwid = true;
        ImGui::Checkbox("Use fake HWID for this login", &s_useFakeHwid);
        if (!IsMultiboxLaunch()) {
            ImGui::SameLine();
            ImGui::TextDisabled("(HWID swap requires multibox launch)");
        }
        if (ImGui::Button("Login", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {

            if (MultiboxMode::CurrentIdState() != 0) {
                g_logoutFirstPopupOpen = true;
                g_logoutFirstPopupAcct = sel.uuid;
            } else {
                if (MultiboxMode::LoginAs(sel.username, sel.password)) {
                    MarkUsedNow(sel.uuid);
                    ShowNotification("Accounts",
                        std::string("Logging in as ") + sel.alias + "...",
                        NotificationType::Info, 4.0f);
                } else {
                    ShowNotification("Login Failed",
                        "Game function pointers not resolved. Are you "
                        "in a world / past the splash?",
                        NotificationType::Error, 5.0f);
                }
            }
        }

        ImGui::Spacing();

        static char s_aliasBuf[64];
        static std::string s_aliasUuid;
        if (s_aliasUuid != sel.uuid) {

            s_aliasUuid = sel.uuid;
            strncpy_s(s_aliasBuf, sel.alias.c_str(), _TRUNCATE);
        }
        ImGui::InputTextWithHint("##acct_alias_edit", "Alias",
            s_aliasBuf, sizeof(s_aliasBuf));
        ImGui::SameLine();
        if (ImGui::Button("Save alias")) {
            std::string newAlias = s_aliasBuf;
            if (!newAlias.empty() && newAlias != sel.alias) {
                if (RenameAlias(sel.uuid, newAlias)) {
                    ShowNotification("Accounts",
                        "Alias updated.",
                        NotificationType::Success, 3.0f);
                }
            }
        }

        ImGui::Spacing();

        if (ImGui::Button("Logout (current session)",
            ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            if (MultiboxMode::LogoutCurrent()) {
                ShowNotification("Accounts",
                    "Logged out — game will reload.",
                    NotificationType::Info, 4.0f);
            } else {
                ShowNotification("Logout Failed",
                    "Game function pointers not resolved.",
                    NotificationType::Error, 4.0f);
            }
        }

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.20f, 0.20f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.80f, 0.30f, 0.30f, 1.0f));
        if (ImGui::Button("Delete this account",
            ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {

            g_pendingDeleteIndex = g_selectedIndex;
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::Button("Refresh from disk")) {
        ReloadAllFromDisk();
    }
    ImGui::SameLine();
    ImGui::TextDisabled("Mode: %s", DllAuth::g_launchMode.c_str());

    ImGui::EndChild();
}

inline void ProcessPendingDelete() {
    if (g_pendingDeleteIndex < 0) return;
    std::string uuid;
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        if (g_pendingDeleteIndex < static_cast<int>(g_accounts.size())) {
            uuid = g_accounts[g_pendingDeleteIndex].uuid;
        }
    }
    g_pendingDeleteIndex = -1;
    if (!uuid.empty()) {
        if (DeleteAccount(uuid)) {
            g_selectedIndex = -1;
            ShowNotification("Accounts",
                "Account deleted.",
                NotificationType::Success, 3.0f);
        }
    }
}

inline void PollForCapture() {
    if (!IsMultiboxLaunch()) return;
    if (g_popupOpenRequested) return;
    MultiboxMode::CapturedCreds c;
    if (MultiboxMode::TakeArmedCapture(c)) {

        if (HasAccountForUsername(c.username)) return;
        g_capturedForPopup   = std::move(c);
        g_popupOpenRequested = true;
    }
}

inline void DrawCapturePopup() {
    static char s_aliasBuf[64] = {};
    static bool s_aliasSeeded  = false;

    if (g_popupOpenRequested) {
        ImGui::OpenPopup("Save Pixel Worlds account?");
        g_popupOpenRequested = false;

        s_aliasSeeded = false;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 0), ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Save Pixel Worlds account?", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        if (!s_aliasSeeded) {

            strncpy_s(s_aliasBuf, g_capturedForPopup.username.c_str(), _TRUNCATE);
            s_aliasSeeded = true;
        }

        ImGui::TextWrapped(
            "You just logged into a Pixel Worlds account on a multibox "
            "instance. Save these credentials locally so you can "
            "auto-login this account from the loader's Multibox tab "
            "without typing the password again?");

        ImGui::Spacing();
        ImGui::SeparatorText("Privacy");
        ImGui::BulletText("File path: %%APPDATA%%\\ZMod\\Accounts\\<uuid>.acct");
        ImGui::BulletText("Encryption: AES-256-GCM, key tied to THIS PC only");
        ImGui::BulletText("Network: NEVER sent anywhere — no servers, no analytics");
        ImGui::BulletText("Other PCs: file is unreadable on a different machine");
        ImGui::BulletText("Other Windows users on this PC: cannot read the file");
        ImGui::Spacing();

        ImGui::SeparatorText("Account");
        ImGui::Text("Username: %s", g_capturedForPopup.username.c_str());
        ImGui::Text("Password: %s",
            std::string(g_capturedForPopup.password.size(), '*').c_str());
        ImGui::InputTextWithHint("Alias", "Friendly name shown in list",
            s_aliasBuf, sizeof(s_aliasBuf));

        ImGui::Spacing();

        const float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.20f, 0.55f, 0.30f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.70f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.30f, 0.80f, 0.40f, 1.0f));
        if (ImGui::Button("Save locally", ImVec2(w, 0))) {
            std::string alias = s_aliasBuf;
            if (alias.empty()) alias = g_capturedForPopup.username;
            std::string uuid = AddAccount(g_capturedForPopup.username,
                                          g_capturedForPopup.password,
                                          alias);
            if (!uuid.empty()) {
                ShowNotification("Accounts",
                    "Saved locally as " + alias,
                    NotificationType::Success, 4.0f);
                MarkStatus(uuid, TokenStatus::Valid, false);
                MarkUsedNow(uuid);
            } else {
                ShowNotification("Save Failed",
                    "Could not write encrypted file. Disk full or "
                    "ACL issue?",
                    NotificationType::Error, 5.0f);
            }

            g_capturedForPopup = {};
            s_aliasBuf[0] = 0;
            s_aliasSeeded = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (ImGui::Button("Cancel (don't save)", ImVec2(w, 0))) {

            g_capturedForPopup = {};
            s_aliasBuf[0] = 0;
            s_aliasSeeded = false;
            MultiboxMode::DiscardCapture();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

inline void DrawLogoutFirstPopup() {
    if (g_logoutFirstPopupOpen) {
        ImGui::OpenPopup("Already logged in");
        g_logoutFirstPopupOpen = false;
    }

    if (ImGui::BeginPopupModal("Already logged in", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::TextWrapped(
            "You are already logged into a Pixel Worlds account. "
            "Log out first to switch accounts cleanly — silently "
            "overwriting can land you in the wrong character / world.");
        ImGui::Spacing();

        const float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
        if (ImGui::Button("Logout & switch", ImVec2(w, 0))) {

            std::string uuid = g_logoutFirstPopupAcct;
            AccountEntry a;
            bool found = false;
            {
                std::lock_guard<std::mutex> lk(g_mutex);
                for (auto& it : g_accounts) {
                    if (it.uuid == uuid) { a = it; found = true; break; }
                }
            }
            if (found) {
                MultiboxMode::LogoutCurrent();
                if (MultiboxMode::LoginAs(a.username, a.password)) {
                    MarkUsedNow(a.uuid);
                    ShowNotification("Accounts",
                        std::string("Switching to ") + a.alias + "...",
                        NotificationType::Info, 4.0f);
                }
            }
            g_logoutFirstPopupAcct.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(w, 0))) {
            g_logoutFirstPopupAcct.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

inline void DrawDebugModeFlip() {
    if (!ImGui::CollapsingHeader("Debug — runtime mode flip")) return;
    ImGui::TextDisabled(
        "Use ONLY for testing the capture popup. In production the\n"
        "mode is set by the loader's Multibox tab via the handshake\n"
        "mapping; flipping it here does NOT spawn a real instance.");
    ImGui::Spacing();

    const char* modes[] = { "normal", "empty", "auto" };
    int current = 0;
    if (DllAuth::g_launchMode == "empty") current = 1;
    else if (DllAuth::g_launchMode == "auto") current = 2;
    if (ImGui::Combo("Force mode", &current, modes, 3)) {
        DllAuth::g_launchMode = modes[current];
        ShowNotification("Multibox",
            std::string("Forced mode -> ") + modes[current],
            NotificationType::Info, 3.0f);
    }
}

}

inline void DrawAccountTab() {
    Initialize();

    ui::PollForCapture();

    const float total = ImGui::GetContentRegionAvail().x;
    const float left  = total * 0.40f;

    ui::DrawLeftPane(left);
    ImGui::SameLine();
    ui::DrawRightPane();

    ui::ProcessPendingDelete();

    ui::DrawCapturePopup();
    ui::DrawLogoutFirstPopup();

    ui::DrawDebugModeFlip();
}

}

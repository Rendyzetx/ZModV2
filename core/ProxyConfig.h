#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <system_error>
#include <windows.h>
#include <shlobj.h>

namespace ProxyCfg {

    inline bool        g_enabled = false;
    inline std::string g_host;
    inline int         g_port    = 8080;
    inline bool        g_useAuth = false;
    inline std::string g_user;
    inline std::string g_pass;
    inline int         g_scheme  = 0;
    inline bool        g_tunnelGame = false;

    inline std::string FilePath() {
        char buf[MAX_PATH] = {};
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buf))) {
            std::string p = buf;
            p += "\\ZMod";
            std::error_code ec;
            std::filesystem::create_directories(p, ec);
            return p + "\\proxy.cfg";
        }
        return "proxy.cfg";
    }

    inline void LoadFromDisk() {
        std::ifstream f(FilePath());
        if (!f) return;
        std::string line;
        while (std::getline(f, line)) {
            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string k = line.substr(0, eq);
            std::string v = line.substr(eq + 1);
            while (!v.empty() && (v.back() == '\r' || v.back() == '\n')) v.pop_back();
            if      (k == "enabled") g_enabled = (v == "1" || v == "true");
            else if (k == "host")    g_host = v;
            else if (k == "port")    { try { g_port = std::stoi(v); } catch (...) {} }
            else if (k == "useauth") g_useAuth = (v == "1" || v == "true");
            else if (k == "user")    g_user = v;
            else if (k == "pass")    g_pass = v;
            else if (k == "scheme")  { try { g_scheme = std::stoi(v); } catch (...) {} }
            else if (k == "tunnelgame") g_tunnelGame = (v == "1" || v == "true");
        }
    }

    inline void SaveToDisk() {
        std::ofstream f(FilePath(), std::ios::trunc);
        if (!f) return;
        f << "enabled=" << (g_enabled ? 1 : 0) << "\n"
          << "host="    << g_host              << "\n"
          << "port="    << g_port              << "\n"
          << "useauth=" << (g_useAuth ? 1 : 0) << "\n"
          << "user="    << g_user              << "\n"
          << "pass="    << g_pass              << "\n"
          << "scheme="  << g_scheme            << "\n"
          << "tunnelgame=" << (g_tunnelGame ? 1 : 0) << "\n";
    }

    inline std::wstring Widen(const std::string& s) {
        return std::wstring(s.begin(), s.end());
    }
    inline std::wstring ProxyWide() {
        return Widen(g_host + ":" + std::to_string(g_port));
    }

}

#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <fstream>
#include <cctype>
#include "ProxyConfig.h"

namespace ProxyPool {

    struct Entry {
        int         scheme    = 1;
        std::string host;
        int         port      = 1080;
        std::string user, pass;
        bool        useAuth   = false;

        int         status    = 0;
        int         latencyMs = -1;
        std::string display;
    };

    inline std::mutex          g_mtx;
    inline std::vector<Entry>  g_game;
    inline std::vector<Entry>  g_auth;
    inline int                 g_gameMode = 0;
    inline int                 g_authMode = 0;
    inline size_t              g_gameRot  = 0;
    inline size_t              g_authRot  = 0;

    inline std::string Canonical(const Entry& e, bool maskPass) {
        std::string s = (e.scheme == 1 ? "socks5://" : "http://");
        if (e.useAuth && !e.user.empty()) {
            s += e.user;
            s += ":";
            s += maskPass ? std::string(e.pass.empty() ? 0 : 3, '*') : e.pass;
            s += "@";
        }
        s += e.host + ":" + std::to_string(e.port);
        return s;
    }
    inline std::string Serialize(const Entry& e) { return Canonical(e, false); }

    inline bool Parse(const std::string& raw, Entry& e) {

        auto isWs = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
        size_t b = 0, en = raw.size();
        while (b < en && isWs(raw[b])) ++b;
        while (en > b && isWs(raw[en - 1])) --en;
        std::string s = raw.substr(b, en - b);
        if (s.empty() || s[0] == '#' || s[0] == ';') return false;

        e = Entry{};

        std::string rest = s;
        size_t sch = s.find("://");
        if (sch != std::string::npos) {
            std::string scheme = s.substr(0, sch);
            for (char& c : scheme) c = (char)std::tolower((unsigned char)c);
            if      (scheme == "http" || scheme == "https" || scheme == "http-connect" || scheme == "connect") e.scheme = 0;
            else    e.scheme = 1;
            rest = s.substr(sch + 3);
        } else {
            e.scheme = 1;
        }

        std::string host, portStr;
        size_t at = rest.rfind('@');
        if (at != std::string::npos) {

            std::string userinfo = rest.substr(0, at);
            std::string hostport = rest.substr(at + 1);
            size_t colon = hostport.rfind(':');
            if (colon == std::string::npos) return false;
            host = hostport.substr(0, colon);
            portStr = hostport.substr(colon + 1);
            size_t uc = userinfo.find(':');
            if (uc != std::string::npos) { e.user = userinfo.substr(0, uc); e.pass = userinfo.substr(uc + 1); }
            else                          e.user = userinfo;
            e.useAuth = !e.user.empty();
        } else {

            std::vector<std::string> parts;
            std::string cur;
            for (char c : rest) { if (c == ':') { parts.push_back(cur); cur.clear(); } else cur.push_back(c); }
            parts.push_back(cur);
            if (parts.size() < 2) return false;
            host = parts[0];
            portStr = parts[1];
            if (parts.size() >= 4) {
                e.user = parts[2];
                std::string p = parts[3];
                for (size_t i = 4; i < parts.size(); ++i) { p += ":"; p += parts[i]; }
                e.pass = p;
                e.useAuth = !e.user.empty();
            } else if (parts.size() == 3) {
                e.user = parts[2];
                e.useAuth = !e.user.empty();
            }
        }

        if (host.empty() || portStr.empty()) return false;
        int p = 0;
        try { p = std::stoi(portStr); } catch (...) { return false; }
        if (p < 1 || p > 65535) return false;
        e.host = host;
        e.port = p;
        e.display = Canonical(e, true);
        return true;
    }

    inline int ImportInto(std::vector<Entry>& v, const std::string& blob) {
        std::lock_guard<std::mutex> lk(g_mtx);
        int added = 0;
        std::string tok;
        auto flush = [&]() {
            if (tok.empty()) return;
            Entry e;
            if (Parse(tok, e)) {
                bool dup = false;
                for (const Entry& x : v)
                    if (x.host == e.host && x.port == e.port && x.scheme == e.scheme) { dup = true; break; }
                if (!dup) { v.push_back(e); ++added; }
            }
            tok.clear();
        };
        for (char c : blob) {
            if (c == '\n' || c == '\r' || c == ',' || c == ';' || c == ' ' || c == '\t') flush();
            else tok.push_back(c);
        }
        flush();
        return added;
    }
    inline int  ImportGame(const std::string& blob) { return ImportInto(g_game, blob); }
    inline int  ImportAuth(const std::string& blob) { return ImportInto(g_auth, blob); }
    inline void ClearGame() { std::lock_guard<std::mutex> lk(g_mtx); g_game.clear(); g_gameRot = 0; }
    inline void ClearAuth() { std::lock_guard<std::mutex> lk(g_mtx); g_auth.clear(); g_authRot = 0; }
    inline int  CountGame() { std::lock_guard<std::mutex> lk(g_mtx); return (int)g_game.size(); }
    inline int  CountAuth() { std::lock_guard<std::mutex> lk(g_mtx); return (int)g_auth.size(); }

    inline bool Pick(std::vector<Entry>& v, int mode, size_t& rot, Entry& out) {
        std::lock_guard<std::mutex> lk(g_mtx);
        if (v.empty()) return false;
        std::vector<size_t> idx;
        for (size_t i = 0; i < v.size(); ++i) if (v[i].status != 2) idx.push_back(i);
        if (idx.empty()) for (size_t i = 0; i < v.size(); ++i) idx.push_back(i);
        size_t pick = (mode == 0) ? idx.front() : idx[(rot++) % idx.size()];
        out = v[pick];
        return true;
    }
    inline bool NextGame(Entry& out) { return Pick(g_game, g_gameMode, g_gameRot, out); }
    inline bool NextAuth(Entry& out) { return Pick(g_auth, g_authMode, g_authRot, out); }

    inline std::string FilePath() {
        std::string p = ProxyCfg::FilePath();
        size_t slash = p.find_last_of("\\/");
        if (slash != std::string::npos) return p.substr(0, slash + 1) + "proxies.cfg";
        return "proxies.cfg";
    }
    inline void SaveToDisk() {
        std::lock_guard<std::mutex> lk(g_mtx);
        std::ofstream f(FilePath(), std::ios::trunc);
        if (!f) return;
        f << "gamemode=" << g_gameMode << "\n";
        f << "authmode=" << g_authMode << "\n";
        for (const Entry& e : g_game) f << "game=" << Serialize(e) << "\n";
        for (const Entry& e : g_auth) f << "auth=" << Serialize(e) << "\n";
    }
    inline void LoadFromDisk() {
        std::lock_guard<std::mutex> lk(g_mtx);
        std::ifstream f(FilePath());
        if (!f) return;
        g_game.clear(); g_auth.clear(); g_gameRot = 0; g_authRot = 0;
        std::string line;
        while (std::getline(f, line)) {
            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string k = line.substr(0, eq), v = line.substr(eq + 1);
            while (!v.empty() && (v.back() == '\r' || v.back() == '\n')) v.pop_back();
            if      (k == "gamemode") { try { g_gameMode = std::stoi(v); } catch (...) {} }
            else if (k == "authmode") { try { g_authMode = std::stoi(v); } catch (...) {} }
            else if (k == "game")     { Entry e; if (Parse(v, e)) g_game.push_back(e); }
            else if (k == "auth")     { Entry e; if (Parse(v, e)) g_auth.push_back(e); }
        }
    }
}

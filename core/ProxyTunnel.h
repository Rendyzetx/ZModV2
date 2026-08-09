#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include "ProxyConfig.h"
#include "ProxyPool.h"

#pragma comment(lib, "Ws2_32.lib")

namespace ProxyTunnel {

    enum { SCHEME_HTTP = 0, SCHEME_SOCKS5 = 1 };

    inline std::atomic<bool> g_active{ false };
    inline std::atomic<bool> g_verbose{ false };

    inline SOCKET   g_listener   = INVALID_SOCKET;
    inline u_short  g_listenPort = 0;
    inline std::atomic<bool> g_stop{ false };
    inline std::thread g_acceptThread;

    inline std::mutex g_mapMtx;
    inline std::unordered_map<u_short, sockaddr_in> g_targetByPort;

    inline thread_local bool t_bypass = false;

    typedef int (WSAAPI* tConnect)(SOCKET, const sockaddr*, int);
    typedef int (WSAAPI* tWSAConnect)(SOCKET, const sockaddr*, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS);
    inline tConnect    oConnect    = nullptr;
    inline tWSAConnect oWSAConnect = nullptr;

    inline void Log(const std::string& s) { if (g_verbose.load()) std::cout << "[ProxyTunnel] " << s << "\n"; }

    inline bool IsLoopback(uint32_t ipBE) { return (ntohl(ipBE) >> 24) == 127; }
    inline bool IsPrivate(uint32_t ipBE) {
        uint32_t ip = ntohl(ipBE);
        uint8_t a = (ip >> 24) & 0xFF, b = (ip >> 16) & 0xFF;
        if (a == 10) return true;
        if (a == 172 && b >= 16 && b <= 31) return true;
        if (a == 192 && b == 168) return true;
        if (a == 169 && b == 254) return true;
        if (a == 0) return true;
        return false;
    }

    inline bool ShouldTunnel(const sockaddr* name) {
        if (!g_active.load() || t_bypass || !name) return false;
        if (name->sa_family != AF_INET) return false;
        const sockaddr_in* in = reinterpret_cast<const sockaddr_in*>(name);
        uint32_t ip = in->sin_addr.s_addr;
        if (IsLoopback(ip) || IsPrivate(ip)) return false;
        return true;
    }

    inline bool SendAll(SOCKET s, const char* buf, int len) {
        int sent = 0;
        while (sent < len) { int n = send(s, buf + sent, len - sent, 0); if (n <= 0) return false; sent += n; }
        return true;
    }
    inline bool RecvExact(SOCKET s, char* buf, int len) {
        int got = 0;
        while (got < len) { int n = recv(s, buf + got, len - got, 0); if (n <= 0) return false; got += n; }
        return true;
    }

    inline std::string Base64(const std::string& in) {
        static const char* T = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string out;
        size_t i = 0;
        while (i + 3 <= in.size()) {
            uint32_t n = (uint8_t)in[i] << 16 | (uint8_t)in[i + 1] << 8 | (uint8_t)in[i + 2];
            out += T[(n >> 18) & 63]; out += T[(n >> 12) & 63]; out += T[(n >> 6) & 63]; out += T[n & 63];
            i += 3;
        }
        if (size_t rem = in.size() - i) {
            uint32_t n = (uint8_t)in[i] << 16 | (rem > 1 ? (uint8_t)in[i + 1] << 8 : 0);
            out += T[(n >> 18) & 63]; out += T[(n >> 12) & 63];
            out += rem == 2 ? T[(n >> 6) & 63] : '=';
            out += '=';
        }
        return out;
    }

    inline bool Socks5Connect(SOCKET ps, uint32_t targetIpBE, uint16_t targetPortBE,
                              const std::string& user, const std::string& pass, bool useAuth) {
        const bool wantsAuth = useAuth && !user.empty();
        if (wantsAuth) { const char g[] = { 0x05, 0x02, 0x00, 0x02 }; if (!SendAll(ps, g, 4)) return false; }
        else           { const char g[] = { 0x05, 0x01, 0x00 };       if (!SendAll(ps, g, 3)) return false; }

        char resp[2];
        if (!RecvExact(ps, resp, 2) || resp[0] != 0x05) return false;
        if ((uint8_t)resp[1] == 0x02) {

            const std::string& u = user; const std::string& p = pass;
            if (u.size() > 255 || p.size() > 255) return false;
            std::string a; a.push_back(0x01);
            a.push_back((char)u.size()); a += u;
            a.push_back((char)p.size()); a += p;
            if (!SendAll(ps, a.data(), (int)a.size())) return false;
            char ar[2];
            if (!RecvExact(ps, ar, 2) || ar[0] != 0x01 || ar[1] != 0x00) return false;
        } else if ((uint8_t)resp[1] != 0x00) {
            return false;
        }

        char req[10];
        req[0] = 0x05; req[1] = 0x01; req[2] = 0x00; req[3] = 0x01;
        memcpy(req + 4, &targetIpBE, 4);
        memcpy(req + 8, &targetPortBE, 2);
        if (!SendAll(ps, req, 10)) return false;

        char hdr[4];
        if (!RecvExact(ps, hdr, 4) || hdr[0] != 0x05 || hdr[1] != 0x00) return false;
        int addrLen;
        switch ((uint8_t)hdr[3]) {
            case 0x01: addrLen = 4; break;
            case 0x04: addrLen = 16; break;
            case 0x03: { char l; if (!RecvExact(ps, &l, 1)) return false; addrLen = (uint8_t)l; break; }
            default: return false;
        }
        std::vector<char> rest(addrLen + 2);
        return RecvExact(ps, rest.data(), addrLen + 2);
    }

    inline bool HttpConnect(SOCKET ps, uint32_t targetIpBE, uint16_t targetPortBE,
                            const std::string& user, const std::string& pass, bool useAuth) {
        char ips[INET_ADDRSTRLEN] = {};
        in_addr a; a.s_addr = targetIpBE;
        inet_ntop(AF_INET, &a, ips, sizeof(ips));
        std::string target = std::string(ips) + ":" + std::to_string(ntohs(targetPortBE));
        std::string req = "CONNECT " + target + " HTTP/1.1\r\nHost: " + target +
                          "\r\nProxy-Connection: keep-alive\r\n";
        if (useAuth && !user.empty()) {
            req += "Proxy-Authorization: Basic " + Base64(user + ":" + pass) + "\r\n";
        }
        req += "\r\n";
        if (!SendAll(ps, req.data(), (int)req.size())) return false;

        std::string buf;
        char chunk[256];
        for (;;) {
            int n = recv(ps, chunk, sizeof(chunk), 0);
            if (n <= 0) return false;
            buf.append(chunk, n);
            if (buf.find("\r\n\r\n") != std::string::npos) break;
            if (buf.size() > 8192) return false;
        }
        size_t eol = buf.find("\r\n");
        std::string status = eol == std::string::npos ? buf : buf.substr(0, eol);
        return status.find(" 200 ") != std::string::npos || status.find(" 200\r") != std::string::npos
            || (status.size() >= 4 && status.compare(status.size() - 4, 4, " 200") == 0);
    }

    inline SOCKET ConnectToProxy(const std::string& host, int port, int timeoutMs = 0) {
        addrinfo hints{}; hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
        addrinfo* res = nullptr;
        std::string portStr = std::to_string(port);
        if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res) != 0 || !res) return INVALID_SOCKET;
        SOCKET ps = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ps == INVALID_SOCKET) { freeaddrinfo(res); return INVALID_SOCKET; }
        BOOL one = TRUE; setsockopt(ps, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(one));

        int rc;
        if (timeoutMs > 0) {
            u_long nb = 1; ioctlsocket(ps, FIONBIO, &nb);
            t_bypass = true;
            rc = oConnect ? oConnect(ps, res->ai_addr, (int)res->ai_addrlen)
                          : ::connect(ps, res->ai_addr, (int)res->ai_addrlen);
            t_bypass = false;
            if (rc == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) {
                fd_set wf; FD_ZERO(&wf); FD_SET(ps, &wf);
                timeval tv{ timeoutMs / 1000, (timeoutMs % 1000) * 1000 };
                int sel = select(0, nullptr, &wf, nullptr, &tv);
                if (sel > 0) {
                    int so = 0, sl = sizeof(so);
                    getsockopt(ps, SOL_SOCKET, SO_ERROR, (char*)&so, &sl);
                    rc = (so == 0) ? 0 : SOCKET_ERROR;
                } else rc = SOCKET_ERROR;
            }
            u_long bl = 0; ioctlsocket(ps, FIONBIO, &bl);
            DWORD to = (DWORD)timeoutMs;
            setsockopt(ps, SOL_SOCKET, SO_RCVTIMEO, (char*)&to, sizeof(to));
            setsockopt(ps, SOL_SOCKET, SO_SNDTIMEO, (char*)&to, sizeof(to));
        } else {
            t_bypass = true;
            rc = oConnect ? oConnect(ps, res->ai_addr, (int)res->ai_addrlen)
                          : ::connect(ps, res->ai_addr, (int)res->ai_addrlen);
            t_bypass = false;
        }
        freeaddrinfo(res);
        if (rc == SOCKET_ERROR) { closesocket(ps); return INVALID_SOCKET; }
        return ps;
    }

    inline void Pump(SOCKET from, SOCKET to) {
        char buf[16384];
        for (;;) {
            int n = recv(from, buf, sizeof(buf), 0);
            if (n <= 0) break;
            if (!SendAll(to, buf, n)) break;
        }
        shutdown(to, SD_SEND);
    }

    inline void HandleConn(SOCKET gameSock, sockaddr_in target) {

        std::string host, user, pass; int port, scheme; bool useAuth;
        ProxyPool::Entry sel;
        if (ProxyPool::NextGame(sel)) {
            host = sel.host; port = sel.port; scheme = sel.scheme;
            user = sel.user; pass = sel.pass; useAuth = sel.useAuth;
        } else {
            host = ProxyCfg::g_host; port = ProxyCfg::g_port; scheme = ProxyCfg::g_scheme;
            user = ProxyCfg::g_user; pass = ProxyCfg::g_pass; useAuth = ProxyCfg::g_useAuth;
        }

        SOCKET ps = ConnectToProxy(host, port);
        if (ps == INVALID_SOCKET) { Log("proxy connect failed"); closesocket(gameSock); return; }

        bool ok = (scheme == SCHEME_SOCKS5)
                  ? Socks5Connect(ps, target.sin_addr.s_addr, target.sin_port, user, pass, useAuth)
                  : HttpConnect(ps, target.sin_addr.s_addr, target.sin_port, user, pass, useAuth);
        if (!ok) {
            char ips[INET_ADDRSTRLEN] = {}; inet_ntop(AF_INET, &target.sin_addr, ips, sizeof(ips));
            Log(std::string("handshake failed to ") + ips + ":" + std::to_string(ntohs(target.sin_port)));
            closesocket(ps); closesocket(gameSock); return;
        }
        {
            char ips[INET_ADDRSTRLEN] = {}; inet_ntop(AF_INET, &target.sin_addr, ips, sizeof(ips));
            Log(std::string("tunnelled -> ") + ips + ":" + std::to_string(ntohs(target.sin_port)));
        }
        std::thread t1(Pump, gameSock, ps);
        Pump(ps, gameSock);
        t1.join();
        closesocket(ps); closesocket(gameSock);
    }

    inline void AcceptLoop() {
        while (!g_stop.load()) {
            sockaddr_in peer{}; int plen = sizeof(peer);
            SOCKET c = accept(g_listener, (sockaddr*)&peer, &plen);
            if (c == INVALID_SOCKET) break;

            u_short srcPort = peer.sin_port;
            sockaddr_in target{}; bool found = false;
            for (int tries = 0; tries < 40 && !found; ++tries) {
                { std::lock_guard<std::mutex> lk(g_mapMtx);
                  auto it = g_targetByPort.find(srcPort);
                  if (it != g_targetByPort.end()) { target = it->second; g_targetByPort.erase(it); found = true; } }
                if (!found) std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            if (!found) { Log("no target for accepted conn — dropping"); closesocket(c); continue; }
            std::thread(HandleConn, c, target).detach();
        }
    }

    inline bool Start() {
        if (g_listener != INVALID_SOCKET) return true;
        g_stop = false;
        SOCKET l = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (l == INVALID_SOCKET) return false;
        sockaddr_in addr{}; addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); addr.sin_port = 0;
        if (bind(l, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { closesocket(l); return false; }
        int alen = sizeof(addr);
        if (getsockname(l, (sockaddr*)&addr, &alen) == SOCKET_ERROR) { closesocket(l); return false; }
        if (listen(l, 16) == SOCKET_ERROR) { closesocket(l); return false; }
        g_listener   = l;
        g_listenPort = ntohs(addr.sin_port);
        g_acceptThread = std::thread(AcceptLoop);
        g_active = true;
        Log("started, listener 127.0.0.1:" + std::to_string(g_listenPort) +
            " scheme=" + (ProxyCfg::g_scheme == SCHEME_SOCKS5 ? "socks5" : "http"));
        return true;
    }

    inline void Stop() {
        g_active = false;
        g_stop = true;
        if (g_listener != INVALID_SOCKET) { closesocket(g_listener); g_listener = INVALID_SOCKET; }
        if (g_acceptThread.joinable()) g_acceptThread.join();
        { std::lock_guard<std::mutex> lk(g_mapMtx); g_targetByPort.clear(); }
        Log("stopped");
    }

    inline void Redirect(SOCKET s, const sockaddr* name) {
        const sockaddr_in* in = reinterpret_cast<const sockaddr_in*>(name);

        (void)s; (void)in;
    }

    inline int WSAAPI hkConnect(SOCKET s, const sockaddr* name, int namelen) {
        if (!ShouldTunnel(name)) return oConnect(s, name, namelen);
        sockaddr_in realTarget = *reinterpret_cast<const sockaddr_in*>(name);
        sockaddr_in local{}; local.sin_family = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_LOOPBACK); local.sin_port = htons(g_listenPort);
        int rc = oConnect(s, (sockaddr*)&local, sizeof(local));

        sockaddr_in src{}; int slen = sizeof(src);
        if (getsockname(s, (sockaddr*)&src, &slen) == 0) {
            std::lock_guard<std::mutex> lk(g_mapMtx);
            g_targetByPort[src.sin_port] = realTarget;
        }
        return rc;
    }
    inline int WSAAPI hkWSAConnect(SOCKET s, const sockaddr* name, int namelen,
                                   LPWSABUF cd, LPWSABUF ced, LPQOS sq, LPQOS gq) {
        if (!ShouldTunnel(name)) return oWSAConnect(s, name, namelen, cd, ced, sq, gq);
        sockaddr_in realTarget = *reinterpret_cast<const sockaddr_in*>(name);
        sockaddr_in local{}; local.sin_family = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_LOOPBACK); local.sin_port = htons(g_listenPort);
        int rc = oWSAConnect(s, (sockaddr*)&local, sizeof(local), cd, ced, sq, gq);
        sockaddr_in src{}; int slen = sizeof(src);
        if (getsockname(s, (sockaddr*)&src, &slen) == 0) {
            std::lock_guard<std::mutex> lk(g_mapMtx);
            g_targetByPort[src.sin_port] = realTarget;
        }
        return rc;
    }

    inline void ApplyConfig() {
        bool haveProxy = ProxyPool::CountGame() > 0 || !ProxyCfg::g_host.empty();
        if (ProxyCfg::g_tunnelGame && haveProxy) {
            if (g_listener == INVALID_SOCKET) Start();
        } else {
            if (g_listener != INVALID_SOCKET) Stop();
        }
    }

    inline std::atomic<bool> g_checking{ false };

    inline bool CheckEntry(const ProxyPool::Entry& e, int& latencyMs) {
        const int TIMEOUT_MS = 6000;
        ULONGLONG t0 = GetTickCount64();
        SOCKET ps = ConnectToProxy(e.host, e.port, TIMEOUT_MS);
        if (ps == INVALID_SOCKET) { latencyMs = -1; return false; }
        uint32_t ip = 0; inet_pton(AF_INET, "1.1.1.1", &ip);
        uint16_t port = htons(443);
        bool ok = (e.scheme == SCHEME_SOCKS5)
                  ? Socks5Connect(ps, ip, port, e.user, e.pass, e.useAuth)
                  : HttpConnect(ps, ip, port, e.user, e.pass, e.useAuth);
        closesocket(ps);
        latencyMs = ok ? (int)(GetTickCount64() - t0) : -1;
        return ok;
    }

    inline void CheckPool(std::vector<ProxyPool::Entry>* which) {
        size_t n;
        { std::lock_guard<std::mutex> lk(ProxyPool::g_mtx); n = which->size(); }
        for (size_t i = 0; i < n; ++i) {
            ProxyPool::Entry snap;
            { std::lock_guard<std::mutex> lk(ProxyPool::g_mtx);
              if (i >= which->size()) break;
              snap = (*which)[i]; }
            int lat = -1;
            bool ok = CheckEntry(snap, lat);
            { std::lock_guard<std::mutex> lk(ProxyPool::g_mtx);
              if (i < which->size() && (*which)[i].host == snap.host
                  && (*which)[i].port == snap.port && (*which)[i].scheme == snap.scheme) {
                  (*which)[i].status    = ok ? 1 : 2;
                  (*which)[i].latencyMs = lat;
              } }
        }
        g_checking = false;
    }
    inline void CheckAllGame() { if (g_checking.exchange(true)) return; std::thread(CheckPool, &ProxyPool::g_game).detach(); }
    inline void CheckAllAuth() { if (g_checking.exchange(true)) return; std::thread(CheckPool, &ProxyPool::g_auth).detach(); }
}

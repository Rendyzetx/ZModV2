

#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <bcrypt.h>
#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <cstdio>

#include "DllAuth.h"

#pragma comment(lib, "Bcrypt.lib")
#ifndef NT_SUCCESS
#define NT_SUCCESS(s) ((s) >= 0)
#endif

namespace MultiboxProfile {

inline std::mutex g_logMutex;

inline std::string LogPath() {
    PWSTR appData = nullptr;
    std::string out = "C:\\multibox.log";
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appData))) {
        wchar_t* w = appData;
        char  buf[MAX_PATH] = {};
        WideCharToMultiByte(CP_UTF8, 0, w, -1, buf, sizeof(buf), nullptr, nullptr);
        std::string base = buf;
        CoTaskMemFree(appData);
        std::string dir = base + "\\ZMod";
        CreateDirectoryA(dir.c_str(), nullptr);
        out = dir + "\\multibox.log";
    }
    return out;
}

inline void Log(const std::string& line) {
    std::lock_guard<std::mutex> lk(g_logMutex);
    static std::string path = LogPath();
    std::ofstream f(path, std::ios::app);
    if (!f) return;
    SYSTEMTIME st; GetLocalTime(&st);
    char ts[32];
    std::snprintf(ts, sizeof(ts), "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    f << ts << line << "\n";
}

typedef Il2CppString* (*t_App_get_persistentDataPath)();

struct ManagedSpanWrapper {
    void*   ptr;
    int32_t len;
    int32_t pad;
};
typedef void (*t_App_get_persistentDataPath_Injected)(ManagedSpanWrapper* outRet);

inline t_App_get_persistentDataPath          g_orig_persistentDataPath      = nullptr;
inline t_App_get_persistentDataPath_Injected g_orig_persistentDataPath_Inj  = nullptr;
inline std::atomic<bool>                     g_hooked{ false };

inline std::string                           g_cachedRedirectStd;
inline Il2CppString*                         g_cachedRedirectIl2 = nullptr;
inline std::wstring                          g_cachedRedirectWide;
inline std::mutex                            g_redirectMutex;

inline std::string Il2CppToStd(Il2CppString* s) {
    if (!s || s->length == 0) return {};
    int sz = WideCharToMultiByte(CP_UTF8, 0, s->chars, s->length,
                                 nullptr, 0, nullptr, nullptr);
    if (sz <= 0) return {};
    std::string out(sz, 0);
    WideCharToMultiByte(CP_UTF8, 0, s->chars, s->length,
                        out.data(), sz, nullptr, nullptr);
    return out;
}

inline std::string ComputeRedirectPath(const std::string& original) {
    if (original.empty()) return original;
    std::string suffix = "_inst" + std::to_string(DllAuth::g_instanceId);
    std::string trimmed = original;
    while (!trimmed.empty() &&
           (trimmed.back() == '\\' || trimmed.back() == '/'))
    {
        trimmed.pop_back();
    }
    return trimmed + suffix;
}

inline bool ShouldRedirect() {
    return DllAuth::g_launchMode == "multibox" && DllAuth::g_instanceId > 0;
}

inline bool EnsureCacheFromUtf8(const std::string& origStd) {
    std::lock_guard<std::mutex> lk(g_redirectMutex);
    if (!g_cachedRedirectStd.empty()) return true;
    if (origStd.empty()) return false;

    g_cachedRedirectStd = ComputeRedirectPath(origStd);

    int wlen = MultiByteToWideChar(CP_UTF8, 0,
        g_cachedRedirectStd.c_str(), -1, nullptr, 0);
    if (wlen > 0) {
        g_cachedRedirectWide.resize(wlen - 1);
        MultiByteToWideChar(CP_UTF8, 0,
            g_cachedRedirectStd.c_str(), -1,
            g_cachedRedirectWide.data(), wlen);
    }

    if (oIl2CppStringNew) {
        g_cachedRedirectIl2 = oIl2CppStringNew(g_cachedRedirectStd.c_str());
    }

    CreateDirectoryA(g_cachedRedirectStd.c_str(), nullptr);

    Log("redirect cache built: original=" + origStd +
        " redirect=" + g_cachedRedirectStd +
        " il2cpp=" + (g_cachedRedirectIl2 ? "ok" : "NULL"));
    return true;
}

inline Il2CppString* H_get_persistentDataPath() {
    Il2CppString* original = g_orig_persistentDataPath
        ? g_orig_persistentDataPath()
        : nullptr;

    if (!ShouldRedirect()) return original;
    if (!original) return nullptr;

    if (!g_cachedRedirectIl2) {
        std::string origStd = Il2CppToStd(original);
        Log("H_get_persistentDataPath: first call, original=" + origStd);
        EnsureCacheFromUtf8(origStd);
    }
    return g_cachedRedirectIl2 ? g_cachedRedirectIl2 : original;
}

inline void H_get_persistentDataPath_Injected(ManagedSpanWrapper* outRet) {
    if (g_orig_persistentDataPath_Inj) {
        g_orig_persistentDataPath_Inj(outRet);
    }
    static std::atomic<bool> s_logged{ false };
    if (!ShouldRedirect()) return;
    if (!outRet || !outRet->ptr || outRet->len <= 0) return;

    bool wasLogged = s_logged.exchange(true);
    if (!wasLogged) {
        std::wstring origWide(reinterpret_cast<wchar_t*>(outRet->ptr),
                              outRet->len);
        int u8len = WideCharToMultiByte(CP_UTF8, 0,
            origWide.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string origStd;
        if (u8len > 0) {
            origStd.resize(u8len - 1);
            WideCharToMultiByte(CP_UTF8, 0,
                origWide.c_str(), -1,
                origStd.data(), u8len, nullptr, nullptr);
        }
        Log("H_get_persistentDataPath_Injected: first call (observe-only),"
            " original=" + origStd);
    }

}

typedef Il2CppString* (*t_SI_get_deviceUniqueIdentifier)();

inline t_SI_get_deviceUniqueIdentifier g_orig_deviceUniqueIdentifier = nullptr;
inline Il2CppString*                   g_cachedFakeDeviceId = nullptr;
inline std::string                     g_cachedFakeDeviceIdStd;
inline std::mutex                      g_hwidMutex;

inline std::atomic<bool>               g_userHwidOverride{ false };
inline std::string                     g_userHwidValue;

inline std::string DeriveFakeDeviceId(const std::string& original, int slot) {
    uint64_t a = 0xcbf29ce484222325ULL;
    uint64_t b = 0xcbf29ce484222325ULL ^ (uint64_t)slot * 0x100000001b3ULL;
    auto mix = [](uint64_t& h, const std::string& s) {
        for (unsigned char c : s) {
            h ^= c;
            h *= 0x100000001b3ULL;
        }
    };
    mix(a, original);
    mix(b, original + "|salt|" + std::to_string(slot));

    char buf[33] = {};
    std::snprintf(buf, sizeof(buf), "%016llx%016llx",
        (unsigned long long)a, (unsigned long long)b);
    return std::string(buf);
}

inline std::string GenerateRandomHwid() {
    BYTE rb[16] = {};
    NTSTATUS s = BCryptGenRandom(nullptr, rb, sizeof(rb),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (s < 0) {

        srand((unsigned)GetTickCount64());
        for (int i = 0; i < 16; ++i) rb[i] = (BYTE)(rand() & 0xFF);
    }
    char buf[40] = {};
    std::snprintf(buf, sizeof(buf),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        rb[0], rb[1], rb[2],  rb[3],  rb[4],  rb[5],  rb[6],  rb[7],
        rb[8], rb[9], rb[10], rb[11], rb[12], rb[13], rb[14], rb[15]);
    return std::string(buf);
}

inline std::string RandomizeHwid() {
    std::lock_guard<std::mutex> lk(g_hwidMutex);
    g_userHwidValue = GenerateRandomHwid();
    g_userHwidOverride.store(true);
    g_cachedFakeDeviceId = nullptr;
    g_cachedFakeDeviceIdStd.clear();
    Log("RandomizeHwid: new value=" + g_userHwidValue);
    return g_userHwidValue;
}

inline bool ShouldFakeHwid() {
    return ShouldRedirect() || g_userHwidOverride.load();
}

inline Il2CppString* H_get_deviceUniqueIdentifier() {
    Il2CppString* original = g_orig_deviceUniqueIdentifier
        ? g_orig_deviceUniqueIdentifier()
        : nullptr;

    if (!ShouldFakeHwid()) return original;
    if (!original) return nullptr;

    std::lock_guard<std::mutex> lk(g_hwidMutex);
    if (!g_cachedFakeDeviceId) {
        std::string origStd = Il2CppToStd(original);
        if (g_userHwidOverride.load() && !g_userHwidValue.empty()) {

            g_cachedFakeDeviceIdStd = g_userHwidValue;
        } else {
            g_cachedFakeDeviceIdStd = DeriveFakeDeviceId(origStd,
                DllAuth::g_instanceId);
        }
        if (oIl2CppStringNew) {
            g_cachedFakeDeviceId = oIl2CppStringNew(g_cachedFakeDeviceIdStd.c_str());
        }
        Log("H_get_deviceUniqueIdentifier: first call, original=" + origStd +
            " fake=" + g_cachedFakeDeviceIdStd);
    }
    return g_cachedFakeDeviceId ? g_cachedFakeDeviceId : original;
}

inline bool _Install(void* target, void* hookFn, void** origFn,
                     const char* desc)
{
    if (!target) {
        Log(std::string("hook skip (no target): ") + desc);
        return false;
    }
    if (MH_CreateHook(target, hookFn, origFn) == MH_OK &&
        MH_EnableHook(target) == MH_OK)
    {
        Log(std::string("hook OK: ") + desc);
        return true;
    }
    Log(std::string("hook FAILED: ") + desc);
    return false;
}

inline bool InstallHook() {
    if (g_hooked.load()) return true;

    Log("InstallHook entry. mode=" + DllAuth::g_launchMode +
        " instanceId=" + std::to_string(DllAuth::g_instanceId));

    auto* appCls = IL2CPP::Class::Find("UnityEngine.Application");
    auto* siCls  = IL2CPP::Class::Find("UnityEngine.SystemInfo");
    if (!appCls && !siCls) {
        Log("InstallHook: both Application and SystemInfo classes missing");
        return false;
    }

    if (appCls) {
        void* mgrPtr = IL2CPP::Class::Utils::GetMethodPointer(
            appCls, "get_persistentDataPath", 0);
        g_orig_persistentDataPath = (t_App_get_persistentDataPath)mgrPtr;
        _Install(mgrPtr,
                 reinterpret_cast<void*>(&H_get_persistentDataPath),
                 reinterpret_cast<void**>(&g_orig_persistentDataPath),
                 "Application.get_persistentDataPath");

        void* injPtr = IL2CPP::Class::Utils::GetMethodPointer(
            appCls, "get_persistentDataPath_Injected", 1);
        g_orig_persistentDataPath_Inj = (t_App_get_persistentDataPath_Injected)injPtr;
        _Install(injPtr,
                 reinterpret_cast<void*>(&H_get_persistentDataPath_Injected),
                 reinterpret_cast<void**>(&g_orig_persistentDataPath_Inj),
                 "Application.get_persistentDataPath_Injected");
    }

    if (siCls) {
        void* devPtr = IL2CPP::Class::Utils::GetMethodPointer(
            siCls, "get_deviceUniqueIdentifier", 0);
        g_orig_deviceUniqueIdentifier = (t_SI_get_deviceUniqueIdentifier)devPtr;
        _Install(devPtr,
                 reinterpret_cast<void*>(&H_get_deviceUniqueIdentifier),
                 reinterpret_cast<void**>(&g_orig_deviceUniqueIdentifier),
                 "SystemInfo.get_deviceUniqueIdentifier");
    }

    g_hooked.store(true);
    Log("InstallHook complete");
    return true;
}

}



#pragma once

#include <Windows.h>
#include <Shlwapi.h>
#include <bcrypt.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <iomanip>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#pragma comment(lib, "Bcrypt.lib")

#include "json.hpp"
#include "IL2CPP_Resolver/Data.hpp"
#include "ManifestFetch.h"
#include "BootState.h"

namespace Manifest {

struct ApiEntry {
    std::string export_name;
    uintptr_t   rva  = 0;
    void*       addr = nullptr;

    std::vector<uintptr_t> candidates;
};

struct MethodEntry {
    std::string class_name;
    std::string name;
    uintptr_t   rva  = 0;
    void*       addr = nullptr;
    int         argc = -1;
    std::string signature;
};

struct FieldEntry {
    std::string class_name;
    std::string name;
    uint32_t    offset = 0;
    std::string type;
};

struct ClassEntry {
    std::string image;
    std::string ns;
    std::string short_name;
    int32_t     type_def_index = -1;
    std::string klass_resolution;
    uintptr_t   klass_ptr_rva = 0;
    void*       cached_klass = nullptr;
};

struct StaticFieldEntry {
    std::string class_name;
    std::string name;
    uint32_t    static_offset = 0;
};

struct State {
    bool      loaded         = false;
    HMODULE   game_assembly  = nullptr;
    uintptr_t module_base    = 0;

    std::string game_version;
    std::string ga_sha256;

    std::unordered_map<std::string, ApiEntry>                api;
    std::unordered_map<std::string, MethodEntry>             methods;
    std::unordered_map<std::string, FieldEntry>              fields;
    std::unordered_map<std::string, ClassEntry>              classes;
    std::unordered_map<std::string, StaticFieldEntry>        static_fields;
    std::unordered_map<std::string, std::vector<std::string>> class_aliases;

    nlohmann::json anticheat;
};

inline State& Get() {
    static State s;
    return s;
}

inline uintptr_t ParseHex(const std::string& s) {
    if (s.empty()) return 0;
    return std::stoull(s, nullptr, 0);
}

inline std::string Sha256File(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) return {};

    DWORD objLen = 0, dataLen = 0;
    ULONG cb = 0;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objLen,  sizeof(objLen),  &cb, 0);
    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,   (PUCHAR)&dataLen, sizeof(dataLen), &cb, 0);

    std::vector<UCHAR> obj(objLen);
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (BCryptCreateHash(hAlg, &hHash, obj.data(), objLen, nullptr, 0, 0) != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return {};
    }

    std::vector<char> buf(1 << 20);
    while (f) {
        f.read(buf.data(), buf.size());
        std::streamsize n = f.gcount();
        if (n > 0) BCryptHashData(hHash, (PUCHAR)buf.data(), (ULONG)n, 0);
    }

    std::vector<UCHAR> digest(dataLen);
    BCryptFinishHash(hHash, digest.data(), dataLen, 0);
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for (auto b : digest) os << std::setw(2) << (int)b;
    return os.str();
}

inline std::unordered_map<std::string, void**>& IL2CppFunctionFieldMap() {
    static std::unordered_map<std::string, void**> m = {
        { "il2cpp_class_from_name",              &IL2CPP::Functions.m_ClassFromName },
        { "il2cpp_class_get_fields",             &IL2CPP::Functions.m_ClassGetFields },
        { "il2cpp_class_get_field_from_name",    &IL2CPP::Functions.m_ClassGetFieldFromName },
        { "il2cpp_class_get_methods",            &IL2CPP::Functions.m_ClassGetMethods },
        { "il2cpp_class_get_method_from_name",   &IL2CPP::Functions.m_ClassGetMethodFromName },
        { "il2cpp_class_get_property_from_name", &IL2CPP::Functions.m_ClassGetPropertyFromName },
        { "il2cpp_class_get_type",               &IL2CPP::Functions.m_ClassGetType },
        { "il2cpp_domain_get",                   &IL2CPP::Functions.m_DomainGet },
        { "il2cpp_domain_get_assemblies",        &IL2CPP::Functions.m_DomainGetAssemblies },
        { "il2cpp_free",                         &IL2CPP::Functions.m_Free },
        { "il2cpp_image_get_class",              &IL2CPP::Functions.m_ImageGetClass },
        { "il2cpp_image_get_class_count",        &IL2CPP::Functions.m_ImageGetClassCount },
        { "il2cpp_resolve_icall",                &IL2CPP::Functions.m_ResolveFunction },
        { "il2cpp_string_new",                   &IL2CPP::Functions.m_StringNew },
        { "il2cpp_thread_attach",                &IL2CPP::Functions.m_ThreadAttach },
        { "il2cpp_thread_detach",                &IL2CPP::Functions.m_ThreadDetach },
        { "il2cpp_type_get_object",              &IL2CPP::Functions.m_TypeGetObject },
        { "il2cpp_object_new",                   &IL2CPP::Functions.m_pObjectNew },
        { "il2cpp_method_get_param_name",        &IL2CPP::Functions.m_MethodGetParamName },
        { "il2cpp_method_get_param",             &IL2CPP::Functions.m_MethodGetParam },
        { "il2cpp_class_from_il2cpp_type",       &IL2CPP::Functions.m_ClassFromIl2cppType },
        { "il2cpp_field_static_get_value",       &IL2CPP::Functions.m_FieldStaticGetValue },
        { "il2cpp_field_static_set_value",       &IL2CPP::Functions.m_FieldStaticSetValue },
    };
    return m;
}

inline bool Init(HMODULE ) {
    State& s = Get();
    if (s.loaded) return true;

    std::string jsonText, fetchErr;
    if (!ManifestFetch::Fetch(jsonText, fetchErr)) {
        std::cout << "[Manifest] Fetch failed: " << fetchErr << std::endl;
        BootState::Fail(fetchErr);
        return false;
    }

    nlohmann::json j;
    try {
        j = nlohmann::json::parse(jsonText);
    } catch (const std::exception& e) {
        std::cout << "[Manifest] Parse error: " << e.what() << std::endl;
        BootState::Fail(std::string("manifest: parse: ") + e.what());
        SecureZeroMemory(jsonText.data(), jsonText.size());
        return false;
    }
    SecureZeroMemory(jsonText.data(), jsonText.size());
    std::cout << "[Manifest] Loaded manifest from worker." << std::endl;

    int schema = j.value("schema_version", 0);
    if (schema != 1) {
        std::cout << "[Manifest] Unsupported schema_version=" << schema << std::endl;
        BootState::Fail("manifest: unsupported schema");
        return false;
    }

    s.game_assembly = GetModuleHandleA("GameAssembly.dll");
    if (!s.game_assembly) {
        std::cout << "[Manifest] GameAssembly.dll not loaded." << std::endl;
        BootState::Fail("manifest: no game module");
        return false;
    }
    s.module_base = reinterpret_cast<uintptr_t>(s.game_assembly);

    if (j.contains("build")) {
        const auto& b = j["build"];
        if (b.contains("game_version_string") && b["game_version_string"].is_string())
            s.game_version = b["game_version_string"].get<std::string>();
        if (b.contains("game_assembly_sha256") && b["game_assembly_sha256"].is_string())
            s.ga_sha256 = b["game_assembly_sha256"].get<std::string>();
        if (b.contains("game_assembly_path") && b["game_assembly_path"].is_string()
            && !s.ga_sha256.empty())
        {

            std::string gaPath = b["game_assembly_path"].get<std::string>();
            std::string actual = Sha256File(gaPath);
            if (actual.empty()) {
                std::cout << "[Manifest] WARN: could not hash " << gaPath
                          << " — SHA guard skipped." << std::endl;
            } else if (actual != s.ga_sha256) {
                std::cout << "[Manifest] ERROR: GameAssembly.dll SHA mismatch.\n"
                          << "  expected: " << s.ga_sha256 << "\n"
                          << "  actual:   " << actual << "\n"
                          << "  Manifest is stale. Refusing to load." << std::endl;
                BootState::Fail("manifest: sha mismatch (game updated mid-boot?)");
                return false;
            } else {
                std::cout << "[Manifest] SHA OK (" << actual.substr(0, 16) << "...)" << std::endl;
            }
        }
    }

    if (j.contains("il2cpp_api")) {
        auto& field_map = IL2CppFunctionFieldMap();
        for (auto it = j["il2cpp_api"].begin(); it != j["il2cpp_api"].end(); ++it) {
            ApiEntry e;
            const auto& v = it.value();
            if (v.contains("export") && v["export"].is_string()) e.export_name = v["export"].get<std::string>();
            if (v.contains("rva")    && v["rva"].is_string())    e.rva         = ParseHex(v["rva"].get<std::string>());

            if (v.contains("candidates") && v["candidates"].is_array()) {
                for (const auto& c : v["candidates"]) {
                    if (c.is_string()) {
                        uintptr_t crva = ParseHex(c.get<std::string>());
                        if (crva) e.candidates.push_back(crva);
                    }
                }
            }

            if (e.rva) {
                e.addr = reinterpret_cast<void*>(s.module_base + e.rva);
            } else if (!e.export_name.empty()) {
                e.addr = GetProcAddress(s.game_assembly, e.export_name.c_str());
            }

            const std::string canon = it.key();
            s.api[canon] = e;

            auto fit = field_map.find(canon);
            if (fit != field_map.end() && e.addr) {
                *(fit->second) = e.addr;
            }
        }
    }

    IL2CPP::Globals.m_GameAssembly = s.game_assembly;

    if (j.contains("methods")) {
        for (auto it = j["methods"].begin(); it != j["methods"].end(); ++it) {
            MethodEntry e;
            const auto& v = it.value();
            if (v.contains("class")     && v["class"].is_string())     e.class_name = v["class"].get<std::string>();
            if (v.contains("name")      && v["name"].is_string())      e.name       = v["name"].get<std::string>();
            if (v.contains("rva")       && v["rva"].is_string())       e.rva        = ParseHex(v["rva"].get<std::string>());
            if (v.contains("argc"))                                     e.argc       = v["argc"].get<int>();
            if (v.contains("signature") && v["signature"].is_string()) e.signature  = v["signature"].get<std::string>();
            if (e.rva) e.addr = reinterpret_cast<void*>(s.module_base + e.rva);
            s.methods[it.key()] = e;
        }
    }

    if (j.contains("fields")) {
        for (auto it = j["fields"].begin(); it != j["fields"].end(); ++it) {
            FieldEntry e;
            const auto& v = it.value();
            if (v.contains("class")  && v["class"].is_string())  e.class_name = v["class"].get<std::string>();
            if (v.contains("name")   && v["name"].is_string())   e.name       = v["name"].get<std::string>();
            if (v.contains("offset") && v["offset"].is_string()) e.offset     = static_cast<uint32_t>(ParseHex(v["offset"].get<std::string>()));
            if (v.contains("type")   && v["type"].is_string())   e.type       = v["type"].get<std::string>();
            s.fields[it.key()] = e;
        }
    }

    if (j.contains("classes")) {
        for (auto it = j["classes"].begin(); it != j["classes"].end(); ++it) {
            ClassEntry e;
            const auto& v = it.value();
            if (v.contains("image")          && v["image"].is_string())     e.image          = v["image"].get<std::string>();
            if (v.contains("namespace")      && v["namespace"].is_string()) e.ns             = v["namespace"].get<std::string>();
            if (v.contains("short_name")     && v["short_name"].is_string()) e.short_name    = v["short_name"].get<std::string>();
            if (v.contains("type_def_index") && v["type_def_index"].is_number_integer())
                e.type_def_index = v["type_def_index"].get<int32_t>();
            if (v.contains("klass_resolution") && v["klass_resolution"].is_string())
                e.klass_resolution = v["klass_resolution"].get<std::string>();
            if (v.contains("klass_ptr_rva")  && v["klass_ptr_rva"].is_string())
                e.klass_ptr_rva = ParseHex(v["klass_ptr_rva"].get<std::string>());

            if (e.short_name.empty()) {
                auto dot = it.key().find_last_of('.');
                e.short_name = (dot == std::string::npos) ? it.key() : it.key().substr(dot + 1);
            }
            s.classes[it.key()] = e;
        }
    }

    if (j.contains("static_fields")) {
        for (auto it = j["static_fields"].begin(); it != j["static_fields"].end(); ++it) {
            StaticFieldEntry e;
            const auto& v = it.value();
            if (v.contains("class") && v["class"].is_string())  e.class_name   = v["class"].get<std::string>();

            if (v.contains("field") && v["field"].is_string())  e.name = v["field"].get<std::string>();
            else if (v.contains("name") && v["name"].is_string()) e.name = v["name"].get<std::string>();

            if (v.contains("static_offset") && v["static_offset"].is_string())
                e.static_offset = static_cast<uint32_t>(ParseHex(v["static_offset"].get<std::string>()));
            else if (v.contains("offset") && v["offset"].is_string())
                e.static_offset = static_cast<uint32_t>(ParseHex(v["offset"].get<std::string>()));
            s.static_fields[it.key()] = e;
        }
    }

    for (const auto& kv : s.classes) {
        const std::string& fqcn = kv.first;
        const ClassEntry&  e    = kv.second;
        if (!e.short_name.empty() && e.short_name != fqcn) {
            s.class_aliases[e.short_name].push_back(fqcn);
        }

        auto last_dot = fqcn.find_last_of('.');
        if (last_dot != std::string::npos) {
            std::string tail = fqcn.substr(last_dot + 1);
            if (tail != e.short_name) s.class_aliases[tail].push_back(fqcn);
        }
    }

    if (j.contains("anticheat") && j["anticheat"].is_object()) {
        s.anticheat = j["anticheat"];
    }

    s.loaded = true;
    std::cout << "[Manifest] Loaded "
              << s.api.size()           << " APIs, "
              << s.classes.size()       << " classes, "
              << s.methods.size()       << " methods, "
              << s.fields.size()        << " fields, "
              << s.static_fields.size() << " static-fields, "
              << s.class_aliases.size() << " aliases from worker."
              << std::endl;
    return true;
}

inline bool Loaded() { return Get().loaded; }

inline std::string MakeKey(std::string_view klass, std::string_view name) {
    std::string k; k.reserve(klass.size() + 2 + name.size());
    k.append(klass).append("$$").append(name);
    return k;
}

inline void* GetMethod(std::string_view klass, std::string_view method) {
    auto& s = Get();
    auto it = s.methods.find(MakeKey(klass, method));
    if (it != s.methods.end()) return it->second.addr;

    auto ai = s.class_aliases.find(std::string(klass));
    if (ai != s.class_aliases.end()) {
        for (const auto& fqcn : ai->second) {
            auto it2 = s.methods.find(MakeKey(fqcn, method));
            if (it2 != s.methods.end()) return it2->second.addr;
        }
    }
    return nullptr;
}

inline void* GetMethod(const char* klass, const char* method) {
    return GetMethod(std::string_view(klass ? klass : ""), std::string_view(method ? method : ""));
}

inline void* GetMethodByKey(std::string_view key) {
    auto& s = Get();
    auto it = s.methods.find(std::string(key));
    if (it != s.methods.end()) return it->second.addr;
    return nullptr;
}
inline void* GetMethodByKey(const char* key) {
    return GetMethodByKey(std::string_view(key ? key : ""));
}

inline std::optional<uint32_t> GetFieldOffset(std::string_view klass, std::string_view field) {
    auto& s = Get();
    auto it = s.fields.find(MakeKey(klass, field));
    if (it != s.fields.end()) return it->second.offset;
    auto ai = s.class_aliases.find(std::string(klass));
    if (ai != s.class_aliases.end()) {
        for (const auto& fqcn : ai->second) {
            auto it2 = s.fields.find(MakeKey(fqcn, field));
            if (it2 != s.fields.end()) return it2->second.offset;
        }
    }
    return std::nullopt;
}

inline const ClassEntry* GetClassEntry(std::string_view fqcn) {
    auto& s = Get();
    auto it = s.classes.find(std::string(fqcn));
    if (it != s.classes.end()) return &it->second;
    auto ai = s.class_aliases.find(std::string(fqcn));
    if (ai != s.class_aliases.end() && !ai->second.empty()) {
        auto it2 = s.classes.find(ai->second.front());
        if (it2 != s.classes.end()) return &it2->second;
    }
    return nullptr;
}

inline std::optional<uint32_t> GetStaticFieldOffset(std::string_view klass, std::string_view field) {
    auto& s = Get();
    auto it = s.static_fields.find(MakeKey(klass, field));
    if (it != s.static_fields.end()) return it->second.static_offset;
    auto ai = s.class_aliases.find(std::string(klass));
    if (ai != s.class_aliases.end()) {
        for (const auto& fqcn : ai->second) {
            auto it2 = s.static_fields.find(MakeKey(fqcn, field));
            if (it2 != s.static_fields.end()) return it2->second.static_offset;
        }
    }
    return std::nullopt;
}

template <typename T>
inline T GetApi(std::string_view canonical) {
    auto& s = Get();
    auto it = s.api.find(std::string(canonical));
    if (it == s.api.end()) return nullptr;
    return reinterpret_cast<T>(it->second.addr);
}

inline std::vector<void*> GetApiCandidates(std::string_view canonical) {
    std::vector<void*> out;
    auto& s = Get();
    auto it = s.api.find(std::string(canonical));
    if (it == s.api.end() || !s.module_base) return out;
    const ApiEntry& e = it->second;
    if (e.addr) out.push_back(e.addr);
    for (uintptr_t crva : e.candidates) {
        void* a = reinterpret_cast<void*>(s.module_base + crva);
        if (a != e.addr) out.push_back(a);
    }
    return out;
}

inline uintptr_t ModuleBase()    { return Get().module_base; }
inline HMODULE   GameAssembly()  { return Get().game_assembly; }
inline const std::string& GameVersion() { return Get().game_version; }

inline const nlohmann::json* GetAnticheatSection() {
    auto& s = Get();
    if (!s.loaded || s.anticheat.is_null() || !s.anticheat.is_object()) return nullptr;
    return &s.anticheat;
}

namespace detail {
    inline bool g_class_from_name_broken = false;

    static void* SafeClassFromName(void* image, const char* ns, const char* name) {
        using FnT = void* (*)(void*, const char*, const char*);
        if (g_class_from_name_broken) return nullptr;
        auto fn = GetApi<FnT>("il2cpp_class_from_name");
        if (!fn || !image) return nullptr;
        void* r = nullptr;
        __try { r = fn(image, ns, name); }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            g_class_from_name_broken = true;
            std::cout << "[Manifest] il2cpp_class_from_name AV — fallback disabled." << std::endl;
            return nullptr;
        }
        return r;
    }

    inline std::unordered_map<std::string, void*> g_image_cache;
    inline bool g_image_walk_broken = false;

    static void* SafeImageByName(const std::string& imageName) {
        if (imageName.empty()) return nullptr;
        auto it = g_image_cache.find(imageName);
        if (it != g_image_cache.end()) return it->second;
        if (g_image_walk_broken) return nullptr;

        using DomainGetT     = void* (*)();
        using DomainGetAsmsT = void* (*)(void*, size_t*);
        using AsmGetImageT   = void* (*)(void*);
        using ImageGetNameT  = const char* (*)(void*);

        auto dg = GetApi<DomainGetT>("il2cpp_domain_get");
        auto dga = GetApi<DomainGetAsmsT>("il2cpp_domain_get_assemblies");
        auto agi = GetApi<AsmGetImageT>("il2cpp_assembly_get_image");
        auto ign = GetApi<ImageGetNameT>("il2cpp_image_get_name");
        if (!dg || !dga || !agi || !ign) { g_image_walk_broken = true; return nullptr; }

        void* domain = nullptr;
        void** asms  = nullptr;
        size_t n = 0;
        __try {
            domain = dg();
            if (!domain) { g_image_walk_broken = true; return nullptr; }
            asms = (void**)dga(domain, &n);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            g_image_walk_broken = true;
            std::cout << "[Manifest] domain/assemblies walk AV — image lookup disabled." << std::endl;
            return nullptr;
        }
        if (!asms || n == 0 || n > 10000) return nullptr;

        static bool diagDumped = false;
        if (!diagDumped) {
            diagDumped = true;
            std::cout << "[Manifest] image-walk diag: domain=" << domain
                      << " assemblies=" << (void*)asms
                      << " count=" << n << std::endl;
            for (size_t k = 0; k < n && k < 5; ++k) {
                if (!asms[k]) { std::cout << "[Manifest]   asm[" << k << "] NULL\n"; continue; }
                void* img2 = nullptr;
                const char* nm2 = nullptr;
                __try { img2 = agi(asms[k]); if (img2) nm2 = ign(img2); }
                __except (EXCEPTION_EXECUTE_HANDLER) {}
                std::cout << "[Manifest]   asm[" << k << "] img=" << img2
                          << " name=" << (nm2 ? nm2 : "(null)") << std::endl;
            }
            std::cout << "[Manifest] looking for image '" << imageName << "'..." << std::endl;
        }

        for (size_t i = 0; i < n; ++i) {
            if (!asms[i]) continue;
            void* img = nullptr;
            const char* name = nullptr;
            __try {
                img = agi(asms[i]);
                if (img) name = ign(img);
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                continue;
            }
            if (img && name && imageName == name) {
                g_image_cache[imageName] = img;
                return img;
            }
        }
        return nullptr;
    }
}

namespace detail {
    static void* SafeDerefKlassSlot(uintptr_t slot_addr) {
        void* r = nullptr;
        __try { r = *reinterpret_cast<void**>(slot_addr); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
        return r;
    }
}

inline void* GetClassPtr(std::string_view fqcn) {
    auto& s = Get();
    auto it = s.classes.find(std::string(fqcn));
    if (it == s.classes.end()) {

        auto ai = s.class_aliases.find(std::string(fqcn));
        if (ai == s.class_aliases.end() || ai->second.empty()) return nullptr;
        it = s.classes.find(ai->second.front());
        if (it == s.classes.end()) return nullptr;
    }
    ClassEntry& e = it->second;
    if (e.cached_klass) return e.cached_klass;

    if (e.klass_resolution == "metadata_usage" && e.klass_ptr_rva && s.module_base) {
        e.cached_klass = detail::SafeDerefKlassSlot(s.module_base + e.klass_ptr_rva);
        return e.cached_klass;
    }

    if (e.klass_resolution == "class_from_name") {
        const char* nsArg = e.ns.c_str();
        const char* nameArg = e.short_name.empty() ? std::string(fqcn).c_str() : e.short_name.c_str();

        auto tryImage = [&](const std::string& imgName) -> void* {
            void* img = detail::SafeImageByName(imgName);
            if (!img) return nullptr;
            return detail::SafeClassFromName(img, nsArg, nameArg);
        };

        void* k = nullptr;
        if (!e.image.empty() && e.image != "<hidden>") {
            k = tryImage(e.image);
        }
        if (!k) {
            static const char* kFallbackImages[] = {
                "mscorlib.dll",
                "System.dll",
                "Assembly-CSharp.dll",
                "UnityEngine.dll",
                "UnityEngine.CoreModule.dll",
                "UnityEngine.UIElementsModule.dll",
                "ACTk.Runtime.dll",
                "Unity.TextMeshPro.dll",
                "netstandard.dll",
                "System.Core.dll",
            };
            for (const char* img : kFallbackImages) {
                k = tryImage(img);
                if (k) {
                    std::cout << "[Manifest] " << std::string(fqcn)
                              << " resolved via fallback image '" << img
                              << "' (manifest said '" << e.image << "')" << std::endl;
                    break;
                }
            }
        }
        e.cached_klass = k;
        return e.cached_klass;
    }

    return nullptr;
}

inline void* GetClassPtr(const char* fqcn) {
    return GetClassPtr(std::string_view(fqcn ? fqcn : ""));
}

}

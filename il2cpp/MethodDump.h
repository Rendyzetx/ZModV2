#pragma once
#include <windows.h>
#include <fstream>
#include <string>
#include <thread>
#include <atomic>
#include "json.hpp"

namespace MethodDump
{
    inline std::atomic<bool> g_DumpRunning{ false };
    inline std::string       g_DumpStatus  = "Idle";

    using t_image_get_name        = const char* (*)(const void*);
    using t_image_get_class_count = uint32_t    (*)(const void*);
    using t_image_get_class       = void*       (*)(const void*, uint32_t);
    using t_class_get_name        = const char* (*)(void*);
    using t_class_get_methods     = const void* (*)(void*, void**);
    using t_method_get_name       = const char* (*)(const void*);

    extern void* il2cpp_domain_get();
    extern void** il2cpp_domain_get_assemblies(void*, size_t*);
    extern void*  il2cpp_assembly_get_image(const void*);

    static std::string RunDump()
    {
        HMODULE ga = GetModuleHandleA("GameAssembly.dll");
        if (!ga) return "[Dump] GameAssembly.dll not found";
        uintptr_t base = (uintptr_t)ga;

        auto fn_img_name  = (t_image_get_name)       GetProcAddress(ga, "il2cpp_image_get_name");
        auto fn_img_cnt   = (t_image_get_class_count)GetProcAddress(ga, "il2cpp_image_get_class_count");
        auto fn_img_cls   = (t_image_get_class)      GetProcAddress(ga, "il2cpp_image_get_class");
        auto fn_cls_name  = (t_class_get_name)       GetProcAddress(ga, "il2cpp_class_get_name");
        auto fn_cls_meths = (t_class_get_methods)    GetProcAddress(ga, "il2cpp_class_get_methods");
        auto fn_mth_name  = (t_method_get_name)      GetProcAddress(ga, "il2cpp_method_get_name");

        if (!fn_img_cnt || !fn_img_cls || !fn_cls_name || !fn_cls_meths || !fn_mth_name)
            return "[Dump] Missing IL2CPP exports — check GameAssembly.dll";

        using t_dom_get  = void*(*)();
        using t_dom_asms = void**(*)(void*, size_t*);
        using t_asm_img  = void* (*)(const void*);

        auto fn_dom_get  = (t_dom_get) GetProcAddress(ga, "il2cpp_domain_get");
        auto fn_dom_asms = (t_dom_asms)GetProcAddress(ga, "il2cpp_domain_get_assemblies");
        auto fn_asm_img  = (t_asm_img) GetProcAddress(ga, "il2cpp_assembly_get_image");
        if (!fn_dom_get || !fn_dom_asms || !fn_asm_img)
            return "[Dump] Missing domain/assembly IL2CPP exports";

        void*   domain = fn_dom_get();
        size_t  asmCount = 0;
        void**  assemblies = fn_dom_asms(domain, &asmCount);
        if (!assemblies || asmCount == 0)
            return "[Dump] No assemblies found";

        nlohmann::json arr = nlohmann::json::array();
        uint32_t totalMethods = 0;

        for (size_t ai = 0; ai < asmCount; ai++)
        {
            if (!assemblies[ai]) continue;
            void* image = fn_asm_img(assemblies[ai]);
            if (!image) continue;

            if (fn_img_name) {
                const char* imgName = fn_img_name(image);
                if (!imgName || std::string(imgName) != "Assembly-CSharp.dll") continue;
            }

            uint32_t classCount = fn_img_cnt(image);
            g_DumpStatus = "[Dump] Enumerating " + std::to_string(classCount) + " classes...";

            for (uint32_t ci = 0; ci < classCount; ci++)
            {
                void* klass = fn_img_cls(image, ci);
                if (!klass) continue;
                const char* className = fn_cls_name(klass);
                if (!className) continue;

                void*       iter   = nullptr;
                const void* method = nullptr;
                while ((method = fn_cls_meths(klass, &iter)) != nullptr)
                {
                    const char* methodName = fn_mth_name(method);
                    if (!methodName) continue;

                    uintptr_t methodPtr = *(uintptr_t*)method;
                    if (methodPtr == 0) continue;

                    if (methodPtr < base || methodPtr > base + 0x20000000) continue;

                    uintptr_t rva = methodPtr - base;
                    std::string name = std::string(className) + "$$" + methodName;
                    arr.push_back({ {"Address", (uint64_t)rva}, {"Name", name} });
                    totalMethods++;
                }
            }
            break;
        }

        if (totalMethods == 0)
            return "[Dump] No methods found — IL2CPP may not be fully initialized yet";

        char tempPath[MAX_PATH] = {};
        GetTempPathA(MAX_PATH, tempPath);
        std::string outFile = std::string(tempPath) + "SlopSpark_methods.json";

        nlohmann::json root = { {"ScriptMethod", arr} };
        std::ofstream f(outFile);
        if (!f.is_open())
            return "[Dump] Failed to write to " + outFile;
        f << root.dump(2);
        f.close();

        return "[Dump] Wrote " + std::to_string(totalMethods)
             + " methods to:\n" + outFile
             + "\n\nRun: python update_offsets\\update_offsets.py --dump-file \"" + outFile + "\"";
    }

    static void StartDump()
    {
        if (g_DumpRunning.exchange(true)) return;
        g_DumpStatus = "[Dump] Running...";
        std::thread([] {
            g_DumpStatus = RunDump();
            g_DumpRunning.store(false);
        }).detach();
    }

}

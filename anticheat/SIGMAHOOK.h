#include "IL2CPP_Resolver/kiero/minhook/include/MinHook.h"
uintptr_t moduleBase = (uintptr_t)GetModuleHandle("GameAssembly.dll");
#define SIGMAHOOK(OFFSET, HOOKFUNC, ORIGINALFUNC) \
    do { \
        void* target = reinterpret_cast<void*>(moduleBase + OFFSET); \
        if (MH_CreateHook(target, HOOKFUNC, reinterpret_cast<LPVOID*>(ORIGINALFUNC)) != MH_OK) { \
            std::cout << "Invalid Offset\n"; \
        } else if (MH_EnableHook(target) != MH_OK) { \
        } \
    } while (0)

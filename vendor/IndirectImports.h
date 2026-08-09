

#pragma once

#include <Windows.h>
#include "XorStr.h"

#define IMP(mod, name) \
    ((decltype(&name))::GetProcAddress( \
        []() -> HMODULE { \
            HMODULE h = ::GetModuleHandleA(XS(#mod)); \
            if (!h) h = ::LoadLibraryA(XS(#mod)); \
            return h; \
        }(), \
        XS(#name)))

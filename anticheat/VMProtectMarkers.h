

#pragma once

#if defined(USE_VMPROTECT) && __has_include("VMProtectSDK.h")
    #include "VMProtectSDK.h"
    #define VMP_BEGIN(name)        VMProtectBegin(name)
    #define VMP_BEGIN_VIRT(name)   VMProtectBeginVirtualization(name)
    #define VMP_BEGIN_MUT(name)    VMProtectBeginMutation(name)
    #define VMP_END()              VMProtectEnd()
    #define VMP_ENABLED            1
#else
    #define VMP_BEGIN(name)        ((void)0)
    #define VMP_BEGIN_VIRT(name)   ((void)0)
    #define VMP_BEGIN_MUT(name)    ((void)0)
    #define VMP_END()              ((void)0)
    #define VMP_ENABLED            0
#endif

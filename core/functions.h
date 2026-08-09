#pragma once
#include "main.h"       
#include "IL2CPP_Resolver/IL2CPP_Resolver.hpp" 

#include <iostream>    
#include <functional>
#include <vector>
#include <mutex>
#include <string>
#include <algorithm>
#include <map>

struct NetworkPlayer {
    Unity::System_String* clientId;
};

inline std::map<std::string, bool> excludedPlayerIds;

class CFeatures
{
private:

    bool m_afkPatched = false;

    void HandleFly()
    {
        if (!b_keysToFlyEnabled || !g_LocalPlayer || !oGetTransform) return;

        auto* velocity = reinterpret_cast<Unity::Vector3*>((uintptr_t)g_LocalPlayer + g_off_Player_velocity);
        bool isMovingY = false, isMovingX = false;

        if (ImGui::IsKeyDown(ImGuiKey_W)) { velocity->y = f_flySpeed; isMovingY = true; }
        else if (ImGui::IsKeyDown(ImGuiKey_S)) { velocity->y = -f_flySpeed; isMovingY = true; }

        if (ImGui::IsKeyDown(ImGuiKey_A)) { velocity->x = -f_flySpeed; isMovingX = true; }
        else if (ImGui::IsKeyDown(ImGuiKey_D)) { velocity->x = f_flySpeed; isMovingX = true; }

        if (!isMovingY) velocity->y = -9.81f * ImGui::GetIO().DeltaTime;
        if (!isMovingX) velocity->x = 0;
    }

    void UpdateAfkPatch()
    {
       
        if (b_patchAfkTimers && !m_afkPatched)
        {
            Unity::il2cppClass* configDataClass = IL2CPP::Class::Find("ConfigData");
            if (configDataClass)
            {
                int newTimeoutValue = 2147483647;
                IL2CPP::Class::Utils::SetStaticField(configDataClass, "playerChangeToSleepSeconds", &newTimeoutValue);
                IL2CPP::Class::Utils::SetStaticField(configDataClass, "playerInactivitySeconds", &newTimeoutValue);
                IL2CPP::Class::Utils::SetStaticField(configDataClass, "playerInactivitySecondsFishing", &newTimeoutValue);

                m_afkPatched = true; 
            }
        }
       
    }

public:
  
    bool b_keysToFlyEnabled = false;
    float f_flySpeed = 4.0f;
    bool b_patchAfkTimers = false;

 
    void Run()
    {
      
        HandleFly();
        UpdateAfkPatch();

    }
};

inline CFeatures g_features;
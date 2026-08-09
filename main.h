#pragma once
#include "ProxyTunnel.h"
#include "PWMath.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include "SIGMAHOOK.h"
#include <locale>
#include <codecvt>
#include <mutex>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <functional>
#include <cstring>
#include "json.hpp"
#include <random>
#include <filesystem>
#include <atomic>
#include "json.hpp"
#include <corecrt_math.h>
#include "IL2CPP_Resolver/IL2CPP_Resolver.hpp"
#include "Manifest.h"

inline bool g_AntiCollect = false;
inline bool g_autoCollect = false;
inline bool g_antiPoison = false;
inline bool g_autoGiftBox = false;
inline bool g_extraZoom   = false;
inline float g_extraZoomMul = 3.0f;

inline bool g_dropStackShift = true;

inline bool g_antiSpeedKick = true;

inline void AntiSpeedKickTick(void* wc);

namespace Store {
    void Resolve();
    void TickFromHook();
}

inline void* g_GameplayUI = nullptr;
inline void* g_InventoryControl = nullptr;
typedef bool(__fastcall* tCanPlayerPickCollectableFromBlock)(int blockType);
inline tCanPlayerPickCollectableFromBlock oCanPlayerPickCollectableFromBlock = nullptr;
inline bool __fastcall hkCanPlayerPickCollectableFromBlock(int blockType) {
    if (g_AntiCollect) {
        return false;
    }
    return oCanPlayerPickCollectableFromBlock(blockType);
}

typedef float(__fastcall* tVector2_Distance)(Vector2 a, Vector2 b);
inline tVector2_Distance oVector2_Distance = nullptr;

enum PlayerJumpMode { Normal = 0, Double, LongJump, Parachute, ContinuousJumping, Triple, Rocket, };
enum class GravityMode { Normal = 0, Low, High, Zero };

enum AIEnemyType {
    AI_None = 0,
    AI_TestBlob = 1,
    AI_FlameBallGround01 = 2,
    AI_FlameBallAir01 = 3,
    AI_FlameBallCharging01 = 4,
    AI_ArmoredWalkerCharge01 = 5,
    AI_RobeCaster01 = 6,
    AI_TentacleShooter01 = 7,
    AI_RobeCasterSpell01 = 8,
    AI_TentacleShooterBullet01 = 9,
    AI_StaffCaster01 = 10,
    AI_StaffCasterSpell01 = 11,
    AI_Wraith01 = 12,
    AI_WheeledBuzzSaw = 13,
    AI_DrillCharger = 14,
    AI_SelfDestructCannon = 15,
    AI_SelfDestructCannonBullet = 16,
    AI_PropellerBomber = 17,
    AI_PropellerBomberBomb = 18,
    AI_ShotgunPatrol = 19,
    AI_ShotgunPatrolBullet = 20,
    AI_LaboratoryBoss = 21,
    AI_LaboratoryBossProjectileA = 22,
    AI_LaboratoryBossProjectileB = 23,
    AI_LaboratoryBossProjectileC = 24,
    AI_LaboratoryBossProjectileD = 25,
    AI_PropellerBomberPropeller_Visual = 26,
    AI_LaboratoryBossTreadFront_Visual = 27,
    AI_LaboratoryBossTreadBack_Visual = 28,
    AI_LaboratoryBossCannonConnector_Visual = 29,
    AI_LaboratoryBossCannonBottom_Visual = 30,
    AI_LaboratoryBossCannonTop_Visual = 31,
    AI_LaboratoryBossSkull_Visual = 32,
    AI_LaboratoryBossBodyTop_Visual = 33,
    AI_LaboratoryBossBodyGlass_Visual = 34,
    AI_LaboratoryBossVillain_Visual = 35,
    AI_LaboratoryBossBodyTop_VisualOffset = 36,
    AI_LaboratoryBossCannonBottom_VisualOffset = 37,
    AI_LaboratoryBossCannonTop_VisualOffset = 38,
    AI_LaboratoryBossPropulsion_Visual = 39,
    AI_LaboratoryBossSkull_VisualOffset = 40,
    AI_LaboratoryBossJetFlame = 41,
    AI_LaboratoryBossDustLeft_Visual = 42,
    AI_LaboratoryBossDustRight_Visual = 43,
    AI_LaboratoryBossProjectileE = 44,
    AI_SummerFish = 45,
    AI_NetherBossWraith = 46,
    AI_NetherBossWraithSpell = 47,
    AI_NetherBossWraithShield = 48,
    AI_Bat = 49,
    AI_DeepMaggot = 50,
    Gassy = 51,
    AI_RockyHorror = 52,
    AI_DeepMaggotBullet = 53,
    AI_RockyHorrorRocks = 54,
    AI_ArmoredWalkerCharge02 = 55,
    AI_ArmoredWalkerCharge03 = 56,
    AI_ArmoredWalkerCharge04 = 57,
    AI_ArmoredWalkerCharge05 = 58,
    AI_StaffCaster02 = 59,
    AI_StaffCaster03 = 60,
    AI_StaffCaster04 = 61,
    AI_StaffCaster05 = 62,
    AI_StaffCasterSpell02 = 63,
    AI_StaffCasterSpell03 = 64,
    AI_StaffCasterSpell04 = 65,
    AI_StaffCasterSpell05 = 66,
    AI_RobeCaster02 = 67,
    AI_RobeCaster03 = 68,
    AI_RobeCaster04 = 69,
    AI_RobeCaster05 = 70,
    AI_RobeCasterSpell02 = 71,
    AI_RobeCasterSpell03 = 72,
    AI_RobeCasterSpell04 = 73,
    AI_RobeCasterSpell05 = 74,
    AI_Wraith02 = 75,
    AI_Wraith03 = 76,
    AI_Wraith04 = 77,
    AI_Wraith05 = 78,
    AI_TentacleShooter02 = 79,
    AI_TentacleShooter03 = 80,
    AI_TentacleShooter04 = 81,
    AI_TentacleShooter05 = 82,
    AI_TentacleShooterBullet02 = 83,
    AI_TentacleShooterBullet03 = 84,
    AI_TentacleShooterBullet04 = 85,
    AI_TentacleShooterBullet05 = 86,
    AI_FlameBallGround02 = 87,
    AI_FlameBallGround03 = 88,
    AI_FlameBallGround04 = 89,
    AI_FlameBallGround05 = 90,
    AI_FlameBallAir02 = 91,
    AI_FlameBallAir03 = 92,
    AI_FlameBallAir04 = 93,
    AI_FlameBallAir05 = 94,
    AI_FlameBallCharging02 = 95,
    AI_FlameBallCharging03 = 96,
    AI_FlameBallCharging04 = 97,
    AI_FlameBallCharging05 = 98,
    AI_END_OF_ENUM = 99,
};

inline bool IsProjectileEnemyType(int et) {
    switch (et) {
        case 8: case 9: case 11: case 16: case 18: case 20:
        case 22: case 23: case 24: case 25: case 26: case 27: case 28:
        case 29: case 30: case 31: case 32: case 33: case 34: case 35:
        case 36: case 37: case 38: case 39: case 40: case 42: case 43:
        case 44: case 47:
        case 53:
        case 54:
        case 63: case 64: case 65: case 66: case 71: case 72: case 73: case 74:
        case 83: case 84: case 85: case 86:
            return true;
        default: return false;
    }
}
enum class JoinState { Idle, Joining, InWorld };

inline JoinState g_joinState = JoinState::Idle;

struct AIBase {
    char pad_0000[0x10];
    AIEnemyType enemyType;
    int id;
    char pad_0018[0x58];
    int health;
};

struct World {
    enum BlockType {
        None = 0,
        MineworldPortal = 3965,
        MiningGemStoneDiamond = 3995,
        MiningGemStoneEmerald = 3996,
        MiningGemStoneRuby = 3999,
        MiningGemStoneSapphire = 4000,
        MiningGemStoneTopaz = 4002,
        MiningGemStoneAmethyst = 15773,
        OrbForestBackground = 2824,
        OrbNightBackground = 2825,
        OrbSpaceBackground = 2826,
        OrbDesertBackground = 2827,
        OrbIceBackground = 2828,
        OrbStarBackground = 2829,
        OrbCandyBackground = 2830,
        OrbHalloweenTowerBackground = 3381,
        OrbCemeteryBackground = 3591,
        OrbNetherBackground = 8014,
        OrbCityBackground = 8241,
        OrbBlueSkyBackground = 8295,
        OrbJetRaceBackground = 8842,
        GiftBox = 966,
        NetherGiftBox = 1392,
        LabGiftBox = 17565,
        SoilBlock = 1,
        MiningNuggetBronze = 3985,
        MiningNuggetSilver = 3986,
        MiningNuggetGold = 3987,
        MiningNuggetPlatinum = 3988,
        PortalMineExit = 3966,
        DeepNetherExit = 8040,
        NetherExit = 1419,
        SpikeTrap = 109,
        DeathSpikeTrap = 4551,
        OrbLightingMining = 3973,
        MiningLightCrystalSmall = 3974,
        MiningLightCrystalMedium = 3975,
        MiningLightCrystalLarge = 3976,
        MiningTimeCrystalSmall = 3977,
        MiningTimeCrystalMedium = 3978,
        MiningTimeCrystalLarge = 3979,
        CrackedPottery = 1663,
        NetherTreasure = 1400,
    };

    enum WeatherType { WeatherNone = 0, HeavyRain, PixelTrail, SandStorm, LightRain, LightSnow, SnowStorm, DeepNether, Halloween, HalloweenTower, Hearts, Mining, AuroraBorealis, Armageddon, };

    enum WorldLayoutType { LayoutDefault = 0, LayoutNetherWorld = 17, LayoutGeneratedMine = 21 };
    enum LightingType { LightingNone = 0, Dark, LightingMining, LesserDark, GreatDark, };
    enum LayerBackgroundType { ForestBackground, NightBackground, SpaceBackground, DesertBackground, IceBackground, StarBackground, CandyBackground, HalloweenTowerBackground, NetherBackground, CityBackground, BlueSkyBackground, JetRaceBackground };
};

enum InventoryItemType : char { Block = 0, BlockBackground, Seed, BlockWater, WearableItem, Weapon, Throwable, Consumable, Shard, Blueprint, Familiar, FAMFood, BlockWiring };

#pragma pack(push, 1)
struct CollectableData {
    char             pad_0000[0x10];
    int              id;
    World::BlockType blockType;
    int              inventoryItemType;
    char             pad_001C[0x4];
    void* inventoryData;
    float            posX;
    float            posY;
    Vector2i         mapPoint;
    short            amount;
    bool             isGem;
};

struct CollectablesList {
    char pad_0000[0x10];
    CollectableData** items;
    char pad_0018[0x8];
    int size;
};
#pragma pack(pop)

inline bool g_antiDeflector = false;

#pragma pack(push, 1)
struct PlayerData_InventoryKey {
    int blockType;
    int itemType;
};
#pragma pack(pop)

typedef bool(__fastcall* tIsBlockDeflector)(int);
typedef float(__fastcall* tGetDeflectorForce)(int);
typedef float(__fastcall* tGetDeflectorMaxForce)(int);
typedef float(__fastcall* tGetDeflectorForceGrounded)(int);
typedef float(__fastcall* tGetDeflectorMaxForceGrounded)(int);

inline tIsBlockDeflector oIsBlockDeflector = nullptr;
inline tGetDeflectorForce oGetDeflectorForce = nullptr;
inline tGetDeflectorMaxForce oGetDeflectorMaxForce = nullptr;
inline tGetDeflectorForceGrounded oGetDeflectorForceGrounded = nullptr;
inline tGetDeflectorMaxForceGrounded oGetDeflectorMaxForceGrounded = nullptr;

typedef bool(__cdecl* tIsBlockGiftBox)(World::BlockType blockType);
inline tIsBlockGiftBox oIsBlockGiftBox = nullptr;

typedef bool(__cdecl* tIsBlockExitPortal)(World::BlockType blockType);
inline tIsBlockExitPortal oIsBlockExitPortal = nullptr;

inline bool __fastcall hkIsBlockDeflector(int blockType) {
    if (g_antiDeflector) return false;
    return oIsBlockDeflector(blockType);
}

inline float __fastcall hkGetDeflectorForce(int blockType) {
    if (g_antiDeflector) return 0.0f;
    return oGetDeflectorForce(blockType);
}

inline float __fastcall hkGetDeflectorMaxForce(int blockType) {
    if (g_antiDeflector) return 0.0f;
    return oGetDeflectorMaxForce(blockType);
}

inline float __fastcall hkGetDeflectorForceGrounded(int blockType) {
    if (g_antiDeflector) return 0.0f;
    return oGetDeflectorForceGrounded(blockType);
}

inline float __fastcall hkGetDeflectorMaxForceGrounded(int blockType) {
    if (g_antiDeflector) return 0.0f;
    return oGetDeflectorMaxForceGrounded(blockType);
}

typedef void(__cdecl* tReloadFunction)();
inline tReloadFunction oReloadGame = nullptr;

using json = nlohmann::json;
struct Il2CppString { void* klass; void* monitor; int length; wchar_t chars[1]; };
struct Il2CppArray { void* klass; void* monitor; void* bounds; size_t max_length; void* items[1]; };
enum class PacketDirection { Outgoing, Incoming };
struct PacketInfo { int logId; std::chrono::system_clock::time_point timestamp; PacketDirection direction; std::string idString; std::string fullPacketJson; };

inline uintptr_t gameAssemblyBase = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
inline std::vector<PacketInfo> g_PacketLog;
inline std::mutex g_PacketLogMutex;
inline int g_LogCounter = 0;
inline bool g_CaptureOutgoing = false;
inline bool g_CaptureIncoming = false;

inline std::atomic<bool> g_haveAgiCipher{ false };
inline std::mutex g_agiCipherMutex;
inline std::string g_agiCipherId;
inline std::vector<std::string> g_packetIdIgnoreList = { "p", "mP", "mp", "ST", "AI", "PSicU", "PPA", "HBB", "nCo" };
inline bool g_AutoScroll = true;
inline bool g_newPacketReceived = false;
inline std::set<int> g_selectedPackets;
inline int  g_anchorPacketIndex = -1;
inline char g_packetFilterText[256] = { 0 };
inline int g_directionFilter = 0;
inline char g_packetRepeaterContent[16384] = { 0 };
inline char g_packetIgnoreListBuffer[1024] = "p,mP,mp,ST,AI,PSicU,PPA,HBB,nCo";
inline std::string g_networkStatus = "Ready.";

inline void DiceExploit_ProcessPacket(const json&, bool) {}
inline void SoilGame_ProcessPacket(const json&, bool) {}
inline void FossilDebug_ProcessPacket(const json&, bool) {}

typedef void(__fastcall* tAddOneMessageToList)(void* bsonPacket);
typedef void(__fastcall* tHandleMessages)(void* thisPtr, void* bsonPacket);
typedef Il2CppArray* (__fastcall* tBsonDump)(void* bsonPacket);
typedef void* (__fastcall* tSimpleBSON_Load)(void* il2cppByteArray);

typedef void(__fastcall* tHandleWarpPlayerMessage)(void* thisPtr, void* bsonMsg, void* methodInfo);
inline tHandleWarpPlayerMessage oHandleWarpPlayerMessage_orig = nullptr;

inline tHandleWarpPlayerMessage oHandlePlayerActivateInPortal_orig = nullptr;
inline tHandleWarpPlayerMessage oHandleResurrectPlayer_orig        = nullptr;
typedef Il2CppArray* (*til2cpp_array_new)(void* klass, size_t count);
typedef void* (*til2cpp_domain_get)();
typedef Il2CppArray* (*til2cpp_domain_get_assemblies)(void* domain, size_t* size);
typedef void* (*til2cpp_assembly_get_image)(void* assembly);
typedef void* (*til2cpp_class_from_name)(void* image, const char* name_space, const char* name);

typedef void* (*til2cpp_object_new)(void* klass);
typedef void* (*til2cpp_method_get_param)(const void* methodInfo, uint32_t index);
typedef void* (*til2cpp_class_from_il2cpp_type)(void* type);
typedef Il2CppString* (*tIl2CppStringNew)(const char* text);
typedef void* (*til2cpp_class_get_field_from_name)(void* klass, const char* name);
typedef void (*til2cpp_field_static_get_value)(void* field, void* value);
typedef void (*til2cpp_field_static_set_value)(void* field, void* value);

typedef size_t (*til2cpp_image_get_class_count)(void* image);
typedef void*  (*til2cpp_image_get_class)(void* image, size_t index);
typedef const char* (*til2cpp_class_get_name)(void* klass);
typedef const char* (*til2cpp_class_get_namespace)(void* klass);

typedef void* (*til2cpp_class_get_type)(void* klass);
typedef void* (*til2cpp_type_get_object)(void* type);
typedef void* (*til2cpp_object_get_class)(void* object);

inline tAddOneMessageToList oAddOneMessageToList = nullptr;
inline tHandleMessages oHandleMessages = nullptr;
inline tBsonDump oBsonDump = nullptr;
inline tSimpleBSON_Load oSimpleBSON_Load = nullptr;
inline til2cpp_array_new il2cpp_array_new = nullptr;
inline void* byteClass = nullptr;

inline bool g_il2cpp_string_new_validated = false;

static int ProbeIl2CppStringNew(void* fnPtr) {
    using FnT = void* (*)(const char*);
    auto fn = (FnT)fnPtr;
    void* probe = nullptr;
    __try { probe = fn("ZMOD_PROBE"); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    if (!probe) return 0;

    int32_t len = -1;
    __try { len = *(int32_t*)((uint8_t*)probe + 0x10); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    if (len != 10) {
        return -((len < 0 || len > 1000000) ? 999999 : len);
    }

    uint16_t ch = 0;
    __try { ch = *(uint16_t*)((uint8_t*)probe + 0x14); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    if (ch != 0x005A) return -2;
    return len;
}

static uintptr_t SehReadModuleSizeOfImage(uintptr_t modBase) {
    if (!modBase) return 0;
    uintptr_t sz = 0;
    __try {
        auto* dos = (IMAGE_DOS_HEADER*)modBase;
        if (dos->e_magic == IMAGE_DOS_SIGNATURE) {
            auto* nt = (IMAGE_NT_HEADERS*)(modBase + dos->e_lfanew);
            if (nt->Signature == IMAGE_NT_SIGNATURE) {
                sz = nt->OptionalHeader.SizeOfImage;
            }
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) { sz = 0; }
    return sz;
}

static bool ProbeIl2CppStringNewLenient(void* fnPtr, uintptr_t , uintptr_t ) {
    using FnT = void* (*)(const char*);
    auto fn = (FnT)fnPtr;
    void* probe = nullptr;
    __try { probe = fn("ZMOD_PROBE"); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    if (!probe) return false;

    void* klass = nullptr;
    __try { klass = *(void**)probe; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    if (!klass) return false;
    uintptr_t k = (uintptr_t)klass;

    if (k < 0x10000 || k > 0x7FFFFFFFFFFFull || (k & 0x7) != 0) return false;

    __try {
        volatile uintptr_t probeWord = *(volatile uintptr_t*)klass;
        (void)probeWord;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}

static void* WalkThunks(void* fn, int maxHops) {
    uint8_t* p = (uint8_t*)fn;
    for (int i = 0; i < maxHops; ++i) {
        uint8_t op = 0;
        __try { op = *p; } __except (EXCEPTION_EXECUTE_HANDLER) { return p; }
        if (op == 0xE9) {
            int32_t disp = 0;
            __try { disp = *(int32_t*)(p + 1); } __except (EXCEPTION_EXECUTE_HANDLER) { return p; }
            p = p + 5 + disp;
        } else if (op == 0xFF) {
            uint8_t modrm = 0;
            __try { modrm = *(p + 1); } __except (EXCEPTION_EXECUTE_HANDLER) { return p; }
            if (modrm != 0x25) return p;
            int32_t disp = 0;
            uint8_t* slot = nullptr;
            void* tgt = nullptr;
            __try {
                disp = *(int32_t*)(p + 2);
                slot = p + 6 + disp;
                tgt  = *(void**)slot;
            } __except (EXCEPTION_EXECUTE_HANDLER) { return p; }
            if (!tgt) return p;
            p = (uint8_t*)tgt;
        } else {
            return p;
        }
    }
    return p;
}

static void* ResolveStringNewWithThunkChase(void* manifestFn) {
    if (!manifestFn) return nullptr;
    if (ProbeIl2CppStringNew(manifestFn) > 0) return manifestFn;
    void* hop = WalkThunks(manifestFn, 8);
    if (hop && hop != manifestFn && ProbeIl2CppStringNew(hop) > 0) return hop;
    return nullptr;
}
inline til2cpp_domain_get il2cpp_domain_get = nullptr;
inline til2cpp_domain_get_assemblies il2cpp_domain_get_assemblies = nullptr;
inline til2cpp_assembly_get_image il2cpp_assembly_get_image = nullptr;
inline til2cpp_class_from_name il2cpp_class_from_name = nullptr;
inline tIl2CppStringNew oIl2CppStringNew = nullptr;
inline til2cpp_class_get_field_from_name il2cpp_class_get_field_from_name = nullptr;

inline til2cpp_object_new                il2cpp_object_new                = nullptr;
inline til2cpp_method_get_param          il2cpp_method_get_param          = nullptr;
inline til2cpp_class_from_il2cpp_type    il2cpp_class_from_il2cpp_type    = nullptr;
inline til2cpp_field_static_get_value il2cpp_field_static_get_value = nullptr;
inline til2cpp_field_static_set_value il2cpp_field_static_set_value = nullptr;
inline til2cpp_image_get_class_count il2cpp_image_get_class_count = nullptr;
inline til2cpp_image_get_class       il2cpp_image_get_class       = nullptr;
inline til2cpp_class_get_name        il2cpp_class_get_name        = nullptr;
inline til2cpp_class_get_namespace   il2cpp_class_get_namespace   = nullptr;
inline til2cpp_class_get_type        il2cpp_class_get_type        = nullptr;
inline til2cpp_type_get_object       il2cpp_type_get_object       = nullptr;
inline til2cpp_object_get_class      il2cpp_object_get_class      = nullptr;

inline std::string FormatTimestamp(const std::chrono::system_clock::time_point& tp) {
    auto in_time_t = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    tm local_tm;
    localtime_s(&local_tm, &in_time_t);
    ss << std::put_time(&local_tm, "%H:%M:%S");
    return ss.str();
}

inline void InitializeIl2CppHelpers() {

    il2cpp_array_new                 = Manifest::GetApi<til2cpp_array_new>("il2cpp_array_new");
    oIl2CppStringNew                 = Manifest::GetApi<tIl2CppStringNew>("il2cpp_string_new");

    il2cpp_class_get_field_from_name = Manifest::GetApi<til2cpp_class_get_field_from_name>("il2cpp_class_get_field_from_name");
    il2cpp_field_static_get_value    = Manifest::GetApi<til2cpp_field_static_get_value>("il2cpp_field_static_get_value");
    il2cpp_field_static_set_value    = Manifest::GetApi<til2cpp_field_static_set_value>("il2cpp_field_static_set_value");

    il2cpp_object_new                = Manifest::GetApi<til2cpp_object_new>("il2cpp_object_new");
    il2cpp_method_get_param          = Manifest::GetApi<til2cpp_method_get_param>("il2cpp_method_get_param");
    il2cpp_class_from_il2cpp_type    = Manifest::GetApi<til2cpp_class_from_il2cpp_type>("il2cpp_class_from_il2cpp_type");

    il2cpp_domain_get             = Manifest::GetApi<til2cpp_domain_get>("il2cpp_domain_get");
    il2cpp_domain_get_assemblies  = Manifest::GetApi<til2cpp_domain_get_assemblies>("il2cpp_domain_get_assemblies");
    il2cpp_assembly_get_image     = Manifest::GetApi<til2cpp_assembly_get_image>("il2cpp_assembly_get_image");
    il2cpp_class_from_name        = Manifest::GetApi<til2cpp_class_from_name>("il2cpp_class_from_name");
    il2cpp_image_get_class_count  = Manifest::GetApi<til2cpp_image_get_class_count>("il2cpp_image_get_class_count");
    il2cpp_image_get_class        = Manifest::GetApi<til2cpp_image_get_class>("il2cpp_image_get_class");
    il2cpp_class_get_name         = Manifest::GetApi<til2cpp_class_get_name>("il2cpp_class_get_name");
    il2cpp_class_get_namespace    = Manifest::GetApi<til2cpp_class_get_namespace>("il2cpp_class_get_namespace");

    il2cpp_class_get_type         = Manifest::GetApi<til2cpp_class_get_type>("il2cpp_class_get_type");
    il2cpp_type_get_object        = Manifest::GetApi<til2cpp_type_get_object>("il2cpp_type_get_object");
    il2cpp_object_get_class       = Manifest::GetApi<til2cpp_object_get_class>("il2cpp_object_get_class");

    byteClass = Manifest::GetClassPtr("System.Byte");
    std::cout << "[Init] System.Byte Il2CppClass* = " << byteClass << std::endl;

    if (!oIl2CppStringNew || !il2cpp_array_new) {
        g_networkStatus = "Error: il2cpp_string_new / il2cpp_array_new missing from manifest.";
        std::cout << "[Init] Allocator APIs missing from manifest: "
                  << "string_new=" << (void*)oIl2CppStringNew
                  << " array_new="  << (void*)il2cpp_array_new << std::endl;
    } else if (!byteClass) {
        g_networkStatus = "Warning: byteClass unresolved — BSON packet sends will short-circuit.";
    } else {
        g_networkStatus = "Manifest helpers wired.";
    }

    if (oIl2CppStringNew) {
        int directResult = ProbeIl2CppStringNew(oIl2CppStringNew);
        if (directResult > 0) {
            g_il2cpp_string_new_validated = true;
            std::cout << "[Init] il2cpp_string_new probe OK (len=" << directResult << ")" << std::endl;
        } else {

            {
                auto cands = Manifest::GetApiCandidates("il2cpp_string_new");
                for (void* c : cands) {
                    if (!c || c == (void*)oIl2CppStringNew) continue;
                    int r = ProbeIl2CppStringNew(c);
                    if (r > 0) {
                        std::cout << "[Init] il2cpp_string_new candidate-probe picked "
                                  << c << " (was " << (void*)oIl2CppStringNew
                                  << ", probe OK len=" << r << ")" << std::endl;
                        oIl2CppStringNew = (tIl2CppStringNew)c;
                        g_il2cpp_string_new_validated = true;
                        break;
                    }
                }
            }

            void* chased = g_il2cpp_string_new_validated
                         ? nullptr
                         : ResolveStringNewWithThunkChase(oIl2CppStringNew);
            if (chased && chased != oIl2CppStringNew) {
                int chasedResult = ProbeIl2CppStringNew(chased);
                if (chasedResult > 0) {
                    std::cout << "[Init] il2cpp_string_new thunk-chased "
                              << oIl2CppStringNew << " -> " << chased
                              << " (probe OK, len=" << chasedResult << ")" << std::endl;
                    oIl2CppStringNew = (tIl2CppStringNew)chased;
                    g_il2cpp_string_new_validated = true;
                }
            }

            if (!g_il2cpp_string_new_validated && directResult < 0) {
                uintptr_t gaBase = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
                uintptr_t gaSize = SehReadModuleSizeOfImage(gaBase);
                if (ProbeIl2CppStringNewLenient(oIl2CppStringNew, gaBase, gaSize)) {
                    g_il2cpp_string_new_validated = true;
                    std::cout << "[Init] il2cpp_string_new lenient-OK — strict probe "
                              << "failed (Unity 6 string layout) but call returns a "
                              << "valid managed object. Trusting manifest RVA "
                              << "(strategy-a-rollback-unverifiable). SEH-guarded in use." << std::endl;
                }
            }

            if (!g_il2cpp_string_new_validated) {
                if (directResult == 0) {
                    std::cout << "[Init] il2cpp_string_new AV on probe — RVA wrong. Thunk-chase exhausted. "
                              << "String-dependent features (Streamer, Multibox, AutoMine warp) disabled." << std::endl;
                } else {
                    std::cout << "[Init] il2cpp_string_new BROKEN: probe returned shape len="
                              << (-directResult) << " (expected 10), lenient check also failed. "
                              << "Thunk-chase exhausted. String-dependent features disabled." << std::endl;
                }
            }
        }
    }
}

inline void FormatJsonPacket(nlohmann::json& j) {
    if (j.is_object()) {
        if (j.contains("$numberLong") && j["$numberLong"].is_string()) {
            try {
                long long val = std::stoll(j["$numberLong"].get<std::string>());
                j = val;
                return;
            }
            catch (...) {}
        }
        for (auto& el : j.items()) {
            FormatJsonPacket(el.value());
        }
    }
    else if (j.is_array()) {
        for (auto& el : j) {
            FormatJsonPacket(el);
        }
    }
    else if (j.is_string()) {
        std::string val = j.get<std::string>();
        if (val == "$timeutc") {
            long long ticks = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 100 + 621355968000000000LL;
            j = ticks;
        }
    }
}

inline void* _SafeIl2cppThreadAttach() {
    if (!IL2CPP::Functions.m_ThreadAttach) return nullptr;
    __try {
        void* dom = IL2CPP::Domain::Get();
        return dom ? IL2CPP::Thread::Attach(dom) : nullptr;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}
inline void _SafeIl2cppThreadDetach(void* t) {
    if (!t || !IL2CPP::Functions.m_ThreadDetach) return;
    __try { IL2CPP::Thread::Detach(t); } __except (EXCEPTION_EXECUTE_HANDLER) {}
}
struct Il2cppThreadScope {
    void* m_thread;
    Il2cppThreadScope()  : m_thread(_SafeIl2cppThreadAttach()) {}
    ~Il2cppThreadScope() { _SafeIl2cppThreadDetach(m_thread); }
    Il2cppThreadScope(const Il2cppThreadScope&)            = delete;
    Il2cppThreadScope& operator=(const Il2cppThreadScope&) = delete;
};

inline void RepeatPacket(const std::string& packetJson) {
    if (!oSimpleBSON_Load || !oAddOneMessageToList || !il2cpp_array_new || !byteClass) {
        g_networkStatus = "Error: Repeater dependencies are not initialized.";
        return;
    }
    if (packetJson.empty()) {
        g_networkStatus = "Error: Repeater content is empty.";
        return;
    }

    size_t currentPos = 0;
    int packetsSent = 0;
    int packetsFailed = 0;
    std::string lastError = "";

    while (currentPos < packetJson.length()) {
        size_t startPos = packetJson.find('{', currentPos);
        if (startPos == std::string::npos) break;

        size_t endPos = std::string::npos;
        int braceCount = 0;
        for (size_t i = startPos; i < packetJson.length(); ++i) {
            if (packetJson[i] == '{') braceCount++;
            else if (packetJson[i] == '}') braceCount--;
            if (braceCount == 0) {
                endPos = i;
                break;
            }
        }

        if (endPos == std::string::npos) {
            lastError = "Malformed JSON: Unmatched braces found.";
            packetsFailed++;
            break;
        }

        std::string singlePacketStr = packetJson.substr(startPos, endPos - startPos + 1);

        try {
            nlohmann::json j = nlohmann::json::parse(singlePacketStr);
            if (!j.is_object()) throw std::runtime_error("Parsed content is not a JSON object.");

            FormatJsonPacket(j);
            std::vector<uint8_t> bson_data = nlohmann::json::to_bson(j);
            Il2CppArray* il2cppByteArray = il2cpp_array_new(byteClass, bson_data.size());
            if (!il2cppByteArray) throw std::runtime_error("Failed to create Il2CppArray.");

            memcpy(il2cppByteArray->items, bson_data.data(), bson_data.size());
            void* bsonPacket = oSimpleBSON_Load(il2cppByteArray);
            if (!bsonPacket) throw std::runtime_error("Game failed to load BSON packet.");

            oAddOneMessageToList(bsonPacket);
            packetsSent++;
        }
        catch (const nlohmann::json::parse_error& e) {
            packetsFailed++;
            lastError = "JSON Parse Error: ";
            lastError += e.what();
        }
        catch (const std::exception& e) {
            packetsFailed++;
            lastError = "Error processing packet: ";
            lastError += e.what();
        }
        currentPos = endPos + 1;
    }

    std::stringstream status;
    status << "Sent " << packetsSent << " packets.";
    if (packetsFailed > 0) {
        status << " Failed: " << packetsFailed << ". Last error: " << lastError;
    }
    g_networkStatus = status.str();
}

namespace FishBot {
    inline void ProcessIncomingPacket(const json& j);
    inline void Tick(float deltaTime);
}

void* Safe_GetInventoryControl_FromPlayer();

namespace AutoFossil {
    inline void Tick(float deltaTime);
    inline void OnEnable();
    inline void OnDisable();
    inline void ProcessIncomingPacket(const json& j);
    inline bool IsBusy();
}

namespace Pnb {
    inline void ScanInventoryTick();

}

inline void AutoFarm_ProcessIncomingPacket(const nlohmann::json&) {}

inline void BankBot_ProcessIncomingPacket(const nlohmann::json& j);

namespace AutoMine {
    inline void OnMineCompleteSWD();
    inline void DiagInspectIncoming(const nlohmann::json& m);

    inline std::atomic<bool> g_botBusy{ false };
}

namespace ReplyDispatch {
    enum Slot { S_MPR = 0, S_CI = 1, S_RA = 2, S_COUNT = 3 };

    struct Pending {
        bool        armed           = false;
        bool        wireDiscovered  = false;
        std::string wireId;
        std::chrono::steady_clock::time_point sentAt;
        int         i_a = 0, i_b = 0;
    };

    inline Pending     g_pending[S_COUNT];
    inline std::mutex  g_mu;
    inline std::atomic<int> g_armedSlot{ -1 };
    inline std::chrono::steady_clock::time_point g_armedUntil;

    inline std::vector<int> g_raBlocks;
    inline std::vector<int> g_raQtys;
    inline int              g_raGemDeltaExpected = 0;
    inline size_t           g_raCursor = 0;

    inline bool TakeRABatch(std::vector<int>& outBlocks,
                            std::vector<int>& outQtys, int& outDelta) {
        std::lock_guard<std::mutex> lk(g_mu);
        if (g_raCursor >= g_raBlocks.size()) return false;
        outBlocks = g_raBlocks;
        outQtys   = g_raQtys;
        outDelta  = g_raGemDeltaExpected;
        g_raCursor = g_raBlocks.size();
        return true;
    }

    using HandlerMPR = void(*)(int pickBT, const json& bson);
    using HandlerCI  = void(*)(int srcBT, int srcIT, const json& bson);
    using HandlerRA  = void(*)(const json& bson);
    inline HandlerMPR g_handlerMPR = nullptr;
    inline HandlerCI  g_handlerCI  = nullptr;
    inline HandlerRA  g_handlerRA  = nullptr;

    inline void SetHandlers(HandlerMPR mpr, HandlerCI ci, HandlerRA ra) {
        g_handlerMPR = mpr; g_handlerCI = ci; g_handlerRA = ra;
    }

    inline void Arm(int slot) {
        g_armedSlot.store(slot, std::memory_order_release);
        g_armedUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
    }

    inline void NoteSendMPR(int pickBT) {
        std::lock_guard<std::mutex> lk(g_mu);
        g_pending[S_MPR].i_a    = pickBT;
        g_pending[S_MPR].sentAt = std::chrono::steady_clock::now();
        g_pending[S_MPR].armed  = true;
        Arm(S_MPR);
    }
    inline void NoteSendCI(int srcBT, int srcIT) {
        std::lock_guard<std::mutex> lk(g_mu);
        g_pending[S_CI].i_a    = srcBT;
        g_pending[S_CI].i_b    = srcIT;
        g_pending[S_CI].sentAt = std::chrono::steady_clock::now();
        g_pending[S_CI].armed  = true;
        Arm(S_CI);
    }

    inline void NoteSendRA(const std::vector<std::pair<int,int>>& gems, int gemDeltaExpected,
                           const char* knownWire = nullptr) {
        std::lock_guard<std::mutex> lk(g_mu);
        g_raBlocks.clear();
        g_raQtys.clear();
        for (auto& g : gems) {
            g_raBlocks.push_back(g.first);
            g_raQtys.push_back(g.second);
        }
        g_raGemDeltaExpected   = gemDeltaExpected;
        g_raCursor             = 0;
        g_pending[S_RA].sentAt = std::chrono::steady_clock::now();
        g_pending[S_RA].armed  = true;
        if (knownWire && *knownWire) {
            g_pending[S_RA].wireId         = knownWire;
            g_pending[S_RA].wireDiscovered = true;

        } else {
            Arm(S_RA);
        }
    }

    inline void OnOutgoing(const std::string& wireId) {
        int slot = g_armedSlot.load(std::memory_order_acquire);
        if (slot < 0) return;
        if (std::chrono::steady_clock::now() > g_armedUntil) {
            g_armedSlot.store(-1, std::memory_order_release);
            return;
        }
        if (wireId.empty()) return;
        std::lock_guard<std::mutex> lk(g_mu);
        if (slot >= 0 && slot < S_COUNT && !g_pending[slot].wireDiscovered) {
            g_pending[slot].wireId         = wireId;
            g_pending[slot].wireDiscovered = true;

            std::cout << "[ReplyDispatch] slot=" << slot
                      << " send-wire = '" << wireId << "'\n";
        }
        g_armedSlot.store(-1, std::memory_order_release);
    }

    inline void OnIncoming(const std::string& id, const json& bson) {
        if (id.empty()) return;

        int  matched = -1;
        int  a = 0, b = 0;
        {
            std::lock_guard<std::mutex> lk(g_mu);
            auto now = std::chrono::steady_clock::now();
            for (int i = 0; i < S_COUNT; ++i) {
                auto& p = g_pending[i];
                if (!p.armed) continue;
                if (!p.wireDiscovered) continue;
                if (p.wireId != id) continue;
                auto age = std::chrono::duration_cast<std::chrono::milliseconds>(now - p.sentAt).count();
                if (age > 10000) { p.armed = false; continue; }
                matched = i; a = p.i_a; b = p.i_b;

                break;
            }
        }
        if (matched < 0) return;

        switch (matched) {
            case S_MPR: if (g_handlerMPR) g_handlerMPR(a, bson);    break;
            case S_CI:  if (g_handlerCI)  g_handlerCI(a, b, bson);  break;
            case S_RA:  if (g_handlerRA)  g_handlerRA(bson);        break;
        }
    }
}

inline void __fastcall hkAddOneMessageToList(void* bsonPacket) {

    if (bsonPacket && oBsonDump) {
        try {
            Il2CppArray* bsonByteArray = oBsonDump(bsonPacket);
            if (bsonByteArray && bsonByteArray->max_length > 0) {
                std::vector<uint8_t> original_bytes((uint8_t*)bsonByteArray->items, (uint8_t*)bsonByteArray->items + bsonByteArray->max_length);
                json j = json::from_bson(original_bytes);

                if (j.is_object() && AutoFossil::IsBusy()
                    && ((j.value("ID", "") == "cvDZ" && j.value("DYws", -1) == 1)
                        || (j.value("ID", "") == "dmTH" && j.value("cDGm", -1) == 1))) {
                    return;
                }

                ReplyDispatch::OnOutgoing(j.value("ID", ""));

                if (j.is_object() && j.contains("PT") && j["PT"].is_number_integer()) {
                    std::string aid = j.value("ID", "");
                    if (!aid.empty()) {
                        std::lock_guard<std::mutex> lk(g_agiCipherMutex);
                        g_agiCipherId = aid;
                        g_haveAgiCipher.store(true, std::memory_order_release);
                    }
                }

                DiceExploit_ProcessPacket(j, false);

                SoilGame_ProcessPacket(j, false);

                FossilDebug_ProcessPacket(j, false);

                if (g_CaptureOutgoing) {
                    std::string packetId = j.value("ID", "NO_ID");
                    bool isIgnored = false;
                    for (const auto& ignoredId : g_packetIdIgnoreList) {
                        if (packetId == ignoredId) { isIgnored = true; break; }
                    }
                    if (!isIgnored) {
                        std::string packetJson = j.dump(4);
                        std::lock_guard<std::mutex> lock(g_PacketLogMutex);
                        g_PacketLog.push_back({ g_LogCounter++, std::chrono::system_clock::now(), PacketDirection::Outgoing, packetId, packetJson });
                        g_newPacketReceived = true;
                    }
                }
            }
        }
        catch (const std::exception&) {}
    }

    return oAddOneMessageToList(bsonPacket);
}

namespace PingProbe {
    typedef void (*tSendPing)(void* methodInfo);
    inline tSendPing oSendPing = nullptr;
    inline std::atomic<int>       g_lastPingMs{ 0 };
    inline std::atomic<long long> g_sentAtMs{ 0 };
    inline std::chrono::steady_clock::time_point g_lastProbe =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);

    inline long long NowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    inline void NoteInbound() {
        long long t0 = g_sentAtMs.exchange(0);
        if (t0 == 0) return;
        int rtt = (int)(NowMs() - t0);
        if (rtt < 0 || rtt >= 60000) return;
        int prev = g_lastPingMs.load();
        g_lastPingMs.store(prev == 0 ? rtt : (int)(prev * 0.7f + rtt * 0.3f));
    }
    void Tick();
}

inline void _ProcessInboundMessage(json& singleMessageJson) {

    ReplyDispatch::OnIncoming(singleMessageJson.value("ID", ""),
                              singleMessageJson);

    DiceExploit_ProcessPacket(singleMessageJson, true);
    SoilGame_ProcessPacket(singleMessageJson, true);
    FossilDebug_ProcessPacket(singleMessageJson, true);
    AutoFossil::ProcessIncomingPacket(singleMessageJson);
    FishBot::ProcessIncomingPacket(singleMessageJson);
    AutoFarm_ProcessIncomingPacket(singleMessageJson);
    BankBot_ProcessIncomingPacket(singleMessageJson);

    if (singleMessageJson.is_object() &&
        singleMessageJson.contains("SWDt") &&
        singleMessageJson.contains("SWDe"))
    {
        int swdt = singleMessageJson.value("SWDt", 0);
        int swde = singleMessageJson.value("SWDe", 0);
        if (swdt == 4 && swde == 4) {
            AutoMine::OnMineCompleteSWD();
        }
    }

    AutoMine::DiagInspectIncoming(singleMessageJson);

    if (g_CaptureIncoming) {
        std::string packetId = singleMessageJson.value("ID", "NO_ID");
        bool isIgnored = false;
        for (const auto& ignoredId : g_packetIdIgnoreList) {
            if (packetId == ignoredId) { isIgnored = true; break; }
        }
        if (!isIgnored) {
            std::string packetJson = singleMessageJson.dump(4);
            std::lock_guard<std::mutex> lock(g_PacketLogMutex);
            g_PacketLog.push_back({ g_LogCounter++, std::chrono::system_clock::now(), PacketDirection::Incoming, packetId, packetJson });
            g_newPacketReceived = true;
        }
    }
}

inline void __fastcall hkHandleMessages(void* thisPtr, void* bsonPacket) {

    if (bsonPacket && oBsonDump) {
        try {
            Il2CppArray* batchByteArray = oBsonDump(bsonPacket);
            if (batchByteArray && batchByteArray->max_length > 0) {
                json batchJson = json::from_bson(std::vector<uint8_t>((uint8_t*)batchByteArray->items, (uint8_t*)batchByteArray->items + batchByteArray->max_length));

                PingProbe::NoteInbound();

                int messageCount = batchJson.value("mc", 0);
                if (messageCount == 0) {
                    messageCount = batchJson.value("sGot", 0);
                }

                if (messageCount == 0 && batchJson.is_object() && batchJson.contains("ID")) {
                    _ProcessInboundMessage(batchJson);
                }
                for (int i = 0; i < messageCount; ++i) {
                    std::string key = "m" + std::to_string(i);
                    if (batchJson.contains(key)) {
                        json singleMessageJson = batchJson[key];

                        ReplyDispatch::OnIncoming(singleMessageJson.value("ID", ""),
                                                  singleMessageJson);

                        DiceExploit_ProcessPacket(singleMessageJson, true);

                        SoilGame_ProcessPacket(singleMessageJson, true);

                        FossilDebug_ProcessPacket(singleMessageJson, true);

                        AutoFossil::ProcessIncomingPacket(singleMessageJson);

                        FishBot::ProcessIncomingPacket(singleMessageJson);

                        AutoFarm_ProcessIncomingPacket(singleMessageJson);

                        BankBot_ProcessIncomingPacket(singleMessageJson);

                        if (singleMessageJson.is_object() &&
                            singleMessageJson.contains("SWDt") &&
                            singleMessageJson.contains("SWDe"))
                        {
                            int swdt = singleMessageJson.value("SWDt", 0);
                            int swde = singleMessageJson.value("SWDe", 0);
                            if (swdt == 4 && swde == 4) {
                                AutoMine::OnMineCompleteSWD();
                            }
                        }

                        AutoMine::DiagInspectIncoming(singleMessageJson);

                        if (g_CaptureIncoming) {
                            std::string packetId = singleMessageJson.value("ID", "NO_ID");
                            bool isIgnored = false;
                            for (const auto& ignoredId : g_packetIdIgnoreList) {
                                if (packetId == ignoredId) { isIgnored = true; break; }
                            }
                            if (!isIgnored) {
                                std::string packetJson = singleMessageJson.dump(4);
                                std::lock_guard<std::mutex> lock(g_PacketLogMutex);
                                g_PacketLog.push_back({ g_LogCounter++, std::chrono::system_clock::now(), PacketDirection::Incoming, packetId, packetJson });
                                g_newPacketReceived = true;
                            }
                        }
                    }
                }
            }
        }
        catch (const std::exception&) {}
    }

    return oHandleMessages(thisPtr, bsonPacket);
}

inline bool g_forceWeather = false;
inline World::WeatherType g_forcedWeatherType = World::WeatherType::WeatherNone;
inline World::WeatherType g_currentWeatherType = World::WeatherType::WeatherNone;
inline bool g_forceLighting = false;
inline World::LightingType g_forcedLightingType = World::LightingType::LightingNone;

typedef void(__fastcall* tChangeWeather)(void* instance, int weatherType, void* methodInfo);
typedef void(__fastcall* tChangeLighting)(void* instance, int lightingType, void* methodInfo);
typedef bool(__fastcall* tChangeBackground)(void* instance, int backgroundType, void* methodInfo);

inline tChangeWeather oChangeWeather = nullptr;
inline tChangeLighting oChangeLighting = nullptr;
inline tChangeBackground oChangeBackground = nullptr;

inline void __fastcall hkChangeWeather(void* instance, int weatherType, void* methodInfo) {
    g_currentWeatherType = (World::WeatherType)weatherType;
    if (g_forceWeather) oChangeWeather(instance, (int)g_forcedWeatherType, methodInfo);
    else oChangeWeather(instance, weatherType, methodInfo);
}

inline void __fastcall hkChangeLighting(void* instance, int lightingType, void* methodInfo) {
    if (g_forceLighting) oChangeLighting(instance, (int)g_forcedLightingType, methodInfo);
    else oChangeLighting(instance, lightingType, methodInfo);
}

inline std::vector<void*> g_PlayerInstances;
inline std::mutex g_PlayerInstancesMutex;
inline std::mutex g_UnityObjectMutex;

inline std::atomic<bool> g_isTransitioning{false};

inline bool g_pathRendererResetPending = false;

inline bool g_autoNetherResetPending  = false;

inline HWND g_OverlayHwnd = nullptr;

inline bool g_uncapFps       = false;
inline int  g_targetFps      = 240;
inline int  g_lastAppliedFps = -2;
inline void* g_LocalPlayer = nullptr;
inline void* g_WorldController = nullptr;

inline void* g_NetworkClient = nullptr;

inline constexpr uintptr_t IL2CPP_KLASS_STATIC_FIELDS_OFFSET = 0xB8;

inline void* SafeReadStaticInstance(void* klass, uintptr_t staticOffset) {
    if (!klass) return nullptr;
    __try {
        void* sb = *(void**)((uint8_t*)klass + IL2CPP_KLASS_STATIC_FIELDS_OFFSET);
        if (!sb) return nullptr;
        return *(void**)((uint8_t*)sb + staticOffset);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

inline void TryResolveNetworkClient() {
    if (g_NetworkClient) return;
    void* klass = Manifest::GetClassPtr("ControllerHelper");
    if (!klass) return;
    auto offOpt = Manifest::GetStaticFieldOffset("ControllerHelper", "networkClient");
    if (!offOpt) return;
    g_NetworkClient = SafeReadStaticInstance(klass, *offOpt);
}

inline std::vector<void*> g_AIInstances;
inline std::mutex g_AIInstancesMutex;

inline std::atomic<float> g_cachedCamX    { 0.0f };
inline std::atomic<float> g_cachedCamY    { 0.0f };
inline std::atomic<float> g_cachedCamOrtho{ 5.0f };
inline std::atomic<bool>  g_cachedCamValid{ false };

inline std::unordered_map<void*, Vector3> g_playerPosCache;

inline std::unordered_map<void*, Vector3> g_aiPosCache;

struct GemstoneInfo {
    Vector2i mapPoint;
    World::BlockType blockType;

    bool operator==(const GemstoneInfo& other) const {
        return mapPoint.x == other.mapPoint.x && mapPoint.y == other.mapPoint.y;
    }
};

inline std::vector<GemstoneInfo> g_gemstoneLocations;
inline std::mutex g_gemstoneLocationsMutex;

struct NuggetInfo {
    Vector2i mapPoint;
    World::BlockType blockType;
    bool operator==(const NuggetInfo& o) const {
        return mapPoint.x == o.mapPoint.x && mapPoint.y == o.mapPoint.y;
    }
};
inline std::vector<NuggetInfo> g_nuggetLocations;
inline std::mutex              g_nuggetLocationsMutex;

struct LightCrystalInfo {
    Vector2i mapPoint;
    World::BlockType blockType;

    bool operator==(const LightCrystalInfo& other) const {
        return mapPoint.x == other.mapPoint.x && mapPoint.y == other.mapPoint.y;
    }
};

inline std::vector<LightCrystalInfo> g_lightCrystalLocations;
inline std::mutex g_lightCrystalLocationsMutex;

struct PotInfo {
    Vector2i mapPoint;
    World::BlockType blockType;

    bool operator==(const PotInfo& other) const {
        return mapPoint.x == other.mapPoint.x && mapPoint.y == other.mapPoint.y;
    }
};

inline std::vector<PotInfo> g_potLocations;
inline std::mutex g_potLocationsMutex;

struct TreasureInfo {
    Vector2i mapPoint;
    World::BlockType blockType;

    bool operator==(const TreasureInfo& other) const {
        return mapPoint.x == other.mapPoint.x && mapPoint.y == other.mapPoint.y;
    }
};

inline std::vector<TreasureInfo> g_treasureLocations;
inline std::mutex g_treasureLocationsMutex;
inline bool AntiBounce = false;
inline bool AntiBounceIgnoreMushrooms = false;

inline bool IsMushroomBlockType(int bt) {
    switch (bt) {
        case 75:
        case 757:
        case 1382:
        case 1626:
        case 1627:
        case 2998:
            return true;
        default:
            return false;
    }
}
inline bool antifan = false;
inline bool InfiniteJetpack = false;
inline bool GodModeEnabled = false;

inline bool g_debugDamageLog = false;

inline bool g_actkBypass = true;

inline bool  g_playerRunSpeedHack = false;
inline float g_playerRunSpeedMul  = 1.0f;
inline bool AntiCensor = false;
inline bool AntiDarkness = false;
inline bool PlaceSeedAnywhere = false;
inline bool StopFishMovement = false;
inline bool FreecamEnabled = false;
inline bool EnableJumpModeOverride = false;
inline bool fastHitEnabled = false;
inline bool ExtraFasthitEnabled = false;
inline bool AutoFishEnabled = false;
inline bool fullSimFishBot = false;
inline bool autoLandFish = false;
inline bool unlockAllRecipes = false;
inline bool noBlockKill = false;
inline bool DisableEnemies = false;
inline bool DisableGassy = false;
inline bool EnableGravityOverride = false;
inline float runSpeedMultiplier = 1.0f;
extern inline PlayerJumpMode SelectedJumpMode = PlayerJumpMode::Normal;
inline Vector3 freecamPosition = { 0, 0, 0 };
inline float freecamSpeed = 0.01f;
inline bool freecamInitialized = false;
inline std::atomic<bool> g_IsInWorld = false;
inline bool unlimitedOxygenEnabled = false;
inline bool g_BlockOnPlayer = false;

inline void PingProbe::Tick() {
    if (!oSendPing || !g_IsInWorld.load()) return;
    auto now = std::chrono::steady_clock::now();
    if (now - g_lastProbe < std::chrono::seconds(3)) return;
    g_lastProbe = now;
    if (g_sentAtMs.load() != 0) return;
    g_sentAtMs.store(NowMs());
    __try { oSendPing(nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        g_sentAtMs.store(0);
        oSendPing = nullptr;
    }
}

namespace Offsets {
    constexpr uintptr_t WorldObject = 0x38;
    constexpr uintptr_t WorldBlockLayer = 0x40;
    constexpr uintptr_t WorldSize = 0xE0;
    constexpr uintptr_t CollectablesList = 0x150;
    constexpr uintptr_t AIBase_enemyType = 0x10;
    constexpr uintptr_t AIBase_id = 0x14;
    constexpr uintptr_t AIBase_health = 0x70;
    constexpr uintptr_t AIEnemyMonoBehaviourBase_tempPosition = 0xF0;
    constexpr uintptr_t AIEnemyMonoBehaviourBase_aiBase = 0x118;
    constexpr uintptr_t Door_isLocked = 0x20;
    constexpr uintptr_t World_collectablesList = 0x150;
    constexpr uintptr_t WorldController_world = 0x38;
    constexpr uintptr_t WorldController_aiEnemies = 0x710;
    constexpr uintptr_t Player_velocity = 0x410;
    constexpr uintptr_t World_worldBlockLayer = 0x40;
    constexpr uintptr_t World_worldItemDataLayer = 0x60;
    constexpr uintptr_t World_worldSize = 0xE0;
    constexpr uintptr_t Player_isSubAquaticPerkActive = 0x1284;
    constexpr uintptr_t NetworkPlayers_otherPlayers = 0x0;
    constexpr uintptr_t NetworkPlayer_playerScript = 0x18;
    constexpr uintptr_t PlayerData_playerId = 0x70;
    constexpr uintptr_t List_items = 0x10;
    constexpr uintptr_t List_size = 0x18;
    constexpr uintptr_t NetworkPlayers_Static = 0x1A173F0;
    constexpr uintptr_t ControllerHelper_playerNamesManager = 0xF0;
    constexpr uintptr_t ControllerHelper_TypeInfo_RVA = 0x2789888;
    constexpr uintptr_t Player_myPlayerData = 0x58;

    constexpr uintptr_t playerChangeToSleepSeconds = 0xE82B0;
    constexpr uintptr_t playerInactivitySeconds = 0xE8320;
    constexpr uintptr_t Player_inventoryControl = 0x9E8;
    constexpr uintptr_t PlayerData_currentSelection = 0x3C0;
    constexpr uintptr_t PlayerData_xpAmount = 0x374;

    constexpr uintptr_t Player_currentPlayerMapPoint      = 0x550;
    constexpr uintptr_t Player_currentPlayerAboveMapPoint = 0x580;
    constexpr uintptr_t Player_currentPlayerRightMapPoint = 0x598;
    constexpr uintptr_t Player_currentPlayerLeftMapPoint  = 0x5B0;
}

inline uintptr_t g_off_AITempPosition         = Offsets::AIEnemyMonoBehaviourBase_tempPosition;
inline uintptr_t g_off_AIBase                 = Offsets::AIEnemyMonoBehaviourBase_aiBase;
inline uintptr_t g_off_WCWorld                = Offsets::WorldController_world;
inline uintptr_t g_off_WCAIEnemies            = Offsets::WorldController_aiEnemies;
inline uintptr_t g_off_WorldBlockLayer        = Offsets::World_worldBlockLayer;
inline uintptr_t g_off_WorldSize              = Offsets::World_worldSize;
inline uintptr_t g_off_WorldCollectablesList  = Offsets::World_collectablesList;

inline uintptr_t g_off_PlayerNameTMP          = 0xD20;

inline uintptr_t g_off_PlayerJumpMode         = 0x8c;

inline uintptr_t g_off_Player_velocity                  = 0x410;
inline uintptr_t g_off_Player_isSubAquaticPerkActive    = 0x1284;
inline uintptr_t g_off_Player_currentPlayerMapPoint     = 0x550;
inline uintptr_t g_off_Player_currentPlayerAboveMapPoint= 0x580;
inline uintptr_t g_off_Player_currentPlayerRightMapPoint= 0x598;
inline uintptr_t g_off_Player_currentPlayerLeftMapPoint = 0x5B0;
inline uintptr_t g_off_Player_rocketFuelConsumption     = 0x1FC;
inline uintptr_t g_off_Player_rocketFuel                = 0x238;
inline uintptr_t g_off_Player_rocketFuelConsumption60   = 0x24C;
inline uintptr_t g_off_Player_fullRocketFuel            = 0x300;
inline uintptr_t g_off_WCWorldField                     = 0x38;

inline bool g_needsScanForGems = true;
inline bool g_isInMineworld = false;

typedef void(__fastcall* tSetGravityMode)(void* instance, GravityMode gravityMode);

typedef void(__fastcall* tSetWorldLighting)(void* instance, int lightingType, void* methodInfo);

typedef void(__fastcall* tInstantiateFogOfWar)(void* instance, void* methodInfo);
typedef bool(__fastcall* tIsFishStrikeActive)(void* controller);
typedef void(__fastcall* tUpdate)(void* instance);
typedef void(__fastcall* tOnDestroy)(void* instance);
typedef bool(__fastcall* tIsBlockPinball)(int);
typedef bool(__fastcall* tIsBlockHot)(int);
typedef bool(__fastcall* tIsBlockSpring)(int);
typedef bool(__fastcall* tIsBlockTrampolin)(int);
typedef bool(__fastcall* tIsBlockElastic)(int);
typedef bool(__fastcall* tIsBlockWind)(int);
typedef bool(__fastcall* tShouldBelowBlockDoBounce)(void*);
typedef bool(__fastcall* tHitPlayerFromBlock)(void*, void*, int, Vector2i, bool);
typedef bool(__fastcall* tHitPlayerFromBlockHF)(void*, void*, int, int, Vector2i);
typedef bool(__fastcall* tHitPlayerFromAIEnemy)(void*, void*, int, int);
typedef bool(__fastcall* tHitPlayerFromExplosion)(void*, void*, int, Vector2i, Vector3, void*);
typedef bool(__fastcall* tPlayerData_HitPlayer)(void*, void*, int, bool, bool);
typedef void(__fastcall* tPlayerCausePoisonedBT)(void*, void*, int);
typedef void(__fastcall* tPlayerCausePoisonedAI)(void*, void*, int);
typedef void(__fastcall* tPlayerCausePoisonedF) (void*, void*, float);
typedef void(__fastcall* tPlayerKillPlayer)(void*, void*, int);
typedef void(__fastcall* tPlayerDeathHelper)(void*);
typedef bool(__fastcall* tPlayerCheckDeathByColliderHelper)(void*, void*, Vector2i);
typedef void(__fastcall* tPlayerDoDyingAnimation)(void*, void*, int);
typedef void(__fastcall* tPlayerDoTakeHitStunnedAnim)(void*, void*, int);
typedef void(__fastcall* tPlayerWaitRespawn)(void*);

typedef void(__fastcall* tKC_SetZoomValueTo)(void*, float, void*);
typedef void(__fastcall* tKC_UpdateZoomByZoomValue)(void*, void*);
typedef void(__fastcall* tSetOrthographicSize)(void*, float, void*);

typedef void(__fastcall* tInvDropButtonClicked)(void*, void*);
typedef void(__fastcall* tInvActualDrop)(void*, void*, PlayerData_InventoryKey, int, void*);

typedef void(__fastcall* tWCSpeedHackDetected)(void*, void*);

typedef void(__fastcall* tACTkDetectorVoid)(void* detector, void* methodInfo);
typedef bool(__fastcall* tACTkIsTicksCheated)(void* detector, long long curTicks, long long* prevTicks, long long reliableDelta, void* methodInfo);
typedef void(__fastcall* tACTkObsOnCheat1)(void* detector, void* obscuredType, void* decrypted, void* fake, void* methodInfo);
typedef void(__fastcall* tACTkObsOnCheat2)(void* detector, void* obscuredType, bool hash, void* decrypted, void* fake, void* methodInfo);

typedef float(__fastcall* tPlayerGetRunSpeed)(void* player, void* methodInfo);

typedef void(__fastcall* tPlayerWarpPlayer)(void* player, int x, int y, void* methodInfo);
typedef void(__fastcall* tOutSendCheckpoint)(Vector2i mp, void* methodInfo);
typedef void(__fastcall* tOutSendResurrect)(long long ticks, Vector2i mp, void* methodInfo);

typedef bool(__fastcall* tPlayerHitBlock)(
    void* player,
    Vector2i mapPoint,
    int topArmBlockType,
    void* playerNameStr,
    bool isLocalPlayer,
    void* playerData,
    bool extraBool,
    void* methodInfo);

typedef bool(__fastcall* tPlayerMineBlockWithTool)(
    void* player,
    float floatArg,
    bool boolArg1,
    Vector2i vec2iArg,
    int blockTypeArg,
    bool boolArg2,
    bool boolArg3,
    void* methodInfo);

typedef const char* (__fastcall* tCensor)(const char*);
typedef bool(__fastcall* tCanPlaceSeedTopOf)(int);
typedef void(__fastcall* tRandomizeFishTargetPoint)(void*, bool);
typedef void(__fastcall* tUpdateCameraPosition)(void*);
typedef void(__fastcall* tForceUpdatePosition)(void*, bool);
typedef float(__fastcall* tGetBlockRunSpeed)(int);
typedef float(__cdecl* tGetCooldownToUseFist)(float multiplier);
typedef Vector3(__fastcall* tWorldToScreenPoint)(void* camera, Vector3 position);
typedef Vector3(__fastcall* tGetPosition)(void* instance);
typedef void* (__fastcall* tGetTransform)(void* instance);
typedef void* (__fastcall* tGetMainCamera)();
typedef float(__fastcall* tGetOrthographicSize)(void* instance);
typedef void(__fastcall* tSetPosition)(void* instance, Vector3 value);
typedef Vector3(__fastcall* tScreenToWorldPoint)(void* __this, Vector3 screenPosition);
typedef void(__fastcall* tSetVelocity)(void* player, Vector3 newVelocity);
typedef bool(__fastcall* tHasUnlockedRecipe)(void* thisptr, void* edx, int blockType);
typedef void(__fastcall* tDeathByColliderInCollider)(void* thisptr, void* edx, Vector2i mapPoint);
typedef void(__fastcall* tAIEnemyMonoBehaviourBase_Start)(void* instance);
typedef void(__fastcall* tDeactivateAIEnemy)(void* instance);
typedef bool(__fastcall* tIsPlayerInMapPoint)(void* instance, Vector2i mapPoint);
typedef bool(__fastcall* tTouchDamageHelper)(void* instance);
typedef float(__cdecl* tGetOxygenConsumptionSpeedMultiplier)(int blockType);
typedef Vector2i(__fastcall* tConvertWorldPointToMapPoint)(void* instance, Vector3 worldPoint);
typedef Vector3(__fastcall* tConvertMapPointToWorldPoint_V3)(void* instance, Vector2i mapPoint);

inline tSetGravityMode oSetGravityMode = nullptr;
inline tSetWorldLighting oSetWorldLighting = nullptr;
inline tInstantiateFogOfWar oInstantiateFogOfWar = nullptr;
inline tUpdate oUpdate = nullptr;
inline tUpdate oPlayerUpdate = nullptr;
inline tUpdate oWorldControllerUpdate = nullptr;
inline tUpdate oAIUpdate = nullptr;
inline tOnDestroy oWorldControllerOnDestroy = nullptr;
inline tOnDestroy oPlayerOnDestroy = nullptr;
inline tOnDestroy oAIOnDestroy = nullptr;
inline tIsBlockPinball oIsBlockPinball = nullptr;
inline tIsBlockHot oIsBlockHot = nullptr;
inline tIsBlockSpring oIsBlockSpring = nullptr;
inline tIsBlockTrampolin oIsBlockTrampolin = nullptr;
inline tIsBlockElastic oIsBlockElastic = nullptr;
inline tIsBlockWind oIsBlockWind = nullptr;
inline tShouldBelowBlockDoBounce oShouldBelowBlockDoBounce = nullptr;
inline tHitPlayerFromBlock oHitPlayerFromBlock = nullptr;
inline tHitPlayerFromBlockHF oHitPlayerFromBlockHF = nullptr;
inline tHitPlayerFromAIEnemy oHitPlayerFromAIEnemy = nullptr;
inline tHitPlayerFromExplosion oHitPlayerFromExplosion = nullptr;
inline tPlayerData_HitPlayer oPlayerData_HitPlayer = nullptr;
inline tPlayerCausePoisonedBT oPlayerCausePoisonedBT = nullptr;
inline tPlayerCausePoisonedAI oPlayerCausePoisonedAI = nullptr;
inline tPlayerCausePoisonedF  oPlayerCausePoisonedF  = nullptr;
inline tPlayerKillPlayer      oPlayerKillPlayer      = nullptr;
inline tPlayerDeathHelper     oPlayerDeathHelper     = nullptr;
inline tPlayerCheckDeathByColliderHelper oPlayerCheckDeathByColliderHelper = nullptr;
inline tPlayerDoDyingAnimation         oPlayerDoDyingAnimation       = nullptr;
inline tPlayerDoTakeHitStunnedAnim     oPlayerDoTakeHitStunnedAnim   = nullptr;
inline tPlayerWaitRespawn              oPlayerWaitRespawn            = nullptr;
inline tKC_SetZoomValueTo         oKC_SetZoomValueTo         = nullptr;
inline tKC_UpdateZoomByZoomValue  oKC_UpdateZoomByZoomValue  = nullptr;
inline tSetOrthographicSize       oSetOrthographicSize       = nullptr;
inline tInvDropButtonClicked      oInvDropButtonClicked      = nullptr;
inline tInvActualDrop             oInvActualDrop             = nullptr;
inline tWCSpeedHackDetected       oWCSpeedHackDetected       = nullptr;

typedef Il2CppString* (__fastcall* tTM_GetBlockTypeName)(int blockType, void* methodInfo);
inline tTM_GetBlockTypeName        oTM_GetBlockTypeName        = nullptr;

inline const char* BlockName(int bt) {
    if (!oTM_GetBlockTypeName) return "?";
    Il2CppString* s = nullptr;
    __try { s = oTM_GetBlockTypeName(bt, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return "?"; }
    if (!s) return "?";

    static thread_local char buf[96];
    __try {
        int len = *(int*)((uintptr_t)s + 0x10);
        if (len < 0 || len > 90) len = 90;
        const wchar_t* w = (const wchar_t*)((uintptr_t)s + 0x14);
        int i = 0;
        for (; i < len; ++i) {
            wchar_t c = w[i];
            buf[i] = (c >= 32 && c < 127) ? (char)c : '?';
        }
        buf[i] = '\0';
    } __except (EXCEPTION_EXECUTE_HANDLER) { return "?"; }
    return buf;
}

inline int g_lastDamageBT     = -1;
inline int g_lastDamageHF     = 0;
inline Vector2i g_lastDamageMp = { 0, 0 };

inline int SehReadFgBlock(int tx, int ty) {
    int bt = -1;
    if (!g_WorldController || !g_off_WCWorld) return -1;
    __try {
        void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (!worldObj || IsBadReadPtr(worldObj, sizeof(void*))) return -1;
        void* fg = *(void**)((uintptr_t)worldObj + g_off_WorldBlockLayer);
        if (!fg || IsBadReadPtr(fg, 0x28)) return -1;
        int wsx = *(int*)((uintptr_t)fg + 0x18);
        void** cols = (void**)((uintptr_t)fg + 0x20);
        if (tx < 0 || tx >= wsx) return -1;
        void* col = cols[tx];
        if (!col || IsBadReadPtr(col, 0x28)) return -1;
        int wsy = *(int*)((uintptr_t)col + 0x18);
        if (ty < 0 || ty >= wsy) return -1;
        char* colData = (char*)((uintptr_t)col + 0x20);
        bt = *(int*)((uintptr_t)colData + ty * 0x28);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
    return bt;
}

extern inline tGetTransform oGetTransform;
extern inline tGetPosition  oGetPosition;

inline bool SehGetPlayerTile(int& outTx, int& outTy) {
    outTx = -1;
    outTy = -1;
    if (!g_LocalPlayer || !oGetTransform || !oGetPosition) return false;
    __try {
        void* tr = oGetTransform(g_LocalPlayer);
        if (!tr) return false;
        Vector3 pos = oGetPosition(tr);
        const float tileW = 0.64f;
        outTx = (int)(pos.x / tileW);

        outTy = (int)((pos.y + tileW * 0.5f) / tileW);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}

struct DumpResolved {
    void** cols;
    int    wsx;
    int    wsy;
    int    errKind;
};
inline DumpResolved Seh_ResolveBlockLayer() {
    DumpResolved r{ nullptr, 0, 0, 4 };
    if (!g_WorldController || !g_off_WCWorld) return r;
    __try {
        void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (!worldObj) { r.errKind = 1; return r; }
        void* fg = *(void**)((uintptr_t)worldObj + g_off_WorldBlockLayer);
        if (!fg) { r.errKind = 1; return r; }
        int wsx = *(int*)((uintptr_t)fg + 0x18);
        if (wsx <= 0 || wsx > 1000) { r.errKind = 2; return r; }
        void** cols = (void**)((uintptr_t)fg + 0x20);
        int wsy = 0;
        for (int i = 0; i < wsx && wsy == 0; ++i)
            if (cols[i]) wsy = *(int*)((uintptr_t)cols[i] + 0x18);
        if (wsy <= 0 || wsy > 1000) { r.errKind = 3; return r; }
        r.cols = cols; r.wsx = wsx; r.wsy = wsy; r.errKind = 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) { r.errKind = 4; }
    return r;
}

inline int Seh_ReadColTileBT(void** cols, int x, int y) {
    int bt = 0;
    __try {
        void* col = cols[x];
        if (!col) return 0;
        char* colData = (char*)((uintptr_t)col + 0x20);
        bt = *(int*)((uintptr_t)colData + y * 0x28);
    } __except (EXCEPTION_EXECUTE_HANDLER) { bt = 0; }
    return bt;
}

inline std::string DumpWorldBlocksToJson();

inline std::string DumpBlockPredicatesToJson();

inline void LogBlockContext(const char* tag, int sourceX) {
    int ptx = -1, pty = -1;
    if (!SehGetPlayerTile(ptx, pty)) {
        std::cout << "[CTX] " << tag << " player_tile=? (transform read failed)\n";
        return;
    }
    int playerBT = SehReadFgBlock(ptx, pty);
    std::cout << "[CTX] " << tag
              << " player_tile=(" << ptx << "," << pty << ")"
              << " fg_bt=" << playerBT
              << " (" << (playerBT >= 0 ? BlockName(playerBT) : "?") << ")";
    if (sourceX >= 0) {
        int srcBT = SehReadFgBlock(sourceX, pty);
        std::cout << "  damage_src=(" << sourceX << "," << pty << ")"
                  << " src_bt=" << srcBT
                  << " (" << (srcBT >= 0 ? BlockName(srcBT) : "?") << ")";
    }
    std::cout << "\n";
}

inline tACTkDetectorVoid    oACTkSpdStartAuto        = nullptr;
inline tACTkDetectorVoid    oACTkTimeStartAuto       = nullptr;
inline tACTkDetectorVoid    oACTkInjStartAuto        = nullptr;
inline tACTkDetectorVoid    oACTkObsStartAuto        = nullptr;
inline tACTkDetectorVoid    oACTkWallStartAuto       = nullptr;
inline tACTkIsTicksCheated  oACTkSpdIsTicksCheated   = nullptr;
inline tACTkObsOnCheat1     oACTkObsOnCheat1         = nullptr;
inline tACTkObsOnCheat2     oACTkObsOnCheat2         = nullptr;

inline tPlayerGetRunSpeed   oPlayerGetRunSpeed       = nullptr;

inline tPlayerWarpPlayer    oPlayerWarpPlayer        = nullptr;
inline tOutSendCheckpoint   oOutSendCheckpoint       = nullptr;
inline tOutSendResurrect    oOutSendResurrect        = nullptr;

inline tPlayerHitBlock        oPlayerHitBlock           = nullptr;
inline tPlayerMineBlockWithTool oPlayerMineBlockWithTool = nullptr;

inline tCensor oCensor = nullptr;
inline tCanPlaceSeedTopOf oCanPlaceSeedTopOf = nullptr;
inline tRandomizeFishTargetPoint oRandomizeFishTargetPoint = nullptr;
inline tUpdateCameraPosition oUpdateCameraPosition = nullptr;
inline tForceUpdatePosition oForceUpdatePosition = nullptr;
inline tGetBlockRunSpeed oGetBlockRunSpeed = nullptr;
inline tGetCooldownToUseFist oGetCooldownToUseFist = nullptr;
inline tWorldToScreenPoint oWorldToScreenPoint = nullptr;
inline tGetPosition oGetPosition = nullptr;
inline tGetTransform oGetTransform = nullptr;
inline tGetMainCamera oGetMainCamera = nullptr;
inline tGetOrthographicSize oGetOrthographicSize = nullptr;
inline tSetPosition oSetPosition = nullptr;
inline tScreenToWorldPoint oScreenToWorldPoint = nullptr;
inline tSetVelocity oSetVelocity = nullptr;
inline tHasUnlockedRecipe oHasUnlockedRecipe = nullptr;
inline tDeathByColliderInCollider oDeathByColliderInCollider = nullptr;
inline tAIEnemyMonoBehaviourBase_Start oAIEnemyMonoBehaviourBase_Start = nullptr;
inline tDeactivateAIEnemy oDeactivateAIEnemy = nullptr;
inline tTouchDamageHelper oTouchDamageHelper = nullptr;
inline tGetOxygenConsumptionSpeedMultiplier oGetOxygenConsumptionSpeedMultiplier = nullptr;
inline tIsPlayerInMapPoint oIsPlayerInMapPoint = nullptr;
inline tConvertWorldPointToMapPoint oConvertWorldPointToMapPoint = nullptr;
inline tConvertMapPointToWorldPoint_V3 oConvertMapPointToWorldPoint_V3 = nullptr;

inline bool g_pfGravityOverridden = false;
typedef float(__fastcall* tCfgGetBlockGravity)(int blockType, int gravityMode, int fpsSetting);
inline tCfgGetBlockGravity oCfgGetBlockGravity = nullptr;
inline float __fastcall hkCfgGetBlockGravity(int blockType, int gravityMode, int fpsSetting) {
    if (g_pfGravityOverridden) return 0.f;
    return oCfgGetBlockGravity ? oCfgGetBlockGravity(blockType, gravityMode, fpsSetting) : 0.f;
}

inline float __cdecl hkGetOxygenConsumptionSpeedMultiplier(int blockType) {
    if (unlimitedOxygenEnabled) return 0.0f;
    return oGetOxygenConsumptionSpeedMultiplier(blockType);
}

inline bool __fastcall hkTouchDamageHelper(void* instance) {

    if (g_debugDamageLog) {
        std::cout << "[DMG] TouchDamageHelper instance=" << instance
                  << " godmode=" << (GodModeEnabled ? "1" : "0")
                  << " -> " << (GodModeEnabled ? "BLOCKED" : "passthrough") << "\n";
    }
    return GodModeEnabled ? false : oTouchDamageHelper(instance);
}

inline void SendKeyToGame(WORD vk_key, bool press_down) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk_key;
    if (!press_down) input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

inline bool IsGemstone(World::BlockType type) {
    int v = (int)type;
    if (v >= 4012 && v <= 4056) return true;
    if (v >= 3995 && v <= 4003) return true;
    return false;
}

inline bool IsLightCrystal(World::BlockType type) {
    switch (type) {
    case World::OrbLightingMining:
    case World::MiningLightCrystalSmall:
    case World::MiningLightCrystalMedium:
    case World::MiningLightCrystalLarge:
    case World::MiningTimeCrystalSmall:
    case World::MiningTimeCrystalMedium:
    case World::MiningTimeCrystalLarge:
        return true;
    default:
        return false;
    }
}

inline bool IsPot(World::BlockType type) {
    switch (type) {
    case World::CrackedPottery:
        return true;
    default:
        return false;
    }
}

inline bool IsTreasure(World::BlockType type) {
    switch (type) {
    case World::NetherTreasure:
        return true;
    default:
        return false;
    }
}

inline void ChangeGravity(GravityMode newGravity) {
    if (g_WorldController && oSetGravityMode) {

        void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (worldObject) oSetGravityMode(worldObject, newGravity);
    }
}

inline bool GetPlayerMapPoint(Vector2i& mapPoint) {
    if (!g_LocalPlayer || !oGetTransform || !oGetPosition || !oConvertWorldPointToMapPoint || !g_WorldController) return false;
    Vector3 playerWorldPos = oGetPosition(oGetTransform(g_LocalPlayer));
    mapPoint = oConvertWorldPointToMapPoint(g_WorldController, playerWorldPos);
    return true;
}

inline std::string g_currentWorldNameCache;
inline std::mutex  g_currentWorldNameCacheMtx;

inline std::string GetCurrentWorldName() {
    if (!g_WorldController || IsBadReadPtr(g_WorldController, sizeof(void*))) return "";

    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject || IsBadReadPtr(worldObject, sizeof(void*))) return "";
    void* pStringObject = *(void**)((uintptr_t)worldObject + 0xD8);
    if (!pStringObject || IsBadReadPtr(pStringObject, 0x14)) return "";
    int length = *(int*)((uintptr_t)pStringObject + 0x10);
    if (length <= 0 || length > 50) return "";
    wchar_t* worldNameWChars = (wchar_t*)((uintptr_t)pStringObject + 0x14);
    if (IsBadReadPtr(worldNameWChars, length * sizeof(wchar_t))) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, worldNameWChars, length, NULL, 0, NULL, NULL);
    if (size_needed == 0) return "";
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, worldNameWChars, length, &result[0], size_needed, NULL, NULL);
    return result;
}

inline std::string GetCachedWorldName() {
    std::lock_guard<std::mutex> lk(g_currentWorldNameCacheMtx);
    return g_currentWorldNameCache;
}

inline void UpdateCachedWorldName() {
    std::string n = GetCurrentWorldName();
    if (n.empty()) return;
    std::lock_guard<std::mutex> lk(g_currentWorldNameCacheMtx);
    g_currentWorldNameCache = n;
}

inline std::string DumpWorldBlocksToJson() {
    if (!g_WorldController || !g_off_WCWorld) return "WC not ready";

    std::string worldName = GetCachedWorldName();
    if (worldName.empty()) worldName = GetCurrentWorldName();
    if (worldName.empty()) worldName = "unknown_world";

    std::string safeName;
    for (char c : worldName) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-') safeName += c;
        else safeName += '_';
    }
    if (safeName.empty()) safeName = "unknown_world";

    char appData[MAX_PATH] = {0};
    if (!GetEnvironmentVariableA("APPDATA", appData, MAX_PATH) || !appData[0]) {
        return "no APPDATA";
    }
    std::string baseDir = std::string(appData) + "\\ZMod";
    CreateDirectoryA(baseDir.c_str(), nullptr);
    std::string dumpDir = baseDir + "\\dumps";
    CreateDirectoryA(dumpDir.c_str(), nullptr);
    std::string outPath = dumpDir + "\\" + safeName + ".json";

    DumpResolved res = Seh_ResolveBlockLayer();
    switch (res.errKind) {
        case 1: return "fg layer null";
        case 2: return "bad worldSizeX";
        case 3: return "bad worldSizeY";
        case 4: return "SEH reading fg layer";
        default: break;
    }

    std::ofstream f(outPath, std::ios::binary);
    if (!f) return "open failed: " + outPath;
    f << "{\n";
    f << "  \"world\": \"" << worldName << "\",\n";
    f << "  \"size\": { \"x\": " << res.wsx << ", \"y\": " << res.wsy << " },\n";
    f << "  \"blocks\": [\n";

    int written = 0;
    int skipped = 0;
    for (int x = 0; x < res.wsx; ++x) {
        for (int y = 0; y < res.wsy; ++y) {
            int bt = Seh_ReadColTileBT(res.cols, x, y);
            if (bt == 0) { ++skipped; continue; }
            if (written > 0) f << ",\n";
            const char* nm = BlockName(bt);
            f << "    { \"x\": " << x << ", \"y\": " << y
              << ", \"bt\": " << bt << ", \"name\": \"";
            for (const char* p = nm; *p; ++p) {
                if (*p == '"' || *p == '\\') f << '\\';
                f << *p;
            }
            f << "\" }";
            ++written;
        }
    }
    f << "\n  ]\n}\n";
    f.close();

    char msg[512];
    std::snprintf(msg, sizeof(msg),
                  "OK: %d tiles (Air %d skipped) -> %s",
                  written, skipped, outPath.c_str());
    return std::string(msg);
}

inline void StreamerMode_ResetAnonMap();

inline void ResetWorldPointers() {
    std::lock_guard<std::mutex> unityLock(g_UnityObjectMutex);

    g_IsInWorld = false;
    g_WorldController = nullptr;
    g_LocalPlayer = nullptr;

    g_cachedCamValid.store(false, std::memory_order_release);
    { std::lock_guard<std::mutex> lock(g_PlayerInstancesMutex); g_PlayerInstances.clear(); g_playerPosCache.clear(); }

    StreamerMode_ResetAnonMap();
    g_isInMineworld = false;
    g_needsScanForGems = true;
    g_joinState = JoinState::Idle;

    { std::lock_guard<std::mutex> lk(g_AIInstancesMutex); g_AIInstances.clear(); g_aiPosCache.clear(); }
    { std::lock_guard<std::mutex> lk(g_gemstoneLocationsMutex); g_gemstoneLocations.clear(); }
    { std::lock_guard<std::mutex> lk(g_lightCrystalLocationsMutex); g_lightCrystalLocations.clear(); }
    { std::lock_guard<std::mutex> lk(g_potLocationsMutex); g_potLocations.clear(); }
    { std::lock_guard<std::mutex> lk(g_treasureLocationsMutex); g_treasureLocations.clear(); }
}

inline void UpdateGemstonesDynamic() {
    if (!g_WorldController) return;
    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return;

    int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldSizeX <= 0 || worldSizeX > 500) return;
    void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldSizeY = 0;
    for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
        if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
    if (worldSizeY <= 0 || worldSizeY > 500) return;

    std::lock_guard<std::mutex> lock(g_gemstoneLocationsMutex);

    g_gemstoneLocations.erase(std::remove_if(g_gemstoneLocations.begin(), g_gemstoneLocations.end(), [&](const GemstoneInfo& gem) {
        if (gem.mapPoint.x < 0 || gem.mapPoint.x >= worldSizeX) return true;
        if (gem.mapPoint.y < 0 || gem.mapPoint.y >= worldSizeY) return true;
        void* pColObject = pCols[gem.mapPoint.x];
        if (!pColObject) return true;
        char* pColData = (char*)((uintptr_t)pColObject + 0x20);
        uintptr_t structAddress = (uintptr_t)(pColData + (gem.mapPoint.y * 0x28));
        World::BlockType blockType = *(World::BlockType*)structAddress;
        return !IsGemstone(blockType);
        }), g_gemstoneLocations.end());

    static auto lastScan = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastScan).count() >= 1000) {
        lastScan = now;

        for (int x = 0; x < worldSizeX; ++x) {
            void* pColObject = pCols[x];
            if (!pColObject) continue;
            char* pColData = (char*)((uintptr_t)pColObject + 0x20);
            for (int y = 0; y < worldSizeY; ++y) {
                uintptr_t structAddress = (uintptr_t)(pColData + (y * 0x28));
                World::BlockType blockType = *(World::BlockType*)structAddress;

                if (IsGemstone(blockType)) {
                    bool already_exists = false;
                    for (const auto& existingGem : g_gemstoneLocations) {
                        if (existingGem.mapPoint.x == x && existingGem.mapPoint.y == y) {
                            already_exists = true;
                            break;
                        }
                    }
                    if (!already_exists) {
                        GemstoneInfo newGem = { {x, y}, blockType };
                        g_gemstoneLocations.push_back(newGem);
                    }
                }
            }
        }
    }
    g_isInMineworld = true;
}

inline bool IsNugget(World::BlockType type);

inline void UpdateNuggetsDynamic() {
    if (!g_WorldController) return;
    static auto lastScan = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastScan).count() < 1000) return;
    lastScan = now;

    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return;
    int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldSizeX <= 0 || worldSizeX > 500) return;
    void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldSizeY = 0;
    for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
        if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
    if (worldSizeY <= 0 || worldSizeY > 500) return;

    std::vector<NuggetInfo> found;
    for (int x = 0; x < worldSizeX; ++x) {
        void* pColObject = pCols[x];
        if (!pColObject) continue;
        char* pColData = (char*)((uintptr_t)pColObject + 0x20);
        for (int y = 0; y < worldSizeY; ++y) {
            uintptr_t addr = (uintptr_t)(pColData + (y * 0x28));
            World::BlockType bt = *(World::BlockType*)addr;
            if (IsNugget(bt)) found.push_back({ { x, y }, bt });
        }
    }
    std::lock_guard<std::mutex> lock(g_nuggetLocationsMutex);
    g_nuggetLocations = std::move(found);
}

inline void UpdateLightCrystalsDynamic() {
    if (!g_WorldController) return;
    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return;

    int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldSizeX <= 0 || worldSizeX > 500) return;
    void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldSizeY = 0;
    for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
        if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
    if (worldSizeY <= 0 || worldSizeY > 500) return;

    std::lock_guard<std::mutex> lock(g_lightCrystalLocationsMutex);

    g_lightCrystalLocations.erase(std::remove_if(g_lightCrystalLocations.begin(), g_lightCrystalLocations.end(), [&](const LightCrystalInfo& crystal) {
        if (crystal.mapPoint.x < 0 || crystal.mapPoint.x >= worldSizeX) return true;
        if (crystal.mapPoint.y < 0 || crystal.mapPoint.y >= worldSizeY) return true;
        void* pColObject = pCols[crystal.mapPoint.x];
        if (!pColObject) return true;
        char* pColData = (char*)((uintptr_t)pColObject + 0x20);
        uintptr_t structAddress = (uintptr_t)(pColData + (crystal.mapPoint.y * 0x28));
        World::BlockType blockType = *(World::BlockType*)structAddress;
        return !IsLightCrystal(blockType);
        }), g_lightCrystalLocations.end());

    static auto lastScan = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastScan).count() >= 1000) {
        lastScan = now;

        for (int x = 0; x < worldSizeX; ++x) {
            void* pColObject = pCols[x];
            if (!pColObject) continue;
            char* pColData = (char*)((uintptr_t)pColObject + 0x20);
            for (int y = 0; y < worldSizeY; ++y) {
                uintptr_t structAddress = (uintptr_t)(pColData + (y * 0x28));
                World::BlockType blockType = *(World::BlockType*)structAddress;

                if (IsLightCrystal(blockType)) {
                    bool already_exists = false;
                    for (const auto& existingCrystal : g_lightCrystalLocations) {
                        if (existingCrystal.mapPoint.x == x && existingCrystal.mapPoint.y == y) {
                            already_exists = true;
                            break;
                        }
                    }
                    if (!already_exists) {
                        LightCrystalInfo newCrystal = { {x, y}, blockType };
                        g_lightCrystalLocations.push_back(newCrystal);
                    }
                }
            }
        }
    }
}

inline void UpdatePotsDynamic() {
    if (!g_WorldController) return;
    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return;

    int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldSizeX <= 0 || worldSizeX > 500) return;
    void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldSizeY = 0;
    for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
        if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
    if (worldSizeY <= 0 || worldSizeY > 500) return;

    std::lock_guard<std::mutex> lock(g_potLocationsMutex);

    g_potLocations.erase(std::remove_if(g_potLocations.begin(), g_potLocations.end(), [&](const PotInfo& pot) {
        if (pot.mapPoint.x < 0 || pot.mapPoint.x >= worldSizeX) return true;
        if (pot.mapPoint.y < 0 || pot.mapPoint.y >= worldSizeY) return true;
        void* pColObject = pCols[pot.mapPoint.x];
        if (!pColObject) return true;
        char* pColData = (char*)((uintptr_t)pColObject + 0x20);
        uintptr_t structAddress = (uintptr_t)(pColData + (pot.mapPoint.y * 0x28));
        World::BlockType blockType = *(World::BlockType*)structAddress;
        return !IsPot(blockType);
        }), g_potLocations.end());

    static auto lastScan = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastScan).count() >= 1000) {
        lastScan = now;

        for (int x = 0; x < worldSizeX; ++x) {
            void* pColObject = pCols[x];
            if (!pColObject) continue;
            char* pColData = (char*)((uintptr_t)pColObject + 0x20);
            for (int y = 0; y < worldSizeY; ++y) {
                uintptr_t structAddress = (uintptr_t)(pColData + (y * 0x28));
                World::BlockType blockType = *(World::BlockType*)structAddress;

                if (IsPot(blockType)) {
                    bool already_exists = false;
                    for (const auto& existingPot : g_potLocations) {
                        if (existingPot.mapPoint.x == x && existingPot.mapPoint.y == y) {
                            already_exists = true;
                            break;
                        }
                    }
                    if (!already_exists) {
                        PotInfo newPot = { {x, y}, blockType };
                        g_potLocations.push_back(newPot);
                    }
                }
            }
        }
    }
}

inline void UpdateTreasureDynamic() {
    if (!g_WorldController) return;
    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return;

    int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldSizeX <= 0 || worldSizeX > 500) return;
    void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldSizeY = 0;
    for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
        if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
    if (worldSizeY <= 0 || worldSizeY > 500) return;

    std::lock_guard<std::mutex> lock(g_treasureLocationsMutex);

    g_treasureLocations.erase(std::remove_if(g_treasureLocations.begin(), g_treasureLocations.end(), [&](const TreasureInfo& treasure) {
        if (treasure.mapPoint.x < 0 || treasure.mapPoint.x >= worldSizeX) return true;
        if (treasure.mapPoint.y < 0 || treasure.mapPoint.y >= worldSizeY) return true;
        void* pColObject = pCols[treasure.mapPoint.x];
        if (!pColObject) return true;
        char* pColData = (char*)((uintptr_t)pColObject + 0x20);
        uintptr_t structAddress = (uintptr_t)(pColData + (treasure.mapPoint.y * 0x28));
        World::BlockType blockType = *(World::BlockType*)structAddress;
        return !IsTreasure(blockType);
        }), g_treasureLocations.end());

    static auto lastScan = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastScan).count() >= 1000) {
        lastScan = now;

        for (int x = 0; x < worldSizeX; ++x) {
            void* pColObject = pCols[x];
            if (!pColObject) continue;
            char* pColData = (char*)((uintptr_t)pColObject + 0x20);
            for (int y = 0; y < worldSizeY; ++y) {
                uintptr_t structAddress = (uintptr_t)(pColData + (y * 0x28));
                World::BlockType blockType = *(World::BlockType*)structAddress;

                if (IsTreasure(blockType)) {
                    bool already_exists = false;
                    for (const auto& existingTreasure : g_treasureLocations) {
                        if (existingTreasure.mapPoint.x == x && existingTreasure.mapPoint.y == y) {
                            already_exists = true;
                            break;
                        }
                    }
                    if (!already_exists) {
                        TreasureInfo newTreasure = { {x, y}, blockType };
                        g_treasureLocations.push_back(newTreasure);
                    }
                }
            }
        }
    }
}

inline void DisableAllEnemies() {
    std::lock_guard<std::mutex> lock(g_AIInstancesMutex);
    for (void* aiInstance : g_AIInstances) {
        if (aiInstance) oDeactivateAIEnemy(aiInstance);
    }
}

extern void* g_LocalPlayer;
extern void* g_WorldController;

inline bool g_AIAimBot = false;
inline float g_AIAimBotTimer = 0.0f;

const float AIMBOT_COOLDOWN = 0.20f;

inline float VectorDistance(Vector3 a, Vector3 b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

typedef void(__fastcall* tSendHitAIEnemyMessage)(Vector2i mapPoint, int aiID, int randomHitTextIndex);
typedef bool(__fastcall* tCanPlayerHitAIEnemy)(int aiID);

inline tSendHitAIEnemyMessage oSendHitAIEnemyMessage = nullptr;
inline tCanPlayerHitAIEnemy oCanPlayerHitAIEnemy = nullptr;

typedef bool(__fastcall* tDeepMaggotCanBeHit)(void* maggotInstance);
inline tDeepMaggotCanBeHit oDeepMaggotCanBeHit = nullptr;

inline bool TryHitNearestAI(float maxRange) {
    if (!g_LocalPlayer || !g_WorldController) return false;
    if (!oGetPosition || !oGetTransform ||
        !oConvertWorldPointToMapPoint || !oSendHitAIEnemyMessage) return false;

    void* localPlayer = g_LocalPlayer;
    void* worldController = g_WorldController;
    void* playerTransform = oGetTransform(localPlayer);
    if (!playerTransform) return false;
    Vector3 playerPos = oGetPosition(playerTransform);

    void* targetEnemy = nullptr;
    float closestDistance = maxRange;

    std::lock_guard<std::mutex> lock(g_AIInstancesMutex);
    if (g_AIInstances.empty()) return false;

    for (void* aiInstance : g_AIInstances) {
        if (!aiInstance || IsBadReadPtr(aiInstance, sizeof(void*))) continue;
        AIBase* aiBase = *(AIBase**)((uintptr_t)aiInstance + g_off_AIBase);
        if (!aiBase || IsBadReadPtr(aiBase, sizeof(AIBase))) continue;
        if (aiBase->health <= 0) continue;
        if (IsProjectileEnemyType((int)aiBase->enemyType)) continue;
        if (oCanPlayerHitAIEnemy && !oCanPlayerHitAIEnemy(aiBase->id)) continue;
        Vector3 enemyPos = *(Vector3*)((uintptr_t)aiInstance + g_off_AITempPosition);
        float distance = VectorDistance(playerPos, enemyPos);
        if (distance < closestDistance) {
            closestDistance = distance;
            targetEnemy = aiInstance;
        }
    }

    if (!targetEnemy) return false;
    AIBase* targetBase = *(AIBase**)((uintptr_t)targetEnemy + g_off_AIBase);
    if (!targetBase || IsBadReadPtr(targetBase, sizeof(AIBase))) return false;
    Vector3 targetPos = *(Vector3*)((uintptr_t)targetEnemy + g_off_AITempPosition);
    Vector2i targetMapPoint = oConvertWorldPointToMapPoint(worldController, targetPos);
    oSendHitAIEnemyMessage(targetMapPoint, targetBase->id, -1);
    return true;
}

inline void AIAimBotLogic(float deltaTime) {
    if (!g_IsInWorld || !g_AIAimBot) return;
    g_AIAimBotTimer += deltaTime;
    if (g_AIAimBotTimer < AIMBOT_COOLDOWN) return;
    g_AIAimBotTimer = 0.0f;
    TryHitNearestAI(1.8f);
}

inline std::vector<Vector2i> g_giftBoxLocations;
inline std::mutex g_giftBoxLocationsMutex;

inline std::vector<Vector2i> g_netherKeyLocations;
inline std::mutex g_netherKeyLocationsMutex;

inline bool IsNetherKeyBlockType(int bt) {
    return bt == 1420
        || bt == 3780;
}

inline void UpdateNetherKeysDynamic() {
    if (!g_WorldController) return;
    static auto lastScan = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastScan).count() < 2) return;
    lastScan = now;

    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;

    std::vector<Vector2i> found;

    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (worldBlockLayer_ptr && !IsBadReadPtr(worldBlockLayer_ptr, 0x20)) {
        int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
        if (worldSizeX > 0 && worldSizeX <= 500) {
            void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
            int worldSizeY = 0;
            for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
                if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
            if (worldSizeY > 0 && worldSizeY <= 500) {
                for (int x = 0; x < worldSizeX; ++x) {
                    void* pColObject = pCols[x];
                    if (!pColObject) continue;
                    char* pColData = (char*)((uintptr_t)pColObject + 0x20);
                    for (int y = 0; y < worldSizeY; ++y) {
                        uintptr_t addr = (uintptr_t)(pColData + (y * 0x28));
                        World::BlockType bt = *(World::BlockType*)addr;
                        if (IsNetherKeyBlockType((int)bt))
                            found.push_back({ x, y });
                    }
                }
            }
        }
    }

    uintptr_t pCollectableList = *(uintptr_t*)((uintptr_t)worldObject + g_off_WorldCollectablesList);
    if (pCollectableList && !IsBadReadPtr((void*)pCollectableList, 0x1C)) {
        int size = *(int*)(pCollectableList + Offsets::List_size);
        uintptr_t pItemsArray = *(uintptr_t*)(pCollectableList + Offsets::List_items);
        if (size > 0 && size <= 10000 && pItemsArray &&
            !IsBadReadPtr((void*)pItemsArray, 0x20 + sizeof(void*) * size)) {
            for (int i = 0; i < size; ++i) {
                CollectableData* c = *(CollectableData**)(pItemsArray + 0x20 + (i * sizeof(void*)));
                if (!c || IsBadReadPtr(c, sizeof(CollectableData))) continue;
                if (IsNetherKeyBlockType((int)c->blockType))
                    found.push_back(c->mapPoint);
            }
        }
    }

    std::lock_guard<std::mutex> lock(g_netherKeyLocationsMutex);
    g_netherKeyLocations = std::move(found);
}

inline bool IsNugget(World::BlockType type) {
    int v = (int)type;
    return (v >= 4154 && v <= 4157) || v == 4162;
}

inline bool IsAnyMineBlock(World::BlockType type) {
    return IsGemstone(type)
        || IsNugget(type)
        || IsLightCrystal(type)
        || IsPot(type);
}

inline int GetMiningResistance(int bt) {
    switch (bt) {

        case 0: return 1;

        case 3989:                            return 2;
        case 3980: case 3981: case 3982:
        case 3983: case 3984:                 return 3;
        case 3994:                            return 4;
        case 3991:                            return 5;
        case 3992: case 3985: case 3986:      return 6;
        case 3993: case 3990: case 3988:      return 10;
        case 3987:                            return 18;
        default: break;
    }

    if (bt >= 4012 && bt <= 4056) return 25;
    if (bt >= 3995 && bt <= 4003) return 25;
    if (IsNugget((World::BlockType)bt))       return 25;

    if (IsLightCrystal((World::BlockType)bt)) return 4;
    if (IsPot((World::BlockType)bt))          return 4;

    if (bt >= 4004 && bt <= 4011)             return 1;

    return 7;
}

struct FloorCollectableInfo {
    Vector2i mapPoint;
    World::BlockType blockType;
    int id;
};
inline std::vector<FloorCollectableInfo> g_floorCollectables;
inline std::mutex                        g_floorCollectablesMutex;

inline void _DoFloorCollectablesScan() {
    if (!g_WorldController) return;
    std::vector<FloorCollectableInfo> found;
    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) {
        std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
        g_floorCollectables.clear();
        return;
    }
    uintptr_t pCollectableList = *(uintptr_t*)((uintptr_t)worldObject + g_off_WorldCollectablesList);
    if (!pCollectableList || IsBadReadPtr((void*)pCollectableList, 0x1C)) {
        std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
        g_floorCollectables.clear();
        return;
    }
    int size = *(int*)(pCollectableList + Offsets::List_size);
    uintptr_t pItemsArray = *(uintptr_t*)(pCollectableList + Offsets::List_items);
    if (size <= 0 || size > 10000 || !pItemsArray ||
        IsBadReadPtr((void*)pItemsArray, 0x20 + sizeof(void*) * size)) {
        std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
        g_floorCollectables.clear();
        return;
    }
    for (int i = 0; i < size; ++i) {
        CollectableData* c = *(CollectableData**)(pItemsArray + 0x20 + (i * sizeof(void*)));
        if (!c || IsBadReadPtr(c, sizeof(CollectableData))) continue;

        if (IsNetherKeyBlockType((int)c->blockType)) continue;
        FloorCollectableInfo fc;
        fc.mapPoint  = c->mapPoint;
        fc.blockType = c->blockType;
        fc.id        = c->id;
        found.push_back(fc);
    }
    std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
    g_floorCollectables = std::move(found);
}

inline void UpdateFloorCollectablesDynamic() {
    static auto lastScan = std::chrono::steady_clock::now() - std::chrono::seconds(60);
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastScan).count() < 250) return;
    lastScan = now;
    _DoFloorCollectablesScan();
}

inline void UpdateFloorCollectablesForce() {
    _DoFloorCollectablesScan();
}

inline bool IsGiftBox(World::BlockType type) {
    switch (type) {
    case World::GiftBox:
    case World::NetherGiftBox:
    case World::LabGiftBox:
        return true;
    default:
        return false;
    }
}

inline void UpdateGiftBoxesDynamic() {
    if (!g_WorldController) return;
    static auto lastScan = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastScan).count() < 3) return;
    lastScan = now;

    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return;

    int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldSizeX <= 0 || worldSizeX > 500) return;
    void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldSizeY = 0;
    for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
        if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
    if (worldSizeY <= 0 || worldSizeY > 500) return;

    std::vector<Vector2i> foundBoxes;
    for (int x = 0; x < worldSizeX; ++x) {
        void* pColObject = pCols[x];
        if (!pColObject) continue;
        char* pColData = (char*)((uintptr_t)pColObject + 0x20);
        for (int y = 0; y < worldSizeY; ++y) {
            uintptr_t structAddress = (uintptr_t)(pColData + (y * 0x28));
            World::BlockType blockType = *(World::BlockType*)structAddress;
            if (oIsBlockGiftBox && oIsBlockGiftBox(blockType)) foundBoxes.push_back({ x, y });
        }
    }
    std::lock_guard<std::mutex> lock(g_giftBoxLocationsMutex);
    g_giftBoxLocations = foundBoxes;
}

inline std::vector<Vector2i> g_exitPortalLocations;
inline std::mutex g_exitPortalLocationsMutex;

inline bool IsExitPortal(World::BlockType type) {
    switch (type) {
    case World::PortalMineExit:
    case World::NetherExit:
    case World::DeepNetherExit:
        return true;
    default:
        return false;
    }
}

inline void UpdateExitPortalsDynamic() {
    if (!g_WorldController) return;
    static auto lastScan = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastScan).count() < 3) return;
    lastScan = now;

    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return;

    int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldSizeX <= 0 || worldSizeX > 500) return;
    void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldSizeY = 0;
    for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
        if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
    if (worldSizeY <= 0 || worldSizeY > 500) return;

    std::vector<Vector2i> foundPortals;
    for (int x = 0; x < worldSizeX; ++x) {
        void* pColObject = pCols[x];
        if (!pColObject) continue;
        char* pColData = (char*)((uintptr_t)pColObject + 0x20);
        for (int y = 0; y < worldSizeY; ++y) {
            uintptr_t structAddress = (uintptr_t)(pColData + (y * 0x28));
            World::BlockType blockType = *(World::BlockType*)structAddress;
            bool isPortal = false;
            if (oIsBlockExitPortal)
                isPortal = oIsBlockExitPortal(blockType);
            else
                isPortal = IsExitPortal(blockType);
            if (isPortal) foundPortals.push_back({ x, y });
        }
    }
    std::lock_guard<std::mutex> lock(g_exitPortalLocationsMutex);
    g_exitPortalLocations = foundPortals;
}

void AutoMineworldLogic();

inline uintptr_t g_off_KC_zoomValue        = 0x110;
inline uintptr_t g_off_KC_maxZoom          = 0x6C;
inline uintptr_t g_off_KC_cameraZoomLevel  = 0x10C;

inline void* g_KukouriCameraCached = nullptr;

inline void* g_mainCameraCached = nullptr;

inline void __fastcall hkUpdate(void* t) {
    if (oUpdate) oUpdate(t);

    if (t) g_KukouriCameraCached = t;

    if (g_isTransitioning.load(std::memory_order_acquire)) return;

    static auto last_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = current_time - last_time;
    last_time = current_time;
    float deltaTime = delta.count();

    if (FreecamEnabled) {
        if (!freecamInitialized && g_LocalPlayer) {
            void* localTransform = oGetTransform(g_LocalPlayer);
            if (localTransform) {
                freecamPosition = oGetPosition(localTransform);
                freecamInitialized = true;
            }
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) freecamPosition.x += freecamSpeed;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000)  freecamPosition.x -= freecamSpeed;
        if (GetAsyncKeyState(VK_UP) & 0x8000)    freecamPosition.y += freecamSpeed;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)  freecamPosition.y -= freecamSpeed;
        if (t) {
            void* transform = oGetTransform(t);
            if (transform) oSetPosition(transform, freecamPosition);
        }
    }
    else {
        freecamInitialized = false;
    }
    if (EnableJumpModeOverride && g_LocalPlayer) *(int*)((uintptr_t)g_LocalPlayer + g_off_PlayerJumpMode) = SelectedJumpMode;

    UpdateGiftBoxesDynamic();
    UpdateGemstonesDynamic();
    UpdateNuggetsDynamic();
    UpdateExitPortalsDynamic();
    UpdateNetherKeysDynamic();
    UpdateLightCrystalsDynamic();
    UpdatePotsDynamic();
    UpdateTreasureDynamic();
    UpdateFloorCollectablesDynamic();
}

inline void WriteJetpackFuelFields();

inline bool g_fishTickDead   = false;
inline bool g_fossilTickDead = false;
inline bool g_pnbTickDead    = false;
inline bool g_pingTickDead   = false;
static bool Seh_FishTick(float dt)   { __try { FishBot::Tick(dt);       return true; } __except (EXCEPTION_EXECUTE_HANDLER) { return false; } }
static bool Seh_FossilTick(float dt) { __try { AutoFossil::Tick(dt);    return true; } __except (EXCEPTION_EXECUTE_HANDLER) { return false; } }
static bool Seh_PnbTick()            { __try { Pnb::ScanInventoryTick(); return true; } __except (EXCEPTION_EXECUTE_HANDLER) { return false; } }
static bool Seh_PingTick()           { __try { PingProbe::Tick();        return true; } __except (EXCEPTION_EXECUTE_HANDLER) { return false; } }

inline void __fastcall hkWorldControllerUpdate(void* instance) {
    if (oWorldControllerUpdate) oWorldControllerUpdate(instance);

    {
        std::lock_guard<std::mutex> unityLock(g_UnityObjectMutex);
        g_WorldController = instance;
        g_IsInWorld = true;
    }

    {
        void* curWorld = g_off_WCWorld
            ? *(void**)((uintptr_t)instance + g_off_WCWorld) : nullptr;
        static void* s_lastWorld = nullptr;
        static int   s_settle    = 0;
        if (curWorld != s_lastWorld) {
            if (s_lastWorld != nullptr) {
                g_isTransitioning.store(true, std::memory_order_release);
                g_pathRendererResetPending = true;
                g_autoNetherResetPending   = true;
                {
                    std::lock_guard<std::mutex> lk(g_UnityObjectMutex);
                    g_LocalPlayer = nullptr;
                }
                {
                    std::lock_guard<std::mutex> lk(g_PlayerInstancesMutex);
                    g_PlayerInstances.clear();
                    g_playerPosCache.clear();
                }
                {
                    std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
                    g_AIInstances.clear();
                    g_aiPosCache.clear();
                }
                std::cout << "[Transition] World swap " << s_lastWorld << " -> " << curWorld
                          << " — guard on, stale instances purged" << std::endl;
            }
            s_lastWorld = curWorld;
            s_settle    = 0;
        } else {

            if (s_settle < 6) {
                ++s_settle;
            } else {
                g_isTransitioning.store(false, std::memory_order_release);
            }
        }
    }

    AntiSpeedKickTick(instance);

    WriteJetpackFuelFields();

    {
        uintptr_t wc = (uintptr_t)instance;
        if (!IsBadReadPtr((void*)(wc + g_off_WCAIEnemies), sizeof(uintptr_t))) {
            uintptr_t aiListPtr = *(uintptr_t*)(wc + g_off_WCAIEnemies);
            if (aiListPtr && !IsBadReadPtr((void*)aiListPtr, 0x20)) {
                int count = *(int*)(aiListPtr + Offsets::List_size);
                uintptr_t items = *(uintptr_t*)(aiListPtr + Offsets::List_items);
                if (count > 0 && count < 500 && items &&
                    !IsBadReadPtr((void*)items, 0x20 + (uintptr_t)count * sizeof(void*))) {
                    std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
                    g_AIInstances.clear();
                    g_aiPosCache.clear();
                    for (int i = 0; i < count; i++) {
                        void* aiInst = *(void**)(items + 0x20 + (uintptr_t)i * sizeof(void*));
                        if (!aiInst) continue;
                        uintptr_t ai = (uintptr_t)aiInst;
                        if (!IsBadReadPtr((void*)(ai + g_off_AITempPosition), sizeof(Vector3))) {
                            g_AIInstances.push_back(aiInst);
                            g_aiPosCache[aiInst] = *(Vector3*)(ai + g_off_AITempPosition);
                        }
                    }
                }
            }
        }
    }

    if (oGetMainCamera && oGetTransform && oGetPosition && oGetOrthographicSize) {
        void* cam = oGetMainCamera();
        if (cam) {

            g_mainCameraCached = cam;
            void* camTr = oGetTransform(cam);
            if (camTr) {
                Vector3 camPos = oGetPosition(camTr);
                float ortho    = oGetOrthographicSize(cam);
                if (ortho > 0.0f) {
                    g_cachedCamX    .store(camPos.x, std::memory_order_relaxed);
                    g_cachedCamY    .store(camPos.y, std::memory_order_relaxed);
                    g_cachedCamOrtho.store(ortho,    std::memory_order_relaxed);
                    g_cachedCamValid.store(true,     std::memory_order_release);
                }
            }
        }
    }

    g_joinState = JoinState::InWorld;
    static auto lastDebugTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastDebugTime).count() >= 2) {
        lastDebugTime = now;
        Vector2i playerMapPoint;
        if (GetPlayerMapPoint(playerMapPoint)) {
            if (g_WorldController) {
                void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
                if (worldObject) {
                    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
                    if (worldBlockLayer_ptr && !IsBadReadPtr(worldBlockLayer_ptr, 0x20)) {

                        int worldHeight = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
                        void** pRows = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
                        int worldWidth = 0;
                        for (int i = 0; i < worldHeight && worldWidth == 0; ++i)
                            if (pRows[i]) worldWidth = *(int*)((uintptr_t)pRows[i] + 0x18);
                        if (playerMapPoint.x >= 0 && playerMapPoint.x < worldWidth &&
                            playerMapPoint.y >= 0 && playerMapPoint.y < worldHeight) {
                            void* pRowObject = pRows[playerMapPoint.y];
                            if (pRowObject) {
                                char* pRowData = (char*)((uintptr_t)pRowObject + 0x20);
                                uintptr_t structAddress = (uintptr_t)(pRowData + (playerMapPoint.x * 0x28));
                                World::BlockType blockUnderPlayer = *(World::BlockType*)structAddress;
                                (void)blockUnderPlayer;
                            }
                        }
                    }
                }
            }
        }
        int giftBoxCount = 0;
        {
            std::lock_guard<std::mutex> lock(g_giftBoxLocationsMutex);
            giftBoxCount = g_giftBoxLocations.size();
        }
    }

    if (g_needsScanForGems) {
        std::string cname = GetCurrentWorldName();
        if (!cname.empty() && strstr(cname.c_str(), "MINEWORLD")) {
            g_needsScanForGems = false;
        }
        else {
            g_isInMineworld = false;
            g_needsScanForGems = false;
        }
    }

    UpdateGiftBoxesDynamic();
    UpdateGemstonesDynamic();
    UpdateNuggetsDynamic();
    UpdateExitPortalsDynamic();
    UpdateNetherKeysDynamic();
    UpdateLightCrystalsDynamic();
    UpdatePotsDynamic();
    UpdateTreasureDynamic();
    UpdateFloorCollectablesDynamic();

    {
        static auto s_lastFish = std::chrono::high_resolution_clock::now();
        auto now2 = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now2 - s_lastFish).count();
        s_lastFish = now2;

        if (!g_fishTickDead && !Seh_FishTick(dt)) {
            g_fishTickDead = true;
            std::cout << "[Crash] FishBot::Tick faulted on transition — disabled this world.\n";
        }
        if (!g_fossilTickDead && !Seh_FossilTick(dt)) {
            g_fossilTickDead = true;
            std::cout << "[Crash] AutoFossil::Tick faulted on transition — disabled this world.\n";
        }
        if (!g_pnbTickDead && !Seh_PnbTick()) {
            g_pnbTickDead = true;
            std::cout << "[Crash] Pnb::ScanInventoryTick faulted on transition — disabled this world.\n";
        }
        if (!g_pingTickDead && !Seh_PingTick()) {
            g_pingTickDead = true;
            std::cout << "[Crash] PingProbe::Tick faulted on transition — disabled this world.\n";
        }
    }
}

inline void __fastcall hkWorldControllerOnDestroy(void* instance) {

    g_isTransitioning.store(true, std::memory_order_release);

    g_pathRendererResetPending = true;
    g_autoNetherResetPending   = true;

    {
        std::lock_guard<std::mutex> lock(g_AIInstancesMutex);
        g_AIInstances.clear();
    }
    ResetWorldPointers();
    if (oWorldControllerOnDestroy) oWorldControllerOnDestroy(instance);
    g_needsScanForGems = true;

    g_fishTickDead = g_fossilTickDead = g_pnbTickDead = g_pingTickDead = false;
}

typedef void(__fastcall* tHandleOtherOwnerIPMessage)(void* thisPtr, void* bson);
inline tHandleOtherOwnerIPMessage oHandleOtherOwnerIPMessage = nullptr;

inline void __fastcall hkHandleOtherOwnerIPMessage(void* thisPtr, void* bson) {
    g_isTransitioning.store(true, std::memory_order_release);
    g_pathRendererResetPending = true;
    g_autoNetherResetPending   = true;
    {
        std::lock_guard<std::mutex> lock(g_AIInstancesMutex);
        g_AIInstances.clear();
    }
    ResetWorldPointers();
    if (oHandleOtherOwnerIPMessage) oHandleOtherOwnerIPMessage(thisPtr, bson);
}

inline void __fastcall hkAIUpdate(void* instance) {
    if (oAIUpdate) oAIUpdate(instance);
    if (!instance) return;

    if (g_isTransitioning.load(std::memory_order_acquire)) return;
    {
        std::lock_guard<std::mutex> lock(g_AIInstancesMutex);
        bool found = false;
        for (void* p : g_AIInstances) { if (p == instance) { found = true; break; } }
        if (!found) g_AIInstances.push_back(instance);
    }

    uintptr_t inst = (uintptr_t)instance;
    if (!IsBadReadPtr((void*)(inst + g_off_AITempPosition), sizeof(Vector3))) {
        Vector3 pos = *(Vector3*)(inst + g_off_AITempPosition);
        std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
        g_aiPosCache[instance] = pos;
    } else if (oGetTransform && oGetPosition) {
        void* tr = oGetTransform(instance);
        if (tr) {
            Vector3 pos = oGetPosition(tr);
            std::lock_guard<std::mutex> lk(g_AIInstancesMutex);
            g_aiPosCache[instance] = pos;
        }
    }
}

inline void __fastcall hkAIOnDestroy(void* instance) {
    {
        std::lock_guard<std::mutex> lock(g_AIInstancesMutex);
        auto it = std::find(g_AIInstances.begin(), g_AIInstances.end(), instance);
        if (it != g_AIInstances.end()) g_AIInstances.erase(it);
    }
    if (oAIOnDestroy) oAIOnDestroy(instance);
}

enum class MineworldBotState { Idle, ReadyToEnterPortal, WaitingForResultsScreen };
inline MineworldBotState g_mineworldBotState = MineworldBotState::Idle;
inline bool g_autoMineworldEnabled = false;
inline std::chrono::steady_clock::time_point g_autoMineworldCooldown;
const int AUTO_MINEWORLD_COOLDOWN_SECONDS = 7;

typedef void(__fastcall* tGoFromPortal)(void* instance, Vector2i mapPoint);
inline tGoFromPortal oGoFromPortal = nullptr;

typedef void(__fastcall* tSetAndSendWorldLoadArgs)(void* playerData, int arg);
inline tSetAndSendWorldLoadArgs oSetAndSendWorldLoadArgs = nullptr;

static inline void Seh_SetAndSendWorldLoadArgs(void* pd, int arg) {
    if (!oSetAndSendWorldLoadArgs || !pd) return;
    __try { oSetAndSendWorldLoadArgs(pd, arg); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

inline void SendWorldLoadArgs(int arg) {
    if (!g_LocalPlayer) return;
    void* pd = nullptr;
    __try {
        pd = *(void**)((uintptr_t)g_LocalPlayer + 0x58);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return; }
    if (!pd) return;
    Seh_SetAndSendWorldLoadArgs(pd, arg);
}

typedef void(__fastcall* tSendRequestGeneratedMineExit)(Vector2i mp);
typedef void(__fastcall* tSendRequestNetherWorldExit)(Vector2i mp);
typedef void(__fastcall* tSendRequestDeepNetherExit)(Vector2i mp);
typedef void(__fastcall* tSendRequestItemFromGiftBox)(Vector2i mp);
typedef void(__fastcall* tSendFishOnAreaMessage)();
typedef void(__fastcall* tSendFishOffAreaMessage)(float distance);
typedef void(__fastcall* tFishingLandButtonPressed)(void* instance);

inline tSendRequestGeneratedMineExit  oSendRequestGeneratedMineExit  = nullptr;
inline tSendRequestNetherWorldExit    oSendRequestNetherWorldExit    = nullptr;
inline tSendRequestDeepNetherExit     oSendRequestDeepNetherExit     = nullptr;
inline tSendRequestItemFromGiftBox    oSendRequestItemFromGiftBox    = nullptr;
inline tSendFishOnAreaMessage         oSendFishOnAreaMessage         = nullptr;
inline tSendFishOffAreaMessage        oSendFishOffAreaMessage        = nullptr;

typedef void(__fastcall* tRequestPlayerData)(void* thisPtr);
inline tRequestPlayerData oRequestPlayerData = nullptr;

typedef void(__fastcall* tRecycleMiningGemstoneSingle)(PlayerData_InventoryKey ik, short amount);
inline tRecycleMiningGemstoneSingle oRecycleMiningGemstone = nullptr;

typedef void(__fastcall* tRecycleAllMiningGemstoneBatch)(void* listIK, void* listInt);
inline tRecycleAllMiningGemstoneBatch oRecycleAllMiningGemstone = nullptr;

inline void SchedulePlayerDataRefresh(int  = 350) {

}

static inline void Seh_RecycleMiningGemstone(PlayerData_InventoryKey ik, short amount) {
    if (!oRecycleMiningGemstone) return;
    __try { oRecycleMiningGemstone(ik, amount); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SendRequestGeneratedMineExit(Vector2i mp) {
    if (!oSendRequestGeneratedMineExit) return;
    __try { oSendRequestGeneratedMineExit(mp); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SendRequestNetherWorldExit(Vector2i mp) {
    if (!oSendRequestNetherWorldExit) return;
    __try { oSendRequestNetherWorldExit(mp); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SendRequestDeepNetherExit(Vector2i mp) {
    if (!oSendRequestDeepNetherExit) return;
    __try { oSendRequestDeepNetherExit(mp); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SendRequestItemFromGiftBox(Vector2i mp) {
    if (!oSendRequestItemFromGiftBox) return;
    __try { oSendRequestItemFromGiftBox(mp); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SendFishOnArea() {
    if (!oSendFishOnAreaMessage) return;
    __try { oSendFishOnAreaMessage(); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}
static inline void Seh_SendFishOffArea(float distance) {
    if (!oSendFishOffAreaMessage) return;
    __try { oSendFishOffAreaMessage(distance); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

static inline bool Seh_GoFromPortal(void* player, Vector2i mp) {
    extern tGoFromPortal oGoFromPortal;
    if (!oGoFromPortal || !player) return false;
    __try { oGoFromPortal(player, mp); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

typedef void(__fastcall* tHandleConvertItemsReply)(void* uiInst, void* bsonObj);
typedef void(__fastcall* tHandleRepairReply)(void* uiInst, void* bsonObj);
typedef void(__fastcall* tHandleRecycleAllReply)(void* uiInst, void* bsonObj);

inline tHandleConvertItemsReply  oOrig_CI_HandleGenericItemConversion       = nullptr;
inline tHandleRepairReply        oOrig_MPR_ConvertMaterialsAndRepairItem    = nullptr;
inline tHandleRecycleAllReply    oOrig_GR_RecycleAllMiningGemstone          = nullptr;

inline std::atomic<bool> g_dbgReplyObserved_ConvertItems{ false };
inline std::atomic<bool> g_dbgReplyObserved_RepairItem{ false };
inline std::atomic<bool> g_dbgReplyObserved_RecycleAll{ false };

inline void __fastcall hk_CI_HandleGenericItemConversion(void* uiInst, void* bsonObj) {
    g_dbgReplyObserved_ConvertItems.store(true, std::memory_order_release);
    if (oOrig_CI_HandleGenericItemConversion)
        oOrig_CI_HandleGenericItemConversion(uiInst, bsonObj);
}
inline void __fastcall hk_MPR_ConvertMaterialsAndRepairItem(void* uiInst, void* bsonObj) {
    g_dbgReplyObserved_RepairItem.store(true, std::memory_order_release);
    if (oOrig_MPR_ConvertMaterialsAndRepairItem)
        oOrig_MPR_ConvertMaterialsAndRepairItem(uiInst, bsonObj);
}
inline void __fastcall hk_GR_RecycleAllMiningGemstone(void* uiInst, void* bsonObj) {
    g_dbgReplyObserved_RecycleAll.store(true, std::memory_order_release);
    if (oOrig_GR_RecycleAllMiningGemstone)
        oOrig_GR_RecycleAllMiningGemstone(uiInst, bsonObj);
}

typedef void(__fastcall* tHandleGetInvData)(void* netInst, void* bsonObj);
inline tHandleGetInvData oOrig_NC_HandleGetCurrentInventoryData = nullptr;
inline std::atomic<bool> g_dbgReplyObserved_GetInvData{ false };

inline void __fastcall hk_NC_HandleGetCurrentInventoryData(void* netInst, void* bsonObj) {
    g_dbgReplyObserved_GetInvData.store(true, std::memory_order_release);
    if (oOrig_NC_HandleGetCurrentInventoryData)
        oOrig_NC_HandleGetCurrentInventoryData(netInst, bsonObj);
}

inline void* g_GeneratedMineResultsUIType = nullptr;
inline void* g_positiveButtonActionField = nullptr;

struct Il2CppDelegate {
    void* klass;
    void* monitor;
    void* invoke_impl;
    char  padding[0x10];
    void* m_target;
};

typedef bool(__fastcall* tIsMiningNugget)(World::BlockType);
typedef int(__fastcall* tGetConversionRate)(World::BlockType);
inline tIsMiningNugget oIsMiningNugget = nullptr;
inline tGetConversionRate oGetConversionRate = nullptr;

typedef void(__fastcall* tSendCollectCollectableMessage)(int id);
inline tSendCollectCollectableMessage oSendCollectCollectableMessage = nullptr;

typedef void(__fastcall* tConvertItems)(PlayerData_InventoryKey ik);
inline tConvertItems oConvertItems = nullptr;

typedef int(__fastcall* tGetXPLevel)(int xpAmount);
inline tGetXPLevel oGetXPLevel = nullptr;

inline int GetLocalPlayerLevel();
inline int GetInventoryCount(int blockType, int itemType);

namespace ExtendedPickup {
    inline bool g_enabled = false;
    inline float g_cadenceSec = 0.5f;
    inline std::chrono::steady_clock::time_point g_lastFire =
        std::chrono::steady_clock::now() - std::chrono::seconds(10);

    inline void Reset() {
        g_lastFire = std::chrono::steady_clock::now();
    }

    inline bool Tick() {
        if (!g_enabled) return false;
        if (!g_LocalPlayer || !oSendCollectCollectableMessage) return false;

        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now - g_lastFire).count() / 1000.0f;
        if (elapsed < g_cadenceSec) return false;
        g_lastFire = now;

        Vector2i tFoot  = *(Vector2i*)((uintptr_t)g_LocalPlayer + g_off_Player_currentPlayerMapPoint);
        Vector2i tAbove = *(Vector2i*)((uintptr_t)g_LocalPlayer + g_off_Player_currentPlayerAboveMapPoint);
        Vector2i tRight = *(Vector2i*)((uintptr_t)g_LocalPlayer + g_off_Player_currentPlayerRightMapPoint);
        Vector2i tLeft  = *(Vector2i*)((uintptr_t)g_LocalPlayer + g_off_Player_currentPlayerLeftMapPoint);

        std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
        for (const auto& fc : g_floorCollectables) {
            bool match = (fc.mapPoint.x == tFoot.x  && fc.mapPoint.y == tFoot.y)  ||
                         (fc.mapPoint.x == tAbove.x && fc.mapPoint.y == tAbove.y) ||
                         (fc.mapPoint.x == tRight.x && fc.mapPoint.y == tRight.y) ||
                         (fc.mapPoint.x == tLeft.x  && fc.mapPoint.y == tLeft.y);
            if (match) oSendCollectCollectableMessage(fc.id);
        }
        return true;
    }
}

inline uintptr_t g_off_PlayerPoisonedCooldown = 0x480;

inline void AntiPoisonTick() {
    if (!g_antiPoison || !g_LocalPlayer) return;
    float* pCd = (float*)((uintptr_t)g_LocalPlayer + g_off_PlayerPoisonedCooldown);
    if (!IsBadWritePtr(pCd, sizeof(float)) && *pCd > 0.0f) *pCd = 0.0f;
}

inline Vector2i g_godmodePreDeathTile = { -1, -1 };
inline bool     g_godmodeWarpPending  = false;
inline DWORD    g_godmodeDeathTimeMs  = 0;
inline bool Godmode_SnapshotCurrentTile(Vector2i& out);
inline void Godmode_WarpBackTick();

inline uint64_t GodmodeRebuildDeadlyColliders(bool verbose);
extern inline bool g_lastGodModeEnabled;
extern inline bool g_lastGmMasterSwap;
extern inline bool g_gmMasterSwap;

inline uintptr_t g_off_Player_isDeadTimeCounter        = 0x60;
inline uintptr_t g_off_Player_allowedToResurrect       = 0x65;
inline uintptr_t g_off_Player_isDead                   = 0x66;
inline uintptr_t g_off_Player_isResetDoneAfterDying    = 0x67;
inline uintptr_t g_off_Player_isWaitingDeath           = 0x68;
inline uintptr_t g_off_Player_waitingRespawn           = 0x69;

inline void GodModeStateTick() {

    if (g_isTransitioning.load(std::memory_order_acquire)) return;

    {

        static void* s_rebuiltWorld = nullptr;
        bool toggleChanged = (GodModeEnabled != g_lastGodModeEnabled) ||
                             (g_gmMasterSwap != g_lastGmMasterSwap);
        if (toggleChanged) {
            g_lastGodModeEnabled = GodModeEnabled;
            g_lastGmMasterSwap   = g_gmMasterSwap;
            s_rebuiltWorld = nullptr;
        }

        static void* s_attemptWorld = nullptr;
        static int   s_attempts     = 0;
        if (g_WorldController != s_attemptWorld) {
            s_attemptWorld = g_WorldController;
            s_attempts = 0;
        }
        if (GodModeEnabled && g_WorldController &&
            g_WorldController != s_rebuiltWorld &&
            s_attempts < 600) {
            ++s_attempts;

            bool verbose = (s_attempts == 1) || (s_attempts % 120 == 0);
            uint64_t n = GodmodeRebuildDeadlyColliders(verbose);
            if (n > 0) {
                s_rebuiltWorld = g_WorldController;
                std::cout << "[GM-Rebuild] world pinned after "
                          << s_attempts << " attempt(s), injected " << n << "\n";
            } else if (s_attempts == 600) {
                s_rebuiltWorld = g_WorldController;
                std::cout << "[GM-Rebuild] gave up after 600 attempts "
                             "(no deadly tiles injected this world)\n";
            }
        }
    }

}

inline void AutoGiftBoxLogic() {
    if (!g_autoGiftBox || !g_LocalPlayer || !g_WorldController ||
        !oSendRequestItemFromGiftBox) return;

    static auto lastTick = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count() < 250) return;
    lastTick = now;

    static std::unordered_map<long long, std::chrono::steady_clock::time_point> recent;
    static constexpr int PER_BOX_COOLDOWN_MS = 2500;

    std::vector<Vector2i> snapshot;
    {
        std::lock_guard<std::mutex> lk(g_giftBoxLocationsMutex);
        snapshot = g_giftBoxLocations;
    }
    if (snapshot.empty()) return;

    for (const Vector2i& mp : snapshot) {
        long long key = ((long long)(uint32_t)mp.x << 32) | (uint32_t)mp.y;
        auto it = recent.find(key);
        if (it != recent.end() &&
            std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count() < PER_BOX_COOLDOWN_MS)
            continue;
        recent[key] = now;
        Seh_SendRequestItemFromGiftBox(mp);
    }

    if (recent.size() > 256) {
        for (auto it = recent.begin(); it != recent.end();) {
            if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count() > 30)
                it = recent.erase(it);
            else
                ++it;
        }
    }
}

inline void AutoCollectLogic() {
    if (!g_autoCollect || !g_LocalPlayer || !g_WorldController || !oGetTransform || !oGetPosition || !oConvertMapPointToWorldPoint_V3 || !oSendCollectCollectableMessage) return;
    static auto lastCollectTime = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastCollectTime).count() < 200) return;
    lastCollectTime = std::chrono::steady_clock::now();

    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    uintptr_t pCollectableList = *(uintptr_t*)((uintptr_t)worldObject + g_off_WorldCollectablesList);
    if (!pCollectableList) return;
    int listSize = *(int*)(pCollectableList + Offsets::List_size);
    uintptr_t pItemsArray = *(uintptr_t*)(pCollectableList + Offsets::List_items);
    if (!pItemsArray) return;
    if (listSize < 0 || listSize > 10000) {
        return;
    }
    if (listSize == 0) return;

    const float collectRadiusSquared = 8.0f * 8.0f;
    const Vector3 playerWorldPos = oGetPosition(oGetTransform(g_LocalPlayer));

    for (int i = 0; i < listSize; i++) {
        void* collectible_ptr = *(void**)(pItemsArray + 0x20 + (i * sizeof(void*)));
        if (IsBadReadPtr(collectible_ptr, sizeof(CollectableData))) continue;
        const Vector2i mapPoint = *(Vector2i*)((uintptr_t)collectible_ptr + 0x30);
        const Vector3 itemWorldPos = oConvertMapPointToWorldPoint_V3(g_WorldController, { mapPoint.x, mapPoint.y });
        float dx = itemWorldPos.x - playerWorldPos.x;
        float dy = itemWorldPos.y - playerWorldPos.y;
        if ((dx * dx) + (dy * dy) < collectRadiusSquared) {
            const int itemID = *(int*)((uintptr_t)collectible_ptr + 0x10);
            oSendCollectCollectableMessage(itemID);
        }
    }
}

typedef void(__cdecl* tJoinDynamicWorld)(Il2CppString* worldName, Il2CppString* entryPoint, bool fromMainMenu, void* methodInfo);
inline tJoinDynamicWorld oJoinDynamicWorld = nullptr;

inline void FindAndUseMineworldPortal() {
    std::cout << "[PortalFinder] Starting SAFE scan for Mineworld portal..." << std::endl;
    if (!oGoFromPortal || !g_LocalPlayer || !g_WorldController) return;

    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return;
    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return;

    int worldSizeX = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldSizeX <= 0 || worldSizeX > 500) return;
    void** pCols = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldSizeY = 0;
    for (int i = 0; i < worldSizeX && worldSizeY == 0; ++i)
        if (pCols[i]) worldSizeY = *(int*)((uintptr_t)pCols[i] + 0x18);
    if (worldSizeY <= 0 || worldSizeY > 500) return;

    for (int x = 0; x < worldSizeX; ++x) {
        if (!pCols[x]) continue;
        char* pColData = (char*)((uintptr_t)pCols[x] + 0x20);
        for (int y = 0; y < worldSizeY; ++y) {
            __try {
                uintptr_t structAddress = (uintptr_t)(pColData + (y * 0x28));
                World::BlockType blockType = *(World::BlockType*)structAddress;
                if (blockType == World::BlockType::MineworldPortal) {
                    std::cout << "[PortalFinder] SUCCESS: Found MineworldPortal at tile (" << x << ", " << y << ")!" << std::endl;
                    oGoFromPortal(g_LocalPlayer, { x, y });
                    return;
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
    }
    std::cout << "[PortalFinder] FAILED: Scanned the entire world safely but did not find the portal." << std::endl;
}

inline std::chrono::steady_clock::time_point g_conversionCooldown;

inline void _SehInjectRawBson(const uint8_t* data, size_t len) {
    if (!oSimpleBSON_Load || !oAddOneMessageToList || !il2cpp_array_new || !byteClass) return;
    __try {
        Il2CppArray* arr = il2cpp_array_new(byteClass, len);
        if (!arr) return;
        memcpy(arr->items, data, len);
        void* pkt = oSimpleBSON_Load(arr);
        if (!pkt) return;
        oAddOneMessageToList(pkt);
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
}

inline void SendAutoMineworldPacket(nlohmann::json& j) {
    std::vector<uint8_t> bson_data;
    try { bson_data = nlohmann::json::to_bson(j); }
    catch (const std::exception&) { return; }
    if (bson_data.empty()) return;
    _SehInjectRawBson(bson_data.data(), bson_data.size());
}

inline std::mutex                  g_rawSendMutex;
inline std::vector<nlohmann::json> g_rawSendQueue;
inline void QueueRawBsonSend(const nlohmann::json& j) {
    std::lock_guard<std::mutex> lk(g_rawSendMutex);
    g_rawSendQueue.push_back(j);
}
inline void DrainRawBsonSendQueue() {
    std::vector<nlohmann::json> local;
    {
        std::lock_guard<std::mutex> lk(g_rawSendMutex);
        if (g_rawSendQueue.empty()) return;
        local.swap(g_rawSendQueue);
    }
    for (auto& j : local) SendAutoMineworldPacket(j);
}

inline int64_t GetDateTimeTicksGlobal() {
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::system_clock::now().time_since_epoch()).count();
    return us * 10 + 621355968000000000LL;
}

inline void TryConvertBronzeKeys() {
    if (!oConvertItems) return;
    PlayerData_InventoryKey ik{ 4154, 7 };
    __try { oConvertItems(ik); }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

inline std::chrono::steady_clock::time_point g_lastInventoryRefresh =
    std::chrono::steady_clock::now() - std::chrono::seconds(60);
inline int g_inventoryRefreshesSent = 0;
inline void SendInventoryRefreshPacket() {

}
inline void RequestInventoryRefresh() {
    auto now = std::chrono::steady_clock::now();
    auto sinceMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                     now - g_lastInventoryRefresh).count();
    if (sinceMs < 600) return;

    g_lastInventoryRefresh = now;
    ++g_inventoryRefreshesSent;
}

inline void AutoMineworldLogic() {
    if (!g_autoMineworldEnabled || g_mineworldBotState == MineworldBotState::WaitingForResultsScreen) return;
    if (std::chrono::steady_clock::now() < g_autoMineworldCooldown) return;

    if (g_WorldController == nullptr) {
        if (g_joinState == JoinState::Idle) {
            if (!oJoinDynamicWorld || !oIl2CppStringNew) return;
            std::cout << "[AutoMineworld] In Main Menu. Joining PIXELMINES..." << std::endl;
            oJoinDynamicWorld(oIl2CppStringNew("PIXELMINES"), oIl2CppStringNew("default"), true, nullptr);
            g_joinState = JoinState::Joining;
            g_autoMineworldCooldown = std::chrono::steady_clock::now() + std::chrono::seconds(15);
        }
        return;
    }

    std::string worldName = GetCurrentWorldName();
    std::transform(worldName.begin(), worldName.end(), worldName.begin(), ::toupper);

    if (worldName.find("MINEWORLD") != std::string::npos) {
        Vector2i playerCoords;
        if (GetPlayerMapPoint(playerCoords)) {

            Seh_SendRequestGeneratedMineExit(playerCoords);
            g_mineworldBotState = MineworldBotState::WaitingForResultsScreen;
        }
    }
    else if (worldName.find("PIXELMINES") != std::string::npos) {
        if (g_mineworldBotState == MineworldBotState::Idle) {

            std::cout << "[AutoMineworld] In PIXELMINES. Sending wlA level-1 hint via PlayerData helper." << std::endl;
            SendWorldLoadArgs(0);
            g_mineworldBotState = MineworldBotState::ReadyToEnterPortal;
            g_autoMineworldCooldown = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        }
        else if (g_mineworldBotState == MineworldBotState::ReadyToEnterPortal) {
            std::cout << "[AutoMineworld] Searching for portal..." << std::endl;
            FindAndUseMineworldPortal();
            g_autoMineworldCooldown = std::chrono::steady_clock::now() + std::chrono::seconds(AUTO_MINEWORLD_COOLDOWN_SECONDS);
        }
    }
    else {
        g_autoMineworldCooldown = std::chrono::steady_clock::now() + std::chrono::seconds(AUTO_MINEWORLD_COOLDOWN_SECONDS);
    }
}

inline bool g_rainbowNameEnabled = false;
inline char g_rainbowNameText[128] = "Zwanez";
inline float g_rainbowNameTicks = 0.0f;

typedef void(__fastcall* t_TMP_SetText)(void* instance, Il2CppString* text, bool syncTextInputBox);
inline t_TMP_SetText o_TMP_SetText = nullptr;

inline void RainbowNameLogic(float deltaTime) {
    if (!g_rainbowNameEnabled || !g_LocalPlayer || !o_TMP_SetText || !oIl2CppStringNew) return;

    void* playerNameTMP = *(void**)((uintptr_t)g_LocalPlayer + g_off_PlayerNameTMP);
    if (!playerNameTMP) return;
    g_rainbowNameTicks += deltaTime * 2.5f;
    std::string visualName = g_rainbowNameText;
    std::stringstream richTextBuilder;

    for (int i = 0; i < visualName.length(); ++i) {
        float r = 0.5f + 0.5f * cosf(g_rainbowNameTicks + (float)i / 4.0f);
        float g = 0.5f + 0.5f * cosf(g_rainbowNameTicks + (float)i / 4.0f + 2.0f);
        float b = 0.5f + 0.5f * cosf(g_rainbowNameTicks + (float)i / 4.0f + 4.0f);
        std::stringstream colorStream;
        colorStream << "#" << std::setfill('0') << std::setw(2) << std::hex << (int)(r * 255) << std::setfill('0') << std::setw(2) << std::hex << (int)(g * 255) << std::setfill('0') << std::setw(2) << std::hex << (int)(b * 255) << "FF";
        richTextBuilder << "<color=" << colorStream.str() << ">" << visualName[i] << "</color>";
    }

    Il2CppString* il2cppText = oIl2CppStringNew(richTextBuilder.str().c_str());
    if (il2cppText) o_TMP_SetText(playerNameTMP, il2cppText, true);
}

inline bool g_streamerMode = false;
inline std::unordered_map<void*, int> g_streamerAnonIdx;

inline std::unordered_set<void*> g_streamerAnonApplied;
inline std::mutex g_streamerAnonMutex;
inline int g_streamerAnonNext = 0;
inline float g_streamerTicks = 0.0f;

inline void StreamerMode_ResetAnonMap() {
    std::lock_guard<std::mutex> lk(g_streamerAnonMutex);
    g_streamerAnonIdx.clear();
    g_streamerAnonApplied.clear();
    g_streamerAnonNext = 0;
}

namespace streamer_seh {

    static bool SafeCopyBytes(void* dst, const void* src, size_t n) {
        __try {
            for (size_t i = 0; i < n; ++i) ((uint8_t*)dst)[i] = ((const uint8_t*)src)[i];
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    }
    static Il2CppString* SafeStringNew(const char* s, bool* faulted) {
        *faulted = false; Il2CppString* r = nullptr;
        __try { r = oIl2CppStringNew(s); }
        __except (EXCEPTION_EXECUTE_HANDLER) { *faulted = true; }
        return r;
    }
    static void SafeTmpSetText(void* tmp, Il2CppString* s, bool sync, bool* faulted) {
        *faulted = false;
        __try { o_TMP_SetText(tmp, s, sync); }
        __except (EXCEPTION_EXECUTE_HANDLER) { *faulted = true; }
    }
    static void* SafeDerefQword(void* base, uintptr_t off, bool* faulted) {
        *faulted = false; void* r = nullptr;
        __try { r = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(base) + off); }
        __except (EXCEPTION_EXECUTE_HANDLER) { *faulted = true; }
        return r;
    }
}

inline void StreamerModeLogic(float deltaTime) {
    if (!g_streamerMode || !o_TMP_SetText || !oIl2CppStringNew) {

        static bool s_diagPreReqDone = false;
        if (g_streamerMode && !s_diagPreReqDone) {
            s_diagPreReqDone = true;
            std::cout << "[Streamer] prereq miss:"
                      << " enabled=" << (int)g_streamerMode
                      << " TMP_SetText=" << (void*)o_TMP_SetText
                      << " string_new=" << (void*)oIl2CppStringNew
                      << " — feature dead unless all three resolve." << std::endl;
        }
        return;
    }

    if (!g_il2cpp_string_new_validated) {
        static bool s_diagValGateDone = false;
        if (!s_diagValGateDone) {
            s_diagValGateDone = true;
            std::cout << "[Streamer] DISABLED — il2cpp_string_new failed boot "
                         "validation (manifest RVA wrong for this game build). "
                         "Refusing to run: would feed garbage strings to "
                         "TMP_SetText and trip a server kick. Fix the manifest "
                         "string_new RVA to re-enable." << std::endl;
        }
        return;
    }

    {
        static bool s_diagInitDone = false;
        if (!s_diagInitDone) {
            s_diagInitDone = true;
            bool f1 = false;
            Il2CppString* probe = streamer_seh::SafeStringNew("ZMOD_PROBE", &f1);
            std::cout << "[Streamer] init probe: SafeStringNew('ZMOD_PROBE')"
                      << " ret=" << (void*)probe
                      << " faulted=" << (int)f1 << std::endl;
            if (!f1 && probe) {

                uint8_t buf[0x20] = {};
                bool copyOk = streamer_seh::SafeCopyBytes(buf, probe, 0x20);
                if (!copyOk) {
                    std::cout << "[Streamer] init probe: byte dump AV — pointer bogus." << std::endl;
                } else {
                    std::cout << "[Streamer] init probe bytes:";
                    char hex[8];
                    for (int i = 0; i < 0x20; ++i) {
                        snprintf(hex, sizeof(hex), " %02X", buf[i]);
                        std::cout << hex;
                    }
                    int32_t lenAt10 = *(int32_t*)(buf + 0x10);
                    int32_t lenAt08 = *(int32_t*)(buf + 0x08);
                    int32_t lenAt18 = *(int32_t*)(buf + 0x18);
                    uint16_t chAt12  = *(uint16_t*)(buf + 0x12);
                    uint16_t chAt14  = *(uint16_t*)(buf + 0x14);
                    uint16_t chAt1A  = *(uint16_t*)(buf + 0x1A);
                    std::cout << " | lenAt+8=" << lenAt08
                              << " lenAt+10=" << lenAt10
                              << " lenAt+18=" << lenAt18
                              << " ch+12=0x" << std::hex << chAt12
                              << " ch+14=0x" << chAt14
                              << " ch+1A=0x" << chAt1A << std::dec
                              << std::endl;
                }

                void* inner = nullptr;
                bool innerCopyOk = streamer_seh::SafeCopyBytes(&inner, (uint8_t*)probe + 0x10, sizeof(inner));
                std::cout << "[Streamer] init probe inner ptr=" << inner
                          << " readOk=" << (int)innerCopyOk << std::endl;
                if (innerCopyOk && inner) {
                    uint8_t ibuf[0x30] = {};
                    bool ic = streamer_seh::SafeCopyBytes(ibuf, inner, 0x30);
                    if (!ic) {
                        std::cout << "[Streamer] init probe inner: byte dump AV." << std::endl;
                    } else {
                        std::cout << "[Streamer] init probe inner bytes:";
                        char hex[8];
                        for (int i = 0; i < 0x30; ++i) {
                            snprintf(hex, sizeof(hex), " %02X", ibuf[i]);
                            std::cout << hex;
                        }
                        int32_t iLenAt08 = *(int32_t*)(ibuf + 0x08);
                        int32_t iLenAt10 = *(int32_t*)(ibuf + 0x10);
                        uint16_t iCh14   = *(uint16_t*)(ibuf + 0x14);
                        uint16_t iCh12   = *(uint16_t*)(ibuf + 0x12);
                        std::cout << " | iLen+8=" << iLenAt08
                                  << " iLen+10=" << iLenAt10
                                  << " iCh+12=0x" << std::hex << iCh12
                                  << " iCh+14=0x" << iCh14 << std::dec
                                  << " (expect len=10 ch+14=0x5A 'Z' if inner is System.String)"
                                  << std::endl;
                    }
                }
            }
            std::cout << "[Streamer] init: TMP_off=0x" << std::hex << g_off_PlayerNameTMP
                      << std::dec << " players_tracked=" << g_PlayerInstances.size()
                      << " localPlayer=" << g_LocalPlayer << std::endl;
        }
    }

    g_streamerTicks += deltaTime * 2.5f;

    const uintptr_t TMP_OFFSET = g_off_PlayerNameTMP;

    static uint32_t s_stringNewFaults = 0;
    static uint32_t s_tmpSetTextFaults = 0;
    static uint32_t s_derefFaults = 0;
    static uint32_t s_tickCount = 0;
    static uint32_t s_localApplied = 0;
    static uint32_t s_anonApplied = 0;
    ++s_tickCount;

    bool faulted = false;
    static Il2CppString* s_localStrCached = nullptr;
    static uint32_t s_localBuildFrame = 0;
    const uint32_t kLocalBuildEveryNTicks = 6;
    if (s_tickCount - s_localBuildFrame >= kLocalBuildEveryNTicks) {
        s_localBuildFrame = s_tickCount;
        std::string zmod = "ZMod";
        std::stringstream rb;
        for (size_t i = 0; i < zmod.size(); ++i) {
            float r = 0.5f + 0.5f * cosf(g_streamerTicks + (float)i / 4.0f);
            float g = 0.5f + 0.5f * cosf(g_streamerTicks + (float)i / 4.0f + 2.0f);
            float b = 0.5f + 0.5f * cosf(g_streamerTicks + (float)i / 4.0f + 4.0f);
            char hex[16];
            sprintf_s(hex, sizeof(hex), "#%02X%02X%02XFF",
                (int)(r * 255), (int)(g * 255), (int)(b * 255));
            rb << "<color=" << hex << ">" << zmod[i] << "</color>";
        }
        s_localStrCached = streamer_seh::SafeStringNew(rb.str().c_str(), &faulted);
        if (faulted) {
            ++s_stringNewFaults;
            if (s_stringNewFaults == 1 || s_stringNewFaults % 300 == 0) {
                std::cout << "[Streamer] string_new AV (count=" << s_stringNewFaults
                          << " of " << s_tickCount << " ticks)" << std::endl;
            }
            s_localStrCached = nullptr;
        }
    }
    Il2CppString* localStr = s_localStrCached;

    std::vector<void*> snap;
    {
        std::lock_guard<std::mutex> lk(g_PlayerInstancesMutex);
        snap = g_PlayerInstances;
    }

    for (void* p : snap) {
        if (!p) continue;

        bool derefFaulted = false;
        void* tmp = streamer_seh::SafeDerefQword(p, TMP_OFFSET, &derefFaulted);
        if (derefFaulted) {
            ++s_derefFaults;
            if (s_derefFaults == 1 || s_derefFaults % 300 == 0) {
                std::cout << "[Streamer] Player+0x" << std::hex << TMP_OFFSET << std::dec
                          << " deref AV (count=" << s_derefFaults << ")" << std::endl;
            }
            continue;
        }
        if (!tmp) continue;

        if (p == g_LocalPlayer) {

            if (localStr && s_tickCount == s_localBuildFrame) {
                streamer_seh::SafeTmpSetText(tmp, localStr, true, &faulted);
                if (faulted) {
                    ++s_tmpSetTextFaults;
                    if (s_tmpSetTextFaults == 1 || s_tmpSetTextFaults % 300 == 0) {
                        std::cout << "[Streamer] local TMP.SetText AV (count="
                                  << s_tmpSetTextFaults << ")" << std::endl;
                    }
                } else {
                    ++s_localApplied;
                }
            }
            continue;
        }

        bool alreadyApplied = false;
        int idx = 0;
        {
            std::lock_guard<std::mutex> lk(g_streamerAnonMutex);
            auto it = g_streamerAnonIdx.find(p);
            if (it == g_streamerAnonIdx.end()) {
                idx = g_streamerAnonNext++;
                g_streamerAnonIdx[p] = idx;
            } else {
                idx = it->second;
            }
            alreadyApplied = (g_streamerAnonApplied.find(p) != g_streamerAnonApplied.end());
        }
        if (alreadyApplied) continue;

        char anonBuf[32];
        sprintf_s(anonBuf, sizeof(anonBuf), "Anon_%d", idx);
        Il2CppString* anonStr = streamer_seh::SafeStringNew(anonBuf, &faulted);
        if (faulted) { ++s_stringNewFaults; continue; }
        if (anonStr) {
            streamer_seh::SafeTmpSetText(tmp, anonStr, true, &faulted);
            if (faulted) { ++s_tmpSetTextFaults; continue; }
            ++s_anonApplied;
            std::lock_guard<std::mutex> lk(g_streamerAnonMutex);
            g_streamerAnonApplied.insert(p);
        }
    }

    if (g_debugDamageLog && s_tickCount % 300 == 0) {
        std::cout << "[Streamer] tick=" << s_tickCount
                  << " players=" << snap.size()
                  << " localApplied=" << s_localApplied
                  << " anonApplied=" << s_anonApplied
                  << " faults(sn/tmp/deref)=" << s_stringNewFaults
                  << "/" << s_tmpSetTextFaults
                  << "/" << s_derefFaults
                  << std::endl;
    }
}

typedef void* (__fastcall* tGO_AddComponent)(void* this_, void* type, void* methodInfo);
typedef void  (__fastcall* tCollider2D_set_isTrigger)(void* this_, bool value, void* methodInfo);
typedef void  (__fastcall* tBoxCollider2D_set_size)(void* this_, Vector2 size, void* methodInfo);

typedef void  (__fastcall* tCircleCollider2D_set_radius)(void* this_, float radius, void* methodInfo);
typedef void  (__fastcall* tCollider2D_set_offset)(void* this_, Vector2 offset, void* methodInfo);

inline void* g_BoxCollider2D_Klass     = nullptr;
inline void* g_BoxCollider2D_TypeObj   = nullptr;

inline void* g_WheeledBuzzSaw_Klass    = nullptr;
inline tGO_AddComponent          oGO_AddComponent          = nullptr;
inline tCollider2D_set_isTrigger oCollider2D_set_isTrigger = nullptr;
inline tBoxCollider2D_set_size   oBoxCollider2D_set_size   = nullptr;
inline tCircleCollider2D_set_radius oCircle_set_radius     = nullptr;
inline tCollider2D_set_offset       oCollider2D_set_offset = nullptr;

inline const void* g_mi_AddComponent     = nullptr;
inline const void* g_mi_set_isTrigger    = nullptr;
inline const void* g_mi_set_size         = nullptr;
inline const void* g_mi_set_radius       = nullptr;
inline const void* g_mi_set_offset       = nullptr;
inline std::atomic<bool> g_colliderApiReady{ false };
inline std::atomic<bool> g_colliderApiTried{ false };

inline void* FindClassDynamic(const char* className, const char* ns);
inline void* GetMethodPtr(const char* className, const char* methodName, int paramCount);

using t_il2cpp_class_get_method_from_name = const void* (*)(void*, const char*, int);
extern inline t_il2cpp_class_get_method_from_name p_il2cpp_class_get_method_from_name;

inline bool ResolveColliderApiOnce() {
    if (g_colliderApiReady.load(std::memory_order_relaxed)) return true;
    bool expected = false;
    if (!g_colliderApiTried.compare_exchange_strong(expected, true)) {

        return g_colliderApiReady.load(std::memory_order_relaxed);
    }

    if (!il2cpp_class_get_type || !il2cpp_type_get_object) {
        std::cout << "[GM-Inject] missing il2cpp_class_get_type/type_get_object — "
                     "manifest API not wired\n";
        return false;
    }

    void* boxKlass = FindClassDynamic("UnityEngine.BoxCollider2D", "");

    void* sawKlass = FindClassDynamic("WheeledBuzzSawMonoBehaviour", "");
    if (!sawKlass) {
        std::cout << "[GM-Inject] note: WheeledBuzzSawMonoBehaviour klass "
                     "absent — AI-enemy solidify path disabled (block path "
                     "unaffected)\n";
    }

    void* boxTypeObj = nullptr;
    if (boxKlass) {
        void* boxType = il2cpp_class_get_type(boxKlass);
        boxTypeObj = boxType ? il2cpp_type_get_object(boxType) : nullptr;
    }
    if (!boxTypeObj) {
        std::cout << "[GM-Inject] note: BoxCollider2D Type obj unavailable — "
                     "AddComponent fallback disabled (set_isTrigger path "
                     "unaffected)\n";
    }

    void* addComp = GetMethodPtr("UnityEngine.GameObject", "AddComponent", -1);
    void* setTrig = GetMethodPtr("UnityEngine.Collider2D", "set_isTrigger", -1);
    void* setSize = GetMethodPtr("UnityEngine.BoxCollider2D", "set_size", -1);

    void* setRad = GetMethodPtr("UnityEngine.CircleCollider2D", "set_radius", -1);
    void* setOff = GetMethodPtr("UnityEngine.Collider2D", "set_offset", -1);

    if (!setTrig) {
        std::cout << "[GM-Inject] FATAL: Collider2D.set_isTrigger method ptr "
                     "missing — cannot solidify any deadly tile\n";
        return false;
    }
    if (!addComp) {
        std::cout << "[GM-Inject] note: GameObject.AddComponent ptr missing — "
                     "fallback path disabled (set_isTrigger primary unaffected)\n";
    }

    if (!p_il2cpp_class_get_method_from_name) {
        p_il2cpp_class_get_method_from_name = Manifest::GetApi<t_il2cpp_class_get_method_from_name>(
            "il2cpp_class_get_method_from_name");
    }
    const void* miAdd   = nullptr;
    const void* miTrig  = nullptr;
    const void* miSize  = nullptr;
    const void* miRad   = nullptr;
    const void* miOff   = nullptr;
    if (p_il2cpp_class_get_method_from_name) {
        void* goKlass  = FindClassDynamic("UnityEngine.GameObject", "");
        void* colKlass = FindClassDynamic("UnityEngine.Collider2D", "");
        void* circleKlass = FindClassDynamic("UnityEngine.CircleCollider2D", "");
        if (goKlass)  miAdd  = p_il2cpp_class_get_method_from_name(goKlass,  "AddComponent",   1);
        if (colKlass) miTrig = p_il2cpp_class_get_method_from_name(colKlass, "set_isTrigger",  1);
        if (colKlass) miOff  = p_il2cpp_class_get_method_from_name(colKlass, "set_offset",     1);
        if (circleKlass) miRad = p_il2cpp_class_get_method_from_name(circleKlass, "set_radius", 1);
        miSize = p_il2cpp_class_get_method_from_name(boxKlass, "set_size", 1);
    } else {
        std::cout << "[GM-Inject] WARN: p_il2cpp_class_get_method_from_name "
                     "could not be resolved — methodInfo will be null, may crash\n";
    }

    g_BoxCollider2D_Klass     = boxKlass;
    g_BoxCollider2D_TypeObj   = boxTypeObj;
    g_WheeledBuzzSaw_Klass    = sawKlass;
    oGO_AddComponent          = (tGO_AddComponent)addComp;
    oCollider2D_set_isTrigger = (tCollider2D_set_isTrigger)setTrig;
    oBoxCollider2D_set_size   = (tBoxCollider2D_set_size)setSize;
    oCircle_set_radius        = (tCircleCollider2D_set_radius)setRad;
    oCollider2D_set_offset    = (tCollider2D_set_offset)setOff;
    g_mi_AddComponent         = miAdd;
    g_mi_set_isTrigger        = miTrig;
    g_mi_set_size             = miSize;
    g_mi_set_radius           = miRad;
    g_mi_set_offset           = miOff;
    g_colliderApiReady.store(true);
    std::cout << "[GM-Inject] resolver OK — set_isTrigger=" << setTrig
              << " set_radius=" << setRad
              << " set_offset=" << setOff
              << " miTrig=" << miTrig
              << " miRad=" << miRad
              << " miOff=" << miOff << "\n";
    return true;
}

inline bool IsDeadlyAIInstance(void* instance) {
    if (!il2cpp_object_get_class || !instance) return false;
    void* klass = nullptr;
    __try { klass = il2cpp_object_get_class(instance); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    if (!klass) return false;
    if (klass == g_WheeledBuzzSaw_Klass) return true;
    return false;
}

inline std::mutex g_injectedGOsMu;
inline std::unordered_set<void*> g_injectedGOs;

inline int _Seh_AddSolidCollider(void* go, void** outNewCollider) {
    int result = -1;
    *outNewCollider = nullptr;
    __try {

        void* newCollider = oGO_AddComponent(go, g_BoxCollider2D_TypeObj,
                                             (void*)g_mi_AddComponent);
        if (!newCollider) {
            result = 0;
        } else {
            *outNewCollider = newCollider;
            oCollider2D_set_isTrigger(newCollider, false,
                                      (void*)g_mi_set_isTrigger);
            if (oBoxCollider2D_set_size) {
                Vector2 size{ 0.64f, 0.64f };
                oBoxCollider2D_set_size(newCollider, size,
                                        (void*)g_mi_set_size);
            }
            result = 1;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) { result = -1; }
    return result;
}

inline std::atomic<int>  g_gmInjectFailStreak{ 0 };

inline std::atomic<bool> g_gmInjectDisabled{ true };
static constexpr int kGmInjectFailLimit = 8;

inline bool TryAddSolidColliderToGO(void* go, const char* sourceTag) {
    if (!go) return false;
    if (g_gmInjectDisabled.load(std::memory_order_relaxed)) return false;
    if (!ResolveColliderApiOnce()) return false;

    if (!oGO_AddComponent || !g_BoxCollider2D_TypeObj) return false;
    {
        std::lock_guard<std::mutex> lk(g_injectedGOsMu);
        if (g_injectedGOs.count(go)) return false;
        g_injectedGOs.insert(go);
    }
    void* newCollider = nullptr;
    int rc = _Seh_AddSolidCollider(go, &newCollider);
    if (rc == 1) {
        g_gmInjectFailStreak.store(0, std::memory_order_relaxed);
        std::cout << "[GM-Inject] solid collider added go=" << go
                  << " newCollider=" << newCollider
                  << " src=" << sourceTag << "\n";
        return true;
    }

    int streak = g_gmInjectFailStreak.fetch_add(1, std::memory_order_relaxed) + 1;
    if (streak <= kGmInjectFailLimit) {
        std::cout << "[GM-Inject] " << (rc == 0 ? "AddComponent null" : "SEH")
                  << " go=" << go << " src=" << sourceTag
                  << " (fail " << streak << "/" << kGmInjectFailLimit << ")\n";
    }
    if (streak == kGmInjectFailLimit) {
        g_gmInjectDisabled.store(true, std::memory_order_relaxed);
        std::cout << "[GM-Inject] DISABLED for session — " << kGmInjectFailLimit
                  << " consecutive failures. AddComponent method ptr or "
                     "calling convention wrong for this manifest build. "
                     "Suppressing further attempts (no more spam).\n";
    }
    return false;
}

inline void TryMakeDeadlyAIEnemySolid(void* instance) {
    if (!instance) return;
    if (!ResolveColliderApiOnce()) return;
    if (!IsDeadlyAIInstance(instance)) return;

    void* go = *(void**)((uintptr_t)instance + 0x28);
    if (!go || IsBadReadPtr(go, sizeof(void*))) return;
    TryAddSolidColliderToGO(go, "AI-Start");
}

inline std::atomic<int>  g_gmTriggerFailStreak{ 0 };
inline std::atomic<bool> g_gmTriggerDisabled{ false };
inline std::unordered_set<void*> g_solidifiedColliders;

inline std::mutex g_forceSolidMu;
inline std::unordered_set<void*> g_forceSolidColliders;

static const int kRoundSawIDs[] = {
    453,
    959,
    3044,
    3234,
    3235,
    4419,
};
inline bool Godmode_IsRoundSaw(int bt) {
    for (int id : kRoundSawIDs) if (id == bt) return true;
    return false;
}

inline int _Seh_SetColliderSolid(void* collider, bool enlargeCircle) {
    int result = -1;
    __try {
        oCollider2D_set_isTrigger(collider, false, (void*)g_mi_set_isTrigger);
        if (enlargeCircle) {
            if (oCollider2D_set_offset) {
                Vector2 z{ 0.0f, 0.0f };
                oCollider2D_set_offset(collider, z, (void*)g_mi_set_offset);
            }
            if (oCircle_set_radius) {
                oCircle_set_radius(collider, 0.34f, (void*)g_mi_set_radius);
            }
        }
        result = 1;
    } __except (EXCEPTION_EXECUTE_HANDLER) { result = -1; }
    return result;
}

inline bool TryMakeColliderSolid(void* collider, const char* sourceTag, bool enlargeCircle) {
    if (!collider) return false;
    if (g_gmTriggerDisabled.load(std::memory_order_relaxed)) return false;
    if (!ResolveColliderApiOnce()) return false;
    if (!oCollider2D_set_isTrigger) return false;
    {
        std::lock_guard<std::mutex> lk(g_injectedGOsMu);
        if (g_solidifiedColliders.count(collider)) return false;
        g_solidifiedColliders.insert(collider);
    }
    int rc = _Seh_SetColliderSolid(collider, enlargeCircle);
    if (rc == 1) {
        g_gmTriggerFailStreak.store(0, std::memory_order_relaxed);

        {
            std::lock_guard<std::mutex> lk(g_forceSolidMu);
            g_forceSolidColliders.insert(collider);
        }
        std::cout << "[GM-Solid] set_isTrigger(false) ok collider=" << collider
                  << " src=" << sourceTag << "\n";
        return true;
    }
    int streak = g_gmTriggerFailStreak.fetch_add(1, std::memory_order_relaxed) + 1;
    if (streak <= 8) {
        std::cout << "[GM-Solid] SEH collider=" << collider
                  << " src=" << sourceTag << " (fail " << streak << "/8)\n";
    }
    if (streak == 8) {
        g_gmTriggerDisabled.store(true, std::memory_order_relaxed);
        std::cout << "[GM-Solid] DISABLED — 8 consecutive set_isTrigger faults. "
                     "Collider2D.set_isTrigger ptr wrong for this build.\n";
    }
    return false;
}

inline tCollider2D_set_isTrigger oColl_set_isTrigger_orig = nullptr;
inline std::atomic<uint64_t> g_reArmBlocked{ 0 };

inline void __fastcall hkCollider2D_set_isTrigger(void* this_, bool value, void* mi) {

    if (value && this_ && GodModeEnabled) {
        bool tracked = false;
        {
            std::lock_guard<std::mutex> lk(g_forceSolidMu);
            tracked = g_forceSolidColliders.count(this_) > 0;
        }
        if (tracked) {
            uint64_t n = g_reArmBlocked.fetch_add(1, std::memory_order_relaxed) + 1;
            if (n <= 20 || (n % 200 == 0)) {
                std::cout << "[GM-ReArm] blocked set_isTrigger(true) on solid "
                          << "collider=" << this_ << " (#" << n << ")\n";
            }
            value = false;
        }
    }
    if (oColl_set_isTrigger_orig) oColl_set_isTrigger_orig(this_, value, mi);
}

inline void __fastcall hkAIEnemyMonoBehaviourBase_Start(void* instance) {
    if (DisableGassy && instance) {
        AIBase* aiBase = *(AIBase**)((uintptr_t)instance + 0xD8);
        if (aiBase && aiBase->enemyType == Gassy) {
            if (oDeactivateAIEnemy) oDeactivateAIEnemy(instance);
            return;
        }
    }
    if (oAIEnemyMonoBehaviourBase_Start) oAIEnemyMonoBehaviourBase_Start(instance);

    TryMakeDeadlyAIEnemySolid(instance);
}

#define ZMOD_BOUNCE_GATE(t) (AntiBounce && !(AntiBounceIgnoreMushrooms && IsMushroomBlockType(t)))
inline bool __fastcall hkIsBlockPinball(int t) { return ZMOD_BOUNCE_GATE(t) ? false : oIsBlockPinball(t); }
inline bool __fastcall hkIsBlockHot(int t) { return ZMOD_BOUNCE_GATE(t) ? false : oIsBlockHot(t); }
inline bool __fastcall hkIsBlockSpring(int t) { return ZMOD_BOUNCE_GATE(t) ? false : oIsBlockSpring(t); }
inline bool __fastcall hkIsBlockTrampolin(int t) { return ZMOD_BOUNCE_GATE(t) ? false : oIsBlockTrampolin(t); }
inline bool __fastcall hkIsBlockElastic(int t) { return ZMOD_BOUNCE_GATE(t) ? false : oIsBlockElastic(t); }
inline bool __fastcall hkIsBlockWind(int t) { return antifan ? false : oIsBlockWind(t); }

inline bool __fastcall hkShouldBelowBlockDoBounce(void* t) { return (AntiBounce && !AntiBounceIgnoreMushrooms) ? false : oShouldBelowBlockDoBounce(t); }

inline bool __fastcall hkHitPlayerFromBlock(void* t, void* edx, int type, Vector2i pt, bool trap) {

    int realBT  = (int)(intptr_t)edx;
    int realMpX = type;
    bool realTrap = (pt.x != 0);
    g_lastDamageBT = realBT;
    g_lastDamageMp = Vector2i{realMpX, 0};
    if (g_debugDamageLog) {
        std::cout << "[DMG] HitPlayerFromBlock bt=" << realBT
                  << " (" << BlockName(realBT) << ")"
                  << " mp.x=" << realMpX
                  << " trap=" << (realTrap ? "1" : "0")
                  << " godmode=" << (GodModeEnabled ? "1" : "0") << "\n";
        LogBlockContext("HitPlayerFromBlock", realMpX);
    }
    return oHitPlayerFromBlock ? oHitPlayerFromBlock(t, edx, type, pt, trap) : false;
}
inline bool __fastcall hkHitPlayerFromBlockHF(void* t, void* edx, int hitForce, int blockType, Vector2i mp) {

    int realHF  = (int)(intptr_t)edx;
    int realBT  = hitForce;
    int realMpX = blockType;
    g_lastDamageBT = realBT;
    g_lastDamageHF = realHF;
    g_lastDamageMp = Vector2i{realMpX, 0};
    if (g_debugDamageLog) {
        std::cout << "[DMG] HitPlayerFromBlock(HF) hf=" << realHF
                  << " bt=" << realBT
                  << " (" << BlockName(realBT) << ")"
                  << " mp.x=" << realMpX
                  << " godmode=" << (GodModeEnabled ? "1" : "0") << "\n";
        LogBlockContext("HitPlayerFromBlock(HF)", realMpX);
    }
    return oHitPlayerFromBlockHF ? oHitPlayerFromBlockHF(t, edx, hitForce, blockType, mp) : false;
}
inline bool __fastcall hkHitPlayerFromAIEnemy(void* t, void* edx, int force, int type) {

    g_lastDamageBT = -type;
    g_lastDamageHF = force;
    if (g_debugDamageLog) {
        std::cout << "[DMG] HitPlayerFromAIEnemy force=" << force
                  << " aiType=" << type
                  << " godmode=" << (GodModeEnabled ? "1" : "0") << "\n";
    }
    return oHitPlayerFromAIEnemy ? oHitPlayerFromAIEnemy(t, edx, force, type) : false;
}
inline bool __fastcall hkHitPlayerFromExplosion(void* t, void* edx, int hf, Vector2i mp, Vector3 ep, void* did) {
    g_lastDamageHF = hf;
    g_lastDamageMp = mp;
    if (g_debugDamageLog) {
        std::cout << "[DMG] HitPlayerFromExplosion hf=" << hf
                  << " mp=(" << mp.x << "," << mp.y
                  << ") explosionPos=(" << ep.x << "," << ep.y << "," << ep.z << ")"
                  << " godmode=" << (GodModeEnabled ? "1" : "0") << "\n";
    }
    return oHitPlayerFromExplosion ? oHitPlayerFromExplosion(t, edx, hf, mp, ep, did) : false;
}

inline bool __fastcall hkPlayerData_HitPlayer(void* t, void* edx, int hitForce, bool isInBattle, bool useRegen) {
    if (g_debugDamageLog) {
        std::cout << "[DMG] PlayerData::HitPlayer hf=" << hitForce
                  << " inBattle=" << (isInBattle ? "1" : "0")
                  << " useRegen=" << (useRegen ? "1" : "0")
                  << " lastDmgBT=" << g_lastDamageBT
                  << " (" << (g_lastDamageBT >= 0 ? BlockName(g_lastDamageBT) : "ai") << ")"
                  << " godmode=" << (GodModeEnabled ? "1" : "0")
                  << " -> " << (GodModeEnabled ? "BLOCKED" : "allowed") << "\n";
    }
    return GodModeEnabled ? false : oPlayerData_HitPlayer(t, edx, hitForce, isInBattle, useRegen);
}

inline void __fastcall hkPlayerCausePoisonedBT(void* t, void* edx, int bt)     {
    if (g_debugDamageLog) {
        std::cout << "[DMG] CausePoisoned(bt) bt=" << bt
                  << " (" << BlockName(bt) << ")"
                  << " godmode=" << (GodModeEnabled ? "1" : "0")
                  << " antiPoison=" << (g_antiPoison ? "1" : "0") << "\n";
    }
    if (g_antiPoison || GodModeEnabled) return;
    if (oPlayerCausePoisonedBT) oPlayerCausePoisonedBT(t, edx, bt);
}
inline void __fastcall hkPlayerCausePoisonedAI(void* t, void* edx, int ai)     {
    if (g_debugDamageLog) {
        std::cout << "[DMG] CausePoisoned(ai) ai=" << ai
                  << " godmode=" << (GodModeEnabled ? "1" : "0")
                  << " antiPoison=" << (g_antiPoison ? "1" : "0") << "\n";
    }
    if (g_antiPoison || GodModeEnabled) return;
    if (oPlayerCausePoisonedAI) oPlayerCausePoisonedAI(t, edx, ai);
}
inline void __fastcall hkPlayerCausePoisonedF (void* t, void* edx, float cd)   {
    if (g_debugDamageLog) {
        std::cout << "[DMG] CausePoisoned(cd) cd=" << cd
                  << " godmode=" << (GodModeEnabled ? "1" : "0")
                  << " antiPoison=" << (g_antiPoison ? "1" : "0") << "\n";
    }
    if (g_antiPoison || GodModeEnabled) return;
    if (oPlayerCausePoisonedF)  oPlayerCausePoisonedF (t, edx, cd);
}

inline void __fastcall hkWCSpeedHackDetected(void* wc, void* methodInfo) {
    if (g_antiSpeedKick) return;
    if (oWCSpeedHackDetected) oWCSpeedHackDetected(wc, methodInfo);
}

inline void __fastcall hkACTkSpdStartAuto(void* t, void* mi)  { if (g_actkBypass) return; if (oACTkSpdStartAuto)  oACTkSpdStartAuto(t, mi); }
inline void __fastcall hkACTkTimeStartAuto(void* t, void* mi) { if (g_actkBypass) return; if (oACTkTimeStartAuto) oACTkTimeStartAuto(t, mi); }
inline void __fastcall hkACTkInjStartAuto(void* t, void* mi)  { if (g_actkBypass) return; if (oACTkInjStartAuto)  oACTkInjStartAuto(t, mi); }
inline void __fastcall hkACTkObsStartAuto(void* t, void* mi)  { if (g_actkBypass) return; if (oACTkObsStartAuto)  oACTkObsStartAuto(t, mi); }
inline void __fastcall hkACTkWallStartAuto(void* t, void* mi) { if (g_actkBypass) return; if (oACTkWallStartAuto) oACTkWallStartAuto(t, mi); }

inline bool __fastcall hkACTkSpdIsTicksCheated(void* t, long long c, long long* p, long long r, void* mi) {
    if (g_actkBypass) return false;
    return oACTkSpdIsTicksCheated ? oACTkSpdIsTicksCheated(t, c, p, r, mi) : false;
}

inline void __fastcall hkACTkObsOnCheat1(void* t, void* o, void* d, void* f, void* mi) {
    if (g_actkBypass) return;
    if (oACTkObsOnCheat1) oACTkObsOnCheat1(t, o, d, f, mi);
}
inline void __fastcall hkACTkObsOnCheat2(void* t, void* o, bool h, void* d, void* f, void* mi) {
    if (g_actkBypass) return;
    if (oACTkObsOnCheat2) oACTkObsOnCheat2(t, o, h, d, f, mi);
}

inline float __fastcall hkPlayerGetRunSpeed(void* player, void* methodInfo) {
    float orig = oPlayerGetRunSpeed ? oPlayerGetRunSpeed(player, methodInfo) : 0.0f;
    if (g_playerRunSpeedHack && g_playerRunSpeedMul > 0.0f) {
        return orig * g_playerRunSpeedMul;
    }
    return orig;
}

inline bool                  g_hitBlockObserveLog = false;
inline std::atomic<uint32_t> g_hitBlockObserveCount{ 0 };
inline std::atomic<int>      g_hitBlockObserveTopArm{ 0 };
inline std::atomic<bool>     g_hitBlockObserveExtraBool{ false };
inline std::atomic<bool>     g_hitBlockObserveIsLocal{ false };

inline std::atomic<void*>    g_hitBlockCapturedNameStr{ nullptr };
inline std::atomic<void*>    g_hitBlockCapturedPlayerData{ nullptr };
inline std::atomic<void*>    g_hitBlockCapturedMethodInfo{ nullptr };
inline std::atomic<int>      g_hitBlockCapturedTopArm{ 0 };

inline bool __fastcall hkPlayerHitBlock(
    void* player,
    Vector2i mapPoint,
    int topArmBlockType,
    void* playerNameStr,
    bool isLocalPlayer,
    void* playerData,
    bool extraBool,
    void* methodInfo)
{
    uint32_t n = g_hitBlockObserveCount.fetch_add(1, std::memory_order_relaxed) + 1;
    g_hitBlockObserveTopArm.store(topArmBlockType, std::memory_order_relaxed);
    g_hitBlockObserveExtraBool.store(extraBool, std::memory_order_relaxed);
    g_hitBlockObserveIsLocal.store(isLocalPlayer, std::memory_order_relaxed);

    if (isLocalPlayer && playerNameStr && playerData && methodInfo) {
        g_hitBlockCapturedNameStr.store(playerNameStr, std::memory_order_release);
        g_hitBlockCapturedPlayerData.store(playerData, std::memory_order_release);
        g_hitBlockCapturedMethodInfo.store(methodInfo, std::memory_order_release);
        g_hitBlockCapturedTopArm.store(topArmBlockType, std::memory_order_release);
    }
    if (g_hitBlockObserveLog && (n <= 5 || n % 60 == 0)) {

        int strLen = -1;
        uint16_t firstChar = 0;
        if (playerNameStr) {
            __try {
                strLen    = *(int32_t*)((uint8_t*)playerNameStr + 0x10);
                firstChar = *(uint16_t*)((uint8_t*)playerNameStr + 0x14);
            } __except (EXCEPTION_EXECUTE_HANDLER) { strLen = -2; }
        }
        std::cout << "[HitBlockObs] #" << n
                  << " player=" << player
                  << " mp=(" << mapPoint.x << "," << mapPoint.y << ")"
                  << " topArm=" << topArmBlockType
                  << " nameStr=" << playerNameStr
                  << " (len=" << strLen << " ch0=0x" << std::hex << firstChar << std::dec << ")"
                  << " isLocal=" << (int)isLocalPlayer
                  << " pData=" << playerData
                  << " extraBool=" << (int)extraBool
                  << " mi=" << methodInfo
                  << std::endl;
    }
    if (oPlayerHitBlock) {
        return oPlayerHitBlock(player, mapPoint, topArmBlockType, playerNameStr,
                               isLocalPlayer, playerData, extraBool, methodInfo);
    }
    return false;
}

inline std::atomic<uint32_t> g_mineToolObserveCount{ 0 };
inline std::atomic<int>      g_mineToolObserveBlockType{ 0 };
inline std::atomic<int>      g_mineToolObserveBoolMask{ 0 };
inline bool __fastcall hkPlayerMineBlockWithTool(
    void* player,
    float floatArg,
    bool boolArg1,
    Vector2i vec2iArg,
    int blockTypeArg,
    bool boolArg2,
    bool boolArg3,
    void* methodInfo)
{
    uint32_t n = g_mineToolObserveCount.fetch_add(1, std::memory_order_relaxed) + 1;
    g_mineToolObserveBlockType.store(blockTypeArg, std::memory_order_relaxed);
    int bm = (boolArg1 ? 1 : 0) | (boolArg2 ? 2 : 0) | (boolArg3 ? 4 : 0);
    g_mineToolObserveBoolMask.store(bm, std::memory_order_relaxed);
    if (g_hitBlockObserveLog && (n <= 5 || n % 60 == 0)) {
        std::cout << "[MineToolObs] #" << n
                  << " player=" << player
                  << " float=" << floatArg
                  << " b1=" << (int)boolArg1
                  << " vec2i=(" << vec2iArg.x << "," << vec2iArg.y << ")"
                  << " blockType=" << blockTypeArg
                  << " b2=" << (int)boolArg2
                  << " b3=" << (int)boolArg3
                  << " mi=" << methodInfo
                  << std::endl;
    }
    if (oPlayerMineBlockWithTool) {
        return oPlayerMineBlockWithTool(player, floatArg, boolArg1, vec2iArg,
                                        blockTypeArg, boolArg2, boolArg3, methodInfo);
    }
    return false;
}

inline bool Player_WarpToTile(int tileX, int tileY) {
    if (!oPlayerWarpPlayer || !g_LocalPlayer) return false;
    __try { oPlayerWarpPlayer(g_LocalPlayer, tileX, tileY, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}

inline bool OutMsg_SendCheckpoint(int tileX, int tileY) {
    if (!oOutSendCheckpoint) return false;
    Vector2i mp = { tileX, tileY };
    __try { oOutSendCheckpoint(mp, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}

inline bool OutMsg_SendResurrect(int tileX, int tileY) {
    if (!oOutSendResurrect) return false;
    Vector2i mp = { tileX, tileY };
    long long ticks = 0;
    __try { oOutSendResurrect(ticks, mp, nullptr); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}

namespace Teleport {

    inline DWORD g_lastWarpMs = 0;

    inline std::atomic<uint64_t> g_silentWarpCount{0};

    static inline bool _SilentWarp_Dispatch(const uint8_t* bytes, size_t n) {
        Il2CppArray* il2cppByteArray = il2cpp_array_new(byteClass, n);
        if (!il2cppByteArray) return false;
        std::memcpy(il2cppByteArray->items, bytes, n);
        __try {
            void* bsonBatch = oSimpleBSON_Load(il2cppByteArray);
            if (!bsonBatch) return false;
            oHandleMessages(g_NetworkClient, bsonBatch);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    inline bool SilentWarp(int tileX, int tileY) {

        if (!oHandleMessages || !oSimpleBSON_Load ||
            !il2cpp_array_new   || !byteClass ||
            !g_NetworkClient) {
            std::cout << "[Teleport] SilentWarp aborted: dependency unresolved "
                      << "(handle=" << (void*)oHandleMessages
                      << " bsonLoad=" << (void*)oSimpleBSON_Load
                      << " arrNew=" << (void*)il2cpp_array_new
                      << " byteCls=" << byteClass
                      << " netCli=" << g_NetworkClient << ")\n";
            return false;
        }

        nlohmann::json packet = {
            { "ID", "WP" },
            { "PX", tileX },
            { "PY", tileY }
        };
        nlohmann::json batch = {
            { "mc", 1 },
            { "m0", packet }
        };

        std::vector<uint8_t> bsonBytes;
        try {
            bsonBytes = nlohmann::json::to_bson(batch);
        }
        catch (const std::exception& e) {
            std::cout << "[Teleport] SilentWarp BSON-encode threw: " << e.what() << "\n";
            return false;
        }

        if (!_SilentWarp_Dispatch(bsonBytes.data(), bsonBytes.size())) {
            std::cout << "[Teleport] SilentWarp: dispatch failed (see [Teleport] errors above if any)\n";
            return false;
        }

        g_lastWarpMs = GetTickCount();
        g_silentWarpCount.fetch_add(1, std::memory_order_relaxed);
        std::cout << "[Teleport] SilentWarp -> (" << tileX << "," << tileY
                  << ") OK  count=" << g_silentWarpCount.load() << "\n";
        return true;
    }

    inline std::atomic<uint64_t> g_warpHookHits{0};
    inline std::atomic<uint64_t> g_portalInHookHits{0};
    inline std::atomic<uint64_t> g_resurrectHookHits{0};

    inline void _DumpBsonPayload(const char* tag, void* bsonMsg, uint64_t n) {
        if (!bsonMsg || !oBsonDump) return;
        try {
            Il2CppArray* arr = oBsonDump(bsonMsg);
            if (arr && arr->max_length > 0) {
                std::vector<uint8_t> bytes((uint8_t*)arr->items,
                                           (uint8_t*)arr->items + arr->max_length);
                nlohmann::json j = nlohmann::json::from_bson(bytes);
                std::cout << "[WarpHook] " << tag << " #" << n
                          << " payload=" << j.dump() << "\n";
            }
        } catch (const std::exception& e) {
            std::cout << "[WarpHook] " << tag << " dump threw: " << e.what() << "\n";
        } catch (...) {
            std::cout << "[WarpHook] " << tag << " dump threw (unknown)\n";
        }
    }

    inline void __fastcall hkHandleWarpPlayerMessage(void* thisPtr, void* bsonMsg, void* methodInfo) {
        uint64_t n = g_warpHookHits.fetch_add(1, std::memory_order_relaxed) + 1;
        _DumpBsonPayload("HandleWarpPlayerMessage", bsonMsg, n);
        if (oHandleWarpPlayerMessage_orig)
            oHandleWarpPlayerMessage_orig(thisPtr, bsonMsg, methodInfo);
    }
    inline void __fastcall hkHandlePlayerActivateInPortal(void* thisPtr, void* bsonMsg, void* methodInfo) {
        uint64_t n = g_portalInHookHits.fetch_add(1, std::memory_order_relaxed) + 1;
        _DumpBsonPayload("HandlePlayerActivateInPortal", bsonMsg, n);
        if (oHandlePlayerActivateInPortal_orig)
            oHandlePlayerActivateInPortal_orig(thisPtr, bsonMsg, methodInfo);
    }
    inline void __fastcall hkHandleResurrectPlayer(void* thisPtr, void* bsonMsg, void* methodInfo) {
        uint64_t n = g_resurrectHookHits.fetch_add(1, std::memory_order_relaxed) + 1;
        _DumpBsonPayload("HandleResurrectPlayer", bsonMsg, n);
        if (oHandleResurrectPlayer_orig)
            oHandleResurrectPlayer_orig(thisPtr, bsonMsg, methodInfo);
    }

    inline std::string ProbePayload(int tileX, int tileY) {
        nlohmann::json packet = {
            { "ID", "WP" },
            { "PX", tileX },
            { "PY", tileY }
        };
        nlohmann::json batch = {
            { "mc", 1 },
            { "m0", packet }
        };
        std::string out = batch.dump();
        try {
            auto bson = nlohmann::json::to_bson(batch);
            out += "  (BSON bytes: " + std::to_string(bson.size()) + ")";
        } catch (...) { out += "  (BSON encode failed)"; }
        return out;
    }

}

inline uintptr_t g_off_WC_isSpeedHackDetected = 0x708;
inline void AntiSpeedKickTick(void* wc) {
    if (!g_antiSpeedKick || !wc) return;
    bool* p = (bool*)((uintptr_t)wc + g_off_WC_isSpeedHackDetected);
    if (!IsBadWritePtr(p, 1) && *p) *p = false;
}

inline void __fastcall hkPlayerKillPlayer(void* t, void* edx, int blockType) {

    int realBT = (int)(intptr_t)edx;
    std::cout << "[DEATH] Player::KillPlayer bt=" << realBT
              << " (" << BlockName(realBT) << ")"
              << " lastDmg=bt=" << g_lastDamageBT
              << " (" << (g_lastDamageBT >= 0 ? BlockName(g_lastDamageBT) : "?")
              << ") lastDmg.hf=" << g_lastDamageHF
              << " lastDmg.mp.x=" << g_lastDamageMp.x
              << " godmode=" << (GodModeEnabled ? "1" : "0")
              << " -> ALLOWED (server authority — let respawn run)\n";
    LogBlockContext("KillPlayer", g_lastDamageMp.x);
    if (oPlayerKillPlayer) oPlayerKillPlayer(t, edx, blockType);
}
inline void __fastcall hkPlayerDeathHelper(void* t) {

    if (g_debugDamageLog) {
        std::cout << "[DEATH] Player::DeathHelper "
                  << "godmode=" << (GodModeEnabled ? "1" : "0")
                  << " -> passthrough\n";
    }
    if (oPlayerDeathHelper) oPlayerDeathHelper(t);
}

inline bool __fastcall hkPlayerCheckDeathByColliderHelper(void* t, void* edx, Vector2i mp) {
    bool r = oPlayerCheckDeathByColliderHelper ? oPlayerCheckDeathByColliderHelper(t, edx, mp) : false;

    if (r) {
        std::cout << "[DEATH] CheckDeathByColliderHelper mp=("
                  << mp.x << "," << mp.y << ") -> true (KILL TRIGGERED)\n";
    } else if (g_debugDamageLog) {
        std::cout << "[DEATH] CheckDeathByColliderHelper mp=("
                  << mp.x << "," << mp.y << ") -> false (safe)\n";
    }
    return r;
}

inline void __fastcall hkPlayerDoDyingAnimation(void* t, void* edx, int blockType) {

    std::cout << "[DEATH] DoDyingAnimation bt=" << blockType
              << " (" << BlockName(blockType) << ")"
              << " lastDmg=bt=" << g_lastDamageBT
              << " (" << (g_lastDamageBT >= 0 ? BlockName(g_lastDamageBT) : "ai") << ")"
              << " godmode=" << (GodModeEnabled ? "1" : "0")
              << " -> ALLOWED (server authority — death anim)\n";
    if (oPlayerDoDyingAnimation) oPlayerDoDyingAnimation(t, edx, blockType);
}
inline void __fastcall hkPlayerWaitRespawn(void* t) {
    std::cout << "[DEATH] WaitRespawn godmode=" << (GodModeEnabled ? "1" : "0")
              << " -> ALLOWED (server authority — respawn flow)\n";
    if (oPlayerWaitRespawn) oPlayerWaitRespawn(t);
}

inline void __fastcall hkPlayerDoTakeHitStunnedAnim(void* t, void* edx, int blockType) {
    if (GodModeEnabled) return;
    if (oPlayerDoTakeHitStunnedAnim) oPlayerDoTakeHitStunnedAnim(t, edx, blockType);
}

namespace Door {

    typedef bool(__fastcall* tIsDoorPredicate)(int blockType);
    typedef bool(__fastcall* tWorldRightAccess)(void* world, Vector2i mp, void* pd, void* mi);
    typedef bool(__fastcall* tWcRightGo)       (void* wc,    Vector2i mp, void* pd, void* mi);
    typedef bool(__fastcall* tWcRightCollider) (void* wc,    Vector2i mp,            void* mi);
    typedef bool(__fastcall* tWantsThroughDoors)(void* pd,                            void* mi);

    inline tIsDoorPredicate oIsAnyDoor                       = nullptr;
    inline tIsDoorPredicate oIsNormalDoor                    = nullptr;
    inline tIsDoorPredicate oIsClanDoor                      = nullptr;
    inline tIsDoorPredicate oIsScifiDoor                     = nullptr;
    inline tIsDoorPredicate oIsVIPDoor                       = nullptr;
    inline tIsDoorPredicate oIsLevelVIPDoor                  = nullptr;
    inline tIsDoorPredicate oIsClanFactionDark               = nullptr;
    inline tIsDoorPredicate oIsClanFactionLight              = nullptr;
    inline tIsDoorPredicate oIsEditableLevelVIPDoorOrHatch   = nullptr;

    inline tWorldRightAccess  oWorldRightAccess = nullptr;
    inline tWcRightGo         oWcRightGo        = nullptr;
    inline tWcRightCollider   oWcRightCollider  = nullptr;
    inline tWantsThroughDoors oAdminWants       = nullptr;
    inline tWantsThroughDoors oModWants         = nullptr;

    inline bool g_walkThroughAll = false;

    inline bool IsDoor(int bt) {
        if (bt <= 0) return false;
        auto tryCall = [bt](tIsDoorPredicate fp) -> bool {
            if (!fp) return false;
            __try { return fp(bt); }
            __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        };
        if (tryCall(oIsAnyDoor))                       return true;
        if (tryCall(oIsNormalDoor))                    return true;
        if (tryCall(oIsClanDoor))                      return true;
        if (tryCall(oIsScifiDoor))                     return true;
        if (tryCall(oIsVIPDoor))                       return true;
        if (tryCall(oIsLevelVIPDoor))                  return true;
        if (tryCall(oIsClanFactionDark))               return true;
        if (tryCall(oIsClanFactionLight))              return true;
        if (tryCall(oIsEditableLevelVIPDoorOrHatch))   return true;
        return false;
    }

    inline bool PlayerHasAccess(int tileX, int tileY) {
        if (g_walkThroughAll) return true;
        if (!g_WorldController || !g_off_WCWorld || !g_LocalPlayer) return false;

        void* world = nullptr;
        void* pd    = nullptr;
        __try {
            world = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
            pd    = *(void**)((uintptr_t)g_LocalPlayer + 0x58);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }

        Vector2i mp = { tileX, tileY };
        if (oWorldRightAccess && world && pd) {
            __try { if (oWorldRightAccess(world, mp, pd, nullptr)) return true; }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
        if (oWcRightGo && pd) {
            __try { if (oWcRightGo(g_WorldController, mp, pd, nullptr)) return true; }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
        if (oWcRightCollider) {
            __try { if (oWcRightCollider(g_WorldController, mp, nullptr)) return true; }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
        return false;
    }

    inline bool __fastcall hkWorldRightAccess(void* world, Vector2i mp, void* pd, void* mi) {
        if (g_walkThroughAll) return true;
        return oWorldRightAccess ? oWorldRightAccess(world, mp, pd, mi) : false;
    }
    inline bool __fastcall hkWcRightGo(void* wc, Vector2i mp, void* pd, void* mi) {
        if (g_walkThroughAll) return true;
        return oWcRightGo ? oWcRightGo(wc, mp, pd, mi) : false;
    }
    inline bool __fastcall hkWcRightCollider(void* wc, Vector2i mp, void* mi) {
        if (g_walkThroughAll) return true;
        return oWcRightCollider ? oWcRightCollider(wc, mp, mi) : false;
    }
    inline bool __fastcall hkAdminWants(void* pd, void* mi) {
        if (g_walkThroughAll) return true;
        return oAdminWants ? oAdminWants(pd, mi) : false;
    }
    inline bool __fastcall hkModWants(void* pd, void* mi) {
        if (g_walkThroughAll) return true;
        return oModWants ? oModWants(pd, mi) : false;
    }

}

typedef bool(__fastcall* tDoesBlockHaveCollider)(int);
typedef bool(__fastcall* tDoesBlockCauseDeathByCollider)(int);
typedef bool(__fastcall* tIsBlockInstakill)(int);
inline tDoesBlockHaveCollider           oDoesBlockHaveCollider           = nullptr;
inline tDoesBlockCauseDeathByCollider   oDoesBlockCauseDeathByCollider   = nullptr;
inline tIsBlockInstakill                oIsBlockInstakill                = nullptr;

typedef void(__fastcall* tBlockColliderAndLayerHelper)(void* this_, int blockType, void* go, Vector2i mp, void* methodInfo);
inline tBlockColliderAndLayerHelper     oBlockColliderAndLayerHelper     = nullptr;

typedef bool(__fastcall* tIsBlockInstaDamageWater)(int);
typedef bool(__fastcall* tIsBlockShooterTrap)    (int);
typedef bool(__fastcall* tIsBlockTrap)           (int);
typedef bool(__fastcall* tIsBlockCheckPoint)     (int);
inline tIsBlockInstaDamageWater oIsBlockInstaDamageWater = nullptr;
inline tIsBlockShooterTrap      oIsBlockShooterTrap      = nullptr;
inline tIsBlockTrap             oIsBlockTrap             = nullptr;
inline tIsBlockCheckPoint       oIsBlockCheckPoint       = nullptr;

static const int kKnownCheckpointIDs[] = {
    969,
};

inline bool g_antiAcidWater = true;

inline bool __fastcall hkIsBlockInstaDamageWater(int bt) {
    if (GodModeEnabled || g_antiAcidWater) return false;
    return oIsBlockInstaDamageWater ? oIsBlockInstaDamageWater(bt) : false;
}
inline bool __fastcall hkIsBlockShooterTrap(int bt) {
    if (GodModeEnabled) return false;
    return oIsBlockShooterTrap ? oIsBlockShooterTrap(bt) : false;
}

inline bool __fastcall hkIsBlockTrap(int bt) {
    if (GodModeEnabled) return false;
    return oIsBlockTrap ? oIsBlockTrap(bt) : false;
}

typedef void(__fastcall* tPlayerCheckCheckPoints)(void* player, void* methodInfo);
inline tPlayerCheckCheckPoints oPlayerCheckCheckPoints = nullptr;
inline std::atomic<uint64_t> g_checkpointHitCount{0};
inline int g_lastCheckpointBT = -1;
inline void __fastcall hkPlayerCheckCheckPoints(void* player, void* methodInfo) {

    if (oIsBlockCheckPoint && g_LocalPlayer && g_WorldController) {
        int tx = 0, ty = 0;
        if (SehGetPlayerTile(tx, ty)) {
            int bt = SehReadFgBlock(tx, ty);
            if (bt > 0 && oIsBlockCheckPoint(bt)) {
                g_lastCheckpointBT = bt;
                g_checkpointHitCount.fetch_add(1, std::memory_order_relaxed);
                if (g_debugDamageLog) {
                    std::cout << "[Checkpoint] hit bt=" << bt
                              << " (" << BlockName(bt) << ") tile=("
                              << tx << "," << ty << ") count="
                              << g_checkpointHitCount.load() << "\n";
                }
            }
        }
    }
    if (oPlayerCheckCheckPoints) oPlayerCheckCheckPoints(player, methodInfo);
}

inline bool Godmode_SnapshotCurrentTile(Vector2i& out) {
    if (!g_LocalPlayer || !oGetTransform || !oGetPosition ||
        !oConvertWorldPointToMapPoint || !g_WorldController)
        return false;
    void* tr = nullptr;
    Vector3 wp = {0,0,0};
    Vector2i mp = {0,0};
    __try {
        tr = oGetTransform(g_LocalPlayer);
        if (!tr) return false;
        wp = oGetPosition(tr);
        mp = oConvertWorldPointToMapPoint(g_WorldController, wp);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    out = mp;
    return true;
}

inline void Godmode_WarpBackTick() {
    if (!g_godmodeWarpPending || !GodModeEnabled || !g_LocalPlayer) return;
    DWORD now = GetTickCount();
    if (now - g_godmodeDeathTimeMs < 750) return;
    if (oPlayerWarpPlayer && g_godmodePreDeathTile.x >= 0 && g_godmodePreDeathTile.y >= 0) {
        __try {
            oPlayerWarpPlayer(g_LocalPlayer,
                              g_godmodePreDeathTile.x,
                              g_godmodePreDeathTile.y,
                              nullptr);
            std::cout << "[GodMode] Warp-back to pre-death tile ("
                      << g_godmodePreDeathTile.x << ","
                      << g_godmodePreDeathTile.y << ")\n";
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    g_godmodeWarpPending = false;
}

inline bool __fastcall hkDoesBlockCauseDeathByCollider(int blockType) {
    if (GodModeEnabled) return false;
    return oDoesBlockCauseDeathByCollider ? oDoesBlockCauseDeathByCollider(blockType) : false;
}

inline bool __fastcall hkIsBlockInstakill(int blockType) {
    if (GodModeEnabled) return false;
    return oIsBlockInstakill ? oIsBlockInstakill(blockType) : false;
}

static const int kForcedSolidIDs[] = {

    453,
    959,
    1340,
    1409,
    2345,
    2683,
    3142,
    4464,

    1339,
    1408,
    3141,

    3044,
    3234,
    3235,
    4419,

    7,
    301,
    344,

    4457,
    4458,

};

static const int kForcedPassableIDs[] = {

    -1,
};

static const int kGodmodeStandTiles[] = {
    109,
    111,
    301,
    957,
    2999,
    3042,

};

inline bool __fastcall hkDoesBlockHaveCollider(int blockType) {
    if (GodModeEnabled) {

        for (int id : kForcedPassableIDs) if (id == blockType) return false;

        for (int id : kForcedSolidIDs) if (id == blockType) return true;

        for (int id : kGodmodeStandTiles) if (id == blockType) return true;

        bool wasDeathCol = oDoesBlockCauseDeathByCollider && oDoesBlockCauseDeathByCollider(blockType);
        bool wasInsta    = oIsBlockInstakill              && oIsBlockInstakill(blockType);
        if (wasDeathCol || wasInsta) return true;
    }
    return oDoesBlockHaveCollider ? oDoesBlockHaveCollider(blockType) : false;
}

inline std::atomic<uint64_t> g_blockSwapCount{0};

inline bool g_gmMasterSwap = true;

inline uintptr_t g_off_WC_blockGameObjects = 0x390;
inline uintptr_t g_off_WC_blockCollider2Ds = 0x398;

inline bool Godmode_IsDeadlyTile(int blockType);

inline int _Seh_FlipDeadlyTileColliderSolid(void* wc, int mpx, int mpy, bool enlargeCircle) {
    if (!oCollider2D_set_isTrigger || mpx < 0 || mpy < 0) return 0;
    int rc = -1;
    __try {
        Il2CppArray* colArr = *(Il2CppArray**)((uintptr_t)wc + g_off_WC_blockCollider2Ds);
        if (!colArr || IsBadReadPtr(colArr, sizeof(Il2CppArray))) return 0;
        void* boundsPtr = colArr->bounds;
        if (!boundsPtr || IsBadReadPtr(boundsPtr, 32)) return 0;

        uintptr_t wsy = *(uintptr_t*)((uintptr_t)boundsPtr + 16);
        if (wsy == 0 || wsy > 100000) return 0;
        void** items = (void**)((uintptr_t)colArr + offsetof(Il2CppArray, items));
        void* col = items[(size_t)mpx * wsy + (size_t)mpy];
        if (!col) return 0;
        oCollider2D_set_isTrigger(col, false, (void*)g_mi_set_isTrigger);

        if (enlargeCircle) {
            if (oCollider2D_set_offset) {
                Vector2 z{ 0.0f, 0.0f };
                oCollider2D_set_offset(col, z, (void*)g_mi_set_offset);
            }
            if (oCircle_set_radius) {
                oCircle_set_radius(col, 0.34f, (void*)g_mi_set_radius);
            }
        }
        rc = 1;
    } __except (EXCEPTION_EXECUTE_HANDLER) { rc = -1; }
    return rc;
}

inline void __fastcall hkBlockColliderAndLayerHelper(void* this_, int blockType, void* go, Vector2i mp, void* methodInfo) {

    if (oBlockColliderAndLayerHelper) {
        oBlockColliderAndLayerHelper(this_, blockType, go, mp, methodInfo);
    }

    if (GodModeEnabled && Godmode_IsDeadlyTile(blockType)) {
        if (!g_colliderApiReady.load(std::memory_order_relaxed))
            ResolveColliderApiOnce();
        if (oCollider2D_set_isTrigger) {
            int rc = _Seh_FlipDeadlyTileColliderSolid(this_, mp.x, mp.y,
                                                      Godmode_IsRoundSaw(blockType));
            if (g_debugDamageLog && rc != 1) {
                std::cout << "[GM-Swap] post-flip rc=" << rc
                          << " bt=" << blockType
                          << " mp=(" << mp.x << "," << mp.y << ")\n";
            }
        }
    }
}

inline std::atomic<uint64_t> g_godmodeRebuildCount{0};

inline bool Godmode_IsDeadlyTile(int blockType) {
    if (oIsBlockInstakill        && oIsBlockInstakill(blockType))        return true;
    if (oIsBlockInstaDamageWater && oIsBlockInstaDamageWater(blockType)) return true;
    if (oIsBlockTrap             && oIsBlockTrap(blockType))             return true;
    for (int id : kForcedSolidIDs)    if (id == blockType) return true;
    for (int id : kGodmodeStandTiles) if (id == blockType) return true;
    return false;
}

inline uint64_t GodmodeRebuildDeadlyColliders(bool verbose) {

    if (!g_WorldController || !g_off_WCWorld || !oBlockColliderAndLayerHelper) {
        if (verbose)
            std::cout << "[GM-Rebuild] BAIL precond: WC=" << g_WorldController
                      << " offWorld=" << (void*)g_off_WCWorld
                      << " helper=" << (void*)oBlockColliderAndLayerHelper << "\n";
        return 0;
    }
    uint64_t count = 0;

    int bailCode = 0;
    int dWsx = -1, dWsy = -1;
    int deadlyFound = 0, nullGO = 0, attempted = 0;
    __try {
        do {
            void* worldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
            if (!worldObj || IsBadReadPtr(worldObj, sizeof(void*))) { bailCode = 1; break; }
            void* fg = *(void**)((uintptr_t)worldObj + g_off_WorldBlockLayer);
            if (!fg || IsBadReadPtr(fg, 0x28)) { bailCode = 2; break; }
            int wsx = *(int*)((uintptr_t)fg + 0x18);
            void** cols = (void**)((uintptr_t)fg + 0x20);
            dWsx = wsx;
            if (wsx <= 0 || wsx > 4096) { bailCode = 3; break; }

            Il2CppArray* goArr = *(Il2CppArray**)
                ((uintptr_t)g_WorldController + g_off_WC_blockGameObjects);
            if (!goArr || IsBadReadPtr(goArr, sizeof(Il2CppArray))) { bailCode = 4; break; }

            void** goItems = (void**)((uintptr_t)goArr + offsetof(Il2CppArray, items));

            Il2CppArray* colArr = *(Il2CppArray**)
                ((uintptr_t)g_WorldController + g_off_WC_blockCollider2Ds);
            void** colItems = (colArr && !IsBadReadPtr(colArr, sizeof(Il2CppArray)))
                ? (void**)((uintptr_t)colArr + offsetof(Il2CppArray, items))
                : nullptr;

            if (!cols || !cols[0] || IsBadReadPtr(cols[0], 0x28)) { bailCode = 5; break; }
            int wsy = *(int*)((uintptr_t)cols[0] + 0x18);
            dWsy = wsy;
            if (wsy <= 0 || wsy > 4096) { bailCode = 6; break; }

            for (int x = 0; x < wsx; ++x) {
                void* col = cols[x];
                if (!col || IsBadReadPtr(col, 0x28)) continue;
                char* colData = (char*)((uintptr_t)col + 0x20);
                for (int y = 0; y < wsy; ++y) {
                    int bt = *(int*)((uintptr_t)colData + y * 0x28);
                    if (bt <= 0) continue;
                    if (!Godmode_IsDeadlyTile(bt)) continue;
                    ++deadlyFound;

                    void* go = goItems[(size_t)x * (size_t)wsy + (size_t)y];
                    if (!go) { ++nullGO; continue; }
                    Vector2i mp{ x, y };
                    hkBlockColliderAndLayerHelper(g_WorldController, bt, go, mp, nullptr);
                    ++count;
                    ++attempted;

                    if (colItems) {
                        void* col = colItems[(size_t)x * (size_t)wsy + (size_t)y];
                        if (col) TryMakeColliderSolid(col, "TileColl", Godmode_IsRoundSaw(bt));
                    }

                    TryAddSolidColliderToGO(go, "TileScan");
                }
            }
        } while (0);
    } __except (EXCEPTION_EXECUTE_HANDLER) { bailCode = 9; }
    g_godmodeRebuildCount.fetch_add(count, std::memory_order_relaxed);

    if (verbose || count > 0) {
        std::cout << "[GM-Rebuild] bail=" << bailCode
                  << " wsx=" << dWsx << " wsy=" << dWsy
                  << " deadly=" << deadlyFound
                  << " nullGO=" << nullGO
                  << " attempted=" << attempted << "\n";
    }
    return count;
}

inline bool g_lastGodModeEnabled = false;
inline bool g_lastGmMasterSwap   = true;

inline std::string DumpBlockPredicatesToJson() {
    char appData[MAX_PATH] = {0};
    if (!GetEnvironmentVariableA("APPDATA", appData, MAX_PATH) || !appData[0]) {
        return "no APPDATA";
    }
    std::string baseDir = std::string(appData) + "\\ZMod";
    CreateDirectoryA(baseDir.c_str(), nullptr);
    std::string dumpDir = baseDir + "\\dumps";
    CreateDirectoryA(dumpDir.c_str(), nullptr);
    std::string outPath = dumpDir + "\\block_predicates.json";

    std::ofstream f(outPath, std::ios::binary);
    if (!f) return "open failed: " + outPath;

    const int kProbeMax = 5500;

    auto callIfSet = [](auto* fp, int bt) -> int {
        if (!fp) return -1;
        bool r = false;
        __try { r = fp(bt); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
        return r ? 1 : 0;
    };

    auto emitList = [&](const char* label, auto* fp) {
        f << "  \"" << label << "\": [";
        if (!fp) { f << "],\n"; return; }
        bool first = true;
        for (int bt = 0; bt < kProbeMax; ++bt) {
            int r = callIfSet(fp, bt);
            if (r == 1) {
                if (!first) f << ", ";
                f << bt;
                first = false;
            }
        }
        f << "],\n";
    };

    f << "{\n";
    f << "  \"_comment\": \"Auto-generated runtime probe of every IsBlock* / DoesBlock* predicate. Use to fill WeightTable.md section 17 Open Items.\",\n";
    f << "  \"probeMax\": " << kProbeMax << ",\n";

    emitList("isBlockInstakill",              oIsBlockInstakill);
    emitList("isBlockInstaDamageWater",       oIsBlockInstaDamageWater);
    emitList("isBlockShooterTrap",            oIsBlockShooterTrap);
    emitList("isBlockTrap",                   oIsBlockTrap);
    emitList("doesBlockCauseDeathByCollider", oDoesBlockCauseDeathByCollider);
    emitList("doesBlockHaveCollider",         oDoesBlockHaveCollider);

    emitList("isBlockPinball",                oIsBlockPinball);
    emitList("isBlockHot",                    oIsBlockHot);
    emitList("isBlockSpring",                 oIsBlockSpring);
    emitList("isBlockTrampolin",              oIsBlockTrampolin);
    emitList("isBlockElastic",                oIsBlockElastic);
    emitList("isBlockWind",                   oIsBlockWind);
    emitList("isBlockDeflector",              oIsBlockDeflector);

    f << "  \"_done\": true\n";
    f << "}\n";
    f.close();

    char msg[400];
    std::snprintf(msg, sizeof(msg), "OK -> %s", outPath.c_str());
    return std::string(msg);
}

inline void __fastcall hkKC_SetZoomValueTo(void* t, float value, void* methodInfo) {
    if (t) g_KukouriCameraCached = t;
    if (g_extraZoom && g_extraZoomMul > 1.0f) value *= g_extraZoomMul;
    if (oKC_SetZoomValueTo) oKC_SetZoomValueTo(t, value, methodInfo);
}

namespace DropStackInjector {

using FnClassGetMethod = const void* (*)(void*, const char*, int);
inline FnClassGetMethod g_classGetMethod = nullptr;

typedef void* (*tInstantiate)(void* , void* );
typedef void* (*tGetTransform)(void* );
typedef void* (*tGetParent)(void* );
typedef void* (*tEvtSysCurrent)();
typedef void* (*tEvtSysGetSelected)(void* );
typedef void  (*tGOSetName)(void* , Il2CppString* );
typedef Il2CppString* (*tGOGetName)(void* );
typedef void* (*tGetCompInChildren)(void* , void* );
typedef void* (*tClassGetType)(void* );
typedef void* (*tTypeGetObject)(void* );

inline tInstantiate         pInstantiate         = nullptr;
inline tGetTransform        pGetTransform        = nullptr;
inline tGetParent           pGetParent           = nullptr;
inline tEvtSysCurrent       pEvtSysCurrent       = nullptr;
inline tEvtSysGetSelected   pEvtSysGetSelected   = nullptr;
inline tGOSetName           pGOSetName           = nullptr;
inline tGOGetName           pGOGetName           = nullptr;
inline tGetCompInChildren   pGetCompInChildren   = nullptr;
inline tClassGetType        pClassGetType        = nullptr;
inline tTypeGetObject       pTypeGetObject       = nullptr;
inline void*                g_TMPTypeObject      = nullptr;

inline void* g_clonedBtnGO    = nullptr;
inline bool  g_resolveTried   = false;
inline bool  g_resolveOK      = false;

static inline void* FindClassAnywhere(const char* ns, const char* name) {
    using DomainGetT     = void* (*)();
    using DomainGetAsmsT = void* (*)(void*, size_t*);
    using AsmGetImageT   = void* (*)(void*);
    using ClassFromNameT = void* (*)(void*, const char*, const char*);

    auto dg  = Manifest::GetApi<DomainGetT>("il2cpp_domain_get");
    auto dga = Manifest::GetApi<DomainGetAsmsT>("il2cpp_domain_get_assemblies");
    auto agi = Manifest::GetApi<AsmGetImageT>("il2cpp_assembly_get_image");
    auto cfn = Manifest::GetApi<ClassFromNameT>("il2cpp_class_from_name");
    if (!dg || !dga || !agi || !cfn) return nullptr;

    void* domain = nullptr;
    void** asms  = nullptr;
    size_t n = 0;
    __try {
        domain = dg();
        if (!domain) return nullptr;
        asms = (void**)dga(domain, &n);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    if (!asms || n == 0 || n > 10000) return nullptr;

    for (size_t i = 0; i < n; ++i) {
        if (!asms[i]) continue;
        void* img = nullptr;
        __try { img = agi(asms[i]); } __except (EXCEPTION_EXECUTE_HANDLER) { continue; }
        if (!img) continue;
        void* klass = nullptr;
        __try { klass = cfn(img, ns, name); } __except (EXCEPTION_EXECUTE_HANDLER) { continue; }
        if (klass) return klass;
    }
    return nullptr;
}

static inline void* ResolveMethod(void* klass, const char* name, int argc) {
    if (!klass) return nullptr;
    if (!g_classGetMethod) {
        g_classGetMethod = Manifest::GetApi<FnClassGetMethod>("il2cpp_class_get_method_from_name");
    }
    if (!g_classGetMethod) return nullptr;
    const void* mi = nullptr;
    __try { mi = g_classGetMethod(klass, name, argc); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    if (!mi) return nullptr;
    return *(void**)mi;
}

inline bool ResolveAPIs() {
    if (g_resolveOK)    return true;
    if (g_resolveTried) return false;
    g_resolveTried = true;

    auto resolveClass = [](const char* ns, const char* name) -> void* {

        static const char* kImages[] = {
            "UnityEngine.CoreModule.dll",
            "UnityEngine.UIModule.dll",
            "UnityEngine.UI.dll",
            "UnityEngine.EventSystemsModule.dll",
            "UnityEngine.InputModule.dll",
            "UnityEngine.IMGUIModule.dll",
        };
        for (const char* imgName : kImages) {
            void* img = Manifest::detail::SafeImageByName(imgName);
            if (!img) continue;
            void* k = Manifest::detail::SafeClassFromName(img, ns, name);
            if (k) return k;
        }

        return FindClassAnywhere(ns, name);
    };

    void* cObject     = resolveClass("UnityEngine", "Object");
    void* cGameObject = resolveClass("UnityEngine", "GameObject");
    void* cTransform  = resolveClass("UnityEngine", "Transform");
    void* cEvtSys     = resolveClass("UnityEngine.EventSystems", "EventSystem");
    void* cTMPText    = resolveClass("TMPro", "TMP_Text");
    if (!cTMPText) cTMPText = resolveClass("TMPro", "TextMeshProUGUI");
    if (!cTMPText) cTMPText = resolveClass("TMPro", "TextMeshPro");

    if (!cObject || !cGameObject || !cTransform) {
        std::cout << "[DropStackInjector] Unity class lookup failed (Obj="
                  << cObject << " GO=" << cGameObject << " Tr=" << cTransform << ").\n";
        return false;
    }

    pInstantiate       = (tInstantiate)       ResolveMethod(cObject,     "Instantiate",   2);
    pGetTransform      = (tGetTransform)      ResolveMethod(cGameObject, "get_transform", 0);
    pGetParent         = (tGetParent)         ResolveMethod(cTransform,  "get_parent",    0);
    pGOSetName         = (tGOSetName)         ResolveMethod(cGameObject, "set_name",      1);
    pGOGetName         = (tGOGetName)         ResolveMethod(cGameObject, "get_name",      0);
    if (cEvtSys) {
        pEvtSysCurrent     = (tEvtSysCurrent)     ResolveMethod(cEvtSys, "get_current",                    0);
        pEvtSysGetSelected = (tEvtSysGetSelected) ResolveMethod(cEvtSys, "get_currentSelectedGameObject",  0);
    }

    pGetCompInChildren = (tGetCompInChildren) ResolveMethod(cGameObject, "GetComponentInChildren", 1);

    pClassGetType  = Manifest::GetApi<tClassGetType >("il2cpp_class_get_type");
    pTypeGetObject = Manifest::GetApi<tTypeGetObject>("il2cpp_type_get_object");
    if (cTMPText && pClassGetType && pTypeGetObject) {
        __try {
            void* tmpTypeRaw = pClassGetType(cTMPText);
            if (tmpTypeRaw) g_TMPTypeObject = pTypeGetObject(tmpTypeRaw);
        } __except (EXCEPTION_EXECUTE_HANDLER) { g_TMPTypeObject = nullptr; }
    }

    g_resolveOK = (pInstantiate && pGetTransform && pGetParent
                && pEvtSysCurrent && pEvtSysGetSelected);
    if (g_resolveOK) {
        std::cout << "[DropStackInjector] Unity APIs OK (Instantiate=" << pInstantiate
                  << " EvtSys=" << pEvtSysCurrent << ").\n";
    } else {
        std::cout << "[DropStackInjector] Resolve failed:"
                  << " Instantiate=" << pInstantiate
                  << " GetTr="       << pGetTransform
                  << " GetParent="   << pGetParent
                  << " EvtSysCur="   << pEvtSysCurrent
                  << " EvtSysGS="    << pEvtSysGetSelected << "\n";
    }
    return g_resolveOK;
}

static inline void* GetSelectedGameObject() {
    if (!pEvtSysCurrent || !pEvtSysGetSelected) return nullptr;
    void* es = nullptr;
    __try { es = pEvtSysCurrent(); } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    if (!es) return nullptr;
    void* go = nullptr;
    __try { go = pEvtSysGetSelected(es); } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    return go;
}

static inline void* CloneGO(void* templateGO) {
    if (!templateGO || !pInstantiate || !pGetTransform || !pGetParent) return nullptr;
    void* tr = nullptr;
    __try { tr = pGetTransform(templateGO); } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    if (!tr) return nullptr;
    void* parent = nullptr;
    __try { parent = pGetParent(tr); } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    if (!parent) return nullptr;
    void* clone = nullptr;
    __try { clone = pInstantiate(templateGO, parent); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    return clone;
}

static inline void RenameGO(void* go, const char* newName) {
    if (!go || !pGOSetName || !newName || !oIl2CppStringNew) return;
    Il2CppString* s = (Il2CppString*)oIl2CppStringNew(newName);
    if (!s) return;
    __try { pGOSetName(go, s); } __except (EXCEPTION_EXECUTE_HANDLER) {}
}

}

inline void __fastcall hkInvDropButtonClicked(void* ic, void* methodInfo) {
    using namespace DropStackInjector;

    auto dropFullStack = [&](void* invCtl) -> bool {
        if (!invCtl || !oInvActualDrop) return false;
        int slot = 0;
        Il2CppArray* items = nullptr;
        Il2CppArray* counts = nullptr;
        __try {
            slot   = *(int*)((uintptr_t)invCtl + 0x17C);
            items  = *(Il2CppArray**)((uintptr_t)invCtl + 0x188);
            counts = *(Il2CppArray**)((uintptr_t)invCtl + 0x190);
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        if (!items || !counts || slot < 0 || slot >= (int)items->max_length) return false;
        PlayerData_InventoryKey ik{0,0};
        int amount = 0;
        __try {
            ik     = ((PlayerData_InventoryKey*)items->items)[slot];
            amount = (int)((short*)counts->items)[slot];
        } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        if (amount <= 0) return false;
        __try { oInvActualDrop(invCtl, methodInfo, ik, amount, nullptr); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
        return true;
    };

    ResolveAPIs();
    void* sourceGO = GetSelectedGameObject();
    if (g_clonedBtnGO && sourceGO && sourceGO == g_clonedBtnGO) {
        if (dropFullStack(ic)) return;

    }

    if (!g_clonedBtnGO && sourceGO) {
        void* clone = CloneGO(sourceGO);
        if (clone) {
            g_clonedBtnGO = clone;
            RenameGO(clone, "ZModDropStackButton");
            std::cout << "[DropStackInjector] Drop Stack button injected ("
                      << clone << ").\n";
        }
    }

    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    if (g_dropStackShift && shift && ic && oInvActualDrop) {
        int  slot = 0;
        Il2CppArray* items  = nullptr;
        Il2CppArray* counts = nullptr;
        __try {
            slot   = *(int*)((uintptr_t)ic + 0x17C);
            items  = *(Il2CppArray**)((uintptr_t)ic + 0x188);
            counts = *(Il2CppArray**)((uintptr_t)ic + 0x190);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            items = nullptr; counts = nullptr;
        }
        if (items && counts && slot >= 0 && slot < (int)items->max_length) {
            PlayerData_InventoryKey ik{0, 0};
            int amount = 0;
            __try {
                PlayerData_InventoryKey* iarr = (PlayerData_InventoryKey*)items->items;
                short* carr = (short*)counts->items;
                ik = iarr[slot];
                amount = (int)carr[slot];
            } __except (EXCEPTION_EXECUTE_HANDLER) { amount = 0; }
            if (amount > 0) {
                __try { oInvActualDrop(ic, methodInfo, ik, amount, nullptr); }
                __except (EXCEPTION_EXECUTE_HANDLER) {}
                return;
            }
        }
    }
    if (oInvDropButtonClicked) oInvDropButtonClicked(ic, methodInfo);
}

inline float    g_extraZoomMaxOrtho      = 0.f;
inline void*    g_extraZoomMaxOrthoWorld = nullptr;
inline int      g_extraZoomMaxOrthoSizeX = 0;
inline int      g_extraZoomMaxOrthoSizeY = 0;

static inline void RecomputeExtraZoomMaxOrtho() {
    g_extraZoomMaxOrtho = 0.f;
    if (!g_WorldController || !g_off_WCWorld) return;
    void* world = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!world) return;
    int wsx = 0, wsy = 0;
    __try {
        wsx = *(int*)((uintptr_t)world + g_off_WorldSize);
        wsy = *(int*)((uintptr_t)world + g_off_WorldSize + 4);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return; }
    if (wsx <= 0 || wsy <= 0 || wsx > 5000 || wsy > 5000) return;

    const float tileW = 0.64f;
    float worldW = wsx * tileW;
    float worldH = wsy * tileW;

    float sw = 1920.f, sh = 1080.f;
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x > 1.f) sw = io.DisplaySize.x;
        if (io.DisplaySize.y > 1.f) sh = io.DisplaySize.y;
    }
    float capH = worldH * 0.5f;
    float capW = worldW * 0.5f * (sh / sw);
    g_extraZoomMaxOrtho      = (capH < capW) ? capH : capW;
    g_extraZoomMaxOrthoWorld = world;
    g_extraZoomMaxOrthoSizeX = wsx;
    g_extraZoomMaxOrthoSizeY = wsy;
}

inline void __fastcall hkSetOrthographicSize(void* camera, float value, void* methodInfo) {

    if (g_extraZoom) {
        const bool haveCache = (g_mainCameraCached != nullptr);
        const bool isMain    = (!haveCache) || (camera != nullptr && camera == g_mainCameraCached);
        if (isMain) {
            float mul = g_extraZoomMul;
            if (mul < 0.3f)  mul = 0.3f;
            if (mul > 20.0f) mul = 20.0f;
            if (fabsf(mul - 1.0f) > 0.001f) value *= mul;

            if (g_WorldController && g_off_WCWorld) {
                void* currentWorld = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
                if (currentWorld != g_extraZoomMaxOrthoWorld || g_extraZoomMaxOrtho <= 0.f) {
                    RecomputeExtraZoomMaxOrtho();
                }
            }
            if (g_extraZoomMaxOrtho > 0.f && value > g_extraZoomMaxOrtho) {
                value = g_extraZoomMaxOrtho;
            }
        }
    }

    if (oSetOrthographicSize) oSetOrthographicSize(camera, value, methodInfo);
}

inline void __fastcall hkKC_UpdateZoomByZoomValue(void* kc, void* methodInfo) {
    if (kc) g_KukouriCameraCached = kc;
    if (g_extraZoom && g_extraZoomMul > 1.0f && kc) {

        int level    = 0;
        int maxLevel = 5;
        __try {
            level    = *(int*)((uintptr_t)kc + g_off_KC_cameraZoomLevel);
            maxLevel = *(int*)((uintptr_t)kc + g_off_KC_maxZoom);
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
        if (level >= maxLevel) {
            float* pv = (float*)((uintptr_t)kc + g_off_KC_zoomValue);
            if (!IsBadWritePtr(pv, sizeof(float))) {
                float saved = *pv;
                *pv = saved * g_extraZoomMul;
                if (oKC_UpdateZoomByZoomValue) oKC_UpdateZoomByZoomValue(kc, methodInfo);
                *pv = saved;
                return;
            }
        }
    }
    if (oKC_UpdateZoomByZoomValue) oKC_UpdateZoomByZoomValue(kc, methodInfo);
}
inline const char* __fastcall hkCensor(const char* str) { return AntiCensor ? str : oCensor(str); }
inline bool __fastcall hkCanPlaceSeedTopOf(int blockType) { return PlaceSeedAnywhere ? true : oCanPlaceSeedTopOf(blockType); }
inline void __fastcall hkRandomizeFishTargetPoint(void* t, bool f) { if (StopFishMovement) return; if (oRandomizeFishTargetPoint) oRandomizeFishTargetPoint(t, f); }
inline float __fastcall hkGetBlockRunSpeed(int blockType) { return oGetBlockRunSpeed(blockType) * runSpeedMultiplier; }
inline void __fastcall hkUpdateCameraPosition(void* t) { if (!FreecamEnabled && oUpdateCameraPosition) oUpdateCameraPosition(t); }
inline void __fastcall hkForceUpdatePosition(void* t, bool f) { if (!FreecamEnabled && oForceUpdatePosition) oForceUpdatePosition(t, f); }
inline float __cdecl hkGetCooldownToUseFist(float multiplier) { if (ExtraFasthitEnabled) return 0.001f; if (fastHitEnabled) return 0.18f; return oGetCooldownToUseFist(multiplier); }
inline bool __fastcall hkHasUnlockedRecipe(void* thisptr, void* edx, int blockType) { if (unlockAllRecipes) return true; return oHasUnlockedRecipe(thisptr, edx, blockType); }
inline void __fastcall hkDeathByColliderInCollider(void* thisptr, void* edx, Vector2i mapPoint) { if (noBlockKill || GodModeEnabled) return; oDeathByColliderInCollider(thisptr, edx, mapPoint); }

inline void __fastcall hkSetWorldLighting(void* instance, int lightingType, void* methodInfo) {
    if (AntiDarkness) return;
    if (oSetWorldLighting) oSetWorldLighting(instance, lightingType, methodInfo);
}

inline void __fastcall hkInstantiateFogOfWar(void* instance, void* methodInfo) {
    if (AntiDarkness) return;
    if (oInstantiateFogOfWar) oInstantiateFogOfWar(instance, methodInfo);
}
inline Vector3 __fastcall hkWorldToScreenPoint(void* camera, Vector3 position) { return oWorldToScreenPoint(camera, position); }
inline Vector3 __fastcall hkGetPosition(void* instance) { return oGetPosition(instance); }
inline void* __fastcall hkGetTransform(void* instance) { return oGetTransform(instance); }
inline void* __fastcall hkGetMainCamera() { return oGetMainCamera(); }
inline void __fastcall hkSetPosition(void* instance, Vector3 value) { if (value.y < 0.5f) value.y = 0.5f; if (oSetPosition) oSetPosition(instance, value); }
inline Vector3 __fastcall hkScreenToWorldPoint(void* __this, Vector3 screenPosition) { return oScreenToWorldPoint(__this, screenPosition); }
inline void __fastcall hkSetVelocity(void* player, Vector3 newVelocity) { if (oSetVelocity) oSetVelocity(player, newVelocity); }
inline bool __fastcall hkIsPlayerInMapPoint(void* instance, Vector2i mapPoint) {
    if (g_BlockOnPlayer) return false;
    return oIsPlayerInMapPoint(instance, mapPoint);
}

inline bool g_antiTrap = false; 

typedef bool(__fastcall* tCheckShouldTrapBeOn)(void* instance, Vector2i mapPoint, bool doNotSendAnything, void* beamMapPoints);
typedef bool(__fastcall* tCheckShouldTrapBeOnByWiring)(void* instance, Vector2i mapPoint, void* beamMapPoints);

inline tCheckShouldTrapBeOn oCheckShouldTrapBeOn = nullptr;
inline tCheckShouldTrapBeOnByWiring oCheckShouldTrapBeOnByWiring = nullptr;

inline bool __fastcall hkCheckShouldTrapBeOn(void* instance, Vector2i mapPoint, bool doNotSendAnything, void* beamMapPoints) {
    if (g_antiTrap) {
        return false; 
    }
    return oCheckShouldTrapBeOn(instance, mapPoint, doNotSendAnything, beamMapPoints);
}

inline bool __fastcall hkCheckShouldTrapBeOnByWiring(void* instance, Vector2i mapPoint, void* beamMapPoints) {
    if (g_antiTrap) {
        return false; 
    }
    return oCheckShouldTrapBeOnByWiring(instance, mapPoint, beamMapPoints);
}

inline bool g_unlimitedDeaths = false; 

typedef int(__fastcall* tGetPlayerMaxDeathsCount)(void* instance, void* playerData);
inline tGetPlayerMaxDeathsCount oGetPlayerMaxDeathsCount = nullptr;

inline int __fastcall hkGetPlayerMaxDeathsCount(void* instance, void* playerData) {
    if (g_unlimitedDeaths) {
       
        return 99999;
    }
    
    return oGetPlayerMaxDeathsCount(instance, playerData);
}

inline tUpdate oNetworkClientUpdate = nullptr;

namespace AutoMine { namespace AutoLoop { inline void TickAutoRejoin(); } }

inline void __fastcall hkNetworkClientUpdate(void* instance) {
    if (oNetworkClientUpdate) oNetworkClientUpdate(instance);

    DrainRawBsonSendQueue();
    AutoMineworldLogic();

    AutoMine::AutoLoop::TickAutoRejoin();
}

inline float g_recastDelaySeconds = 1.5f;

typedef PlayerData_InventoryKey(__fastcall* tGetCurrentSelection)(void* inventoryControl);
inline tGetCurrentSelection oGetCurrentSelection = nullptr;

#pragma pack(push, 1)
struct FishingGaugeMinigameUI {
    char  pad_0000[0x108];
    float progress;
    char  pad_010c[0x38];
    float fishPosition;
    float fishSizeMultiplier;

    char  pad_014c[0x04];
    float fishVelocity;
    char  pad_0154[0x3C];

    float targetAreaPosition;

};
#pragma pack(pop)

typedef void(__fastcall* tFishingGaugeMinigameUI_Update)(FishingGaugeMinigameUI* instance);
inline tFishingGaugeMinigameUI_Update oFishingGaugeMinigameUI_Update = nullptr;

typedef void(__fastcall* tFishingGaugeMinigameUI_SetTargetAreaPosition)(FishingGaugeMinigameUI* instance);
inline tFishingGaugeMinigameUI_SetTargetAreaPosition oFishingGaugeMinigameUI_SetTargetAreaPosition = nullptr;

typedef void(__fastcall* tLandButtonPressed)(FishingGaugeMinigameUI* instance);
inline tLandButtonPressed LandButtonPressed = nullptr;

typedef bool(__fastcall* tSetBaitWithTool)(void* worldController, World::BlockType lure, Vector2i mapPoint, float timeToWait, void* methodInfo);
inline tSetBaitWithTool oSetBaitWithTool = nullptr;

typedef bool(__cdecl* tIsFishingLure)(World::BlockType blockType);
inline tIsFishingLure IsFishingLure = nullptr;

typedef bool(__fastcall* tIsFishStrikeActive)(void* instance);
inline tIsFishStrikeActive oIsFishStrikeActive = nullptr;

inline void* g_FishingResultsPopupInstance = nullptr;

typedef void(__fastcall* tTakeFishPressed)(void* instance);
inline tTakeFishPressed oTakeFishPressed = nullptr;

typedef void* (__fastcall* tFishingResultsPopup_DoShowAnimation)(void* instance, bool doInstant);
inline tFishingResultsPopup_DoShowAnimation oFishingResultsPopup_DoShowAnimation = nullptr;

typedef void* (__fastcall* tFishingResultsPopup_DoHideAnimation)(void* instance, bool doInstant);
inline tFishingResultsPopup_DoHideAnimation oFishingResultsPopup_DoHideAnimation = nullptr;

typedef SHORT (WINAPI* tGetAsyncKeyState)(int vKey);
inline tGetAsyncKeyState oGetAsyncKeyState = nullptr;
inline bool g_virtualKeyState[256] = {};

inline SHORT WINAPI hkGetAsyncKeyState(int vKey) {
    if (vKey >= 0 && vKey < 256 && g_virtualKeyState[vKey])
        return SHORT(0x8001);
    return oGetAsyncKeyState(vKey);
}

typedef void(__fastcall* tOnButtonDownNew)(void* gameplayUI, int buttonIndex);
inline tOnButtonDownNew oOnButtonDownNew = nullptr;

inline void* g_gameplayUIField = nullptr;
inline void* GetGameplayUI() {
    if (!g_gameplayUIField || !il2cpp_field_static_get_value) return nullptr;
    void* instance = nullptr;
    il2cpp_field_static_get_value(g_gameplayUIField, &instance);
    return instance;
}

typedef void(__fastcall* tClosePopupFish)(void* instance);
inline tClosePopupFish oClosePopupFish = nullptr;

typedef bool(__fastcall* tAreMapPointsValidForFishing)(void* world, Vector2i playerPos, Vector2i tilePos);
inline tAreMapPointsValidForFishing oAreMapPointsValidForFishing = nullptr;

inline void* g_field_playerChangeToSleepSeconds           = nullptr;
inline void* g_field_playerInactivitySeconds              = nullptr;
inline void* g_field_playerInactivitySecondsFishing       = nullptr;
inline void* g_field_playerInactivitySecondsMatchmaking   = nullptr;

inline void SetAntiSleepFields() {
    if (!il2cpp_field_static_set_value) return;
    static const int kMax = 2147483647;
    if (g_field_playerChangeToSleepSeconds)         il2cpp_field_static_set_value(g_field_playerChangeToSleepSeconds,         (void*)&kMax);
    if (g_field_playerInactivitySeconds)            il2cpp_field_static_set_value(g_field_playerInactivitySeconds,            (void*)&kMax);
    if (g_field_playerInactivitySecondsFishing)     il2cpp_field_static_set_value(g_field_playerInactivitySecondsFishing,     (void*)&kMax);
    if (g_field_playerInactivitySecondsMatchmaking) il2cpp_field_static_set_value(g_field_playerInactivitySecondsMatchmaking, (void*)&kMax);
}

inline void* g_field_rocketFuelConsumptionSpeed       = nullptr;
inline void* g_field_rocketFuelConsumptionSpeed60FPS  = nullptr;
inline float g_origRocketFuelConsumptionSpeed         = 1.15f;
inline float g_origRocketFuelConsumptionSpeed60FPS    = 1.29f;
inline bool  g_jetpackOriginalsCaptured               = false;

inline void WriteJetpackFuelFields() {
    if (!il2cpp_field_static_set_value || !il2cpp_field_static_get_value) return;
    if (!g_field_rocketFuelConsumptionSpeed || !g_field_rocketFuelConsumptionSpeed60FPS) return;

    if (!g_jetpackOriginalsCaptured) {
        float a = 1.15f, b = 1.29f;
        il2cpp_field_static_get_value(g_field_rocketFuelConsumptionSpeed,      &a);
        il2cpp_field_static_get_value(g_field_rocketFuelConsumptionSpeed60FPS, &b);

        if (a > 0.0001f) g_origRocketFuelConsumptionSpeed = a;
        if (b > 0.0001f) g_origRocketFuelConsumptionSpeed60FPS = b;
        g_jetpackOriginalsCaptured = true;
    }

    if (InfiniteJetpack) {
        static const float zero = 0.0f;
        il2cpp_field_static_set_value(g_field_rocketFuelConsumptionSpeed,      (void*)&zero);
        il2cpp_field_static_set_value(g_field_rocketFuelConsumptionSpeed60FPS, (void*)&zero);
    } else {
        il2cpp_field_static_set_value(g_field_rocketFuelConsumptionSpeed,      &g_origRocketFuelConsumptionSpeed);
        il2cpp_field_static_set_value(g_field_rocketFuelConsumptionSpeed60FPS, &g_origRocketFuelConsumptionSpeed60FPS);
    }
}

typedef Il2CppArray*(__fastcall* tGetInventoryAsOrderedByInventoryItemType)(void* playerData);
inline tGetInventoryAsOrderedByInventoryItemType oGetInventoryAsOrderedByInventoryItemType = nullptr;

typedef short(__fastcall* tGetCountByKey)(void* playerData, PlayerData_InventoryKey key);
inline tGetCountByKey oGetCountByKey = nullptr;

typedef void(__fastcall* tAddGems)(void* playerData, int addAmount);
inline tAddGems oAddGems = nullptr;

typedef int(__fastcall* tGetGemAmount)(void* playerData);
inline tGetGemAmount oGetGemAmount = nullptr;

typedef int(__fastcall* tGetByteCoinAmount)(void* playerData);
inline tGetByteCoinAmount oGetByteCoinAmount = nullptr;

inline int GetLocalPlayerLevel() {
    if (!g_LocalPlayer || !oGetXPLevel) return 0;
    void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
    if (!pd) return 0;
    int xp = *(int*)((uintptr_t)pd + Offsets::PlayerData_xpAmount);
    if (xp < 0 || xp > 1000000000) return 0;
    int level = 0;
    __try { level = oGetXPLevel(xp); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    return level;
}

inline int GetInventoryCount(int blockType, int itemType) {
    if (!g_LocalPlayer || !oGetCountByKey) return 0;
    void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
    if (!pd) return 0;
    PlayerData_InventoryKey k{ blockType, itemType };
    short qty = 0;
    __try { qty = oGetCountByKey(pd, k); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    return (int)qty;
}

inline uintptr_t g_off_Gauge_isReadyToLand        = 0;
inline uintptr_t g_off_Gauge_fishTargetPoint      = 0;
inline uintptr_t g_off_Gauge_targetAreaMaxVelocity = 0;

inline uintptr_t g_off_Player_fishingState       = 0;
inline uintptr_t g_off_Player_fishingLeftButton  = 0;
inline uintptr_t g_off_Player_fishingRightButton = 0;
inline uintptr_t g_off_Player_jumpButtonDown     = 0;
inline uintptr_t g_off_Player_leftButton         = 0;
inline uintptr_t g_off_Player_rightButton        = 0;
inline uintptr_t g_off_Player_jumpButton         = 0;

inline uintptr_t g_off_Player_gravity            = 0x1AC;
inline uintptr_t g_off_Player_lastFrameVelocityY = 0x148;

inline bool  g_pfNavActive    = false;
inline float g_pfSavedGravity = 0.f;

inline bool  g_pfGravPulse      = false;
inline int   g_pfGravPulseOnMs  = 1500;
inline int   g_pfGravPulseOffMs = 100;

inline std::chrono::steady_clock::time_point g_pfPulseStart =
    std::chrono::steady_clock::now();

inline void PathFinder_BeginNav() {
    if (!g_LocalPlayer) return;
    if (!g_pfNavActive) {
        __try {
            float* g = (float*)((uintptr_t)g_LocalPlayer + g_off_Player_gravity);
            g_pfSavedGravity = *g;
        } __except (EXCEPTION_EXECUTE_HANDLER) { g_pfSavedGravity = 0.f; }
        g_pfPulseStart = std::chrono::steady_clock::now();
    }
    g_pfNavActive = true;
}

inline void PathFinder_GravityTick() {
    if (!g_pfNavActive || !g_LocalPlayer) return;

    bool zeroNow = true;
    if (g_pfGravPulse && (g_pfGravPulseOnMs + g_pfGravPulseOffMs) > 0) {
        auto now   = std::chrono::steady_clock::now();
        long long ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - g_pfPulseStart).count();
        int total  = g_pfGravPulseOnMs + g_pfGravPulseOffMs;
        int phase  = (int)(ms % total);
        zeroNow    = (phase < g_pfGravPulseOnMs);
    }

    g_pfGravityOverridden = zeroNow;
    __try {
        float* g = (float*)((uintptr_t)g_LocalPlayer + g_off_Player_gravity);
        if (zeroNow) {

            *g = 0.f;
            float* v = (float*)((uintptr_t)g_LocalPlayer + g_off_Player_lastFrameVelocityY);
            *v = 0.f;
        } else {
            *g = g_pfSavedGravity;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
}

inline void PathFinder_EndNav() {
    if (!g_pfNavActive) return;
    if (g_LocalPlayer) {
        __try {
            float* g = (float*)((uintptr_t)g_LocalPlayer + g_off_Player_gravity);
            *g = g_pfSavedGravity;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    g_pfNavActive         = false;
    g_pfGravityOverridden = false;
}

inline void PathFinder_DisableGravity() { PathFinder_BeginNav(); PathFinder_GravityTick(); }
inline void PathFinder_RestoreGravity() { PathFinder_EndNav(); }

inline void PathFinder_WriteInputs(bool left, bool right, bool jump) {
    if (!g_LocalPlayer) return;
    __try {
        if (g_off_Player_leftButton  > 0)
            *(bool*)((uintptr_t)g_LocalPlayer + g_off_Player_leftButton)  = left;
        if (g_off_Player_rightButton > 0)
            *(bool*)((uintptr_t)g_LocalPlayer + g_off_Player_rightButton) = right;
        if (g_off_Player_jumpButton  > 0)
            *(bool*)((uintptr_t)g_LocalPlayer + g_off_Player_jumpButton)  = jump;
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
}
inline void PathFinder_ClearInputs() { PathFinder_WriteInputs(false, false, false); }

inline void PathFinder_PulseJumpDown() {
    if (!g_LocalPlayer) return;
    __try {
        if (g_off_Player_jumpButtonDown > 0)
            *(bool*)((uintptr_t)g_LocalPlayer + g_off_Player_jumpButtonDown) = true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
}

inline World::BlockType GetBlockTypeFromPoint(Vector2i mapPoint) {

    if (!g_WorldController) return World::BlockType::None;

    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return World::BlockType::None;

    void* worldBlockLayer_ptr = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!worldBlockLayer_ptr || IsBadReadPtr(worldBlockLayer_ptr, 0x20)) return World::BlockType::None;

    int worldHeight = *(int*)((uintptr_t)worldBlockLayer_ptr + 0x18);
    if (worldHeight <= 0 || worldHeight > 500) return World::BlockType::None;
    void** pRows = (void**)((uintptr_t)worldBlockLayer_ptr + 0x20);
    int worldWidth = 0;
    for (int i = 0; i < worldHeight && worldWidth == 0; ++i)
        if (pRows[i]) worldWidth = *(int*)((uintptr_t)pRows[i] + 0x18);
    if (worldWidth <= 0 || worldWidth > 500) return World::BlockType::None;

    if (mapPoint.x < 0 || mapPoint.x >= worldWidth || mapPoint.y < 0 || mapPoint.y >= worldHeight)
        return World::BlockType::None;

    auto getBlockFromLayer = [&](void* layerPtr) -> World::BlockType {
        if (!layerPtr) return World::BlockType::None;

        void** rows = (void**)((uintptr_t)layerPtr + 0x20);
        if (!rows || !rows[mapPoint.y]) return World::BlockType::None;
        char* pRowData = (char*)((uintptr_t)rows[mapPoint.y] + 0x20);
        if (!pRowData) return World::BlockType::None;
        uintptr_t structAddress = (uintptr_t)(pRowData + (mapPoint.x * 0x28));
        if (IsBadReadPtr((void*)structAddress, sizeof(World::BlockType))) return World::BlockType::None;
        return *(World::BlockType*)structAddress;
    };

    World::BlockType block = getBlockFromLayer(worldBlockLayer_ptr);
    if (block != World::BlockType::None) return block;

    return getBlockFromLayer(*(void**)((uintptr_t)worldObject + Offsets::World_worldItemDataLayer));
}

inline World::BlockType GetForegroundBlockAt(Vector2i mapPoint) {
    if (!g_WorldController) return World::BlockType::None;
    void* worldObject = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
    if (!worldObject) return World::BlockType::None;
    void* fg = *(void**)((uintptr_t)worldObject + g_off_WorldBlockLayer);
    if (!fg || IsBadReadPtr(fg, 0x20)) return World::BlockType::None;
    int worldHeight = *(int*)((uintptr_t)fg + 0x18);
    if (worldHeight <= 0 || worldHeight > 500) return World::BlockType::None;
    void** pRows = (void**)((uintptr_t)fg + 0x20);
    int worldWidth = 0;
    for (int i = 0; i < worldHeight && worldWidth == 0; ++i)
        if (pRows[i]) worldWidth = *(int*)((uintptr_t)pRows[i] + 0x18);
    if (worldWidth <= 0 || worldWidth > 500) return World::BlockType::None;
    if (mapPoint.x < 0 || mapPoint.x >= worldWidth || mapPoint.y < 0 || mapPoint.y >= worldHeight)
        return World::BlockType::None;
    if (!pRows[mapPoint.y]) return World::BlockType::None;
    char* pRowData = (char*)((uintptr_t)pRows[mapPoint.y] + 0x20);
    uintptr_t structAddress = (uintptr_t)(pRowData + (mapPoint.x * 0x28));
    if (IsBadReadPtr((void*)structAddress, sizeof(World::BlockType))) return World::BlockType::None;
    return *(World::BlockType*)structAddress;
}

namespace FishBot {

    inline bool  g_enabled            = false;
    inline bool  g_autoLand           = true;
    inline bool  g_autoRecast         = true;
    inline bool  g_legitMode          = true;
    inline bool  g_antiSleep          = true;
    inline bool  g_antiAfk            = true;
    inline float g_recastDelayMinSec  = 1.0f;
    inline float g_recastDelayMaxSec  = 2.5f;
    inline float g_hookReactionMinMs  = 160.0f;
    inline float g_hookReactionMaxMs  = 600.0f;

    inline float g_legitPauseDelayMin  = 5.0f;
    inline float g_legitPauseDelayMax  = 12.0f;
    inline float g_legitPauseDurMin    = 0.8f;
    inline float g_legitPauseDurMax    = 2.5f;

    inline int   g_stats_caught  = 0;
    inline int   g_stats_missed  = 0;
    inline int   g_stats_skipped = 0;
    inline std::chrono::steady_clock::time_point g_stats_startTime;

    enum FishSizeBucketId { FSB_Tiny = 0, FSB_Small, FSB_Medium, FSB_Large, FSB_Huge, FSB_COUNT };
    inline bool  g_sizeFilterEnabled = false;
    inline bool  g_keepSize[FSB_COUNT] = { true, true, true, true, true };
    inline float g_lastFishSize      = 0.f;
    inline int   g_lastFishBucket    = -1;
    inline bool  g_rejectCurrentFish = false;
    inline const char* FishSizeName(int b) {
        switch (b) { case FSB_Tiny: return "Tiny"; case FSB_Small: return "Small";
                     case FSB_Medium: return "Medium"; case FSB_Large: return "Large";
                     case FSB_Huge: return "Huge"; default: return "?"; }
    }

    inline int FishSizeBucket(float m) {
        if (m < 0.90f) return FSB_Tiny;
        if (m < 1.10f) return FSB_Small;
        if (m < 1.40f) return FSB_Medium;
        if (m < 1.80f) return FSB_Large;
        return FSB_Huge;
    }

    typedef void(__fastcall* tFishStopMini)(int miniGameType, int actionResult, void* mi);
    inline tFishStopMini oFishStopMini = nullptr;
    constexpr int kMiniGameFishing = 2;

    static bool Seh_StopFishingMinigame() {
        if (!oFishStopMini) return false;
        __try { oFishStopMini(kMiniGameFishing, 0, nullptr); return true; }
        __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    }

    typedef void(__fastcall* tSetupMinigame)(int rod, int caughtFish, void* mi);
    inline tSetupMinigame oSetupMinigame = nullptr;
    typedef int (__fastcall* tGetFishSizeByBlockType)(int blockType, void* mi);
    inline tGetFishSizeByBlockType oGetFishSizeByBlockType = nullptr;
    inline int g_currentFishGameSize = 0;

    static int Seh_GetFishSize(int blockType) {
        if (!oGetFishSizeByBlockType) return 0;
        __try { return oGetFishSizeByBlockType(blockType, nullptr); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
    }

    inline void __fastcall hkSetupMinigame(int rod, int caughtFish, void* mi) {
        if (oSetupMinigame) oSetupMinigame(rod, caughtFish, mi);
        int gs = Seh_GetFishSize(caughtFish);
        g_currentFishGameSize = gs;
        int bucket = gs - 1;
        if (bucket >= 0 && bucket < FSB_COUNT) g_lastFishBucket = bucket;
        g_rejectCurrentFish = false;
        std::cout << "[FishBot/size] new fish blockType=" << caughtFish
                  << " size=" << gs << " ("
                  << ((bucket >= 0 && bucket < FSB_COUNT) ? FishSizeName(bucket) : "None")
                  << ")\n";
    }

    inline void* g_gaugeInstance = nullptr;
    inline bool  g_wasFishing    = false;
    inline bool  g_catchPending  = false;
    inline bool  g_needsRecast   = false;
    inline bool  g_strikePending = false;
    inline std::chrono::steady_clock::time_point g_recastAt;
    inline std::chrono::steady_clock::time_point g_strikeAt;
    inline void* g_cachedGameplayUI  = nullptr;
    inline int   g_castFailCount     = 0;

    inline World::BlockType g_activeLure  = World::BlockType::None;
    inline Vector2i         g_lastBaitPos = { -1, -1 };

    inline std::chrono::steady_clock::time_point g_idleStart;
    inline bool g_idleClockRunning = false;
    inline bool g_afkJumpDone      = false;

    inline bool  g_lPaused       = false;
    inline float g_lAccum        = 0.0f;
    inline float g_lCurrentDelay = 8.0f;
    inline float g_lCurrentDur   = 1.5f;

    inline Vector2i g_pendingBaitTile = { -1, -1 };
    inline std::chrono::steady_clock::time_point g_pendingBaitDeadline;
    inline int      g_lastFishingState = 0;
    inline bool     g_sawMinigameThisCast = false;

    inline float g_lastFishTargetPoint = -999.0f;
    inline std::chrono::steady_clock::time_point g_minigameStopUntil;

    inline float g_aimTarget = 0.5f;
    inline bool  g_aimInit   = false;
    inline std::chrono::steady_clock::time_point g_nextGlanceAt;
    static constexpr float REEL_AIM_ERROR    = 0.03f;
    static constexpr int   REEL_REACT_MIN_MS = 80;
    static constexpr int   REEL_REACT_MAX_MS = 150;

    inline bool  g_verboseMinigame = false;

    inline float g_speedMult = 1.0f;

    inline bool  g_returnToSpot  = true;
    inline float g_maxDriftTiles = 3.0f;

    struct WorldSpot { std::string world; Vector2i tile; };
    inline std::vector<WorldSpot> g_spotCache;
    inline std::string            g_lastWorldName;
    inline bool                   g_returning   = false;
    inline std::chrono::steady_clock::time_point g_returnStart;

    inline Vector2i SpotForWorld(const std::string& w) {
        for (auto& s : g_spotCache) if (s.world == w) return s.tile;
        return { -1, -1 };
    }
    inline void RememberSpot(const std::string& w, Vector2i t) {
        if (w.empty() || t.x < 0) return;
        for (auto& s : g_spotCache) if (s.world == w) { s.tile = t; return; }
        g_spotCache.push_back({ w, t });
        if (g_spotCache.size() > 64) g_spotCache.erase(g_spotCache.begin());
    }
    inline void ClearSpotCache() { g_spotCache.clear(); }

    inline float RandRange(float lo, float hi) {
        return lo + (hi - lo) * (float)rand() / (float)RAND_MAX;
    }

    inline int GetFishingState() {
        if (!g_LocalPlayer || g_off_Player_fishingState == 0) return 0;
        return *(int*)((char*)g_LocalPlayer + g_off_Player_fishingState);
    }

    inline void SetButtons(bool left, bool right) {
        if (!g_LocalPlayer) return;
        if (g_off_Player_fishingLeftButton > 0)
            *(bool*)((char*)g_LocalPlayer + g_off_Player_fishingLeftButton) = left;
        if (g_off_Player_fishingRightButton > 0)
            *(bool*)((char*)g_LocalPlayer + g_off_Player_fishingRightButton) = right;
    }

    inline bool ScanInventoryForLure(World::BlockType& outLure) {
        if (!g_LocalPlayer) {
            std::cout << "[FishBot] ScanInventoryForLure: g_LocalPlayer NULL\n"; return false;
        }
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) {
            std::cout << "[FishBot] ScanInventoryForLure: PlayerData NULL (offset=0x"
                      << std::hex << Offsets::Player_myPlayerData << std::dec << ")\n";
            return false;
        }
        if (!oGetInventoryAsOrderedByInventoryItemType || !IsFishingLure || !oGetCountByKey) {
            std::cout << "[FishBot] ScanInventoryForLure: missing pointers"
                      << " GetInv=" << (oGetInventoryAsOrderedByInventoryItemType ? "OK" : "NULL")
                      << " IsFishingLure=" << (IsFishingLure ? "OK" : "NULL")
                      << " GetCount=" << (oGetCountByKey ? "OK" : "NULL") << "\n";
            return false;
        }
        Il2CppArray* arr = nullptr;
        __try { arr = oGetInventoryAsOrderedByInventoryItemType(pd); }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            std::cout << "[FishBot] ScanInventoryForLure: SEH exception in GetInventory\n";
            return false;
        }
        if (!arr || !arr->max_length) {
            std::cout << "[FishBot] ScanInventoryForLure: inventory array empty/null\n";
            return false;
        }
        auto* keys = (PlayerData_InventoryKey*)arr->items;
        for (size_t i = 0; i < arr->max_length; ++i) {
            PlayerData_InventoryKey k = keys[i];
            if (!IsFishingLure((World::BlockType)k.blockType)) continue;
            short qty = 0;
            __try { qty = oGetCountByKey(pd, k); } __except (EXCEPTION_EXECUTE_HANDLER) { continue; }
            if (qty > 0) {
                outLure = (World::BlockType)k.blockType;
                std::cout << "[FishBot] ScanInventoryForLure: found lure type="
                          << k.blockType << " qty=" << qty << "\n";
                return true;
            }
        }
        std::cout << "[FishBot] ScanInventoryForLure: no lure found in inventory\n";
        return false;
    }

    inline bool FindCastTile(Vector2i& outTile) {
        Vector2i here;
        if (!GetPlayerMapPoint(here)) {
            std::cout << "[FishBot] FindCastTile: GetPlayerMapPoint failed\n";
            return false;
        }
        void* world = g_WorldController
            ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld) : nullptr;

        std::cout << "[FishBot] FindCastTile: player=(" << here.x << "," << here.y
                  << ") AMVFF=" << (oAreMapPointsValidForFishing ? "OK" : "NULL")
                  << " world=" << (world ? "OK" : "NULL") << "\n";

        Vector2i cands[] = {
            { here.x - 1, here.y - 1 }, { here.x + 1, here.y - 1 },
            { here.x,     here.y - 1 },
            { here.x - 2, here.y - 1 }, { here.x + 2, here.y - 1 },
            { here.x - 1, here.y     }, { here.x + 1, here.y     },
        };
        for (auto& c : cands) {
            bool ok = false;
            World::BlockType bt = World::BlockType::None;
            if (world && oAreMapPointsValidForFishing) {
                __try { ok = oAreMapPointsValidForFishing(world, here, c); }
                __except (EXCEPTION_EXECUTE_HANDLER) { ok = false; }
            }
            if (!ok) {

                bt = GetBlockTypeFromPoint(c);

                int bid = static_cast<int>(bt);
                ok = (bid >= 3725 && bid <= 3730) || (bid == 3724);
            }
            std::cout << "[FishBot] FindCastTile: probe (" << c.x << "," << c.y
                      << ") bt=" << static_cast<int>(bt) << " ok=" << ok << "\n";
            if (ok) { outTile = c; return true; }
        }
        std::cout << "[FishBot] FindCastTile: no valid tile found near (" << here.x << "," << here.y << ")\n";
        return false;
    }

    inline bool _SehSetBaitWithTool(World::BlockType lure, Vector2i tile) {
        if (!oSetBaitWithTool || !g_WorldController) return false;
        bool ok = false;
        __try { ok = oSetBaitWithTool(g_WorldController, lure, tile, 0.0f, nullptr); }
        __except (EXCEPTION_EXECUTE_HANDLER) { ok = false; }
        return ok;
    }

    inline bool CastLure() {

        World::BlockType lure = g_activeLure;
        if (lure == World::BlockType::None) {
            if (!ScanInventoryForLure(lure)) {
                std::cout << "[FishBot/cast] no lure in inventory — cast manually once.\n";
                g_needsRecast = true;
                g_recastAt = std::chrono::steady_clock::now() + std::chrono::seconds(2);
                return false;
            }
            g_activeLure = lure;
        }

        Vector2i tile = { -1, -1 };
        const char* tileSrc = "?";
        if (g_lastBaitPos.x != -1) {
            tile = g_lastBaitPos;
            tileSrc = "last-confirmed";
        }
        if (tile.x == -1) {
            Vector2i cached = SpotForWorld(GetCurrentWorldName());
            if (cached.x != -1) { tile = cached; tileSrc = "world-cache"; }
        }
        if (tile.x == -1) {
            if (!FindCastTile(tile)) {
                std::cout << "[FishBot/cast] no valid cast tile near player — "
                             "cast manually once to seed.\n";
                g_needsRecast = true;
                g_recastAt = std::chrono::steady_clock::now() + std::chrono::seconds(2);
                return false;
            }
            tileSrc = "auto-probe";
        }

        if (!oSetBaitWithTool || !g_WorldController) {
            std::cout << "[FishBot/cast] missing ptrs: SetBaitWithTool="
                      << (oSetBaitWithTool ? "OK" : "NULL")
                      << " WC=" << (g_WorldController ? "OK" : "NULL")
                      << " — retry in 1s\n";
            g_recastAt = std::chrono::steady_clock::now() + std::chrono::seconds(1);
            return false;
        }

        std::cout << "[FishBot/cast] SetBaitWithTool lure=" << (int)lure
                  << " tile=(" << tile.x << "," << tile.y
                  << ") src=" << tileSrc << "\n";

        bool ok = _SehSetBaitWithTool(lure, tile);

        if (ok) {

            g_pendingBaitTile     = tile;
            g_pendingBaitDeadline = std::chrono::steady_clock::now()
                                  + std::chrono::milliseconds(2000);
            g_sawMinigameThisCast = false;
            g_needsRecast         = false;
            g_castFailCount       = 0;
            std::cout << "[FishBot/cast] sent OK — awaiting fishingState confirm\n";
        } else {

            g_castFailCount++;
            if (g_castFailCount >= 3) {
                std::cout << "[FishBot/cast] " << g_castFailCount
                          << " consecutive fails — clearing lure/last-confirmed, re-scan\n";
                g_activeLure    = World::BlockType::None;
                g_lastBaitPos   = { -1, -1 };
                g_castFailCount = 0;
            } else {
                std::cout << "[FishBot/cast] returned false (fail " << g_castFailCount
                          << "/3) — retry in 2s\n";
            }
            g_recastAt = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        }
        return ok;
    }

    inline void ProcessIncomingPacket(const json& ) {

    }

    inline void OnEnable() {
        static bool s_seeded = false;
        if (!s_seeded) {
            srand((unsigned)GetCurrentProcessId() ^ (unsigned)GetTickCount());
            s_seeded = true;
        }
        g_stats_caught  = 0;
        g_stats_missed  = 0;
        g_stats_skipped = 0;
        g_stats_startTime = std::chrono::steady_clock::now();
        g_wasFishing    = false;
        g_catchPending  = false;
        g_needsRecast   = true;
        g_strikePending = false;
        g_lPaused       = false;
        g_lAccum        = 0.0f;
        g_lCurrentDelay = RandRange(g_legitPauseDelayMin, g_legitPauseDelayMax);
        g_lCurrentDur   = RandRange(g_legitPauseDurMin,   g_legitPauseDurMax);
        g_idleClockRunning = false;
        g_afkJumpDone      = false;
        g_recastAt      = std::chrono::steady_clock::now();

        std::cout << "[FishBot] Enabled. Lure=" << (int)g_activeLure
                  << " LastBait=(" << g_lastBaitPos.x << "," << g_lastBaitPos.y << ")\n";
    }

    inline void OnDisable() {
        SetButtons(false, false);
        if (g_off_Player_jumpButtonDown > 0 && g_LocalPlayer)
            *(bool*)((char*)g_LocalPlayer + g_off_Player_jumpButtonDown) = false;
        std::cout << "[FishBot] Disabled.\n";
    }

    inline void ResetTransient() {
        SetButtons(false, false);
        g_wasFishing       = false;
        g_catchPending     = false;
        g_strikePending    = false;
        g_needsRecast      = false;
        g_gaugeInstance    = nullptr;
        g_pendingBaitTile  = { -1, -1 };
        g_lastFishingState = 0;
        g_sawMinigameThisCast = false;
        g_returning        = false;
        g_idleClockRunning = false;
    }

    inline void WalkToward(Vector2i from, Vector2i to) {
        bool left  = to.x < from.x;
        bool right = to.x > from.x;
        PathFinder_WriteInputs(left, right, false);
    }

    __declspec(noinline) inline void RememberCurrentSpot(Vector2i tile) {
        RememberSpot(GetCurrentWorldName(), tile);
    }
    __declspec(noinline) inline bool HandleWorldChangeEdge() {
        std::string wname = GetCurrentWorldName();
        if (wname == g_lastWorldName) return false;
        bool hadPrev = !g_lastWorldName.empty();
        g_lastWorldName = wname;
        if (hadPrev) ResetTransient();
        Vector2i cached = SpotForWorld(wname);
        g_lastBaitPos = (cached.x != -1) ? cached : Vector2i{ -1, -1 };
        if (g_autoRecast) {
            g_needsRecast = true;
            g_recastAt = std::chrono::steady_clock::now() + std::chrono::seconds(1);
        }
        return hadPrev;
    }

    inline void Tick(float deltaTime) {
        if (!g_enabled || !g_LocalPlayer || !g_IsInWorld) return;

        if (g_antiSleep) SetAntiSleepFields();

        if (HandleWorldChangeEdge()) return;

        const int  fishState = GetFishingState();
        const bool isFishing = (fishState != 0);
        const auto now       = std::chrono::steady_clock::now();

        if (fishState != g_lastFishingState) {
            std::cout << "[FishBot/state] " << g_lastFishingState << " → "
                      << fishState << "\n";

            if (g_lastFishingState == 0 && fishState != 0 &&
                g_pendingBaitTile.x != -1) {
                g_lastBaitPos     = g_pendingBaitTile;
                g_pendingBaitTile = { -1, -1 };
                RememberCurrentSpot(g_lastBaitPos);
                std::cout << "[FishBot/cast] accepted — bait at ("
                          << g_lastBaitPos.x << "," << g_lastBaitPos.y << ")\n";
            }

            if (fishState == 3) g_sawMinigameThisCast = true;

            if (g_lastFishingState != 0 && fishState == 0) {
                if (!g_sawMinigameThisCast && !g_catchPending) {
                    g_stats_skipped++;
                    std::cout << "[FishBot/miss] bait stolen — no minigame fired. "
                                 "Total skipped: " << g_stats_skipped << "\n";
                }
            }
            g_lastFishingState = fishState;
        }

        if (g_pendingBaitTile.x != -1 && fishState == 0 &&
            now > g_pendingBaitDeadline) {
            std::cout << "[FishBot/cast] rejected (no state edge in 2s) — "
                         "clearing lure + tile, re-scan inventory\n";
            g_activeLure      = World::BlockType::None;
            g_pendingBaitTile = { -1, -1 };

            g_lastBaitPos     = { -1, -1 };
            if (g_autoRecast) {
                g_needsRecast = true;
                g_recastAt    = now + std::chrono::seconds(1);
            }
        }

        if (isFishing && !g_wasFishing) {

            g_wasFishing          = true;
            g_catchPending        = false;
            g_strikePending       = false;
            g_lPaused             = false;
            g_lAccum              = 0.0f;
            g_idleClockRunning    = false;

            g_lastFishTargetPoint = -999.0f;
            g_minigameStopUntil   = std::chrono::steady_clock::time_point{};
            g_aimInit             = false;
        } else if (!isFishing && g_wasFishing) {

            g_wasFishing = false;
            SetButtons(false, false);
            if (g_catchPending) {
                g_stats_caught++;
                g_catchPending = false;
                std::cout << "[FishBot] Catch! Total: " << g_stats_caught << "\n";
            }

            g_idleClockRunning = true;
            g_idleStart        = std::chrono::steady_clock::now();
            g_afkJumpDone      = false;

            if (g_autoRecast) {
                g_needsRecast = true;
                float delay = RandRange(g_recastDelayMinSec, g_recastDelayMaxSec);
                g_recastAt = std::chrono::steady_clock::now()
                    + std::chrono::milliseconds((int)(delay * 1000.0f));
            }
        }

        if (!isFishing) {

            if (g_returnToSpot && g_lastBaitPos.x != -1) {
                Vector2i hereTile;
                if (GetPlayerMapPoint(hereTile)) {
                    float ddx = float(hereTile.x - g_lastBaitPos.x);
                    float ddy = float(hereTile.y - g_lastBaitPos.y);
                    float drift = sqrtf(ddx*ddx + ddy*ddy);
                    if (g_returning) {
                        bool timedOut = std::chrono::duration_cast<std::chrono::seconds>(
                                            now - g_returnStart).count() >= 4;
                        if (drift <= 1.0f || timedOut) {
                            g_returning = false;
                            PathFinder_ClearInputs();
                        } else {
                            WalkToward(hereTile, g_lastBaitPos);
                            return;
                        }
                    } else if (drift > g_maxDriftTiles) {
                        g_returning   = true;
                        g_returnStart = now;
                        WalkToward(hereTile, g_lastBaitPos);
                        return;
                    }
                }
            }

            if (g_FishingResultsPopupInstance && oClosePopupFish) {
                __try { oClosePopupFish(g_FishingResultsPopupInstance); }
                __except (EXCEPTION_EXECUTE_HANDLER) {}
            }

            if (g_antiAfk && g_idleClockRunning && g_LocalPlayer) {
                float idle = std::chrono::duration<float>(
                    std::chrono::steady_clock::now() - g_idleStart).count();

                bool doJump = (idle >= 8.0f && idle < 8.3f);
                if (g_off_Player_jumpButtonDown > 0)
                    *(bool*)((char*)g_LocalPlayer + g_off_Player_jumpButtonDown) = doJump;

                if (idle >= 10.0f && !g_afkJumpDone) {
                    g_afkJumpDone = true;
                    if (g_off_Player_leftButton > 0)
                        *(bool*)((char*)g_LocalPlayer + g_off_Player_leftButton) = false;
                    if (g_autoRecast) {
                        g_needsRecast = true;
                        g_recastAt    = std::chrono::steady_clock::now();
                    }
                }
            }

            if (g_needsRecast && std::chrono::steady_clock::now() >= g_recastAt)
                CastLure();

        } else {

            if (oIsFishStrikeActive && !g_strikePending) {
                bool active = false;
                __try { active = oIsFishStrikeActive(g_LocalPlayer); }
                __except (EXCEPTION_EXECUTE_HANDLER) {}
                if (active) {
                    int delayMs = (int)RandRange(g_hookReactionMinMs, g_hookReactionMaxMs);
                    g_strikePending = true;
                    g_strikeAt = std::chrono::steady_clock::now()
                        + std::chrono::milliseconds(delayMs);
                    std::cout << "[FishBot] Strike! Hooking in " << delayMs << " ms.\n";
                }
            }
            if (g_strikePending && std::chrono::steady_clock::now() >= g_strikeAt) {
                g_strikePending = false;

                void* gui = GetGameplayUI();
                if (!gui) gui = g_cachedGameplayUI;

                if (gui && oOnButtonDownNew) {
                    __try { oOnButtonDownNew(gui, 4); }
                    __except (EXCEPTION_EXECUTE_HANDLER) {}
                    std::cout << "[FishBot] OnButtonDownNew(4) — gui="
                              << (g_cachedGameplayUI ? "cached" : "live") << "\n";
                } else {

                    std::cout << "[FishBot] WARNING: no GameplayUI instance (press hook manually once to populate cache).\n";
                }
            }
        }
    }

    inline void __fastcall hkOnButtonDownNew(void* instance, int buttonIndex) {
        if (instance) g_cachedGameplayUI = instance;
        if (oOnButtonDownNew) oOnButtonDownNew(instance, buttonIndex);
    }

    inline void __fastcall hkFishingGaugeMinigameUI_SetTargetAreaPosition(FishingGaugeMinigameUI* instance) {

        if (oFishingGaugeMinigameUI_SetTargetAreaPosition)
            oFishingGaugeMinigameUI_SetTargetAreaPosition(instance);

        if (!g_enabled || !instance) return;
        if (GetFishingState() != 3) return;

        const float fishPos = instance->fishPosition;
        const float tgtPos  = instance->targetAreaPosition;

        if (g_rejectCurrentFish) {
            float away = (fishPos < 0.5f) ? 1.0f : 0.0f;
            instance->targetAreaPosition = away;
            return;
        }
        float fishTgt = 0.5f;
        if (g_off_Gauge_fishTargetPoint > 0)
            fishTgt = *(float*)((char*)instance + g_off_Gauge_fishTargetPoint);

        float maxVel = 1.5f;
        if (g_off_Gauge_targetAreaMaxVelocity > 0)
            maxVel = *(float*)((char*)instance + g_off_Gauge_targetAreaMaxVelocity);

        const auto now = std::chrono::steady_clock::now();

        static std::chrono::steady_clock::time_point s_lastCall{};
        float dt = 0.016f;
        if (s_lastCall.time_since_epoch().count() != 0) {
            dt = std::chrono::duration<float>(now - s_lastCall).count();
            if (dt <= 0.f || dt > 0.1f) dt = 0.016f;
        }
        s_lastCall = now;

        if (!g_aimInit) { g_aimTarget = fishPos; g_aimInit = true; g_nextGlanceAt = now; }
        if (now >= g_nextGlanceAt) {
            float err = RandRange(-REEL_AIM_ERROR, REEL_AIM_ERROR);
            g_aimTarget = fishPos + err;
            if (g_aimTarget < 0.f) g_aimTarget = 0.f;
            if (g_aimTarget > 1.f) g_aimTarget = 1.f;
            int win = (int)RandRange((float)REEL_REACT_MIN_MS, (float)REEL_REACT_MAX_MS);
            g_nextGlanceAt = now + std::chrono::milliseconds(win);
        }

        float diff = g_aimTarget - tgtPos;
        const float DEAD = 0.004f;
        int dir = (diff < -DEAD) ? -1 : (diff > DEAD ? +1 : 0);
        if (dir != 0) {
            float step = maxVel * dt * (float)dir * g_speedMult;
            if (fabsf(step) > fabsf(diff)) step = diff;
            float newPos = tgtPos + step;
            if (newPos < 0.f) newPos = 0.f;
            if (newPos > 1.f) newPos = 1.f;
            instance->targetAreaPosition = newPos;
        }

        SetButtons(dir == -1, dir == +1);

        if (g_verboseMinigame) {
            std::cout << "[FishBot/mini] fp=" << fishPos
                      << " tp=" << tgtPos
                      << " new=" << instance->targetAreaPosition
                      << " ftgt=" << fishTgt
                      << " maxV=" << maxVel
                      << " dir=" << dir
                      << " aim=" << g_aimTarget
                      << " prog=" << instance->progress
                      << "\n";
        }
    }

    inline void __fastcall hkFishingGaugeMinigameUI_Update(FishingGaugeMinigameUI* instance) {
        if (oFishingGaugeMinigameUI_Update)
            oFishingGaugeMinigameUI_Update(instance);

        if (!g_enabled || !instance) return;
        g_gaugeInstance = instance;
        if (GetFishingState() == 0) {

            g_rejectCurrentFish   = false;
            g_currentFishGameSize = 0;
            g_lastFishBucket      = -1;
            return;
        }

        {

            float sz = instance->fishSizeMultiplier;
            if (sz > 0.f && sz < 100.f) g_lastFishSize = sz;

            int b = g_lastFishBucket;
            if (g_sizeFilterEnabled && !g_rejectCurrentFish &&
                b >= 0 && b < FSB_COUNT && !g_keepSize[b]) {
                g_rejectCurrentFish = true;
                SetButtons(false, false);
                bool leftNative = Seh_StopFishingMinigame();
                std::cout << "[FishBot/size] " << FishSizeName(b)
                          << " not selected — skipping minigame ("
                          << (leftNative ? "native stop" : "box-stop, fish escapes")
                          << "), will recast\n";
            }
        }

        if (g_autoLand && !g_catchPending && !g_rejectCurrentFish) {
            bool ready = false;
            if (g_off_Gauge_isReadyToLand > 0)
                ready = *(bool*)((char*)instance + g_off_Gauge_isReadyToLand);
            else
                ready = (instance->progress >= 1.0f);

            if (ready) {
                g_catchPending = true;
                SetButtons(false, false);
                std::cout << "[FishBot/land] isReadyToLand → LandButtonPressed\n";
                if (LandButtonPressed)
                    LandButtonPressed(instance);
            }
        }
    }
}

#include <queue>
#include <unordered_map>
#include <utility>

typedef void(__fastcall* tForceRefreshInventory)(void* invCtrl);
extern tForceRefreshInventory oForceRefreshInventory;
typedef void(__fastcall* tRefreshInventoryCache)(void* invCtrl);
extern tRefreshInventoryCache oRefreshInventoryCache;

namespace AutoFossil {

    inline bool  g_enabled        = false;
    inline float g_moveDelayMinMs = 160.0f;
    inline float g_moveDelayMaxMs = 420.0f;
    inline bool  g_verbose        = false;
    inline bool  g_repeat         = true;
    inline bool  g_useRawMoves    = true;
    inline int   g_targetCount    = 0;

    inline int       g_stats_solved = 0;
    inline int       g_stats_moves  = 0;
    inline int       g_stats_failed = 0;
    inline long long g_total_reward = 0;

    inline bool g_refreshPending = false;
    inline int  g_refreshCount   = 0;
    inline std::chrono::steady_clock::time_point g_refreshAt;

    typedef void(__fastcall* tSendStartArch)(PlayerData_InventoryKey key, void* mi);
    inline tSendStartArch oSendStartArch = nullptr;

    typedef void(__fastcall* tSendArchAction)(int cell, void* mi);
    inline tSendArchAction oSendArchAction = nullptr;

    typedef void(__fastcall* tSendStopMini)(int miniGameType, int actionResult, void* mi);
    inline tSendStopMini oSendStopMini = nullptr;

    typedef void(__fastcall* tUseConsumable)(void* inventoryControl, PlayerData_InventoryKey ik, void* mi);
    inline tUseConsumable oUseConsumable = nullptr;

    typedef void(__fastcall* tGameSlotClicked)(void* self, int item, void* mi);
    inline tGameSlotClicked oGameSlotClicked = nullptr;
    typedef void(__fastcall* tArchUpdate)(void* self, void* mi);
    inline tArchUpdate oArchUpdate = nullptr;
    inline void* g_cachedArchUI = nullptr;
    inline void* g_field_archGameIsRunning = nullptr;

    typedef bool(__cdecl* tIsBlockPred)(World::BlockType bt);
    inline tIsBlockPred IsBlockExcavatableFossil   = nullptr;
    inline tIsBlockPred IsBlockExcavatableMonolith = nullptr;

    constexpr int kMiniGameArchaeology = 1;

    constexpr int kStartTimeoutMs  = 8000;
    constexpr int kResultTimeoutMs = 8000;
    constexpr int kRetryStartMs    = 4000;
    constexpr int kNextDigGapMin   = 700;
    constexpr int kNextDigGapMax   = 1800;
    constexpr int kShuffleMinMs    = 1100;
    constexpr int kShuffleTimeoutMs = 12000;

    inline std::mutex g_mtx;

    enum class Phase { Idle, Starting, Shuffling, Solving, AwaitingWin };
    inline Phase g_phase = Phase::Idle;
    inline std::chrono::steady_clock::time_point g_boardAt;

    inline std::vector<int> g_moveQueue;
    inline size_t g_moveIdx     = 0;
    inline int  g_predBoard[9]  = { 0,1,2,3,4,5,6,7,8 };
    inline int  g_echoBoard[9]  = { 0,1,2,3,4,5,6,7,8 };
    inline bool g_haveEcho      = false;
    inline PlayerData_InventoryKey g_activeKey = { 0, 0 };

    inline std::chrono::steady_clock::time_point g_nextClickAt;
    inline std::chrono::steady_clock::time_point g_phaseDeadline;
    inline std::chrono::steady_clock::time_point g_nextStartAt;

    inline bool g_noFossilLogged = false;

    inline char g_diagId[12]    = "-";
    inline char g_diagRaw[64]   = "-";
    inline int  g_diagSolveLen  = -1;
    inline char g_diagStart[24] = "-";
    inline char g_diagVia[8]    = "-";

    inline float RandRange(float lo, float hi) {
        if (hi <= lo) return lo;
        return lo + (hi - lo) * (float)rand() / (float)RAND_MAX;
    }
    inline std::chrono::milliseconds ClickDelay() {
        return std::chrono::milliseconds((int)RandRange(g_moveDelayMinMs, g_moveDelayMaxMs));
    }

    inline bool MoreToDo() {
        if (g_targetCount > 0) return g_stats_solved < g_targetCount;
        if (g_repeat)          return true;
        return g_stats_solved < 1;
    }

    inline const char* PhaseLabel() {
        if (g_phase == Phase::Starting)    return "Starting";
        if (g_phase == Phase::Shuffling)   return "Shuffling";
        if (g_phase == Phase::Solving)     return "Solving";
        if (g_phase == Phase::AwaitingWin) return "Finishing";
        return MoreToDo() ? "Idle" : "Done";
    }
    inline bool IsBusy() { return g_phase != Phase::Idle; }

    inline void __fastcall hkArchUpdate(void* self, void* mi) {
        if (self) g_cachedArchUI = self;
        if (oArchUpdate) oArchUpdate(self, mi);
    }

    typedef void(__fastcall* tArchMsg)(void* bson, void* mi);
    inline tArchMsg oArchStartGame    = nullptr;
    inline tArchMsg oArchHandleAction = nullptr;
    inline tArchMsg oArchComplete     = nullptr;
    inline tArchMsg oArchStop         = nullptr;
    inline void __fastcall hkArchStartGame(void* bson, void* mi)    { if (!g_enabled && oArchStartGame)    oArchStartGame(bson, mi); }
    inline void __fastcall hkArchHandleAction(void* bson, void* mi) { if (!g_enabled && oArchHandleAction) oArchHandleAction(bson, mi); }
    inline void __fastcall hkArchComplete(void* bson, void* mi)     { if (!g_enabled && oArchComplete)     oArchComplete(bson, mi); }
    inline void __fastcall hkArchStop(void* bson, void* mi)         { if (!g_enabled && oArchStop)         oArchStop(bson, mi); }

    inline bool ArchGameIsRunning() {
        if (!g_field_archGameIsRunning || !il2cpp_field_static_get_value) return false;
        bool running = false;
        __try { il2cpp_field_static_get_value(g_field_archGameIsRunning, &running); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        return running;
    }

    inline bool ClickViaUI(int cell) {
        if (!g_cachedArchUI || !oGameSlotClicked) return false;
        __try { oGameSlotClicked(g_cachedArchUI, cell, nullptr); return true; }
        __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    }

    static inline int      IAbs(int x)             { return x < 0 ? -x : x; }
    static inline uint32_t PackKey(const int t[9]) {
        uint32_t k = 0, p = 1;
        for (int i = 0; i < 9; ++i) { k += (uint32_t)t[i] * p; p *= 9; }
        return k;
    }
    static inline void UnpackKey(uint32_t k, int t[9]) {
        for (int i = 0; i < 9; ++i) { t[i] = (int)(k % 9); k /= 9; }
    }
    static inline int Manhattan(const int t[9]) {
        int d = 0;
        for (int i = 0; i < 9; ++i) {
            int v = t[i];
            if (v == 8) continue;
            d += IAbs(i / 3 - v / 3) + IAbs(i % 3 - v % 3);
        }
        return d;
    }

    inline std::vector<int> SolvePuzzle(const int start[9]) {
        std::vector<int> out;
        int goal[9] = { 0,1,2,3,4,5,6,7,8 };
        uint32_t goalKey  = PackKey(goal);
        uint32_t startKey = PackKey(start);
        if (startKey == goalKey) return out;

        struct Node { uint32_t key; int g; int f; };
        struct Cmp  { bool operator()(const Node& a, const Node& b) const { return a.f > b.f; } };
        std::priority_queue<Node, std::vector<Node>, Cmp> open;
        std::unordered_map<uint32_t, int> gscore;
        std::unordered_map<uint32_t, std::pair<uint32_t, int>> came;

        gscore[startKey] = 0;
        open.push({ startKey, 0, Manhattan(start) });

        static const int dr[4] = { -1, 1, 0, 0 };
        static const int dc[4] = {  0, 0, -1, 1 };
        const int kMaxExpand = 600000;
        int  expanded = 0;
        bool found    = false;

        while (!open.empty() && expanded < kMaxExpand) {
            Node cur = open.top(); open.pop();
            auto gi = gscore.find(cur.key);
            if (gi == gscore.end() || cur.g > gi->second) continue;
            if (cur.key == goalKey) { found = true; break; }
            ++expanded;

            int t[9]; UnpackKey(cur.key, t);
            int e = 0; for (int i = 0; i < 9; ++i) if (t[i] == 8) { e = i; break; }
            int er = e / 3, ec = e % 3;

            for (int k = 0; k < 4; ++k) {
                int nr = er + dr[k], nc = ec + dc[k];
                if (nr < 0 || nr > 2 || nc < 0 || nc > 2) continue;
                int p = nr * 3 + nc;
                int nt[9]; for (int i = 0; i < 9; ++i) nt[i] = t[i];
                int tmp = nt[e]; nt[e] = nt[p]; nt[p] = tmp;
                uint32_t nk = PackKey(nt);
                int ng = cur.g + 1;
                auto it = gscore.find(nk);
                if (it == gscore.end() || ng < it->second) {
                    gscore[nk] = ng;
                    came[nk]   = { cur.key, p };
                    open.push({ nk, ng, ng + Manhattan(nt) });
                }
            }
        }
        if (!found) return out;

        for (uint32_t k = goalKey; k != startKey; ) {
            auto it = came.find(k);
            if (it == came.end()) { out.clear(); return out; }
            out.push_back(it->second.second);
            k = it->second.first;
        }
        std::reverse(out.begin(), out.end());
        return out;
    }

    inline std::vector<int> ReadIntArray(const nlohmann::json& j, const char* key) {
        std::vector<int> out;
        if (!j.contains(key)) return out;
        const nlohmann::json& a = j[key];
        if (a.is_array()) {
            for (auto& v : a)
                if (v.is_number_integer() || v.is_number_unsigned()) out.push_back(v.get<int>());
        } else if (a.is_object()) {
            std::vector<std::pair<int, int>> kv;
            for (auto it = a.begin(); it != a.end(); ++it)
                if (it->is_number_integer() || it->is_number_unsigned())
                    kv.push_back({ atoi(it.key().c_str()), it->get<int>() });
            std::sort(kv.begin(), kv.end(), [](auto& x, auto& y) { return x.first < y.first; });
            for (auto& p : kv) out.push_back(p.second);
        }
        return out;
    }

    inline bool DecodeWireBoard(const std::vector<int>& wire, int outBoard[9]) {
        if ((int)wire.size() < 9) return false;
        bool seen[9] = { false };
        for (int i = 0; i < 9; ++i) {
            int v = wire[i];
            if (v < 0 || v > 8 || seen[v]) return false;
            seen[v] = true;
            outBoard[i] = (v == 0) ? 8 : (v - 1);
        }
        return true;
    }

    inline bool IsSolvedInternal(const int b[9]) {
        for (int i = 0; i < 9; ++i) if (b[i] != i) return false;
        return true;
    }

    inline int PackIUbb(int blockType, int itemType) {
        return ((itemType & 0xFF) << 24) | (blockType & 0x00FFFFFF);
    }

    inline void _SehSendStartNative(PlayerData_InventoryKey key) {
        if (!oSendStartArch) return;
        __try { oSendStartArch(key, nullptr); } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline void _SehSendActionNative(int cell) {
        if (!oSendArchAction) return;
        __try { oSendArchAction(cell, nullptr); } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline void _SehSendStopNative() {
        if (!oSendStopMini) return;
        __try { oSendStopMini(kMiniGameArchaeology, 0, nullptr); } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline bool _SehUseConsumable(void* inv, PlayerData_InventoryKey key) {
        if (!inv || !oUseConsumable) return false;
        __try { oUseConsumable(inv, key, nullptr); return true; }
        __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    }

    inline void SendStart(PlayerData_InventoryKey key) {

        QueueRawBsonSend(nlohmann::json{ {"ID", "azeb"}, {"DYws", kMiniGameArchaeology},
                                         {"lYeW", PackIUbb(key.blockType, key.itemType)} });
        std::snprintf(g_diagStart, sizeof(g_diagStart), "raw azeb");
    }
    inline void SendClick(int cell) {

        if (g_useRawMoves)
            QueueRawBsonSend(nlohmann::json{ {"ID", "SITb"}, {"DYws", kMiniGameArchaeology}, {"KsvQ", cell} });
        else
            _SehSendActionNative(cell);
    }
    inline void SendGiveUp() {

        if (g_useRawMoves)
            QueueRawBsonSend(nlohmann::json{ {"ID", "cvDZ"}, {"DYws", kMiniGameArchaeology}, {"KsvQ", 0} });
        else
            _SehSendStopNative();
    }

    inline void Reset() {
        g_phase    = Phase::Idle;
        g_moveQueue.clear();
        g_moveIdx  = 0;
        g_haveEcho = false;
    }

    inline bool FindFossilKey(PlayerData_InventoryKey& out) {
        if (!g_LocalPlayer) return false;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd || !oGetInventoryAsOrderedByInventoryItemType || !oGetCountByKey) return false;
        if (!IsBlockExcavatableFossil && !IsBlockExcavatableMonolith) return false;

        Il2CppArray* arr = nullptr;
        __try { arr = oGetInventoryAsOrderedByInventoryItemType(pd); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        if (!arr || !arr->max_length) return false;

        auto* keys = (PlayerData_InventoryKey*)arr->items;
        for (size_t i = 0; i < arr->max_length; ++i) {
            PlayerData_InventoryKey k = keys[i];
            World::BlockType bt = (World::BlockType)k.blockType;
            bool ok = (IsBlockExcavatableFossil   && IsBlockExcavatableFossil(bt)) ||
                      (IsBlockExcavatableMonolith && IsBlockExcavatableMonolith(bt));
            if (!ok) continue;
            short qty = 0;
            __try { qty = oGetCountByKey(pd, k); } __except (EXCEPTION_EXECUTE_HANDLER) { continue; }
            if (qty > 0) { out = k; return true; }
        }
        return false;
    }

    inline bool IsLegalSlide(const int board[9], int cell) {
        if (cell < 0 || cell > 8) return false;
        int e = -1; for (int i = 0; i < 9; ++i) if (board[i] == 8) { e = i; break; }
        if (e < 0) return false;
        int er = e / 3, ec = e % 3, cr = cell / 3, cc = cell % 3;
        return (er == cr && IAbs(ec - cc) == 1) || (ec == cc && IAbs(er - cr) == 1);
    }

    inline void ApplyMove(int cell) {
        if (!IsLegalSlide(g_predBoard, cell)) return;
        int e = -1; for (int i = 0; i < 9; ++i) if (g_predBoard[i] == 8) { e = i; break; }
        int t = g_predBoard[e]; g_predBoard[e] = g_predBoard[cell]; g_predBoard[cell] = t;
    }

    inline void ProcessIncomingPacket(const json& j) {
        if (!g_enabled || !j.is_object()) return;

        if (j.contains("cDGm")) {
            std::string mid = j.value("ID", "?");
            std::snprintf(g_diagId, sizeof(g_diagId), "%s", mid.c_str());
            std::vector<int> raw = ReadIntArray(j, "aiaG");
            std::string rs;
            for (size_t i = 0; i < raw.size() && i < 12; ++i) { rs += std::to_string(raw[i]); if (i + 1 < raw.size()) rs += ","; }
            std::snprintf(g_diagRaw, sizeof(g_diagRaw), "%s", rs.empty() ? "(none)" : rs.c_str());
            if (g_verbose) {
                std::string d = j.dump(); if (d.size() > 400) d = d.substr(0, 400) + "...";
                std::cout << "[AutoFossil/raw] " << d << "\n";
            }
        }

        std::string id = j.value("ID", "");
        if (id != "IzAC" && id != "jNDP" && id != "WJJG" && id != "dmTH") return;
        int cdgm = j.value("cDGm", -1);

        std::lock_guard<std::mutex> lk(g_mtx);
        auto now = std::chrono::steady_clock::now();

        if (id == "IzAC") {

            std::vector<int> wire = ReadIntArray(j, "aiaG");
            int board[9];
            if (!DecodeWireBoard(wire, board)) {
                if (g_verbose) std::cout << "[AutoFossil] IzAC aiaG is not a 3x3 board — ignored.\n";
                return;
            }
            for (int i = 0; i < 9; ++i) g_predBoard[i] = board[i];
            g_moveQueue    = SolvePuzzle(board);
            g_moveIdx      = 0;
            g_diagSolveLen = (int)g_moveQueue.size();
            g_boardAt      = now;
            g_phase        = Phase::Solving;
            g_nextClickAt  = now + ClickDelay();
            if (g_verbose) {
                std::cout << "[AutoFossil] board ";
                for (size_t i = 0; i < 9 && i < wire.size(); ++i) std::cout << wire[i] << (i < 8 ? "," : "");
                std::cout << " -> " << g_moveQueue.size() << " moves\n";
            }
            return;
        }

        if (id == "jNDP") return;

        if (cdgm != kMiniGameArchaeology) return;

        if (id == "WJJG") {

            long long reward = (j.contains("eOPk") && j["eOPk"].is_number_integer())
                             ? j["eOPk"].get<long long>() : 0;
            g_stats_solved++;
            g_total_reward += reward;
            g_refreshPending = true;
            g_refreshCount   = 0;
            g_refreshAt      = now + std::chrono::milliseconds(400);
            Reset();
            g_nextStartAt  = now + std::chrono::milliseconds(
                (int)RandRange((float)kNextDigGapMin, (float)kNextDigGapMax));
            if (g_verbose)
                std::cout << "[AutoFossil] SOLVED! total=" << g_stats_solved
                          << " reward=" << reward
                          << (MoreToDo() ? "" : " — target reached, stopping") << "\n";
        } else if (id == "dmTH") {
            if (g_phase != Phase::Idle) g_stats_failed++;
            Reset();
            g_nextStartAt  = now + std::chrono::milliseconds(kRetryStartMs);
        }
    }

    inline void _SehRefreshInventory() {
        void* invCtrl = g_InventoryControl ? g_InventoryControl : Safe_GetInventoryControl_FromPlayer();
        if (invCtrl) g_InventoryControl = invCtrl;
        if (!invCtrl) return;
        if (oRefreshInventoryCache) {
            __try { oRefreshInventoryCache(invCtrl); }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
        if (oForceRefreshInventory) {
            __try { oForceRefreshInventory(invCtrl); }
            __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
    }

    inline void Tick(float ) {
        if (!g_enabled) return;
        if (!g_LocalPlayer || !g_IsInWorld) return;

        std::lock_guard<std::mutex> lk(g_mtx);
        auto now = std::chrono::steady_clock::now();

        if (g_refreshPending && now >= g_refreshAt) {
            _SehRefreshInventory();

            if (++g_refreshCount >= 3) { g_refreshPending = false; g_refreshCount = 0; }
            else g_refreshAt = now + std::chrono::milliseconds(600);
        }

        switch (g_phase) {
            case Phase::Idle: {
                if (!MoreToDo()) return;
                if (now < g_nextStartAt) return;
                PlayerData_InventoryKey key{ 0, 0 };
                if (!FindFossilKey(key)) {
                    if (!g_noFossilLogged) {
                        std::cout << "[AutoFossil] no diggable fossil item in inventory "
                                     "(stock fossils to dig).\n";
                        g_noFossilLogged = true;
                    }
                    g_nextStartAt = now + std::chrono::milliseconds(kRetryStartMs);
                    return;
                }
                g_noFossilLogged = false;
                g_activeKey      = key;
                SendStart(key);
                g_phase          = Phase::Starting;
                g_phaseDeadline  = now + std::chrono::milliseconds(kStartTimeoutMs);
                if (g_verbose)
                    std::cout << "[AutoFossil] starting dig (blockType=" << key.blockType
                              << ", itemType=" << key.itemType << ")\n";
                break;
            }
            case Phase::Starting: {
                if (now > g_phaseDeadline) {
                    g_stats_failed++;
                    if (g_verbose) std::cout << "[AutoFossil] start timed out (no board) — retrying.\n";
                    g_phase       = Phase::Idle;
                    g_nextStartAt = now + std::chrono::milliseconds(kRetryStartMs);
                }
                break;
            }
            case Phase::Solving: {

                if (now < g_nextClickAt) break;
                if (g_moveIdx < g_moveQueue.size()) {
                    int cell = g_moveQueue[g_moveIdx];
                    if (cell >= 0 && cell <= 8) { SendClick(cell); ApplyMove(cell); }
                    std::snprintf(g_diagVia, sizeof(g_diagVia), "raw");
                    g_moveIdx++;
                    g_stats_moves++;
                    if (g_verbose)
                        std::cout << "[AutoFossil] move " << g_moveIdx << "/" << g_moveQueue.size()
                                  << " click " << cell << "\n";
                    g_nextClickAt = now + ClickDelay();
                }
                if (g_moveIdx >= g_moveQueue.size()) {
                    g_phase         = Phase::AwaitingWin;
                    g_phaseDeadline = now + std::chrono::milliseconds(kResultTimeoutMs);
                    if (g_verbose) std::cout << "[AutoFossil] all moves sent — awaiting win\n";
                }
                break;
            }
            case Phase::AwaitingWin: {

                if (now > g_phaseDeadline) {
                    g_stats_failed++;
                    if (g_verbose) std::cout << "[AutoFossil] no win after moves — re-digging.\n";
                    g_phase       = Phase::Idle;
                    g_nextStartAt = now + std::chrono::milliseconds(kRetryStartMs);
                }
                break;
            }
        }
    }

    inline void OnEnable() {
        std::lock_guard<std::mutex> lk(g_mtx);
        g_stats_solved   = 0;
        g_stats_moves    = 0;
        g_stats_failed   = 0;
        g_total_reward   = 0;
        g_noFossilLogged = false;
        Reset();
        g_nextStartAt = std::chrono::steady_clock::now();
        std::cout << "[AutoFossil] Enabled — auto-digging fossils.\n";
    }

    inline void OnDisable() {
        std::lock_guard<std::mutex> lk(g_mtx);

        Reset();
        std::cout << "[AutoFossil] Disabled.\n";
    }
}

#if 0
    inline std::chrono::steady_clock::time_point g_stats_startTime;

    inline bool  strikeResponsePending = false;
    inline std::chrono::steady_clock::time_point strikeResponseTime;

    inline std::chrono::steady_clock::time_point landSentTime;
    static constexpr float kLandTimeoutSecs = 12.0f;

    inline float prevVelSign     = 0.0f;
    inline float g_missEscapePos = 0.2f;
    inline bool  inMinigame      = false;
    inline bool  prevOverlapping = false;
    inline std::chrono::steady_clock::time_point minigameStartTime;
    inline std::chrono::steady_clock::time_point zoneEntryTime;
    inline int   totalFrames     = 0;
    inline int   onZoneFrames    = 0;
    inline bool  landPacketSent  = false;

    inline WORD g_heldKey = 0;

    inline void SimKey(WORD vk, bool down) {
        if (vk < 256) g_virtualKeyState[vk] = down;
    }

    inline bool  inMissWindow = false;
    inline std::chrono::steady_clock::time_point nextMissTime;
    inline std::chrono::steady_clock::time_point missEndTime;

    inline void ScheduleNextMiss() {
        int delayMs = 2000 + rand() % 1001;
        nextMissTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
    }

    inline void AddToLureQueue(World::BlockType lure);
    inline bool TryNextLure();

    inline int64_t GetDateTimeTicks() {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::system_clock::now().time_since_epoch()).count();
        return us * 10 + 621355968000000000LL;
    }

    inline void SendFishPacket(const json& j) {
        if (!oSimpleBSON_Load || !oAddOneMessageToList || !il2cpp_array_new || !byteClass) return;
        try {
            auto bson = json::to_bson(j);
            auto* arr = il2cpp_array_new(byteClass, bson.size());
            if (!arr) return;
            memcpy(arr->items, bson.data(), bson.size());
            void* pkt = oSimpleBSON_Load(arr);
            if (!pkt) return;
            oAddOneMessageToList(pkt);
        } catch (...) {}
    }

    inline void ProcessIncomingPacket(const json& j) {
        if (!g_enabled) return;
        std::string id = j.value("ID", "");

        static const std::unordered_set<std::string> kFishPktIds = {
            "MGA","MGC","MGD","FiOnAM","FiOffAM","TrTFFMP","MGSt"
        };
        if (kFishPktIds.count(id)) {
            std::cout << "[FishBot][PKT] " << j.dump() << "\n";
        }

        if (id == "MGA") {
            int mgd = j.value("MGD", 0);
            if (mgd == 2) {

                isWaitingForStrike = false;
                int delayMs = 160 + rand() % 441;
                strikeResponsePending = true;
                strikeResponseTime = std::chrono::steady_clock::now()
                                   + std::chrono::milliseconds(delayMs);
                std::cout << "[FishBot] Strike! Responding in " << delayMs << " ms.\n";
            } else if (mgd == 3) {
                strikeResponsePending = false;
                inMinigame        = true;
                prevOverlapping   = false;
                totalFrames       = 0;
                onZoneFrames      = 0;
                landPacketSent    = false;
                inMissWindow      = false;
                prevVelSign       = 0.0f;
                minigameStartTime = std::chrono::steady_clock::now();
                ScheduleNextMiss();
                std::cout << "[FishBot] Fish on! Minigame started.\n";
            } else if (mgd == 5) {
                std::cout << "[FishBot] Missed strike (MGD=5). Waiting for next bite...\n";
            }
        } else if (id == "MGC") {
            inMinigame = false;
            g_stats_fishesCaught++;
            std::cout << "[FishBot] Caught! Total: " << g_stats_fishesCaught << "\n";
            if (g_autoRecast) {
                needsToRecast    = true;
                recastReadyTime  = std::chrono::steady_clock::now()
                                 + std::chrono::milliseconds((int)(g_recastDelaySeconds * 1000));
            }
        } else if (id == "TrTFFMP") {
            bool success = j.value("S", true);
            if (success) {
                int bx = j.value("x", -1);
                int by = j.value("y", -1);
                if (bx != -1 && by != -1) {
                    g_lastBaitPosition = { bx, by };
                    std::cout << "[FishBot] Bait position locked: (" << bx << ", " << by << ")\n";
                }
                isWaitingForStrike = true;
                needsToRecast      = false;
                lureCastTime       = std::chrono::steady_clock::now();
            } else {
                bool explicitFail = j.contains("S") && !j.value("S", true);
                if (explicitFail) {
                    isWaitingForStrike = false;
                    std::cout << "[FishBot] Cast rejected by server (S=false) — lure may be exhausted.\n";
                    if (TryNextLure()) {
                        needsToRecast   = true;
                        recastReadyTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
                    } else {
                        std::cout << "[FishBot] No more lures in queue — stopping.\n";
                        g_enabled = false;
                    }
                }
            }
        }
    }

    inline void AddToLureQueue(World::BlockType lure) {
        if (std::find(g_lureQueue.begin(), g_lureQueue.end(), lure) == g_lureQueue.end()) {
            g_lureQueue.push_back(lure);
            std::cout << "[FishBot] Lure queue: added type " << (int)lure
                      << " (" << g_lureQueue.size() << " total)\n";
        }
        g_activeLureType = lure;
        auto it = std::find(g_lureQueue.begin(), g_lureQueue.end(), lure);
        if (it != g_lureQueue.end())
            g_lureQueueIndex = (int)(it - g_lureQueue.begin());
    }

    inline bool TryNextLure() {
        if (g_lureQueue.size() <= 1) {
            std::cout << "[FishBot] TryNextLure: no fallback — only one lure type in queue.\n";
            return false;
        }
        g_lureQueueIndex = (g_lureQueueIndex + 1) % (int)g_lureQueue.size();
        g_activeLureType = g_lureQueue[g_lureQueueIndex];
        std::cout << "[FishBot] Switching to lure " << (int)g_activeLureType
                  << " (slot " << g_lureQueueIndex + 1 << "/" << g_lureQueue.size() << ")\n";
        return true;
    }

    inline void ScanInventoryForLures();

    bool CastLure();

    inline bool Start() {
        if (!g_IsInWorld || !g_LocalPlayer) {
            std::cout << "[FishBot] Error: Must be in a world to start.\n";
            g_enabled = false;
            return false;
        }
        g_stats_fishesCaught = 0;
        g_stats_startTime = std::chrono::steady_clock::now();
        needsToRecast = false;
        isWaitingForStrike = false;
        strikeResponsePending = false;
        inMinigame = false;
        landPacketSent = false;
        g_lastBaitPosition = { -1, -1 };

        std::cout << "[FishBot] Started. Lure queue has " << g_lureQueue.size() << " type(s).\n";
        return true;
    }

    inline void Stop(const std::string& reason = "User action") {
        std::cout << "[FishBot] Stopped. Reason: " << reason << std::endl;
        needsToRecast = false;
        isWaitingForStrike = false;
        strikeResponsePending = false;
        inMinigame = false;
        landPacketSent = false;
        StopFishMovement = false;
        if (g_heldKey) { SimKey(g_heldKey, false); g_heldKey = 0; }
    }

    inline bool CastLure() {
        needsToRecast = false;
        isWaitingForStrike = false;

        if (!g_LocalPlayer)    { std::cout << "[FishBot] CastLure: g_LocalPlayer NULL\n";    return false; }
        if (!g_WorldController){ std::cout << "[FishBot] CastLure: g_WorldController NULL\n"; return false; }
        if (!IsFishingLure)    { std::cout << "[FishBot] CastLure: IsFishingLure NULL\n";    return false; }
        if (!oSetBaitWithTool) { std::cout << "[FishBot] CastLure: SetBaitWithTool NULL\n";  return false; }

        if (g_activeLureType == World::BlockType::None) {
            if (!g_lureQueue.empty()) {
                g_lureQueueIndex = 0;
                g_activeLureType = g_lureQueue[0];
                std::cout << "[FishBot] CastLure: using queued lure type " << (int)g_activeLureType << "\n";
            } else {
                std::cout << "[FishBot] CastLure: no lure type known — cast your lure manually once to start.\n";
                needsToRecast   = true;
                recastReadyTime = std::chrono::steady_clock::now() + std::chrono::seconds(2);
                return false;
            }
        }

        Vector2i baitTargetPoint = g_lastBaitPosition;

        if (baitTargetPoint.x == -1) {

            Vector2i playerMapPoint;
            if (!GetPlayerMapPoint(playerMapPoint)) {
                std::cout << "[FishBot] CastLure: GetPlayerMapPoint failed\n";
                return false;
            }
            for (const auto& point : std::vector<Vector2i>{
                {playerMapPoint.x + 1, playerMapPoint.y - 1}, {playerMapPoint.x - 1, playerMapPoint.y - 1},
                {playerMapPoint.x,     playerMapPoint.y - 1}, {playerMapPoint.x + 1, playerMapPoint.y},
                {playerMapPoint.x - 1, playerMapPoint.y}
                }) {
                World::BlockType block = GetBlockTypeFromPoint(point);
                if (std::find(g_manualWaterIDs.begin(), g_manualWaterIDs.end(), static_cast<int>(block)) != g_manualWaterIDs.end()) {
                    baitTargetPoint = point;
                    break;
                }
            }
        }

        if (baitTargetPoint.x == -1) {

            std::cout << "[FishBot] CastLure: no bait position known yet — cast your lure manually once to start.\n";
            needsToRecast   = true;
            recastReadyTime = std::chrono::steady_clock::now() + std::chrono::seconds(2);
            return false;
        }

        oSetBaitWithTool(g_WorldController, g_activeLureType, baitTargetPoint, 0.0f, nullptr);
        std::cout << "[FishBot] Lure cast (type=" << (int)g_activeLureType << "). Waiting for a strike...\n";
        return true;
    }

    inline void __fastcall hkFishingGaugeMinigameUI_Update(FishingGaugeMinigameUI* instance) {

        if (g_enabled && instance && inMinigame) {
            if (g_fishBotMode == 0 && !inMissWindow) {

                instance->targetAreaPosition = instance->fishPosition + (instance->fishVelocity * 0.016f);
            } else if (g_fishBotMode == 1) {

                instance->fishPosition       = inMissWindow ? g_missEscapePos : 0.5f;
                instance->targetAreaPosition = 0.5f;
            }
        }

        if (oFishingGaugeMinigameUI_Update) {
            oFishingGaugeMinigameUI_Update(instance);
        }

        if (!g_enabled || !instance) return;

        if (!inMinigame) return;

        isWaitingForStrike = false;
        auto now = std::chrono::steady_clock::now();
        float elapsedSec = std::chrono::duration<float>(now - minigameStartTime).count();

        static float lastScanTime = -3.0f;
        if (elapsedSec - lastScanTime >= 2.0f) {
            lastScanTime = elapsedSec;
            float unk148 = *(float*)((char*)instance + 0x148);
            float unk14C = *(float*)((char*)instance + 0x14C);
            float unk150 = *(float*)((char*)instance + 0x150);
            float unk15C = *(float*)((char*)instance + 0x15C);
            float unk160 = *(float*)((char*)instance + 0x160);
            float unk164 = *(float*)((char*)instance + 0x164);
            std::cout << "[FishBot][DBG] t=" << elapsedSec
                      << "s prog=" << instance->progress
                      << " fish=" << instance->fishPosition
                      << " vel=" << instance->fishVelocity
                      << " bar=" << instance->targetAreaPosition
                      << "\n  [gap] 148=" << unk148 << " 14C=" << unk14C << " 150=" << unk150
                      << "\n  [after] 15C=" << unk15C << " 160=" << unk160 << " 164=" << unk164 << "\n";
        }

        totalFrames++;

        if (g_fishBotMode == 0) {

            StopFishMovement = false;

            float curVelSign = (instance->fishVelocity >= 0.0f) ? 1.0f : -1.0f;
            if (prevVelSign != 0.0f && curVelSign != prevVelSign && !inMissWindow) {
                inMissWindow = true;
                int reactionMs = 80 + rand() % 270;
                missEndTime = now + std::chrono::milliseconds(reactionMs);
            }
            prevVelSign = curVelSign;
            if (inMissWindow && now >= missEndTime) inMissWindow = false;

            if (!inMissWindow) {
                instance->targetAreaPosition = instance->fishPosition + (instance->fishVelocity * 0.016f);
            }

        } else if (g_fishBotMode == 1) {

            StopFishMovement = true;

            if (!inMissWindow && now >= nextMissTime) {
                inMissWindow = true;
                int missMs = 300 + rand() % 101;
                missEndTime = now + std::chrono::milliseconds(missMs);

                bool goRight = (rand() % 2 == 0);
                float offset = 0.25f + (rand() % 200) * 0.001f;
                g_missEscapePos = std::clamp(goRight ? 0.5f + offset : 0.5f - offset, 0.05f, 0.95f);
                std::cout << "[FishBot][Freeze] Miss! fish→" << g_missEscapePos << " for " << missMs << " ms\n";
            }
            if (inMissWindow && now >= missEndTime) {
                inMissWindow = false;
                ScheduleNextMiss();
                std::cout << "[FishBot][Freeze] Back to center.\n";
            }

            instance->fishPosition       = inMissWindow ? g_missEscapePos : 0.5f;
            instance->targetAreaPosition = 0.5f;

        } else if (g_fishBotMode == 2) {

            float diff = instance->fishPosition - instance->targetAreaPosition;
            WORD wantKey = (diff > 0.10f) ? 'D' : (diff < -0.10f) ? 'A' : 0;
            if (wantKey != g_heldKey) {
                if (g_heldKey) SimKey(g_heldKey, false);
                if (wantKey)  SimKey(wantKey,  true);
                g_heldKey = wantKey;
            }
        }

        const float kBarHalfWidth = 0.15f;
        bool curOverlapping = (std::abs(instance->fishPosition - instance->targetAreaPosition) < kBarHalfWidth);
        if (curOverlapping && !prevOverlapping) {
            zoneEntryTime = now;

            Seh_SendFishOnArea();
        } else if (!curOverlapping && prevOverlapping) {
            float displacement = std::abs(instance->fishPosition - instance->targetAreaPosition);

            Seh_SendFishOffArea(displacement);
        }
        prevOverlapping = curOverlapping;
        if (curOverlapping) onZoneFrames++;

        if (g_autoLand && instance->progress >= 1.0f && !landPacketSent && elapsedSec >= 3.0f) {
            landPacketSent = true;
            landSentTime   = now;
            std::cout << "[FishBot][Land] Auto-land! prog=" << instance->progress
                      << " frames=" << totalFrames << " onZone=" << onZoneFrames
                      << " LandButtonPressed=" << (LandButtonPressed ? "valid" : "NULL") << "\n";

            if (g_heldKey) { SimKey(g_heldKey, false); g_heldKey = 0; }

            SendFishPacket({
                {"ID","MGA"}, {"LS",1}, {"MGD",GetDateTimeTicks()}, {"MGT",2},
                {"Amt",-0.5}, {"Idx",totalFrames}, {"vI",onZoneFrames}
            });
            if (g_fishBotMode == 2) {

                SimKey(VK_SPACE, true);
                SimKey(VK_SPACE, false);
            } else {
                if (LandButtonPressed) {
                    LandButtonPressed(instance);
                } else {
                    std::cout << "[FishBot][Land] WARNING: LandButtonPressed is NULL — catch popup may not appear. Waiting for MGC from server.\n";
                }
            }
        }

        if (landPacketSent && inMinigame) {
            float waitedSec = std::chrono::duration<float>(now - landSentTime).count();
            if (waitedSec > kLandTimeoutSecs) {
                std::cout << "[FishBot][Land] Timeout waiting for MGC after " << waitedSec
                          << "s — force-resetting state and recasting.\n";
                inMinigame      = false;
                landPacketSent  = false;
                needsToRecast   = true;
                recastReadyTime = now + std::chrono::milliseconds(500);
            }
        }
    }
}
#endif

inline void* __fastcall hkFishingResultsPopup_DoShowAnimation(void* instance, bool doInstant) {
    if (FishBot::g_enabled) {
        g_FishingResultsPopupInstance = instance;
    }
    return oFishingResultsPopup_DoShowAnimation(instance, doInstant);
}

inline void* __fastcall hkFishingResultsPopup_DoHideAnimation(void* instance, bool doInstant) {
    if (g_FishingResultsPopupInstance == instance) {
        g_FishingResultsPopupInstance = nullptr;
    }
    return oFishingResultsPopup_DoHideAnimation(instance, doInstant);
}

inline bool __fastcall hkSetBaitWithTool(void* worldController, World::BlockType lure, Vector2i mapPoint, float timeToWait, void* methodInfo) {
    if (lure != World::BlockType::None && (!IsFishingLure || IsFishingLure(lure))) {
        FishBot::g_activeLure  = lure;

        if (FishBot::g_lastBaitPos.x == -1)
            FishBot::g_lastBaitPos = mapPoint;
        std::cout << "[FishBot] Manual cast captured: lure=" << (int)lure
                  << " tile=(" << mapPoint.x << "," << mapPoint.y << ")\n";
    }
    return oSetBaitWithTool(worldController, lure, mapPoint, timeToWait, nullptr);
}

typedef void(__fastcall* tSendSetBlockMessage)(Vector2i mapPoint, World::BlockType blockType, int, int);

typedef void(__fastcall* tSendHitBlockMessage)(Vector2i mapPoint, int64_t hitTimeTicks, bool doNotDestroy);

typedef void(__fastcall* tMiningPickaxeRepairing)(PlayerData_InventoryKey ik);
typedef int(__cdecl* tGetHitsRequired)(World::BlockType blockType);

typedef void(__fastcall* tRemoveItemsFromInventory)(void* playerData, PlayerData_InventoryKey key, int amount);
typedef void* (__fastcall* tGetPlayerData)(void* player);

typedef void(__fastcall* tAddItemToInventory)(void* playerData, int blockType, int itemType, short amount, void* inventoryData);

typedef void(__fastcall* tForceRefreshInventory)(void* invCtrl);
typedef void(__fastcall* tRefreshInventoryCache)(void* invCtrl);

typedef void* (__fastcall* tSpawnDataClassForEnum)(int blockType);

typedef void(__fastcall* tSetWornWeaponDurability)(void* playerData, int durability);

inline tRemoveItemsFromInventory oRemoveItemsFromInventory = nullptr;
inline tAddItemToInventory       oAddItemToInventory       = nullptr;
inline tForceRefreshInventory    oForceRefreshInventory    = nullptr;
inline tRefreshInventoryCache    oRefreshInventoryCache    = nullptr;
inline tSpawnDataClassForEnum    oSpawnDataClassForEnum    = nullptr;
inline tSetWornWeaponDurability  oSetWornWeaponDurability  = nullptr;
inline tGetPlayerData oGetPlayerData = nullptr;
inline tSendSetBlockMessage oSendSetBlockMessage = nullptr;
inline tSendHitBlockMessage oSendHitBlockMessage = nullptr;
inline tMiningPickaxeRepairing oMiningPickaxeRepairing = nullptr;
inline tGetHitsRequired oGetHitsRequired = nullptr;

void* Safe_GetInventoryControl_FromPlayer()
{

    void* gui = GetGameplayUI();
    if (!gui) return nullptr;
    constexpr uintptr_t GameplayUI_inventoryControl_Offset = 0x110;
    void* p = nullptr;
    __try {
        p = *(void**)((uintptr_t)gui + GameplayUI_inventoryControl_Offset);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    return p;
}

namespace AutoMine { namespace AutoLoop {
    inline void Tick();
    inline void StampPlayerUpdateHeartbeat();
} }

static void* PathProbe_SafeReadQword(void* base, uintptr_t off) {
    if (!base) return nullptr;
    void* r = nullptr;
    __try { r = *(void**)((uint8_t*)base + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
    return r;
}

static bool PathProbe_SehWarmSetVelocity(void* player) {
    if (!oSetVelocity || !player) return false;
    Vector3 zero{0.f, 0.f, 0.f};
    __try { oSetVelocity(player, zero); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

inline std::atomic<bool> g_setVelocityWarmed{false};

inline std::atomic<float> g_pfPendingVelX{0.f};
inline std::atomic<float> g_pfPendingVelY{0.f};
inline std::atomic<float> g_pfPendingVelZ{0.f};
inline std::atomic<bool>  g_pfPendingVelValid{false};
inline std::atomic<int>   g_pfAppliedCount{0};
inline std::atomic<int>   g_pfApplyFaultCount{0};

static bool PathProbe_SehCallSetVelocity(void* player, float x, float y, float z) {
    if (!oSetVelocity || !player) return false;
    Vector3 v{x, y, z};
    __try { oSetVelocity(player, v); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

inline void __fastcall hkPlayerUpdate(void* instance) {
    if (oPlayerUpdate) oPlayerUpdate(instance);

    {
        static bool s_velWarmTried = false;
        if (!s_velWarmTried && instance && oSetVelocity) {
            s_velWarmTried = true;
            bool ok = PathProbe_SehWarmSetVelocity(instance);
            g_setVelocityWarmed.store(ok, std::memory_order_release);
            std::cout << "[Warmup] Player::SetVelocity game-thread call "
                      << (ok ? "OK — pathfinder will route through here."
                             : "FAULTED — function broken even on game thread.")
                      << std::endl;
        }
    }

    if (instance == g_LocalPlayer && g_pfPendingVelValid.load(std::memory_order_acquire)) {
        float vx = g_pfPendingVelX.load(std::memory_order_relaxed);
        float vy = g_pfPendingVelY.load(std::memory_order_relaxed);
        float vz = g_pfPendingVelZ.load(std::memory_order_relaxed);
        g_pfPendingVelValid.store(false, std::memory_order_release);
        bool ok = PathProbe_SehCallSetVelocity(instance, vx, vy, vz);
        if (ok) g_pfAppliedCount.fetch_add(1, std::memory_order_relaxed);
        else    g_pfApplyFaultCount.fetch_add(1, std::memory_order_relaxed);

        int total = g_pfAppliedCount.load(std::memory_order_relaxed);
        if (total > 0 && total % 300 == 0) {
            std::cout << "[PFApply] applied=" << total
                      << " faults=" << g_pfApplyFaultCount.load(std::memory_order_relaxed)
                      << " last={" << vx << "," << vy << "," << vz << "}"
                      << std::endl;
        }
    }

    if (g_isTransitioning.load(std::memory_order_acquire)) return;

    {
        std::lock_guard<std::mutex> lock(g_PlayerInstancesMutex);
        if (std::find(g_PlayerInstances.begin(), g_PlayerInstances.end(), instance) == g_PlayerInstances.end())
            g_PlayerInstances.push_back(instance);
    }

    Vector3 probePos{};
    void*   probeTr = nullptr;
    bool    didCache = false;
    if (instance && oGetTransform && oGetPosition) {
        probeTr = oGetTransform(instance);
        if (probeTr) {
            probePos = oGetPosition(probeTr);
            {
                std::lock_guard<std::mutex> lk(g_PlayerInstancesMutex);
                g_playerPosCache[instance] = probePos;
            }
            didCache = true;
        }
    }

    if (didCache) {
        static int  s_probeTicks  = 0;
        static bool s_probeDumped = false;
        s_probeTicks++;
        if (!s_probeDumped && s_probeTicks >= 120) {
            s_probeDumped = true;
            void* instKlass  = PathProbe_SafeReadQword(instance, 0);
            void* instCached = PathProbe_SafeReadQword(instance, 0x10);
            void* trKlass    = PathProbe_SafeReadQword(probeTr, 0);
            void* trCached   = PathProbe_SafeReadQword(probeTr, 0x10);

            std::cout << std::hex
                      << "[Probe] instance=0x" << instance
                      << " instKlass=0x" << instKlass
                      << " instNativeCache(+0x10)=0x" << instCached << std::dec
                      << std::endl;
            std::cout << std::hex
                      << "[Probe] transform=0x" << probeTr
                      << " trKlass=0x" << trKlass
                      << " trNativeCache(+0x10)=0x" << trCached << std::dec
                      << std::endl;
            std::cout << "[Probe] oGetPosition returned {"
                      << probePos.x << ", " << probePos.y << ", " << probePos.z << "}" << std::endl;

            auto bad = [](float f){ return f != f || f > 1e7f || f < -1e7f; };
            bool posBad   = bad(probePos.x) || bad(probePos.y) || bad(probePos.z);
            bool ptrsBad  = (instKlass == nullptr) || (trKlass == nullptr);
            if (posBad) {
                std::cout << "[Probe] !! Vector3 garbage. oGetPosition manifest RVA wrong OR "
                          << "Unity 6 return-by-value ABI mismatch. Try _Injected variant." << std::endl;
            } else if (ptrsBad) {
                std::cout << "[Probe] !! klass null. Instance or Transform has no klass header "
                          << "— hook target RVA might be wrong (not a Player.Update body)." << std::endl;
            } else {
                std::cout << "[Probe] OK. Position looks sane, both klass fields populated. "
                          << "Pathfinder/ESP breakage is something else." << std::endl;
            }
        }
    }

    if (!g_LocalPlayer) {
        std::lock_guard<std::mutex> unityLock(g_UnityObjectMutex);
        if (!g_LocalPlayer) g_LocalPlayer = instance;
    }

    if (instance == g_LocalPlayer) {

        if (!g_InventoryControl) {
            g_InventoryControl = Safe_GetInventoryControl_FromPlayer();
        }

        static auto last_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = current_time - last_time;
        last_time = current_time;
        float deltaTime = delta.count();

        if (g_LocalPlayer) {
            uintptr_t p = (uintptr_t)g_LocalPlayer;
            float* pCons   = (float*)(p + g_off_Player_rocketFuelConsumption);
            float* pFuel   = (float*)(p + g_off_Player_rocketFuel);
            float* pCons60 = (float*)(p + g_off_Player_rocketFuelConsumption60);
            float* pFull   = (float*)(p + g_off_Player_fullRocketFuel);
            if (!IsBadWritePtr(pCons, sizeof(float)) &&
                !IsBadWritePtr(pCons60, sizeof(float))) {
                if (InfiniteJetpack) {
                    *pCons   = 0.0f;
                    *pCons60 = 0.0f;
                    if (!IsBadReadPtr(pFull, sizeof(float)) &&
                        !IsBadWritePtr(pFuel, sizeof(float)) &&
                        *pFull > 0.0001f) {
                        *pFuel = *pFull;
                    }
                } else {

                    if (*pCons   < 0.0001f) *pCons   = g_origRocketFuelConsumptionSpeed;
                    if (*pCons60 < 0.0001f) *pCons60 = g_origRocketFuelConsumptionSpeed60FPS;
                }
            }
        }

        static bool wasFishBotEnabled = false;
        if (FishBot::g_enabled && !wasFishBotEnabled)
            FishBot::OnEnable();
        else if (!FishBot::g_enabled && wasFishBotEnabled)
            FishBot::OnDisable();
        wasFishBotEnabled = FishBot::g_enabled;

        static bool wasFossilEnabled = false;
        if (AutoFossil::g_enabled && !wasFossilEnabled)
            AutoFossil::OnEnable();
        else if (!AutoFossil::g_enabled && wasFossilEnabled)
            AutoFossil::OnDisable();
        wasFossilEnabled = AutoFossil::g_enabled;

        AIAimBotLogic(deltaTime);
        AutoCollectLogic();
        AutoGiftBoxLogic();
        AntiPoisonTick();
        GodModeStateTick();
        RainbowNameLogic(deltaTime);
        StreamerModeLogic(deltaTime);

        UpdateCachedWorldName();

        AutoMine::AutoLoop::Tick();

        Store::TickFromHook();

        AutoMine::AutoLoop::StampPlayerUpdateHeartbeat();

        *(bool*)((uintptr_t)g_LocalPlayer + g_off_Player_isSubAquaticPerkActive) = unlimitedOxygenEnabled;
        if (EnableJumpModeOverride) *(int*)((uintptr_t)g_LocalPlayer + g_off_PlayerJumpMode) = SelectedJumpMode;
    }
}

inline void __fastcall hkPlayerOnDestroy(void* instance) {

    {
        std::lock_guard<std::mutex> unityLock(g_UnityObjectMutex);
        if (g_LocalPlayer == instance) g_LocalPlayer = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(g_PlayerInstancesMutex);
        auto it = std::find(g_PlayerInstances.begin(), g_PlayerInstances.end(), instance);
        if (it != g_PlayerInstances.end()) g_PlayerInstances.erase(it);
    }
    if (oPlayerOnDestroy) oPlayerOnDestroy(instance);
}

using t_il2cpp_class_get_method_from_name = const void* (*)(void*, const char*, int);
inline t_il2cpp_class_get_method_from_name p_il2cpp_class_get_method_from_name = nullptr;

inline void* FindClassDynamic(const char* className, const char*  = "") {
    if (!className) return nullptr;
    void* k = Manifest::GetClassPtr(className);
    if (!k) {
        static std::unordered_set<std::string> warned;
        if (warned.insert(className).second) {

            const Manifest::ClassEntry* e = Manifest::GetClassEntry(className);
            if (e) {
                std::cout << "[FindClassDynamic] " << className
                          << " whitelisted but klass resolution failed "
                          << "(image walk broken or class_from_name RVA bad)" << std::endl;
            } else {
                std::cout << "[FindClassDynamic] " << className
                          << " not whitelisted in cheat_manifest.config.json — add to dumper config" << std::endl;
            }
        }
    }
    return k;
}

inline void* GetMethodPtr(const char* className, const char* methodName, int paramCount = -1) {

    if (Manifest::Loaded()) {
        if (void* m = Manifest::GetMethod(className, methodName)) {
            return m;
        }
    }

    if (!p_il2cpp_class_get_method_from_name) {
        if (auto m = Manifest::GetApi<t_il2cpp_class_get_method_from_name>("il2cpp_class_get_method_from_name")) {
            p_il2cpp_class_get_method_from_name = m;
        } else {
            HMODULE ga = GetModuleHandleA("GameAssembly.dll");
            if (!ga) return nullptr;
            p_il2cpp_class_get_method_from_name = (t_il2cpp_class_get_method_from_name)
                GetProcAddress(ga, "il2cpp_class_get_method_from_name");
            if (!p_il2cpp_class_get_method_from_name) return nullptr;
        }
    }
    void* klass = FindClassDynamic(className);
    if (!klass) return nullptr;
    const void* methodInfo = p_il2cpp_class_get_method_from_name(klass, methodName, paramCount);
    if (!methodInfo) return nullptr;
    void* ptr = *(void**)methodInfo;

    HMODULE ga = GetModuleHandleA("GameAssembly.dll");
    uintptr_t base = (uintptr_t)ga;
    if (!ptr || (uintptr_t)ptr < base || (uintptr_t)ptr > base + 0x30000000) return nullptr;
    return ptr;
}

inline void* GetMethodPtrFromCandidates(std::initializer_list<std::pair<const char*, const char*>> candidates) {
    for (auto& [cls, mth] : candidates) {
        void* p = GetMethodPtr(cls, mth);
        if (p) return p;
    }
    return nullptr;
}

inline bool g_verboseInit = false;

inline void DoHook(void* target, void* hookFn, void** origFn, const char* desc) {
    if (!target) { std::cout << "[Hook] NOT FOUND: " << desc << std::endl; return; }
    if (MH_CreateHook(target, hookFn, origFn) == MH_OK && MH_EnableHook(target) == MH_OK) {
        if (g_verboseInit) {
            uintptr_t rva = (uintptr_t)target - (uintptr_t)GetModuleHandleA("GameAssembly.dll");
            std::cout << "[Hook] OK " << desc << " @ 0x" << std::hex << rva << std::dec << std::endl;
        }
    } else {
        std::cout << "[Hook] MH FAILED: " << desc << std::endl;
    }
}

template<typename T>
inline void DoFuncPtr(void* target, T& outPtr, const char* desc) {
    if (!target) { std::cout << "[FPtr] NOT FOUND: " << desc << std::endl; return; }
    outPtr = (T)target;
    if (g_verboseInit) {
        uintptr_t rva = (uintptr_t)target - (uintptr_t)GetModuleHandleA("GameAssembly.dll");
        std::cout << "[FPtr] OK " << desc << " @ 0x" << std::hex << rva << std::dec << std::endl;
    }
}

inline bool DoBytePatchRet(void* target, const char* desc) {
    if (!target) {
        std::cout << "[ACTk] NOT FOUND: " << desc << "\n";
        return false;
    }
    DWORD oldProtect = 0;
    if (!VirtualProtect(target, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::cout << "[ACTk] VP failed for " << desc << " @ " << target << "\n";
        return false;
    }
    *(uint8_t*)target = 0xC3;
    VirtualProtect(target, 1, oldProtect, &oldProtect);
    FlushInstructionCache(GetCurrentProcess(), target, 1);
    std::cout << "[ACTk] RET-patched " << desc << " @ " << target << "\n";
    return true;
}

inline void DumpClassFields(const char* className)
{
    if (!g_verboseInit) return;
    void* klass = FindClassDynamic(className);
    if (!klass) {
        std::cout << "[Fields] Class NOT FOUND: " << className << std::endl;
        return;
    }
    void* iter = nullptr;
    std::cout << "[Fields] === " << className << " ===" << std::endl;
    while (true) {
        auto* field = IL2CPP::Class::GetFields(
            reinterpret_cast<Unity::il2cppClass*>(klass), &iter);
        if (!field) break;
        if (field->m_pName)
            std::cout << "[Fields]   0x" << std::hex << field->m_iOffset
                      << "  " << field->m_pName << std::dec << std::endl;
    }
}

inline uintptr_t ResolveFieldOffset(const char* className,
                                    std::initializer_list<const char*> candidates,
                                    uintptr_t fallback)
{

    for (const char* name : candidates) {
        auto off = Manifest::GetFieldOffset(className, name);
        if (off && *off > 0) {
            if (g_verboseInit) {
                std::cout << "[Offset] " << className << "::" << name
                          << " = 0x" << std::hex << *off << std::dec << std::endl;
            }
            return static_cast<uintptr_t>(*off);
        }
    }
    std::cout << "[Offset] " << className << " — none of the candidates found, using fallback." << std::endl;
    return fallback;
}

inline void SetupHooks() {

    DoHook(GetMethodPtr("WorldController", "Update"),    &hkWorldControllerUpdate,   (void**)&oWorldControllerUpdate,   "WorldController::Update");
    DoHook(GetMethodPtr("WorldController", "OnDestroy"), &hkWorldControllerOnDestroy,(void**)&oWorldControllerOnDestroy,"WorldController::OnDestroy");

    DoHook(GetMethodPtr("WorldController", "SpeedHackDetected"),
           &hkWCSpeedHackDetected, (void**)&oWCSpeedHackDetected,
           "WorldController::SpeedHackDetected");

    DoBytePatchRet(GetMethodPtr("CodeStage.AntiCheat.Detectors.SpeedHackDetector",       "StartDetectionAutomatically"), "SpeedHack.StartAuto");
    DoBytePatchRet(GetMethodPtr("CodeStage.AntiCheat.Detectors.TimeCheatingDetector",    "StartDetectionAutomatically"), "TimeCheating.StartAuto");
    DoBytePatchRet(GetMethodPtr("CodeStage.AntiCheat.Detectors.InjectionDetector",       "StartDetectionAutomatically"), "Injection.StartAuto");
    DoBytePatchRet(GetMethodPtr("CodeStage.AntiCheat.Detectors.ObscuredCheatingDetector","StartDetectionAutomatically"), "Obscured.StartAuto");
    DoBytePatchRet(GetMethodPtr("CodeStage.AntiCheat.Detectors.WallHackDetector",        "StartDetectionAutomatically"), "WallHack.StartAuto");
    DoBytePatchRet(GetMethodPtr("CodeStage.AntiCheat.Detectors.SpeedHackDetector",       "IsTicksCheated"),              "SpeedHack.IsTicksCheated");
    DoBytePatchRet(Manifest::GetMethodByKey("CodeStage.AntiCheat.Detectors.ObscuredCheatingDetector$$OnCheatingDetected"),   "Obscured.OnCheatingDetected#1");
    DoBytePatchRet(Manifest::GetMethodByKey("CodeStage.AntiCheat.Detectors.ObscuredCheatingDetector$$OnCheatingDetected_4"), "Obscured.OnCheatingDetected#2");

    DoBytePatchRet(GetMethodPtr("AnalyticsManager", "Initialize"),                "AnalyticsManager::Initialize");
    DoBytePatchRet(GetMethodPtr("FirebaseManager",  "InitializeFirebaseAndStart"), "FirebaseManager::InitializeFirebaseAndStart");
    DoBytePatchRet(GetMethodPtr("SingularManager",  "Initialize"),                "SingularManager::Initialize");
    DoBytePatchRet(GetMethodPtr("DevtodevManager",  "Initialize"),                "DevtodevManager::Initialize");
    DoBytePatchRet(GetMethodPtr("DevtodevManager",  "SendPurchaseEvent"),         "DevtodevManager::SendPurchaseEvent");
    DoBytePatchRet(GetMethodPtr("DevtodevManager",  "SendLevelUpEvent"),          "DevtodevManager::SendLevelUpEvent");
    DoBytePatchRet(GetMethodPtr("DevtodevManager",  "SendCurrencyAccrualEvent"),  "DevtodevManager::SendCurrencyAccrualEvent");
    DoBytePatchRet(GetMethodPtr("DevtodevManager",  "SentTutorialEvent"),         "DevtodevManager::SentTutorialEvent");
    DoBytePatchRet(GetMethodPtr("DevtodevManager",  "SendTutorialEvent"),         "DevtodevManager::SendTutorialEvent");

    DoHook(GetMethodPtr("Player", "GetRunSpeed"),
           &hkPlayerGetRunSpeed, (void**)&oPlayerGetRunSpeed, "Player::GetRunSpeed");

    oPlayerWarpPlayer  = (tPlayerWarpPlayer)GetMethodPtr("Player",           "WarpPlayer");
    oOutSendCheckpoint = (tOutSendCheckpoint)GetMethodPtr("OutgoingMessages", "SendCheckpoint");
    oOutSendResurrect  = (tOutSendResurrect) GetMethodPtr("OutgoingMessages", "SendResurrect");

    DoHook(GetMethodPtr("Player", "HitBlock"),
           &hkPlayerHitBlock, (void**)&oPlayerHitBlock, "Player::HitBlock");

    DoHook(GetMethodPtr("Player", "MineBlockWithTool"),
           &hkPlayerMineBlockWithTool,
           (void**)&oPlayerMineBlockWithTool,
           "Player::MineBlockWithTool");
    std::cout << "[Movement] WarpPlayer="     << (void*)oPlayerWarpPlayer
              << " SendCheckpoint="           << (void*)oOutSendCheckpoint
              << " SendResurrect="            << (void*)oOutSendResurrect << "\n";

    oTM_GetBlockTypeName = (tTM_GetBlockTypeName)GetMethodPtr("TextManager", "GetBlockTypeName");
    std::cout << "[DamageLog] TextManager.GetBlockTypeName="
              << (void*)oTM_GetBlockTypeName << "\n";
    DoHook(GetMethodPtr("Player",          "Update"),    &hkPlayerUpdate,             (void**)&oPlayerUpdate,            "Player::Update");
    DoHook(GetMethodPtr("Player",          "OnDestroy"), &hkPlayerOnDestroy,          (void**)&oPlayerOnDestroy,         "Player::OnDestroy");

    DoHook(GetMethodPtrFromCandidates({{"KukouriCamera","Update"},{"CameraController","Update"},{"UIAnimator","Update"}}),
           &hkUpdate, (void**)&oUpdate, "KukouriCamera::Update");

    DoHook(GetMethodPtr("AIEnemyMonoBehaviourBase", "Update"),
           &hkAIUpdate, (void**)&oAIUpdate, "AIEnemyMonoBehaviourBase::Update");
    DoHook(GetMethodPtrFromCandidates({{"AIEnemyMonoBehaviourBase","Destroy"},{"AIEnemyMonoBehaviourBase","OnDestroy"},{"AIEnemy","OnDestroy"}}),
           &hkAIOnDestroy, (void**)&oAIOnDestroy, "AIEnemyMonoBehaviourBase::Destroy");
    DoHook(GetMethodPtr("AIEnemyMonoBehaviourBase", "Start"), &hkAIEnemyMonoBehaviourBase_Start, (void**)&oAIEnemyMonoBehaviourBase_Start, "AIEnemyMonoBehaviourBase::Start");

    DoHook(GetMethodPtr("ConfigData", "IsBlockPinball"),  &hkIsBlockPinball,  (void**)&oIsBlockPinball,  "ConfigData::IsBlockPinball");
    DoHook(GetMethodPtr("ConfigData", "IsBlockHot"),      &hkIsBlockHot,      (void**)&oIsBlockHot,      "ConfigData::IsBlockHot");
    DoHook(GetMethodPtr("ConfigData", "IsBlockSpring"),   &hkIsBlockSpring,   (void**)&oIsBlockSpring,   "ConfigData::IsBlockSpring");
    DoHook(GetMethodPtr("ConfigData", "IsBlockTrampolin"),&hkIsBlockTrampolin,(void**)&oIsBlockTrampolin,"ConfigData::IsBlockTrampolin");
    DoHook(GetMethodPtr("ConfigData", "IsBlockElastic"),  &hkIsBlockElastic,  (void**)&oIsBlockElastic,  "ConfigData::IsBlockElastic");
    DoHook(GetMethodPtr("ConfigData", "IsBlockWind"),     &hkIsBlockWind,     (void**)&oIsBlockWind,     "ConfigData::IsBlockWind");
    DoHook(GetMethodPtr("ConfigData", "IsBlockDeflector"),&hkIsBlockDeflector,(void**)&oIsBlockDeflector,"ConfigData::IsBlockDeflector");
    DoHook(GetMethodPtr("ConfigData", "CanPlaceSeedTopOf"),&hkCanPlaceSeedTopOf,(void**)&oCanPlaceSeedTopOf,"ConfigData::CanPlaceSeedTopOf");
    DoHook(GetMethodPtrFromCandidates({{"ConfigData","GetBlockRunSpeed"},{"ConfigData","GetPlayerRunSpeed"}}),
           &hkGetBlockRunSpeed, (void**)&oGetBlockRunSpeed, "ConfigData::GetBlockRunSpeed");
    DoHook(GetMethodPtrFromCandidates({{"ConfigData","GetCooldownToUseFist"},{"ConfigData","GetFistCooldown"}}),
           &hkGetCooldownToUseFist, (void**)&oGetCooldownToUseFist, "ConfigData::GetCooldownToUseFist");
    DoHook(GetMethodPtrFromCandidates({{"Player","ShouldBelowBlockDoBounce"},{"ConfigData","ShouldBelowBlockDoBounce"},{"ConfigData","IsBlockBounce"}}),
           &hkShouldBelowBlockDoBounce, (void**)&oShouldBelowBlockDoBounce, "Player::ShouldBelowBlockDoBounce");
    DoHook(GetMethodPtrFromCandidates({{"ConfigData","CanPlayerPickCollectableFromBlock"},{"Player","CanPlayerPickCollectableFromBlock"}}),
           &hkCanPlayerPickCollectableFromBlock, (void**)&oCanPlayerPickCollectableFromBlock, "ConfigData::CanPlayerPickCollectableFromBlock");
    DoHook(GetMethodPtr("ConfigData", "GetDeflectorForce"),          &hkGetDeflectorForce,          (void**)&oGetDeflectorForce,          "ConfigData::GetDeflectorForce");
    DoHook(GetMethodPtr("ConfigData", "GetDeflectorMaxForce"),       &hkGetDeflectorMaxForce,       (void**)&oGetDeflectorMaxForce,       "ConfigData::GetDeflectorMaxForce");
    DoHook(GetMethodPtr("ConfigData", "GetDeflectorForceGrounded"),  &hkGetDeflectorForceGrounded,  (void**)&oGetDeflectorForceGrounded,  "ConfigData::GetDeflectorForceGrounded");
    DoHook(GetMethodPtr("ConfigData", "GetDeflectorMaxForceGrounded"),&hkGetDeflectorMaxForceGrounded,(void**)&oGetDeflectorMaxForceGrounded,"ConfigData::GetDeflectorMaxForceGrounded");

    DoHook(GetMethodPtr("ConfigData", "GetBlockGravity"),
           &hkCfgGetBlockGravity, (void**)&oCfgGetBlockGravity, "ConfigData::GetBlockGravity");

    DoHook(GetMethodPtr("Player", "HitPlayerFromBlock"),    &hkHitPlayerFromBlock,    (void**)&oHitPlayerFromBlock,    "Player::HitPlayerFromBlock");

    DoHook(Manifest::GetMethodByKey("Player$$HitPlayerFromBlock_3"),
           &hkHitPlayerFromBlockHF, (void**)&oHitPlayerFromBlockHF,
           "Player::HitPlayerFromBlock(hitForce)");
    DoHook(GetMethodPtr("Player", "HitPlayerFromAIEnemy"),  &hkHitPlayerFromAIEnemy,  (void**)&oHitPlayerFromAIEnemy,  "Player::HitPlayerFromAIEnemy");

    DoHook(Manifest::GetMethodByKey("Player$$HitPlayerFromPlayerDyingExplosion"),
           &hkHitPlayerFromExplosion, (void**)&oHitPlayerFromExplosion,
           "Player::HitPlayerFromPlayerDyingExplosion");

    DoHook(Manifest::GetMethodByKey("PlayerData$$HitPlayer"),
           &hkPlayerData_HitPlayer, (void**)&oPlayerData_HitPlayer,
           "PlayerData::HitPlayer");

    DoHook(Manifest::GetMethodByKey("Player$$CausePoisoned"),
           &hkPlayerCausePoisonedBT, (void**)&oPlayerCausePoisonedBT,
           "Player::CausePoisoned(BlockType)");
    DoHook(Manifest::GetMethodByKey("Player$$CausePoisoned_1"),
           &hkPlayerCausePoisonedAI, (void**)&oPlayerCausePoisonedAI,
           "Player::CausePoisoned(AIEnemyType)");
    DoHook(Manifest::GetMethodByKey("Player$$CausePoisoned_1_2"),
           &hkPlayerCausePoisonedF,  (void**)&oPlayerCausePoisonedF,
           "Player::CausePoisoned(float)");

    DoHook(GetMethodPtr("Player", "KillPlayer"),
           &hkPlayerKillPlayer, (void**)&oPlayerKillPlayer,
           "Player::KillPlayer(BlockType)");
    DoHook(GetMethodPtr("Player", "DeathHelper"),
           &hkPlayerDeathHelper, (void**)&oPlayerDeathHelper,
           "Player::DeathHelper");
    DoHook(GetMethodPtr("Player", "CheckDeathByColliderHelper"),
           &hkPlayerCheckDeathByColliderHelper, (void**)&oPlayerCheckDeathByColliderHelper,
           "Player::CheckDeathByColliderHelper");

    DoHook(GetMethodPtr("ConfigData", "DoesBlockCauseDeathByCollider"),
           &hkDoesBlockCauseDeathByCollider, (void**)&oDoesBlockCauseDeathByCollider,
           "ConfigData::DoesBlockCauseDeathByCollider");
    DoHook(GetMethodPtr("ConfigData", "DoesBlockHaveCollider"),
           &hkDoesBlockHaveCollider, (void**)&oDoesBlockHaveCollider,
           "ConfigData::DoesBlockHaveCollider");

    DoHook(GetMethodPtr("ConfigData", "IsBlockInstakill"),
           &hkIsBlockInstakill, (void**)&oIsBlockInstakill,
           "ConfigData::IsBlockInstakill");

    DoHook(GetMethodPtr("ConfigData", "IsBlockInstaDamageWater"),
           &hkIsBlockInstaDamageWater, (void**)&oIsBlockInstaDamageWater,
           "ConfigData::IsBlockInstaDamageWater");
    DoHook(GetMethodPtr("ConfigData", "IsBlockShooterTrap"),
           &hkIsBlockShooterTrap, (void**)&oIsBlockShooterTrap,
           "ConfigData::IsBlockShooterTrap");
    DoHook(GetMethodPtr("ConfigData", "IsBlockTrap"),
           &hkIsBlockTrap, (void**)&oIsBlockTrap,
           "ConfigData::IsBlockTrap");

    DoFuncPtr(GetMethodPtr("ConfigData", "IsBlockCheckPoint"),
              oIsBlockCheckPoint, "ConfigData::IsBlockCheckPoint");

    DoHook(GetMethodPtr("Player", "CheckCheckPoints"),
           &hkPlayerCheckCheckPoints, (void**)&oPlayerCheckCheckPoints,
           "Player::CheckCheckPoints");

    DoFuncPtr(GetMethodPtr("ConfigData", "IsAnyDoor"),                    Door::oIsAnyDoor,                       "ConfigData::IsAnyDoor");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsNormalDoor"),                 Door::oIsNormalDoor,                    "ConfigData::IsNormalDoor");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsClanDoor"),                   Door::oIsClanDoor,                      "ConfigData::IsClanDoor");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsScifiDoor"),                  Door::oIsScifiDoor,                     "ConfigData::IsScifiDoor");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsVIPDoor"),                    Door::oIsVIPDoor,                       "ConfigData::IsVIPDoor");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsLevelVIPDoor"),               Door::oIsLevelVIPDoor,                  "ConfigData::IsLevelVIPDoor");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsClanFactionDarkDoor"),        Door::oIsClanFactionDark,               "ConfigData::IsClanFactionDarkDoor");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsClanFactionLightDoor"),       Door::oIsClanFactionLight,              "ConfigData::IsClanFactionLightDoor");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsEditableLevelVIPDoorOrHatch"),Door::oIsEditableLevelVIPDoorOrHatch,   "ConfigData::IsEditableLevelVIPDoorOrHatch");

    DoFuncPtr(GetMethodPtr("World", "DoesPlayerHaveRightToAccessDoor"),
              Door::oWorldRightAccess,
              "World::DoesPlayerHaveRightToAccessDoor (lookup only)");
    DoFuncPtr(GetMethodPtr("WorldController", "DoesPlayerHaveRightToGoDoor"),
              Door::oWcRightGo,
              "WorldController::DoesPlayerHaveRightToGoDoor (lookup only)");
    DoFuncPtr(GetMethodPtr("WorldController", "DoesPlayerHaveRightToGoDoorForCollider"),
              Door::oWcRightCollider,
              "WorldController::DoesPlayerHaveRightToGoDoorForCollider (lookup only)");
    DoFuncPtr(GetMethodPtr("PlayerData", "DoesAdminWantToGoThroughDoors"),
              Door::oAdminWants,
              "PlayerData::DoesAdminWantToGoThroughDoors (lookup only)");
    DoFuncPtr(GetMethodPtr("PlayerData", "DoesModWantToGoThroughDoors"),
              Door::oModWants,
              "PlayerData::DoesModWantToGoThroughDoors (lookup only)");
    std::cout << "[Door] subsystem resolved (lookup-only). walkThroughAll=" << (Door::g_walkThroughAll ? "ON" : "off") << "\n";

    DoHook(GetMethodPtr("Player", "DoDyingAnimation"),
           &hkPlayerDoDyingAnimation, (void**)&oPlayerDoDyingAnimation,
           "Player::DoDyingAnimation");
    DoHook(GetMethodPtr("Player", "DoTakeHitStunnedAnimation"),
           &hkPlayerDoTakeHitStunnedAnim, (void**)&oPlayerDoTakeHitStunnedAnim,
           "Player::DoTakeHitStunnedAnimation");
    DoHook(GetMethodPtr("Player", "WaitRespawn"),
           &hkPlayerWaitRespawn, (void**)&oPlayerWaitRespawn,
           "Player::WaitRespawn");
    DoHook(GetMethodPtr("Player", "SetVelocity"),
           &hkSetVelocity, (void**)&oSetVelocity, "Player::SetVelocity");
    DoHook(GetMethodPtrFromCandidates({{"Player","DeathByColliderInCollider"},{"TrapProjectile","OnTriggerEnter2D"}}),
           &hkDeathByColliderInCollider, (void**)&oDeathByColliderInCollider, "Player::DeathByColliderInCollider");
    DoHook(GetMethodPtr("Player", "IsPlayerInMapPoint"),
           &hkIsPlayerInMapPoint, (void**)&oIsPlayerInMapPoint, "Player::IsPlayerInMapPoint");
    DoHook(GetMethodPtrFromCandidates({{"AIEnemyMonoBehaviourBase","TouchDamageHelper"},{"Player","TouchDamageHelper"}}),
           &hkTouchDamageHelper, (void**)&oTouchDamageHelper, "AIEnemyMonoBehaviourBase::TouchDamageHelper");
    DoHook(GetMethodPtr("PlayerData", "HasUnlockedRecipe"), &hkHasUnlockedRecipe, (void**)&oHasUnlockedRecipe, "PlayerData::HasUnlockedRecipe");

    DoHook(GetMethodPtr("WorldController", "BlockColliderAndLayerHelper"),
           &hkBlockColliderAndLayerHelper, (void**)&oBlockColliderAndLayerHelper,
           "WorldController::BlockColliderAndLayerHelper");

    DoHook(GetMethodPtr("UnityEngine.Collider2D", "set_isTrigger"),
           &hkCollider2D_set_isTrigger, (void**)&oColl_set_isTrigger_orig,
           "UnityEngine.Collider2D::set_isTrigger");
    DoHook(GetMethodPtr("WorldController", "ChangeWeather"),        &hkChangeWeather,        (void**)&oChangeWeather,        "WorldController::ChangeWeather");
    DoHook(GetMethodPtr("WorldController", "ChangeLighting"),       &hkChangeLighting,       (void**)&oChangeLighting,       "WorldController::ChangeLighting");
    DoHook(GetMethodPtr("WorldController", "InstantiateFogOfWar"),  &hkInstantiateFogOfWar,  (void**)&oInstantiateFogOfWar,  "WorldController::InstantiateFogOfWar");
    DoHook(GetMethodPtrFromCandidates({{"WorldLighting","SetWorldLighting"},{"WorldController","SetWorldLighting"},{"WorldController","SetLighting"}}),
           &hkSetWorldLighting, (void**)&oSetWorldLighting, "WorldLighting::SetWorldLighting");

    DoHook(GetMethodPtr("NetworkClient", "Update"),         &hkNetworkClientUpdate, (void**)&oNetworkClientUpdate, "NetworkClient::Update");
    DoHook(GetMethodPtrFromCandidates({{"OutgoingMessages","AddOneMessageToList"},{"NetworkClient","AddOneMessageToList"}}),
           &hkAddOneMessageToList, (void**)&oAddOneMessageToList, "OutgoingMessages::AddOneMessageToList");
    DoHook(GetMethodPtr("NetworkClient", "HandleMessages"),
           &hkHandleMessages, (void**)&oHandleMessages, "NetworkClient::HandleMessages");

    DoHook(GetMethodPtr("NetworkClient", "HandleOtherOwnerIPMessage"),
           &hkHandleOtherOwnerIPMessage, (void**)&oHandleOtherOwnerIPMessage,
           "NetworkClient::HandleOtherOwnerIPMessage");

    DoHook(GetMethodPtr("NetworkClient", "HandleWarpPlayerMessage"),
           &Teleport::hkHandleWarpPlayerMessage,
           (void**)&oHandleWarpPlayerMessage_orig,
           "NetworkClient::HandleWarpPlayerMessage (probe)");

    DoHook(GetMethodPtr("NetworkClient", "HandlePlayerActivateInPortal"),
           &Teleport::hkHandlePlayerActivateInPortal,
           (void**)&oHandlePlayerActivateInPortal_orig,
           "NetworkClient::HandlePlayerActivateInPortal (probe)");
    DoHook(GetMethodPtr("NetworkClient", "HandleResurrectPlayer"),
           &Teleport::hkHandleResurrectPlayer,
           (void**)&oHandleResurrectPlayer_orig,
           "NetworkClient::HandleResurrectPlayer (probe)");

    DoHook(GetMethodPtrFromCandidates({{"World","CheckShouldTrapBeOn"},{"WorldController","CheckShouldTrapBeOn"}}),
           &hkCheckShouldTrapBeOn, (void**)&oCheckShouldTrapBeOn, "World::CheckShouldTrapBeOn");
    DoHook(GetMethodPtrFromCandidates({{"World","CheckShouldTrapBeOnByWiring"},{"WorldController","CheckShouldTrapBeOnByWiring"}}),
           &hkCheckShouldTrapBeOnByWiring, (void**)&oCheckShouldTrapBeOnByWiring, "World::CheckShouldTrapBeOnByWiring");
    DoHook(GetMethodPtrFromCandidates({{"World","GetPlayerMaxDeathsCount"},{"Player","GetPlayerMaxDeathsCount"},{"WorldController","GetPlayerMaxDeathsCount"}}),
           &hkGetPlayerMaxDeathsCount, (void**)&oGetPlayerMaxDeathsCount, "World::GetPlayerMaxDeathsCount");

    DoHook(GetMethodPtrFromCandidates({{"KukouriCamera","UpdateCameraPosition"},{"CameraController","UpdateCameraPosition"},{"PlayerCamera","UpdateCameraPosition"}}),
           &hkUpdateCameraPosition, (void**)&oUpdateCameraPosition, "KukouriCamera::UpdateCameraPosition");
    DoHook(GetMethodPtrFromCandidates({{"KukouriCamera","ForceUpdatePosition"},{"CameraController","ForceUpdatePosition"},{"PlayerCamera","ForceUpdatePosition"}}),
           &hkForceUpdatePosition, (void**)&oForceUpdatePosition, "KukouriCamera::ForceUpdatePosition");

    DoHook(GetMethodPtr("KukouriCamera", "SetZoomValueTo"),
           &hkKC_SetZoomValueTo, (void**)&oKC_SetZoomValueTo,
           "KukouriCamera::SetZoomValueTo");

    DoHook(GetMethodPtr("KukouriCamera", "UpdateZoomByZoomValue"),
           &hkKC_UpdateZoomByZoomValue, (void**)&oKC_UpdateZoomByZoomValue,
           "KukouriCamera::UpdateZoomByZoomValue");

    DoHook(GetMethodPtr("Camera", "set_orthographicSize"),
           &hkSetOrthographicSize, (void**)&oSetOrthographicSize,
           "Camera::set_orthographicSize");

    DoHook(GetMethodPtr("InventoryControl", "InventoryDropButtonClicked"),
           &hkInvDropButtonClicked, (void**)&oInvDropButtonClicked,
           "InventoryControl::InventoryDropButtonClicked");
    DoFuncPtr(GetMethodPtr("InventoryControl", "ActualDropButtonAction"),
              oInvActualDrop,
              "InventoryControl::ActualDropButtonAction");

    {
        uintptr_t off = ResolveFieldOffset("KukouriCamera",
            {"zoomValue","m_zoomValue","_zoomValue"}, g_off_KC_zoomValue);
        if (off) g_off_KC_zoomValue = off;
    }
    DoHook(GetMethodPtr("Camera",    "WorldToScreenPoint"),  &hkWorldToScreenPoint,  (void**)&oWorldToScreenPoint,  "Camera::WorldToScreenPoint");
    DoHook(GetMethodPtr("Camera",    "ScreenToWorldPoint"),  &hkScreenToWorldPoint,  (void**)&oScreenToWorldPoint,  "Camera::ScreenToWorldPoint");
    DoHook(GetMethodPtr("Camera",    "get_main"),            &hkGetMainCamera,       (void**)&oGetMainCamera,       "Camera::get_main");
    DoHook(GetMethodPtr("Transform", "get_position"),        &hkGetPosition,         (void**)&oGetPosition,         "Transform::get_position");
    DoHook(GetMethodPtr("Transform", "set_position"),        &hkSetPosition,         (void**)&oSetPosition,         "Transform::set_position");
    DoHook(GetMethodPtrFromCandidates({{"Component","get_transform"},{"GameObject","get_transform"}}),
           &hkGetTransform, (void**)&oGetTransform, "Component::get_transform");

    DoHook(GetProcAddress(GetModuleHandleA("user32.dll"), "GetAsyncKeyState"),
           &hkGetAsyncKeyState, (void**)&oGetAsyncKeyState, "GetAsyncKeyState");

    DoHook(GetProcAddress(GetModuleHandleA("ws2_32.dll"), "connect"),
           &ProxyTunnel::hkConnect, (void**)&ProxyTunnel::oConnect, "ws2_32::connect");
    DoHook(GetProcAddress(GetModuleHandleA("ws2_32.dll"), "WSAConnect"),
           &ProxyTunnel::hkWSAConnect, (void**)&ProxyTunnel::oWSAConnect, "ws2_32::WSAConnect");
    ProxyCfg::LoadFromDisk();
    ProxyPool::LoadFromDisk();
    ProxyTunnel::ApplyConfig();

    DoHook(GetMethodPtr("ProfanityFilter",         "Censor"),                  &hkCensor,                    (void**)&oCensor,                    "ProfanityFilter::Censor");
    DoHook(GetMethodPtr("FishingGaugeMinigameUI",  "RandomizeFishTargetPoint"),&hkRandomizeFishTargetPoint,  (void**)&oRandomizeFishTargetPoint,  "FishingGaugeMinigameUI::RandomizeFishTargetPoint");
    DoHook(GetMethodPtr("FishingGaugeMinigameUI",  "Update"),                  &FishBot::hkFishingGaugeMinigameUI_Update, (void**)&oFishingGaugeMinigameUI_Update, "FishingGaugeMinigameUI::Update");
    DoHook(GetMethodPtr("FishingGaugeMinigameUI",  "SetTargetAreaPosition"),   &FishBot::hkFishingGaugeMinigameUI_SetTargetAreaPosition, (void**)&oFishingGaugeMinigameUI_SetTargetAreaPosition, "FishingGaugeMinigameUI::SetTargetAreaPosition");

    DoHook(GetMethodPtr("FishingGaugeMinigameUI",  "SetupMinigame"),           &FishBot::hkSetupMinigame, (void**)&FishBot::oSetupMinigame, "FishingGaugeMinigameUI::SetupMinigame");
    DoFuncPtr(GetMethodPtrFromCandidates({{"FishingData","GetFishSizeByBlockType"},{"ConfigData","GetFishSizeByBlockType"}}), FishBot::oGetFishSizeByBlockType, "FishingData::GetFishSizeByBlockType");

    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendStartArchaeologyGameMessage"},{"NetworkClient","SendStartArchaeologyGameMessage"}}),   AutoFossil::oSendStartArch,  "OutgoingMessages::SendStartArchaeologyGameMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendArchaeologyGameActionMessage"},{"NetworkClient","SendArchaeologyGameActionMessage"}}), AutoFossil::oSendArchAction, "OutgoingMessages::SendArchaeologyGameActionMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendStopMiniGameMessage"},{"NetworkClient","SendStopMiniGameMessage"}}),                   AutoFossil::oSendStopMini,   "OutgoingMessages::SendStopMiniGameMessage");

    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendStopMiniGameMessage"},{"NetworkClient","SendStopMiniGameMessage"}}),                   FishBot::oFishStopMini,      "OutgoingMessages::SendStopMiniGameMessage (fishing)");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsBlockExcavatableFossil"),   AutoFossil::IsBlockExcavatableFossil,   "ConfigData::IsBlockExcavatableFossil");
    DoFuncPtr(GetMethodPtr("ConfigData", "IsBlockExcavatableMonolith"), AutoFossil::IsBlockExcavatableMonolith, "ConfigData::IsBlockExcavatableMonolith");
    DoFuncPtr(GetMethodPtr("InventoryControl", "UseConsumable"),        AutoFossil::oUseConsumable,             "InventoryControl::UseConsumable");
    DoFuncPtr(GetMethodPtr("ArchaeologyGameUI", "GameSlotClicked"),     AutoFossil::oGameSlotClicked,          "ArchaeologyGameUI::GameSlotClicked");
    DoHook(GetMethodPtr("ArchaeologyGameUI", "Update"), &AutoFossil::hkArchUpdate, (void**)&AutoFossil::oArchUpdate, "ArchaeologyGameUI::Update");

    DoHook(GetMethodPtr("ArchaeologyGameUI", "StartGame"),                &AutoFossil::hkArchStartGame,    (void**)&AutoFossil::oArchStartGame,    "ArchaeologyGameUI::StartGame");
    DoHook(GetMethodPtr("ArchaeologyGameUI", "HandleGameActionResponse"), &AutoFossil::hkArchHandleAction, (void**)&AutoFossil::oArchHandleAction, "ArchaeologyGameUI::HandleGameActionResponse");
    DoHook(GetMethodPtr("ArchaeologyGameUI", "CompleteGame"),             &AutoFossil::hkArchComplete,     (void**)&AutoFossil::oArchComplete,     "ArchaeologyGameUI::CompleteGame");
    DoHook(GetMethodPtr("ArchaeologyGameUI", "StopGame"),                 &AutoFossil::hkArchStop,         (void**)&AutoFossil::oArchStop,         "ArchaeologyGameUI::StopGame");
    {
        void* archCls = FindClassDynamic("ArchaeologyGameUI");
        if (archCls && il2cpp_class_get_field_from_name)
            AutoFossil::g_field_archGameIsRunning = il2cpp_class_get_field_from_name(archCls, "gameIsRunning");
        std::cout << "[AutoFossil] gameIsRunning field = "
                  << (AutoFossil::g_field_archGameIsRunning ? "OK" : "NOT FOUND") << "\n";
    }
    DoHook(GetMethodPtrFromCandidates({{"FishingResultsPopupUI","DoShowAnimation"},{"FishingResultsPopup","DoShowAnimation"}}),
           &hkFishingResultsPopup_DoShowAnimation, (void**)&oFishingResultsPopup_DoShowAnimation, "FishingResultsPopupUI::DoShowAnimation");
    DoHook(GetMethodPtrFromCandidates({{"FishingResultsPopupUI","DoHideAnimation"},{"FishingResultsPopup","DoHideAnimation"}}),
           &hkFishingResultsPopup_DoHideAnimation, (void**)&oFishingResultsPopup_DoHideAnimation, "FishingResultsPopupUI::DoHideAnimation");

    DoFuncPtr(GetMethodPtrFromCandidates({{"SimpleBSON","Dump"},{"BSON","Dump"}}),           oBsonDump,              "SimpleBSON::Dump");
    DoFuncPtr(GetMethodPtrFromCandidates({{"SimpleBSON","Load"},{"BSON","Load"}}),            oSimpleBSON_Load,       "SimpleBSON::Load");
    DoFuncPtr(GetMethodPtrFromCandidates({{"World","SetGravityMode"},{"Player","SetGravityMode"},{"CharacterController2D","SetGravityMode"}}), oSetGravityMode, "World::SetGravityMode");
    DoFuncPtr(GetMethodPtr("WorldController", "ConvertWorldPointToMapPoint"),  oConvertWorldPointToMapPoint,  "WorldController::ConvertWorldPointToMapPoint");
    DoFuncPtr(GetMethodPtr("WorldController", "ConvertMapPointToWorldPoint"),  oConvertMapPointToWorldPoint_V3,"WorldController::ConvertMapPointToWorldPoint");
    DoFuncPtr(GetMethodPtrFromCandidates({{"WorldController","ChangeBackground"},{"World","ChangeBackground"}}), oChangeBackground, "WorldController::ChangeBackground");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendHitAIEnemyMessage"},{"NetworkClient","SendHitAIEnemyMessage"}}), oSendHitAIEnemyMessage, "OutgoingMessages::SendHitAIEnemyMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendPing"},{"NetworkClient","SendPing"}}), PingProbe::oSendPing, "OutgoingMessages::SendPing");
    DoFuncPtr(GetMethodPtrFromCandidates({{"AIEnemies","CanPlayerHitAIEnemy"},{"AIEnemyController","CanPlayerHitAIEnemy"},{"Player","CanPlayerHitAIEnemy"}}), oCanPlayerHitAIEnemy, "AIEnemies::CanPlayerHitAIEnemy");
    DoFuncPtr(GetMethodPtr("DeepMaggotMonoBehaviour", "CanBeHit"), oDeepMaggotCanBeHit, "DeepMaggotMonoBehaviour::CanBeHit");
    DoFuncPtr(GetMethodPtr("Player",           "GoFromPortal"),               oGoFromPortal,                 "Player::GoFromPortal");

    DoFuncPtr(GetMethodPtrFromCandidates({{"SceneLoader","ReloadGame"},{"WorldController","ReloadGame"},{"GameManager","ReloadGame"}}), oReloadGame, "SceneLoader::ReloadGame");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendCollectCollectableMessage"},{"NetworkClient","SendCollectCollectableMessage"}}), oSendCollectCollectableMessage, "OutgoingMessages::SendCollectCollectableMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"SceneLoader","JoinDynamicWorld"},{"WorldController","JoinDynamicWorld"},{"NetworkClient","JoinDynamicWorld"}}), oJoinDynamicWorld, "SceneLoader::JoinDynamicWorld");
    DoFuncPtr(GetMethodPtrFromCandidates({{"TMP_Text","SetText"},{"TextMeshPro","SetText"},{"TextMeshProUGUI","SetText"}}), o_TMP_SetText, "TMP_Text::SetText");
    DoFuncPtr(GetMethodPtrFromCandidates({{"AIEnemyMonoBehaviourBase","DeactivateAIEnemy"},{"AIEnemyMonoBehaviourBase","Deactivate"},{"AIEnemy","DeactivateAIEnemy"}}), oDeactivateAIEnemy, "AIEnemyMonoBehaviourBase::DeactivateAIEnemy");
    DoFuncPtr(GetMethodPtr("FishingGaugeMinigameUI", "LandButtonPressed"),    LandButtonPressed,             "FishingGaugeMinigameUI::LandButtonPressed");
    DoFuncPtr(GetMethodPtrFromCandidates({{"ConfigData","IsFishingLure"},{"FishingData","IsFishingLure"}}), IsFishingLure, "ConfigData::IsFishingLure");

    {
        void* sbwt = GetMethodPtrFromCandidates({{"WorldController","SetBaitWithTool"},{"Player","SetBaitWithTool"},{"FishingController","SetBaitWithTool"}});
        DoHook(sbwt, hkSetBaitWithTool, (void**)&oSetBaitWithTool, "WorldController::SetBaitWithTool");
    }
    DoFuncPtr(GetMethodPtrFromCandidates({{"Player","IsFishStrikeActive"},{"FishingGaugeMinigameUI","IsFishStrikeActive"},{"FishingGaugeMinigameUI","get_IsFishStrikeActive"}}), oIsFishStrikeActive, "Player::IsFishStrikeActive");
    DoFuncPtr(GetMethodPtrFromCandidates({{"FishingResultsPopupUI","TakeFishPressed"},{"FishingGaugeMinigameUI","TakeFishPressed"},{"FishingResultsPopup","TakeFishPressed"}}), oTakeFishPressed, "FishingResultsPopupUI::TakeFishPressed");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendSetBlockMessage"},{"NetworkClient","SendSetBlockMessage"}}), oSendSetBlockMessage, "OutgoingMessages::SendSetBlockMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendHitBlockMessage"},{"NetworkClient","SendHitBlockMessage"}}), oSendHitBlockMessage, "OutgoingMessages::SendHitBlockMessage");

    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","MiningPickaxeRepairing"},{"NetworkClient","MiningPickaxeRepairing"}}), oMiningPickaxeRepairing, "OutgoingMessages::MiningPickaxeRepairing");

    DoFuncPtr(GetMethodPtr("NetworkClient", "RequestPlayerData"), oRequestPlayerData, "NetworkClient::RequestPlayerData");

    DoFuncPtr(GetMethodPtr("OutgoingMessages", "RecycleMiningGemstone"), oRecycleMiningGemstone, "OutgoingMessages::RecycleMiningGemstone");

    DoFuncPtr(GetMethodPtr("OutgoingMessages", "RecycleAllMiningGemstone"), oRecycleAllMiningGemstone, "OutgoingMessages::RecycleAllMiningGemstone");

    Store::Resolve();

    {
        void* p = Manifest::GetMethodByKey("PlayerData$$SetAndSendWorldLoadArgs");
        if (p) {
            oSetAndSendWorldLoadArgs = (tSetAndSendWorldLoadArgs)p;
            std::cout << "[Setup] oSetAndSendWorldLoadArgs (int) wired @ "
                      << p << "\n";
        } else {
            std::cout << "[Setup] PlayerData$$SetAndSendWorldLoadArgs missing from manifest\n";
        }
    }

    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendRequestGeneratedMineExitMessage"},{"NetworkClient","SendRequestGeneratedMineExitMessage"}}), oSendRequestGeneratedMineExit, "OutgoingMessages::SendRequestGeneratedMineExitMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendRequestNetherWorldExitMessage"},{"NetworkClient","SendRequestNetherWorldExitMessage"}}), oSendRequestNetherWorldExit, "OutgoingMessages::SendRequestNetherWorldExitMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendRequestDeepNetherExitMessage"},{"NetworkClient","SendRequestDeepNetherExitMessage"}}), oSendRequestDeepNetherExit, "OutgoingMessages::SendRequestDeepNetherExitMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendRequestItemFromGiftBoxMessage"},{"NetworkClient","SendRequestItemFromGiftBoxMessage"}}), oSendRequestItemFromGiftBox, "OutgoingMessages::SendRequestItemFromGiftBoxMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendFishOnAreaMessage"},{"NetworkClient","SendFishOnAreaMessage"}}), oSendFishOnAreaMessage, "OutgoingMessages::SendFishOnAreaMessage");
    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","SendFishOffAreaMessage"},{"NetworkClient","SendFishOffAreaMessage"}}), oSendFishOffAreaMessage, "OutgoingMessages::SendFishOffAreaMessage");

    DoHook(GetMethodPtrFromCandidates({{"ConvertItemsUI","HandleGenericItemConversion"}}),
           &hk_CI_HandleGenericItemConversion,
           (void**)&oOrig_CI_HandleGenericItemConversion,
           "ConvertItemsUI::HandleGenericItemConversion");
    DoHook(GetMethodPtrFromCandidates({{"MiningPickaxeRepairingUI","ConvertMaterialsAndRepairItem"}}),
           &hk_MPR_ConvertMaterialsAndRepairItem,
           (void**)&oOrig_MPR_ConvertMaterialsAndRepairItem,
           "MiningPickaxeRepairingUI::ConvertMaterialsAndRepairItem");
    DoHook(GetMethodPtrFromCandidates({{"MiningGemstoneRecyclerUI","RecycleAllMiningGemstone"}}),
           &hk_GR_RecycleAllMiningGemstone,
           (void**)&oOrig_GR_RecycleAllMiningGemstone,
           "MiningGemstoneRecyclerUI::RecycleAllMiningGemstone");

    DoHook(GetMethodPtrFromCandidates({{"NetworkClient","HandleGetCurrentInventoryData"}}),
           &hk_NC_HandleGetCurrentInventoryData,
           (void**)&oOrig_NC_HandleGetCurrentInventoryData,
           "NetworkClient::HandleGetCurrentInventoryData");
    DoFuncPtr(GetMethodPtrFromCandidates({{"ConfigData","GetHitsRequired"},{"ConfigData","GetBlockHitsRequired"}}), oGetHitsRequired, "ConfigData::GetHitsRequired");
    DoFuncPtr(GetMethodPtrFromCandidates({{"InventoryControl","GetCurrentSelection"},{"Player","GetCurrentSelection"},{"ItemSelector","GetCurrentSelection"}}), oGetCurrentSelection, "InventoryControl::GetCurrentSelection");

    DumpClassFields("AIEnemyMonoBehaviourBase");
    DumpClassFields("WorldController");
    DumpClassFields("World");
    DumpClassFields("Player");

    g_off_AITempPosition = ResolveFieldOffset("AIEnemyMonoBehaviourBase",
        {"tempPosition", "_tempPosition", "m_tempPosition",
         "position", "_position", "m_position"},
        g_off_AITempPosition);

    g_off_AIBase = ResolveFieldOffset("AIEnemyMonoBehaviourBase",
        {"aiBase", "m_aiBase", "aiEnemy", "m_aiEnemy",
         "aiEnemyData", "enemyData", "m_enemyData"},
        g_off_AIBase);

    g_off_WCWorld = ResolveFieldOffset("WorldController",
        {"world", "m_world", "m_World", "_world", "currentWorld", "activeWorld"},
        g_off_WCWorld);

    g_off_WCAIEnemies = ResolveFieldOffset("WorldController",
        {"aiEnemyMonoBehaviours", "aiEnemies", "m_aiEnemies", "spawnedAIEnemies",
         "activeAIEnemies", "enemies", "m_enemies", "spawnedEnemies"},
        g_off_WCAIEnemies);

    g_off_WorldBlockLayer = ResolveFieldOffset("World",
        {"worldBlockLayer", "m_worldBlockLayer", "blockLayer",
         "foregroundLayer", "m_foregroundLayer", "tileLayer"},
        g_off_WorldBlockLayer);

    g_off_WorldSize = ResolveFieldOffset("World",
        {"worldSizeX", "worldSize", "m_worldSize", "size", "m_size", "mapSize", "dimensions"},
        g_off_WorldSize);

    g_off_WorldCollectablesList = ResolveFieldOffset("World",
        {"collectables", "collectablesList", "m_collectables", "m_collectablesList",
         "collectableList", "m_collectableList", "spawnedCollectables", "activeCollectables"},
        g_off_WorldCollectablesList);

    g_off_PlayerNameTMP = ResolveFieldOffset("Player",
        {"playerNameTextMeshPro", "playerNameSubMesh", "playerNameGameObject",
         "playerNameLabel", "nameLabel", "playerNameText", "playerName", "nameText"},
        g_off_PlayerNameTMP);

    if (g_verboseInit) {
        std::cout << "[Offsets] AI tempPos=0x" << std::hex << g_off_AITempPosition
                  << "  aiBase=0x"    << g_off_AIBase
                  << "  WC.world=0x"  << g_off_WCWorld
                  << "  WC.aiList=0x" << g_off_WCAIEnemies
                  << "  W.blkLyr=0x"  << g_off_WorldBlockLayer
                  << "  W.size=0x"    << g_off_WorldSize
                  << "  W.clist=0x"   << g_off_WorldCollectablesList
                  << std::dec << std::endl;
    }

    DoHook(GetMethodPtrFromCandidates({{"GameplayUI","OnButtonDownNew"},{"GameUI","OnButtonDownNew"}}),
        &FishBot::hkOnButtonDownNew, (void**)&oOnButtonDownNew, "GameplayUI::OnButtonDownNew");

    {
        void* ctrlHelperCls = FindClassDynamic("ControllerHelper");
        if (ctrlHelperCls && il2cpp_class_get_field_from_name) {
            g_gameplayUIField = il2cpp_class_get_field_from_name(ctrlHelperCls, "gameplayUI");
            std::cout << "[FishBot] ControllerHelper.gameplayUI field = "
                      << (g_gameplayUIField ? "OK" : "NOT FOUND") << "\n";
        } else {
            std::cout << "[FishBot] WARNING: ControllerHelper class not found.\n";
        }
    }

    DoFuncPtr(GetMethodPtrFromCandidates({{"FishingResultsPopupUI","ClosePopup"},{"FishingResultsPopup","ClosePopup"}}),
        oClosePopupFish, "FishingResultsPopupUI::ClosePopup");
    DoFuncPtr(GetMethodPtrFromCandidates({{"World","AreMapPointsValidForFishing"},{"WorldController","AreMapPointsValidForFishing"}}),
        oAreMapPointsValidForFishing, "World::AreMapPointsValidForFishing");

    {
        void* configDataCls = FindClassDynamic("ConfigData");
        if (configDataCls && il2cpp_class_get_field_from_name) {
            g_field_playerChangeToSleepSeconds         = il2cpp_class_get_field_from_name(configDataCls, "playerChangeToSleepSeconds");
            g_field_playerInactivitySeconds            = il2cpp_class_get_field_from_name(configDataCls, "playerInactivitySeconds");
            g_field_playerInactivitySecondsFishing     = il2cpp_class_get_field_from_name(configDataCls, "playerInactivitySecondsFishing");
            g_field_playerInactivitySecondsMatchmaking = il2cpp_class_get_field_from_name(configDataCls, "playerInactivitySecondsMatchmaking");
            std::cout << "[FishBot] Anti-sleep fields:"
                      << " sleep=" << (g_field_playerChangeToSleepSeconds ? "OK" : "X")
                      << " inact=" << (g_field_playerInactivitySeconds ? "OK" : "X")
                      << " fish=" << (g_field_playerInactivitySecondsFishing ? "OK" : "X")
                      << " mm=" << (g_field_playerInactivitySecondsMatchmaking ? "OK" : "X") << "\n";

            SetAntiSleepFields();

            g_field_rocketFuelConsumptionSpeed      = il2cpp_class_get_field_from_name(configDataCls, "rocketFuelConsumptionSpeed");
            g_field_rocketFuelConsumptionSpeed60FPS = il2cpp_class_get_field_from_name(configDataCls, "rocketFuelConsumptionSpeed60FPS");
            std::cout << "[Jetpack] Fuel fields:"
                      << " consumption=" << (g_field_rocketFuelConsumptionSpeed ? "OK" : "X")
                      << " consumption60FPS=" << (g_field_rocketFuelConsumptionSpeed60FPS ? "OK" : "X") << "\n";

            WriteJetpackFuelFields();
        }
    }

    DoFuncPtr(GetMethodPtrFromCandidates({{"PlayerData","GetInventoryAsOrderedByInventoryItemType"},{"PlayerData","GetOrderedInventory"}}),
        oGetInventoryAsOrderedByInventoryItemType, "PlayerData::GetInventoryAsOrderedByInventoryItemType");
    DoFuncPtr(GetMethodPtrFromCandidates({{"PlayerData","GetCount"},{"PlayerData","GetItemCount"},{"PlayerData","get_Count"}}),
        oGetCountByKey, "PlayerData::GetCount");
    DoFuncPtr(GetMethodPtrFromCandidates({{"PlayerData","AddGems"}}),
        oAddGems, "PlayerData::AddGems");
    DoFuncPtr(GetMethodPtrFromCandidates({{"PlayerData","GetGemAmount"},{"PlayerData","get_GemAmount"}}),
        oGetGemAmount, "PlayerData::GetGemAmount");
    DoFuncPtr(GetMethodPtrFromCandidates({{"PlayerData","GetByteCoinAmount"},{"PlayerData","get_ByteCoinAmount"}}),
        oGetByteCoinAmount, "PlayerData::GetByteCoinAmount");

    {
        uintptr_t off = ResolveFieldOffset("Player", {"jumpMode","m_jumpMode"}, 0x8c);
        if (off) g_off_PlayerJumpMode = off;
    }

    {
        uintptr_t off = ResolveFieldOffset("Player", {"playerPoisonedCooldown","m_playerPoisonedCooldown"}, 0x480);
        if (off) g_off_PlayerPoisonedCooldown = off;
    }

    {
        uintptr_t off;
        off = ResolveFieldOffset("Player", {"velocity","m_velocity"}, g_off_Player_velocity);
        if (off) g_off_Player_velocity = off;
        off = ResolveFieldOffset("Player", {"isSubAquaticPerkActive","m_isSubAquaticPerkActive"}, g_off_Player_isSubAquaticPerkActive);
        if (off) g_off_Player_isSubAquaticPerkActive = off;
        off = ResolveFieldOffset("Player", {"currentPlayerMapPoint","m_currentPlayerMapPoint"}, g_off_Player_currentPlayerMapPoint);
        if (off) g_off_Player_currentPlayerMapPoint = off;
        off = ResolveFieldOffset("Player", {"currentPlayerAboveMapPoint","m_currentPlayerAboveMapPoint"}, g_off_Player_currentPlayerAboveMapPoint);
        if (off) g_off_Player_currentPlayerAboveMapPoint = off;
        off = ResolveFieldOffset("Player", {"currentPlayerRightMapPoint","m_currentPlayerRightMapPoint"}, g_off_Player_currentPlayerRightMapPoint);
        if (off) g_off_Player_currentPlayerRightMapPoint = off;
        off = ResolveFieldOffset("Player", {"currentPlayerLeftMapPoint","m_currentPlayerLeftMapPoint"}, g_off_Player_currentPlayerLeftMapPoint);
        if (off) g_off_Player_currentPlayerLeftMapPoint = off;
        off = ResolveFieldOffset("Player", {"rocketFuelConsumptionSpeed","m_rocketFuelConsumptionSpeed"}, g_off_Player_rocketFuelConsumption);
        if (off) g_off_Player_rocketFuelConsumption = off;
        off = ResolveFieldOffset("Player", {"rocketFuel","m_rocketFuel"}, g_off_Player_rocketFuel);
        if (off) g_off_Player_rocketFuel = off;
        off = ResolveFieldOffset("Player", {"rocketFuelConsumptionSpeed60FPS","m_rocketFuelConsumptionSpeed60FPS"}, g_off_Player_rocketFuelConsumption60);
        if (off) g_off_Player_rocketFuelConsumption60 = off;
        off = ResolveFieldOffset("Player", {"fullRocketFuel","m_fullRocketFuel"}, g_off_Player_fullRocketFuel);
        if (off) g_off_Player_fullRocketFuel = off;
        off = ResolveFieldOffset("Player", {"gravity","m_gravity","playerGravity"}, g_off_Player_gravity);
        if (off) g_off_Player_gravity = off;
        off = ResolveFieldOffset("Player", {"lastFrameVelocityY","m_lastFrameVelocityY","lastVelocityY"}, g_off_Player_lastFrameVelocityY);
        if (off) g_off_Player_lastFrameVelocityY = off;
    }

    DumpClassFields("FishingGaugeMinigameUI");
    DumpClassFields("Player");

    g_off_Gauge_isReadyToLand = ResolveFieldOffset("FishingGaugeMinigameUI",
        {"isReadyToLand","m_isReadyToLand","<isReadyToLand>k__BackingField","_isReadyToLand"},
        0);
    g_off_Gauge_fishTargetPoint = ResolveFieldOffset("FishingGaugeMinigameUI",
        {"fishTargetPoint","m_fishTargetPoint","<fishTargetPoint>k__BackingField","_fishTargetPoint"},
        0);
    g_off_Gauge_targetAreaMaxVelocity = ResolveFieldOffset("FishingGaugeMinigameUI",
        {"targetAreaMaxVelocity","m_targetAreaMaxVelocity","<targetAreaMaxVelocity>k__BackingField"},
        0);
    g_off_Player_fishingState = ResolveFieldOffset("Player",
        {"fishingState","m_fishingState","<fishingState>k__BackingField","_fishingState"},
        0);
    g_off_Player_fishingLeftButton = ResolveFieldOffset("Player",
        {"fishingLeftButton","m_fishingLeftButton","<fishingLeftButton>k__BackingField","_fishingLeftButton"},
        0);
    g_off_Player_fishingRightButton = ResolveFieldOffset("Player",
        {"fishingRightButton","m_fishingRightButton","<fishingRightButton>k__BackingField","_fishingRightButton"},
        0);
    g_off_Player_jumpButtonDown = ResolveFieldOffset("Player",
        {"jumpButtonDown","m_jumpButtonDown","<jumpButtonDown>k__BackingField","_jumpButtonDown"},
        0);
    g_off_Player_leftButton = ResolveFieldOffset("Player",
        {"leftButton","m_leftButton","<leftButton>k__BackingField","_leftButton"},
        0);
    g_off_Player_rightButton = ResolveFieldOffset("Player",
        {"rightButton","m_rightButton","<rightButton>k__BackingField","_rightButton"},
        0);
    g_off_Player_jumpButton = ResolveFieldOffset("Player",
        {"jumpButton","m_jumpButton","<jumpButton>k__BackingField","_jumpButton"},
        0);

    if (g_verboseInit) {
        std::cout << "[FishBot Offsets]"
                  << " isReadyToLand=0x"     << std::hex << g_off_Gauge_isReadyToLand
                  << " fishTargetPoint=0x"   << g_off_Gauge_fishTargetPoint
                  << " fishingState=0x"      << g_off_Player_fishingState
                  << " fishingLeft=0x"       << g_off_Player_fishingLeftButton
                  << " fishingRight=0x"      << g_off_Player_fishingRightButton
                  << " jumpDown=0x"          << g_off_Player_jumpButtonDown
                  << " leftBtn=0x"           << g_off_Player_leftButton
                  << std::dec << std::endl;
    }

    DoFuncPtr(GetMethodPtrFromCandidates({{"OutgoingMessages","ConvertItems"},
                                          {"NetworkClient","ConvertItems"}}),
              oConvertItems, "OutgoingMessages::ConvertItems");
    DoFuncPtr(GetMethodPtrFromCandidates({{"ConfigData","GetXPLevel"}}),
              oGetXPLevel, "ConfigData::GetXPLevel");

    DoFuncPtr(GetMethodPtr("PlayerData", "RemoveItemsFromInventory"),
              oRemoveItemsFromInventory, "PlayerData::RemoveItemsFromInventory");

    {
        void* p = Manifest::GetMethodByKey("PlayerData$$AddItemToInventory_4");
        if (p) {
            oAddItemToInventory = (tAddItemToInventory)p;
            std::cout << "[Setup] oAddItemToInventory wired via _4 overload @ "
                      << p << "\n";
        } else {

            DoFuncPtr(GetMethodPtr("PlayerData", "AddItemToInventory"),
                      oAddItemToInventory, "PlayerData::AddItemToInventory (fallback)");
        }
    }
    DoFuncPtr(GetMethodPtr("InventoryControl", "ForceRefreshInventory"),
              oForceRefreshInventory, "InventoryControl::ForceRefreshInventory");
    DoFuncPtr(GetMethodPtr("InventoryControl", "RefreshInventoryCache"),
              oRefreshInventoryCache, "InventoryControl::RefreshInventoryCache");

    DoFuncPtr(GetMethodPtr("PlayerData", "SetWornWeaponDurability"),
              oSetWornWeaponDurability, "PlayerData::SetWornWeaponDurability");

    DoFuncPtr(GetMethodPtrFromCandidates({
        {"DataFactory", "SpawnDataClassForEnum"},
        {"ConfigData",  "SpawnDataClassForEnum"},
    }), oSpawnDataClassForEnum, "DataFactory::SpawnDataClassForEnum");
}

namespace Pnb {

    inline bool  g_enabled      = false;
    inline int   g_blockType    = 0;
    inline int   g_itemType     = 0;
    inline bool  g_offsets[5][5] = {};
    inline bool  g_offsetsInit = []{
        for (int r = 0; r < 5; ++r)
            for (int c = 0; c < 5; ++c)
                if (!(r == 2 && c == 2)) g_offsets[r][c] = true;
        return true;
    }();
    inline bool  g_autoCollect  = false;
    inline int   g_collectEveryN = 5;
    inline int   g_selectDelayMs = 250;
    inline int   g_placeDelayMs  = 230;
    inline int   g_breakDelayMs  = 230;
    inline int   g_cycleDelayMs  = 300;
    inline int   g_placeRetryMs  = 1200;
    inline int   g_maxPlaceTries = 3;
    inline int   g_maxHits       = 12;

    inline Vector2i    g_anchor    = { -1, -1 };
    inline std::string g_anchorWorld;
    inline bool        g_hasAnchor = false;

    inline int g_cycles = 0, g_placed = 0, g_broken = 0;

    enum class Phase { Idle, ToAnchor, Selecting, Placing, WaitPlace, Breaking, WaitBreak, Collecting, Cooldown };
    inline Phase g_phase = Phase::Idle;
    inline Phase g_resumePhase = Phase::Idle;

    inline std::vector<Vector2i> g_targets;
    inline std::vector<Vector2i> g_placedTiles;
    inline size_t   g_idx           = 0;
    inline int      g_hits          = 0;
    inline bool     g_selected      = false;
    inline int      g_placeAttempts = 0;
    inline Vector2i g_curTarget     = { 0, 0 };
    inline std::chrono::steady_clock::time_point g_nextAction = std::chrono::steady_clock::now();
    inline std::chrono::steady_clock::time_point g_collectDeadline = std::chrono::steady_clock::now();
    inline int    g_collected = 0;

    inline std::vector<Vector2i> g_collectVisited;
    inline Vector2i g_lastCollectTile  = { 0, 0 };
    inline bool     g_lastCollectValid = false;

    inline int g_heldBlock = 0, g_heldItemType = 0, g_heldQty = 0;

    inline void BuildTargets() {
        g_targets.clear();
        for (int row = 0; row < 5; ++row)
            for (int col = 0; col < 5; ++col) {
                if (row == 2 && col == 2) continue;
                if (!g_offsets[row][col]) continue;
                g_targets.push_back({ g_anchor.x + (col - 2), g_anchor.y + (row - 2) });
            }
    }

    inline std::chrono::milliseconds Jitter(int base, int amt) {
        int j = amt > 0 ? (rand() % (2 * amt + 1)) - amt : 0;
        int v = base + j; if (v < 0) v = 0;
        return std::chrono::milliseconds(v);
    }

    inline void SortBreakTargets() {
        Vector2i anch = g_anchor;
        auto d2 = [&](const Vector2i& p) {
            int dx = p.x - anch.x, dy = p.y - anch.y; return dx * dx + dy * dy;
        };
        std::sort(g_placedTiles.begin(), g_placedTiles.end(),
                  [&](const Vector2i& a, const Vector2i& b) { return d2(a) < d2(b); });
        for (size_t i = 0; i + 1 < g_placedTiles.size(); ++i)
            if ((rand() % 10) < 3) std::swap(g_placedTiles[i], g_placedTiles[i + 1]);
    }

    inline int ScanInventoryBlocks(int* outBlock, int* outQty, int cap) {
        if (!g_LocalPlayer || !oGetInventoryAsOrderedByInventoryItemType || !oGetCountByKey)
            return 0;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return 0;
        Il2CppArray* arr = nullptr;
        __try { arr = oGetInventoryAsOrderedByInventoryItemType(pd); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
        if (!arr || !arr->max_length) return 0;
        auto* keys = (PlayerData_InventoryKey*)arr->items;
        int n = 0;
        for (size_t i = 0; i < arr->max_length && n < cap; ++i) {
            PlayerData_InventoryKey k = keys[i];
            if ((int)k.itemType != 0) continue;
            short qty = 0;
            __try { qty = oGetCountByKey(pd, k); }
            __except (EXCEPTION_EXECUTE_HANDLER) { continue; }
            if (qty > 0) { outBlock[n] = (int)k.blockType; outQty[n] = (int)qty; ++n; }
        }
        return n;
    }

    inline bool CurrentSelection(int& bt, int& it);

    inline int CountKeyQty(int bt, int it) {
        if (!g_LocalPlayer || !oGetCountByKey || bt <= 0) return 0;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return 0;
        PlayerData_InventoryKey k{ (World::BlockType)bt, (InventoryItemType)it };
        short qty = 0;
        __try { qty = oGetCountByKey(pd, k); } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
        return (int)qty;
    }

    inline int  g_invBlock[24] = {};
    inline int  g_invQty[24]   = {};
    inline char g_invName[24][64] = {};
    inline int  g_invCount     = 0;
    inline std::chrono::steady_clock::time_point g_invScanAt{};

    inline void ScanInventoryTick() {
        auto now = std::chrono::steady_clock::now();
        if (now < g_invScanAt) return;
        g_invScanAt = now + std::chrono::milliseconds(750);
        int blocks[24] = {}, qtys[24] = {};

        __try {
            int n = ScanInventoryBlocks(blocks, qtys, 24);
            if (n < 0) n = 0; if (n > 24) n = 24;
            for (int i = 0; i < n; ++i) {
                g_invBlock[i] = blocks[i];
                g_invQty[i]   = qtys[i];
                const char* nm = BlockName(blocks[i]);
                int j = 0; for (; nm && nm[j] && j < 63; ++j) g_invName[i][j] = nm[j];
                g_invName[i][j] = '\0';
            }
            g_invCount = n;

            int hb = 0, hi = 0;
            if (CurrentSelection(hb, hi) && hb > 0 && hi == 0) {
                g_heldBlock = hb; g_heldItemType = hi; g_heldQty = CountKeyQty(hb, hi);
            } else {
                g_heldBlock = 0; g_heldItemType = 0; g_heldQty = 0;
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    inline int SelectedBlockQty() {
        if (!g_LocalPlayer || !oGetCountByKey || g_blockType <= 0) return 0;
        void* pd = *(void**)((uintptr_t)g_LocalPlayer + Offsets::Player_myPlayerData);
        if (!pd) return 0;
        PlayerData_InventoryKey k{ (World::BlockType)g_blockType, (InventoryItemType)g_itemType };
        short qty = 0;
        __try { qty = oGetCountByKey(pd, k); } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
        return (int)qty;
    }

    inline bool CurrentSelection(int& bt, int& it) {
        if (!oGetCurrentSelection || !g_InventoryControl) return false;
        PlayerData_InventoryKey k{};
        __try { k = oGetCurrentSelection(g_InventoryControl); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        bt = (int)k.blockType; it = (int)k.itemType;
        return bt > 0;
    }

    inline const char* PhaseLabel() {
        switch (g_phase) {
        case Phase::ToAnchor:   return "returning";
        case Phase::Selecting:  return "selecting";
        case Phase::Placing:
        case Phase::WaitPlace:  return "placing";
        case Phase::Breaking:
        case Phase::WaitBreak:  return "breaking";
        case Phase::Collecting: return "collecting";
        case Phase::Cooldown:   return "cooldown";
        default:                return "idle";
        }
    }

    inline void Stop() {
        g_phase = Phase::Idle;
        g_resumePhase = Phase::Idle;
        g_targets.clear(); g_placedTiles.clear();
        g_collectVisited.clear(); g_lastCollectValid = false;
        g_idx = 0; g_hits = 0; g_placeAttempts = 0; g_selected = false;
        if (!g_hasAnchor) { g_anchor = { -1, -1 }; g_anchorWorld.clear(); }
        PathFinder_ClearInputs();
        PathFinder_RestoreGravity();
    }

    inline void SavePosition() {
        Vector2i mp;
        if (!GetPlayerMapPoint(mp)) return;
        g_anchor      = mp;
        g_anchorWorld = GetCurrentWorldName();
        g_hasAnchor   = true;
        Stop();
    }

    inline void _Place(Vector2i t) {
        if (!oSendSetBlockMessage) return;
        __try { oSendSetBlockMessage(t, (World::BlockType)g_blockType, g_itemType, 0); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    inline void _Break(Vector2i t) {
        if (!oSendHitBlockMessage) return;
        __try { oSendHitBlockMessage(t, GetDateTimeTicksGlobal(), false); }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

}

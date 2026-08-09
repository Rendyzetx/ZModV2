#pragma once

#include "main.h"
#include "PathFinding.h"
#include "AutoNether.h"

#include "AutoMine.h"
#include "BankBot.h"
#include "GemSeller.h"
#include "Theme.h"
#include "Styles.h"
#include "esp.h"
#include <fstream>
#include <filesystem>
#include <shlobj.h>
#include <commdlg.h>
#include "XorStr.h"

namespace Config {

    inline std::chrono::steady_clock::time_point g_lastSaveTime{};
    inline std::filesystem::path                 g_lastSavePath{};
    inline bool                                  g_lastSaveOk = false;

    inline std::filesystem::path GetConfigPath() {
        char buf[MAX_PATH] = {};
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buf))) {
            std::filesystem::path p = buf;
            p /= "ZMod";
            std::error_code ec;
            std::filesystem::create_directories(p, ec);
            return p / "config.json";
        }
        return "config.json";
    }

    inline nlohmann::json BuildConfig() {
        nlohmann::json j;

        auto& pr = j["pathfinder"];
        pr["moveMethod"]      = (int)PathRenderer::g_moveMethod;
        pr["moveSpeed"]       = PathRenderer::g_moveSpeed;
        pr["stopRange"]       = PathRenderer::g_stopRange;
        pr["airborneCheck"]   = PathRenderer::g_airborneCheck;
        pr["maxAirborne"]     = PathRenderer::g_maxAirborne;
        pr["chamfer"]         = PathRenderer::g_pathChamfer;
        pr["chamferAmount"]   = PathRenderer::g_pathChamferAmount;
        pr["bodyOffsetFrac"]  = PathRenderer::g_bodyOffsetFrac;
        pr["walkInstakill"]   = PathRenderer::g_walkInstakill;
        pr["pathGlow"]        = PathRenderer::g_pathGlow;
        pr["pathFlow"]        = PathRenderer::g_pathFlow;
        pr["pathPulse"]       = PathRenderer::g_pathPulse;

        pr["showPath"]        = PathRenderer::g_showPath;
        pr["autoMove"]        = PathRenderer::g_autoMove;
        pr["debugBlockOverlay"] = PathRenderer::g_debugBlockOverlay;

        pr["tpTickIntervalMs"] = PathRenderer::g_tp_tickIntervalMs;
        pr["tpTickJitterMs"]   = PathRenderer::g_tp_tickJitterMs;
        pr["strictTraps"]      = PathRenderer::g_pfStrictTraps;
        pr["hazardBufferRadius"] = PathRenderer::g_pfHazardBufferRadius;

        pr["tpThrottlePackets"] = PathRenderer::g_pf_tp_throttleMovePackets;
        pr["tpZeroVelocity"]    = PathRenderer::g_pf_tp_zeroVelocity;
        pr["tpForceGrounded"]   = PathRenderer::g_pf_tp_forceGrounded;
        pr["tpSuppressGravity"] = PathRenderer::g_pf_tp_suppressGravity;

        auto& an = j["autonether"];
        an["autoMove"]        = AutoNether::g_autoMove;
        an["showPath"]        = AutoNether::g_showPath;
        an["hitInterval"]     = AutoNether::g_hitInterval;
        an["stopRange"]       = AutoNether::g_stopRange;
        an["maxHitRange"]     = AutoNether::g_maxHitRange;
        an["minMonsterHp"]    = AutoNether::g_minMonsterHp;
        an["flightScale"]     = AutoNether::g_flightScale;
        an["targetEnemies"]   = AutoNether::g_targetEnemies;
        an["targetTreasure"]  = AutoNether::g_targetTreasure;
        an["targetGifts"]     = AutoNether::g_targetGifts;
        an["targetFloor"]     = AutoNether::g_targetFloor;
        an["netherOnly"]      = AutoNether::g_netherOnly;
        an["priorityClosest"] = AutoNether::g_priorityClosest;
        an["antiSleep"]       = AutoNether::g_antiSleep;
        an["antiAfk"]         = AutoNether::g_antiAfk;
        an["moveMethod"]      = (int)AutoNether::g_moveMethod;
        an["moveSpeed"]       = AutoNether::g_moveSpeed;
        an["pfStopRange"]     = AutoNether::g_pfStopRange;
        an["bodyOffsetFrac"]  = AutoNether::g_bodyOffsetFrac;
        an["pathGlow"]        = AutoNether::g_pathGlow;
        an["pathFlow"]        = AutoNether::g_pathFlow;
        an["pathPulse"]       = AutoNether::g_pathPulse;
        auto& po = an["pathOpts"];

        po["airborneCheck"]   = AutoNether::g_pathOpts.airborneCheck;
        po["maxAirborne"]     = AutoNether::g_pathOpts.maxAirborne;
        po["chamfer"]         = AutoNether::g_pathOpts.chamfer;
        po["chamferAmount"]   = AutoNether::g_pathOpts.chamferAmount;
        po["walkInstakill"]   = AutoNether::g_pathOpts.walkInstakill;

        auto& gv = j["antigrav"];
        gv["pulse"]   = g_pfGravPulse;
        gv["onMs"]    = g_pfGravPulseOnMs;
        gv["offMs"]   = g_pfGravPulseOffMs;

        auto& am = j["automine"];
        am["enabled"]              = AutoMine::g_enabled;
        am["autoMove"]             = AutoMine::g_autoMove;
        am["showPath"]             = AutoMine::g_showPath;
        am["hitInterval"]          = AutoMine::g_hitInterval;
        am["stopRange"]            = AutoMine::g_stopRange;
        am["flightScale"]          = AutoMine::g_flightScale;
        am["mineOnly"]             = AutoMine::g_mineOnly;
        am["targetGemstones"]      = AutoMine::g_targetGemstones;
        am["targetNuggets"]        = AutoMine::g_targetNuggets;
        am["targetCrystals"]       = AutoMine::g_targetCrystals;
        am["targetPots"]           = AutoMine::g_targetPots;
        am["targetCollect"]        = AutoMine::g_targetCollect;
        am["targetAI"]             = AutoMine::g_targetAI;
        am["targetAINearbyOnly"]   = AutoMine::g_targetAINearbyOnly;
        am["targetExit"]           = AutoMine::g_targetExit;
        am["antiSleep"]            = AutoMine::g_antiSleep;
        am["antiAfk"]              = AutoMine::g_antiAfk;
        am["requirePickaxe"]       = AutoMine::g_requirePickaxe;
        am["autoRepairPickaxe"]    = AutoMine::g_autoRepairPickaxe;
        am["repairThreshold"]      = AutoMine::g_repairThreshold;

        am["combatEnabled"]        = AutoMine::g_combatEnabled;
        am["combatCadence"]        = AutoMine::g_combatCadence;
        am["moveMethod"]           = (int)AutoMine::g_moveMethod;
        am["moveSpeed"]            = AutoMine::g_moveSpeed;
        am["pfStopRange"]          = AutoMine::g_pfStopRange;
        am["bodyOffsetFrac"]       = AutoMine::g_bodyOffsetFrac;
        am["pathGlow"]             = AutoMine::g_pathGlow;
        am["pathFlow"]             = AutoMine::g_pathFlow;
        am["weightUpPenaltySolid"] = AutoMine::g_weights.upPenaltySolid;
        am["weightUnsupportedExtra"] = AutoMine::g_weights.unsupportedExtra;
        am["weightPreferStaircase"]  = AutoMine::g_weights.preferStaircase;

        auto& al = am["autoloop"];
        al["enabled"]         = AutoMine::AutoLoop::g_enabled;
        al["autoCombine"]     = AutoMine::AutoLoop::g_autoCombine;
        al["autoEnter"]       = AutoMine::AutoLoop::g_autoEnter;
        al["combineCadenceMs"]= AutoMine::AutoLoop::g_combineCadenceMs;
        al["autoRejoin"]      = AutoMine::AutoLoop::g_autoRejoin;
        al["rejoinDelayMs"]   = AutoMine::AutoLoop::g_rejoinDelayMs;
        std::vector<int> lvls;
        for (int i = 0; i < 5; ++i) lvls.push_back(AutoMine::AutoLoop::g_levelEnabled[i] ? 1 : 0);
        al["levelEnabled"]    = lvls;

        auto& bb = j["bankbot"];
        bb["autoDeposit"]      = BankBot::g_autoDeposit;
        bb["depositCadenceMs"] = BankBot::g_depositCadenceMs;
        std::vector<int> nug, key, mat, gemM;
        for (int i = 0; i < 5; ++i) nug.push_back(BankBot::g_selNuggets[i] ? 1 : 0);
        for (int i = 0; i < 4; ++i) key.push_back(BankBot::g_selKeys[i] ? 1 : 0);
        for (int i = 0; i < 9; ++i) mat.push_back(BankBot::g_selMaterials[i] ? 1 : 0);

        for (int t = 0; t < 5; ++t) for (int s = 0; s < 5; ++s)
            gemM.push_back(BankBot::g_depositMatrix[t][s] ? 1 : 0);
        bb["selNuggets"]   = nug;
        bb["selKeys"]      = key;
        bb["selMaterials"] = mat;
        bb["depositMatrix"] = gemM;

        auto& ft = j["features"];
        ft["godMode"]           = GodModeEnabled;
        ft["antiPoison"]        = g_antiPoison;
        ft["autoGiftBox"]       = g_autoGiftBox;
        ft["extraZoom"]         = g_extraZoom;
        ft["extraZoomMul"]      = g_extraZoomMul;
        ft["antiBounce"]        = AntiBounce;
        ft["antiBounceIgnoreMushrooms"] = AntiBounceIgnoreMushrooms;
        ft["antiFan"]           = antifan;
        ft["infiniteJetpack"]   = InfiniteJetpack;
        ft["antiCensor"]        = AntiCensor;
        ft["antiDarkness"]      = AntiDarkness;
        ft["disableEnemies"]    = DisableEnemies;
        ft["disableGassy"]      = DisableGassy;
        ft["noBlockKill"]       = noBlockKill;
        ft["unlimitedOxygen"]   = unlimitedOxygenEnabled;
        ft["unlockAllRecipes"]  = unlockAllRecipes;
        ft["placeSeedAnywhere"] = PlaceSeedAnywhere;
        ft["blockOnPlayer"]     = g_BlockOnPlayer;
        ft["fastHit"]           = fastHitEnabled;
        ft["extraFastHit"]      = ExtraFasthitEnabled;
        ft["jumpModeOverride"]  = EnableJumpModeOverride;
        ft["selectedJumpMode"]  = (int)SelectedJumpMode;
        ft["gravityOverride"]   = EnableGravityOverride;
        ft["runSpeedMult"]      = runSpeedMultiplier;
        ft["freecam"]           = FreecamEnabled;
        ft["freecamSpeed"]      = freecamSpeed;
        ft["uncapFps"]          = g_uncapFps;
        ft["targetFps"]         = g_targetFps;
        ft["showMinimap"]       = Minimap::showMinimap;
        ft["minimapZoom"]       = Minimap::minimapZoomAmount;
        ft["streamerMode"]      = g_streamerMode;

        ft["doorWalkThrough"]   = Door::g_walkThroughAll;
        ft["gmMasterSwap"]      = g_gmMasterSwap;
        ft["antiAcidWater"]     = g_antiAcidWater;
        ft["actkBypass"]        = g_actkBypass;
        ft["debugDamageLog"]    = g_debugDamageLog;
        ft["playerRunSpeedHack"]= g_playerRunSpeedHack;
        ft["playerRunSpeedMul"] = g_playerRunSpeedMul;
        ft["forceWeather"]      = g_forceWeather;
        ft["forcedWeatherType"] = (int)g_forcedWeatherType;
        ft["blockTooltip"]      = ::Minimap::g_blockTooltip;

        auto& lt = j["lighting"];
        lt["forceLighting"] = g_forceLighting;
        lt["forcedType"]    = (int)g_forcedLightingType;

        auto& gs = j["gemseller"];
        gs["autoSell"]      = GemSeller::g_autoSell;
        gs["sellCadenceMs"] = GemSeller::g_sellCadenceMs;
        std::vector<int> gsM;

        for (int t = 0; t < 5; ++t) for (int s = 0; s < 5; ++s)
            gsM.push_back(GemSeller::g_sellMatrix[t][s] ? 1 : 0);
        gs["sellMatrix"] = gsM;

        auto& th = j["theme"];
        th["currentStyle"] = (int)currentStyle;
        th["customInitialized"] = g_customThemeInitialized;
        std::vector<std::vector<float>> palette;
        palette.reserve(ImGuiCol_COUNT);
        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
            const ImVec4& c = g_customThemeColors[i];
            palette.push_back({ c.x, c.y, c.z, c.w });
        }
        th["customColors"] = palette;

        auto v4 = [](const ImVec4& c) -> nlohmann::json {
            return nlohmann::json{ c.x, c.y, c.z, c.w };
        };
        auto& esp = j["esp"];
        esp["master"]                       = ESP::bMasterSwitch;
        esp["debugInfo"]                    = ESP::bEnableDebugInfo;

        esp["playerBoxes"]                  = ESP::bEnablePlayerBoxes;
        esp["boxType"]                      = ESP::boxType;
        esp["boxFill"]                      = ESP::bBoxFill;
        esp["boxThickness"]                 = ESP::boxThickness;
        esp["boxColor"]                     = v4(ESP::boxColor);
        esp["boxFillColor"]                 = v4(ESP::boxFillColor);
        esp["snaplines"]                    = ESP::bEnableSnaplines;
        esp["lineOrigin"]                   = ESP::lineOrigin;
        esp["lineThickness"]                = ESP::lineThickness;
        esp["lineColor"]                    = v4(ESP::lineColor);
        esp["playerHealth"]                 = ESP::bEnablePlayerHealthESP;
        esp["playerHealthStyle"]            = ESP::PlayerHealthStyle;
        esp["healthColorHigh"]              = v4(ESP::healthColorHigh);
        esp["healthColorLow"]               = v4(ESP::healthColorLow);

        esp["aiBoxes"]                      = ESP::bEnableAIBoxes;
        esp["aiBoxType"]                    = ESP::AIBoxType;
        esp["aiBoxFill"]                    = ESP::bAIBoxFill;
        esp["aiBoxThickness"]               = ESP::AIBoxThickness;
        esp["aiBoxColor"]                   = v4(ESP::AIBoxColor);
        esp["aiBoxFillColor"]               = v4(ESP::AIBoxFillColor);
        esp["aiLines"]                      = ESP::bEnableAILines;
        esp["aiLineOrigin"]                 = ESP::AILineOrigin;
        esp["aiLineThickness"]              = ESP::ailineThickness;
        esp["aiLineColor"]                  = v4(ESP::AIlineColor);

        esp["gemBoxes"]                     = ESP::bEnableGemstoneBoxes;
        esp["gemLines"]                     = ESP::bEnableGemstoneLines;
        esp["gemLineOrigin"]                = ESP::GemstoneLineOrigin;
        esp["gemSnapThickness"]             = ESP::gemSnapThickness;
        esp["gemSnapColor"]                 = v4(ESP::gemSnapColor);
        esp["gemBoxType"]                   = ESP::gemBoxType;
        esp["gemBoxThickness"]              = ESP::gemBoxThickness;
        esp["gemBoxFill"]                   = ESP::bGemBoxFill;
        esp["gemBoxColor"]                  = v4(ESP::gemBoxColor);
        esp["gemBoxFillColor"]              = v4(ESP::gemBoxFillColor);

        esp["collectBoxes"]                 = ESP::bEnableCollectableBoxes;
        esp["collectLines"]                 = ESP::bEnableCollectableLines;
        esp["collectLineOrigin"]            = ESP::collectableLineOrigin;
        esp["collectLineThickness"]         = ESP::collectableLineThickness;
        esp["collectBoxType"]               = ESP::collectableBoxType;
        esp["collectBoxThickness"]          = ESP::collectableBoxThickness;
        esp["collectBoxFill"]               = ESP::bCollectableBoxFill;
        esp["collectBoxColor"]              = v4(ESP::collectableBoxColor);
        esp["collectLineColor"]             = v4(ESP::collectableLineColor);
        esp["collectBoxFillColor"]          = v4(ESP::collectableBoxFillColor);

        esp["lpBoxes"]                      = ESP::bEnableLocalPlayerBoxes;
        esp["lpBoxType"]                    = ESP::localPlayerBoxType;
        esp["lpBoxFill"]                    = ESP::bLocalPlayerBoxFill;
        esp["lpBoxThickness"]               = ESP::localPlayerBoxThickness;
        esp["lpBoxFillColor"]               = v4(ESP::localPlayerBoxFillColor);
        esp["lpHealth"]                     = ESP::bEnableLocalPlayerHealthESP;
        esp["lpHealthStyle"]                = ESP::localPlayerHealthStyle;
        esp["lpHealthColorHigh"]            = v4(ESP::localPlayerHealthColorHigh);
        esp["lpHealthColorLow"]             = v4(ESP::localPlayerHealthColorLow);

        esp["gbLines"]                      = ESP::bEnableGiftBoxLines;
        esp["gbLineOrigin"]                 = ESP::giftBoxLineOrigin;
        esp["gbLineThickness"]              = ESP::giftBoxLineThickness;
        esp["gbLineColor"]                  = v4(ESP::giftBoxLineColor);
        esp["gbBoxes"]                      = ESP::bEnableGiftBoxBoxes;
        esp["gbBoxType"]                    = ESP::giftBoxBoxType;
        esp["gbBoxFill"]                    = ESP::bGiftBoxBoxFill;
        esp["gbBoxThickness"]               = ESP::giftBoxBoxThickness;
        esp["gbBoxColor"]                   = v4(ESP::giftBoxBoxColor);
        esp["gbBoxFillColor"]               = v4(ESP::giftBoxBoxFillColor);

        esp["exLines"]                      = ESP::bEnableExitPortalLines;
        esp["exLineOrigin"]                 = ESP::exitPortalLineOrigin;
        esp["exLineThickness"]              = ESP::exitPortalLineThickness;
        esp["exLineColor"]                  = v4(ESP::exitPortalLineColor);
        esp["exBoxes"]                      = ESP::bEnableExitPortalBoxes;
        esp["exBoxType"]                    = ESP::exitPortalBoxType;
        esp["exBoxFill"]                    = ESP::bExitPortalBoxFill;
        esp["exBoxThickness"]               = ESP::exitPortalBoxThickness;
        esp["exBoxColor"]                   = v4(ESP::exitPortalBoxColor);
        esp["exBoxFillColor"]               = v4(ESP::exitPortalBoxFillColor);

        esp["lcBoxes"]                      = ESP::bEnableLightCrystalBoxes;
        esp["lcLines"]                      = ESP::bEnableLightCrystalLines;
        esp["lcLineOrigin"]                 = ESP::LightCrystalLineOrigin;
        esp["lcLineThickness"]              = ESP::lightCrystalLineThickness;
        esp["lcLineColor"]                  = v4(ESP::lightCrystalLineColor);
        esp["lcBoxType"]                    = ESP::lightCrystalBoxType;
        esp["lcBoxThickness"]               = ESP::lightCrystalBoxThickness;
        esp["lcBoxFill"]                    = ESP::bLightCrystalBoxFill;
        esp["lcBoxColor"]                   = v4(ESP::lightCrystalBoxColor);
        esp["lcBoxFillColor"]               = v4(ESP::lightCrystalBoxFillColor);

        esp["potBoxes"]                     = ESP::bEnablePotBoxes;
        esp["potLines"]                     = ESP::bEnablePotLines;
        esp["potLineOrigin"]                = ESP::PotLineOrigin;
        esp["potLineThickness"]             = ESP::potLineThickness;
        esp["potLineColor"]                 = v4(ESP::potLineColor);
        esp["potBoxType"]                   = ESP::potBoxType;
        esp["potBoxThickness"]              = ESP::potBoxThickness;
        esp["potBoxFill"]                   = ESP::bPotBoxFill;
        esp["potBoxColor"]                  = v4(ESP::potBoxColor);
        esp["potBoxFillColor"]              = v4(ESP::potBoxFillColor);

        esp["treBoxes"]                     = ESP::bEnableTreasureBoxes;
        esp["treLines"]                     = ESP::bEnableTreasureLines;
        esp["treLineOrigin"]                = ESP::TreasureLineOrigin;
        esp["treLineThickness"]             = ESP::treasureLineThickness;
        esp["treBoxType"]                   = ESP::treasureBoxType;
        esp["treBoxThickness"]              = ESP::treasureBoxThickness;
        esp["treBoxFill"]                   = ESP::bTreasureBoxFill;
        esp["treBoxColor"]                  = v4(ESP::treasureBoxColor);
        esp["treBoxFillColor"]              = v4(ESP::treasureBoxFillColor);

        esp["aiBoxHeightMul"]               = ESP::AI_Box_Height_Multiplier;
        esp["aiBoxWidthMul"]                = ESP::AI_Box_Width_Multiplier;
        esp["pathfinder"]                   = ESP::bEnablePathfinder;

        auto& fb = j["fishbot"];
        fb["enabled"]            = FishBot::g_enabled;
        fb["autoLand"]           = FishBot::g_autoLand;
        fb["autoRecast"]         = FishBot::g_autoRecast;
        fb["legitMode"]          = FishBot::g_legitMode;
        fb["antiSleep"]          = FishBot::g_antiSleep;
        fb["antiAfk"]            = FishBot::g_antiAfk;
        fb["recastDelayMinSec"]  = FishBot::g_recastDelayMinSec;
        fb["recastDelayMaxSec"]  = FishBot::g_recastDelayMaxSec;
        fb["hookReactionMinMs"]  = FishBot::g_hookReactionMinMs;
        fb["hookReactionMaxMs"]  = FishBot::g_hookReactionMaxMs;
        fb["legitPauseDelayMin"] = FishBot::g_legitPauseDelayMin;
        fb["legitPauseDelayMax"] = FishBot::g_legitPauseDelayMax;
        fb["legitPauseDurMin"]   = FishBot::g_legitPauseDurMin;
        fb["legitPauseDurMax"]   = FishBot::g_legitPauseDurMax;
        fb["verboseMinigame"]    = FishBot::g_verboseMinigame;
        fb["speedMult"]          = FishBot::g_speedMult;

        auto& pn = j["pnb"];
        pn["autoCollect"] = Pnb::g_autoCollect;
        pn["blockType"]   = Pnb::g_blockType;
        pn["itemType"]    = Pnb::g_itemType;
        std::vector<int> pnOffs;
        for (int r = 0; r < 5; ++r)
            for (int col = 0; col < 5; ++col)
                pnOffs.push_back(Pnb::g_offsets[r][col] ? 1 : 0);
        pn["offsets"] = pnOffs;

        auto& mm = j["minimap"];
        mm["tooltipFontScale"] = Minimap::g_tooltipFontScale;

        return j;
    }

    inline bool g_loaded = false;

    inline bool Save() {
        try {
            auto path = GetConfigPath();
            std::ofstream f(path, std::ios::trunc);
            if (!f) {
                g_lastSaveOk = false;
                return false;
            }
            f << BuildConfig().dump(2);
            bool ok = f.good();
            g_lastSaveOk   = ok;
            g_lastSaveTime = std::chrono::steady_clock::now();
            g_lastSavePath = path;
            return ok;
        } catch (...) { g_lastSaveOk = false; return false; }
    }

    inline void Tick() {
        if (!g_loaded) return;
        static auto lastCheck = std::chrono::steady_clock::now();
        static std::string lastJson;
        static bool firstWriteLogged = false;
        auto now = std::chrono::steady_clock::now();
        if (now - lastCheck < std::chrono::seconds(2)) return;
        lastCheck = now;
        std::string current;
        try { current = BuildConfig().dump(); }
        catch (...) { return; }
        if (current == lastJson) return;
        auto path = GetConfigPath();
        try {
            std::ofstream f(path, std::ios::trunc);
            if (!f) {
                std::cout << XS("[Config] Save FAILED — could not open ")
                          << path.string() << "\n";
                return;
            }
            f << current;
            if (f.good()) {
                lastJson = std::move(current);
                g_lastSaveOk   = true;
                g_lastSaveTime = std::chrono::steady_clock::now();
                g_lastSavePath = path;
                if (!firstWriteLogged) {
                    std::cout << XS("[Config] Auto-saved to ") << path.string() << "\n";
                    firstWriteLogged = true;
                }
            } else {
                g_lastSaveOk = false;
            }
        } catch (const std::exception& e) {
            std::cout << XS("[Config] Save EXCEPTION: ") << e.what()
                      << XS(" path=") << path.string() << "\n";
        } catch (...) {
            std::cout << XS("[Config] Save EXCEPTION (unknown) path=")
                      << path.string() << "\n";
        }
    }

    inline bool Load() {

        struct Flip { ~Flip() { g_loaded = true; } } flip;
        try {
            auto path = GetConfigPath();
            std::cout << XS("[Config] Load path: ") << path.string() << "\n";
            std::error_code ec;
            if (!std::filesystem::exists(path, ec)) return false;
            std::ifstream f(path);
            if (!f) return false;
            nlohmann::json j;
            f >> j;

            auto getB = [](const nlohmann::json& o, const char* k, bool& out) {
                if (o.contains(k) && o[k].is_boolean()) out = o[k].get<bool>();
            };
            auto getI = [](const nlohmann::json& o, const char* k, int& out) {
                if (o.contains(k) && o[k].is_number_integer()) out = o[k].get<int>();
            };
            auto getF = [](const nlohmann::json& o, const char* k, float& out) {
                if (o.contains(k) && o[k].is_number()) out = (float)o[k].get<double>();
            };

            if (j.contains("pathfinder") && j["pathfinder"].is_object()) {
                auto& pr = j["pathfinder"];
                int mm = (int)PathRenderer::g_moveMethod;
                getI(pr, "moveMethod", mm);
                PathRenderer::g_moveMethod = (PathRenderer::MoveMethod)mm;
                getF(pr, "moveSpeed",      PathRenderer::g_moveSpeed);
                getF(pr, "stopRange",      PathRenderer::g_stopRange);
                getB(pr, "airborneCheck",  PathRenderer::g_airborneCheck);
                getI(pr, "maxAirborne",    PathRenderer::g_maxAirborne);
                getB(pr, "chamfer",        PathRenderer::g_pathChamfer);
                getF(pr, "chamferAmount",  PathRenderer::g_pathChamferAmount);
                getF(pr, "bodyOffsetFrac", PathRenderer::g_bodyOffsetFrac);
                getB(pr, "walkInstakill",  PathRenderer::g_walkInstakill);
                getB(pr, "pathGlow",       PathRenderer::g_pathGlow);
                getB(pr, "pathFlow",       PathRenderer::g_pathFlow);
                getB(pr, "pathPulse",      PathRenderer::g_pathPulse);

                getB(pr, "showPath",       PathRenderer::g_showPath);
                getB(pr, "autoMove",       PathRenderer::g_autoMove);
                getB(pr, "debugBlockOverlay", PathRenderer::g_debugBlockOverlay);
                getF(pr, "tpTickIntervalMs", PathRenderer::g_tp_tickIntervalMs);
                getI(pr, "tpTickJitterMs",   PathRenderer::g_tp_tickJitterMs);
                getB(pr, "strictTraps",      PathRenderer::g_pfStrictTraps);
                getI(pr, "hazardBufferRadius", PathRenderer::g_pfHazardBufferRadius);
                getB(pr, "tpThrottlePackets", PathRenderer::g_pf_tp_throttleMovePackets);
                getB(pr, "tpZeroVelocity",    PathRenderer::g_pf_tp_zeroVelocity);
                getB(pr, "tpForceGrounded",   PathRenderer::g_pf_tp_forceGrounded);
                getB(pr, "tpSuppressGravity", PathRenderer::g_pf_tp_suppressGravity);
            }

            if (j.contains("autonether") && j["autonether"].is_object()) {
                auto& an = j["autonether"];
                getB(an, "autoMove",       AutoNether::g_autoMove);
                getB(an, "showPath",       AutoNether::g_showPath);
                getF(an, "hitInterval",    AutoNether::g_hitInterval);
                getF(an, "stopRange",      AutoNether::g_stopRange);
                getF(an, "maxHitRange",    AutoNether::g_maxHitRange);
                getF(an, "minMonsterHp",   AutoNether::g_minMonsterHp);
                getF(an, "flightScale",    AutoNether::g_flightScale);
                getB(an, "targetEnemies",  AutoNether::g_targetEnemies);
                getB(an, "targetTreasure", AutoNether::g_targetTreasure);
                getB(an, "targetGifts",    AutoNether::g_targetGifts);
                getB(an, "targetFloor",    AutoNether::g_targetFloor);

                AutoNether::g_netherOnly = false;
                getB(an, "priorityClosest",AutoNether::g_priorityClosest);
                getB(an, "antiSleep",      AutoNether::g_antiSleep);
                getB(an, "antiAfk",        AutoNether::g_antiAfk);
                int mm = (int)AutoNether::g_moveMethod;
                getI(an, "moveMethod", mm);
                AutoNether::g_moveMethod = (PathRenderer::MoveMethod)mm;
                getF(an, "moveSpeed",      AutoNether::g_moveSpeed);
                getF(an, "pfStopRange",    AutoNether::g_pfStopRange);
                getF(an, "bodyOffsetFrac", AutoNether::g_bodyOffsetFrac);
                getB(an, "pathGlow",       AutoNether::g_pathGlow);
                getB(an, "pathFlow",       AutoNether::g_pathFlow);
                getB(an, "pathPulse",      AutoNether::g_pathPulse);
                if (an.contains("pathOpts") && an["pathOpts"].is_object()) {
                    auto& po = an["pathOpts"];

                    getB(po, "airborneCheck",  AutoNether::g_pathOpts.airborneCheck);
                    getI(po, "maxAirborne",    AutoNether::g_pathOpts.maxAirborne);
                    getB(po, "chamfer",        AutoNether::g_pathOpts.chamfer);
                    getF(po, "chamferAmount",  AutoNether::g_pathOpts.chamferAmount);
                    getB(po, "walkInstakill",  AutoNether::g_pathOpts.walkInstakill);
                }
            }

            if (j.contains("antigrav") && j["antigrav"].is_object()) {
                auto& gv = j["antigrav"];
                getB(gv, "pulse", g_pfGravPulse);
                getI(gv, "onMs",  g_pfGravPulseOnMs);
                getI(gv, "offMs", g_pfGravPulseOffMs);
            }

            if (j.contains("automine") && j["automine"].is_object()) {
                auto& am = j["automine"];
                getB(am, "enabled",           AutoMine::g_enabled);
                getB(am, "autoMove",          AutoMine::g_autoMove);
                getB(am, "showPath",          AutoMine::g_showPath);
                getF(am, "hitInterval",       AutoMine::g_hitInterval);
                getF(am, "stopRange",         AutoMine::g_stopRange);
                getF(am, "flightScale",       AutoMine::g_flightScale);
                getB(am, "mineOnly",          AutoMine::g_mineOnly);
                getB(am, "targetGemstones",   AutoMine::g_targetGemstones);
                getB(am, "targetNuggets",     AutoMine::g_targetNuggets);
                getB(am, "targetCrystals",    AutoMine::g_targetCrystals);
                getB(am, "targetPots",        AutoMine::g_targetPots);
                getB(am, "targetCollect",     AutoMine::g_targetCollect);
                getB(am, "targetAI",          AutoMine::g_targetAI);
                getB(am, "targetAINearbyOnly", AutoMine::g_targetAINearbyOnly);
                getB(am, "targetExit",        AutoMine::g_targetExit);
                getB(am, "antiSleep",         AutoMine::g_antiSleep);
                getB(am, "antiAfk",           AutoMine::g_antiAfk);
                getB(am, "requirePickaxe",    AutoMine::g_requirePickaxe);
                getB(am, "autoRepairPickaxe", AutoMine::g_autoRepairPickaxe);
                getI(am, "repairThreshold",   AutoMine::g_repairThreshold);

                getB(am, "combatEnabled",     AutoMine::g_combatEnabled);
                getF(am, "combatCadence",     AutoMine::g_combatCadence);
                int mm = (int)AutoMine::g_moveMethod;
                getI(am, "moveMethod", mm);
                AutoMine::g_moveMethod = (PathRenderer::MoveMethod)mm;
                getF(am, "moveSpeed",         AutoMine::g_moveSpeed);
                getF(am, "pfStopRange",       AutoMine::g_pfStopRange);
                getF(am, "bodyOffsetFrac",    AutoMine::g_bodyOffsetFrac);
                getB(am, "pathGlow",          AutoMine::g_pathGlow);
                getB(am, "pathFlow",          AutoMine::g_pathFlow);
                getI(am, "weightUpPenaltySolid",   AutoMine::g_weights.upPenaltySolid);
                getI(am, "weightUnsupportedExtra", AutoMine::g_weights.unsupportedExtra);
                getB(am, "weightPreferStaircase",  AutoMine::g_weights.preferStaircase);

                if (am.contains("autoloop") && am["autoloop"].is_object()) {
                    auto& al = am["autoloop"];
                    getB(al, "enabled",          AutoMine::AutoLoop::g_enabled);
                    getB(al, "autoCombine",      AutoMine::AutoLoop::g_autoCombine);
                    getB(al, "autoEnter",        AutoMine::AutoLoop::g_autoEnter);
                    getI(al, "combineCadenceMs", AutoMine::AutoLoop::g_combineCadenceMs);
                    getB(al, "autoRejoin",       AutoMine::AutoLoop::g_autoRejoin);
                    getI(al, "rejoinDelayMs",    AutoMine::AutoLoop::g_rejoinDelayMs);
                    if (al.contains("levelEnabled") && al["levelEnabled"].is_array()) {
                        const auto& a = al["levelEnabled"];
                        for (size_t i = 0; i < a.size() && i < 5; ++i) {
                            if (a[i].is_number_integer())
                                AutoMine::AutoLoop::g_levelEnabled[i] = (a[i].get<int>() != 0);
                        }
                    }
                }
            }

            if (j.contains("bankbot") && j["bankbot"].is_object()) {
                auto& bb = j["bankbot"];
                getB(bb, "autoDeposit",      BankBot::g_autoDeposit);
                getI(bb, "depositCadenceMs", BankBot::g_depositCadenceMs);
                auto loadBoolArr = [](const nlohmann::json& src, bool* dst, int n) {
                    if (!src.is_array()) return;
                    for (size_t i = 0; i < src.size() && (int)i < n; ++i) {
                        if (src[i].is_number_integer())
                            dst[i] = (src[i].get<int>() != 0);
                    }
                };
                if (bb.contains("selNuggets"))   loadBoolArr(bb["selNuggets"],   BankBot::g_selNuggets,   5);
                if (bb.contains("selKeys"))      loadBoolArr(bb["selKeys"],      BankBot::g_selKeys,      4);
                if (bb.contains("selMaterials")) loadBoolArr(bb["selMaterials"], BankBot::g_selMaterials, 9);
                if (bb.contains("depositMatrix")) loadBoolArr(bb["depositMatrix"], &BankBot::g_depositMatrix[0][0], 25);
            }

            if (j.contains("features") && j["features"].is_object()) {
                auto& ft = j["features"];
                getB(ft, "godMode",           GodModeEnabled);
                getB(ft, "antiPoison",        g_antiPoison);
                getB(ft, "autoGiftBox",       g_autoGiftBox);
                getB(ft, "extraZoom",         g_extraZoom);
                getF(ft, "extraZoomMul",      g_extraZoomMul);
                getB(ft, "antiBounce",        AntiBounce);
                getB(ft, "antiBounceIgnoreMushrooms", AntiBounceIgnoreMushrooms);
                getB(ft, "antiFan",           antifan);
                getB(ft, "infiniteJetpack",   InfiniteJetpack);
                getB(ft, "antiCensor",        AntiCensor);
                getB(ft, "antiDarkness",      AntiDarkness);
                getB(ft, "disableEnemies",    DisableEnemies);
                getB(ft, "disableGassy",      DisableGassy);
                getB(ft, "noBlockKill",       noBlockKill);
                getB(ft, "unlimitedOxygen",   unlimitedOxygenEnabled);
                getB(ft, "unlockAllRecipes",  unlockAllRecipes);
                getB(ft, "placeSeedAnywhere", PlaceSeedAnywhere);
                getB(ft, "blockOnPlayer",     g_BlockOnPlayer);
                getB(ft, "fastHit",           fastHitEnabled);
                getB(ft, "extraFastHit",      ExtraFasthitEnabled);
                getB(ft, "jumpModeOverride",  EnableJumpModeOverride);
                int jm = (int)SelectedJumpMode;
                getI(ft, "selectedJumpMode", jm);
                SelectedJumpMode = (PlayerJumpMode)jm;
                getB(ft, "gravityOverride",   EnableGravityOverride);
                getF(ft, "runSpeedMult",      runSpeedMultiplier);
                getB(ft, "freecam",           FreecamEnabled);
                getF(ft, "freecamSpeed",      freecamSpeed);
                getB(ft, "uncapFps",          g_uncapFps);
                getI(ft, "targetFps",         g_targetFps);
                getB(ft, "showMinimap",       Minimap::showMinimap);
                getF(ft, "minimapZoom",       Minimap::minimapZoomAmount);
                getB(ft, "streamerMode",      g_streamerMode);

                getB(ft, "doorWalkThrough",   Door::g_walkThroughAll);
                getB(ft, "gmMasterSwap",      g_gmMasterSwap);
                getB(ft, "antiAcidWater",     g_antiAcidWater);
                getB(ft, "actkBypass",        g_actkBypass);
                getB(ft, "debugDamageLog",    g_debugDamageLog);
                getB(ft, "playerRunSpeedHack",g_playerRunSpeedHack);
                getF(ft, "playerRunSpeedMul", g_playerRunSpeedMul);
                getB(ft, "forceWeather",      g_forceWeather);
                int wt = (int)g_forcedWeatherType;
                getI(ft, "forcedWeatherType", wt);
                g_forcedWeatherType = (World::WeatherType)wt;
                getB(ft, "blockTooltip",      ::Minimap::g_blockTooltip);
            }

            if (j.contains("lighting") && j["lighting"].is_object()) {
                auto& lt = j["lighting"];
                getB(lt, "forceLighting", g_forceLighting);
                int lk = (int)g_forcedLightingType;
                getI(lt, "forcedType", lk);
                g_forcedLightingType = (World::LightingType)lk;
            }

            if (j.contains("gemseller") && j["gemseller"].is_object()) {
                auto& gs = j["gemseller"];
                getB(gs, "autoSell",      GemSeller::g_autoSell);
                getI(gs, "sellCadenceMs", GemSeller::g_sellCadenceMs);
                auto loadBoolArr = [](const nlohmann::json& src, bool* dst, int n) {
                    if (!src.is_array()) return;
                    for (size_t i = 0; i < src.size() && (int)i < n; ++i) {
                        if (src[i].is_number_integer())
                            dst[i] = (src[i].get<int>() != 0);
                    }
                };
                if (gs.contains("sellMatrix")) loadBoolArr(gs["sellMatrix"], &GemSeller::g_sellMatrix[0][0], 25);
            }

            if (j.contains("theme") && j["theme"].is_object()) {
                auto& th = j["theme"];
                int cs = (int)currentStyle;
                getI(th, "currentStyle", cs);
                if (cs < 0 || cs > (int)ThemeStyle::Custom) cs = 0;
                currentStyle = (ThemeStyle)cs;
                getB(th, "customInitialized", g_customThemeInitialized);
                if (th.contains("customColors") && th["customColors"].is_array()) {
                    const auto& arr = th["customColors"];
                    for (size_t i = 0; i < arr.size() && (int)i < ImGuiCol_COUNT; ++i) {
                        if (!arr[i].is_array() || arr[i].size() < 4) continue;
                        const auto& c = arr[i];
                        if (c[0].is_number()) g_customThemeColors[i].x = (float)c[0].get<double>();
                        if (c[1].is_number()) g_customThemeColors[i].y = (float)c[1].get<double>();
                        if (c[2].is_number()) g_customThemeColors[i].z = (float)c[2].get<double>();
                        if (c[3].is_number()) g_customThemeColors[i].w = (float)c[3].get<double>();
                    }
                    g_customThemeInitialized = true;
                }
            }

            auto getV4 = [](const nlohmann::json& o, const char* k, ImVec4& out) {
                if (!o.contains(k) || !o[k].is_array() || o[k].size() < 4) return;
                const auto& a = o[k];
                if (a[0].is_number()) out.x = (float)a[0].get<double>();
                if (a[1].is_number()) out.y = (float)a[1].get<double>();
                if (a[2].is_number()) out.z = (float)a[2].get<double>();
                if (a[3].is_number()) out.w = (float)a[3].get<double>();
            };
            if (j.contains("esp") && j["esp"].is_object()) {
                auto& esp = j["esp"];
                getB(esp, "master",                       ESP::bMasterSwitch);
                getB(esp, "debugInfo",                    ESP::bEnableDebugInfo);

                getB(esp, "playerBoxes",                  ESP::bEnablePlayerBoxes);
                getI(esp, "boxType",                      ESP::boxType);
                getB(esp, "boxFill",                      ESP::bBoxFill);
                getF(esp, "boxThickness",                 ESP::boxThickness);
                getV4(esp, "boxColor",                    ESP::boxColor);
                getV4(esp, "boxFillColor",                ESP::boxFillColor);
                getB(esp, "snaplines",                    ESP::bEnableSnaplines);
                getI(esp, "lineOrigin",                   ESP::lineOrigin);
                getF(esp, "lineThickness",                ESP::lineThickness);
                getV4(esp, "lineColor",                   ESP::lineColor);
                getB(esp, "playerHealth",                 ESP::bEnablePlayerHealthESP);
                getI(esp, "playerHealthStyle",            ESP::PlayerHealthStyle);
                getV4(esp, "healthColorHigh",             ESP::healthColorHigh);
                getV4(esp, "healthColorLow",              ESP::healthColorLow);

                getB(esp, "aiBoxes",                      ESP::bEnableAIBoxes);
                getI(esp, "aiBoxType",                    ESP::AIBoxType);
                getB(esp, "aiBoxFill",                    ESP::bAIBoxFill);
                getF(esp, "aiBoxThickness",               ESP::AIBoxThickness);
                getV4(esp, "aiBoxColor",                  ESP::AIBoxColor);
                getV4(esp, "aiBoxFillColor",              ESP::AIBoxFillColor);
                getB(esp, "aiLines",                      ESP::bEnableAILines);
                getI(esp, "aiLineOrigin",                 ESP::AILineOrigin);
                getF(esp, "aiLineThickness",              ESP::ailineThickness);
                getV4(esp, "aiLineColor",                 ESP::AIlineColor);

                getB(esp, "gemBoxes",                     ESP::bEnableGemstoneBoxes);
                getB(esp, "gemLines",                     ESP::bEnableGemstoneLines);
                getI(esp, "gemLineOrigin",                ESP::GemstoneLineOrigin);
                getF(esp, "gemSnapThickness",             ESP::gemSnapThickness);
                getV4(esp, "gemSnapColor",                ESP::gemSnapColor);
                getI(esp, "gemBoxType",                   ESP::gemBoxType);
                getF(esp, "gemBoxThickness",              ESP::gemBoxThickness);
                getB(esp, "gemBoxFill",                   ESP::bGemBoxFill);
                getV4(esp, "gemBoxColor",                 ESP::gemBoxColor);
                getV4(esp, "gemBoxFillColor",             ESP::gemBoxFillColor);

                getB(esp, "collectBoxes",                 ESP::bEnableCollectableBoxes);
                getB(esp, "collectLines",                 ESP::bEnableCollectableLines);
                getI(esp, "collectLineOrigin",            ESP::collectableLineOrigin);
                getF(esp, "collectLineThickness",         ESP::collectableLineThickness);
                getI(esp, "collectBoxType",               ESP::collectableBoxType);
                getF(esp, "collectBoxThickness",          ESP::collectableBoxThickness);
                getB(esp, "collectBoxFill",               ESP::bCollectableBoxFill);
                getV4(esp, "collectBoxColor",             ESP::collectableBoxColor);
                getV4(esp, "collectLineColor",            ESP::collectableLineColor);
                getV4(esp, "collectBoxFillColor",         ESP::collectableBoxFillColor);

                getB(esp, "lpBoxes",                      ESP::bEnableLocalPlayerBoxes);
                getI(esp, "lpBoxType",                    ESP::localPlayerBoxType);
                getB(esp, "lpBoxFill",                    ESP::bLocalPlayerBoxFill);
                getF(esp, "lpBoxThickness",               ESP::localPlayerBoxThickness);
                getV4(esp, "lpBoxFillColor",              ESP::localPlayerBoxFillColor);
                getB(esp, "lpHealth",                     ESP::bEnableLocalPlayerHealthESP);
                getI(esp, "lpHealthStyle",                ESP::localPlayerHealthStyle);
                getV4(esp, "lpHealthColorHigh",           ESP::localPlayerHealthColorHigh);
                getV4(esp, "lpHealthColorLow",            ESP::localPlayerHealthColorLow);

                getB(esp, "gbLines",                      ESP::bEnableGiftBoxLines);
                getI(esp, "gbLineOrigin",                 ESP::giftBoxLineOrigin);
                getF(esp, "gbLineThickness",              ESP::giftBoxLineThickness);
                getV4(esp, "gbLineColor",                 ESP::giftBoxLineColor);
                getB(esp, "gbBoxes",                      ESP::bEnableGiftBoxBoxes);
                getI(esp, "gbBoxType",                    ESP::giftBoxBoxType);
                getB(esp, "gbBoxFill",                    ESP::bGiftBoxBoxFill);
                getF(esp, "gbBoxThickness",               ESP::giftBoxBoxThickness);
                getV4(esp, "gbBoxColor",                  ESP::giftBoxBoxColor);
                getV4(esp, "gbBoxFillColor",              ESP::giftBoxBoxFillColor);

                getB(esp, "exLines",                      ESP::bEnableExitPortalLines);
                getI(esp, "exLineOrigin",                 ESP::exitPortalLineOrigin);
                getF(esp, "exLineThickness",              ESP::exitPortalLineThickness);
                getV4(esp, "exLineColor",                 ESP::exitPortalLineColor);
                getB(esp, "exBoxes",                      ESP::bEnableExitPortalBoxes);
                getI(esp, "exBoxType",                    ESP::exitPortalBoxType);
                getB(esp, "exBoxFill",                    ESP::bExitPortalBoxFill);
                getF(esp, "exBoxThickness",               ESP::exitPortalBoxThickness);
                getV4(esp, "exBoxColor",                  ESP::exitPortalBoxColor);
                getV4(esp, "exBoxFillColor",              ESP::exitPortalBoxFillColor);

                getB(esp, "lcBoxes",                      ESP::bEnableLightCrystalBoxes);
                getB(esp, "lcLines",                      ESP::bEnableLightCrystalLines);
                getI(esp, "lcLineOrigin",                 ESP::LightCrystalLineOrigin);
                getF(esp, "lcLineThickness",              ESP::lightCrystalLineThickness);
                getV4(esp, "lcLineColor",                 ESP::lightCrystalLineColor);
                getI(esp, "lcBoxType",                    ESP::lightCrystalBoxType);
                getF(esp, "lcBoxThickness",               ESP::lightCrystalBoxThickness);
                getB(esp, "lcBoxFill",                    ESP::bLightCrystalBoxFill);
                getV4(esp, "lcBoxColor",                  ESP::lightCrystalBoxColor);
                getV4(esp, "lcBoxFillColor",              ESP::lightCrystalBoxFillColor);

                getB(esp, "potBoxes",                     ESP::bEnablePotBoxes);
                getB(esp, "potLines",                     ESP::bEnablePotLines);
                getI(esp, "potLineOrigin",                ESP::PotLineOrigin);
                getF(esp, "potLineThickness",             ESP::potLineThickness);
                getV4(esp, "potLineColor",                ESP::potLineColor);
                getI(esp, "potBoxType",                   ESP::potBoxType);
                getF(esp, "potBoxThickness",              ESP::potBoxThickness);
                getB(esp, "potBoxFill",                   ESP::bPotBoxFill);
                getV4(esp, "potBoxColor",                 ESP::potBoxColor);
                getV4(esp, "potBoxFillColor",             ESP::potBoxFillColor);

                getB(esp, "treBoxes",                     ESP::bEnableTreasureBoxes);
                getB(esp, "treLines",                     ESP::bEnableTreasureLines);
                getI(esp, "treLineOrigin",                ESP::TreasureLineOrigin);
                getF(esp, "treLineThickness",             ESP::treasureLineThickness);
                getI(esp, "treBoxType",                   ESP::treasureBoxType);
                getF(esp, "treBoxThickness",              ESP::treasureBoxThickness);
                getB(esp, "treBoxFill",                   ESP::bTreasureBoxFill);
                getV4(esp, "treBoxColor",                 ESP::treasureBoxColor);
                getV4(esp, "treBoxFillColor",             ESP::treasureBoxFillColor);

                getF(esp, "aiBoxHeightMul",               ESP::AI_Box_Height_Multiplier);
                getF(esp, "aiBoxWidthMul",                ESP::AI_Box_Width_Multiplier);
                getB(esp, "pathfinder",                   ESP::bEnablePathfinder);
            }

            if (j.contains("fishbot") && j["fishbot"].is_object()) {
                auto& fb = j["fishbot"];
                getB(fb, "enabled",            FishBot::g_enabled);
                getB(fb, "autoLand",           FishBot::g_autoLand);
                getB(fb, "autoRecast",         FishBot::g_autoRecast);
                getB(fb, "legitMode",          FishBot::g_legitMode);
                getB(fb, "antiSleep",          FishBot::g_antiSleep);
                getB(fb, "antiAfk",            FishBot::g_antiAfk);
                getF(fb, "recastDelayMinSec",  FishBot::g_recastDelayMinSec);
                getF(fb, "recastDelayMaxSec",  FishBot::g_recastDelayMaxSec);
                getF(fb, "hookReactionMinMs",  FishBot::g_hookReactionMinMs);
                getF(fb, "hookReactionMaxMs",  FishBot::g_hookReactionMaxMs);
                getF(fb, "legitPauseDelayMin", FishBot::g_legitPauseDelayMin);
                getF(fb, "legitPauseDelayMax", FishBot::g_legitPauseDelayMax);
                getF(fb, "legitPauseDurMin",   FishBot::g_legitPauseDurMin);
                getF(fb, "legitPauseDurMax",   FishBot::g_legitPauseDurMax);
                getB(fb, "verboseMinigame",    FishBot::g_verboseMinigame);
                getF(fb, "speedMult",          FishBot::g_speedMult);
            }

            if (j.contains("pnb") && j["pnb"].is_object()) {
                auto& pn = j["pnb"];
                getB(pn, "autoCollect", Pnb::g_autoCollect);
                getI(pn, "blockType",   Pnb::g_blockType);
                getI(pn, "itemType",    Pnb::g_itemType);
                if (pn.contains("offsets") && pn["offsets"].is_array()) {
                    const auto& a = pn["offsets"];
                    for (size_t i = 0; i < a.size() && i < 25; ++i)
                        if (a[i].is_number_integer())
                            Pnb::g_offsets[i / 5][i % 5] = (a[i].get<int>() != 0);
                }
            }

            if (j.contains("minimap") && j["minimap"].is_object()) {
                auto& mm = j["minimap"];
                getF(mm, "tooltipFontScale", Minimap::g_tooltipFontScale);
            }

            return true;
        } catch (...) { return false; }
    }

    inline bool SaveAs(const std::filesystem::path& path) {
        try {
            std::filesystem::create_directories(path.parent_path());
            std::ofstream f(path, std::ios::trunc);
            if (!f) {
                g_lastSaveOk = false;
                std::cout << XS("[Config] SaveAs FAILED — cannot open ")
                          << path.string() << "\n";
                return false;
            }
            f << BuildConfig().dump(2);
            bool ok = f.good();
            g_lastSaveOk   = ok;
            g_lastSaveTime = std::chrono::steady_clock::now();
            g_lastSavePath = path;
            std::cout << XS("[Config] SaveAs ") << (ok ? "OK" : "FAILED")
                      << XS(" -> ") << path.string() << "\n";
            return ok;
        } catch (const std::exception& e) {
            std::cout << XS("[Config] SaveAs EXCEPTION: ") << e.what() << "\n";
            g_lastSaveOk = false;
            return false;
        } catch (...) {
            std::cout << XS("[Config] SaveAs EXCEPTION (unknown)\n");
            g_lastSaveOk = false;
            return false;
        }
    }

    inline std::filesystem::path g_loadOverride{};

    inline bool LoadFrom(const std::filesystem::path& path) {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec)) {
            std::cout << XS("[Config] LoadFrom: file not found ") << path.string() << "\n";
            return false;
        }

        try {
            std::ifstream f(path);
            if (!f) return false;
            nlohmann::json j;
            f >> j;

            auto getB = [](const nlohmann::json& o, const char* k, bool& out) {
                if (o.contains(k) && o[k].is_boolean()) out = o[k].get<bool>();
            };
            auto getI = [](const nlohmann::json& o, const char* k, int& out) {
                if (o.contains(k) && o[k].is_number_integer()) out = o[k].get<int>();
            };
            auto getF = [](const nlohmann::json& o, const char* k, float& out) {
                if (o.contains(k) && o[k].is_number()) out = (float)o[k].get<double>();
            };

            auto autoPath = GetConfigPath();
            std::filesystem::create_directories(autoPath.parent_path(), ec);
            {
                std::ofstream out(autoPath, std::ios::trunc);
                if (!out) return false;
                out << j.dump(2);
            }

            g_loaded = false;
            bool ok = Load();
            std::cout << XS("[Config] LoadFrom ") << (ok ? "OK" : "PARSE-ERR")
                      << XS(" <- ") << path.string() << "\n";
            return ok;
        } catch (const std::exception& e) {
            std::cout << XS("[Config] LoadFrom EXCEPTION: ") << e.what() << "\n";
            return false;
        } catch (...) {
            std::cout << XS("[Config] LoadFrom EXCEPTION (unknown)\n");
            return false;
        }
    }

    inline std::filesystem::path PickSavePath(HWND owner) {
        char buf[MAX_PATH] = {};

        SYSTEMTIME st; GetLocalTime(&st);
        std::snprintf(buf, sizeof(buf),
                      "zwspark-config-%04d%02d%02d-%02d%02d.json",
                      st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner   = owner;
        ofn.lpstrFilter = "ZwSpark Config (*.json)\0*.json\0All Files\0*.*\0";
        ofn.lpstrFile   = buf;
        ofn.nMaxFile    = MAX_PATH;
        ofn.lpstrTitle  = "Export ZwSpark Config";
        ofn.lpstrDefExt = "json";

        std::string initDir = GetConfigPath().parent_path().string();
        ofn.lpstrInitialDir = initDir.c_str();
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
        if (!GetSaveFileNameA(&ofn)) return {};
        return std::filesystem::path(buf);
    }

    inline std::filesystem::path PickOpenPath(HWND owner) {
        char buf[MAX_PATH] = {};
        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner   = owner;
        ofn.lpstrFilter = "ZwSpark Config (*.json)\0*.json\0All Files\0*.*\0";
        ofn.lpstrFile   = buf;
        ofn.nMaxFile    = MAX_PATH;
        ofn.lpstrTitle  = "Import ZwSpark Config";
        ofn.lpstrDefExt = "json";
        std::string initDir = GetConfigPath().parent_path().string();
        ofn.lpstrInitialDir = initDir.c_str();
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        if (!GetOpenFileNameA(&ofn)) return {};
        return std::filesystem::path(buf);
    }

}

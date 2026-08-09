

#include "../Internal.h"
#include "../Tokens.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../widgets/Primitives.h"
#include "../widgets/Row.h"
#include <cstdio>

namespace ZmodUI {

static bool RailButton(const WRow::RowResult& r, const char* hit_id,
                       const char* label, ImU32 text_col, ImU32 hover_col)
{
    Ctx& c = GetCtx();
    ImFont* fb = Tokens::Type::Body();
    ImVec2 ts = Draw::TextSize(fb, label);
    float bw = ts.x + 24.f;
    float bh = (fb ? fb->FontSize : 13) + 10.f;
    ImVec2 ba = ImVec2(r.end_b.x - bw, (r.a.y + r.b.y) * 0.5f - bh * 0.5f);
    ImVec2 bb = ImVec2(ba.x + bw, ba.y + bh);
    Layout::Interact bi = Layout::RegisterHit(hit_id, ba, bb);
    ImU32 bg = bi.held    ? Tokens::Color::BgPressed()
            : bi.hovered ? hover_col
                         : Tokens::Color::BgCard();
    Draw::RectFilledRounded(c.dl, ba, bb, bg, 6.f);
    Draw::RectStrokeRounded(c.dl, ba, bb, Tokens::Color::Line(), 6.f, 1.f);
    Draw::TextL(c.dl, fb,
                ImVec2(ba.x + (bw - ts.x) * 0.5f, ba.y + (bh - ts.y) * 0.5f),
                text_col, label);
    return bi.clicked;
}

static void IkPickerRow(State& s, int target, const char* label,
                        int blockType, int itemType, int qty)
{
    WRow::Opts o;
    o.icon_id = (target == 0) ? "reload-orange" : "spark";
    o.label   = label;
    char desc[64];
    if (blockType != 0 || itemType != 0) {
        ImFormatString(desc, sizeof(desc),
                       "Selected: bt=%d  it=%d  x%d",
                       blockType, itemType, qty);
    } else {
        ImFormatString(desc, sizeof(desc), "No item selected.");
    }
    o.desc = desc;
    WRow::RowResult r = WRow::Begin(o);

    char hit[32];
    ImFormatString(hit, sizeof(hit), "dbg_pick_%d", target);
    if (RailButton(r, hit, "Pick\xE2\x80\xA6",
                   Tokens::Color::Text(), Tokens::Color::BgHover())) {
        s.dbg_inv_picker_target = target;
        if (s.on_dbg_refresh_inventory) s.on_dbg_refresh_inventory();
        OpenDrawer("dbg-inv-pick");
    }
}

void RailDebug(State& s) {
    PageHead("Debug",
             "Manual tester for migrated OutgoingMessages helpers. "
             "Each button fires one packet through the real IL2CPP API.");

    GroupTitle("Traces");
    BeginGroup();
        ToggleRow("eye", "Verbose minigame trace",
                  "FishBot: logs fishPos/targetPos/decision every frame during the reel.",
                  &s.af_verbose_minigame);
    EndGroup();

    GroupTitle("Hit Block (HB)");
    BeginGroup();
        InputRow("Tile X", s.dbg_tile_x_buf, sizeof(s.dbg_tile_x_buf), "e.g. 42");
        InputRow("Tile Y", s.dbg_tile_y_buf, sizeof(s.dbg_tile_y_buf), "e.g. 17");

        WRow::Opts oFill; oFill.icon_id = "magnet-green";
        oFill.label = "Fill from player position";
        oFill.desc  = "Copy current player tile into X/Y above.";
        WRow::RowResult rFill = WRow::Begin(oFill);
        if (RailButton(rFill, "dbg_fill_pos", "Fill",
                       Tokens::Color::Text(), Tokens::Color::BgHover())) {
            if (s.on_dbg_fill_player_pos) s.on_dbg_fill_player_pos();
        }

        WRow::Opts oHit; oHit.icon_id = "pickaxe-indigo";
        oHit.label = "Send SendHitBlockMessage";
        oHit.desc  = "OutgoingMessages.SendHitBlockMessage(mp, DateTime.Now, false).";
        WRow::RowResult rHit = WRow::Begin(oHit);
        if (RailButton(rHit, "dbg_hb_btn", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_hit_block) s.on_dbg_hit_block();
        }
    EndGroup();

    GroupTitle("ConvertItems (CI)");
    BeginGroup();
        IkPickerRow(s, 0, "Inventory item for CI",
                    s.dbg_ik_block, s.dbg_ik_item, s.dbg_ik_qty);

        WRow::Opts oCI; oCI.icon_id = "reload-orange";
        oCI.label = "Send ConvertItems (CI)";
        oCI.desc  = "OutgoingMessages.ConvertItems({blockType, itemType}).";
        WRow::RowResult rCI = WRow::Begin(oCI);
        if (RailButton(rCI, "dbg_ci_btn", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_convert_items) s.on_dbg_convert_items();
        }
    EndGroup();

    GroupTitle("Pickaxe Repair");
    BeginGroup();
        IkPickerRow(s, 1, "Inventory item for Repair",
                    s.dbg_repair_block, s.dbg_repair_item, s.dbg_repair_qty);

        WRow::Opts oRep; oRep.icon_id = "spark";
        oRep.label = "Send MiningPickaxeRepairing";
        oRep.desc  = "OutgoingMessages.MiningPickaxeRepairing({pickBT, pickIT}).";
        WRow::RowResult rRep = WRow::Begin(oRep);
        if (RailButton(rRep, "dbg_rep_btn", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_pickaxe_repair) s.on_dbg_pickaxe_repair();
        }

        WRow::Opts oEq; oEq.icon_id = "spark";
        oEq.label = "Print equipped item info";
        oEq.desc  = "Logs [EquipInfo]: topArmBlockType, name, durability, "
                    "computedMax, progress, isPickaxe, wouldRepair.";
        WRow::RowResult rEq = WRow::Begin(oEq);
        if (RailButton(rEq, "dbg_equip_btn", "Print",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_equipped_info) s.on_dbg_equipped_info();
        }
    EndGroup();

    GroupTitle("GemSeller — Recycle All Gemstones");
    BeginGroup();
        WRow::Opts oRA; oRA.icon_id = "diamond-indigo";
        oRA.label = "Sell every gemstone in inventory";
        oRA.desc  = "Sweeps kGems[5][5], batches into one RecycleAllMiningGem "
                    "send. State mirror fires on server reply (ReplyDispatch).";
        WRow::RowResult rRA = WRow::Begin(oRA);
        if (RailButton(rRA, "dbg_ra_btn", "Sell all",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_recycle_all_gems) s.on_dbg_recycle_all_gems();
        }
    EndGroup();

    GroupTitle("Pre-existing safe helpers (sanity check)");
    BeginGroup();
        SliderRow("AI enemy id",     "id", &s.dbg_ai_id,      0, 1000000);
        WRow::Opts oAI; oAI.icon_id = "aim";
        oAI.label = "Send HitAIEnemy";
        oAI.desc  = "OutgoingMessages.SendHitAIEnemyMessage(mp from X/Y above, aiId, -1).";
        WRow::RowResult rAI = WRow::Begin(oAI);
        if (RailButton(rAI, "dbg_ai_btn", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_hit_ai) s.on_dbg_hit_ai();
        }

        SliderRow("Collectable id",  "id", &s.dbg_collect_id, 0, 1000000);
        WRow::Opts oCC; oCC.icon_id = "magnet-green";
        oCC.label = "Send CollectCollectable";
        oCC.desc  = "OutgoingMessages.SendCollectCollectableMessage(id).";
        WRow::RowResult rCC = WRow::Begin(oCC);
        if (RailButton(rCC, "dbg_cc_btn", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_collect_collectable) s.on_dbg_collect_collectable();
        }
    EndGroup();

    GroupTitle("AGI (force inventory resync, helper)");
    BeginGroup();
        WRow::Opts oAGId; oAGId.icon_id = "reload-orange";
        oAGId.label = "Send AGI dummy (helper)";
        oAGId.desc  = "SendAdjustGiftBoxAndInventoryMessage((0,0), null, NoTool).";
        WRow::RowResult rAGId = WRow::Begin(oAGId);
        if (RailButton(rAGId, "dbg_agi_helper_dummy", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_agi_helper_dummy) s.on_dbg_agi_helper_dummy();
        }

        WRow::Opts oAGIr; oAGIr.icon_id = "reload-orange";
        oAGIr.label = "Send AGI real (helper)";
        oAGIr.desc  = "SendAdjustGiftBoxAndInventoryMessage(playerTile, null, NoTool).";
        WRow::RowResult rAGIr = WRow::Begin(oAGIr);
        if (RailButton(rAGIr, "dbg_agi_helper_real", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_agi_helper_real) s.on_dbg_agi_helper_real();
        }

        WRow::Opts oGID; oGID.icon_id = "reload-orange";
        oGID.label = "Send GetInventoryData (picked IK)";
        oGID.desc  = "SendGetInventoryDataMessage(picked IK). Uses CI picker IK; "
                     "{0,0} if nothing picked. No WorldItemBase null-deref risk.";
        WRow::RowResult rGID = WRow::Begin(oGID);
        if (RailButton(rGID, "dbg_get_inv_data", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_get_inventory_data) s.on_dbg_get_inventory_data();
        }

        WRow::Opts oRAW; oRAW.icon_id = "reload-orange";
        oRAW.label = "Send raw AGI (auto-discovered cipher)";
        oRAW.desc  = "Raw BSON {ID:<cached cipher>, PT:0}. Trigger natural AGI "
                     "first so the outbound observer can capture the rotated ID.";
        WRow::RowResult rRAW = WRow::Begin(oRAW);
        if (RailButton(rRAW, "dbg_agi_raw_disc", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_agi_raw_discovered) s.on_dbg_agi_raw_discovered();
        }

        WRow::Opts oFIGH; oFIGH.icon_id = "reload-orange";
        oFIGH.label = "Send raw AGI (cipher='figh')";
        oFIGH.desc  = "Raw BSON {ID:'figh', PT:0}. External tip — bypass "
                      "discovery, fire immediately.";
        WRow::RowResult rFIGH = WRow::Begin(oFIGH);
        if (RailButton(rFIGH, "dbg_agi_raw_figh", "Fire",
                       Tokens::Color::Accent(), Tokens::Color::BgHover())) {
            if (s.on_dbg_agi_raw_figh) s.on_dbg_agi_raw_figh();
        }
    EndGroup();

    GroupTitle("Legacy raw BSON (deprecated)");
    BeginGroup();
        WRow::Opts oAGI; oAGI.icon_id = "x-gray";
        oAGI.label = "Send raw AGI ping (bypass helpers)";
        oAGI.desc  = "Dispatches {\"ID\":\"AGI\",\"PT\":0} via oAddOneMessageToList. "
                     "Now redundant — game auto-syncs PlayerData on helper replies.";
        WRow::RowResult rAGI = WRow::Begin(oAGI);
        if (RailButton(rAGI, "dbg_agi_btn", "Fire",
                       Tokens::Color::Red(), IM_COL32(255, 59, 48, 30))) {
            if (s.on_dbg_inventory_refresh_legacy) s.on_dbg_inventory_refresh_legacy();
        }
    EndGroup();

    GroupTitle("Action log");
    BeginGroup();

        TextareaFullWidth(s.dbg_log, sizeof(s.dbg_log),
                          "Debug actions logged here.", 180.f);
        if (ButtonFullWidth("Clear log", 32.f)) {
            if (s.on_dbg_clear_log) s.on_dbg_clear_log();
        }
    EndGroup();

    HelpText("All buttons fire one packet per click. SEH-guarded; a faulty "
             "helper pointer logs a line and returns without crashing.");
}

}

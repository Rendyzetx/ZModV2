#pragma once
#include "main.h"
#include "PathFinding.h"
#include <chrono>
#include <mutex>
#include <string>

namespace Pnb {

    inline bool NavigateTo(Vector2i goalTile) {
        if (!PathRenderer::oGetTransform || !PathRenderer::oGetPosition || !g_WorldController)
            return false;
        if (!PathRenderer::g_originCached)
            PathRenderer::CacheWorldOrigin_Safe(g_WorldController);

        Vector2i mp;
        if (GetPlayerMapPoint(mp)) {
            int mdx = mp.x - goalTile.x; if (mdx < 0) mdx = -mdx;
            int mdy = mp.y - goalTile.y; if (mdy < 0) mdy = -mdy;
            if (mdx <= 1 && mdy <= 1) { PathFinder_ClearInputs(); return true; }
        }

        void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
        if (!lt) return false;
        Vector3 here = PathRenderer::oGetPosition(lt);
        const float tileW = (PathRenderer::g_tileSize.x > 0.001f) ? PathRenderer::g_tileSize.x : 0.32f;
        Vector2i cur = PathRenderer::WorldToTile(here.x, here.y + tileW * 0.5f, tileW);

        void* worldObj = g_off_WCWorld
            ? *(void**)((uintptr_t)g_WorldController + g_off_WCWorld) : nullptr;
        if (!worldObj) return false;

        PathRenderer::PathOptions opt;
        PathRenderer::PathResult res = PathRenderer::BuildPath(
            g_WorldController, worldObj, cur, goalTile, opt);
        if (res.status != PathRenderer::PathStatus::Success || res.tiles.size() < 2) {
            PathFinder_ClearInputs();
            return false;
        }
        Vector3 wp = PathRenderer::TileToWorldCenter(res.tiles[1].x, res.tiles[1].y, here.z, tileW);
        float dx = wp.x - here.x, dy = wp.y - here.y;
        const float DEAD = 0.04f;
        PathFinder_WriteInputs(dx < -DEAD, dx > DEAD, dy > DEAD);
        return false;
    }

    inline bool CollectVisited(Vector2i p) {
        for (const auto& v : g_collectVisited)
            if (v.x == p.x && v.y == p.y) return true;
        return false;
    }

    inline bool NearestDrop(Vector2i& out) {
        bool found = false; int best = 99999;
        std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
        for (const auto& fc : g_floorCollectables) {
            int dx = fc.mapPoint.x - g_anchor.x; if (dx < 0) dx = -dx;
            int dy = fc.mapPoint.y - g_anchor.y; if (dy < 0) dy = -dy;
            int cheb = dx > dy ? dx : dy;
            if (cheb > 3) continue;
            if (CollectVisited(fc.mapPoint)) continue;
            if (cheb < best) { best = cheb; out = fc.mapPoint; found = true; }
        }
        return found;
    }

    inline void Tick(float) {
        if (!g_enabled || !g_LocalPlayer || !g_IsInWorld) {
            if (g_phase != Phase::Idle) Stop();
            return;
        }

        Vector2i cur;
        if (!GetPlayerMapPoint(cur)) return;

        std::string wn = GetCurrentWorldName();
        if (g_hasAnchor) {

            if (wn != g_anchorWorld) {
                if (g_phase != Phase::Idle) {
                    g_placedTiles.clear(); g_targets.clear();
                    g_idx = 0; g_hits = 0; g_placeAttempts = 0;
                    g_resumePhase = Phase::Idle;
                    g_phase = Phase::Idle;
                }
                return;
            }
        } else if (!g_anchorWorld.empty() && wn != g_anchorWorld) {

            g_anchor = { -1, -1 }; g_anchorWorld.clear();
            g_placedTiles.clear(); g_targets.clear();
            g_idx = 0; g_hits = 0; g_placeAttempts = 0;
            g_resumePhase = Phase::Idle;
            g_phase = Phase::Idle;
            return;
        }

        auto now = std::chrono::steady_clock::now();

        bool anchored = (g_anchor.x >= 0);
        if (anchored && g_phase != Phase::Collecting && g_phase != Phase::Idle) {
            int adx = cur.x - g_anchor.x; if (adx < 0) adx = -adx;
            int ady = cur.y - g_anchor.y; if (ady < 0) ady = -ady;
            if (adx > 1 || ady > 1) {
                NavigateTo(g_anchor);
                if (g_phase != Phase::Cooldown && g_phase != Phase::ToAnchor) {
                    g_resumePhase = g_phase;
                    g_phase = Phase::ToAnchor;
                }
                return;
            }
        }
        if (g_phase == Phase::ToAnchor) g_phase = g_resumePhase;

        if (g_phase == Phase::Idle || (g_phase == Phase::Cooldown && now >= g_nextAction)) {
            int bt = 0, it = 0;
            if (!CurrentSelection(bt, it) || bt <= 0 || it != 0) return;
            g_blockType = bt; g_itemType = it;
            if (!g_hasAnchor && g_anchor.x < 0) { g_anchor = cur; g_anchorWorld = wn; }
            BuildTargets();
            g_placedTiles.clear();
            g_idx = 0; g_hits = 0; g_placeAttempts = 0;
            g_phase = Phase::Placing;
            g_nextAction = now;
        }

        if (g_blockType <= 0) return;

        if (g_phase == Phase::Placing) {
            if (now < g_nextAction) return;
            if (g_idx >= g_targets.size() || SelectedBlockQty() <= 0) {
                SortBreakTargets();
                g_idx = 0; g_hits = 0;
                g_phase = Phase::Breaking;
                return;
            }
            Vector2i t = g_targets[g_idx];
            if ((int)GetForegroundBlockAt(t) != 0) { ++g_idx; return; }
            _Place(t);
            g_curTarget = t;
            g_placeAttempts = 1;
            g_phase = Phase::WaitPlace;
            g_nextAction = now + std::chrono::milliseconds(g_placeRetryMs);
            return;
        }

        if (g_phase == Phase::WaitPlace) {
            int bt = (int)GetForegroundBlockAt(g_curTarget);
            if (bt == g_blockType) {
                g_placedTiles.push_back(g_curTarget);
                ++g_placed; ++g_idx;
                g_phase = Phase::Placing;
                g_nextAction = now + Jitter(g_placeDelayMs, 40);
                return;
            }
            if (bt != 0) {
                ++g_idx;
                g_phase = Phase::Placing;
                g_nextAction = now + Jitter(g_placeDelayMs, 40);
                return;
            }
            if (now >= g_nextAction) {
                if (g_placeAttempts < g_maxPlaceTries && SelectedBlockQty() > 0) {
                    _Place(g_curTarget);
                    ++g_placeAttempts;
                    g_nextAction = now + std::chrono::milliseconds(g_placeRetryMs);
                } else {
                    ++g_idx;
                    g_phase = Phase::Placing;
                    g_nextAction = now + Jitter(g_placeDelayMs, 40);
                }
            }
            return;
        }

        if (g_phase == Phase::Breaking) {
            if (now < g_nextAction) return;
            if (g_idx >= g_placedTiles.size()) {
                ++g_cycles;
                if (g_autoCollect && g_collectEveryN > 0 && (g_cycles % g_collectEveryN) == 0) {
                    g_collectVisited.clear();
                    g_lastCollectValid = false;
                    g_phase = Phase::Collecting;
                    g_collectDeadline = now + std::chrono::seconds(5);
                } else {
                    g_phase = Phase::Cooldown;
                    g_nextAction = now + Jitter(g_cycleDelayMs, 60);
                }
                return;
            }
            Vector2i t = g_placedTiles[g_idx];
            int bt = (int)GetForegroundBlockAt(t);
            if (bt == 0) { ++g_idx; g_hits = 0; return; }
            if (bt != g_blockType) { ++g_idx; g_hits = 0; return; }
            _Break(t);
            g_curTarget = t;
            g_hits = 1;
            g_phase = Phase::WaitBreak;
            g_nextAction = now + std::chrono::milliseconds(g_breakDelayMs);
            return;
        }

        if (g_phase == Phase::WaitBreak) {
            int bt = (int)GetForegroundBlockAt(g_curTarget);
            if (bt == 0) {
                ++g_broken; ++g_idx; g_hits = 0;
                g_phase = Phase::Breaking;
                g_nextAction = now + Jitter(g_breakDelayMs, 30);
                return;
            }
            if (bt != g_blockType) {
                ++g_idx; g_hits = 0;
                g_phase = Phase::Breaking;
                g_nextAction = now + Jitter(g_breakDelayMs, 30);
                return;
            }
            if (now >= g_nextAction) {
                if (g_hits < g_maxHits) {
                    _Break(g_curTarget);
                    ++g_hits;
                    g_nextAction = now + std::chrono::milliseconds(g_breakDelayMs);
                } else {
                    ++g_idx; g_hits = 0;
                    g_phase = Phase::Breaking;
                    g_nextAction = now + std::chrono::milliseconds(g_breakDelayMs);
                }
            }
            return;
        }

        if (g_phase == Phase::Collecting) {
            if (now > g_collectDeadline) {
                PathFinder_ClearInputs();
                g_collectVisited.clear(); g_lastCollectValid = false;
                g_phase = Phase::Cooldown;
                g_nextAction = now + Jitter(g_cycleDelayMs, 60);
                return;
            }
            Vector2i drop;
            if (!NearestDrop(drop)) {
                PathFinder_ClearInputs();
                g_collectVisited.clear(); g_lastCollectValid = false;
                g_phase = Phase::Cooldown;
                g_nextAction = now + Jitter(g_cycleDelayMs, 60);
                return;
            }
            if (NavigateTo(drop)) {
                if (!(g_lastCollectValid && g_lastCollectTile.x == drop.x && g_lastCollectTile.y == drop.y))
                    ++g_collected;
                g_lastCollectTile = drop; g_lastCollectValid = true;
                g_collectVisited.push_back(drop);
            }
            return;
        }
    }
}

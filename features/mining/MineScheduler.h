#pragma once
#include "PathFinding.h"

#include "includes.h"
#include "main.h"
#include "json.hpp"
#include <vector>
#include <mutex>
#include <string>
#include <chrono>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <random>
#include <algorithm>

namespace MineScheduler {

    enum class TargetKind { GemOre, Nugget, Crystal, Drop };
    struct Target {
        Vector2i  mapPoint;
        TargetKind kind;
        int       collectId = -1;
    };

    inline std::vector<Target>   g_sequence;
    inline std::mutex            g_seqMutex;
    inline Vector2i              g_seqStart{ 0, 0 };
    inline Vector2i              g_exitTile{ 0, 0 };
    inline bool                  g_haveExit = false;
    inline std::string           g_status = "Idle";
    inline int                   g_lastChebDist = 0;
    inline int                   g_lastNNDist   = 0;
    inline int                   g_lastFinalDist = 0;
    inline double                g_lastComputeMs = 0.0;
    inline int                   g_lastTargetCount = 0;
    inline int                   g_lastGemCount = 0;
    inline int                   g_lastNuggetCount = 0;

    inline std::vector<Target>   g_lastScannedTargets;

    inline bool                  g_sessionActive = false;
    inline void*                 g_sessionWorldObj = nullptr;

    inline bool                  g_sessionScanGems    = true;
    inline bool                  g_sessionScanNuggets = true;

    inline int Cheb(Vector2i a, Vector2i b) {
        int dx = a.x - b.x; if (dx < 0) dx = -dx;
        int dy = a.y - b.y; if (dy < 0) dy = -dy;
        return (dx > dy) ? dx : dy;
    }

    static int TourLen(const std::vector<Target>& tour, Vector2i start,
                       Vector2i exit, bool haveExit) {
        if (tour.empty()) return haveExit ? Cheb(start, exit) : 0;
        int total = Cheb(start, tour.front().mapPoint);
        for (size_t i = 1; i < tour.size(); ++i)
            total += Cheb(tour[i - 1].mapPoint, tour[i].mapPoint);
        if (haveExit) total += Cheb(tour.back().mapPoint, exit);
        return total;
    }

    static bool ScanTargets(std::vector<Target>& out,
                            bool scanGems = true, bool scanNuggets = true)
    {
        out.clear();
        g_haveExit = false;
        if (!g_WorldController) {
            g_status = "Scan failed: no WorldController.";
            return false;
        }
        uintptr_t worldObject = *(uintptr_t*)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (!worldObject) { g_status = "Scan failed: no World object."; return false; }

        struct V2 { int x, y; };
        V2 sz = *(V2*)(worldObject + g_off_WorldSize);
        if (sz.x <= 1 || sz.x > 1000 || sz.y <= 1 || sz.y > 1000) {
            g_status = "Scan failed: bad worldSize " + std::to_string(sz.x) + "x" + std::to_string(sz.y);
            return false;
        }

        uintptr_t pBlockArrayObject = *(uintptr_t*)(worldObject + g_off_WorldBlockLayer);
        if (!pBlockArrayObject) { g_status = "Scan failed: no blockLayer."; return false; }
        uintptr_t* pRows = (uintptr_t*)(pBlockArrayObject + 0x20);

        int gemCount = 0;

        Vector2i exitCandidate{ -1, -1 };
        for (int x = 0; x < sz.x; ++x) {
            uintptr_t pRowObject = pRows[x];
            if (!pRowObject) continue;
            char* pRowData = (char*)(pRowObject + 0x20);
            for (int y = 0; y < sz.y; ++y) {
                uintptr_t structAddr = (uintptr_t)(pRowData + (y * 0x28));
                int bt = *(int*)structAddr;
                if (scanGems && IsGemstone((World::BlockType)bt)) {
                    out.push_back({ Vector2i{ x, y }, TargetKind::GemOre });
                    ++gemCount;
                }
                if (IsExitPortal((World::BlockType)bt)) {

                    if (exitCandidate.x < 0 || y < exitCandidate.y) {
                        exitCandidate = Vector2i{ x, y };
                    }
                }
            }
        }
        g_lastGemCount = gemCount;

        int nuggetCount = 0;
        if (scanNuggets) {
            std::vector<Vector2i> nuggetPts;
            {
                std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
                for (const auto& fc : g_floorCollectables) {
                    if (!IsNugget(fc.blockType)) continue;
                    nuggetPts.push_back(fc.mapPoint);
                }
            }
            std::sort(nuggetPts.begin(), nuggetPts.end(),
                [](const Vector2i& a, const Vector2i& b) {
                    return a.x != b.x ? a.x < b.x : a.y < b.y;
                });
            for (const auto& mp : nuggetPts) {
                out.push_back({ mp, TargetKind::Nugget });
                ++nuggetCount;
            }
        }
        g_lastNuggetCount = nuggetCount;
        g_lastTargetCount = (int)out.size();

        if (exitCandidate.x >= 0) {
            g_exitTile = exitCandidate;
            g_haveExit = true;
        } else {
            g_haveExit = false;
        }
        return true;
    }

    static void NearestNeighbor(std::vector<Target>& targets, Vector2i start) {
        if (targets.empty()) return;
        std::vector<Target> tour;
        tour.reserve(targets.size());

        std::vector<bool> taken(targets.size(), false);
        Vector2i cur = start;
        for (size_t step = 0; step < targets.size(); ++step) {
            int bestIdx = -1;
            int bestD   = INT_MAX;
            for (size_t i = 0; i < targets.size(); ++i) {
                if (taken[i]) continue;
                int d = Cheb(cur, targets[i].mapPoint);
                if (d < bestD) { bestD = d; bestIdx = (int)i; }
            }
            if (bestIdx < 0) break;
            taken[bestIdx] = true;
            tour.push_back(targets[bestIdx]);
            cur = targets[bestIdx].mapPoint;
        }
        targets = std::move(tour);
    }

    static void TwoOpt(std::vector<Target>& tour, Vector2i start,
                       Vector2i exit, bool haveExit, int maxPasses = 200) {
        if (tour.size() < 3) return;
        bool improved = true;
        int pass = 0;
        while (improved && pass < maxPasses) {
            improved = false;
            ++pass;
            for (size_t i = 0; i < tour.size() - 1; ++i) {
                for (size_t j = i + 1; j < tour.size(); ++j) {
                    Vector2i a = (i == 0) ? start : tour[i - 1].mapPoint;
                    Vector2i b = tour[i].mapPoint;
                    Vector2i c = tour[j].mapPoint;
                    Vector2i d;
                    if (j + 1 < tour.size())      d = tour[j + 1].mapPoint;
                    else if (haveExit)            d = exit;
                    else                          d = c;
                    int before = Cheb(a, b) + Cheb(c, d);
                    int after  = Cheb(a, c) + Cheb(b, d);
                    if (after < before) {
                        std::reverse(tour.begin() + i, tour.begin() + j + 1);
                        improved = true;
                    }
                }
            }
        }
    }

    static void OrOpt(std::vector<Target>& tour, Vector2i start,
                      Vector2i exit, bool haveExit, int maxPasses = 50) {
        if (tour.size() < 3) return;
        bool improved = true;
        int pass = 0;
        while (improved && pass < maxPasses) {
            improved = false;
            ++pass;
            for (size_t i = 0; i < tour.size(); ++i) {

                Vector2i a = (i == 0) ? start : tour[i - 1].mapPoint;
                Vector2i b = tour[i].mapPoint;
                Vector2i c;
                if (i + 1 < tour.size())          c = tour[i + 1].mapPoint;
                else if (haveExit)                c = exit;
                else                              c = b;
                int removed = Cheb(a, b) + Cheb(b, c) - Cheb(a, c);

                int bestDelta = 0;
                size_t bestK   = (size_t)-1;
                for (size_t k = 0; k < tour.size(); ++k) {
                    if (k == i || k == i + 1) continue;
                    Vector2i p = (k == 0) ? start : tour[k - 1].mapPoint;
                    Vector2i q;
                    if (k < tour.size())              q = tour[k].mapPoint;
                    else if (haveExit)                q = exit;
                    else continue;
                    if (k > i) {

                        if (k - 1 == i) continue;
                    }
                    int added = Cheb(p, b) + Cheb(b, q) - Cheb(p, q);
                    int delta = added - removed;
                    if (delta < bestDelta) {
                        bestDelta = delta;
                        bestK = k;
                    }
                }
                if (bestK != (size_t)-1) {
                    Target moved = tour[i];
                    tour.erase(tour.begin() + i);
                    if (bestK > i) bestK--;
                    tour.insert(tour.begin() + bestK, moved);
                    improved = true;
                    break;
                }
            }
        }
    }

    static bool OrOptSegment(std::vector<Target>& tour, Vector2i start,
                             Vector2i exit, bool haveExit, size_t L)
    {
        if (tour.size() < L + 2) return false;
        bool anyImproved = false;
        bool improved = true;
        int safety = 200;
        while (improved && safety-- > 0) {
            improved = false;
            for (size_t i = 0; i + L <= tour.size(); ++i) {
                Vector2i a = (i == 0) ? start : tour[i - 1].mapPoint;
                Vector2i b = tour[i].mapPoint;
                Vector2i c = tour[i + L - 1].mapPoint;
                Vector2i d;
                if (i + L < tour.size())     d = tour[i + L].mapPoint;
                else if (haveExit)            d = exit;
                else                          d = c;
                int removed = Cheb(a, b) + Cheb(c, d) - Cheb(a, d);

                int bestDelta = 0;
                size_t bestK = (size_t)-1;
                for (size_t k = 0; k <= tour.size(); ++k) {
                    if (k >= i && k <= i + L) continue;
                    Vector2i p = (k == 0) ? start : tour[k - 1].mapPoint;
                    Vector2i q;
                    if (k < tour.size())          q = tour[k].mapPoint;
                    else if (haveExit)            q = exit;
                    else                          q = p;
                    int added = Cheb(p, b) + Cheb(c, q) - Cheb(p, q);
                    int delta = added - removed;
                    if (delta < bestDelta) {
                        bestDelta = delta;
                        bestK = k;
                    }
                }
                if (bestK != (size_t)-1) {

                    std::vector<Target> slice(tour.begin() + i, tour.begin() + i + L);
                    tour.erase(tour.begin() + i, tour.begin() + i + L);
                    if (bestK > i) bestK -= L;
                    tour.insert(tour.begin() + bestK, slice.begin(), slice.end());
                    improved = true;
                    anyImproved = true;
                    break;
                }
            }
        }
        return anyImproved;
    }

    static bool ThreeOpt(std::vector<Target>& tour, Vector2i start,
                         Vector2i exit, bool haveExit, int maxPasses = 20)
    {
        if (tour.size() < 6) return false;
        bool anyImproved = false;
        for (int pass = 0; pass < maxPasses; ++pass) {
            bool improved = false;
            for (size_t i = 0; i + 4 < tour.size(); ++i) {
                for (size_t j = i + 2; j + 2 < tour.size(); ++j) {
                    for (size_t k = j + 2; k < tour.size(); ++k) {

                        Vector2i a = (i == 0) ? start : tour[i - 1].mapPoint;
                        Vector2i b = tour[i].mapPoint;
                        Vector2i c = tour[j - 1].mapPoint;
                        Vector2i d = tour[j].mapPoint;
                        Vector2i e = tour[k].mapPoint;
                        Vector2i f;
                        if (k + 1 < tour.size())   f = tour[k + 1].mapPoint;
                        else if (haveExit)          f = exit;
                        else                         f = e;
                        int before = Cheb(a, b) + Cheb(c, d) + Cheb(e, f);

                        int p1 = Cheb(a, c) + Cheb(b, d) + Cheb(e, f);

                        int p2 = Cheb(a, b) + Cheb(c, e) + Cheb(d, f);

                        int p3 = Cheb(a, d) + Cheb(e, b) + Cheb(c, f);

                        int p4 = Cheb(a, e) + Cheb(d, c) + Cheb(b, f);

                        int bestPat = 0;
                        int bestCost = before;
                        if (p1 < bestCost) { bestCost = p1; bestPat = 1; }
                        if (p2 < bestCost) { bestCost = p2; bestPat = 2; }
                        if (p3 < bestCost) { bestCost = p3; bestPat = 3; }
                        if (p4 < bestCost) { bestCost = p4; bestPat = 4; }
                        if (bestPat == 0) continue;

                        std::vector<Target> A_pre(tour.begin(), tour.begin() + i);
                        std::vector<Target> B(tour.begin() + i, tour.begin() + j);
                        std::vector<Target> C(tour.begin() + j, tour.begin() + k + 1);
                        std::vector<Target> A_post(tour.begin() + k + 1, tour.end());

                        std::vector<Target> rebuilt;
                        rebuilt.reserve(tour.size());
                        rebuilt.insert(rebuilt.end(), A_pre.begin(), A_pre.end());
                        switch (bestPat) {
                        case 1: {
                            std::reverse(B.begin(), B.end());
                            rebuilt.insert(rebuilt.end(), B.begin(), B.end());
                            rebuilt.insert(rebuilt.end(), C.begin(), C.end());
                            break;
                        }
                        case 2: {
                            rebuilt.insert(rebuilt.end(), B.begin(), B.end());
                            std::reverse(C.begin(), C.end());
                            rebuilt.insert(rebuilt.end(), C.begin(), C.end());
                            break;
                        }
                        case 3: {
                            rebuilt.insert(rebuilt.end(), C.begin(), C.end());
                            rebuilt.insert(rebuilt.end(), B.begin(), B.end());
                            break;
                        }
                        case 4: {
                            std::reverse(B.begin(), B.end());
                            std::reverse(C.begin(), C.end());
                            rebuilt.insert(rebuilt.end(), C.begin(), C.end());
                            rebuilt.insert(rebuilt.end(), B.begin(), B.end());
                            break;
                        }
                        }
                        rebuilt.insert(rebuilt.end(), A_post.begin(), A_post.end());
                        tour = std::move(rebuilt);
                        improved = true;
                        anyImproved = true;
                        goto restart_pass;
                    }
                }
            }
        restart_pass:
            if (!improved) break;
        }
        return anyImproved;
    }

    static void PolishFull(std::vector<Target>& tour, Vector2i start,
                           Vector2i exit, bool haveExit)
    {
        TwoOpt(tour, start, exit, haveExit);
        OrOptSegment(tour, start, exit, haveExit, 1);
        OrOptSegment(tour, start, exit, haveExit, 2);
        OrOptSegment(tour, start, exit, haveExit, 3);
        TwoOpt(tour, start, exit, haveExit);
        ThreeOpt(tour, start, exit, haveExit);
        TwoOpt(tour, start, exit, haveExit);
    }

    inline int AStarValidatedTourLen(const std::vector<Target>& tour,
                                     Vector2i start, Vector2i exit, bool haveExit)
    {
        if (!g_WorldController || !PathRenderer::oGetTransform) return 0;
        uintptr_t worldObj = *(uintptr_t*)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (!worldObj) return 0;

        PathRenderer::PathOptions opt;
        opt.airborneCheck = true;
        opt.maxAirborne = 5;
        opt.chamfer = false;
        opt.walkInstakill = false;

        auto edge = [&](Vector2i a, Vector2i b) -> int {
            if (a.x == b.x && a.y == b.y) return 0;
            auto pr = PathRenderer::BuildPath((void*)g_WorldController,
                                              (void*)worldObj, a, b, opt);
            if (pr.status != PathRenderer::PathStatus::Success || pr.tiles.empty()) {
                return Cheb(a, b);
            }
            return (int)pr.tiles.size();
        };

        int total = 0;
        Vector2i prev = start;
        for (size_t i = 0; i < tour.size(); ++i) {
            total += edge(prev, tour[i].mapPoint);
            prev = tour[i].mapPoint;
        }
        if (haveExit) total += edge(prev, exit);
        return total;
    }

    inline void ComputeAndCache(bool scanGems = true, bool scanNuggets = true) {
        auto t0 = std::chrono::steady_clock::now();

        Vector2i start{ 0, 0 };
        if (g_LocalPlayer && g_WorldController &&
            PathRenderer::oGetTransform && PathRenderer::oGetPosition && oConvertWorldPointToMapPoint) {
            void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
            if (lt) {
                Vector3 wp = PathRenderer::oGetPosition(lt);
                start = oConvertWorldPointToMapPoint(g_WorldController, wp);
            }
        } else {
            std::lock_guard<std::mutex> lk(g_seqMutex);
            g_status = "Compute failed: missing player / world / convert fn pointers.";
            g_sequence.clear();
            return;
        }

        std::vector<Target> targets;
        if (!ScanTargets(targets, scanGems, scanNuggets)) {

            return;
        }
        if (targets.empty()) {
            std::lock_guard<std::mutex> lk(g_seqMutex);
            g_sequence.clear();
            g_seqStart = start;
            g_lastFinalDist = 0;
            g_lastComputeMs = 0;
            g_lastScannedTargets.clear();
            if (g_status == "Idle" || g_status.find("failed") == std::string::npos)
                g_status = "No targets found in current world.";
            return;
        }

        g_lastScannedTargets = targets;

        g_lastChebDist = TourLen(targets, start, g_exitTile, g_haveExit);

        std::vector<Target> baseTargets = targets;

        NearestNeighbor(targets, start);
        g_lastNNDist = TourLen(targets, start, g_exitTile, g_haveExit);
        TwoOpt(targets, start, g_exitTile, g_haveExit);
        OrOpt(targets, start, g_exitTile, g_haveExit);
        TwoOpt(targets, start, g_exitTile, g_haveExit);
        int bestDist = TourLen(targets, start, g_exitTile, g_haveExit);

        unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937 rng(seed);
        for (int k = 0; k < 4; ++k) {
            std::vector<Target> trial = baseTargets;
            std::shuffle(trial.begin(), trial.end(), rng);
            TwoOpt(trial, start, g_exitTile, g_haveExit);
            OrOpt(trial, start, g_exitTile, g_haveExit);
            TwoOpt(trial, start, g_exitTile, g_haveExit);
            int d = TourLen(trial, start, g_exitTile, g_haveExit);
            if (d < bestDist) {
                bestDist = d;
                targets = std::move(trial);
            }
        }
        g_lastFinalDist = bestDist;

        auto t1 = std::chrono::steady_clock::now();
        g_lastComputeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

        {
            std::lock_guard<std::mutex> lk(g_seqMutex);
            g_sequence = std::move(targets);
            g_seqStart = start;
        }

        char buf[400];
        std::snprintf(buf, sizeof(buf),
            "%d targets (%d gems, %d nuggets) | exit %s | %.1fms | "
            "Cheb pre %d -> NN %d -> multistart %d tiles",
            g_lastTargetCount, g_lastGemCount, g_lastNuggetCount,
            g_haveExit ? "FOUND" : "NOT FOUND",
            g_lastComputeMs,
            g_lastChebDist, g_lastNNDist, g_lastFinalDist);
        g_status = buf;
        std::cout << "[Scheduler] " << g_status << std::endl;
    }

    inline bool IsSessionActive() { return g_sessionActive; }

    inline bool TryGetFront(Target& out) {
        std::lock_guard<std::mutex> lk(g_seqMutex);
        if (g_sequence.empty()) return false;
        out = g_sequence.front();
        return true;
    }

    inline size_t SequenceSize() {
        std::lock_guard<std::mutex> lk(g_seqMutex);
        return g_sequence.size();
    }

    inline void StartSession(bool scanGems, bool scanNuggets) {
        g_sessionScanGems    = scanGems;
        g_sessionScanNuggets = scanNuggets;
        if (g_WorldController) {
            g_sessionWorldObj = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
        } else {
            g_sessionWorldObj = nullptr;
        }
        ComputeAndCache(scanGems, scanNuggets);
        bool ready = (SequenceSize() > 0) || g_haveExit;
        g_sessionActive = ready;
        std::cout << "[Scheduler] Session START gems=" << scanGems
                  << " nuggets=" << scanNuggets
                  << " seqSize=" << SequenceSize()
                  << " active=" << ready << std::endl;
    }

    inline void EndSession() {
        if (!g_sessionActive && g_sequence.empty()) return;
        {
            std::lock_guard<std::mutex> lk(g_seqMutex);
            g_sequence.clear();
            g_lastScannedTargets.clear();
        }
        g_sessionActive   = false;
        g_sessionWorldObj = nullptr;
        g_haveExit        = false;
        g_lastFinalDist   = 0;
        g_lastTargetCount = 0;
        g_status          = "Idle (session ended).";
        std::cout << "[Scheduler] Session END" << std::endl;
    }

    inline void Recompute(bool scanGems, bool scanNuggets) {
        g_sessionScanGems    = scanGems;
        g_sessionScanNuggets = scanNuggets;
        ComputeAndCache(scanGems, scanNuggets);
        std::cout << "[Scheduler] Recompute gems=" << scanGems
                  << " nuggets=" << scanNuggets
                  << " seqSize=" << SequenceSize() << std::endl;
    }

    inline bool WorldObjectChanged() {
        if (!g_WorldController) return true;
        void* cur = *(void**)((uintptr_t)g_WorldController + g_off_WCWorld);
        return cur != g_sessionWorldObj;
    }

    inline void PopFrontIfComplete() {
        if (!g_WorldController || !g_sessionActive) return;
        uintptr_t worldObj = *(uintptr_t*)((uintptr_t)g_WorldController + g_off_WCWorld);
        if (!worldObj) return;
        uintptr_t pBlockArrayObject = *(uintptr_t*)(worldObj + g_off_WorldBlockLayer);
        if (!pBlockArrayObject) return;
        uintptr_t* pRows = (uintptr_t*)(pBlockArrayObject + 0x20);

        std::lock_guard<std::mutex> lk(g_seqMutex);
        while (!g_sequence.empty()) {
            const Target& t = g_sequence.front();
            bool gone = false;
            if (t.kind == TargetKind::GemOre || t.kind == TargetKind::Crystal) {

                uintptr_t pRow = pRows[t.mapPoint.x];
                if (!pRow) { gone = true; }
                else {
                    char* pRowData = (char*)(pRow + 0x20);
                    int bt = *(int*)((uintptr_t)pRowData + (t.mapPoint.y * 0x28));
                    if (t.kind == TargetKind::GemOre) {
                        if (!IsGemstone((World::BlockType)bt)) gone = true;
                    } else {
                        if (!IsLightCrystal((World::BlockType)bt)) gone = true;
                    }
                }
            } else if (t.kind == TargetKind::Nugget) {

                std::lock_guard<std::mutex> lk2(g_floorCollectablesMutex);
                bool stillThere = false;
                for (const auto& fc : g_floorCollectables) {
                    if (fc.mapPoint.x == t.mapPoint.x &&
                        fc.mapPoint.y == t.mapPoint.y &&
                        IsNugget(fc.blockType)) {
                        stillThere = true;
                        break;
                    }
                }
                if (!stillThere) gone = true;
            } else {

                std::lock_guard<std::mutex> lk2(g_floorCollectablesMutex);
                bool stillThere = false;
                for (const auto& fc : g_floorCollectables) {
                    if (t.collectId >= 0 ? (fc.id == t.collectId)
                                         : (fc.mapPoint.x == t.mapPoint.x &&
                                            fc.mapPoint.y == t.mapPoint.y)) {
                        stillThere = true;
                        break;
                    }
                }
                if (!stillThere) gone = true;
            }
            if (!gone) break;
            g_sequence.erase(g_sequence.begin());
        }
    }

    inline void ReactiveSync(bool scanGems, bool scanNuggets, bool scanCollect) {
        if (!g_sessionActive) return;

        std::vector<Target> gemDrops, nuggetDrops, miscDrops;
        {
            std::lock_guard<std::mutex> lk(g_floorCollectablesMutex);
            for (const auto& fc : g_floorCollectables) {
                bool isGem    = (fc.blockType >= (World::BlockType)4012 &&
                                 fc.blockType <= (World::BlockType)4056);
                bool isNug    = IsNugget(fc.blockType);
                if (isGem) {
                    if (scanGems)
                        gemDrops.push_back({ fc.mapPoint, TargetKind::Drop, fc.id });
                } else if (isNug) {
                    if (scanNuggets)
                        nuggetDrops.push_back({ fc.mapPoint, TargetKind::Nugget, fc.id });
                } else {
                    if (scanCollect)
                        miscDrops.push_back({ fc.mapPoint, TargetKind::Drop, fc.id });
                }
            }
        }
        if (gemDrops.empty() && nuggetDrops.empty() && miscDrops.empty()) return;

        std::lock_guard<std::mutex> lk(g_seqMutex);
        auto isInSequence = [&](Vector2i p) {
            for (const auto& s : g_sequence) {
                if (s.mapPoint.x == p.x && s.mapPoint.y == p.y) return true;
            }
            return false;
        };
        std::vector<Target> toPrepend;
        for (const auto& g : gemDrops)    if (!isInSequence(g.mapPoint)) toPrepend.push_back(g);
        for (const auto& n : nuggetDrops) if (!isInSequence(n.mapPoint)) toPrepend.push_back(n);
        for (const auto& m : miscDrops)   if (!isInSequence(m.mapPoint)) toPrepend.push_back(m);
        if (toPrepend.empty()) return;

        g_sequence.insert(g_sequence.begin(), toPrepend.begin(), toPrepend.end());
        std::cout << "[Scheduler] ReactiveSync prepended " << toPrepend.size()
                  << " (gemDrops=" << gemDrops.size()
                  << " nuggets="   << nuggetDrops.size()
                  << " misc="      << miscDrops.size() << ")" << std::endl;
    }

    inline void ReactiveSyncCrystal(bool scanCrystals,
                                    float barFraction, float threshold,
                                    Vector2i playerTile)
    {
        if (!g_sessionActive || !scanCrystals) return;
        if (barFraction < 0.f) return;
        if (barFraction > threshold) return;

        {
            std::lock_guard<std::mutex> lk(g_seqMutex);
            for (const auto& t : g_sequence) {
                if (t.kind == TargetKind::Crystal) return;
            }
        }

        Vector2i bestPt{ -1, -1 };
        int bestDist = INT_MAX;
        {
            std::lock_guard<std::mutex> lk(g_lightCrystalLocationsMutex);
            for (const auto& c : g_lightCrystalLocations) {
                int dx = c.mapPoint.x - playerTile.x;
                if (dx < 0) dx = -dx;
                int dy = c.mapPoint.y - playerTile.y;
                if (dy < 0) dy = -dy;
                int d = (dx > dy) ? dx : dy;
                if (d < bestDist) {
                    bestDist = d;
                    bestPt = c.mapPoint;
                }
            }
        }
        if (bestPt.x < 0) return;

        std::lock_guard<std::mutex> lk(g_seqMutex);

        for (const auto& t : g_sequence) {
            if (t.kind == TargetKind::Crystal) return;
        }
        g_sequence.insert(g_sequence.begin(),
                          Target{ bestPt, TargetKind::Crystal });
        std::cout << "[Scheduler] ReactiveSyncCrystal prepended ("
                  << bestPt.x << "," << bestPt.y << ") bar="
                  << (int)(barFraction * 100.f) << "%" << std::endl;
    }

    inline void RunVariantComparison() {
        auto t0 = std::chrono::steady_clock::now();

        Vector2i start{ 0, 0 };
        if (g_LocalPlayer && g_WorldController &&
            PathRenderer::oGetTransform && PathRenderer::oGetPosition && oConvertWorldPointToMapPoint) {
            void* lt = PathRenderer::oGetTransform(g_LocalPlayer);
            if (lt) {
                Vector3 wp = PathRenderer::oGetPosition(lt);
                start = oConvertWorldPointToMapPoint(g_WorldController, wp);
            }
        } else {
            g_status = "Variant compare failed: missing player / world / convert fn pointers.";
            return;
        }

        std::vector<Target> baseTargets;
        if (!ScanTargets(baseTargets)) return;
        if (baseTargets.empty()) { g_status = "Variant compare: no targets in this world."; return; }

        Vector2i exitT = g_exitTile;
        bool     hasE  = g_haveExit;

        struct VariantResult {
            std::string         name;
            int                 distance = 0;
            int                 aStarDist = 0;
            double              ms        = 0.0;
            double              aStarMs   = 0.0;
            std::vector<Target> tour;
        };
        std::vector<VariantResult> results;

        auto runVariant = [&](const char* name, auto proc) {
            VariantResult r;
            r.name = name;
            r.tour = baseTargets;
            auto v0 = std::chrono::steady_clock::now();
            proc(r.tour);
            auto v1 = std::chrono::steady_clock::now();
            r.ms = std::chrono::duration<double, std::milli>(v1 - v0).count();
            r.distance = TourLen(r.tour, start, exitT, hasE);
            results.push_back(std::move(r));
        };

        runVariant("nn_only", [&](std::vector<Target>& t) {
            NearestNeighbor(t, start);
        });

        runVariant("nn_2opt", [&](std::vector<Target>& t) {
            NearestNeighbor(t, start);
            TwoOpt(t, start, exitT, hasE);
        });

        runVariant("nn_oropt", [&](std::vector<Target>& t) {
            NearestNeighbor(t, start);
            OrOpt(t, start, exitT, hasE);
        });

        runVariant("nn_2opt_oropt_2opt", [&](std::vector<Target>& t) {
            NearestNeighbor(t, start);
            TwoOpt(t, start, exitT, hasE);
            OrOpt(t, start, exitT, hasE);
            TwoOpt(t, start, exitT, hasE);
        });

        runVariant("nn_oropt_2opt", [&](std::vector<Target>& t) {
            NearestNeighbor(t, start);
            OrOpt(t, start, exitT, hasE);
            TwoOpt(t, start, exitT, hasE);
        });

        runVariant("random_shuffle", [&](std::vector<Target>& t) {
            unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
            std::mt19937 rng(seed);
            std::shuffle(t.begin(), t.end(), rng);
        });

        runVariant("shuffle_2opt_oropt_2opt", [&](std::vector<Target>& t) {
            unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
            std::mt19937 rng(seed);
            std::shuffle(t.begin(), t.end(), rng);
            TwoOpt(t, start, exitT, hasE);
            OrOpt(t, start, exitT, hasE);
            TwoOpt(t, start, exitT, hasE);
        });

        runVariant("multistart_4_2opt_oropt", [&](std::vector<Target>& t) {
            unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
            std::mt19937 rng(seed);
            std::vector<Target> best = t;
            NearestNeighbor(best, start);
            TwoOpt(best, start, exitT, hasE);
            OrOpt(best, start, exitT, hasE);
            int bestDist = TourLen(best, start, exitT, hasE);
            for (int k = 0; k < 4; ++k) {
                std::vector<Target> trial = baseTargets;
                std::shuffle(trial.begin(), trial.end(), rng);
                TwoOpt(trial, start, exitT, hasE);
                OrOpt(trial, start, exitT, hasE);
                TwoOpt(trial, start, exitT, hasE);
                int d = TourLen(trial, start, exitT, hasE);
                if (d < bestDist) { bestDist = d; best = std::move(trial); }
            }
            t = std::move(best);
        });

        runVariant("nn_oropt23", [&](std::vector<Target>& t) {
            NearestNeighbor(t, start);
            OrOptSegment(t, start, exitT, hasE, 2);
            OrOptSegment(t, start, exitT, hasE, 3);
            TwoOpt(t, start, exitT, hasE);
        });

        runVariant("nn_3opt_2opt", [&](std::vector<Target>& t) {
            NearestNeighbor(t, start);
            ThreeOpt(t, start, exitT, hasE);
            TwoOpt(t, start, exitT, hasE);
        });

        runVariant("nn_polish_full", [&](std::vector<Target>& t) {
            NearestNeighbor(t, start);
            PolishFull(t, start, exitT, hasE);
        });

        runVariant("multistart_polish_full", [&](std::vector<Target>& t) {
            unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
            std::mt19937 rng(seed);
            std::vector<Target> best = t;
            NearestNeighbor(best, start);
            PolishFull(best, start, exitT, hasE);
            int bestDist = TourLen(best, start, exitT, hasE);
            for (int k = 0; k < 4; ++k) {
                std::vector<Target> trial = baseTargets;
                std::shuffle(trial.begin(), trial.end(), rng);
                PolishFull(trial, start, exitT, hasE);
                int d = TourLen(trial, start, exitT, hasE);
                if (d < bestDist) { bestDist = d; best = std::move(trial); }
            }
            t = std::move(best);
        });

        int bestIdx = 0;
        for (size_t i = 1; i < results.size(); ++i) {
            if (results[i].distance < results[bestIdx].distance) bestIdx = (int)i;
        }
        {
            std::lock_guard<std::mutex> lk(g_seqMutex);
            g_sequence = results[bestIdx].tour;
            g_seqStart = start;
        }
        g_lastFinalDist = results[bestIdx].distance;
        g_lastNNDist    = results[0].distance;

        namespace fs = std::filesystem;
        char* appDataBuf = nullptr;
        size_t appDataLen = 0;
        _dupenv_s(&appDataBuf, &appDataLen, "APPDATA");
        fs::path dir = (appDataBuf ? fs::path(appDataBuf) : fs::path("."))
                       / "ZMod" / "diagnostics" / "scheduler";
        if (appDataBuf) free(appDataBuf);
        std::error_code ec;
        fs::create_directories(dir, ec);

        std::time_t now = std::time(nullptr);
        std::tm tm_utc{};
#ifdef _WIN32
        gmtime_s(&tm_utc, &now);
#else
        gmtime_r(&now, &tm_utc);
#endif
        char ts[32];
        std::strftime(ts, sizeof(ts), "%Y%m%dT%H%M%SZ", &tm_utc);
        fs::path file = dir / (std::string("scheduler_variants_") + ts + ".json");

        nlohmann::json j;
        if (g_WorldController) {
            uintptr_t worldObj = *(uintptr_t*)((uintptr_t)g_WorldController + g_off_WCWorld);
            if (worldObj) {
                struct V2 { int x, y; };
                V2 sz = *(V2*)(worldObj + g_off_WorldSize);
                j["worldSize"] = { {"x", sz.x}, {"y", sz.y} };
            }
        }
        j["start"] = { {"x", start.x}, {"y", start.y} };
        if (hasE) j["exit"] = { {"x", exitT.x}, {"y", exitT.y} };
        else      j["exit"] = nullptr;

        auto& scanArr = j["scanned"];
        scanArr = nlohmann::json::array();
        for (const auto& t : baseTargets) {
            scanArr.push_back({
                {"x", t.mapPoint.x}, {"y", t.mapPoint.y},
                {"kind", t.kind == TargetKind::GemOre ? "gem" : "nugget"}
            });
        }

        auto& varArr = j["variants"];
        varArr = nlohmann::json::array();
        for (const auto& r : results) {
            nlohmann::json vj;
            vj["name"]          = r.name;
            vj["distance"]      = r.distance;
            vj["aStarDistance"] = r.aStarDist;
            vj["computeMs"]     = r.ms;
            vj["aStarMs"]       = r.aStarMs;
            auto& seq = vj["sequence"];
            seq = nlohmann::json::array();
            for (const auto& t : r.tour) {
                seq.push_back({
                    {"x", t.mapPoint.x}, {"y", t.mapPoint.y},
                    {"kind", t.kind == TargetKind::GemOre ? "gem" : "nugget"}
                });
            }
            varArr.push_back(vj);
        }

        j["meta"] = {
            {"targetCount",     (int)baseTargets.size()},
            {"gemCount",        g_lastGemCount},
            {"nuggetCount",     g_lastNuggetCount},
            {"haveExit",        hasE},
            {"bestVariant",     results[bestIdx].name},
            {"bestDistance",    results[bestIdx].distance},
            {"bestAStarDistance", results[bestIdx].aStarDist}
        };

        std::ofstream(file) << j.dump(2);

        auto t1 = std::chrono::steady_clock::now();
        double totalMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
        char buf[400];
        std::snprintf(buf, sizeof(buf),
            "Variants done in %.1fms. Best=%s cheb=%d astar=%d. %d variants in %s",
            totalMs, results[bestIdx].name.c_str(),
            results[bestIdx].distance, results[bestIdx].aStarDist,
            (int)results.size(), file.string().c_str());
        g_status = buf;
        std::cout << "[Scheduler] " << g_status << std::endl;
        for (const auto& r : results) {
            std::cout << "  variant " << r.name
                      << " | cheb=" << r.distance
                      << " | astar=" << r.aStarDist
                      << " | " << r.ms << "ms"
                      << " | astar_validate=" << r.aStarMs << "ms"
                      << std::endl;
        }
    }

    inline void ExportToFile() {
        namespace fs = std::filesystem;

        char* appDataBuf = nullptr;
        size_t appDataLen = 0;
        _dupenv_s(&appDataBuf, &appDataLen, "APPDATA");
        fs::path dir = (appDataBuf ? fs::path(appDataBuf) : fs::path("."))
                       / "ZMod" / "diagnostics" / "scheduler";
        if (appDataBuf) free(appDataBuf);
        std::error_code ec;
        fs::create_directories(dir, ec);

        std::time_t now = std::time(nullptr);
        std::tm tm_utc{};
#ifdef _WIN32
        gmtime_s(&tm_utc, &now);
#else
        gmtime_r(&now, &tm_utc);
#endif
        char ts[32];
        std::strftime(ts, sizeof(ts), "%Y%m%dT%H%M%SZ", &tm_utc);
        fs::path file = dir / (std::string("scheduler_") + ts + ".json");

        nlohmann::json j;

        if (g_WorldController) {
            uintptr_t worldObj = *(uintptr_t*)((uintptr_t)g_WorldController + g_off_WCWorld);
            if (worldObj) {
                struct V2 { int x, y; };
                V2 sz = *(V2*)(worldObj + g_off_WorldSize);
                j["worldSize"] = { {"x", sz.x}, {"y", sz.y} };
            }
        }
        j["start"] = { {"x", g_seqStart.x}, {"y", g_seqStart.y} };
        if (g_haveExit) j["exit"] = { {"x", g_exitTile.x}, {"y", g_exitTile.y} };
        else            j["exit"] = nullptr;

        {
            auto& arr = j["scanned"];
            arr = nlohmann::json::array();
            for (const auto& t : g_lastScannedTargets) {
                arr.push_back({
                    {"x", t.mapPoint.x},
                    {"y", t.mapPoint.y},
                    {"kind", t.kind == TargetKind::GemOre ? "gem" : "nugget"}
                });
            }
        }

        {
            std::lock_guard<std::mutex> lk(g_seqMutex);
            auto& arr = j["sequence"];
            arr = nlohmann::json::array();
            for (const auto& t : g_sequence) {
                arr.push_back({
                    {"x", t.mapPoint.x},
                    {"y", t.mapPoint.y},
                    {"kind", t.kind == TargetKind::GemOre ? "gem" : "nugget"}
                });
            }
        }

        j["meta"] = {
            {"computeMs",       g_lastComputeMs},
            {"chebPreSort",     g_lastChebDist},
            {"distAfterNN",     g_lastNNDist},
            {"distAfterTwoOpt", g_lastFinalDist},
            {"targetCount",     g_lastTargetCount},
            {"gemCount",        g_lastGemCount},
            {"nuggetCount",     g_lastNuggetCount},
            {"haveExit",        g_haveExit}
        };

        std::ofstream(file) << j.dump(2);
        g_status = std::string("Exported: ") + file.string();
        std::cout << "[Scheduler] " << g_status << std::endl;
    }

    inline void RenderOnMinimap(ImDrawList* drawList, ImVec2 canvas_pos,
                                int worldSizeX, int worldSizeY, float zoom) {

        if (!g_sessionActive) return;
        std::lock_guard<std::mutex> lk(g_seqMutex);
        if (g_sequence.empty()) return;

        auto tileToCanvas = [&](Vector2i t) -> ImVec2 {
            return ImVec2(
                canvas_pos.x + t.x * zoom + zoom * 0.5f,
                canvas_pos.y + (worldSizeY - 1 - t.y) * zoom + zoom * 0.5f);
        };

        const ImU32 lineColor = IM_COL32(255, 255, 255, 200);
        const ImU32 exitLineColor = IM_COL32(255, 80, 80, 220);
        ImVec2 prev = tileToCanvas(g_seqStart);
        for (size_t i = 0; i < g_sequence.size(); ++i) {
            ImVec2 cur = tileToCanvas(g_sequence[i].mapPoint);
            drawList->AddLine(prev, cur, lineColor, 1.5f);
            prev = cur;
        }
        if (g_haveExit) {
            ImVec2 exitC = tileToCanvas(g_exitTile);
            drawList->AddLine(prev, exitC, exitLineColor, 2.0f);
        }

        const ImU32 gemDot     = IM_COL32(0,   180, 255, 255);
        const ImU32 nuggetDot  = IM_COL32(255, 200, 50,  255);
        const ImU32 crystalDot = IM_COL32(255, 255, 255, 255);
        const ImU32 dropDot    = IM_COL32(255, 100, 200, 255);
        const ImU32 dotEdge    = IM_COL32(0,   0,   0,   255);
        for (size_t i = 0; i < g_sequence.size(); ++i) {
            ImVec2 c = tileToCanvas(g_sequence[i].mapPoint);
            float r = zoom * 0.45f; if (r < 2.0f) r = 2.0f;
            ImU32 col;
            switch (g_sequence[i].kind) {
                case TargetKind::GemOre:  col = gemDot;     break;
                case TargetKind::Nugget:  col = nuggetDot;  break;
                case TargetKind::Crystal: col = crystalDot; break;
                case TargetKind::Drop:    col = dropDot;    break;
                default:                  col = nuggetDot;  break;
            }
            drawList->AddCircleFilled(c, r, col);
            drawList->AddCircle(c, r, dotEdge, 0, 1.0f);
        }

        ImVec2 startC = tileToCanvas(g_seqStart);
        drawList->AddCircle(startC, zoom * 0.7f, IM_COL32(255, 255, 255, 255), 0, 2.0f);

        size_t labelN = g_sequence.size();
        if (labelN > 30) labelN = 30;
        for (size_t i = 0; i < labelN; ++i) {
            ImVec2 c = tileToCanvas(g_sequence[i].mapPoint);
            char num[8]; std::snprintf(num, sizeof(num), "%d", (int)(i + 1));
            ImVec2 textSize = ImGui::CalcTextSize(num);
            drawList->AddText(ImVec2(c.x - textSize.x * 0.5f, c.y - textSize.y * 0.5f),
                              IM_COL32(0, 0, 0, 220), num);
        }
    }

}

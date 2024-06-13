#pragma once
#include "pch.h"

namespace RobotSimulate {

    struct Scene;
    struct Robot;

    struct Item {
        Scene* scene{};
        std::string name;
        XY pos{};
        float radius{}, healthPoint{};

        bool Hit(Robot& attacker);                          // return true: death
    };

    inline float DistancePow2(float x1, float y1, float x2, float y2) {
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    }

    inline float DistancePow2(Item const& a, Item const& b) {
        return DistancePow2(a.pos.x, a.pos.y, b.pos.x, b.pos.y);
    }

    struct Tree : Item {
    };

    using Trees = xx::BlockLink<Tree>;
    using Tree_w = Trees::WeakType;

    struct Robot : Item {
        float searchDelaySeconds{};
        float moveStepSeconds, moveSpeed{};
        float attackDelaySeconds{}, attackRange{}, damage{};
        Tree_w target;

        xx::Task<bool> SearchTarget();                      // let target = nearest tree. not found: return false
        xx::Task<bool> MoveToTarget();                      // move a step. return robot & target 's distance < attackRange
        xx::Task<bool> Attack();                            // let target's healthPoint -= damage, then sleep attackDelaySeconds. miss: return false

        xx::Task<> Update = Update_();
        xx::Task<> Update_();
    };

    using Robots = xx::BlockLink<Robot>;
    using Robot_w = Robots::WeakType;

    struct Scene {
        /*

      0       15  25           50    62    75          100
      ┌────────────────────────────────────────────────┐
      │                                                │
      │                             ┌──┐               │
    15│                             │  │tree           │
      │                             └──┘               │
      │                                                │
      │                                                │
      │                                   ┌──┐         │
      │      robot                        │  │         │
    35│      ───►                         └──┘         │
      │                                                │
      │                                                │
      │                                                │
      │                           ┌──┐                 │
    50│                           │  │                 │
      │                           └──┘                 │
      │                                                │
      │                                                │
      └────────────────────────────────────────────────┘
     70

        */
        static constexpr float cFramePerSeconds{ 60.f };
        static constexpr float cFrameDelaySeconds{ 1.f / cFramePerSeconds };

        bool gameOver{};
        float timePool{}, now{};

        xx::BlockLink<Tree> trees;
        xx::BlockLink<Robot> robots;

        xx::Task<> Logic = Logic_();
        xx::Task<> Logic_() {
            xx::CoutN("play");
            // make some trees
            {
                auto&& o = trees.Emplace();
                o.scene = this;
                o.name = "tree1";
                o.pos = { 62, 15 };
                o.radius = 10;
                o.healthPoint = 3;
            }
            {
                auto&& o = trees.Emplace();
                o.scene = this;
                o.name = "tree2";
                o.pos = { 75, 33 };
                o.radius = 10;
                o.healthPoint = 4;
            }
            {
                auto&& o = trees.Emplace();
                o.scene = this;
                o.name = "tree3";
                o.pos = { 60, 50 };
                o.radius = 10;
                o.healthPoint = 5;
            }
            // make a robot
            {
                auto&& o = robots.Emplace();
                o.scene = this;
                o.name = "robot";
                o.pos = { 15, 35 };
                o.radius = 10;
                o.healthPoint = 100;
                o.searchDelaySeconds = 0.5f;
                o.moveStepSeconds = 0.5f;
                o.moveSpeed = 100.f / 5.f / cFramePerSeconds;
                o.attackDelaySeconds = 1.f;
                o.attackRange = 15;
                o.damage = 1;
            }
            co_yield 0;

            while (!trees.Empty() && !robots.Empty()) {
                robots.ForeachFlags([](Robot& robot)->xx::ForeachResult {
                    if (robot.Update()) return xx::ForeachResult::RemoveAndContinue;
                    else return  xx::ForeachResult::Continue;
                });
                co_yield 0;
            }

            xx::CoutN("game over");
            gameOver = true;
        }

        void Update(float delta) {
            now += delta;
            timePool += delta;
            if (timePool >= cFrameDelaySeconds) {
                timePool -= cFrameDelaySeconds;
                Logic();
            }
        }
    };


    inline xx::Task<> Robot::Update_() {
    LabSearch:
        if (!co_await SearchTarget()) goto LabSearch;       // keep searching

    LabMove:
        if (!target) goto LabSearch;                        // target is dead ? search again
        if (!co_await MoveToTarget()) goto LabMove;         // far away from the target: keep moving

    LabAttack:
        if (!co_await Attack()) goto LabMove;               // attack failed: move to target
        if (!target) goto LabSearch;                        // target is dead: search again
        goto LabAttack;                                     // keep attacking
    }

    inline bool Item::Hit(Robot& attacker) {
        healthPoint -= attacker.damage;
        if (healthPoint <= 0) {
            xx::CoutN(attacker.name, " killed ", name, " !");
            return true;
        } else {
            xx::CoutN(attacker.name, " hit ", name, " and deal ", attacker.damage," damage ! tree's HP = ", healthPoint);
            return false;
        }
    }

    inline xx::Task<bool> Robot::SearchTarget() {
        xx::CoutN(name, " is searching target ...");
        assert(!target);

        // sleep
        for (auto e = scene->now + searchDelaySeconds; scene->now < e;) co_yield 0;

        // search
        float minDistancePow2{ std::numeric_limits<float>::max() };
        scene->trees.ForeachFlags([&](Tree& tree)->void {
            if (auto dp2 = DistancePow2(tree, *this); dp2 < minDistancePow2) {
                minDistancePow2 = dp2;
                target = tree;
            }
        });

        if (target) {
            xx::CoutN(name, " found target: ", target().name, " !");
            co_return true;
        }
        co_return false;
    }

    inline xx::Task<bool> Robot::MoveToTarget() {
        for (auto e = scene->now + moveStepSeconds; scene->now < e;) {
            if (!target) co_return false;
            auto&& tree = target();
            auto d = tree.pos - pos;
            auto r = radius + attackRange;
            auto rr = (tree.radius + r) * (tree.radius + r);
            auto dd = d.x * d.x + d.y * d.y;
            if (rr > dd) co_return true;                    // cross
            if (dd > 0.f) {
                auto v = d / std::sqrt(dd);		// normalize
                pos += v * moveSpeed;                       // move
                xx::CoutN(name, " is moving to ", target().name, ". pos = ", pos.x, ", ", pos.y);
            }
            co_yield 0;
        }
        co_return false;
    }

    inline xx::Task<bool> Robot::Attack() {
        if (!target) co_return false;
        {
            auto&& tree = target();
            if (tree.Hit(*this)) {
                scene->trees.Remove(target);
                co_return false;
            }
        }
        for (auto e = scene->now + attackDelaySeconds; scene->now < e;) co_yield 0;
        co_return true;
    }
}

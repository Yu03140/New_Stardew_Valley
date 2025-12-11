#pragma once
#include "cocos2d.h"
#include "IInteractable.h"
#include "crop.h"
#include "getable_goods.h"
#include "Global/Global.h"

// ==========================================
// 1. 抽象策略接口
// ==========================================
class IInteractionStrategy {
public:
    virtual ~IInteractionStrategy() {}
    // 检查是否可以使用此策略 (相当于原来的 if 判断)
    virtual bool canExecute(const InteractContext& ctx, IInteractable* target) = 0;
    // 执行逻辑
    virtual void execute(const InteractContext& ctx, IInteractable* target) = 0;
};

// ==========================================
// 2. 作物相关策略
// ==========================================

// --- 浇水策略 ---
class WateringStrategy : public IInteractionStrategy {
public:
    bool canExecute(const InteractContext& ctx, IInteractable* target) override {
        // 只有当作物理处于生长阶段 (1-4) 且手里拿着水壶时
        auto c = dynamic_cast<crop*>(target); // 安全转换
        return c && (c->getDevelopLevel() >= 1 && c->getDevelopLevel() <= 4) && ctx.toolName == "Can";
    }

    void execute(const InteractContext& ctx, IInteractable* target) override {
        auto c = dynamic_cast<crop*>(target);
        if (c) {
            // 拼凑旧逻辑需要的全名 "Can1"
            std::string fullToolName = ctx.toolName + std::to_string(ctx.toolLevel);
            c->water(fullToolName);
        }
    }
};

// --- 收获策略 ---
class HarvestStrategy : public IInteractionStrategy {
public:
    bool canExecute(const InteractContext& ctx, IInteractable* target) override {
        auto c = dynamic_cast<crop*>(target);
        // 成熟 (level 5) 且在控制范围内，不需要特定工具
        return c && c->getDevelopLevel() == 5;
    }

    void execute(const InteractContext& ctx, IInteractable* target) override {
        auto c = dynamic_cast<crop*>(target);
        if (c) c->harvest();
    }
};

// --- 铲除/枯萎清理策略 ---
class ClearStrategy : public IInteractionStrategy {
public:
    bool canExecute(const InteractContext& ctx, IInteractable* target) override {
        auto c = dynamic_cast<crop*>(target);
        // 枯萎 (-1)
        return c && c->getDevelopLevel() == -1;
    }

    void execute(const InteractContext& ctx, IInteractable* target) override {
        auto c = dynamic_cast<crop*>(target);
        if (c) c->clear();
    }
};

// --- 施肥策略 ---
class FertilizeStrategy : public IInteractionStrategy {
public:
    bool canExecute(const InteractContext& ctx, IInteractable* target) override {
        auto c = dynamic_cast<crop*>(target);
        return c && (c->getDevelopLevel() >= 1 && c->getDevelopLevel() <= 4) && ctx.toolName == "fertilizer";
    }

    void execute(const InteractContext& ctx, IInteractable* target) override {
        auto c = dynamic_cast<crop*>(target);
        if (c) {
            std::string fullToolName = ctx.toolName + std::to_string(ctx.toolLevel);
            c->fertilize(fullToolName);
        }
    }
};

// ==========================================
// 3. 物品采集相关策略 (通用化)
// ==========================================

class GatheringStrategy : public IInteractionStrategy {
public:
    bool canExecute(const InteractContext& ctx, IInteractable* target) override {
        auto g = dynamic_cast<getable_goods*>(target);
        if (!g || !g->get_is_getable()) return false;

        // 获取该物品需要的工具名称 (需要在 getable_goods 中公开此方法或通过 helper 获取)
        // 假设我们在 getable_goods 中添加了 getRequiredTool() 方法
        std::string required = g->getRequiredTool();

        if (required.empty()) return true; // 不需要工具
        return ctx.toolName == required;   // 工具名称匹配
    }

    void execute(const InteractContext& ctx, IInteractable* target) override {
        auto g = dynamic_cast<getable_goods*>(target);
        if (g) {
            // 增加点击计数
            // 注意：这里需要你把 getable_goods 的 click_count 和 update 逻辑公开或者封装成方法
            // 为了最小化修改，假设我们在 getable_goods 里加了一个 processHit(int power)
            int power = (ctx.toolLevel > 0) ? ctx.toolLevel : 1;
            g->processToolHit(power);
        }
    }
};
#pragma once
#include "InteractionStrategies.h"
#include <vector>

class InteractionStrategyContext {
private:
    std::vector<IInteractionStrategy*> _strategies;

public:
    InteractionStrategyContext() {
        // 注册所有可能的策略
        // 顺序很重要！类似于原来的 if-else if 顺序
        _strategies.push_back(new ClearStrategy());
        _strategies.push_back(new HarvestStrategy());
        _strategies.push_back(new WateringStrategy());
        _strategies.push_back(new FertilizeStrategy());
        // 种植逻辑比较特殊，因为它依赖背包选中的是不是种子，建议保留在 crop 内部或单独处理

        _strategies.push_back(new GatheringStrategy());
    }

    ~InteractionStrategyContext() {
        for (auto s : _strategies) delete s;
        _strategies.clear();
    }

    // 核心方法：找到合适的策略并执行
    bool handleInteraction(const InteractContext& ctx, IInteractable* target) {
        for (auto strategy : _strategies) {
            if (strategy->canExecute(ctx, target)) {
                CCLOG("[Strategy] Match found! Executing strategy.");
                strategy->execute(ctx, target);
                return true; // 策略已执行
            }
        }
        CCLOG("[Strategy] No matching strategy found for this tool/target combination.");
        return false;
    }

    // 单例模式获取
    static InteractionStrategyContext* getInstance() {
        static InteractionStrategyContext instance;
        return &instance;
    }
};
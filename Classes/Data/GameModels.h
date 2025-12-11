#pragma once
// 存放所有享元数据的定义

// Classes/Data/GameModels.h
#ifndef __GAME_MODELS_H__
#define __GAME_MODELS_H__

#include <string>
#include "Global/Global.h" 

// 1. 物品模型 (ItemModel) - 对应原来的 getable_goods
// 定义了石头、木头等不可移动资源的属性
struct ItemModel {
    std::string id;            // ID (e.g., "stones")
    std::string dropItem;      // 掉落物名称 (e.g., "stone")
    std::string requiredTool;  // 需要的工具 (e.g., "Pick")
    int maxClicks;             // 需要点击的次数 (e.g., 10)

    ItemModel(std::string _id, std::string _drop, std::string _tool, int _clicks)
        : id(_id), dropItem(_drop), requiredTool(_tool), maxClicks(_clicks) {
    }
};

// 2. 作物模型 (CropModel) - 对应原来的 crop
// 定义了农作物的生长属性
struct CropModel {
    std::string id;            // ID (e.g., "strawberry")
    std::string harvestItem;   // 收获产物 (e.g., "strawberry_fruit")
    int developDays;           // 生长周期 (e.g., 5)
    int season;                // 适宜季节 (e.g., SPRING)

    CropModel(std::string _id, std::string _harvest, int _days, int _season)
        : id(_id), harvestItem(_harvest), developDays(_days), season(_season) {
    }
};

// 3. 动物模型 (AnimalModel) - 对应原来的 animals
// 定义了动物的产出属性
struct AnimalModel {
    std::string id;            // ID (e.g., "Cow")
    std::string produceItem;   // 产出物 (e.g., "cow_milk")
    int produceCycle;          // 产出周期 (e.g., 7 天)

    AnimalModel(std::string _id, std::string _produce, int _cycle)
        : id(_id), produceItem(_produce), produceCycle(_cycle) {
    }
};

// 4. 鱼类模型 (FishModel) - 对应原来的 fish
struct FishModel {
    std::string id;            // ID (e.g., "bluefish")
    std::string harvestItem;   // 收获产物

    //存 [季节ID -> 生长天数] 的映射
    std::map<int, int> seasonRules;

    FishModel(std::string _id, std::string _harvest)
        : id(_id), harvestItem(_harvest) {
    }

    // 添加规则
    void addSeasonRule(int season, int days) {
        seasonRules[season] = days;
    }

    // 辅助函数：查询某季节是否可养殖，返回所需天数
    // 如果返回 -1 表示该季节不能养
    int getDevelopDays(int currentSeason) {
        if (seasonRules.find(currentSeason) != seasonRules.end()) {
            return seasonRules[currentSeason];
        }
        return -1;
    }
};
#endif // __GAME_MODELS_H__
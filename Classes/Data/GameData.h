#pragma once
// 游戏的大管家

#ifndef __GAME_DATA_H__
#define __GAME_DATA_H__

#include "GameModels.h"
#include <map>
#include <string>

class GameData {
private:
    static GameData* _instance;

    // 数据存储池
    std::map<std::string, ItemModel*> _itemModels;
    std::map<std::string, CropModel*> _cropModels;
    std::map<std::string, AnimalModel*> _animalModels;
    std::map<std::string, FishModel*> _fishModels;

    // 私有构造，防止外部创建
    GameData();

public:
    static GameData* getInstance();

    // 初始化所有数据（在游戏启动时调用一次）
    void loadAllData();

    // 获取对应的 Model 指针
    // 如果找不到返回 nullptr，使用前最好检查一下
    ItemModel* getItemModel(const std::string& id);
    CropModel* getCropModel(const std::string& id);
    AnimalModel* getAnimalModel(const std::string& id);
    FishModel* getFishModel(const std::string& id);
};

#endif // __GAME_DATA_H__
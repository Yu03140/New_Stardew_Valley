#pragma once
#ifndef __GETABLE_ITEM_MODEL_H__
#define __GETABLE_ITEM_MODEL_H__

#include "cocos2d.h"
#include <string>
#include <unordered_map>

// 1. 享元类：只包含不随实例改变的数据
class GetableItemModel {
private:
    std::string _id;            // 例如 "stones"
    std::string _dropItem;      // 例如 "stone"
    std::string _requiredTool;  // 例如 "Pick"
    int _maxClicks;             // 例如 10

public:
    GetableItemModel(std::string id, std::string drop, std::string tool, int clicks)
        : _id(id), _dropItem(drop), _requiredTool(tool), _maxClicks(clicks) {
    }

    // 只读访问器
    const std::string& getID() const { return _id; }
    const std::string& getDropItem() const { return _dropItem; }
    const std::string& getRequiredTool() const { return _requiredTool; }
    int getMaxClicks() const { return _maxClicks; }
};

// 2. 享元工厂：负责创建和缓存享元对象
class ItemModelFactory {
private:
    static ItemModelFactory* _instance;
    std::map<std::string, GetableItemModel*> _modelPool;

public:
    static ItemModelFactory* getInstance() {
        if (!_instance) _instance = new ItemModelFactory();
        return _instance;
    }

    GetableItemModel* getModel(const std::string& name) {
        // 如果池里已经有了，直接返回
        if (_modelPool.find(name) != _modelPool.end()) {
            return _modelPool[name];
        }

        // 如果没有，创建新的（这里硬编码了配置，实际可以从JSON读取）
        // 原来的 GOODS_MAP 和 GOODS_CLICK_MAP 数据移到这里
        std::string drop = "";
        std::string tool = "";
        int clicks = 1;

        if (name == "grass") { drop = "straw"; tool = "Hoe"; clicks = 6; }
        else if (name == "stones") { drop = "stone"; tool = "Pick"; clicks = 10; }
        else if (name == "bigstone") { drop = "stone"; tool = "Pick"; clicks = 20; }
        else if (name == "mine") { drop = "copper"; tool = "Pick"; clicks = 30; }
        else if (name == "tree") { drop = "wood"; tool = "Axe"; clicks = 20; }
        else if (name == "badGreenhouse") { drop = ""; tool = ""; clicks = 60; }

        GetableItemModel* newModel = new GetableItemModel(name, drop, tool, clicks);
        _modelPool[name] = newModel;
        return newModel;
    }
};

// 初始化静态成员（通常放在.cpp里，简化起见放在这里，如果不通过编译请移至cpp）
// ItemModelFactory* ItemModelFactory::_instance = nullptr; 

#endif
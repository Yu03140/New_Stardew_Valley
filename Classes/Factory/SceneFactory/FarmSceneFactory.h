// FarmSceneFactory.h
#ifndef __FARM_SCENE_FACTORY_H__
#define __FARM_SCENE_FACTORY_H__

#include "SceneFactory.h"
#include "GetableItem/Crop.h"
#include "GetableItem/Fish.h"
#include "NPC/NPC.h"
#include "Board/Board.h"
#include "TaskBar/TaskBarLayer.h"
#include "Shop/Shop_board_Layer.h"
#include "Moveable/moveable_sprite_key.h"
#include <vector>

// FarmScene的具体产品类
class FarmSceneProduct : public SceneBase {
public:
    CREATE_FUNC(FarmSceneProduct);
    
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExit() override;
    virtual void update(float delta) override;
    
    // 实现抽象方法
    virtual SceneType getSceneType() const override { return SceneType::FARMGROUND_SCENE; }
    virtual std::string getSceneName() const override { return "FarmScene"; }
    
    // FarmScene特有方法
    void addItem(const std::string& itemName);
    void replaceHouseImage();
    bool checkForwarmhouseClick(cocos2d::Vec2 mousePosition);
    
    // 初始化FarmScene特有的组件（由工厂方法调用）
    void initFarmSpecificComponents();
    
protected:
    virtual void setupPlayer() override;
    virtual void setupUI() override;
    virtual void onMouseClick(cocos2d::Event* event) override;
    
private:
    void updateItemTexture(int slotIndex);
    void clearItemTexture(int slotIndex);
    void updateCameraPosition(float dt, cocos2d::Node* player);
    void init_mouselistener();
    void on_mouse_click(cocos2d::Event* event);
    void checkForButtonClick(cocos2d::Vec2 mousePosition);
    void checkNPCInteraction();
    
    // FarmScene特有成员
    struct Crops {
        std::string name;
        crop* sprite;
    };
    struct Fish {
        std::string name;
        fish* sprite;
    };
    
    std::vector<Crops> crops;
    std::vector<Fish> fish;
    NPC* npc1 = nullptr;
    NPC* npc2 = nullptr;
    Board* board = nullptr;
    TaskBarLayer* taskBarLayer = nullptr;
    Shop_Board_Layer* shop_board_layer = nullptr;
    cocos2d::Sprite* house = nullptr;
    cocos2d::Sprite* shed = nullptr;
    cocos2d::Sprite* badWarmHouse = nullptr;
    moveable_sprite_key_walk* sprite_move = nullptr;
    moveable_sprite_key_tool* sprite_tool = nullptr;
};

// FarmScene的具体工厂类
class FarmSceneFactory : public SceneFactory {
public:
    virtual ~FarmSceneFactory() = default;
    
    // 实现工厂方法
    virtual cocos2d::Scene* createScene() override;
    virtual cocos2d::Scene* createSceneWithParams(const SceneCreateParams& params) override;
    
    // 实现抽象方法
    virtual std::string getSceneName() const override { return "FarmScene"; }
    virtual SceneType getSceneType() const override { return SceneType::FARMGROUND_SCENE; }
    
protected:
    // 实现创建具体产品的工厂方法
    virtual SceneBase* createSceneBase() override;
    
    // FarmScene特有的初始化逻辑
    virtual void initSceneSpecific(SceneBase* scene, const SceneCreateParams& params);
};

#endif // __FARM_SCENE_FACTORY_H__


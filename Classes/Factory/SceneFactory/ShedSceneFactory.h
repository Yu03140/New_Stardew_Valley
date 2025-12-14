// ShedSceneFactory.h
#ifndef __SHED_SCENE_FACTORY_H__
#define __SHED_SCENE_FACTORY_H__

#include "SceneFactory.h"
#include "GetableItem/animals.h"
#include "Moveable/moveable_sprite_key.h"

// ShedScene的具体产品类
class ShedSceneProduct : public SceneBase {
public:
    CREATE_FUNC(ShedSceneProduct);
    
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float delta) override;
    
    // 实现抽象方法
    virtual SceneType getSceneType() const override { return SceneType::SHED_SCENE; }
    virtual std::string getSceneName() const override { return "ShedScene"; }
    
    // 初始化ShedScene特有的组件（由工厂方法调用）
    void initShedSpecificComponents();
    
protected:
    virtual void setupPlayer() override;
    virtual void setupUI() override;
    virtual void onMouseClick(cocos2d::Event* event) override;
    
private:
    void initSceneSpecific();
    void initAnimals();
    bool checkDoorClick(const cocos2d::Vec2& clickLocation);
    void createDoorDebugSprite(float x, float y, float width, float height);
    
    // ShedScene特有成员
    AnimalsManager* animals_manager = nullptr;

    moveable_sprite_key_walk* sprite_move = nullptr;
    moveable_sprite_key_tool* sprite_tool = nullptr;
};

// ShedScene的具体工厂类
class ShedSceneFactory : public SceneFactory {
public:
    virtual ~ShedSceneFactory() = default;
    
    // 实现工厂方法
    virtual cocos2d::Scene* createScene() override;
    virtual cocos2d::Scene* createSceneWithParams(const SceneCreateParams& params) override;
    
    // 实现抽象方法
    virtual std::string getSceneName() const override { return "ShedScene"; }
    virtual SceneType getSceneType() const override { return SceneType::SHED_SCENE; }
    
protected:
    // 实现创建具体产品的工厂方法
    virtual SceneBase* createSceneBase() override;
    
    // ShedScene特有的初始化逻辑
    virtual void initSceneSpecific(SceneBase* scene, const SceneCreateParams& params);
};

#endif // __SHED_SCENE_FACTORY_H__


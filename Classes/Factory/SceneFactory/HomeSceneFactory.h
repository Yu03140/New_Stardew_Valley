// HomeSceneFactory.h
#ifndef __HOME_SCENE_FACTORY_H__
#define __HOME_SCENE_FACTORY_H__

#include "SceneFactory.h"
#include "Charactor/RecipeLayer.h"

// HomeScene的具体产品类
class HomeSceneProduct : public SceneBase {
public:
    CREATE_FUNC(HomeSceneProduct);
    
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float delta) override;
    
    // 实现抽象方法
    virtual SceneType getSceneType() const override { return SceneType::HOME_SCENE; }
    virtual std::string getSceneName() const override { return "HomeScene"; }
    
    // 初始化HomeScene特有的组件（由工厂方法调用）
    void initHomeSpecificComponents();
    
protected:
    virtual void setupPlayer() override;
    virtual void setupUI() override;
    virtual void onMouseClick(cocos2d::Event* event) override;
    
private:
    void changeScene(cocos2d::Event* event);

    moveable_sprite_key_walk* sprite_move = nullptr;
    moveable_sprite_key_tool* sprite_tool = nullptr;
};

// HomeScene的具体工厂类
class HomeSceneFactory : public SceneFactory {
public:
    virtual ~HomeSceneFactory() = default;
    
    // 实现工厂方法
    virtual cocos2d::Scene* createScene() override;
    virtual cocos2d::Scene* createSceneWithParams(const SceneCreateParams& params) override;
    
    // 实现抽象方法
    virtual std::string getSceneName() const override { return "HomeScene"; }
    virtual SceneType getSceneType() const override { return SceneType::HOME_SCENE; }
    
protected:
    // 实现创建具体产品的工厂方法
    virtual SceneBase* createSceneBase() override;
    
    // HomeScene特有的初始化逻辑
    virtual void initSceneSpecific(SceneBase* scene, const SceneCreateParams& params);
};

#endif // __HOME_SCENE_FACTORY_H__


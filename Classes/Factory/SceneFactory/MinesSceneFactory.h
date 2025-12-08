// MinesSceneFactory.h
#ifndef __MINES_SCENE_FACTORY_H__
#define __MINES_SCENE_FACTORY_H__

#include "SceneFactory.h"
#include "GetableItem/getable_goods.h"

// MinesScene的具体产品类
class MinesSceneProduct : public SceneBase {
public:
    CREATE_FUNC(MinesSceneProduct);
    
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float delta) override;
    
    // 实现抽象方法
    virtual SceneType getSceneType() const override { return SceneType::MINES_SCENE; }
    virtual std::string getSceneName() const override { return "MinesScene"; }
    
    // 初始化MinesScene特有的组件（由工厂方法调用）
    void initMinesSpecificComponents();
    
protected:
    virtual void setupPlayer() override;
    virtual void setupUI() override;
    virtual void onMouseClick(cocos2d::Event* event) override;
    
private:
    void changeScene(cocos2d::Event* event);
    void initStones();
};

// MinesScene的具体工厂类
class MinesSceneFactory : public SceneFactory {
public:
    virtual ~MinesSceneFactory() = default;
    
    // 实现工厂方法
    virtual cocos2d::Scene* createScene() override;
    virtual cocos2d::Scene* createSceneWithParams(const SceneCreateParams& params) override;
    
    // 实现抽象方法
    virtual std::string getSceneName() const override { return "MinesScene"; }
    virtual SceneType getSceneType() const override { return SceneType::MINES_SCENE; }
    
protected:
    // 实现创建具体产品的工厂方法
    virtual SceneBase* createSceneBase() override;
    
    // MinesScene特有的初始化逻辑
    virtual void initSceneSpecific(SceneBase* scene, const SceneCreateParams& params);
};

#endif // __MINES_SCENE_FACTORY_H__


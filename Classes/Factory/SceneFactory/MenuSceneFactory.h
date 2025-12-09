// MenuSceneFactory.h
#ifndef __MENU_SCENE_FACTORY_H__
#define __MENU_SCENE_FACTORY_H__

#include "SceneFactory.h"
#include "SceneBase.h"
#include "cocos2d.h"

// MenuScene的具体产品类（统一继承SceneBase以保持产品类的一致性）
class MenuSceneProduct : public SceneBase {
public:
    CREATE_FUNC(MenuSceneProduct);
    
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
    
    // 实现抽象方法
    virtual SceneType getSceneType() const override { return SceneType::HENU_SCENE; }
    virtual std::string getSceneName() const override { return "MenuScene"; }
    
    // MenuScene特有方法
    void menuCloseCallback(cocos2d::Ref* pSender);
    void onMenuItemClicked(cocos2d::Ref* sender);
};

// MenuScene的具体工厂类
class MenuSceneFactory : public SceneFactory {
public:
    virtual ~MenuSceneFactory() = default;
    
    // 实现工厂方法
    virtual cocos2d::Scene* createScene() override;
    virtual cocos2d::Scene* createSceneWithParams(const SceneCreateParams& params) override;
    
    // 实现抽象方法
    virtual std::string getSceneName() const override { return "MenuScene"; }
    virtual SceneType getSceneType() const override { return SceneType::HENU_SCENE; }
    
protected:
    // 实现创建具体产品的工厂方法
    virtual SceneBase* createSceneBase() override;
};

#endif // __MENU_SCENE_FACTORY_H__


#ifndef __HOME_SCENE_H__
#define __HOME_SCENE_H__

#include "SubScene.h"
#include "cocos2d.h"
#include "Charactor/RecipeLayer.h"

class HomeScene : public SubScene{
public:

    // 添加一个初始化地图的函数
    void initTileMap() override;
    
	// 实现单例模式
    static HomeScene* getInstance();
    static cocos2d::Scene* createScene();

    // 重写update方法，便于扩展动态逻辑
    virtual void update(float delta) override;

    // 鼠标点击事件处理
    void changeScene(cocos2d::Event* event);

    // 使用CREATE_FUNC宏定义create方法
    CREATE_FUNC(HomeScene);

protected:
    static HomeScene* instance;
}; 

#endif // __HOME_SCENE_H__

#ifndef __SHED_SCENE_H__
#define __SHED_SCENE_H__

#include "cocos2d.h"
#include "SubScene.h"
#include "GetableItem/animals.h"
class ShedScene : public SubScene {
public:

    // 添加一个初始化地图的函数
    void initTileMap() override;

    // 实现单例模式
    static ShedScene* getInstance();
    static cocos2d::Scene* createScene();

    bool init();

    // 重写update方法，便于扩展动态逻辑
    virtual void update(float delta) override {}

    // 鼠标点击事件处理
    void changeScene(cocos2d::Event* event);

    // 使用CREATE_FUNC宏定义create方法
    CREATE_FUNC(ShedScene);

protected:
    static ShedScene* instance;
    AnimalsManager* animals_manager;
};

#endif // __SHED_SCENE_H__
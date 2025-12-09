#ifndef __MINES_SCENE_H__
#define __MINES_SCENE_H__

#include "cocos2d.h"
#include "FarmGround.h"
#include "SubScene.h"
#include "GetableItem/getable_goods.h"

class MinesScene : public SubScene
{
public:

    static MinesScene* getInstance();
    static Scene* createScene();

    // 添加一个初始化地图的函数
    void initTileMap();

    // 鼠标点击事件处理
    void changeScene(cocos2d::Event* event);

    // 使用CREATE_FUNC宏定义create方法
    CREATE_FUNC(MinesScene);

protected:

    static MinesScene* instance;
};

#endif // __MINES_SCENE_H__
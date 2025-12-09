#ifndef __MENUSCENE_SCENE_H__
#define __MENUSCENE_SCENE_H__

#include "cocos2d.h"
#include "Charactor/BackpackLayer.h"
#include "Global/Global.h"


class MenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void onMenuItemClicked(cocos2d::Ref* sender);
    // implement the "static create()" method manually
    CREATE_FUNC(MenuScene);
};

#endif // __HELLOWORLD_SCENE_H__

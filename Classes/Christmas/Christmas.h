#pragma once
#ifndef __CHRISTMAS_SCENE_H__
#define __CHRISTMAS_SCENE_H__

#include "cocos2d.h"

class ChristmasScene : public cocos2d::Scene {
public:
    // 构造函数和析构函数
    ChristmasScene();
    ~ChristmasScene();

    // 创建场景的静态方法
    static ChristmasScene* createScene();

    // 初始化方法
    virtual bool init();


    // 领取奖励按钮点击事件
    void onGetRewardClicked(cocos2d::Ref* sender);

    // 返回主场景
    void goBackToMainScene();


    void createFixedText();
    void updateTextPerCharacter(float dt);

private:
    cocos2d::Sprite* _background;       // 圣诞背景
    cocos2d::Sprite* _talkBoard;        // 对话框
    cocos2d::Label* _scrollingText;     // 滚动文本
    cocos2d::MenuItemImage* _getButton; // 领取按钮
    bool _isTextScrolling;              // 文本是否正在滚动
    float _scrollingSpeed;              // 滚动速度
    std::string _message;               // 滚动的消息
};

#endif // __CHRISTMAS_SCENE_H__


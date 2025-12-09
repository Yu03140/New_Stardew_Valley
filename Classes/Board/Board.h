#pragma once
#ifndef __BOARD_H__
#define __BOARD_H__

#include "cocos2d.h"

class Board : public cocos2d::Layer {
public:
    static Board* createBoard(const std::string& weather, int money, int experience);
    virtual bool init(const std::string& weather, int money, int experience);

    void updateWeather(const std::string& weather);  // 更新天气
    void updateTimeLabel(float dt);
    void updateMoneyLabel();
    void updateExperienceLabel();

private:
    // 私有成员变量
    cocos2d::Sprite* background;        // 背景图片
    cocos2d::Sprite* weatherSprite;     // 天气图片
    cocos2d::Label* moneyLabel;         // 显示玩家钱财的标签
    cocos2d::Label* timeLabel;          // 显示年月日时间的标签
    cocos2d::Label* experienceLabel;    //显示玩家经验值的标签

    // 工具函数
    void setWeatherSprite(const std::string& weather);

};

#endif // __BOARD_H__


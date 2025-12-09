#pragma once
#include "cocos2d.h"

class NightTransitionScene : public cocos2d::Scene {
public:
    // 构造函数
    NightTransitionScene() {}

    // 创建场景
    static NightTransitionScene* createScene() {
        NightTransitionScene* ret = new NightTransitionScene();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        else {
            delete ret;
            return nullptr;
        }
    }

    // 初始化函数
    bool init() {
        if (!Scene::init()) {
            return false;
        }

        // 获取屏幕尺寸
        auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

        // 创建全屏的 night.png 图片
        auto nightOverlay = cocos2d::Sprite::create("night.png");
        if (!nightOverlay) {
            CCLOG("Failed to load night.png");
            return false;
        }

        // 调整图片大小以适应整个屏幕
        nightOverlay->setContentSize(visibleSize);
        nightOverlay->setPosition(visibleSize.width, visibleSize.height / 2);  // 初始位置在屏幕右侧外面

        // 将图片添加到场景中
        this->addChild(nightOverlay, 100);

        // 创建从右至左的动画
        auto moveLeft = cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(-visibleSize.width/2, 0));  // 向左移动整个屏幕宽度
        auto delay = cocos2d::DelayTime::create(3.0f);  // 停留3秒
        auto removeOverlay = cocos2d::CallFunc::create([nightOverlay]() {
            nightOverlay->removeFromParent();  // 移除夜晚遮罩
            });

        // 动画序列：夜晚图片向左滚动 -> 停留3秒 -> 移除图片
        nightOverlay->runAction(cocos2d::Sequence::create(moveLeft, delay, removeOverlay, nullptr));

        // 在3秒延迟后返回到原场景
        this->scheduleOnce([this](float dt) {
            // 在转场结束后返回原场景
            cocos2d::Director::getInstance()->popScene();
            }, 5.0f, "return_scene_key");

        return true;
    }
};


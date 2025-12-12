#pragma once
#ifndef __GAME_ENGINE_H__
#define __GAME_ENGINE_H__

#include "cocos2d.h"
#include "Factory/SceneFactory/SceneFactory.h"


/**
 * [外观模式] GameEngine
 * 职责：
 * 1. 封装 Cocos2d-x 底层系统 (Director, GLView, Audio)
 * 2. 统一管理游戏流程 (启动、切换场景)
 * 3. 解决场景切换时的资源清理和 UI 保持问题
 */
class GameEngine
{
private:
    static GameEngine* _instance;
    GameEngine();

public:
    static GameEngine* getInstance();

    // 1. 初始化系统 (屏幕适配、GLView等)
    void initSystem();

    // 2. 启动游戏
    void startGame();

    // 3. 切换场景 (核心：修复 pushScene 导致的内存问题 + 自动处理背包层)
    void changeScene(SceneType type);
};

#endif // __GAME_ENGINE_H__
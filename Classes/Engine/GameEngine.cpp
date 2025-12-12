#include "GameEngine.h"
#include "Global/Global.h" 
#include "TaskBar/TaskBarLayer.h" 

USING_NS_CC;

GameEngine* GameEngine::_instance = nullptr;

GameEngine* GameEngine::getInstance()
{
    if (!_instance)
    {
        _instance = new GameEngine();
    }
    return _instance;
}

GameEngine::GameEngine()
{
}

void GameEngine::initSystem()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

    if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("New Stardew Valley", Rect(0, 0, 1024, 768));
#else
        glview = GLViewImpl::create("New Stardew Valley");
#endif
        director->setOpenGLView(glview);
    }

    director->setDisplayStats(true);
    director->setAnimationInterval(1.0f / 60);

    // --- 你的原始屏幕适配逻辑 ---
    Size designResolutionSize = Size(1024, 768);
    Size smallResolutionSize = Size(800, 600);
    Size mediumResolutionSize = Size(1600, 1200);
    Size largeResolutionSize = Size(2048, 1536);

    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);

    auto frameSize = glview->getFrameSize();
    if (frameSize.height > mediumResolutionSize.height) {
        director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
    }
    else if (frameSize.height > smallResolutionSize.height) {
        director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
    }
    else {
        director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
    }

    // 注册所有场景工厂
    SceneFactoryRegistry::registerAllFactories();
}

void GameEngine::startGame()
{
    // 这里要注意：你的代码里写的是 HENU_SCENE (如果是拼写错误请自行修改为 MENU_SCENE)
    auto scene = SceneFactoryManager::getInstance()->createScene(SceneType::HENU_SCENE);

    if (scene) {
        Director::getInstance()->runWithScene(scene);
    }
    else {
        CCLOG("Error: Failed to create start scene!");
    }
}

// 【核心修复】安全的场景切换
void GameEngine::changeScene(SceneType type)
{
    auto director = Director::getInstance();

    // 1. 保护 BackpackLayer (背包层)
    // 在销毁旧场景前，先把背包层拿出来，不然它会被一起销毁
    auto backpack = BackpackLayer::getInstance();
    if (backpack && backpack->getParent()) {
        backpack->removeFromParent();
        // 确保背包层不会因为引用计数归零而被释放，通常单例模式自己会持有引用
    }

    // 2. 创建新场景
    auto newScene = SceneFactoryManager::getInstance()->createScene(type);
    if (!newScene) {
        CCLOG("Error: Failed to create scene type: %d", (int)type);
        return;
    }

    // 3. 切换场景 (使用 replaceScene 释放旧内存)
    director->replaceScene(TransitionFade::create(0.5f, newScene));
}
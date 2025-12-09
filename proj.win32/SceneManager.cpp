#include "SceneManager.h"
#include "MenuScene.h"    // 引入具体的场景类头文件
#include "FarmScene.h"
//#include "HomeScene.h"
#include "MinesScene.h"
//#include "ShedScene.h"
#include <iostream>

// 构造函数：加载所有场景
SceneManager::SceneManager() {
    // 加载场景
    loadScene(std::make_shared<MenuScene>());
    loadScene(std::make_shared<FarmScene>());
    loadScene(std::make_shared<HomeScene>());
    loadScene(std::make_shared<MinesScene>());
    loadScene(std::make_shared<ShedScene>());
}

// 加载一个场景
void SceneManager::loadScene(const std::shared_ptr<Scene>& scene) {
    scenes.push_back(scene);
}

// 切换场景
void SceneManager::switchScene(const std::string& sceneName) {
    // 查找并切换到指定名称的场景
    for (auto& scene : scenes) {
        if (scene->getName() == sceneName) {
            currentScene = scene;
            currentScene->init();  // 初始化场景
            std::cout << "Switched to " << sceneName << std::endl;
            break;
        }
    }
}

// 更新当前场景
void SceneManager::update() {
    if (currentScene) {
        currentScene->update();
    }
}

// 渲染当前场景
void SceneManager::render() {
    if (currentScene) {
        currentScene->render();
    }
}

// 析构函数
SceneManager::~SceneManager() {
    // 智能指针会自动处理资源释放，不需要手动释放
}

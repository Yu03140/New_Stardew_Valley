#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <memory>
#include <vector>
#include <string>
#include "Scene.h"  // 引入基类 Scene 的头文件

// 场景管理器类声明
class SceneManager {
private:
    std::shared_ptr<Scene> currentScene;  // 当前活动的场景
    std::vector<std::shared_ptr<Scene>> scenes;  // 所有加载的场景

public:
    // 构造函数：加载所有场景
    SceneManager();

    // 加载一个场景
    void loadScene(const std::shared_ptr<Scene>& scene);

    // 切换到指定的场景
    void switchScene(const std::string& sceneName);

    // 更新当前场景
    void update();

    // 渲染当前场景
    void render();

    // 析构函数
    ~SceneManager();
};

#endif // SCENEMANAGER_H

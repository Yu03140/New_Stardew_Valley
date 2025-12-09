#include "TaskBarLayer.h"
#include "Global/Global.h"
#include "Scene/Farmground.h"

cocos2d::Scene* TaskBarLayer::createScene() {
    auto scene = cocos2d::Scene::create(); // 创建一个新场景
    auto layer = TaskBarLayer::create();  // 创建任务栏层
    scene->addChild(layer); // 将任务栏层添加到场景中
    return scene; // 返回场景
}

bool TaskBarLayer::init() {
    if (!Layer::init()) { // 调用父类初始化方法
        return false; // 如果失败，则返回 false
    }

    // 创建 TaskBar
    taskBar = TaskBar::create();
    //第一个
    taskBar->addTask("Collect one strawberry\nfor Fizzer.", [this]() {
        if (backpackLayer->removeItem("strawberry", 1)) {
            auto player = Player::getInstance();
            player->playerproperty.addExperience(50);
            player->playerproperty.addMoney(50);
            cocos2d::log("First task completed: 50 XP and 50$ added.");

            // 标记任务完成
            taskBar->taskCompleted = true; // 显式设置任务完成标志
        }
        else {
            cocos2d::log("Failed to complete the first task: Strawberry not found!");
        }
        });
    taskBar->showNextTask();

    taskBar->addTask("Repair the house\nfor Bouncer, please.", [this]() {
        if (1) {
            auto player = Player::getInstance();
            player->playerproperty.addExperience(50);
            player->playerproperty.addMoney(50);

            this->taskBar->taskCompleted = true; // 使用 this 捕获后访问成员变量
            cocos2d::log("Second task completed: 50 XP and 50$ added.");
        }

        });


    // 设置任务栏到屏幕中心
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    taskBar->setAnchorPoint(Vec2(0.5, 0.5));
    taskBar->setPosition(cocos2d::Vec2(visibleSize.width / 2 - visibleSize.width * (0.1), visibleSize.height / 2 - visibleSize.height * (0.05)));
    taskBar->setVisible(false); // 初始不可见
    taskBar->setScale(4.0);
    this->addChild(taskBar, 2); // 将任务栏添加到当前层中

    // 创建按钮，用于显示任务栏
    auto showTaskBarButton = cocos2d::MenuItemImage::create(
        "TaskBarOrigin.png", // 按钮的正常状态图像
        "TaskBarOrigin.png", // 按钮的选中状态图像
        [this](Ref* sender) { // 使用 [this] 捕获当前类实例指针
            this->taskBar->show(); // 显示任务栏
        });

    showTaskBarButton->setScale(1.5);
    showTaskBarButton->setPosition(cocos2d::Vec2(visibleSize.width * 0.72, visibleSize.height * 0.6)); // 设置按钮位置
    showTaskBarButton->setOpacity(255); // 设置完全不透明


    // 创建菜单并将按钮添加到菜单中
    auto menu = cocos2d::Menu::create(showTaskBarButton, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO + Vec2(visibleSize.width * (-0.01), visibleSize.width * (-0.05))); // 设置菜单位置为原点
    menu->setScale(1.5);
    this->addChild(menu, 2); // 将菜单添加到当前层中

    return true; // 初始化成功
}
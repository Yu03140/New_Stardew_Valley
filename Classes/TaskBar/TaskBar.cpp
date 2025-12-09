#include "TaskBar.h"
#include "Charactor/BackpackLayer.h"
#include "Global/Global.h"

// 创建任务栏对象
TaskBar* TaskBar::create() {
    TaskBar* taskBar = new (std::nothrow) TaskBar();
    if (taskBar && taskBar->init()) {
        taskBar->autorelease();
        return taskBar;
    }
    delete taskBar;
    return nullptr;
}

// 初始化任务栏
bool TaskBar::init() {
    if (!Node::init()) {
        return false;
    }

    this->taskCompleted = false;

    // 创建任务栏背景图片
    auto background = cocos2d::Sprite::create("HelpWanted.png");
    background->setScale(0.5);
    this->addChild(background);

    // 创建任务描述标签
    descriptionLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 15);
    descriptionLabel->setTextColor(cocos2d::Color4B::BLACK);
    descriptionLabel->setPosition(cocos2d::Vec2(background->getContentSize().width / 2,
        background->getContentSize().height / 3 * 2));
    descriptionLabel->setLocalZOrder(10);
    background->addChild(descriptionLabel);

    // 创建“完成任务”按钮
    completeTaskButton = cocos2d::MenuItemImage::create(
        "done_button.png", "done_button.png",
        CC_CALLBACK_0(TaskBar::onCompleteTask, this));
    completeTaskButton->setPosition(cocos2d::Vec2(background->getContentSize().width / 2,
        background->getContentSize().height / 3));

    // 创建“关闭”按钮
    closeButton = cocos2d::MenuItemImage::create(
        "XforTaskBar.png", "XforTaskBar.png",
        CC_CALLBACK_0(TaskBar::onClose, this));
    closeButton->setPosition(cocos2d::Vec2(background->getContentSize().width - 30,
        background->getContentSize().height - 30));

    // 创建菜单并添加按钮
    auto menu = cocos2d::Menu::create(completeTaskButton, closeButton, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO);
    background->addChild(menu);

    return true;
}

// 添加任务到任务队列
void TaskBar::addTask(const std::string& taskDescription, std::function<void()> completeTaskCallback) {
    taskQueue.push({ taskDescription, completeTaskCallback });

    // 如果当前没有任务正在进行，则立即显示第一个任务
    //if (taskQueue.size() == 1 && !taskCompleted) {
        //showNextTask();
    //}
}


// 显示下一个任务
void TaskBar::showNextTask() {
    if (!taskQueue.empty()) {
        // 从队列中取出下一个任务
        currentTask = taskQueue.front();
        taskQueue.pop();

        // 更新任务描述
        descriptionLabel->setString(currentTask.description);
        taskCompleted = false; // 重置任务完成标志
        completeTaskButton->setVisible(true); // 确保按钮可见
    }
    else {
        // 如果没有任务了，显示完成状态
        descriptionLabel->setString("Done!"); // 所有任务完成
        completeTaskButton->setVisible(false); // 隐藏完成按钮
    }
}


void TaskBar::onCompleteTask() {
    // 如果当前任务未完成，执行当前任务的回调
    if (!taskCompleted) {
        if (currentTask.completeCallback) {
            currentTask.completeCallback(); // 执行任务的回调
        }
        taskCompleted = true; // 标记当前任务完成
    }

    // 如果任务已完成，切换到下一个任务
    if (taskCompleted) {
        showNextTask(); // 显示下一个任务
    }
}


// “关闭”按钮的回调函数
void TaskBar::onClose() {
    this->hide();
}

// 显示任务栏
void TaskBar::show() {
    this->setVisible(true);
}

// 隐藏任务栏
void TaskBar::hide() {
    this->setVisible(false);
}
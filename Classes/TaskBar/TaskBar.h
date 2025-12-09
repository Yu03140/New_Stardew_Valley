#ifndef TASKBAR_H
#define TASKBAR_H

#include "cocos2d.h"
#include <string>
#include <queue>
#include <functional>

class TaskBar : public cocos2d::Node {
public:
   
    static TaskBar* create();
    virtual bool init();
    void show();
    void hide();
    void addTask(const std::string& taskDescription, std::function<void()> completeTaskCallback);
    bool taskCompleted = false; ///< 当前任务是否完成的标志。

    void showNextTask();

private:
    struct Task {
        std::string description; ///< 任务的描述文字。
        std::function<void()> completeCallback; ///< 完成任务的回调函数。
    };

    std::queue<Task> taskQueue; ///< 任务队列。
    Task currentTask; ///< 当前任务。


    cocos2d::Label* descriptionLabel; ///< 显示任务描述的标签。
    cocos2d::MenuItemImage* completeTaskButton; ///< “完成任务”按钮。
    cocos2d::MenuItemImage* closeButton; ///< “关闭”按钮。
    void onCompleteTask();
    void onClose();
};

#endif // TASKBAR_H
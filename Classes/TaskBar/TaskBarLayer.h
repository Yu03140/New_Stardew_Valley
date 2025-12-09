#ifndef TASKBARLAYER_H
#define TASKBARLAYER_H

#include "cocos2d.h"
#include "TaskBar.h"

class TaskBarLayer : public cocos2d::Layer {
public:
    virtual bool init();
    CREATE_FUNC(TaskBarLayer);
    static cocos2d::Scene* createScene();

private:
    TaskBar* taskBar;

};

#endif // TASKBARLAYER_H
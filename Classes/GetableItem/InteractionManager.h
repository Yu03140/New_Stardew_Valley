#pragma once
#ifndef INTERACTION_MANAGER_H
#define INTERACTION_MANAGER_H

#include "cocos2d.h"
#include "IInteractable.h"
#include <vector>

class InteractionManager {
private:
    static InteractionManager* _instance;
    std::vector<IInteractable*> _subscribers;
    bool _isListening;
    cocos2d::EventListenerMouse* _mouseListener; // 保存监听器指针用于停止

    InteractionManager();

public:
    static InteractionManager* getInstance();

    // 注册/注销对象
    void registerObject(IInteractable* obj);
    void unregisterObject(IInteractable* obj);

    // 启动/停止监听
    void startListening(cocos2d::EventDispatcher* dispatcher);
    void stopListening(cocos2d::EventDispatcher* dispatcher);

    // 核心点击处理
    void onMouseDown(cocos2d::Event* event);
    
    // 调试方法
    void printSubscriberInfo();
    int getSubscriberCount() { return (int)_subscribers.size(); }
};

#endif
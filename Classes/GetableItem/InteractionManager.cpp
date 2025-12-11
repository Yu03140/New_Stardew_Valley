#include "InteractionManager.h"
#include "Global/Global.h" // 引入你的全局变量
#include "./Charactor/BackpackLayer.h" // 为了获取工具

InteractionManager* InteractionManager::_instance = nullptr;

InteractionManager::InteractionManager() : _isListening(false), _mouseListener(nullptr) {}

InteractionManager* InteractionManager::getInstance() {
    if (!_instance) _instance = new InteractionManager();
    return _instance;
}

void InteractionManager::registerObject(IInteractable* obj) {
    CCLOG("[InteractionManager] Registering object: %p", obj);
    if (std::find(_subscribers.begin(), _subscribers.end(), obj) == _subscribers.end()) {
        _subscribers.push_back(obj);
        CCLOG("[InteractionManager] Object registered successfully. Total subscribers: %d", (int)_subscribers.size());
    } else {
        CCLOG("[InteractionManager] Object %p already registered", obj);
    }
}

void InteractionManager::unregisterObject(IInteractable* obj) {
    CCLOG("[InteractionManager] Unregistering object: %p", obj);
    auto it = std::remove(_subscribers.begin(), _subscribers.end(), obj);
    if (it != _subscribers.end()) {
        _subscribers.erase(it, _subscribers.end());
        CCLOG("[InteractionManager] Object unregistered successfully. Total subscribers: %d", (int)_subscribers.size());
    } else {
        CCLOG("[InteractionManager] Object %p not found in subscribers list", obj);
    }
}

void InteractionManager::startListening(cocos2d::EventDispatcher* dispatcher) {
    if (_isListening) {
        CCLOG("[InteractionManager] Already listening, skipping startListening");
        return;
    }
    CCLOG("[InteractionManager] Starting mouse event listening");
    _mouseListener = cocos2d::EventListenerMouse::create();
    _mouseListener->onMouseDown = CC_CALLBACK_1(InteractionManager::onMouseDown, this);
    // 使用较低优先级，保证 UI 层 (Backpack) 能先拦截
    dispatcher->addEventListenerWithFixedPriority(_mouseListener, 1);
    _isListening = true;
    CCLOG("[InteractionManager] Mouse event listener started successfully");
}

void InteractionManager::onMouseDown(cocos2d::Event* event) {
    CCLOG("[InteractionManager] Mouse down event received. Subscribers count: %d", (int)_subscribers.size());
    
    // 1. 准备上下文数据
    InteractContext ctx;
    ctx.isInControl = is_in_control; // 来自 Global.h
    CCLOG("[InteractionManager] isInControl: %s", ctx.isInControl ? "true" : "false");

    // 2. 解析鼠标位置 
    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    //CCLOG("[InteractionManager] is_infarm: %s", is_infarm ? "true" : "false");
    
    if (is_infarm) {
        // 在农场中，使用的是全局变量 MOUSE_POS
        ctx.mousePosWorld = MOUSE_POS;
        CCLOG("[InteractionManager] Farm mode - using MOUSE_POS: (%.2f, %.2f)", ctx.mousePosWorld.x, ctx.mousePosWorld.y);
    }
    else {
        // 在非农场，你需要把屏幕坐标转为节点空间坐标
        cocos2d::Vec2 viewPos = e->getLocationInView();
        ctx.mousePosWorld = cocos2d::Director::getInstance()->convertToGL(viewPos);
        CCLOG("[InteractionManager] Non-farm mode - ViewPos: (%.2f, %.2f), WorldPos: (%.2f, %.2f)", 
              viewPos.x, viewPos.y, ctx.mousePosWorld.x, ctx.mousePosWorld.y);
    }

    // 3. 解析工具信息 (从 BackpackLayer 获取)
    ctx.toolName = "";
    ctx.toolLevel = 0;

    if (backpackLayer) {
        std::string itemStr = backpackLayer->getSelectedItem();
        CCLOG("[InteractionManager] Selected item from backpack: '%s'", itemStr.c_str());
        
        if (!itemStr.empty()) {
            // 解析等级 (假设格式为 "Hoe1")
            char lastChar = itemStr.back();
            if (isdigit(lastChar)) {
                ctx.toolLevel = lastChar - '0';
                ctx.toolName = itemStr.substr(0, itemStr.size() - 1); // 去掉数字
                CCLOG("[InteractionManager] Parsed tool - Name: '%s', Level: %d", ctx.toolName.c_str(), ctx.toolLevel);
            }
            else {
                ctx.toolLevel = 1; // 默认等级
                ctx.toolName = itemStr;
                CCLOG("[InteractionManager] Tool without level digit - Name: '%s', Default Level: %d", ctx.toolName.c_str(), ctx.toolLevel);
            }
        } else {
            CCLOG("[InteractionManager] No item selected in backpack");
        }
    } else {
        CCLOG("[InteractionManager] BackpackLayer is null!");
    }

    // 4. 寻找被点击的对象
    IInteractable* target = nullptr;
    int maxPriority = -999;
    int checkedObjects = 0;
    int interactableObjects = 0;
    int hitObjects = 0;

    CCLOG("[InteractionManager] Starting object collision detection...");
    
    for (auto obj : _subscribers) {
        checkedObjects++;
        
        if (!obj->isInteractable()) {
            CCLOG("[InteractionManager] Object %p is not interactable, skipping", obj);
            continue;
        }
        interactableObjects++;

        // 获取对象的包围盒
        cocos2d::Rect bbox = obj->getBoundingBoxWorld();
        CCLOG("[InteractionManager] Object %p bbox: (%.2f, %.2f, %.2f, %.2f)", 
              obj, bbox.origin.x, bbox.origin.y, bbox.size.width, bbox.size.height);

        // 执行碰撞检测
        if (bbox.containsPoint(ctx.mousePosWorld)) {
            hitObjects++;
            int p = obj->getInteractPriority();
            //CCLOG("[InteractionManager] HIT! Object %p priority: %d, current max: %d", obj, p, maxPriority);
            
            if (p > maxPriority) {
                if (target) {
                    //CCLOG("[InteractionManager] Replacing previous target %p (priority %d) with %p (priority %d)", 
                          target, maxPriority, obj, p);
                }
                maxPriority = p;
                target = obj;
            }
        } else {
            CCLOG("[InteractionManager] Object %p not hit by mouse click", obj);
        }
    }
    
    //CCLOG("[InteractionManager] Collision detection complete - Checked: %d, Interactable: %d, Hit: %d, Final target: %p", 
          checkedObjects, interactableObjects, hitObjects, target);

    // 5. 触发交互
    if (target) {
        CCLOG("[InteractionManager] Triggering interaction with target %p", target);
        bool handled = target->onInteract(ctx);
        CCLOG("[InteractionManager] Interaction result: %s", handled ? "handled" : "not handled");
        
        if (handled) {
            event->stopPropagation(); // 阻止事件继续传播
            CCLOG("[InteractionManager] Event propagation stopped");
        }
    } else {
        CCLOG("[InteractionManager] No target found for interaction");
    }
    CCLOG("[InteractionManager] onMouseDown completed\n");
}

void InteractionManager::stopListening(cocos2d::EventDispatcher* dispatcher) {
    if (!_isListening || !_mouseListener) {
        CCLOG("[InteractionManager] Not listening or no listener to stop");
        return;
    }
    CCLOG("[InteractionManager] Stopping mouse event listening");
    dispatcher->removeEventListener(_mouseListener);
    _mouseListener = nullptr;
    _isListening = false;
    CCLOG("[InteractionManager] Mouse event listener stopped successfully");
}

void InteractionManager::printSubscriberInfo() {
    CCLOG("[InteractionManager] === Subscriber Info ===");
    CCLOG("[InteractionManager] Total subscribers: %d", (int)_subscribers.size());
    CCLOG("[InteractionManager] Listening state: %s", _isListening ? "active" : "inactive");
    
    for (int i = 0; i < _subscribers.size(); i++) {
        auto obj = _subscribers[i];
        CCLOG("[InteractionManager] [%d] Object: %p, Interactable: %s, Priority: %d", 
              i, obj, 
              obj->isInteractable() ? "true" : "false", 
              obj->getInteractPriority());
    }
    CCLOG("[InteractionManager] =====================");
}
#ifndef IINTERACTABLE_H
#define IINTERACTABLE_H

#include "cocos2d.h"
#include <string>

// 定义交互上下文：把所有判断需要的数据打包传进来
struct InteractContext {
    cocos2d::Vec2 mousePosWorld; // 转换后的世界坐标
    std::string toolName;        // 当前工具名称 (如 "Hoe")
    int toolLevel;               // 当前工具等级 (如 1)
    bool isInControl;            // 是否在控制范围内
};

class IInteractable {
public:
    virtual ~IInteractable() {}

    // 1. 获取对象的包围盒（必须返回世界坐标系下的 Rect）
    virtual cocos2d::Rect getBoundingBoxWorld() = 0;

    // 2. 交互优先级（数值越大越先响应，用于处理重叠）
    // 默认：Crop(0), Goods(10) - 物品通常盖在作物上面
    virtual int getInteractPriority() { return 0; }

    // 3. 核心响应方法
    // 返回 true 表示事件被处理了，不再传递给其他对象
    virtual bool onInteract(const InteractContext& ctx) = 0;

    // 4. 是否处于可交互状态（如是否可见、是否被销毁）
    virtual bool isInteractable() = 0;
};

#endif
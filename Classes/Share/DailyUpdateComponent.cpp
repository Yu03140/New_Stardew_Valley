#include "DailyUpdateComponent.h"
#include "cocos2d.h"

DailyUpdateComponent::DailyUpdateComponent()
    : currentDay(0), getDayFunc(nullptr), onDayChanged(nullptr), onDayReset(nullptr)
{
}

void DailyUpdateComponent::setup(
    std::function<int()> getDayFunc,
    std::function<void()> onDayChanged,
    std::function<void()> onDayReset)
{
    this->getDayFunc = getDayFunc;
    this->onDayChanged = onDayChanged;
    this->onDayReset = onDayReset;
    
    // 初始化当前天数
    if (getDayFunc) {
        currentDay = getDayFunc();
    }
    
    CCLOG("[DailyUpdate] Component setup completed, initial day: %d", currentDay);
}

void DailyUpdateComponent::update(float deltaTime)
{
    checkAndUpdate();
}

bool DailyUpdateComponent::checkAndUpdate()
{
    if (!getDayFunc) {
        return false;
    }
    
    int newDay = getDayFunc();
    
    // 检测天数是否变化
    if (newDay != currentDay) {
        CCLOG("[DailyUpdate] Day changed from %d to %d", currentDay, newDay);
        
        // 执行每日逻辑
        if (onDayChanged) {
            onDayChanged();
        }
        
        // 更新当前天数
        currentDay = newDay;
        
        // 执行重置逻辑
        if (onDayReset) {
            onDayReset();
        }
        
        return true;
    }
    
    return false;
}

int DailyUpdateComponent::getCurrentDay() const
{
    return currentDay;
}

void DailyUpdateComponent::setCurrentDay(int day)
{
    currentDay = day;
    CCLOG("[DailyUpdate] Current day set to %d", day);
}

void DailyUpdateComponent::reset()
{
    currentDay = 0;
    getDayFunc = nullptr;
    onDayChanged = nullptr;
    onDayReset = nullptr;
    CCLOG("[DailyUpdate] Component reset");
}

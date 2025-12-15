#ifndef __DAILY_UPDATE_COMPONENT_H__
#define __DAILY_UPDATE_COMPONENT_H__

#include <functional>

class DailyUpdateComponent
{
private:
    int currentDay;                                 // 当前记录的天数
    std::function<int()> getDayFunc;                // 获取游戏天数的函数
    std::function<void()> onDayChanged;             // 天数变化时执行的逻辑
    std::function<void()> onDayReset;               // 每日重置的逻辑（可选）

public:
    /**
     * 构造函数
     */
    DailyUpdateComponent();

    /**
     * 设置每日更新逻辑
     * @param getDayFunc 获取当前游戏天数的回调函数
     * @param onDayChanged 天数变化时执行的逻辑
     * @param onDayReset 每日重置的逻辑（可选）
     */
    void setup(
        std::function<int()> getDayFunc,
        std::function<void()> onDayChanged,
        std::function<void()> onDayReset = nullptr
    );

    /**
     * 更新函数，在游戏循环中调用
     * @param deltaTime 时间增量
     */
    void update(float deltaTime);

    /**
     * 手动触发天数检查（如果不想每帧都检查）
     * @return 如果触发了每日更新返回 true，否则返回 false
     */
    bool checkAndUpdate();

    /**
     * 获取当前记录的天数
     * @return 当前天数
     */
    int getCurrentDay() const;

    /**
     * 手动设置当前天数（用于初始化）
     * @param day 天数
     */
    void setCurrentDay(int day);

    /**
     * 重置组件（清空所有回调和状态）
     */
    void reset();
};

#endif // __DAILY_UPDATE_COMPONENT_H__

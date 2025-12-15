#ifndef __PROGRESS_TRACKER_COMPONENT_H__
#define __PROGRESS_TRACKER_COMPONENT_H__

#include <functional>

class ProgressTrackerComponent
{
private:
    int count;                                      // 累计操作次数
    std::function<bool()> condition;                // 触发条件（返回 true 时触发）
    std::function<void()> action;                   // 触发时执行的动作

public:
    /**
     * 构造函数
     * @param condition 触发条件的回调函数，返回 true 表示满足条件
     * @param action 触发时执行的动作回调函数
     */
    ProgressTrackerComponent(std::function<bool()> condition, std::function<void()> action);

    /**
     * 默认构造函数（用于延迟初始化）
     */
    ProgressTrackerComponent();

    /**
     * 设置触发条件和动作
     * @param condition 触发条件的回调函数
     * @param action 触发时执行的动作回调函数
     */
    void setup(std::function<bool()> condition, std::function<void()> action);

    /**
     * 增加计数
     * @param amount 增加的数量，默认为 1
     */
    void increment(int amount = 1);

    /**
     * 检查条件并触发动作
     * @return 如果触发了动作返回 true，否则返回 false
     */
    bool checkAndTrigger();

    /**
     * 重置计数器
     */
    void reset();

    /**
     * 获取当前计数
     * @return 当前的计数值
     */
    int getCount() const;

    /**
     * 设置计数值
     * @param value 新的计数值
     */
    void setCount(int value);
};

#endif // __PROGRESS_TRACKER_COMPONENT_H__

#ifndef __HARVEST_COMPONENT_H__
#define __HARVEST_COMPONENT_H__

#include <string>
#include <functional>

class HarvestComponent
{
private:
    std::string productName;                        // 产出物品名称
    int experienceAmount;                           // 经验值数量
    std::function<void()> onHarvestComplete;        // 收获完成后的回调（用于清理/重置）

public:
    /**
     * 构造函数
     */
    HarvestComponent();

    /**
     * 配置收获逻辑
     * @param productName 产出的物品名称
     * @param experience 给予的经验值
     * @param onComplete 收获完成后的回调（可选，用于清理状态）
     */
    void setup(
        const std::string& productName,
        int experience,
        std::function<void()> onComplete = nullptr
    );

    /**
     * 配置动态产品名称的收获逻辑
     * @param getProductFunc 获取产品名称的回调函数（支持运行时决定产品）
     * @param experience 给予的经验值
     * @param onComplete 收获完成后的回调（可选）
     */
    void setupDynamic(
        std::function<std::string()> getProductFunc,
        int experience,
        std::function<void()> onComplete = nullptr
    );

    /**
     * 执行收获
     * 1. 添加物品到背包
     * 2. 增加玩家经验值
     * 3. 调用清理回调
     * @return 是否成功执行收获
     */
    bool harvest();

    /**
     * 获取产品名称
     * @return 产品名称
     */
    std::string getProductName() const;

    /**
     * 获取经验值数量
     * @return 经验值
     */
    int getExperienceAmount() const;

    /**
     * 重置组件
     */
    void reset();

private:
    std::function<std::string()> getProductFunc;    // 动态获取产品名称的函数
};

#endif // __HARVEST_COMPONENT_H__

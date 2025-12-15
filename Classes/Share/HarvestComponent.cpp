#include "HarvestComponent.h"
#include "cocos2d.h"
#include "Global/Global.h"
#include "Player/PlayerAccount.h"

HarvestComponent::HarvestComponent()
    : productName(""), experienceAmount(0), onHarvestComplete(nullptr), getProductFunc(nullptr)
{
}

void HarvestComponent::setup(
    const std::string& productName,
    int experience,
    std::function<void()> onComplete)
{
    this->productName = productName;
    this->experienceAmount = experience;
    this->onHarvestComplete = onComplete;
    this->getProductFunc = nullptr;  // 使用静态产品名称
    
    CCLOG("[HarvestComponent] Setup: product='%s', experience=%d", productName.c_str(), experience);
}

void HarvestComponent::setupDynamic(
    std::function<std::string()> getProductFunc,
    int experience,
    std::function<void()> onComplete)
{
    this->getProductFunc = getProductFunc;
    this->experienceAmount = experience;
    this->onHarvestComplete = onComplete;
    this->productName = "";  // 动态模式下不使用静态名称
    
    CCLOG("[HarvestComponent] Setup dynamic harvest, experience=%d", experience);
}

bool HarvestComponent::harvest()
{
    // 获取产品名称（静态或动态）
    std::string actualProduct;
    if (getProductFunc) {
        actualProduct = getProductFunc();
    } else {
        actualProduct = productName;
    }
    
    if (actualProduct.empty()) {
        CCLOG("[HarvestComponent] Error: No product to harvest");
        return false;
    }
    
    CCLOG("[HarvestComponent] Harvesting: %s", actualProduct.c_str());
    
    // 1. 添加物品到背包
    if (backpackLayer) {
        backpackLayer->addItem(actualProduct);
        CCLOG("[HarvestComponent] Added '%s' to backpack", actualProduct.c_str());
    } else {
        CCLOG("[HarvestComponent] Warning: backpackLayer is null");
    }
    
    // 2. 增加玩家经验值
    Player* player = Player::getInstance("me");
    if (player) {
        player->playerproperty.addExperience(experienceAmount);
        CCLOG("[HarvestComponent] Added %d experience to player", experienceAmount);
    } else {
        CCLOG("[HarvestComponent] Warning: player instance is null");
    }
    
    // 3. 执行清理/重置回调
    if (onHarvestComplete) {
        CCLOG("[HarvestComponent] Executing harvest complete callback...");
        onHarvestComplete();
    }
    
    CCLOG("[HarvestComponent] Harvest completed successfully");
    return true;
}

std::string HarvestComponent::getProductName() const
{
    if (getProductFunc) {
        return getProductFunc();
    }
    return productName;
}

int HarvestComponent::getExperienceAmount() const
{
    return experienceAmount;
}

void HarvestComponent::reset()
{
    productName = "";
    experienceAmount = 0;
    onHarvestComplete = nullptr;
    getProductFunc = nullptr;
    CCLOG("[HarvestComponent] Component reset");
}

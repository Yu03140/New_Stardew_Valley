#include "TimeSystem.h"

USING_NS_CC;

// 静态成员变量初始化
TimeSystem* TimeSystem::instance = nullptr;

// 构造函数，初始化所有时间相关信息
TimeSystem::TimeSystem() {
    year = 2024;               // 初始年份为2024年
    day = 1;                   // 初始日期为1日
    hour = 0;                  // 初始小时为0点
    season = 1;                // 初始季节为春季（1表示春季）
    weather = "normal";        // 初始时，不是干旱日

    Node::onEnter();
    schedule([this](float deltaTime) {
        update(deltaTime);
        }, 10.0f, "update_key");  // 设置每 5 秒调用一次 update
    this->scheduleUpdate();
}

// 析构函数
TimeSystem::~TimeSystem() {
    // 此处可以清理资源，实际中这里并不需要做额外清理
}

// 获取全局实例，若不存在则创建
TimeSystem* TimeSystem::getInstance() {
    if (instance == nullptr) {
        instance = new TimeSystem();
    }

    // 确保 TimeSystem 实例被添加到当前运行的场景
    auto currentScene = Director::getInstance()->getRunningScene();
    if (instance->getParent() != currentScene) {
        //currentScene->addChild(instance);
    }

    return instance;
}


// 更新游戏时间
void TimeSystem::update(float deltaTime) {

    // 根据deltaTime和TIME_SCALE来增加小时
    hour += (deltaTime * TIME_SCALE);

    // 如果小时数超过24，代表一天结束，更新日期
    if (hour >= 24) {
        hour -= 24;  // 重置小时数
        changeDay();  // 改变一天
    }


}

// 天数更新
void TimeSystem::changeDay() {
    day++;  // 日期增加
    randomWeather();  // 随机决定是否是干旱日

    // 如果日期超过了一个月的最大天数，重置为1，并改变月份
    if (day > MAX_DAYS_IN_MONTH) {
        day = 1;
        changeSeason();
    }
    // 触发夜晚转场（在改变日期后触发）
    triggerNightTransition();  // 每天晚上转换到第二天时触发夜晚转场
}

// 季节更新
void TimeSystem::changeSeason() {
    season++;  // 月份增加
    if (season > SEASON_COUNT) {  // 如果季节大于4，重置为1，改变年份
        season = 1;
        changeYear();
    }
}

// 年份更新
void TimeSystem::changeYear() {
    year++;  // 年份增加
}

// 随机决定是否是干旱日，使用DROUGHT_DAY_PROBABILITY的概率
void TimeSystem::randomWeather() {
    // 生成一个 0 到 99 之间的随机数
    int randomValue = rand() % 100;

    if (randomValue < 60) {
        weather = "normal";  // 60% 概率是正常天气
    }
    else if (randomValue < 80) {
        weather = "rain";    // 20% 概率是雨天
    }
    else {
        weather = "dry";     // 20% 概率是干旱天气
    }
}



//void TimeSystem::checkForHoliday() {
//    // 如果已经进入过圣诞场景，不再触发
//    if (hasEnteredChristmasScene) {
//        return;
//    }
//
//    // 检查时间条件
//    if (year == 2024 && season == 1 && day == 1 && hour == 5) {
//        // 设置标志为 true，防止再次触发
//        hasEnteredChristmasScene = true;
//
//        // 切换到圣诞场景
//        auto christmasScene = ChristmasScene::createScene();
//        cocos2d::Director::getInstance()->pushScene(christmasScene);
//    }
//}

// 夜晚转场动画，每天晚上转换到第二天时触发
void TimeSystem::triggerNightTransition() {
    // 获取当前场景
    auto currentScene = Director::getInstance()->getRunningScene();

    // 将当前场景压入栈中
    Director::getInstance()->pushScene(NightTransitionScene::createScene());

    // 如果你需要在夜晚转场场景结束后执行一些逻辑，通常是返回到当前场景并继续时间计时
    // 可以通过 `scheduleOnce` 来实现，给定一个延迟，等待夜晚转场场景结束。
}
#include "Global/Global.h"

USING_NS_CC;

// 创建 Board 对象
Board* Board::createBoard(const std::string& weather, int money, int experience) {
    Board* ret = new (std::nothrow) Board();
    if (ret && ret->init(weather, money, experience)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

// 初始化函数
bool Board::init(const std::string& weather, int money, int experience) {
    if (!Layer::init()) {
        return false;
    }

    // 人物初始化
    Player* player = Player::getInstance("me");
    background = Sprite::create("Board.png");
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto mapSize = background->getContentSize();
    float X = visibleSize.width / 1.7 - mapSize.width / 4 * 3;
    float Y = visibleSize.height / 1.7 - mapSize.height / 4 * 3;
	CCLOG("X: %f, Y: %f", X, Y);
    background->setPosition(X, Y);
    this->addChild(background);

    // 添加天气图片
    weatherSprite = Sprite::create();
    this->addChild(weatherSprite);
    setWeatherSprite(weather); 
    weatherSprite->setPosition(background->getPosition()+ Vec2(background->getContentSize().height /16, background->getContentSize().height / 8));

    // 添加玩家钱财标签（背景下方）
    moneyLabel = Label::createWithTTF(std::to_string(player->playerproperty.getMoney()), "fonts/arial.ttf", 12);
    moneyLabel->setPosition(background->getPosition() - Vec2(0, background->getContentSize().height /8*3));
    moneyLabel->setColor(Color3B::BLACK);
    moneyLabel->setScale(0.5);
    this->addChild(moneyLabel);
    // 每秒更新一次钱财标签
    schedule([this](float dt) {
        // 每秒更新一次钱财标签
        this->updateMoneyLabel();
        }, 1.0f, "money_update_key");


    // 添加时间标签（背景上方）
    timeLabel = Label::createWithTTF("", "fonts/arial.ttf", 12);
    timeLabel->setPosition(background->getPosition() + Vec2(8, -3));
    timeLabel->setColor(Color3B::BLACK);
    timeLabel->setScale(0.4);
    this->addChild(timeLabel);

    // 设置更新时间的调度，每秒调用一次
    schedule([this](float dt) {
        this->updateTimeLabel(dt);
        }, 1.0f, "time_update_key");

    // 添加经验值标签
    experienceLabel = Label::createWithTTF("Exp: " + std::to_string(player->playerproperty.getExperience()), "fonts/arial.ttf", 11);
    experienceLabel->setPosition(background->getPosition() + Vec2(10- background->getContentSize().height / 8, background->getContentSize().height / 8*3-2.5));
    experienceLabel->setColor(Color3B::BLACK);
    experienceLabel->setScale(0.4);
    this->addChild(experienceLabel);
    // 每秒更新一次经验标签
    schedule([this](float dt) {
        this->updateExperienceLabel();
        }, 1.0f, "experience_update_key");



    return true;
}

void Board::updateTimeLabel(float dt) {
    std::string timeStr = std::to_string(timeSystem->getYear()) + "-" +
        std::to_string(timeSystem->getSeason()) + "-" +
        std::to_string(timeSystem->getDay()) + "-" +
        std::to_string(timeSystem->getHour());
    timeLabel->setString(timeStr);  // 更新标签显示
}



// 更新天气
void Board::updateWeather(const std::string& weather) {
    setWeatherSprite(weather);
}

// 设置天气图片
void Board::setWeatherSprite(const std::string& weather) {
    std::string imagePath;
    if (weather == "normal") {
        imagePath = "normal.png";
    }
    else if (weather == "dry") {
        imagePath = "dry.png";
    }
    else if (weather == "rain") {
        imagePath = "rain.png";
    }
    else {
        return;
    }

    weatherSprite->setTexture(imagePath);
}


// 更新玩家钱财标签
void Board::updateMoneyLabel() {
    //人物初始化
    Player* player = Player::getInstance("me");
    // 更新 moneyLabel 的文本
    moneyLabel->setString( std::to_string(player->playerproperty.getMoney()));
}


// 更新玩家经验标签
void Board::updateExperienceLabel() {
    //人物初始化
    Player* player = Player::getInstance("me");
    // 更新 moneyLabel 的文本
    experienceLabel->setString("Exp: " + std::to_string(player->playerproperty.getExperience()));
}
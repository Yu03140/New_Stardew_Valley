#include "attribute.h"
#include "cocos2d.h"  

// 初始化 Player 的单例
Player* Player::player = nullptr;

// 增加经验
void PlayerProperty::addExperience(int exp) {
    experience += exp;
    CCLOG("Experience: %d", experience);
}

// 增加钱财
void PlayerProperty::addMoney(int mon) {
    money += mon;
    CCLOG("Money: %d", money);
}

// 增加声望
void PlayerProperty::addReputation(int reputation_change) {
    reputation += reputation_change;
    CCLOG("Reputation: %d", reputation);
}

Player::Player(const std::string& username)
    : playerproperty(username) {
    CCLOG("Player %s created!", username.c_str());
}

Player* Player::getInstance(const std::string& username) {
    if (player == nullptr) {
        player = new Player(username);
    }
    return player;
}

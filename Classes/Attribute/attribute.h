#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>     
#include "cocos2d.h"  

// 玩家基本信息
class PlayerInformation {
public:
    PlayerInformation(const std::string& username, int playerId, const std::string& phoneNumber)
        : username(username), playerId(playerId), phoneNumber(phoneNumber) {}

	std::string getUsername() const { return username; }         // 获取玩家用户名
	int getPlayerId() const { return playerId; }				 // 获取玩家ID
	std::string getPhoneNumber() const { return phoneNumber; }   // 获取玩家手机号

private:
    std::string username;                                        // 玩家用户名
    int playerId;                                                // 玩家ID
    std::string phoneNumber;                                     // 玩家手机号
};

// 玩家财产属性
class PlayerProperty {
public:
    PlayerProperty(const std::string& playerName)
        : experience(0), money(0), reputation(0) {}

    // 增加经验、钱财和声望
	void addExperience(int exp);                                 // 增加经验
	void addMoney(int mon);									     // 增加钱财
	void addReputation(int reputation_change);				     // 增加声望 

    // 获取财产属性
    int getExperience() const { return experience; }
    int getMoney() const { return money; }
    int getReputation() const { return reputation; }

private:
    int experience;                                              // 经验值
    int money;                                                   // 钱财
    int reputation;                                              // 声望
};

// 玩家类：管理玩家的财产属性和信息
class Player {
public:
    static Player* getInstance(const std::string& username = "");// 获取 Player 的单例实例 
    Player(const Player&) = delete;                              // 删除拷贝构造函数和赋值操作符，确保单例不被复制
    Player& operator=(const Player&) = delete;                      
    PlayerProperty playerproperty;                               // 玩家财产属性

private:
    Player(const std::string& username);                         
    static Player* player;                                       // 唯一的 Player 实例

};

#endif // __PLAYER_H__

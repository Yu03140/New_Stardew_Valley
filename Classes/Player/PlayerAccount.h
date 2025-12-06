#ifndef PLAYER_ACCOUNT_H
#define PLAYER_ACCOUNT_H

#include <string>
#include <fstream>
#include <iostream>

class PlayerAccount {
public:
    PlayerAccount(const std::string& username, const std::string& password);

    bool createAccount();  // 创建新账号
    bool login();          // 用户登录

    std::string getUsername() const;
    bool saveProgress(const std::string& progressData);  // 保存进度
    std::string loadProgress();                         // 加载进度

private:
    std::string username;       // 用户名
    std::string password;       // 密码
    std::string saveFilePath;   // 存储路径

    bool accountExists();       // 检查账号是否存在
};

#endif // PLAYER_ACCOUNT_H

#include "PlayerAccount.h"

// 构造函数
PlayerAccount::PlayerAccount(const std::string& username, const std::string& password)
    : username(username), password(password) {
    saveFilePath = username + "_save.txt";  // 使用用户名生成唯一的保存路径
}

// 创建新账号
bool PlayerAccount::createAccount() {
    if (accountExists()) {
        std::cerr << "账号已存在!\n";
        return false;
    }

    std::ofstream accountFile(username + ".txt");
    if (accountFile.is_open()) {
        accountFile << username << "\n" << password << "\n";
        accountFile.close();
        std::cout << "账号已成功创造!\n";
        return true;
    }
    return false;
}

// 用户登录
bool PlayerAccount::login() {
    std::ifstream accountFile(username + ".txt");
    if (accountFile.is_open()) {
        std::string storedUsername, storedPassword;
        getline(accountFile, storedUsername);
        getline(accountFile, storedPassword);
        accountFile.close();

        if (storedUsername == username && storedPassword == password) {
            std::cout << "登录成功!\n";
            return true;
        }
    }
    std::cerr << "账号或密码错误!\n";
    return false;
}

// 检查账号是否存在
bool PlayerAccount::accountExists() {
    std::ifstream accountFile(username + ".txt");
    return accountFile.is_open();
}

// 保存游戏进度
bool PlayerAccount::saveProgress(const std::string& progressData) {
    std::ofstream saveFile(saveFilePath);
    if (saveFile.is_open()) {
        saveFile << progressData;
        saveFile.close();
        return true;
    }
    std::cerr << "保存游戏进度失败!\n";
    return false;
}

// 加载游戏进度
std::string PlayerAccount::loadProgress() {
    std::ifstream saveFile(saveFilePath);
    if (saveFile.is_open()) {
        std::string progressData((std::istreambuf_iterator<char>(saveFile)), std::istreambuf_iterator<char>());
        saveFile.close();
        return progressData;
    }
    std::cerr << "加载游戏进度失败!\n";
    return "";
}

// 获取用户名
std::string PlayerAccount::getUsername() const {
    return username;
}

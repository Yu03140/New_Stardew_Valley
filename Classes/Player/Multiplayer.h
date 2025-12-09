#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <string>
#include <thread>
#include <iostream>
#include <vector>
#include "cocos2d.h"

class Multiplayer {
public:
    Multiplayer();
    ~Multiplayer();

    bool connectToServer(const std::string& serverIP, int port);  // 连接服务器
    bool startServer(int port);                                  // 启动服务器
    bool sendMessage(const std::string& message);                // 发送消息
    std::string receiveMessage();                                // 接收消息

private:
    int socketFD;                     // 套接字描述符
    std::thread serverThread;         // 用于运行服务器的线程
    std::vector<std::string> messages;  // 消息队列
    bool isRunning;                   // 标记服务器是否运行中

    void serverHandler();             // 服务器处理函数
};

#endif // MULTIPLAYER_H

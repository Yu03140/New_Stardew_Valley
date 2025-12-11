/*
 * Multiplayer.cpp
 *
 * 功能描述：
 * 该模块实现了多人联机功能，提供了以下主要功能：
 * 1. 启动服务器：允许玩家创建一个游戏服务器，并接受其他玩家的连接请求。
 * 2. 连接服务器：玩家可以通过指定服务器的 IP 地址和端口号加入现有的游戏服务器。
 * 3. 消息发送：提供接口让玩家在联机环境中发送文本消息或数据。
 * 4. 消息接收：支持接收来自其他玩家或服务器发送的消息。
 *
 * 主要实现：
 * - 在 Windows 平台下使用 Winsock 网络编程接口，支持跨平台扩展。
 * - 多线程处理服务器运行，确保主线程能够继续响应用户操作。
 * - 使用简单的消息队列，处理玩家之间的消息通信。
 *
 * 注意事项：
 * 1. 该模块当前仅支持简单的文本通信，后续可扩展为游戏内复杂的数据同步功能。
 * 2. 在使用该模块时，确保服务器端口未被占用。
 * 3. 如果需要运行于 Linux 等其他平台，请确保启用 POSIX 网络接口支持。
 *
 * 作者：
 * 日期：2024-12-07
 * 版本：1.0
 *
 */


#include "Multiplayer.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib") // 链接 Winsock 库
#include <cstring>

// 构造函数
Multiplayer::Multiplayer() : socketFD(-1), isRunning(false) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup失败，出现错误: " << result << "\n";
    }
}


// 析构函数
Multiplayer::~Multiplayer() {
    if (isRunning) {
        isRunning = false;
        serverThread.join();
    }
    if (socketFD != -1) {
        closesocket(socketFD);
    }
    WSACleanup();
}

// 连接到服务器
bool Multiplayer::connectToServer(const std::string& serverIP, int port) {
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        std::cerr << "Failed to create socket!\n";
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

    if (connect(socketFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to connect to server!\n";
        return false;
    }
    std::cout << "Connected to server successfully!\n";
    return true;
}

// 启动服务器
bool Multiplayer::startServer(int port) {
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        std::cerr << "Failed to create socket!\n";
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socketFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind socket!\n";
        return false;
    }

    if (listen(socketFD, 5) == -1) {
        std::cerr << "Failed to listen on socket!\n";
        return false;
    }

    isRunning = true;
    serverThread = std::thread(&Multiplayer::serverHandler, this);
    std::cout << "Server started successfully!\n";
    return true;
}

// 服务器处理函数
void Multiplayer::serverHandler() {
    while (isRunning) {
        sockaddr_in clientAddr{};
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(socketFD, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket != -1) {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            recv(clientSocket, buffer, sizeof(buffer), 0);
            messages.push_back(std::string(buffer));
            closesocket(clientSocket);
        }
    }
}

// 发送消息
bool Multiplayer::sendMessage(const std::string& message) {
    if (socketFD == -1) {
        std::cerr << "No active connection to send message!\n";
        return false;
    }

    if (send(socketFD, message.c_str(), message.size(), 0) == -1) {
        std::cerr << "Failed to send message!\n";
        return false;
    }
    return true;
}

// 接收消息
std::string Multiplayer::receiveMessage() {
    if (!messages.empty()) {
        std::string message = messages.back();
        messages.pop_back();
        return message;
    }
    return "";
}

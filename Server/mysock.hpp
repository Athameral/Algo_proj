/*****************************************************************//**
 * \file   mysock.hpp
 * \brief  对Winsock简单封装
 * 
 * \author sunny
 * \date   January 2024
 *********************************************************************/

#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // 链接ws2_32.lib库

class MySocket_tcp
{
public:
    MySocket_tcp() : socket_(INVALID_SOCKET)
    {
        // 初始化Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "Failed to initialize Winsock." << std::endl;
            exit(1);
        }
    }

    ~MySocket_tcp()
    {
        // 清理Winsock
        if (socket_ != INVALID_SOCKET)
        {
            closesocket(socket_);
        }
        WSACleanup();
    }

    bool Connect(const std::string& ipAddress, int port)
    {
        // 创建套接字
        socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_ == INVALID_SOCKET)
        {
            std::cerr << "Failed to create socket." << std::endl;
            return false;
        }

        // 设置服务器地址
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
        serverAddress.sin_port = htons(port);

        // 连接服务器
        if (connect(socket_, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0)
        {
            std::cerr << "Failed to connect to server." << std::endl;
            return false;
        }

        return true;
    }

    bool Send(const std::string& message)
    {
        // 发送数据
        if (send(socket_, message.c_str(), static_cast<int>(message.length()), 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send data." << std::endl;
            return false;
        }

        return true;
    }
    bool Send(const void* buffer, int length)
    {
        // 发送数据
        if (send(socket_, static_cast<const char*>(buffer), length, 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send data." << std::endl;
            return false;
        }

        return true;
    }

    std::string Receive()
    {
        constexpr int bufferSize = 4096;
        char buffer[bufferSize];

        // 接收数据
        int bytesRead = recv(socket_, buffer, bufferSize - 1, 0);
        if (bytesRead <= 0)
        {
            std::cerr << "Failed to receive data." << std::endl;
            return std::string();
        }

        buffer[bytesRead] = '\0';
        return std::string(buffer);
    }

    int Receive(void* buffer, int maxLength)
    {
        // 接收数据
        int bytesRead = recv(socket_, static_cast<char*>(buffer), maxLength, 0);
        if (bytesRead == SOCKET_ERROR)
        {
            std::cerr << "Failed to receive data." << std::endl;
            return -1;
        }

        return bytesRead;
    }

private:
    SOCKET socket_;
};
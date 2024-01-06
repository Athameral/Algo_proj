/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author sunny
 * \date   January 2024
 *********************************************************************/

#include <iostream>
#include "mysock.hpp"

int main()
{
    MySocket_tcp socket;

    // 连接到服务器
    if (!socket.Connect("127.0.0.1", 8123))
    {
        return 1;
    }

    // 发送数据
    if (!socket.Send("Hello, server!"))
    {
        return 1;
    }

    // 接收数据
    std::string receivedData = socket.Receive();
    if (receivedData.empty())
    {
        return 1;
    }

    std::cout << "Received: " << receivedData << std::endl;

    return 0;
}
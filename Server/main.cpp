/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author sunny
 * \date   January 2024
 *********************************************************************/

#include <iostream>
#include <array>
#include <random>
#include "mysock.hpp"

std::array<int, 256000> rawFloatData;

int main()
{
    {
        auto i = 0;
        for (auto& e : rawFloatData)
            e = i++;
    }

    MySocket_tcp socket;

    // 连接到服务器
    if (!socket.Connect("127.0.0.1", 8123))
    {
        return 1;
    }

    // 发送数据
    if (!socket.Send(static_cast<const void*>(rawFloatData.data()), rawFloatData.size() * sizeof(int)))
    {
        std::cout << "finish" << std::endl;
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
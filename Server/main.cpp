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

    // ���ӵ�������
    if (!socket.Connect("127.0.0.1", 8123))
    {
        return 1;
    }

    // ��������
    if (!socket.Send("Hello, server!"))
    {
        return 1;
    }

    // ��������
    std::string receivedData = socket.Receive();
    if (receivedData.empty())
    {
        return 1;
    }

    std::cout << "Received: " << receivedData << std::endl;

    return 0;
}
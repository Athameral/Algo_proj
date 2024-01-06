/*****************************************************************//**
 * \file   main.cpp
 * \brief  服务端
 *
 * \author sunny
 * \date   January 2024
 *********************************************************************/

#include <iostream>
#include <array>
#include <random>
#include "mysock.hpp"

std::array<int, 2560> rawFloatData;

int main()
{
	MySocket_tcp socket;

	// 开始监听
	if (!socket.Listen_Accept("127.0.0.1", 8123))
	{
		return 1;
	}

	// 接收数据
	if (!socket.Receive(static_cast<void*>(rawFloatData.data()), rawFloatData.size() * sizeof(float)))
	{
		return 1;
	}
	std::cout << "finish" << std::endl;

	for (auto i = rawFloatData.cbegin(); i < rawFloatData.cbegin() + 10; ++i)
		std::cout << *i << std::endl;

	for (auto i = rawFloatData.cend() - 10; i < rawFloatData.cend(); ++i)
		std::cout << *i << std::endl;

	return 0;
}
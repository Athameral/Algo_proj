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
#include <omp.h>
#include <string>
#include "main.h"
#include "mysock.hpp"
#include "speed_functions.hpp"
std::array<float, DATANUM / 2> second;

int main()
{
	std::string serverAddress;
	std::cout << "请输入服务器IP:Port" << std::endl;
	std::cin >> serverAddress;
	auto ip = serverAddress.substr(0, serverAddress.find(':'));
	auto port = std::stoi(serverAddress.substr(serverAddress.find(':') + 1));

	MySocket_tcp socket;

	// 绑定
	if (!socket.Listen_Accept(ip,port))
	{
		return 1;
	}

	std::cout << "客户端已建立连接..." << std::endl;

	if (!socket.Receive(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "数据接收失败或未完整接收..." << std::endl;
	}

	// Server开始查找最大值
	std::cout << "Server开始查找最大值..." << std::endl;
	auto server_max = *getMaxIter(second);

	if (!socket.Send(&server_max, sizeof(server_max)))
	{
		std::cerr << "未发送最大值或发送失败..." << std::endl;
	}

	// Server开始求和
	std::cout << "Server开始求和..." << std::endl;
	auto server_sum = getSum(second);

	if (!socket.Send(&server_sum, sizeof(server_sum)))
	{
		std::cerr << "未发送求和结果或发送失败..." << std::endl;
	}

	// Server开始排序
	std::cout << "Server开始排序..." << std::endl;
	quickSort(second, second.begin(), second.end());
	std::cout << "Server排序完毕..." << std::endl;

	if (!socket.Send(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "未发送排序结果或发送失败..." << std::endl;
	}

	std::cout << "任务均已完成..." << std::endl;

	//system("PAUSE");
	return 0;
}
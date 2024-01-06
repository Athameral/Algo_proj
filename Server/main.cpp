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
	std::cout << "Client开始查找最大值..." << std::endl;
	auto start_time = omp_get_wtime();
	auto max_num = *getMaxIter(first);

	auto server_max = 0.0f;
	if (!socket.Receive(&server_max, sizeof(server_max)))
	{
		std::cerr << "未接收最大值或接收失败..." << std::endl;
	}

	if (server_max < max_num)
	{
		server_max = max_num;
		std::cout << "最大值：" << server_max << std::endl;
	}

	// Client开始求和
	std::cout << "Client开始求和..." << std::endl;
	auto sum = getSum(rawFloatData);

	auto server_sum = 0.0f;
	if (!socket.Receive(&server_sum, sizeof(server_sum)))
	{
		std::cerr << "未接收求和结果或接收失败..." << std::endl;
	}

	std::cout << "求和结果：" << sum + server_sum << std::endl;

	// Client开始排序
	std::cout << "Client开始排序..." << std::endl;
	quickSort(first, first.begin(), first.end());

	if (!socket.Receive(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "未接收排序结果或接收失败..." << std::endl;
	}

	{
		auto i = 0;
		auto it1 = first.cbegin(), it2 = second.cbegin();
		while (true)
		{
			if (it1 != first.cend() && it2 != second.cend())
			{
				if (*it1 < *it2)
					sortResult[i] = *(it2++);
				else
					sortResult[i] = *(it1++);
			}
			else if (it1 == first.cend() && it2 != second.cend())
			{
				std::copy(it2, second.cend(), sortResult.begin() + i);
				break;
			}
			else if (it1 != first.cend() && it2 == second.cend())
			{
				std::copy(it1, first.cend(), sortResult.begin() + i);
				break;
			}
			else
				break; // 其实没必要
		}
	}

	std::cout << "测试均已完成..." << std::endl;
	std::cout << "共消耗：" << omp_get_wtime() - start_time << "秒" << std::endl;

	{
		std::cout << "开始测试排序..." << std::endl;
		auto flag = true;
		for (auto i = sortResult.cbegin(); i != sortResult.cend() - 1; ++i)
			if (*i > *(i + 1))
			{
				std::cout << "排序测试未通过..." << std::endl;
				flag = false;
				break;
			}
		if (flag)
			std::cout << "排序测试通过..." << std::endl;
	}

	return 0;

}
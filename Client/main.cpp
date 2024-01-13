/*****************************************************************//**
 * \file   main.cpp
 * \brief  客户端
 * 本来在单机的时候统计三项测试的时间是分开的，但是双机时传输时间不能忽略
 * 不管把传输时间算给单独哪一项，都不是那么合适，所以干脆三项一起统计
 * 创建多线程是有时间消耗的，可以忽略，但本例选择不忽略
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

std::array<float, DATANUM> rawFloatData, sortResult;
std::array<float, DATANUM / 2> first, second;

int main()
{
	// 生成随机数用于测试
	std::minstd_rand engine(1234); // 固定随机数种子，确保结果一致性
	std::uniform_real_distribution<float> dist(0.0F, 100.0F); // 在[0,100]内均匀分布的随机数
	for (auto& e : rawFloatData)
		e = dist(engine);

	// 数据拷贝，用于分开计算
	std::copy(rawFloatData.cbegin(), rawFloatData.cbegin() + rawFloatData.size() / 2, first.begin());
	std::copy(rawFloatData.cbegin() + rawFloatData.size() / 2, rawFloatData.cend(), second.begin());

	std::string serverAddress;
	std::cout << "请输入服务器IP:Port" << std::endl;
	std::cin >> serverAddress;
	auto ip = serverAddress.substr(0, serverAddress.find(':'));
	auto port = std::stoi(serverAddress.substr(serverAddress.find(':') + 1));

	MySocket_tcp socket;

	// 连接到服务器
	if (!socket.Connect(ip, port))
	{
		return 1;
	}

	std::cout << "已连接至服务器..." << std::endl;


	// 开始测试
	// 其实用2线程是可以的，但是num_threads有传递性
	// 本来是想发送数据同时计算的，但是OpenMP不是很容易这样写
	auto start = omp_get_wtime();
	// 其实发送数据要考虑长度是否是奇数，不过一般都是偶数，就不改了
	if (!socket.Send(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "数据发送失败或未完整发送..." << std::endl;
	}

	// Client开始查找最大值
	std::cout << "Client开始查找最大值..." << std::endl;
	auto max_num = *getMaxIter(first);

	auto server_max = 0.0f;
	if (!socket.Receive(&server_max, sizeof(server_max)))
	{
		std::cerr << "未接收最大值或接收失败..." << std::endl;
	}

	if (server_max < max_num)
		server_max = max_num;
	std::cout << "最大值：" << server_max << std::endl;

	// Client开始求和
	std::cout << "Client开始求和..." << std::endl;
	auto sum = getSum(first);

	auto server_sum = 0.0f;
	if (!socket.Receive(&server_sum, sizeof(server_sum)))
	{
		std::cerr << "未接收求和结果或接收失败..." << std::endl;
	}

	std::cout << "求和结果：" << sum + server_sum << std::endl;

	// Client开始排序
	std::cout << "Client开始排序..." << std::endl;
	//quickSort(first, first.begin(), first.end());
	std::sort(first.begin(), first.end());
	std::cout << "Client排序完毕..." << std::endl;

	omp_set_num_threads(1);

	if (!socket.Receive(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "未接收排序结果或接收失败..." << std::endl;
	}

	{
		auto i = 0;
		auto it1 = first.cbegin(), it2 = second.cbegin();
		while (it1 != first.cend() && it2 != second.cend())
		{
			if (*it1 < *it2)
				sortResult[i++] = *(it1++);
			else
				sortResult[i++] = *(it2++);
		}
		while (it1 != first.cend())
			sortResult[i++] = *(it1++);
		while (it2 != second.cend())
			sortResult[i++] = *(it2++);
	}

	std::cout << "测试均已完成..." << std::endl;
	std::cout << "共消耗：" << omp_get_wtime() - start << "秒" << std::endl;

	{
		std::cout << "开始测试排序..." << std::endl;
		auto flag = true;
		for (auto i = 0; i < MAX_THREADS * SUBDATANUM - 2; ++i)
			if (sortResult[i] > sortResult[i + 1])
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
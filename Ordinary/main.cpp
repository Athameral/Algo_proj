/*****************************************************************//**
 * \file   main.cpp
 * \brief  用于测试的程序逻辑主框架
 *
 * \author 2151522
 * \date   January 2024
 *********************************************************************/

#include <iostream>
#include <array>
#include <random>
#include <Windows.h>
#include <omp.h>

#include "plain_functions.hpp"

constexpr auto MAX_THREADS = 16;
constexpr auto SUBDATANUM = 2000000;
constexpr auto DATANUM = MAX_THREADS * SUBDATANUM;
//constexpr auto DATANUM = 24;

std::array<float, DATANUM> rawFloatData, sortResult;

int main()
{
	// 生成随机数用于测试
	std::minstd_rand engine(1234); // 固定随机数种子，确保结果一致性
	std::uniform_real_distribution<float> dist(0.0F, 100.0F); // 在[0,100]内均匀分布的随机数
	for (auto& e : rawFloatData)
		e = dist(engine);

	// 最大值查找测试
	{
		std::cout << "开始查找最大值..." << std::endl;
		auto start_time = omp_get_wtime();
		auto max_iter = getMaxIter(rawFloatData);
		std::cout << "最大值：" << *max_iter << std::endl;
		std::cout << "查找最大值消耗：" << omp_get_wtime() - start_time << "秒" << std::endl;
	}

	// 求和测试
	{
		std::cout << "开始求和..." << std::endl;
		auto start_time = omp_get_wtime();
		auto sum = getSum(rawFloatData);
		std::cout << "求和结果：" << sum << std::endl;
		std::cout << "求和消耗：" << omp_get_wtime() - start_time << "秒" << std::endl;
	}

	// 排序测试
	{
		auto start_time = omp_get_wtime();
		sortResult = rawFloatData;
		quickSort(sortResult, sortResult.begin(), sortResult.end());
		std::cout << "排序消耗：" << omp_get_wtime() - start_time << "秒" << std::endl;

		// 检查排序是否有错
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

	}

	return 0;
}

/*****************************************************************//**
 * \file   main.cpp
 * \brief  ���ڲ��Եĳ����߼������
 *
 * \author 2151522
 * \date   January 2024
 *********************************************************************/

#include <iostream>
#include <array>
#include <random>
#include <Windows.h>
#include <omp.h>

#include "main.h"
#include "speed_functions.hpp"

std::array<float, DATANUM> rawFloatData, sortResult;

int main()
{
	// ������������ڲ���
	std::minstd_rand engine(1234); // �̶���������ӣ�ȷ�����һ����
	std::uniform_real_distribution<float> dist(0.0F, 100.0F); // ��[0,100]�ھ��ȷֲ��������
	for (auto& e : rawFloatData)
		e = dist(engine);

	// ���ֵ���Ҳ���
	// ˵������Ϊ�ڲ������ֵʱ�ſ�ʼ�����̣߳���˻�����������һЩ
	// �����������ǰʹ��parallel�������̣߳��������ƽ̨ʵ����أ�����Ϊû��Ҫ������
	{
		std::cout << "��ʼ�������ֵ..." << std::endl;
		auto start_time = omp_get_wtime();
		auto max_iter = getMaxIter(rawFloatData);
		std::cout << "���ֵ��" << *max_iter << std::endl;
		std::cout << "�������ֵ���ģ�" << omp_get_wtime() - start_time << "��" << std::endl;
	}

	// ��Ͳ���
	{
		std::cout << "��ʼ���..." << std::endl;
		auto start_time = omp_get_wtime();
		auto sum = getSum(rawFloatData);
		std::cout << "��ͽ����" << sum << std::endl;
		std::cout << "������ģ�" << omp_get_wtime() - start_time << "��" << std::endl;
	}

	// �������
	{
		auto start_time = omp_get_wtime();
		sortResult = rawFloatData;
		quickSort(sortResult, sortResult.begin(), sortResult.end());
		std::cout << "�������ģ�" << omp_get_wtime() - start_time << "��" << std::endl;

		// ��������Ƿ��д�
		{
			std::cout << "��ʼ��������..." << std::endl;
			auto flag = true;
			for (auto i = sortResult.cbegin(); i != sortResult.cend() - 1; ++i)
				if (*i > *(i + 1))
				{
					std::cout << "�������δͨ��..." << std::endl;
					flag = false;
					break;
				}
			if (flag)
				std::cout << "�������ͨ��..." << std::endl;
		}

	}

	return 0;
}

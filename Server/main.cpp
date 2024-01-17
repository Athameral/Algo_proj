/*****************************************************************//**
 * \file   main.cpp
 * \brief  �����
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
	std::cout << "�����������IP:Port" << std::endl;
	std::cin >> serverAddress;
	auto ip = serverAddress.substr(0, serverAddress.find(':'));
	auto port = std::stoi(serverAddress.substr(serverAddress.find(':') + 1));

	MySocket_tcp socket;

	// ��
	if (!socket.Listen_Accept(ip,port))
	{
		return 1;
	}

	std::cout << "�ͻ����ѽ�������..." << std::endl;

	if (!socket.Receive(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "���ݽ���ʧ�ܻ�δ��������..." << std::endl;
	}

	// Server��ʼ�������ֵ
	std::cout << "Client��ʼ�������ֵ..." << std::endl;
	auto start_time = omp_get_wtime();
	auto max_num = *getMaxIter(first);

	auto server_max = 0.0f;
	if (!socket.Receive(&server_max, sizeof(server_max)))
	{
		std::cerr << "δ�������ֵ�����ʧ��..." << std::endl;
	}

	if (server_max < max_num)
	{
		server_max = max_num;
		std::cout << "���ֵ��" << server_max << std::endl;
	}

	// Client��ʼ���
	std::cout << "Client��ʼ���..." << std::endl;
	auto sum = getSum(rawFloatData);

	auto server_sum = 0.0f;
	if (!socket.Receive(&server_sum, sizeof(server_sum)))
	{
		std::cerr << "δ������ͽ�������ʧ��..." << std::endl;
	}

	std::cout << "��ͽ����" << sum + server_sum << std::endl;

	// Client��ʼ����
	std::cout << "Client��ʼ����..." << std::endl;
	quickSort(first, first.begin(), first.end());

	if (!socket.Receive(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "δ���������������ʧ��..." << std::endl;
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
				break; // ��ʵû��Ҫ
		}
	}

	std::cout << "���Ծ������..." << std::endl;
	std::cout << "�����ģ�" << omp_get_wtime() - start_time << "��" << std::endl;

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

	return 0;

}
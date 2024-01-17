/*****************************************************************//**
 * \file   main.cpp
 * \brief  �ͻ���
 * �����ڵ�����ʱ��ͳ��������Ե�ʱ���Ƿֿ��ģ�����˫��ʱ����ʱ�䲻�ܺ���
 * ���ܰѴ���ʱ�����������һ���������ô���ʣ����Ըɴ�����һ��ͳ��
 * �������߳�����ʱ�����ĵģ����Ժ��ԣ�������ѡ�񲻺���
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
	// ������������ڲ���
	std::minstd_rand engine(1234); // �̶���������ӣ�ȷ�����һ����
	std::uniform_real_distribution<float> dist(0.0F, 100.0F); // ��[0,100]�ھ��ȷֲ��������
	for (auto& e : rawFloatData)
		e = dist(engine);

	// ���ݿ��������ڷֿ�����
	std::copy(rawFloatData.cbegin(), rawFloatData.cbegin() + rawFloatData.size() / 2, first.begin());
	std::copy(rawFloatData.cbegin() + rawFloatData.size() / 2, rawFloatData.cend(), second.begin());

	std::string serverAddress;
	std::cout << "�����������IP:Port" << std::endl;
	std::cin >> serverAddress;
	auto ip = serverAddress.substr(0, serverAddress.find(':'));
	auto port = std::stoi(serverAddress.substr(serverAddress.find(':') + 1));

	MySocket_tcp socket;

	// ���ӵ�������
	if (!socket.Connect(ip, port))
	{
		return 1;
	}

	std::cout << "��������������..." << std::endl;


	// ��ʼ����
	// ��ʵ��2�߳��ǿ��Եģ�����num_threads�д�����
	// �������뷢������ͬʱ����ģ�����OpenMP���Ǻ���������д
	auto start = omp_get_wtime();
	// ��ʵ��������Ҫ���ǳ����Ƿ�������������һ�㶼��ż�����Ͳ�����
	if (!socket.Send(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "���ݷ���ʧ�ܻ�δ��������..." << std::endl;
	}

	// Client��ʼ�������ֵ
	std::cout << "Client��ʼ�������ֵ..." << std::endl;
	auto max_num = *getMaxIter(first);

	auto server_max = 0.0f;
	if (!socket.Receive(&server_max, sizeof(server_max)))
	{
		std::cerr << "δ�������ֵ�����ʧ��..." << std::endl;
	}

	if (server_max < max_num)
		server_max = max_num;
	std::cout << "���ֵ��" << server_max << std::endl;

	// Client��ʼ���
	std::cout << "Client��ʼ���..." << std::endl;
	auto sum = getSum(first);

	auto server_sum = 0.0f;
	if (!socket.Receive(&server_sum, sizeof(server_sum)))
	{
		std::cerr << "δ������ͽ�������ʧ��..." << std::endl;
	}

	std::cout << "��ͽ����" << sum + server_sum << std::endl;

	// Client��ʼ����
	std::cout << "Client��ʼ����..." << std::endl;
	//quickSort(first, first.begin(), first.end());
	std::sort(first.begin(), first.end());
	std::cout << "Client�������..." << std::endl;

	omp_set_num_threads(1);

	if (!socket.Receive(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "δ���������������ʧ��..." << std::endl;
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

	std::cout << "���Ծ������..." << std::endl;
	std::cout << "�����ģ�" << omp_get_wtime() - start << "��" << std::endl;

	{
		std::cout << "��ʼ��������..." << std::endl;
		auto flag = true;
		for (auto i = 0; i < MAX_THREADS * SUBDATANUM - 2; ++i)
			if (sortResult[i] > sortResult[i + 1])
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
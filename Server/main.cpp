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
	std::cout << "Server��ʼ�������ֵ..." << std::endl;
	auto server_max = *getMaxIter(second);

	if (!socket.Send(&server_max, sizeof(server_max)))
	{
		std::cerr << "δ�������ֵ����ʧ��..." << std::endl;
	}

	// Server��ʼ���
	std::cout << "Server��ʼ���..." << std::endl;
	auto server_sum = getSum(second);

	if (!socket.Send(&server_sum, sizeof(server_sum)))
	{
		std::cerr << "δ������ͽ������ʧ��..." << std::endl;
	}

	// Server��ʼ����
	std::cout << "Server��ʼ����..." << std::endl;
	quickSort(second, second.begin(), second.end());
	std::cout << "Server�������..." << std::endl;

	if (!socket.Send(static_cast<void*>(second.data()), second.size() * sizeof(float)))
	{
		std::cerr << "δ��������������ʧ��..." << std::endl;
	}

	std::cout << "����������..." << std::endl;

	//system("PAUSE");
	return 0;
}
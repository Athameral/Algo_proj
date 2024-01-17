/*****************************************************************//**
 * \file   mysock.hpp
 * \brief  ��Winsock�򵥷�װ
 *
 * \author sunny
 * \date   January 2024
 *********************************************************************/

#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // ����ws2_32.lib��

class MySocket_tcp
{
public:
	MySocket_tcp() : socket_(INVALID_SOCKET)
	{
		// ��ʼ��Winsock
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			std::cerr << "Failed to initialize Winsock." << std::endl;
			exit(1);
		}
	}

	~MySocket_tcp()
	{
		// ����Winsock
		if (socket_ != INVALID_SOCKET)
		{
			closesocket(socket_);
		}
		WSACleanup();
	}

	bool Connect(const std::string& ipAddress, int port)
	{
		// �����׽���
		if (socket_ == INVALID_SOCKET)
			socket_ = socket(AF_INET, SOCK_STREAM, 0);
		else
		{
			std::cerr << "Cannot create a socket that already exists." << std::endl;
			return false;
		}
		if (socket_ == INVALID_SOCKET)
		{
			std::cerr << "Failed to create socket." << std::endl;
			return false;
		}

		// ���÷�������ַ
		sockaddr_in serverAddress{};
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
		serverAddress.sin_port = htons(port);

		// ���ӷ�����
		if (connect(socket_, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0)
		{
			std::cerr << "Failed to connect to server." << std::endl;
			return false;
		}

		return true;
	}

	bool Listen_Accept(const std::string& ipAddress, int port)
	{
		// ֻ�����һ������
		// �����׽���
		if (socket_ == INVALID_SOCKET)
			socket_ = socket(AF_INET, SOCK_STREAM, 0);
		else
		{
			std::cerr << "Cannot create a socket that already exists." << std::endl;
			return false;
		}
		if (socket_ == INVALID_SOCKET)
		{
			std::cerr << "Failed to create socket." << std::endl;
			return false;
		}

		// ���÷�������ַ
		sockaddr_in serverAddress{};
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
		serverAddress.sin_port = htons(port);

		// �󶨲���ʼ����
		if (bind(socket_, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0)
		{
			std::cerr << "Failed to bind:" << ipAddress << ":" << port << std::endl;
			return false;
		}

		if (listen(socket_, 1) != 0)
		{
			std::cerr << "Failed to listen on:" << ipAddress << ":" << port << std::endl;
			return false;
		}
		// ����serverAddress��ʵ�����Ѿ���clientAddress��
		auto& clientAddress = serverAddress;
		int clientAddressLength = sizeof(clientAddress);
		socket_ = accept(socket_, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
		if (socket_ == INVALID_SOCKET)
		{
			std::cerr << "Failed to accept:" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;
			return false;
		}

		return true;
	}

	bool Send(const std::string& message)
	{
		// ��������
		if (send(socket_, message.c_str(), static_cast<int>(message.length()), 0) == SOCKET_ERROR)
		{
			std::cerr << "Failed to send data." << std::endl;
			return false;
		}

		return true;
	}
	bool Send(const void* buffer, int length)
	{
		// ��������
		if (send(socket_, static_cast<const char*>(buffer), length, 0) == SOCKET_ERROR)
		{
			std::cerr << "Failed to send data." << std::endl;
			return false;
		}

		return true;
	}

	std::string Receive()
	{
		constexpr int bufferSize = 4096;
		char buffer[bufferSize];

		// ��������
		int bytesRead = recv(socket_, buffer, bufferSize - 1, 0);
		if (bytesRead <= 0)
		{
			std::cerr << "Failed to receive data." << std::endl;
			return std::string();
		}

		buffer[bytesRead] = '\0';
		return std::string(buffer);
	}

	int Receive(void* buffer, int maxLength)
	{
		// ��������
		int bytesRead = recv(socket_, static_cast<char*>(buffer), maxLength, 0);
		if (bytesRead == SOCKET_ERROR)
		{
			std::cerr << "Failed to receive data." << std::endl;
			return -1;
		}

		return bytesRead;
	}

private:
	SOCKET socket_;
};
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
#include "main.h"

#pragma comment(lib, "ws2_32.lib") // ����ws2_32.lib��

// ÿ����������ֽ���
const int maxChunkSize = 8192; // ��ʵ��󣬽��鲻ҪС��2048��Ҳ��Ҫ����10240�����Ǻ��Ͻ��Ľ��

class MySocket_tcp
{
public:
	MySocket_tcp() : socket_(INVALID_SOCKET)
	{
		// ��ʼ��Winsock
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
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
		auto socket_old = socket_;
		socket_ = accept(socket_, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
		closesocket(socket_old);


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
		const char* data = static_cast<const char*>(buffer);  // ��������ת��Ϊcharָ��

		int remainingLength = length;  // ʣ�����ݵĳ���

		while (remainingLength > 0)
		{
			int chunkSize = min(remainingLength, maxChunkSize);  // ��ȡ��ǰ���ݿ�Ĵ�С

			// �������ݿ�
			auto sent_chunk = send(socket_, data, chunkSize, 0);
			if (sent_chunk == SOCKET_ERROR || sent_chunk == 0)
			{
				std::cerr << "Failed to send data." << std::endl;
				return false;
			}

			data += sent_chunk;  // ��������ָ��
			remainingLength -= sent_chunk;  // ����ʣ�����ݳ���
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

		char* data = static_cast<char*>(buffer);  // ��������ת��Ϊcharָ��

		int totalBytesRead = 0;  // �ܹ��ѽ��յ��ֽ���

		while (maxLength > 0)
		{
			int chunkSize = min(maxLength, maxChunkSize);  // ��ȡ��ǰ���ݿ�Ĵ�С

			// �������ݿ�
			int bytesRead = recv(socket_, data, chunkSize, 0);
			if (bytesRead == SOCKET_ERROR)
			{
				std::cerr << "Failed to receive data." << std::endl;
				return 0;
			}

			if (bytesRead == 0)
			{
				// ���ӹر�
				break;
			}

			data += bytesRead;  // ��������ָ��
			maxLength -= bytesRead;  // ����ʣ�����ݳ���
			totalBytesRead += bytesRead;  // �����ܹ��ѽ��յ��ֽ���

			if (bytesRead < chunkSize)
			{
				// �ѽ��յ��������ݿ�
				break;
			}
		}

		return totalBytesRead;
	}

private:
	SOCKET socket_;
};
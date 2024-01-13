/*****************************************************************//**
 * \file   mysock.hpp
 * \brief  对Winsock简单封装
 *
 * \author sunny
 * \date   January 2024
 *********************************************************************/

#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include "main.h"

#pragma comment(lib, "ws2_32.lib") // 链接ws2_32.lib库

// 每个区块最大字节数
const int maxChunkSize = 8192; // 简单实验后，建议不要小于2048，也不要大于10240，不是很严谨的结果

class MySocket_tcp
{
public:
	MySocket_tcp() : socket_(INVALID_SOCKET)
	{
		// 初始化Winsock
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
		{
			std::cerr << "Failed to initialize Winsock." << std::endl;
			exit(1);
		}
	}

	~MySocket_tcp()
	{
		// 清理Winsock
		if (socket_ != INVALID_SOCKET)
		{
			closesocket(socket_);
		}
		WSACleanup();
	}

	bool Connect(const std::string& ipAddress, int port)
	{
		// 创建套接字
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

		// 设置服务器地址
		sockaddr_in serverAddress{};
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
		serverAddress.sin_port = htons(port);

		// 连接服务器
		if (connect(socket_, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0)
		{
			std::cerr << "Failed to connect to server." << std::endl;
			return false;
		}

		return true;
	}

	bool Listen_Accept(const std::string& ipAddress, int port)
	{
		// 只会接受一个连接
		// 创建套接字
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

		// 设置服务器地址
		sockaddr_in serverAddress{};
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
		serverAddress.sin_port = htons(port);

		// 绑定并开始监听
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
		// 重用serverAddress，实际上已经是clientAddress了
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
		// 发送数据
		if (send(socket_, message.c_str(), static_cast<int>(message.length()), 0) == SOCKET_ERROR)
		{
			std::cerr << "Failed to send data." << std::endl;
			return false;
		}

		return true;
	}
	bool Send(const void* buffer, int length)
	{
		const char* data = static_cast<const char*>(buffer);  // 将缓冲区转换为char指针

		int remainingLength = length;  // 剩余数据的长度

		while (remainingLength > 0)
		{
			int chunkSize = min(remainingLength, maxChunkSize);  // 获取当前数据块的大小

			// 发送数据块
			auto sent_chunk = send(socket_, data, chunkSize, 0);
			if (sent_chunk == SOCKET_ERROR || sent_chunk == 0)
			{
				std::cerr << "Failed to send data." << std::endl;
				return false;
			}

			data += sent_chunk;  // 更新数据指针
			remainingLength -= sent_chunk;  // 更新剩余数据长度
		}

		return true;
	}

	std::string Receive()
	{
		constexpr int bufferSize = 4096;
		char buffer[bufferSize];

		// 接收数据
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

		char* data = static_cast<char*>(buffer);  // 将缓冲区转换为char指针

		int totalBytesRead = 0;  // 总共已接收的字节数

		while (maxLength > 0)
		{
			int chunkSize = min(maxLength, maxChunkSize);  // 获取当前数据块的大小

			// 接收数据块
			int bytesRead = recv(socket_, data, chunkSize, 0);
			if (bytesRead == SOCKET_ERROR)
			{
				std::cerr << "Failed to receive data." << std::endl;
				return 0;
			}

			if (bytesRead == 0)
			{
				// 连接关闭
				break;
			}

			data += bytesRead;  // 更新数据指针
			maxLength -= bytesRead;  // 更新剩余数据长度
			totalBytesRead += bytesRead;  // 更新总共已接收的字节数

			if (bytesRead < chunkSize)
			{
				// 已接收到最后的数据块
				break;
			}
		}

		return totalBytesRead;
	}

private:
	SOCKET socket_;
};
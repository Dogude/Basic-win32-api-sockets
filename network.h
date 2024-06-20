#pragma once
#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include "framework.h"

#pragma comment(lib,"ws2_32")

template<typename ... T>
void print(const char* spec, T... data)
{

	char arr[200];
	sprintf(arr, spec, data...);
	MessageBoxA(0, arr, "Hello", 0);

}

struct Socket
{

	Socket() : iResult(0), hints({}), result(0), ptr(0)
	{


		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			print("%s", "Error at WSAStartup");
			exit(1);
		}

	}


protected:
	int iResult;
	WSADATA wsaData;
	ADDRINFOA hints;
	PADDRINFOA  result, ptr;


};


struct Client : Socket
{

	Client() : ConnectSocket(INVALID_SOCKET)
	{

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

	}


	void start(const char* addr, const char* port)
	{

		iResult = getaddrinfo(addr, port, &hints, &result);
		if (iResult != 0)
		{

			print("%s", "error at getaddrinfo");
			WSACleanup();
			exit(1);

		}

		ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			print("%s", "error at socket()");
			freeaddrinfo(result);
			WSACleanup();
			exit(1);

		}

		iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			print("%s", "error at connect()");
			closesocket(ConnectSocket);
			freeaddrinfo(result);
			WSACleanup();
			exit(1);
		}

		char buf[200] = {};

		iResult = recv(ConnectSocket, buf, 180, 0);
		if (iResult <= 0)
		{
			print("%s", "error at recv()");
			closesocket(ConnectSocket);
			freeaddrinfo(result);
			WSACleanup();
			exit(1);
		}

		/*
		sockaddr_in* ipa = reinterpret_cast<struct sockaddr_in*>(result->ai_addr);
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(ipa->sin_addr),ip, INET_ADDRSTRLEN);
		print("%s - %d",ip,ntohs(ipa->sin_port));
		*/

		print("%s", buf);

		closesocket(ConnectSocket);
		freeaddrinfo(result);
		WSACleanup();

	}


private:
	SOCKET ConnectSocket;

};

struct Server : Socket
{
	

	Server() : iSendResult(0), ListenSocket(INVALID_SOCKET), ClientSocket(INVALID_SOCKET)
	{

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;
			
	}
		
	void start()
	{	
			
		iResult = getaddrinfo(0, "27015", &hints, &result);
		if (iResult != 0)
		{
			print("getaddrinfo failed: %d", iResult);
			WSACleanup();
			exit(1);
		}

		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET)
		{
			print("Error at socket %d", 1);
			freeaddrinfo(result);
			WSACleanup();
			exit(1);
		}

		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			print("bind failed with error: %d", 0);
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			exit(1);
		}

		freeaddrinfo(result);

		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == INVALID_SOCKET)
		{
			print("%s", "listen failed");
			closesocket(ListenSocket);
			WSACleanup();
			exit(1);

		}

		char buf[] = "test ve elma";

		for (int i = 0; i != 3; i++)
		{
	
			ClientSocket = accept(ListenSocket, 0, 0);
			if (ClientSocket == INVALID_SOCKET)
			{
				print("Error at accept %d", 1);
				closesocket(ListenSocket);
				WSACleanup();
				exit(1);
			}
			

			iResult = send(ClientSocket, buf, strlen(buf), 0);
			if (iResult <= 0)
			{
				print("%s", "error at send()");
				closesocket(ClientSocket);
				WSACleanup();
				exit(1);
			}
			

		}

		print("%s", "succes!");
		closesocket(ListenSocket);
		closesocket(ClientSocket);
		WSACleanup();


	}

private:
	int iSendResult;
	SOCKET ListenSocket;
	SOCKET ClientSocket;

};
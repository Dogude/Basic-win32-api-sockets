#ifdef REQUEST

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>		
#include <string.h>

void request(char* address,char* port,char* buf,char* req)
{
	
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		// clean_up
		exit(1);
	}
	
	ADDRINFOA hints = {};
	PADDRINFOA  result = 0;	
	int re = 0;	
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
		
	
				
	re = getaddrinfo(address, port, &hints, &result);
	if(re != 0){  WSACleanup(); exit(1);  }
	
		
	SOCKET sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);	
	if(sockfd == INVALID_SOCKET){ freeaddrinfo(result); WSACleanup();  exit(1);   }
	
		
	re = connect(sockfd, result->ai_addr, (int)result->ai_addrlen);
	if (re == SOCKET_ERROR)
	{
		
		closesocket(sockfd);
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	re = send(sockfd,req, strlen(req) , 0);	
	if(re <= 0 )
	{
		closesocket(sockfd);
		freeaddrinfo(result);
		WSACleanup();
		exit(1);

	}
	
	re = recv(sockfd, buf, 512 , 0);
	if (re  <= 0)
	{
			
		closesocket(sockfd);
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}
	
	buf[re] = '\0';
		
	
	closesocket(sockfd);
        freeaddrinfo(result);
        WSACleanup();
		
		
	
}




#endif
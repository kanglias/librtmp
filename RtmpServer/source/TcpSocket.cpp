#include <stdio.h>
#include "TcpSocket.h"
#include "string.h"


CTcpSocket::CTcpSocket()
:CInternetSocket()
{

}

CTcpSocket::CTcpSocket(socket_t sockfd)
: CInternetSocket(sockfd)
{

}


int CTcpSocket::CreateSocket(char* ip, unsigned short port, int backlog)
{
	if( (m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("CreateSocket | create socket error!\n");
		return -1;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if(bind(m_socket, (sockaddr*) &addr, sizeof(addr)) < 0)
	{
		printf("CreateSocket | bind socket error! (port=%d)\n", port);
		return -1;
	}

	if(listen(m_socket, backlog) < 0)
	{
		printf("CreateSocket | listen error!\n");
		return -1;
	}

	return m_socket;
}

int CTcpSocket::CreateRTMPSocket(char* ip, unsigned short port, int backlog)
{
	if( (m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("CreateSocket | create socket error!\n");
		return -1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if(bind(m_socket, (struct sockaddr *)(&addr), sizeof(addr)) < 0)
	{
		printf("CreateSocket | bind socket error! (port=%d)\n", port);
		return -1;
	}

	if(listen(m_socket, backlog) < 0)
	{
		printf("CreateSocket | listen error!\n");
		return -1;
	}

	return m_socket;
}

socket_t CTcpSocket::Accept() const
{
	int sockfd = accept( m_socket, 0, 0);
	if( sockfd < 0)
	{
		return INVALID_SOCKET;
	}
	return sockfd;
}


socket_t CTcpSocket::Accept(int timeout) const
{
	struct timeval stTimeOut		= {0};
	stTimeOut.tv_sec				= timeout/1000;
	stTimeOut.tv_usec				= 0;

	fd_set	readFd	= {0}; 
	FD_ZERO(&readFd);
	FD_SET(m_socket, &readFd);

	int nRet = select(m_socket+1, &readFd, NULL, NULL, &stTimeOut);
	if (nRet <= 0)
	{
		return INVALID_SOCKET;
	}

	if (FD_ISSET(m_socket, &readFd))
	{
		int sockfd = accept(m_socket, 0, 0);
		if( sockfd < 0)
		{
			return INVALID_SOCKET;
		}

		return sockfd;
	}

	return INVALID_SOCKET;
}


socket_t CTcpSocket::GetSocket()
{
	return m_socket;
}


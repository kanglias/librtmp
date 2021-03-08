#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "nplog.h"

#include "InternetSocket.h"
#include <windows.h>
#include<stdlib.h>
#include <string.h>

CInternetSocket::CInternetSocket()
:m_socket(INVALID_SOCKET)
,m_peerDisconnected(false)
{
}

CInternetSocket::CInternetSocket(socket_t sockfd)
: m_socket(sockfd)
, m_peerDisconnected(false)
{
	if(sockfd < 0)
		printf("Warn | sockfd is invalid!\n");
}

CInternetSocket::~CInternetSocket()
{
	if (m_socket != INVALID_SOCKET)
	{
		printf("InternetSocket | release socket!\n");
#ifdef WIN32
		closesocket(m_socket);
#else
		close(m_socket);
#endif
		m_socket = INVALID_SOCKET;
	}
}


int CInternetSocket::Send( const void* buffer, size_t len)
{
	size_t sented = 0;
	while(sented < len)
	{
		const char* buf = static_cast<const char*>(buffer) + sented;
		int ret = send(m_socket, buf, (int)(len - sented), 0);
		if( ret < 0) 
		{		
#ifndef WIN32
			int wsError = errno;
			/*NPLogInfo(("Send: strerror(%d): %s\n", wsError, strerror(wsError)));*/
			if (wsError != EINTR && wsError != EWOULDBLOCK && wsError != EAGAIN)
			{
				m_peerDisconnected = true;
				break;
			}
			else
			{
				usleep(200);
				continue;
			}
#else
			int wsError = WSAGetLastError();
			/*NPLogInfo(("Send: strerror(%d): %s\n", wsError, strerror(wsError)));*/
			if (wsError != WSAEINTR && wsError != WSAEWOULDBLOCK)
			{
				m_peerDisconnected = true;
				break;
			}
			else
			{
				Sleep(1);
				continue;
			}
#endif
		}
		sented += ret;
	}
	return (int)sented;
}

int CInternetSocket::Receive( void* buffer, size_t len)
{
	int ret = recv(m_socket, (char*)buffer, (int)len, 0);

	if( ret < 0)
	{
#ifndef WIN32
		int wsError = errno;
		NPLogInfo(("Recv: strerror(%d): %s\n", wsError, strerror(wsError)));
		if (wsError != EINTR && wsError != EWOULDBLOCK && wsError != EAGAIN)
		{
			m_peerDisconnected = true;
		}
#else
		int wsError = WSAGetLastError();
		NPLogInfo(("Recv: strerror(%d): %s\n", wsError, strerror(wsError)));
		if (wsError != WSAEINTR && wsError != WSAEWOULDBLOCK)
		{
			m_peerDisconnected = true;
		}
#endif
	}
	else if (0 == ret)
	{
		m_peerDisconnected = true;
	}

	return ret;
}

int CInternetSocket::Receive( void* buffer, size_t len, int timeout)
{
	struct timeval stTimeOut		= {0};
	stTimeOut.tv_sec				= timeout/1000;
	stTimeOut.tv_usec				= timeout%1000;

	fd_set	readFd; 
	FD_ZERO(&readFd);
	FD_SET(m_socket, &readFd);

	int nRet = select(m_socket+1, &readFd, NULL, NULL, &stTimeOut);
	if (nRet < 0)
	{
		printf("ReceiveTimeout: strerror(%d): %s\n", errno, strerror(errno));
		m_peerDisconnected = true;
		return -1;
	}
	else if (0 == nRet)
	{
		return 0;
	}

	if (FD_ISSET(m_socket, &readFd))
	{	
		nRet = Receive(buffer, len);
		return nRet;
	}

	return 0;
}


void CInternetSocket::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		printf("close socket!\n");
#ifdef WIN32
		closesocket(m_socket);
#else
		close(m_socket);
#endif
		m_socket = INVALID_SOCKET;
	}
}

bool CInternetSocket::PeerDisconnected() const
{
	return m_peerDisconnected;
}


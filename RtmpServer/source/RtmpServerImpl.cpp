#include "RtmpServerImpl.h"
#include "rtmp-server.h"
#include "aio-socket.h"
#include "npthread.h"
#include "ipcvt.h"
#include "nplog.h"


CRtmpServerImpl::CRtmpServerImpl(IPP ipp,CRtmpServer* server)
: m_serverIPP(ipp)
, m_serverIp(DwordIPToStrA(ipp.ip_))
, m_serverPort(ipp.port_)
, m_server(server)
{
	m_rtmp = NULL;

	m_exit = false;

	m_checkThread = INVALID_OSTHREAD;
}

CRtmpServerImpl::~CRtmpServerImpl()
{
	StopService();
}

RmipError CRtmpServerImpl::Run()
{
	int ret = StartService();
	return RmipError(ret);
}

int CRtmpServerImpl::StartService()
{
	socket_init();

	if (server.CreateRTMPSocket(NULL, (unsigned short)m_serverPort, 64) < 0)
	{
		NPLogError(("%s | create tcp sockfd error!\n", __FUNCTION__));
		return -1;
	}

	return 0;
}

void CRtmpServerImpl::StopService()
{
	m_exit = true;
	socket_cleanup();
}

int CRtmpServerImpl::ConntionThread(CRtmpSession** rtmpSession)
{
	if(!m_exit)
	{
		socket_t sockfd = server.Accept(5000);
		if(INVALID_SOCKET == sockfd) 
		{
			return -1;
		}

		int timeout;
		timeout = 1000;
		setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(int));
		if (*rtmpSession)
		{
			(*rtmpSession)->CreateSocket(sockfd);
		}
		return 0;
	}
	return -1;
}
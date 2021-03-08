#include "RtmpServerImpl.h"
#include "rtmp-server.h"
#include "aio-socket.h"
#include "npthread.h"
#include "ipcvt.h"


CRtmpServerImpl::CRtmpServerImpl(IPP ipp,CRtmpServer* server)
: m_serverIPP(ipp)
, m_serverIp(DwordIPToStrA(ipp.ip_))
, m_serverPort(ipp.port_)
, m_server(server)
{
	m_rtmp = NULL;

	m_threadCount = 64;

	m_exit = false;

	m_checkThread = INVALID_OSTHREAD;
}

CRtmpServerImpl::~CRtmpServerImpl()
{
	StopService();
}

RmipError CRtmpServerImpl::Run()
{

}

int CRtmpServerImpl::StartService()
{
	int error = 0;

	if (server.CreateRTMPSocket(NULL, (unsigned short)m_serverPort, 64) < 0)
	{
		NPLogError(("%s | create tcp sockfd error!\n", __FUNCTION__));
		return -1;
	}

	OSCreateThread(&m_checkThread, NULL, CRtspServerImpl::ConntionCheckThread, this, 0);

	return 0;
}

void CRtmpServerImpl::StopService()
{
	m_exit = true;
}

CRtmpSession* CRtmpServerImpl::FetchSession_(void* sessionParam)
{
	{
		CNPAutoLock locker(m_sessionLocker);
		std::map<void*,CRtmpSession*>::iterator it = m_sessions.find(sessionParam);
		if(it!=m_sessions.end())
		{
			m_sessionCount[sessionParam] += 1;
			return it->second;
		}
	}

	CRtmpSession* s = NULL;
	m_server->CreateSession(sessionParam,&s);

	if(s)
	{
		CNPAutoLock locker(m_sessionLocker);
		m_sessions[sessionParam] = s;
		m_sessionCount[sessionParam] = 1;
	}

	return s;
}

void CRtspServerImpl::UnfetchSession_(void* sessionParam)
{
	CNPAutoLock locker(m_sessionLocker);
	std::map<void*,CRtmpSession*>::iterator it = m_sessions.find(sessionParam);
	if(it!=m_sessions.end())
	{
		m_sessionCount[sessionParam] -= 1;
	}
	else
	{
		m_sessionCount.erase(sessionParam);
	}
}

bool CRtmpServerImpl::ReleaseSession_(void* sessionParam)
{
	CNPAutoLock locker(m_sessionLocker);
	std::map<void*,CRtmpSession*>::iterator it = m_sessions.find(sessionParam);
	if(it!=m_sessions.end())
	{
		if(m_sessionCount.find(sessionParam)==m_sessionCount.end() || m_sessionCount[sessionParam] <= 0)
		{
			m_server->DeleteSession(it->second);
			m_sessions.erase(it);
			m_sessionCount.erase(sessionParam);
			return true;
		}
	}
	return false;
}

int CRtmpServerImpl::OnSend(void* param, const void* header, size_t len, const void* data, size_t bytes)
{
	CRtmpServerImpl* p = (CRtmpServerImpl*)param;
	if(p)
	{
		p->OnSend(header, len, data, bytes);
	}
	return 0;
}
int CRtmpServerImpl::OnSend(const void* header, size_t len, const void* data, size_t bytes)
{
	return 0;
}

int CRtmpServerImpl::OnPlay(void* param, const char* app, const char* stream, double start, double duration, uint8_t reset)
{
	CRtmpServerImpl* p = (CRtmpServerImpl*)param;
	if(p)
	{
		p->OnPlay(app, stream, start, duration, reset);
	}
	return 0;
}
int CRtmpServerImpl::OnPlay(const char* app, const char* stream, double start, double duration, uint8_t reset)
{
	return 0;
}

void CRtmpServerImpl::OnPause(void* param, int pause, uint32_t ms)
{
	CRtmpServerImpl* p = (CRtmpServerImpl*)param;
	if(p)
	{
		p->OnPause(pause, ms);
	}
	return 0;
}
void CRtmpServerImpl::OnPause(int pause, uint32_t ms)
{
	return 0;
}

void CRtmpServerImpl::OnSeek(void* param, uint32_t ms)
{
	CRtmpServerImpl* p = (CRtmpServerImpl*)param;
	if(p)
	{
		p->OnSeek(ms);
	}
	return 0;
}
void CRtmpServerImpl::OnSeek(uint32_t ms)
{
	return 0;
}

void CRtmpServerImpl::OnGetDuration(void* param, const char* app, const char* stream, double* duration)
{
	CRtmpServerImpl* p = (CRtmpServerImpl*)param;
	if(p)
	{
		p->OnGetDuration(app, stream, duration);
	}
}
void CRtmpServerImpl::OnGetDuration(const char* app, const char* stream, double* duration)
{
	return 0;
}


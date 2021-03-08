#include "rtmp-server.h"

CMyRtmpServer::CMyRtmpServer(IPP ipp)
: CRtmpServer(ipp)
{

}

CMyRtmpServer::~CMyRtmpServer()
{

}

int CMyRtmpServer::CreateSession(CRtmpSession* session)
{
	CNPAutoLock locker(m_sessionLocker);
	m_sessions.push_back(session);
	return 0;
}

int CMyRtmpServer::DeleteSession(CRtmpSession* session)
{
	CNPAutoLock locker(m_sessionLocker);
	std::vector<CRtmpSession*>::iterator it = m_sessions.begin();
	for(;it!=m_sessions.end();it++)
	{
		if(*it == session)
		{
			m_sessions.erase(it);
			delete session;
			break;
		}
	}
	return 0;
}
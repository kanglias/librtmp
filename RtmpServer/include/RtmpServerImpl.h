#ifndef _pub_rtsp_server_impl_h_
#define _pub_rtsp_server_impl_h_

#include "RtmpServer.h"
#include "ctypedef.h"
#include "npmsel.h"
#include "ILocker.h"
#include <map>
#include "TcpSocket.h"

class CRtmpServerImpl
{

public:

	CRtmpServerImpl(IPP ipp,CRtmpServer* server);

	virtual ~CRtmpServerImpl();

public:

	RmipError Run();

private:

	int StartService();

	void StopService();


private:

	static int AioWorkerThread(void* param);
	void AioWorkerThread();

public:

	int ConntionThread(CRtmpSession** rtmpSession);

private:

	IPP	m_serverIPP;
	std::string m_serverIp;
	int m_serverPort;

	CRtmpServer* m_server;

	void* m_rtmp;

	int m_threadCount;

	bool m_exit;

	OSThread m_checkThread;

	std::vector<OSThread> m_aioThreads;

	std::map<void*,CRtmpSession*> m_sessions;
	std::map<void*,int> m_sessionCount;
	CLocker m_sessionLocker;

	std::map<void*,LTMSEL> m_sessionTime;
	CLocker m_sessionTimeLocker;
	CTcpSocket server;

};

#endif

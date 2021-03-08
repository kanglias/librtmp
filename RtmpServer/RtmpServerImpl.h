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


	CRtmpSession* FetchSession_(void* sessionParam);

	void UnfetchSession_(void* sessionParam);

	bool ReleaseSession_(void* sessionParam);

private:
	static int OnSend(void* param, const void* header, size_t len, const void* data, size_t bytes);
	int OnSend(const void* header, size_t len, const void* data, size_t bytes);

	static int OnPlay(void* param, const char* app, const char* stream, double start, double duration, uint8_t reset);
	int OnPlay(const char* app, const char* stream, double start, double duration, uint8_t reset);

	static int OnPause(void* param, int pause, uint32_t ms);
	int OnPause(int pause, uint32_t ms);

	static int OnSeek(void* param, uint32_t ms);
	int OnSeek(uint32_t ms);

	static int OnGetDuration(void* param, const char* app, const char* stream, double* duration);
	int OnGetDuration(const char* app, const char* stream, double* duration);

private:

	static int AioWorkerThread(void* param);
	void AioWorkerThread();

	static int ConntionCheckThread(void* param);
	void ConntionCheckThread();

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

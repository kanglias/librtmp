#ifndef _cms_rtmp_server_
#define _cms_rtmp_server_

/*
说明：重载CRtspServer，接收RTSP信令并创建Session去处理信令。
线程安全：模块内部保证。
*/

#include "RtmpServer.h"
#include "rtmp-session.h"
#include "ILocker.h"

class CMyRtmpServer : public CRtmpServer
{

public:

	CMyRtmpServer(IPP ipp);

	virtual ~CMyRtmpServer();

public:

	///	创建信令Session，由本模块内部调用，需要重载。
	///	@param[in]	sessionParam	内部session指针，需要传给要新创建的重载后的CIscmMsgSession
	///	@param[out]	session			重载后的CIscmMsgSession指针
	virtual int CreateSession(CRtmpSession* session);

	///	销毁信令Session，由本模块内部调用，需要重载。
	///	@param[in]	session		CreateMsgSession创建的CIscmMsgSession指针
	virtual int DeleteSession(CRtmpSession* session);

private:

	std::vector<CRtmpSession*> m_sessions;
	CLocker m_sessionLocker;

};

#endif
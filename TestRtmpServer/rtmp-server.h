#ifndef _cms_rtmp_server_
#define _cms_rtmp_server_

/*
˵��������CRtspServer������RTSP�������Sessionȥ�������
�̰߳�ȫ��ģ���ڲ���֤��
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

	///	��������Session���ɱ�ģ���ڲ����ã���Ҫ���ء�
	///	@param[in]	sessionParam	�ڲ�sessionָ�룬��Ҫ����Ҫ�´��������غ��CIscmMsgSession
	///	@param[out]	session			���غ��CIscmMsgSessionָ��
	virtual int CreateSession(CRtmpSession* session);

	///	��������Session���ɱ�ģ���ڲ����ã���Ҫ���ء�
	///	@param[in]	session		CreateMsgSession������CIscmMsgSessionָ��
	virtual int DeleteSession(CRtmpSession* session);

private:

	std::vector<CRtmpSession*> m_sessions;
	CLocker m_sessionLocker;

};

#endif
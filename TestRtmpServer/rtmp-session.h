#ifndef _cms_rtsp_session_h_
#define _cms_rtsp_session_h_

/*
说明：RTSP Session处理模块，解析消息并调用相关的功能模块。
线程安全：实现者保证。
*/

#include "RtmpServer.h"

class CMyRtmpSession : public CRtmpSession
{

public:

	CMyRtmpSession();

	virtual ~CMyRtmpSession();

public:

	virtual int OnPlay(const char* app, const char* stream, double start, double duration, uint8_t reset);

	virtual int OnPause(int pause, uint32_t ms);

	virtual int OnSeek(uint32_t ms);

	virtual int OnGetDuration(const char* app, const char* stream, double* duration);

	virtual int OnClose();
};

#endif
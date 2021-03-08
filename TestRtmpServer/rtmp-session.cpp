#include "rtmp-session.h"
#include "nplog.h"

CMyRtmpSession::CMyRtmpSession()
:CRtmpSession()
{

}

CMyRtmpSession::~CMyRtmpSession()
{

}

int CMyRtmpSession::OnPlay(const char* app, const char* stream, double start, double duration, uint8_t reset)
{
	NPLogInfo(("OnPlay app %s, stream %s", app, stream));
	return 0;
}

int CMyRtmpSession::OnPause(int pause, uint32_t ms)
{
	NPLogInfo(("OnPause"));
	return 0;
}

int CMyRtmpSession::OnSeek(uint32_t ms)
{
	NPLogInfo(("OnSeek"));
	return 0;
}

int CMyRtmpSession::OnGetDuration(const char* app, const char* stream, double* duration)
{
	NPLogInfo(("app %s,stream %s, duration %ld", app, stream, *duration));
	return 0;
}

int CMyRtmpSession::OnClose()
{
	NPLogInfo(("OnClose"));
	return 0;
}
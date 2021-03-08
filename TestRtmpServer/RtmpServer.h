#ifndef _pub_rtsp_server_h_
#define _pub_rtsp_server_h_

/*
说明：本模块为RTSP的服务端，采用异步模式，在收到信令后，由重载的虚函数处理，采用线程池的方式对信令进行异步处理。
	CRtspServer：服务器模块，每个服务启用一个此模块。（此需要重载之后才能使用）
	CRtspSession：信令Session模块，每个信令都创建一个此模块使用。（此需要重载之后才能使用）
线程安全：实现者保证。
*/
#include "npdebug.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "rmiperr.h"
#include "npmsel.h"
#include "ctypedef.h"
#include "sys/sock.h"


//#ifdef __PUB_RTSP_SERVER_DLL__
//#define __DO_EXPORT_DLL__
//#endif
//#ifdef __PUB_RTSP_SERVER_LIB__
//#define __DO_EXPORT_LIB__
//#endif
//#include "ApiExport.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#define RTMPSERVERAPI_EXPORT __declspec(dllexport)
#define RTMPSERVERAPI_IMPORT __declspec(dllimport)
#define POMODULEAPI __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define RTMPSERVERAPI_EXPORT __attribute__((visibility ("default")))
#define RTMPSERVERAPI_IMPORT __attribute__((visibility ("default")))
#define POMODULEAPI __attribute__((visibility ("default")))
#else
#define RTMPSERVERAPI_EXPORT
#define RTMPSERVERAPI_IMPORT
#define POMODULEAPI
#endif
#endif

#ifdef RTMPSERVER_EXPORTS
#define RTMPSERVERAPI RTMPSERVERAPI_EXPORT
#else
#define RTMPSERVERAPI RTMPSERVERAPI_IMPORT
#endif

typedef struct _NaluUnit  
{  
	int type;  
	int size;  
	unsigned char *data;  
}NaluUnit;


class CRtmpSession;
class CRtmpServerImpl;
class RTMPSERVERAPI CRtmpServer
{

public:

	///	构造函数
	///	@param[in]	ipp		要运行的服务器地址
	CRtmpServer(IPP ipp);

	virtual ~CRtmpServer();

public:

	///	运行RTMP服务
	/// @return
	///	-	RMIP_OK		成功
	///	-	其它		失败,错误码
	RmipError Run();

public:
	///	创建信令Session，由本模块内部调用，需要重载。
	///	@param[in]	sessionParam	内部session指针，需要传给要新创建的重载后的CIscmMsgSession
	///	@param[out]	session			重载后的CIscmMsgSession指针
	int RtmpAccept(CRtmpSession** session);

public:

	///	创建信令Session，由本模块内部调用，需要重载。
	///	@param[in]	sessionParam	内部session指针，需要传给要新创建的重载后的CIscmMsgSession
	///	@param[out]	session			重载后的CIscmMsgSession指针
	virtual int CreateSession(CRtmpSession* session) = 0;

	///	销毁信令Session，由本模块内部调用，需要重载。
	///	@param[in]	session		CreateMsgSession创建的CIscmMsgSession指针
	virtual int DeleteSession(CRtmpSession* session) = 0;

private:

	CRtmpServerImpl* m_impl;
};

class RTMPSERVERAPI CRtmpSession
{

public:

	CRtmpSession();

	virtual ~CRtmpSession();

public:

	virtual int OnPlay(const char* app, const char* stream, double start, double duration, uint8_t reset) = 0;

	virtual int OnPause(int pause, uint32_t ms) = 0;

	virtual int OnSeek(uint32_t ms) = 0;

	virtual int OnGetDuration(const char* app, const char* stream, double* duration) = 0;

	virtual int OnClose() = 0;

public:
	
	static int OnPlay(void* param, const char* app, const char* stream, double start, double duration, uint8_t reset);

	static int OnPause(void* param, int pause, uint32_t ms);

	static int OnSeek(void* param, uint32_t ms);

	static int OnGetDuration(void* param, const char* app, const char* stream, double* duration);

	static int OnSend(void* param, const void* header, size_t len, const void* data, size_t bytes);
	int OnSend(const void* header, size_t len, const void* data, size_t bytes);

	///	得到http对端的ip和port
	///	@param[out]	ip		
	///	@param[out]	port		
	/// @return
	///	-	0		成功
	///	-	其它		失败,错误码
	int GetClientIPP(const char** ip, int* port);

	int CreateSocket(socket_t sockfd);

	int fnRtmpData(const void* data, int size, unsigned int timestamp, bool isIFrame, int streamtype);

private:

	static int AioWorkerThread(void* param);
	void AioWorkerThread();

	static int OnFlvPacket(void* flv, int type, const void* data, size_t bytes, uint32_t timestamp);
	int OnFlvPacket(int type, const void* data, size_t bytes, uint32_t timestamp);

	bool ReadOneNaluFromBuf(NaluUnit &nalu,uint8_t *buf, int buf_size);

	int OnFlvCreate();

	

private:

	void* m_rtmp;
	OSThread m_checkThread;
	void* m_client;
	unsigned char* m_received_buffer;
	bool m_bRuning;
	void* m_flv;
	int pts;

	int audio_pts;
	int nalhead_pos;
	int m_port;
	std::string m_ip;
};

#endif

#include "RtmpServer.h"
#include "RtmpServerImpl.h"
#include "rtmp-server.h"
#include "TcpSocket.h"
#include "flv-muxer.h"
#include "flv-proto.h"

#define MAX_BUF_SIZE (1024 * 10)
#define MAX_RTMP_BUF 2048

CRtmpServer::CRtmpServer(IPP ipp)
{
	m_impl = new CRtmpServerImpl(ipp,this);
}

CRtmpServer::~CRtmpServer()
{
	delete m_impl;
}

RmipError CRtmpServer::Run()
{
	return m_impl->Run();
}

int CRtmpServer::RtmpAccept(CRtmpSession** rtmpSession)
{
	return m_impl->ConntionThread(rtmpSession);
}

//////////////////////////////////////////////////////////////////////////

CRtmpSession::CRtmpSession()
{
	m_rtmp = NULL;
	m_flv = NULL;
	m_checkThread = INVALID_OSTHREAD;
	m_received_buffer = NULL;
}

CRtmpSession::~CRtmpSession()
{
	m_bRuning = false;

	if(m_checkThread != INVALID_OSTHREAD)
	{		
		OSCloseThread(m_checkThread);
		m_checkThread = INVALID_OSTHREAD;
	}

	if (NULL != m_received_buffer)
	{
		delete []m_received_buffer;
		m_received_buffer = NULL;
	}

	if (m_rtmp)
	{
		rtmp_server_destroy((rtmp_server_t*)m_rtmp);
		m_rtmp = NULL;
	}

	if (m_flv)
	{
		flv_muxer_destroy((flv_muxer_t*)m_flv);
		m_flv = NULL;
	}
}

int CRtmpSession::CreateSocket(socket_t sockfd)
{
	m_client = new CTcpSocket(sockfd);
	m_received_buffer = new unsigned char[MAX_BUF_SIZE];
	memset(m_received_buffer, 0, MAX_BUF_SIZE);
	m_bRuning = true;

	struct rtmp_server_handler_t handler;
	memset(&handler, 0, sizeof(handler));
	handler.send = OnSend;
	handler.onplay = OnPlay;
	handler.onpause = OnPause;
	handler.onseek = OnSeek;
	handler.ongetduration = OnGetDuration;

	m_rtmp = rtmp_server_create(this, &handler);

	OSCreateThread(&m_checkThread, NULL, CRtmpSession::AioWorkerThread, this, 0);
	return 0;
}

int CRtmpSession::AioWorkerThread(void* param)
{
	CRtmpSession* ptr = (CRtmpSession*)param;
	ptr->AioWorkerThread();
	return 0;
}

void CRtmpSession::AioWorkerThread()
{
	int iRecvd = 0;

	while(m_bRuning)
	{
		iRecvd = ((CTcpSocket*)m_client)->Receive(m_received_buffer, MAX_BUF_SIZE, 5000);
		if (((CTcpSocket*)m_client)->PeerDisconnected())
		{
			m_bRuning = false;
			OnClose();
			break;
		}
		if (0 == iRecvd) // timeout
		{
			continue;
		}

		if (iRecvd >= MAX_BUF_SIZE) 
		{
			memset(m_received_buffer, 0, MAX_BUF_SIZE);
			continue;
		}
		rtmp_server_input((rtmp_server_t *)m_rtmp, m_received_buffer, iRecvd);
	}
}

int CRtmpSession::OnPlay(void* param, const char* app, const char* stream, double start, double duration, uint8_t reset)
{
	CRtmpSession* ptr = (CRtmpSession*)param;
	int ret = ptr->OnPlay(app, stream, start, duration, reset);
	if (ret == 0)
	{
		return ptr->OnFlvCreate();
	}
	return 0;
}
int CRtmpSession::OnFlvCreate()
{
	m_flv = flv_muxer_create(OnFlvPacket, this);
	return 0;
}

int CRtmpSession::OnPause(void* param, int pause, uint32_t ms)
{
	CRtmpSession* ptr = (CRtmpSession*)param;
	return ptr->OnPause(pause, ms);
}

int CRtmpSession::OnSeek(void* param, uint32_t ms)
{
	CRtmpSession* ptr = (CRtmpSession*)param;
	return ptr->OnSeek(ms);
}

int CRtmpSession::OnGetDuration(void* param, const char* app, const char* stream, double* duration)
{
	CRtmpSession* ptr = (CRtmpSession*)param;
	return ptr->OnGetDuration(app, stream, duration);
}

int CRtmpSession::OnSend(void* param, const void* header, size_t len, const void* data, size_t bytes)
{
	CRtmpSession* ptr = (CRtmpSession*)param;
	return ptr->OnSend(header, len, data, bytes);
}

int CRtmpSession::OnSend(const void* header, size_t len, const void* data, size_t bytes)
{
	socket_t socket = ((CTcpSocket*)m_client)->m_socket;
	socket_bufvec_t vec[2];
	socket_setbufvec(vec, 0, (void*)header, len);
	socket_setbufvec(vec, 1, (void*)data, bytes);
	return socket_send_v(socket, vec, bytes > 0 ? 2 : 1, 0);
}

int CRtmpSession::fnRtmpData(const void* data, int size, unsigned int timestamp, bool isIFrame, int streamtype)
{
	if (streamtype == 3)
	{
		flv_muxer_aac((flv_muxer_t*)m_flv, data, size, audio_pts, audio_pts);
		audio_pts += 1024;
	}
	else
	{
		nalhead_pos = 0;
		NaluUnit nalu;
		while (ReadOneNaluFromBuf(nalu, (uint8_t*)data, size))
		{
			flv_muxer_h264_nalu((flv_muxer_t*)m_flv, nalu.data, nalu.size, pts, pts);
		}
		pts += 3600;
		delete[] nalu.data;
		nalu.data = NULL;
	}
	return 0;
}

bool CRtmpSession::ReadOneNaluFromBuf(NaluUnit &nalu,uint8_t *buf, int buf_size)  
{    
	int pos = 0;
	int naltail_pos=nalhead_pos;
	while(nalhead_pos<buf_size)  
	{  
		//search for nal header
		if(buf[nalhead_pos++] == 0x00 && 
			buf[nalhead_pos] == 0x00) 
		{
			if(buf[nalhead_pos + 1] == 0x01)
			{
				nalhead_pos += 2;
				goto gotnal_head;
			}
			else 
			{
				//cuz we have done an i++ before,so we need to roll back now
				//nalhead_pos--;		
				if(buf[nalhead_pos + 1] == 0x00 && buf[nalhead_pos + 2] == 0x01)
				{
					nalhead_pos += 3;
					goto gotnal_head;
				}	
				else
					continue;
			}
		}
		else 
			continue;

		//search for nal tail which is also the head of next nal
gotnal_head:
		//normal case:the whole nal is in this m_pFileBuf
		naltail_pos = nalhead_pos;  
		while (naltail_pos < buf_size)  
		{  
			if(buf[naltail_pos++] == 0x00 && 
				buf[naltail_pos] == 0x00 )
			{  
				if(buf[naltail_pos + 1] == 0x01)
				{
					naltail_pos += 2;
					pos = naltail_pos-3;
					nalu.size = pos-nalhead_pos;
					break;
				}
				else
				{
					//naltail_pos--;
					if(buf[naltail_pos + 1] == 0x00 &&
						buf[naltail_pos + 2] == 0x01)
					{	
						naltail_pos += 3;
						pos = naltail_pos-4;
						nalu.size = pos-nalhead_pos;
						break;
					}
				}
			}  
		}
		if(naltail_pos>=buf_size)
		{
			nalu.size = buf_size - nalhead_pos;
		}

		nalu.type = buf[nalhead_pos]&0x1f; 
		nalu.data=(unsigned char*)malloc(nalu.size);
		memset(nalu.data, 0, nalu.size);
		memcpy(nalu.data,buf+nalhead_pos,nalu.size);
		if(naltail_pos>=buf_size)
		{
			nalhead_pos = naltail_pos;
		}
		else
		{
			nalhead_pos = pos;
		}

		return TRUE;   		
	}
	return FALSE;
}


int CRtmpSession::OnFlvPacket(void* flv, int type, const void* data, size_t bytes, uint32_t timestamp)
{
	CRtmpSession* rtmp = (CRtmpSession*)flv;
	if (rtmp)
	{
		return rtmp->OnFlvPacket(type, data, bytes, timestamp);
	}
	return 0;
}

int CRtmpSession::OnFlvPacket(int type, const void* data, size_t bytes, uint32_t timestamp)
{
	int r = 0;

	if (FLV_TYPE_AUDIO == type)
	{
		r = rtmp_server_send_audio((rtmp_server_t *)m_rtmp, data, bytes, timestamp);
	}
	else if (FLV_TYPE_VIDEO == type)
	{
		r = rtmp_server_send_video((rtmp_server_t *)m_rtmp, data, bytes, timestamp);
	}
	else if (FLV_TYPE_SCRIPT == type)
	{
		r = rtmp_server_send_script((rtmp_server_t *)m_rtmp, data, bytes, timestamp);
	}
	return r;
}
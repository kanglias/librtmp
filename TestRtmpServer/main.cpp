#include "rtmp-server.h"

int main()
{
	CMyRtmpServer* s = new CMyRtmpServer(IPP("127.0.0.1",1935));
	s->Run();
	while(1)
	{
		CRtmpSession* session = new CMyRtmpSession;
		int ret = s->RtmpAccept(&session);
		if (ret == 0)
		{
			s->CreateSession(session);
		}
		else
		{
			delete session;
			session = NULL;
		}
		
	}

	return 0;
}
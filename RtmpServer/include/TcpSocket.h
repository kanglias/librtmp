#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include "InternetSocket.h"

class CTcpSocket:public CInternetSocket
{
public:

	/*!
	 * Construct a TCP socket
	 * \exception SocketException thrown if unable to create the socket
	 */
	CTcpSocket();

	/*!
	 * Construct a Socket from a Handle returned by accept()
	 * \exception SocketException thrown if handle is invalid
	 */
	CTcpSocket(socket_t sockfd);

	/*!
	 * \create socket
	 * ip is local bind address 
	 * port is local bind port 
	 * backlog is listen count 
	 * binds to any available interface / address
	 */
	int CreateSocket(char* ip, unsigned short port, int backlog = 8);

	int CreateRTMPSocket(char* ip, unsigned short port, int backlog = 8);

	//! wait for another socket to connect
	socket_t Accept() const;

	/*!
	 * wait for another socket to connect
	 * timeout is ms
	 */
	socket_t Accept(int timeout) const;


	socket_t GetSocket();
};

#endif // _TCP_SOCKET_H_

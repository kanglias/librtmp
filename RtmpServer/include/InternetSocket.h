#ifndef _INTERNET_SOCKET_H_
#define _INTERNET_SOCKET_H_

#include "stdint.h"
#include "sys/sock.h"

#ifdef WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

//typedef int socket_t;

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
//! Internet socket class
	class CInternetSocket
	{
	public:

		~CInternetSocket();

		//! send data through a connected socket
		/*!
		 * \param buffer data to be send
		 * \param len length of the data to be sent
		 * \return number of bytes sent
		 * \exception SocketException if sending went wrong
		 */
		int Send( const void* buffer, size_t len);
		
		//! receive data from a bound socket
		/*!
		 * \param buffer the buffer the received data will be written to
		 * \param len length of the provided buffer, receive will not read more than that
		 * \return number of bytes received
		 * \exception SocketException in case an error occured
		 */
		int Receive( void* buffer, size_t len);

		//! receive data from a bound socket, return after the given timespan
		/*!
		 * \param buffer the buffer the received data will be written to
		 * \param len length of the provided buffer, receive will not read more than that
		 * \param timeout the timeout in ms after which receive will give up and return
		 * \return number of bytes received, 0 on timeout
		 * \exception SocketException in case an error occured
		 */
		int Receive( void* buffer, size_t len, int timeout);
		
		//! Close the connection
		void Close();

		//! returns whether a peer disconnected
		/*!
		 * Will only work if you use a connection oriented, connected socket.
		 * Returns true if the peer disconnected. Use this function after
		 * a call to receive, returned 0 received bytes.
		 *
		 * \return true if a peer disconnected
		 */
		bool PeerDisconnected() const;

	protected:

		//! enables return of an accepted socket
		CInternetSocket(socket_t sockfd);

		//! allows a subclass to create new socket
		CInternetSocket();

	private:
		// dont' allow
		CInternetSocket(const CInternetSocket&);
		const CInternetSocket& operator=( const CInternetSocket&);

	public:
		socket_t m_socket;
		bool m_peerDisconnected;
	};

	

#endif // _INTERNET_SOCKET_H_


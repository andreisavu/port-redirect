/*
  (c) GarajCode
  Clasa pentru winsock - initializare automata
  Date : 30 iunie 2005
  Last date : 23 noiembrie 2005
  Change log :
	23 noiembrie 2005 - reference count - prevent reinitialisation
					  - more informations
*/

#ifndef GARAJ_WINSOCK_H
#define GARAJ_WINSOCK_H

#include <winsock.h>

namespace GN
{

	class CWinsock
	{
	public:
		CWinsock();
		~CWinsock();

		int max_tcp_sockets() const;
		int max_udp_datagrams() const;
		const char * vendor_info() const;
		const char * description() const;

	protected:
		WSAData m_wsadata;
		static int m_refc;

	};	// CWinsock

};	// namespace GN

#endif // GARAJ_WINSOCK_H
/*
  (c) GarajCode
  Clasa pentru winsock - initializare automata
  Date : 30 iunie 2005
  Last date : 23 noiembrie 2005
  Change log :
	23 noiembrie 2005 - instance count
					  - more informations
*/

#include "gwinsock.h"
#include "gerror.h"

namespace GN
{
	int CWinsock::m_refc = 0;

	CWinsock::CWinsock()
	{
		if( m_refc ) 
		{
			// increment instance count
			m_refc++;
			return;
		}


		GSTD::CError error;

		// initialise Winsock
		error.set_code( WSAStartup(MAKEWORD(1,1),&m_wsadata) );

		// check error code
		if( error.get_code() )
		{
			switch( error.get_code() )
			{
			case WSASYSNOTREADY:
				error.set_text("Network subsystem not ready");
				break;
			case WSAVERNOTSUPPORTED:
				error.set_text("Version not suported 2.0");
				break;
			case WSAEINPROGRESS:
				error.set_text("A blocking operation is in progress");
				break;
			case WSAEPROCLIM:
				error.set_text("Implementation task limit has reached");
				break;
			case WSAEFAULT:
				error.set_text("Invalid pointer");
				break;
			}

			throw (GSTD::CError)error;
		}

		// increment instance count
		m_refc++;
	}

	// returneaza numarul maxim de conexiuni simultane
	int CWinsock::max_tcp_sockets() const
	{
		return m_wsadata.iMaxSockets;
	}

	// numarul maxim de conexiuni UDP posibile
	int CWinsock::max_udp_datagrams() const
	{
		return m_wsadata.iMaxUdpDg;
	}

	// informatii despre vendor
	const char * CWinsock::vendor_info() const 
	{
		return m_wsadata.lpVendorInfo;
	}

	// winsock implementation description
	const char * CWinsock::description() const
	{
		return m_wsadata.szDescription;
	}

	CWinsock::~CWinsock()
	{
		// check to see if this is the last instance
		m_refc--;
		if( m_refc ) return;

		// cleanup winsock
		if( WSACleanup() == SOCKET_ERROR)
		{
			GSTD::CError error;

			error.set_code( WSAGetLastError() );

			switch( error.get_code() )
			{
			case WSANOTINITIALISED:
				error.set_text( "Winsock not initialised");
				break;
			case WSAENETDOWN:
				error.set_text( "Network subsystem down");
				break;
			case WSAEINPROGRESS:
				error.set_text( "A blocking operation is in progress");
				break;
			}

			throw (GSTD::CError)error;
		}
	}

};	// namespace GN

// (c) GarajCode 2005 - programmed by Savu Andrei
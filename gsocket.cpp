/*
  (c) GarajCode
  Clasa pentru socket
  Date : 30 iunie 2005
  Last date : 23 noiembrie 2005
*/

/*
	Change log :
		10 august 2005 - adaugata functia pentru impachetare IP in DWORD
		11 august 2005 - safe_send()
		23 august 2005 - thread-safe multiblock send
		6 septembrie 2005 - schimbare interfata clasa
		23 noiembrie 2005 - data type fix
  */

#include <winsock.h>

#include "gsocket.h"

namespace GN
{

	// functie auxiliara care impacheteaza un IP
	DWORD pack_ip( char * ip )
	{
		char buffer[20];
		int aux,m_ip_pack;

		strcpy( buffer, ip);

		char *p = strtok( buffer, ". " );
		aux = atoi(p);
		m_ip_pack = aux;

		p = strtok( NULL, ". " );
		while( p )
		{
			m_ip_pack <<= 8;
			aux = atoi(p);
			m_ip_pack += aux;

			p = strtok( NULL, ". ");
		}		

		return m_ip_pack;
	}

	// returneaza adresa locala, functia este folosita atunci
	// cand se creeaza un socket pentru server si care are
	// ca adresa IP adresa locala
	int GetLocalAddress(LPSTR lpStr, LPDWORD lpdwStrLen)
	{
		struct in_addr *pinAddr;
		LPHOSTENT	lpHostEnt;
		int			nRet;
		int			nLen;

		//
		// Get our local name
		//
		nRet = gethostname(lpStr, *lpdwStrLen);
		if (nRet == SOCKET_ERROR)
		{
			lpStr[0] = '\0';
			return SOCKET_ERROR;
		}

		//
		// "Lookup" the local name
		//
		lpHostEnt = gethostbyname(lpStr);
		if (lpHostEnt == NULL)
		{
			lpStr[0] = '\0';
			return SOCKET_ERROR;
		}	

		//
		// Format first address in the list
		//
		pinAddr = ((LPIN_ADDR)lpHostEnt->h_addr);
		nLen = strlen(inet_ntoa(*pinAddr));
		if ((DWORD)nLen > *lpdwStrLen)
		{
			*lpdwStrLen = nLen;
			WSASetLastError(WSAEINVAL);
			return SOCKET_ERROR;
		}

		*lpdwStrLen = nLen;
		strcpy(lpStr, inet_ntoa(*pinAddr));
	    return 0;
	}

	// initializare
	CSocket::CSocket( SOCKET s ):
		m_socket( s )
	{
		m_ip[0] = 0;		
	}

	// creeaza socket pentru conexiune TCP/IP
	void CSocket::create()
	{
		m_socket = socket(AF_INET, SOCK_STREAM, 0);

		if( m_socket == INVALID_SOCKET )
		{
			GSTD::CError error;
			
			error.set_code( WSAGetLastError() );

			switch( error.get_code() )
			{
			case WSANOTINITIALISED:
				error.set_text( "Winsock not initialised");
				break;
			case WSAENETDOWN:
				error.set_text( "The network subsystem has failed");
				break;
			case WSAEAFNOSUPPORT:
				error.set_text( "Address family not suported");
				break;
			case WSAEINPROGRESS:
				error.set_text( "A blocking operation in progress");
				break;
			case WSAEMFILE:
				error.set_text( "Reached winsock limit");
				break;
			case WSAENOBUFS:
				error.set_text( "No buffer space avaible");
				break;
			case WSAEPROTONOSUPPORT:
				error.set_text( "Protocol not supported");
				break;
			case WSAEPROTOTYPE:
				error.set_text( "Wrong protocol for this socket");
				break;
			case WSAESOCKTNOSUPPORT:
				error.set_text( "Wrong socket for address family");
				break;
			default:
				error.set_text( "Unknown error");
				break;
			}
				
			throw (GSTD::CError)error;
		}
		
		// gaseste adresa IP locala
		DWORD str_len=20;
		GetLocalAddress(m_ip,&str_len);

		// calculeaza ip-ul impachetat
		m_ip_pack = pack_ip( m_ip );
	}

	// inchide socket 
	void CSocket::close()
	{
		try
		{
			m_mutex.lock();
		}
		catch( GSTD::CError error )
		{
			throw error;
		}
		if( m_socket == INVALID_SOCKET) return;
		if( closesocket( m_socket ) == SOCKET_ERROR )
		{
			GSTD::CError error;

			error.set_code( WSAGetLastError() );

			switch( error.get_code() )
			{
			case WSANOTINITIALISED:
				error.set_text( "Winsock not initialised");
				break;
			case WSAENETDOWN:
				error.set_text( "Network subsystem has failed");
				break;
			case WSAENOTSOCK:
				error.set_text( "Parameter is not socket");
				break;
			case WSAEINPROGRESS:
				error.set_text( "A blocking operation is in progress");
				break;
			case WSAEINTR:
				error.set_text( "Call canceled");
				break;
			case WSAEWOULDBLOCK:
				error.set_text( "Socket is marked as nonblocking");
				break;
			default:
				error.set_text( "Unknown error");
				break;
			}
			
			throw (GSTD::CError)error;
		}
		m_socket = INVALID_SOCKET;
		try
		{
			m_mutex.unlock();
		}
		catch( GSTD::CError error )
		{
			throw error;
		}
	}

	void CSocket::connect(int port, char *lpServerName)
	{
		try
		{
			m_mutex.lock();
		}
		catch( GSTD::CError error )
		{
			throw error;
		}
		// verifica initializarea socket-ului
		if( m_socket == INVALID_SOCKET ) 
		{
			// initializeaza
			try
			{
				create();
			}
			catch(GSTD::CError error)
			{
				throw (GSTD::CError)error;
			}
		}

		struct hostent *lpHostEntry;
		struct in_addr iaHost;

		iaHost.s_addr = inet_addr(lpServerName);
		if (iaHost.s_addr == INADDR_NONE)
		{
			// Nu a fost IP, poate e nume de server
			lpHostEntry = gethostbyname(lpServerName);
		}
		else
		{
			// Adresa de IP
			lpHostEntry = gethostbyaddr((const char *)&iaHost,sizeof(struct in_addr), AF_INET);
		}
		if (lpHostEntry == NULL)
		{
			// a aparut o eroare
			// initializeaza structura si 
			// arunca o exceptie
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
			case WSAHOST_NOT_FOUND:
				error.set_text( "Host not found");
				break;
			case WSATRY_AGAIN:
				error.set_text( "Host not found or server fail");
				break;
			case WSANO_RECOVERY:
				error.set_text( "Nonrecoverable error occurred");
				break;
			case WSANO_DATA:
				error.set_text( "No data record of requested type");
				break;
			case WSAEINPROGRESS:
				error.set_text( "A blocking operation is in progress");
				break;
			case WSAEFAULT:
				error.set_text( "Invalid argument");
				break;
			case WSAEINTR:
				error.set_text( "Call was canceled");
				break;
			default:
				error.set_text( "Unknown error");
				break;
			}
				
			throw (GSTD::CError)error;
		}
	
		// Seteaza portul pentru conexiune
		SOCKADDR_IN saServer;
		saServer.sin_port = htons((unsigned short)port);
	
		// Umple restul din structura pentru server
		saServer.sin_family = AF_INET;
		saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

		// Conecteaza la server
		int nRet;
		nRet = ::connect(m_socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
		if (nRet == SOCKET_ERROR)
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
			case WSAEADDRINUSE:
				error.set_text( "Address in use");
				break;
			case WSAEINTR:
				error.set_text( "Call canceled");
				break;
			case WSAEINPROGRESS:
				error.set_text( "A blocking call is in progress");
				break;
			case WSAEALREADY:
				error.set_text( "A nonblocking call is in progress");
				break;
			case WSAEADDRNOTAVAIL:
				error.set_text( "Invalid address from local machine");
				break;
			case WSAEAFNOSUPPORT:
				error.set_text( "Invalid family address");
				break;
			case WSAECONNREFUSED:
				error.set_text( "Attempt to connect was rejected");
				break;
			case WSAEFAULT:
				error.set_text( "Parameters error");
				break;
			case WSAEINVAL:
				error.set_text( "Connect on listening socket");
				break;
			case WSAEISCONN:
				error.set_text( "Socket already connected");
				break;
			case WSAENETUNREACH:
				error.set_text( "The network is not accesible");
				break;
			case WSAENOBUFS:
				error.set_text( "No space for buffer");
				break;
			case WSAENOTSOCK:
				error.set_text( "Descriptor is not a socket");
				break;
			case WSAETIMEDOUT:
				error.set_text( "Attemp to connect time exceded");
				break;
			default:
				error.set_text( "Unknown error");
				break;
			}
			throw (GSTD::CError)error;
		}
		try
		{
			m_mutex.unlock();
		}
		catch( GSTD::CError error )
		{
			throw error;
		}
	}	

	// asculta port pentru conectare clienti
	void CSocket::listen( int port )
	{
		if(m_socket == INVALID_SOCKET)
		{
			try
			{
				create();
			}
			catch(GSTD::CError error)
			{
				throw (GSTD::CError)error;
			}
		}

		struct sockaddr_in server;

		// seteaza informatii
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		server.sin_addr.s_addr = INADDR_ANY;

		// asociaza socket cu portul
		if (bind(m_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		{
			GSTD::CError error;

			error.set_code( WSAGetLastError() );

			switch(error.get_code())
			{
			case WSANOTINITIALISED:
				error.set_text( "Winsock not initialised");
				break;
			case WSAENETDOWN:
				error.set_text( "Network subsystem down");
				break;
			case WSAEADDRINUSE:
				error.set_text( "Address in use");
				break;
			case WSAEINVAL:
				error.set_text( "Socket already bound");
				break;
			case WSAENOBUFS:
				error.set_text( "No space for buffer");
				break;
			case WSAENOTSOCK:
				error.set_text( "Parameters error");
				break;
			default:
				error.set_text( "Unknown error");
				break;
			}

			throw (GSTD::CError)error;
		}
	
		// seteaza socket-ul pentru a asculta un port
		if ( ::listen(m_socket, 5) == SOCKET_ERROR )
		{
			GSTD::CError error;
			
			error.set_code( WSAGetLastError() );

			switch( error.get_code() )
			{
			case WSAEISCONN:
				error.set_text( "Already connected");
				break;
			case WSAEMFILE:
				error.set_text( "No socket descriptor avaible");
				break;
			case WSAENOBUFS:
				error.set_text( "No space for buffers");
				break;
			default:
				error.set_text( "Unknown error");
				break;
			}
	
			throw (GSTD::CError)error;	
		}
	}

	// accepta o conexiune
	CSocket CSocket::accept()
	{
		SOCKET s;
		SOCKADDR_IN addr;
		int len = sizeof(SOCKADDR_IN);
		s = ::accept(m_socket, (LPSOCKADDR)&addr,&len);

		if( s == INVALID_SOCKET )
		{
			GSTD::CError error;
			error.pack( WSAGetLastError(), "Unable to accept connection");
			throw (GSTD::CError)error;
		}

		CSocket ret(s);
		strcpy(ret.m_ip, inet_ntoa(addr.sin_addr));

		// calculeaza ip-ul impachetat
		ret.m_ip_pack = pack_ip( ret.m_ip );
		
		return ret;
	}

	// recepteaza informatii
	int CSocket::receive(char *buffer, int len) const
	{
		int ret = recv(m_socket, buffer, len, 0);

		if( !ret || ret == SOCKET_ERROR )
		{
			GSTD::CError error;

			error.pack( WSAGetLastError(), "Receive error");

			throw (GSTD::CError)error;
		}

		return ret;
	}

	// trimite informatii
	void CSocket::send(const char *buffer, int len) const
	{
		// acces exclusiv
		try
		{
			m_mutex.lock();
		}
		catch( GSTD::CError error )
		{
			throw error;
		}
		int ret = ::send(m_socket, buffer, len, 0);

		if( ret!=len || ret == SOCKET_ERROR)
		{
			GSTD::CError error( WSAGetLastError(), "Send error");
			throw (GSTD::CError)error;
		}
		// eliberare acces
		try
		{
			m_mutex.unlock();
		}
		catch( GSTD::CError error )
		{
			throw error;
		}
	}

	// thread-safe multiblock send

	// acces exclusiv la trimitere
	void CSocket::std_lock()
	{
		try
		{
			m_stdmtx.lock();
		}
		catch( GSTD::CError error )
		{
			throw error;
		}
	}

	// trmite block de mesaj
	void CSocket::std_send( const char * buffer, int len ) const
	{
		int ret = ::send(m_socket, buffer, len, 0);

		if( ret!=len || ret == SOCKET_ERROR)
		{
			GSTD::CError error( WSAGetLastError(), "Send error");
			throw (GSTD::CError)error;
		}
	}

	// elibereaza acces exclusiv
	void CSocket::std_unlock()
	{
		try
		{
			m_stdmtx.unlock();
		}
		catch( GSTD::CError error )
		{
			throw error;
		}
	}

};	// namespace GN

// (c) GarajCode 2005 - programmed by Savu Andrei
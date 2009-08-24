/*
  (c) GarajCode
  Clasa pentru socket
  Date : 30 iunie 2005
  Last date : 23 noiembrie 2005
*/

/*
	Change log :
		10 august 2005 - adaugata functia pentru impachetare ip in DWORD
		11 august 2005 - safe_send()
		23 august 2005 - thread-safe multiblock send
		6 septembrie 2005 - schimbare interfata clasa
						  - aranjare cod
		23 noiembire 2005 - data type fix
  */

#ifndef GARAJ_SOCKET_H
#define GARAJ_SOCKET_H

#include "gwinsock.h"
#include "gerror.h"
#include "gmutex.h"

namespace GN	// GARAJ_NETWORK
{

	class CSocket
	{
	public:
		// initializare
		CSocket(SOCKET s=INVALID_SOCKET);

		// creaza socket pentru conexiune TCP/IP 
		void create();
		// inchide socket-ul
		void close();

		// se conecteaza la un host dat prin nume sau ip in forma xxx,xxx,xxx,xxx
		void connect(int port,char *host_or_ip);

		// seteaza socket ca server pentru un anumit port
		void listen(int port);
		// accepta o conexiune 
		CSocket accept();

		// recepteaza informatii in buffer 
		int receive(char *buffer,int len) const; 
		
		// trimite informatii
		void send(const char *buffer,int len) const;
		
		// thread-safe multiblock send
		void std_lock();
		void std_unlock();
		void std_send( const char * buffer, int len ) const; 

		// adresa ip a socket-ului
		const char *get_ip() const { return m_ip; }
				
		// returneaza ip-ul impachetat intr-un DWORD
		DWORD get_ip_pack() const { return m_ip_pack; }

		// transmite orice fel de structura de date
		// ATENTIE : la structuri dinamice		
		template<class T> 
		const CSocket & operator << (const T value) const
		{
			try
			{
				send((char*)&value, sizeof(value));
			}
			catch(GSTD::CError error)
			{
				throw (GSTD::CError)error;
			}

			return *this;
		}

		// receptioneaza orice fel de structura
		// ATENTIE la structurile dinamice
		template<class T>
		const CSocket & operator >> (T &value) const
		{
			try
			{
				receive((char*)&value, sizeof(value));
			}
			catch(GSTD::CError error)
			{
				throw (GSTD::CError)error;
			}
			
			return *this;
		}
		
	protected:
		char m_ip[20];			// adresa ip 
		DWORD m_ip_pack;		// adresa ip impachetata
		SOCKET m_socket;		// socket-ul real
		GSTD::CMutex m_mutex;	// pentru thread-safe, nu stiu daca este chiar necesar
		GSTD::CMutex m_stdmtx;	// mutex pentru thread-safe multiblock send

	};	// CSocket

	// functie auxiliara care impacheteaza un IP
	DWORD pack_ip( char * ip );

};	// namespace GN

#endif // GARAJ_SOCKET_H



// (c) GarajCode 2005 - programmed by Savu Andrei
/*
  (c) GarajCode 2005
  Multithreaded server
  Date : 30 iunie 2005
  Last date : 23 noiembrie 2005
*/

/*
	Change log :
		10 august 2005 - suport pentru gestionare MCSC
		11 august 2005 - suport pentru trusted ip
  				       - suport pentru banned ip
		6 septembrie 2005 - modificare interfata clasa
		7 septembrie 2005 - utilizare arbori binari la cautare IP
		31 octombrie 2005 - win9x hack - problema cu create thread 
							using define WIN9X_HACK - _beginthread();
*/

#ifndef GARAJ_SERVER_H
#define GARAJ_SERVER_H

#ifndef _MT		// proiectul threbuie setat pentru aplicatii multithread
	#error Please set your compiler to generate Multithreaded aplications.
#endif

#include "gsocket.h"

/*
	Daca compilezi programul pe un sistem de operare Win9x
	trebuie sa definesti WIN9X_HACK pentru a evita o
	problema care apare la folosirea functiei CreateThread
  */
// #define WIN9X_HACK	

// dezactiveaza avertizarile pentru STL
#pragma warning(disable:4786)
#pragma warning(disable:4786)

#include <list>
#include <set>

namespace GN
{
	class CServer;

	// lista in care sunt retinuti clientii
	// orice client conectat este adaugat in lista
	// interna mentinuta de server
	typedef std::list<CSocket *> CLIENT_LIST;

	// folosit pentru traversarea listei de clienti
	typedef std::list<CSocket *>::iterator CLIENT_LIST_ITERATOR;

	// functiile necesare pentru server
	
	// functia responsabila pentru protocolul de comunicare cu clientul
	// fiecare client conectat este pasat acestei functii pentru a fi 
	// gestionat. orice eroare poate fi semnalata printr-o exceptie
	// care va fi semnalata ca un eveniment SM_CLIENT_ERROR in event handler
	typedef void (*SERVER_PROCEDURE)(CLIENT_LIST &,CSocket &);

	// aceasta functie primeste informatii despre orice fel de eveniment
	// care are loc la nivelul server-ului. in cazul mesajului SM_CLIENT_CONNECTED
	// valoarea de return a acestei functii determina daca clientul este
	// acceptat sau conexiunea este inchisa imediat.
	typedef bool (*SERVER_EVENT_HANDLER)(UINT msg , GSTD::CError *error, CSocket *);


	// lista de ip in care se are incredere si carora le este permisa conectarea
	typedef std::set<DWORD> IP_LIST;
	typedef IP_LIST TRUSTED_IP_LIST;	// ip-uri care se pot conecta
	typedef IP_LIST BANNED_IP_LIST;		// ip-uri care nu se pot conecta


	// iterator pentru lista de IP in care se are incredere
	typedef std::set<DWORD>::iterator IP_LIST_ITERATOR;
	typedef IP_LIST_ITERATOR TRUSTED_IP_LIST_ITERATOR;
	typedef IP_LIST_ITERATOR BANNED_IP_LIST_ITERATOR;


	// mesaje trimise de server
	enum SERVER_MESSAGE
	{
		SM_CLIENT_CONNECTED,		// s-a conectat un nou client
		SM_CLIENT_DISCONNECTED,		// un client s-a deconectat sau a fost deconectat fortat

		SM_MCSC_ERROR,				// s-a incercat o a doua conectarea la un server cu mcsc true
		SM_TIP_ERROR,				// s-a incercat conectarea de la un IP in care nu se are incredere
		SM_BIP_ERROR,				// s-a incercat conectarea de la un IP care nu are void sa se conecteze

		SM_CLIENT_ERROR,			// eroare nedefinita la client
		SM_CLIENT_LIMIT_REACHED,	// s-a atins limita de clinti conectati simultan
		SM_NO_MEMORY_FOR_CLIENT,	// nu se poate aloca memorie pentru client
		SM_THREAD_ERROR,			// nu se poate creea thread-ul pentru client
		
		SM_CONNECTION_ERROR,		// eroare in functia de acceptare a conexiunii
		
		SM_CREATE,					// s-a creat daemonul pentru acceptare conexiuni
		SM_CLOSE					// urmeaza sa fie inchis 
	};
	
	// datele necesare pentru a creea un server
	class SERVER_CREATION_DATA
	{
	public:

		SERVER_CREATION_DATA()
		{
			server_procedure = NULL;		
			server_event_handler = NULL;
			port = 4572;					 
			max_client_number = 128;		
			mcsc = 1;								// setarea standard
			utip = 0;								// standard sunt acceptate conexiune de la orice IP
			ubip = 0;						
		}

		SERVER_PROCEDURE server_procedure;			// functie furnizata de programator care este apelata atunci cand se conecteaza un nou client
		SERVER_EVENT_HANDLER server_event_handler;	// orice mesaj de la server este trimis acestei functii

		UINT port;									// portul pe care il gestioneaza server-ul

		UINT max_client_number;						// numarul maxim de clienti conectati simultan
		UINT mcsc;									// conexiuni multiple de la acelasi IP

		UINT utip;									// trusted ip 
		UINT ubip;									// use banned ip list
	};

	// clasa pentru server - un server multiclient folosind o arhitectura multithreading simpla
	class CServer
	{
	public:
		CServer();
		~CServer();

		// creaza sau inchide serverul
		void create( SERVER_CREATION_DATA &server_data );
		void close();		
		
		// adauga un IP in lista - trebuie sa fie in forma impachetata
		void add_trusted_ip( DWORD ip ) { m_tip_list.insert(ip); }
		void remove_trusted_ip( DWORD ip ) { m_tip_list.erase(ip); }

		// adauga IP oprit la conectare
		void add_banned_ip( DWORD ip ) { m_bip_list.insert(ip); }
		void remove_banned_ip( DWORD ip ) { m_bip_list.erase(ip); }

		// adauga lista de ip-uri separate prin ;
		void add_tiplist( char * list );
		void add_bannedlist( char *list );

		// citeste ip
		const char *get_ip() { return m_listen_socket.get_ip(); }

		// returneaza lista de clienti
		CLIENT_LIST & get_client_list() { return m_client_list; }

	protected:
		CSocket m_listen_socket;		// socket used for listening
		HANDLE m_listen_thread;			// handle to port daemon
				
		CLIENT_LIST m_client_list;		// a list of connected clients
		TRUSTED_IP_LIST m_tip_list;		// lista de ip acceptate la conectare
		BANNED_IP_LIST m_bip_list;		// lista de ip care nu sunt acceptate pentru conectare

	};

};	// namespace GN

#endif // GARAJ_SERVER_H

// (c) GarajCode 2005 - programmed by Savu Andrei
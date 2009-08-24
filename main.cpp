/*
(c) GarajCode 2005 - port redirect software
It allows a connection in a sub network
or can be used for something else
Date : 10 noiembrie 2005
Last date : 23 noiembrie 2005
*/

// dezactiveaza avertizarile pentru STL
#pragma warning(disable:4786)
#pragma warning(disable:4786)

#include "gnet.h"		
#include "gini.h"
#include "glog.h"
#include "gutil.h"

#include <iostream.h>
#include <conio.h>

#define INI_SAFE_SET(a,value) if( !g_ini.is_set(a) ) g_ini.set( a, value )

GSTD::CIni g_ini;
GSTD::CLog g_log("redirect.log");

bool RedirectEH( UINT msg, GSTD::CError *error, GN::CSocket*sock);
void RedirectServer( GN::CLIENT_LIST&, GN::CSocket& );

int main( int argc, char **argv )
{
	// proceseaza fisierul de configurare
	try
	{
		g_ini.load("redirect.ini"); 
	}
	catch(...){ }

	// preia parametrii din linia de comanda daca sunt
	if( argc == 4 )
	{
		// redirect <source_port> <dest_host> <dest_port>
		g_ini.set("redirect_port", atoi( argv[1] ) );
		g_ini.set("destination_host", argv[2] );
		g_ini.set("destination_port", atoi( argv[3] ) );
	}

	// seteaza valori standard pentru variabilele nesetate
	if( !g_ini.is_set("destination_host") ||
		!g_ini.is_set("destination_port") ||
		!g_ini.is_set("redirect_port") ) 
	{
		cout << "Redirect configuration error..."<<endl;
		cout << "Command line usage : redirect <redirect_port> <dest_host> <dest_port>" << endl;
		cout << "You can also set configuration data in redirect.ini" << endl;
		g_log.fatal( "Redirect configuration error" );
		getch();
		return 0;
	}

	// set unset ini variables
	INI_SAFE_SET( "max_client_number", 128 );
	INI_SAFE_SET( "mcsc", 1 );
	INI_SAFE_SET( "utip", 0 );
	INI_SAFE_SET( "ubip", 0 );
	INI_SAFE_SET( "tiplist", "");
	INI_SAFE_SET( "biplist", "");

	// log redirect settings
	g_log.message( "destination host : %s", (char*)g_ini["destination_host"] );
	g_log.message( "destination port : %d", (int)g_ini["destination_port"] );
	g_log.message( "redirect port : %d", (int)g_ini["redirect_port"] );
	g_log.message( "max_client_number : %d", (int)g_ini["max_client_number"] );
	g_log.message( "multiple connections same client  : %d", (int)g_ini["mcsc"] );
	g_log.message( "use trusted ip : %d", (int)g_ini["utip"] );
	g_log.message( "trusted ip list : %s", (char*)g_ini["tiplist"] );
	g_log.message( "use banned ip : %d", (int)g_ini["ubip"] );
	g_log.message( "banned ip list : %s", (char*)g_ini["biplist"] );
	
	// creeaza server
	try
	{
		GN::CWinsock winsock;
		GN::CServer server;
		GN::SERVER_CREATION_DATA sd;
		GSTD::ConsoleCtrl ctrl;

		sd.max_client_number = (int)g_ini["max_client_number"];
		sd.mcsc = (int)g_ini["mcsc"];
		sd.port = (int)g_ini["redirect_port"];
		sd.server_event_handler = RedirectEH;
		sd.server_procedure = RedirectServer;
		sd.ubip = (int)g_ini["ubip"];
		sd.utip = (int)g_ini["utip"];

		// adauga listele de ip in care se are incredere
		// si cele a caror conectare este interzisa
		if( sd.utip ) server.add_tiplist( (char*)g_ini["tiplist"] );
		if( sd.ubip ) server.add_bannedlist( (char*)g_ini["biplist"]);

		cout << "Destination host : " << (char*)g_ini["destination_host"] << endl;
		cout << "Destination port : " << (int)g_ini["destination_port"] << endl;
		cout << "Local port : " << (int)g_ini["redirect_port"] << endl;
		cout << "Starting redirect server .... ";
		// activare server
		server.create( sd );
		cout << "OK" << endl;
	
		// astepta un mesaj de inchidere
		while( !ctrl.exit_request() ) Sleep(200); // pentru a evita incarcarea procesorului inutil
	}	
	catch( GSTD::CError error )
	{
		cout << "Error" << endl;
		g_log.fatal( error.get_text() );
		MessageBox( NULL, error.get_text(), "Network exception!", MB_OK | MB_ICONERROR );
	}

	return 0;
}

// Redirect Server Event Handler
bool RedirectEH( UINT msg, GSTD::CError *error, GN::CSocket*sock)
{
	switch( msg )
	{
	case GN::SM_CLIENT_CONNECTED:
		g_log.message( "client connected %s", sock->get_ip() );
		break;

	case GN::SM_CLIENT_DISCONNECTED:
		g_log.message( "client disconnected %s", sock->get_ip() );
		break;

	case GN::SM_MCSC_ERROR:
		g_log.error( "mcsc policy violation %s", sock->get_ip() );
		break;

	case GN::SM_TIP_ERROR:
		g_log.error( "attemted connection from untruster ip %s", sock->get_ip() );
		break;

	case GN::SM_BIP_ERROR:
		g_log.error( "attempted connection from banned ip %s", sock->get_ip() );
		break;

	case GN::SM_CLIENT_ERROR:
		g_log.error( "client error : %s ( %s )", error->get_text(), sock->get_ip() );
		break;

	case GN::SM_CLIENT_LIMIT_REACHED:
		g_log.error( "client limit reached, client connection rejected %s", sock->get_ip() );
		break;

	case GN::SM_NO_MEMORY_FOR_CLIENT:
		g_log.error( "client data memory alocation error %s", sock->get_ip() );
		break;

	case GN::SM_THREAD_ERROR:
		g_log.error( "client thread creation error %s", sock->get_ip() );
		break;

	case GN::SM_CREATE:
		g_log.message( "redirect daemon started" );
		break;

	case GN::SM_CLOSE:
		g_log.message( "redirect daemon close" );
	}
	return true;
}

struct RedirectSockInfo
{
	GN::CSocket *dest;
	GN::CSocket *src;
};

#define BUFFER_SIZE 512

// Firul de executie care se ocupa sa
// trimita datele primite de la sursa la destinatie
DWORD WINAPI SendThread( LPVOID p )
{
	// src -> dest
	RedirectSockInfo *d = (RedirectSockInfo*)p;
	char buffer[ BUFFER_SIZE ];
	int nb;
	try
	{
		while( true )
		{
			nb = d->src->receive( buffer, BUFFER_SIZE );		
			d->dest->send( buffer, nb );
		}
	}
	catch(...)
	{
		return 1;
	}
	return 0;
}

DWORD WINAPI ReceiveThread( LPVOID p )
{
	// dest -> src
	RedirectSockInfo *d = (RedirectSockInfo*)p;
	char buffer[ BUFFER_SIZE ];
	int nb;
	try
	{
		while( true )
		{
			nb = d->dest->receive( buffer, BUFFER_SIZE );		
			d->src->send( buffer, nb );
		}
	}
	catch(...)
	{
		return 1;
	}
	return 0;
}

void RedirectServer( GN::CLIENT_LIST&, GN::CSocket& client )
{
	try
	{
		// Conectare la calculatorul destinatie
		GN::CSocket dest;

		dest.connect( (int)g_ini["destination_port"], (char*)g_ini["destination_host"] );

		// fill structure for thread
		RedirectSockInfo r;
		r.dest = &dest;
		r.src = &client;

		// create two threads one for receiving and one for sending
		HANDLE h[2];

		// Create a thread for sending data and a thread for receiving data
		h[0] = CreateThread( NULL, 0, SendThread, (void*)&r, 0, NULL );
		h[1] = CreateThread( NULL, 0, ReceiveThread, (void*)&r, 0, NULL );

		if( !h[0] || !h[1] ) throw GSTD::CError( 0, "Thread creation error" );

		// Wait for threads to close
		WaitForMultipleObjects( 2, h, false, INFINITE );

		// close connections
		dest.close();
		client.close();

		// wait for threads to close
		WaitForMultipleObjects( 2, h, true, INFINITE );
	}
	catch( GSTD::CError error )
	{
		throw error;
	}
}


// (c) GarajCode 2005 - by Andrei Savu
/*
  (c) GarajCode
  Clasa pentru inregistrare mesaje in fisier si in
  debuger. Este thread-safe.
  Date : 21 iulie 2005
  Last date : 21 iulie 2005
*/

#ifndef GARAJ_LOG
#define GARAJ_LOG

#include "gerror.h"
#include "gmutex.h"

#include <fstream.h>
#include <windows.h>
#include <stdlib.h>

namespace GSTD
{

	class CLog
	{
	public:
		CLog( char * file = NULL )
		{
			m_has_file = false;
			if( file )
			{
				m_fout.open( file, ios::out | ios::app );
				m_has_file = true;
				putstring("");
			}
		}
		~CLog()
		{
			if( m_has_file )
			{
				m_fout.close();
			}
		}
		void message( const char *text,... )
		{
			char buffer[512];
			strcpy(buffer,"MESSAGE ");
			va_list arg;
			va_start(arg,text);
			vsprintf( buffer + strlen(buffer) , text,arg );
			va_end(arg);
			try
			{
				putstring( buffer );
			}
			catch( CError error )
			{
				throw error;
			}
		}
		void error( const char *text,...)
		{
			char buffer[512];
			strcpy(buffer,"ERROR ");
			va_list arg;
			va_start(arg,text);
			vsprintf( buffer + strlen(buffer) , text,arg );
			va_end(arg);
			try
			{
				putstring( buffer );
			}
			catch( CError error )
			{
				throw error;
			}

		}
		void fatal( const char *text, ...)
		{
			char buffer[512];
			strcpy(buffer,"FATAL ");
			va_list arg;
			va_start(arg,text);
			vsprintf( buffer + strlen(buffer) , text,arg );
			va_end(arg);
			try
			{
				putstring( buffer );
			}
			catch( CError error )
			{
				throw error;
			}

		}
		void important( const char *text, ... )
		{
			char buffer[512];
			strcpy(buffer,"IMPORTANT ");
			va_list arg;
			va_start(arg,text);
			vsprintf( buffer + strlen(buffer) , text,arg );
			va_end(arg);
			try
			{
				putstring( buffer );
			}
			catch( CError error )
			{
				throw error;
			}
		}

	protected:
		fstream m_fout;
		bool m_has_file;
		CMutex m_mutex;
		void putstring( const char * text )
		{
			// foloseste un mutex pentru acces exclusiv
			try
			{
				m_mutex.lock();
			}
			catch( CError error )
			{
				throw error;
			}

			// scrie mesaj in debuger si in fisier
			OutputDebugString( text );
			OutputDebugString( "\n" );
			if( m_has_file ) m_fout<<text<<endl;

			// elibereaza acces exclusiv
			try
			{
				m_mutex.unlock();
			}
			catch( CError error )
			{
				throw error;
			}
		}

	};	// CLog


};	// namespace GSTD

#endif	// GARAJ_LOG
/*
  (c) GarajCode
  Garaj error handle header
  Date : 4 iulie 2005
  Last date : 19 iulie 2005
*/

/*
	Toata libraria standard garaj foloseste
	acest header pentru a semnala errori.
  */

#ifndef GARAJ_ERROR_HANDLE_H
#define GARAJ_ERROR_HANDLE_H

#define MAX_ERROR_BUFFER 100

#include <string.h>

namespace GSTD
{

	enum STD_ERROR
	{
		BAD_ALLOC,	// eroare la alocare memorie
		BUFFER_UNDERFLOW,   // depasire limita inferioara buffer
		BUFFER_OVERFLOW		// depasire limita superioara buffer
	};		

	
	// orice eroare este impchetata intr-o astfel de
	// structura si aruncata
	class CError
	{
	public:
		// constructori
		CError()
		{
			m_code = 0;
			m_text[0] = 0;
		}
		CError(STD_ERROR code)
		{
			pack( code );
		}
		CError( int code, char *text)
		{
			pack( code, text );
		}

		// codul erorii
		int get_code()
		{
			return m_code;
		}

		// textul erorii
		char * get_text()
		{
			return m_text;
		}

		// seteaza cod
		void set_code( int code )
		{
			m_code = code;
		}

		// seteaza text
		void set_text( char *text )
		{
			int l = strlen(text);
			if( l > MAX_ERROR_BUFFER) l = MAX_ERROR_BUFFER;

			// avoid buffer overflow
			m_text[0] = 0;
			strncat( m_text, text, l);
		}
		
		// initializeaza structura cu datele transmise
		void pack( int code, char *text )
		{
			m_code = code;

			int l = strlen(text);
			if( l > MAX_ERROR_BUFFER) l = MAX_ERROR_BUFFER;

			// avoid buffer overflow
			m_text[0] = 0;
			strncat( m_text, text, l);
		}
		
		//	eroare standard
		void pack( STD_ERROR code )
		{
			m_code = code;
			switch( code )
			{
			case BAD_ALLOC:
				strcpy(m_text, "Memory allocation error");
				break;
			case BUFFER_UNDERFLOW:
				strcpy(m_text, "Buffer underflow");
				break;
			case BUFFER_OVERFLOW:
				strcpy(m_text, "Buffer overflow");
				break;
			default:
				strcpy(m_text, "Unknown error");
			}
		}

	protected:
		int m_code;
		char m_text[ MAX_ERROR_BUFFER+1 ];

	};	// CError
};

#endif

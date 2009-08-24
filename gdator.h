/*
  (c) GarajCode
  Structura generala pentru a retine informatii
  Date : 20 iulie 2005
  Last date : 20 iulie 2005
*/

#ifndef GARAJ_DATOR_H
#define GARAJ_DATOR_H

#include "gerror.h"

#include <stdlib.h>

#include <ctype.h>

namespace GSTD
{
	// tipuri de informatii retinute
	enum DATOR_TYPE
	{
		EMPTY_DATOR,
		INT_DATOR,
		FLOAT_DATOR,
		STRING_DATOR,
		BINARY_DATOR
	};
	
	// o clasa generala care imagazineaza informatii
	class CDator
	{
	public:
		// initializare
		CDator()
		{
			m_type = EMPTY_DATOR;
			m_size = 0;
			m_data = NULL;
		}
		// constructor de copiere
		CDator( const CDator & d )
		{
			m_type = EMPTY_DATOR;
			m_size = 0;
			m_data = NULL;
			try
			{
				set( d.m_size, d.m_data );
			}
			catch( GSTD::CError error )
			{
				throw error;
			}
			m_type = d.m_type;
		}
		// eliberare memorie
		~CDator()
		{
			release();
		}
		// elibereaza memorie
		void release()
		{
			if( m_type != EMPTY_DATOR )
			{
				delete[] m_data;
				m_data = NULL;
				m_type = EMPTY_DATOR;
				m_size = 0;
			}
		}
		// copiere
		CDator & operator = ( const CDator & d)
		{
			if( this == &d ) return *this;
			try
			{
				set( d.m_size, d.m_data );
			}
			catch( GSTD::CError error )
			{
				throw error;
			}
			m_type = d.m_type;
			return *this;
		}

		// memoreaza intreg
		void set(int value) 
		{
			try
			{
				set( sizeof(int), (char*)&value);
			}
			catch( GSTD::CError error )
			{
				throw error;
			}
			m_type = INT_DATOR;
		}
		// memoreaza float
		void set(float value) 
		{
			try
			{
				set( sizeof(float), (char*)&value);
			}
			catch( GSTD::CError error )
			{
				throw error;
			}
			m_type = FLOAT_DATOR;
		}
		// memoreaza sir de caractere terminat cu zero
		void set(char *string)
		{
			try
			{
				set( strlen(string)+1, string);
			}
			catch( GSTD::CError error )
			{
				throw error;
			}
			m_type = STRING_DATOR;
		}
		// seteaza pentru date binare - folositor la memorarea structurilor
		void set(int size, char *data)
		{
			if( size == m_size )
			{
				memcpy( m_data, data, size);
				m_type = BINARY_DATOR;
			}
			else
			{
				release();

				m_size = size;

				// aloca memorie
				if( !(m_data = new char[ m_size ]) )
				{
					GSTD::CError error( BAD_ALLOC );
					throw error;
				}

				memcpy( m_data, data, m_size );

				m_type = BINARY_DATOR;
			}
		}
	
		// converteste un string la tipul cel mai apropiat
		void convert( char *text )
		{
			// analizeaza un string si converteste-l corespunzator
			int type = is_number( text );

			switch( type )
			{
			case INT_DATOR:
				set(atoi(text));
				break;
			case FLOAT_DATOR:
				set((float)atof(text));
				break;
			case STRING_DATOR:
				set(text);
			}
		}

		// returneaza dimensiunea
		int get_size()
		{
			return m_size;
		}
		// returneaza tipul
		int get_type()
		{
			return m_type;
		}

		// converteste la integer
		operator int()
		{
			float aux;
			switch( m_type )
			{
			case INT_DATOR:
				return (*(int*)m_data);
			case FLOAT_DATOR:
				aux = (*(float*)m_data);
				return (int)aux;
			default:
				return 0;
			}
		}

		// converteste la float
		operator float()
		{
			switch( m_type )
			{
			case INT_DATOR:
				return (float)(*(int*)m_data);
			case FLOAT_DATOR:
				return (*(float*)m_data);
			default:
				return 0;
			}
		}

		// converteste la char *
		operator char*()
		{
			return m_data;
		}

	protected:
		int m_type;		// tipul informatiei
		int m_size;		// dimensiunea
		char *m_data;	// informatia

		int is_number( char * text )
		{
			int point = 0;
			int l = strlen(text);
			for(int i=0;i<l;i++)
			{
				if( !isdigit(text[i]) && text[i] != '.') 
				{
					return STRING_DATOR;
				}
				else
				{
					if( text[i] == '.' ) point++;
				}
			}
			if( !point ) return INT_DATOR;
			if( point == 1) return FLOAT_DATOR;
			return STRING_DATOR;
		}

	};	// class CDator

};	// namespace GSTD

#endif	// GARAJ_DATOR_H
/*
  (c) GarajCode
  Ini file preprocesor
  Date : 16 iulie 2005
  Last date : 17 august 2005
*/
// Change log:
//	17 august 2005 - marite bufferele pentru parsing

#ifndef GARAJ_INI_PREPROCESOR
#define GARAJ_INI_PRPRPOCESOR

#pragma warning(disable:4786)
#pragma warning(disable:4786)

#include "gdator.h"

#include <fstream.h>

#include <string>
#include <map>

#define MAX_LINE_LENGTH 2056
#define MAX_VAR_NAME_LENGTH 128
#define MAX_DATA_LENGTH ( MAX_LINE_LENGTH - MAX_VAR_NAME_LENGTH - 5 )

namespace GSTD
{
	// informatia este retinuta in perechi de forma string si informatie
	typedef std::pair<std::string, CDator> INI_DATA;

	// proceseaza un fisier ini
	class CIni
	{
	public:
		CIni() 
		{
		}

		CIni( const CIni & ini )
		{
			m_data = ini.m_data;
		}

		CIni & operator = ( const CIni & ini )
		{
			m_data = ini.m_data;
			return *this;
		}

		~CIni()
		{
			clear();
		}

		void load( char * file )
		{
			// elibereaza datele actuale
			clear();

			// deschide fisier
			fstream fin(file, ios::in);

			// verifica existenta
			if( !fin )
			{
				GSTD::CError error(0, "Unable to open ini file");
				throw error;				
			}

			// citeste linie
			char buffer[ MAX_LINE_LENGTH ];
			char var_name[ MAX_VAR_NAME_LENGTH ];
			char data[ MAX_DATA_LENGTH ];
			while( fin.getline( buffer, MAX_LINE_LENGTH ) )
			{
				// verifica daca avem comentariu
				if( is_comment(buffer) ) continue;

				// imparte linia in doua blocuri fata de =
				if( !split(buffer,var_name,data) ) continue;

				// adauga in lista noua variabila
				if( !m_data.count(var_name) )
				{
					INI_DATA new_data;

					new_data.first = var_name;
					new_data.second.convert(data);

					m_data.insert(new_data);
				}
				else
				{
					m_data[ var_name ].convert(data);
				}
			}
		}
		
		void clear()
		{
			m_data.clear();
		}

		void set( char* var_name, int value )
		{
			if( m_data.count( var_name ) )
			{
				m_data[ var_name ].set( value );
			}
			else
			{
				INI_DATA new_data;
				new_data.first = var_name;
				new_data.second.set( value );
				m_data.insert( new_data );
			}
		}

		void set( char* var_name, float value )
		{
			if( m_data.count( var_name ) )
			{
				m_data[ var_name ].set( value );
			}
			else
			{
				INI_DATA new_data;
				new_data.first = var_name;
				new_data.second.set( value );
				m_data.insert( new_data );
			}
		}

		void set( char* var_name, char *value )
		{
			if( m_data.count( var_name ) )
			{
				m_data[ var_name ].set( value );
			}
			else
			{
				INI_DATA new_data;
				new_data.first = var_name;
				new_data.second.set( value );
				m_data.insert( new_data );
			}
		}

		int is_set( char* var_name )
		{
			return m_data.count( var_name );
		}

		CDator & get( char *var_name )
		{
			if( is_set(var_name) ) return m_data[ var_name ];
			return m_empty_dator;
		}

		CDator & operator [] ( char *var )
		{
			return get(var);
		}

	protected:
		std::map< std::string, CDator > m_data;
		CDator m_empty_dator;

		bool is_comment( char *buffer )
		{
			int l = strlen(buffer);
			int i=0;
			// skip initial white spaces
			while( buffer[i] == ' ' || buffer[i] == '\t') i++;
			// check for comment
			if( buffer[i] == '#' ) return true;
			return false;
		}

		void skip_white_space( int &i, char *buffer, int l )
		{
			while( (buffer[i] == ' ' || buffer[i] == '\t') && i<l ) i++;
		}

		int split( char *buffer, char * var, char *data)
		{
			int k;
			int l = strlen(buffer);
			int i = 0;

			// sare peste spatiile initiale
			skip_white_space( i, buffer, l );

			// copiaza numele variabilei
			k = 0;
			while( buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '=' && i < l)
			{
				var[ k ] = buffer[i];
				k++;
				i++;
			}
			var[k] = 0;
			
			// daca avem un sir vid atunci avem o problema
			if( !k ) return 0;

			if( buffer[i] == '=' ) i++;

			// sari peste spatii albe
			skip_white_space( i, buffer, l);

			// vezi daca am ajuns la egal
			if( buffer[i] == '=' ) i++;

			// evita spatii albe
			skip_white_space( i, buffer, l);

			// verifica daca avem un string
			if( buffer[i] == '\"' )
			{
				// copiaza tot pana la urmatoarea ghilimea
				i++;
				k = 0;
				while( buffer[i] != '\"' && i < l)
				{
					data[k] = buffer[i];
					k++;
					i++;
				}
				data[k] = 0;
				return 1;
			}

			// copiaza pana la primul spatiu liber sau comentariu 
			k = 0;
			while( buffer[i] != ' ' && buffer[i] != '#' && buffer[i] != '\t' && i < l)
			{
				data[k] = buffer[i];
				k++;
				i++;
			}
			data[k] = 0;
			if( !k ) return 0;
			return 1;
		}

	};

};

#endif

// (c) GarajCode 2005 - programmed by Savu Andrei


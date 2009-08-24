/*
  (c) GarajCode
  Mutex class - used for thread-safe
  Date : 4 iulie 2005
  Last date : 16 iulie 2005
*/
/*
	Codul pentru mutex exista doar daca proiectul
	este setat pentru multithreading( definit _MT )
  */
#ifndef GARAJ_MUTEX_H
#define GARAJ_MUTEX_H

#include <windows.h>
#include "gerror.h"

namespace GSTD
{

	class CMutex
	{
	public:
		// creat the mutex object
		CMutex()
		{
#ifdef _MT
			m_mutex = CreateMutex(NULL, false, NULL);
			if( !m_mutex )
			{
				CError error( GetLastError(), "Mutex creation error");
				throw error;
			}
#endif
		}

		// release the mutex
		~CMutex()
		{
#ifdef _MT
			m_mutex = NULL;
#endif
		}

		// lock the mutex
		void lock() const
		{
#ifdef _MT
			if( WaitForSingleObject( m_mutex, INFINITE ) == WAIT_FAILED )
			{
				CError error( GetLastError(), "Error locking mutex");
				throw error;
			}			
#endif
		}

		// free mutex
		void unlock() const
		{
#ifdef _MT
			if( ReleaseMutex( m_mutex ) == 0 )
			{
				CError error( GetLastError(), "Error releasing mutex");
				throw error;
			}
#endif
		}
			
	protected:
#ifdef _MT
		HANDLE m_mutex;
#endif
	
	};	// CMutex

};	// namespace gstd

#endif	// GARAJ_MUTEX_H

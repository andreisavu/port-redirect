/*
(c) GarajCode 2005
General utility functions
Date : 23 noiembrie 2005
Last date : 23 noiembrie 2005
*/

#include "gutil.h"

namespace GSTD
{

// Utility class for handling console Ctrl events
bool ConsoleCtrl::m_exit = false;
int ConsoleCtrl::m_refc = 0;

ConsoleCtrl::ConsoleCtrl() 
{
	set();
}

ConsoleCtrl::~ConsoleCtrl()
{
	unset();
}

void ConsoleCtrl::set()
{
	if( !m_refc ) SetConsoleCtrlHandler( (PHANDLER_ROUTINE)CtrlHandler, true );
	m_refc++;
}

void ConsoleCtrl::unset()
{
	m_refc--;
	if( !m_refc ) SetConsoleCtrlHandler( (PHANDLER_ROUTINE)CtrlHandler, false );
}

bool CtrlHandler( DWORD type )
{
	switch( type )
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		ConsoleCtrl::m_exit = true;
		return true;
	}
	return false;
}

bool ConsoleCtrl::exit_request()
{
	return m_exit;
}


}; // namespace GSTD




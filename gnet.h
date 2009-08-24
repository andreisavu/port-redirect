/*
  (c) GarajCode 2005
  General include header for network programs
  Date : 1 iulie 2005
  Last date : 23 noiembrie 2005
*/

#ifndef GARAJ_NETWORK_H
#define GARAJ_NETWORK_H

/*

	Acesta este framework-ul pentru retea GarajCode. Are incluse
  clase care abstractizeaza socket-urile si permit realizarea
  foarte usoara a unui server multiclient cu o arhitectura
  multithreading. Pentru tratarea erorilor este folosit mecanismul
  exceptiilor. 

	Framework-ul se imparte in doua componente: interfata cu socket-urile
  care foloseste winsock si tcp/ip si clasa pentru server. Pentru crearea 
  unui server trebuie furnizata o functie pentru protocolul de comunicare
  cu clientul si o functie optionala pentru tratarea evenimentelor serverului.

	Proiectul este inca in dezvoltare. Inca mai are nevoie de flexibilitate.
  */

// initializare winsock
#include "gwinsock.h"

// clasa pentru socket
#include "gsocket.h"

// server multiclient - multithreading
#include "gserver.h"

#endif	// GARAJ_NETWORK_H

// (c) GarajCode 2005 - programmed by Savu Andrei
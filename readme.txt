
Port redirect tool (c) GarajCode 2005

By Savu Andrei

Un utilitar pentru redirectarea traficului de la un port catre
un alt calculator - extern sau din reteaua interna
inaccesibil altfel. Redirectarea este complet 
transparenta nefiind vizibila la nivelul clientului
si nici la nivelul serverului. 

Poate fi configurat numarul maxim de conexiuni, 
portul care este ascultat, portul destinatie si 
calculatorul destinatie. Permite si configurarea de 
lista de IP care au acces si de IP care nu au acces.

Poate fi apelat din linia de comanda astfel :
	redirect <redirect_port> <destination_host> <destination_port>

Daca lipsesc parametrii din linia de comanda acestia sunt preluati
din fisierul de configurare redirect.ini. Fisierul de configurare
este procesat oricum pentru a gasi listele de acces sau de ban.

Programul poate fi inchis prin CTRL_C, CTRL_BREAK. In cazul in care
sistemul de operare se opreste sau face logoff programul se va opri
singur.

Daca doriti sa porniti programul fara consola folositi comanda
"start /B redirect.exe" din linia de comanda


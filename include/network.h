/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/


/*
	Module de gestion du réseau et de la communication avec le backend.

	EN CONSTRUCTION

*/


#ifndef network_h
#define network_h


#ifndef commons_h
#include "commons.h"
#endif

#ifndef error_handler_h
#include "error_handler.h"
#endif


#ifndef JANSSON_H
#include "jansson/jansson.h"
#endif



typedef struct message_for_you {
   char text[8192];
   char from[64];
   char date[32];
   int message_id;
} message_for_you;




/*
	Lance une requête synchrone vers l'adresse spécifiée via "request" et retourne un buffer contenant le résultat 
	(à libéré manuellement). 
	
	Retourne NULL en cas d'échec.
*/

char * kal_net_sync_xml_http_request (char *request, char *buffer, int buff_size);



/*
	Requête (actuellement synchrone) vers la bdd. 

	NON IMPLEMENTE. Ne fait actuellement qu'émettre une requête synchrone vers une url spécifiée via "request".
*/

message_for_you * kal_net_get_message_from_db (char *request);




#endif
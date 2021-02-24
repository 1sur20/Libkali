/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/


/*
	Module de gestion des erreurs / warnings / affichage des backtraces / etc.

	EN CONSTRUCTION
*/



#ifndef error_handler_h
#define error_handler_h


#ifndef commons_h
#include "commons.h"
#endif



#define kal_error_exit(err_str) kal_error_fatal(err_str, __FILE__, __LINE__)

#define kal_error_warn(err_str) kal_error_warn_user(err_str, __FILE__, __LINE__)

#define kal_log_info(message) kal_error_log_user_message (message)



/*
	Termine le programme et log une erreur dans la console du nav ainsi que quelques informations sur celle-ci
*/

void kal_error_fatal (const char *error_str, const char *filename, int line);

/*
	Termine le programme et log un warning dans la console du nav ainsi que quelques informations sur celle-ci
*/

void kal_error_warn_user (const char *error_str, const char *filename, int line);


/*
	Envoie un message destiné à l'utilisateur via la console du nav
*/

void kal_error_log_user_message (const char *message);






/*
	Défini un message d'erreur en cas de problème lors de l'appel à une fonction de libkali 
	(actuellement uniquement utilisé par le module audio)
*/

void kal_set_non_fatal_error_warn (const char * err_message); 


/*
	Renvoie le dernier message d'erreur en cas d'échec lors de l'appel d'une fonction de libkali.
	
	Devrait-être appelée systématiquement à la suite d'une erreur.
	(actuellement uniquement utilisé par le module audio)
*/

char * kal_get_non_fatal_error_warn ();





#endif

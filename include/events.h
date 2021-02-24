/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

/*
   User's input / interractions module 

   Module de gestion des entrées utilisateurs (clavier, touchscreen, souris, etc).

	Les fonctions de celui-ci prendront toujours en paramètres un objet représentant une scène (kal_scene *), 
	celui-ci contenant une référence vers l'affichage courant.
*/


#ifndef events_h
#define events_h

#ifndef commons_h
#include "commons.h"
#endif

#ifndef scenes_h
#include "scenes.h"
#endif




/*
	Retrouve les entrées utilisateurs depuis le dernier appel à la fonction et appelle à son tour les fonctions de 
	rapppel défini par l'utilisateur du framework / en fonction des paramètres passées lors de l'initialisation de la scène.

	Il est recommandé d'appeler cette fonction au moins une fois par ittération de la main loop.

	Cette fonction ne peut pas crash (fatal).
*/

void kal_events_read_sdl_queue (kal_scene ** scene, int *, int *);





#endif

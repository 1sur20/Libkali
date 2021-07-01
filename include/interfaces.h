/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/


/*
   Module de gestion des "interfaces" (console, fake UI, clavier virtuel, speech bubble, etc)

   EN CONSTRUCTION
*/


#ifndef interfaces_h
#define interfaces_h

#ifndef commons_h
#include "commons.h"
#endif



#ifndef error_handler_h
#include "error_handler.h"
#endif



#ifndef display_h
#include "display.h"
#endif


/*
   Valeur plus ou moins arbitraire, à revoir
*/

#define TERM_H_OFFSET 214
#define TERM_V_OFFSET 108
#define TERM_WIDTH 531
#define TERM_HEIGHT 328

#define TERM_MAX_LINE 20
#define TERM_MAX_COL 66

#define DISP_QUEUE_MAX 128



typedef struct kal_scene kal_scene;



#define kal_interfaces_console_up(scene) kal_interfaces_console_updown(scene, 0)
#define kal_interfaces_console_down(scene) kal_interfaces_console_updown(scene, 1)



/*
   Structure représentant une queue/buffer circulaire de DISP_QUEUE_MAX (128) chaines de TERM_MAX_COL (66 chars)
   multiplié par 4 pour l'encodage utf8 sur 4 bytes.

   -data : tableau de pointeur sur les entrées (chaines) de la queue
   -curr_end : index de la dernière entrée du tableau (dernière chaine ajoutée)
   -curr_pos : index de l'entrée affichée actuellement (position du scrolling)
*/

typedef struct kal_console_queue {
   char data[DISP_QUEUE_MAX][TERM_MAX_COL * 8];
   int curr_end;
   int curr_pos;
   int left_align;
} kal_console_queue;


/*
   Structure représentant une "console" (terminal).
   
   -queue : struct kal_console_queue contenant les données à afficher sur la console
   -charmap : SDL_Texture des sprites utilisés pour l'affichage des lettres
   -width : largeur de la console
   -height : hauteur de la console
   -x : position de la console
   -y : position de la console
   -active: actuellement affiché si 1, sinon 0
*/

typedef struct kal_console_text {
   struct kal_console_queue queue;
   SDL_Texture *charmap;
   int width;
   int height;
   int x;
   int y;
   int active;
} kal_console_text;



/*
   Actuellement non implémenté. Supposé représenter les bulles de texte.
*/

typedef struct kal_speech_bubble {
   SDL_Texture *charmap;
   int width;
   int height;
   int x;
   int y;
} kal_speech_bubble;



/*
   Créer un nouvel objet de type kal_console_text représentant une console (terminal).
*/

kal_console_text * kal_create_console_interface (kal_scene * scene, char * charmap);


/*
   Ajoute une ligne à la console
*/

void kal_interfaces_add_text_to_console_queue (kal_scene * scene, char * text);


/*
   Scrolling de la console (change la position actuelle à afficher)
*/

void kal_interfaces_console_updown(kal_scene * scene, int mode);







#endif
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




typedef struct kal_console_queue {
   char data[DISP_QUEUE_MAX][TERM_MAX_COL + 1];
   int curr_end;
   int curr_pos;
} kal_console_queue;



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





kal_console_text * kal_create_console_interface (kal_scene * scene, char * charmap);

void kal_interfaces_add_text_to_console_queue (kal_scene * scene, char * text);

void kal_interfaces_console_updown(kal_scene * scene, int mode);







#endif
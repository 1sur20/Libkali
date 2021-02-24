/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#ifndef libkali_h
#define libkali_h


#ifndef commons_h
#include "commons.h"
#endif


#ifndef error_handler_h
#include "error_handler.h"
#endif


#ifndef display_h
#include "display.h"
#endif


#ifndef events_h
#include "events.h"
#endif

#ifndef network_h
#include "network.h"
#endif

#ifndef scenes_h
#include "scenes.h"
#endif


#ifndef JANSSON_H
#include "jansson/jansson.h"
#endif




typedef struct kal_em_loop_ud {
   kal_display *display;
   kal_scene *scene;
   int req_restart;
   int cont;
} kal_em_loop_ud;




#endif

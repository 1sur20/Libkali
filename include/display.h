/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/


/*
   Screen display module (in charge of drawing things on screen) 

   Module de gestion de l'affichage utilisé par l'ensemble du programme pour dessiner sur l'écran.

   Après création de l'objet représentant l'affichage à l'aide de la fonction kal_init_display(), 
   celui-ci est passé en paramètre lors des appels de fonctions pour la création d'autres objets 
   (qui utiliseront l'interface et les propriétés de l'affichage).

   Ex: la création d'une nouvelle scène nécessitera en paramètre une structure représentant l'affichage utilisé. 



   960x540

Notre cas :

Si 16:9
   Fullscreen: 
      Max background res : 1920x1080 (x6)
      Default background res : 960x540 (x3)
      Min background res : 640x360 (x2)
      Default files res : 320x180
*/



#ifndef display_h
#define display_h


#ifndef commons_h
#include "commons.h"
#endif

#ifndef audio_h
#include "audio.h"
#endif


#ifndef error_handler_h
#include "error_handler.h"
#endif

#ifndef interfaces_h
#include "interfaces.h"
#endif

#ifndef JANSSON_H
#include "jansson/jansson.h"
#endif




#define SCREEN_H 960
#define SCREEN_V 540

#define KAL_SECRET "\x43\x2b\x6a\x68\x6b\x63\x30\x74\x7f\x63\x60\x6c\x3a\x37\x51\x39\x68"\
                   "\x7e\x7d\x71\x72\x66\x00\x4c\x4b\x50\x57\x05\x5f\x48\x5d\x09\x67\x4a"\
                   "\x44\x4c\x5b\x5b\x1d\x5d\x5d\x46\x14\x78\x53\x45\x54\x40\x1a\x01\x14"
#define KAL_SEC_START 102019135




/*
   --------- Struct definitions -----------
*/


typedef struct kal_sounds_linked_list kal_sounds_linked_list;
typedef struct kal_console_text kal_console_text;



typedef struct kal_aspect_ratio {
	int x;
	int y;
	int w;
	int h;
} kal_aspect_ratio;


/*
   Structure présentant diverses information concernant l'affichage.

   Membres :
      - monitor_width : largeur réelle de l'écran/moniteur du matériel de l'utilisateur
      - monitor_height : hauteur réelle de l'écran/moniteur du matériel de l'utilisateur
      - window_width : largeur actuelle de la fenêtre représenté sur l'écran 
         (peut être plus grand que la taille réelle de l'écran)
      - window_height : hauteur actuelle de la fenêtre représenté sur l'écran 
         (peut être plus grand que la taille réelle de l'écran)
      - default_width : largeur par défaut demandée par l'application
      - default_height : hauteur par défaut demandée par l'application
      - refresh_rate : taux de rafraichissement de l'écran
      - is_fullscreen : 1 si la fenêtre est en plein écran, sinon 0
      - fullscreen_disabled : 0 si le plein écran est autorisé, sinon 1
      - background_w : largeur actuelle de l'image d'arrière plan 
         (peut être différent de la fenêtre si l'écran n'est pas en 16:9)
      - background_h : hauteur actuelle de l'image d'arrière plan 
         (peut être différent de la fenêtre si l'écran n'est pas en 16:9)
      - background_xdiff : différence en X nécessaire pour centrer l'arrière plan au centre de la fenêtre
      - background_ydiff : différence en Y nécessaire pour centrer l'arrière plan au centre de la fenêtre
      - ratio_w : ratio en X pour l'affichage des objets/hitboxes en fonction de différents types d'écran
      - ratio_h : ratio en H pour l'affichage des objets/hitboxes en fonction de différents types d'écran
      - is_touchscreen : 1 si l'appareil possède un touchscreen, sinon 0
      - mouse_is_down : 1 si la souris (ou touchscreen) est actuellement appuyé, sinon 0
      - brightness : luminosité de l'affichage, entre 0 et 1
      - real_fps : vrai framerate (refresh rate valant 0 ...)
      - last_refresh_time : temps écoulé depuis le dernier appel à la fonction de rafraichissement
*/

typedef struct kal_display_infos {
   int monitor_width;
   int monitor_height;
   int window_width;
   int window_height;
   int default_width;
   int default_height;
   int refresh_rate;
   int is_fullscreen;
   int fullscreen_disabled;
   int background_w;
   int background_h;
   int background_xdiff;
   int background_ydiff;
   double ratio_w;
   double ratio_h;
   int is_touchscreen;
   int mouse_is_down;
   float brightness;
   int real_fps;
   double last_refresh_time;
} kal_display_infos;


/*
   Structure représentant l'affichage / moniteur.

   Membres :
      - window : ptr vers un objet SDL représentant la fenêtre associée à cet affichage
      - renderer : ptr vers un objet SDL représentant un "renderer" 
         (objet générique permettant d'afficher des images/textes/.. sur une fenêtre)
      - sounds_list : une liste chainée contenant la bande son de l'application
      - infos : structure contenant les informations concernant cet affichage
*/

typedef struct kal_display {
   SDL_Window *window;
   SDL_Renderer *renderer;
   kal_sounds_linked_list *sounds_list;
   kal_display_infos infos;
} kal_display;





#ifndef scenes_h
#include "scenes.h"
#endif




/*
   Initialise l'affichage et retourne une structure opaque correspondante (kal_display *).

   Cette fonction ne peut pas crash (fatal).
*/

kal_display * kal_init_display (int width, int height, char *appname);





/*
   Passe l'affichage en plein écran.

   Cette fonction peut ne pas fonctionner sous certaines conditions.
   Ex: smatphone ou tablette en mode portrait.
*/

void kal_display_switch_fullscreen_mode (kal_display * disp);




/*
   Affiche le background de la scene passé en paramètre (kal_scene *, voir scenes.h:kal_create_new_scene). 

   La structure représentant une scène contient un lien vers l'affichage.

   Cette fonction ne peut pas crash (fatal).
*/

void kal_display_render_background (kal_scene * scene);




/*
   Affiche une console par dessus la scene aux coordonnées absolues spécifiées.

   Cette fonction peut ne pas fonctionner si la scene en paramètre n'a actuellement pas de console 
   initialisée (voir interfaces.h:kal_create_console_interface).
*/

void kal_display_render_console_text(kal_scene * scene, int x, int y, int w, int h);





/*
   Affiche les différents sprites (animés) passé en paramètre via la structure kal_scene.

   Cette fonction ne peut pas crash (fatal).
*/

void kal_display_render_scene_sprites (kal_scene * scene, double time);




/*
	Met à jour le sprite du curseur de la souris en fonction de celui défini actuellement à la position de la souris 
   en fonction de la scène passé en paramètre et de ses attributs (cursor_events).
*/

void kal_display_set_curr_cursor (kal_scene *scene);





/*
   Fonction de debug.

   Affiche différents paramètres liés à l'affichage.
*/

void kal_display_debug_get_screen_infos (kal_display * disp);




/*
   Met à jour l'affichage, à appeler dans la main loop à interval régulier / après chaque changement.

   Cette fonction ne peut pas crash (fatal).
*/

void kal_display_refresh (kal_scene * scene);




/*
   Libère la mémoire allouée pour l'affichage. Cette fonction devrait être appelée en dernier, après avoir libéré 
   la mémoire occupée par la scène liée à l'affichage.

   Ne pas suivre cette recommandation peut (ou non) fonctionner (non recommandé). 

   Cette fonction ne peut pas crash (fatal).
*/

void kal_display_free (kal_display * display);






/*
   Retourne une texture à partir d'un fichier.

	I don't know why it's a public function, but I think that because function is
	(dirty) called from interfaces module (interfaces.c).
*/

SDL_Texture * kal_display_load_texture_from_file (kal_scene * scene, char * path);






#endif

/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/


/*
   Module de gestion des scènes (background, chemins vers les différents fichiers de sprites associés, etc).

   Après avoir initialisé l'affichage, l'appelle de la fonction kal_create_new_scene retournera une structure 
   représentant une scène du point'n click qui sera utilisée tout le long du fonctionnement du programme pour 
   mettre à jour l'affichage, évaluer les actions de l'utilisateur, etc.

   La structure représentant une scène (kal_scene *) doit être libérée *avant* l'appel de la fonction pour libérer 
   l'affichage et terminer le programme.

*/


#ifndef scenes_h
#define scenes_h



#ifndef commons_h
#include "commons.h"
#endif


#ifndef display_h
#include "display.h"
#endif


#ifndef error_handler_h
#include "error_handler.h"
#endif




#define KAL_MAX_SCENE_SPRITES 64
#define KAL_MAX_SCENE_HITBOXES 64

#define KAL_HITBOX_STR_MAX 128
#define KAL_SCENE_NAME_MAX 64



#define KAL_CONSOLE 1

#define KAL_BUBBLE 2




/*
   Représente un sprite dans une scène (voir struct kal_scene).
   Descriptions des membres :
      -src_select : structure représentant un rectangle utilisée pour
                    sélectionner le sprite adéquat (x, y, w, h).
      -dest_select : structure représentant un rectangle utilisée pour
                     sélectionner la destination sur le renderer/canvas (x, y, w, h)
      -start_frame_idx : la frame de départ du sprite (à partir de 0)
      -curr_frame_idx : frame actuellement affichée
      -end_frame_idx : dernière frame du sprite
      -loop_frame_on_idx : index de la frame sur laquelle reboucler (si différent de 0)
      -active_hover : 1 si actif au survol du curseur, 0 si actif perpetuellement
      -active : non utilisé ?
      -frame_delay : délai en milisecondes entre l'affichage de chaque frame du sprite
      -frame_time_elapsed : délai écoulé depuis le dernier changement de frame du sprite
      -change_on_click : 1 si changement de sprite lors d'un click, 0 sinon
*/


typedef struct kal_sprite {
   SDL_Rect src_select;
   SDL_Rect dest_select;
   int start_frame_idx;
   int curr_frame_idx;
   int end_frame_idx;
   int click_frame_idx;
   int loop_on_frame_idx;
   int active_hover;
   int active;
   int frame_delay;
   int frame_time_elapsed;
   int change_on_click;
} kal_sprite;




/*
   A renommer ? Pas vraiment une hitbox..
   Représente une zone déclenchant un événement qui sera passé en paramètre à la fonction de rappel
   défini par l'utilisateur (kal_events_callback).
   Description des membres :
      -dest_select : zone déclenchant l'action (x, z, w, h)
      -action_name : nom d'action défini par l'utlisateur pour la fonction de rappel
      -action_arg : argument supplémentaire (optionel) défini par l'utilsateur pour la fonction de rappel
      -button : nom du "boutton" défini par l'utilisateur et pour la fonction de rappel
*/

typedef struct kal_hibtox {
   SDL_Rect dest_select;
   char action_name[KAL_HITBOX_STR_MAX];
   char action_arg[KAL_HITBOX_STR_MAX];
   char button[KAL_HITBOX_STR_MAX];
   int is_ev_up;
} kal_hitbox;



/*
   Représente une zone de changement de sprite pour le curseur.
   Membres :
      -src_rect (x, y, w, h) : curseur sélectionné dans le spritesheet
      -x : coordonnées déclenchant le changement de sprite (x)
      -y : // 
      -w : // 
      -h : //
*/

typedef struct kal_cursor_ev {
   SDL_Rect src_rect;
   int x;
   int y;
   int w;
   int h;
} kal_cursor_ev;


/*
   Représente le curseur actuellement utilisé ainsi que la liste d'événements associés.
   Membres :
      -assets : SDL_surface contenant le spritesheet
      -cursor_surf : SDL_surface contenant le curseur sélectionné
      -cursor : curseur actuellement sélectionné
      -cursor_select : coordonnées du curseur actuellement utilisé sur le spritesheet
      -cursor_default : coordonnées du curseur par défaut
      -sprite_path : path vers le fichier de sprites
      -events : kal_cursor_ev représentant les différents évnénements déclenchant le changement de curseur
      -tot_events : nombre total d'événements
*/

typedef struct kal_cursor {
   SDL_Surface *assets;
   SDL_Surface *cursor_surf;
   SDL_Cursor *cursor;
   SDL_Rect cursor_select;
   SDL_Rect cursor_default;
   char sprite_path[KAL_HITBOX_STR_MAX];
   kal_cursor_ev events[KAL_MAX_SCENE_HITBOXES];
   int tot_events;
} kal_cursor;






typedef struct kal_scene kal_scene;
typedef struct kal_speech_bubble kal_speech_bubble;


/*
   Représente une scène complète.
   Membres :
      -background : vers arrière plan utilisé
      -assets : spritesheet actuellement utilisé
      -display : ptr vers l'affichage actuel (crée/initialisé via la fonction kal_init_display)
      -cursor : information sur le curseur
      -sprites : liste et infos des sprites appartenant à cette scène
      -tot_sprites : nombre total de sprites
      -hitboxes : liste et infos des "hitboxes" (événements)
      -tot_hibtboxes : nombre total de "hitbox" défini
      -text_console : ptr vers une console affichant du texte
      -speech_buble : ptr vers une structure représentant des bulles d'infos (non implémenté)
      -keypress_callback : fonction de rappel défini par l'utilisateur pour les touches clavier
      -touchscreen_callback : fonction de rappel défini par l'utilisateur pour les écrans tactiles 
         (à vérif, en parti remplacé par hitbox_event_callback)
      -hitbox_event_callback : fonction de rappel défini par l'utilisateur pour les zones déclenchant une action
      -mouseclick_callback : ?????? it's 2:42am and I haven't slept ... probably not implemented/an artifact 
      -flags : différents drapeaux définissants des options (CONSOLE, BUBBLE, etc)
      -name : nom de la scène défini par l'utilisateur
      -id : id unique de la scène
*/

typedef struct kal_scene {
   SDL_Texture *background;
   SDL_Texture *assets;
   kal_display *display;
   kal_cursor cursor;
   kal_sprite sprites[KAL_MAX_SCENE_SPRITES];
   int tot_sprites;
   kal_hitbox hitboxes[KAL_MAX_SCENE_HITBOXES];
   int tot_hitboxes;
   kal_console_text *text_console;
   kal_speech_bubble *speech_bubble;
   void (*keypress_callback)(kal_scene **pscene, SDL_Keycode keycode, void *ud);
   void (*touchscreen_callback)(kal_scene **pscene, Uint32 ev_type, SDL_TouchFingerEvent *finger_ev, void *ud);
   void (*hitbox_event_callback)(kal_scene **pscene, kal_hitbox *hitbox, void *userdata);
   void (*mouseclick_callback)(void);
   int flags;
   char name[KAL_SCENE_NAME_MAX];
   int id;
} kal_scene;







/*
   Crée une nouvelle scène et retourne la structure correspondante.

   Prend en paramètre un pointeur vers une structure représentant l'affichage préalablement initialisé. Le fichier 
   de description json de la scène, les trois fonctions de rappels (optionnelles) et une pointeur vers 
   une scène précédente pour désalouer sa mémoire automatiquement (ou NULL).

   La structure doit être détruite *avant* de l'affichage et l'arrêt du programme via un appel à kal_scene_free. 

   Cette fonction ne retourne pas d'erreur. 
*/

kal_scene * kal_create_new_scene (kal_display * display, char * json_file_path, \
   void (*keypress_callback_func)(kal_scene **pscene, SDL_Keycode keycode, void *userdata), \
   void (*touchscreen_callback_func)(kal_scene **pscene, Uint32 ev_type, SDL_TouchFingerEvent *finger_ev, void *userdata), \
   void (*hitbox_event_callback_func)(kal_scene **pscene, kal_hitbox *hitbox, void *userdata), \
   kal_scene *old_scene);



/*
   Libère la mémoire allouée à la scène passée en paramètre.
*/

void kal_scene_free (kal_scene * scene);




#endif
/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "libkali.h"


const char gl_title[] = { 
	" \n"
	"                "
	"          "
	"< LIBKALI >\n"
	"                "
	"        "
	"  -----------\n"
	" \n"
	" \n"
};


const char gl_intro[] = { 
	"Welcome,\n\n" 
	"Tu t'es perdu ?\n"
	"Libkali est un framework en développement pour la création d'un \"jeu\" en hommage a une amie qui nous a quittée le 29 juillet 2020\n\n\n"
	"Appuyer sur la touche ENTER pour continuer.\n\n"
	"Liste des touches (debug) :\n"
	"    - Gauche : piste précédente\n"
	"    - Droite : piste suivante\n"
	"    - Haut : défilement console\n"
	"    - Bas : défilement console\n"
	"    - Espace : infos affichage (debug)\n"
	"    - P : pause audio\n"
	"    - F12 : plein écran\n"
	"    - G : test requête réseau (debug)\n"
	"    - D : test buffering console (debug)"
	"	  - M : menu\n"
	"    - Enter : commencer\n"
	"\n\n"
};


kal_em_loop_ud gl_kal_em_loop_ud;







void kal_touchscreen_callback (kal_scene **pscene,  uint32_t ev_type, SDL_TouchFingerEvent *finger_ev, void *ud);

void kal_hitbox_event_callback (kal_scene **pscene, kal_hitbox *hitbox, void *ud);



int tmp_i = 0;
char tmp_buffer[64];

void kal_keypress_callback (kal_scene **pscene, SDL_Keycode keycode, void *ud) {
   kal_scene *scene = *pscene;
   kal_display *disp = scene->display;
   SDL_Window *window = scene->display->window;
   message_for_you *m;

	kal_sounds_linked_list *p;


   switch (keycode) {
      case SDLK_UP:
			if (scene->flags == KAL_CONSOLE || scene->flags == (KAL_CONSOLE|KAL_BUBBLE))
				kal_interfaces_console_up(scene);

         break;
      case SDLK_DOWN:
			if (scene->flags == KAL_CONSOLE || scene->flags == (KAL_CONSOLE|KAL_BUBBLE))
				kal_interfaces_console_down(scene);

         break;
		case SDLK_LEFT:
         p = kal_audio_play_next_sound_fadeout(scene->display->sounds_list);
         if (p)
            scene->display->sounds_list = p;
         break;
		case SDLK_RIGHT:
         p = kal_audio_play_prev_sound_fadeout(scene->display->sounds_list);
         if (p)
            scene->display->sounds_list = p;			
			break;
		case SDLK_F12:
			kal_display_switch_fullscreen_mode(disp);
			break;
      case SDLK_g:
			m = kal_net_get_message_from_db("http://192.168.0.33/test.txt");
			if (m && (scene->flags == KAL_CONSOLE || scene->flags == (KAL_CONSOLE|KAL_BUBBLE)))
         	kal_interfaces_add_text_to_console_queue(scene, m->text);
			if (m)
         	free(m);
         break;
		case SDLK_p:
			kal_audio_pause_current_sound();
			break;
      case SDLK_RETURN:
			if (scene->id == 1)
				*pscene = kal_create_new_scene(disp, "data/json/scene2.json",\
					kal_keypress_callback, kal_touchscreen_callback, kal_hitbox_event_callback, scene);
         break;
      case SDLK_SPACE:
			kal_display_debug_get_screen_infos(scene->display); // DEBUG !!!
         break;
		case SDLK_m:
			*pscene = kal_create_new_scene(disp, "data/json/scene1.json",\
				kal_keypress_callback, kal_touchscreen_callback, kal_hitbox_event_callback, scene);
			scene = *pscene;
			if (scene->flags == KAL_CONSOLE || scene->flags == (KAL_CONSOLE|KAL_BUBBLE)) {
				kal_interfaces_add_text_to_console_queue(scene, (char *)gl_title);
				kal_interfaces_add_text_to_console_queue(scene, (char *)gl_intro);
			}
			break;
		case SDLK_d:
         snprintf(tmp_buffer, 64, "Console test, line  #%d", tmp_i);
			if (scene->flags == KAL_CONSOLE || scene->flags == (KAL_CONSOLE|KAL_BUBBLE))
         	kal_interfaces_add_text_to_console_queue(scene, tmp_buffer);
         (tmp_i >= 65535) ? (tmp_i = 0) : (tmp_i++);
			break;
   }

}



void kal_touchscreen_callback (kal_scene **pscene,  uint32_t ev_type, SDL_TouchFingerEvent *finger_ev, void *ud) {
  	kal_scene *scene = *pscene;
   SDL_Window *window = scene->display->window;


   switch (ev_type) {
      case SDL_FINGERMOTION:
         //printf("[DEBUG] FINGERMOTION x:%f   y:%f  dx:%f  dy:%f\n", finger_ev->x, finger_ev->y, 
            //finger_ev->dx, finger_ev->dy);
         if (finger_ev->x >= 0.20 && finger_ev->x <= 0.80\
         && finger_ev->y >= 0.15  && finger_ev->y <= 0.80) {
            //printf("[DEBUG] FINGERMOTION x:%f   y:%f  dx:%f  dy:%f\n", finger_ev->x, finger_ev->y, 
               //finger_ev->dx, finger_ev->dy);
            if (finger_ev->dy < 0) {
					if (scene->flags == KAL_CONSOLE || scene->flags == (KAL_CONSOLE|KAL_BUBBLE))
						kal_interfaces_console_down(scene);
            } else if (finger_ev->dy > 0) {
					if (scene->flags == KAL_CONSOLE || scene->flags == (KAL_CONSOLE|KAL_BUBBLE))
						kal_interfaces_console_up(scene);
            }
         }
         break;
   }

}



void kal_hitbox_event_callback (kal_scene **pscene, kal_hitbox *hitbox, void *ud) {
	kal_scene *scene = (kal_scene *)*pscene;
	kal_display *disp = scene->display;

	if (strncmp(hitbox->action_name, "fullscreen", 10) == 0) {
		//printf("[DEBUG] In hitbox event callback / fullscreen\n");
		kal_display_switch_fullscreen_mode(scene->display);
	} else if (strncmp(hitbox->action_name, "new-scene", 9) == 0) {
		//printf("[DEBUG] In hitbox event callback / new-scene : %s\n", hitbox->action_arg);


		*pscene = kal_create_new_scene(disp, hitbox->action_arg,\
					kal_keypress_callback, kal_touchscreen_callback, kal_hitbox_event_callback, scene);
	}

}








EM_BOOL main_loop (double time, void *ud) {
	kal_em_loop_ud *pud;

	pud = (kal_em_loop_ud *)ud;


	kal_events_read_sdl_queue(&pud->scene, &pud->cont, &pud->req_restart);


	kal_display_render_background(pud->scene);


	kal_display_render_console_text(pud->scene, TERM_H_OFFSET, TERM_V_OFFSET, 8, 16);



	kal_display_render_scene_sprites(pud->scene, time);

	pud->display->sounds_list = kal_audio_update_sounds_loop(pud->display->sounds_list);


	kal_display_refresh(pud->scene);



	if (!pud->cont) {
		kal_scene_free(pud->scene);
		kal_display_free(pud->display);

      if (pud->req_restart)
         printf("Req restart not implemented\n");

      return EM_FALSE;
   }

   return EM_TRUE;

}






int main (int argc, char **argv) {
	kal_display *disp = kal_init_display(SCREEN_H, SCREEN_V, "kal-web");
	kal_sounds_linked_list *p;


	kal_scene *scene = kal_create_new_scene(disp, "data/json/scene1.json",\
		kal_keypress_callback, kal_touchscreen_callback, kal_hitbox_event_callback, NULL);
	


	kal_interfaces_add_text_to_console_queue(scene, (char *)gl_title);
	kal_interfaces_add_text_to_console_queue(scene,  (char *)gl_intro);

	gl_kal_em_loop_ud.cont = 1;
	gl_kal_em_loop_ud.req_restart = 0;
	gl_kal_em_loop_ud.display = disp;
	gl_kal_em_loop_ud.scene = scene;


	p = kal_audio_add_sound_to_list(disp->sounds_list, "pixel_beats-tired.ogg");
	p = kal_audio_add_sound_to_list(disp->sounds_list, "pixel_beats-silent_slow_night.ogg");
	p = kal_audio_add_sound_to_list(disp->sounds_list, "pixel_beats-lost_in_thought.ogg");

	p = kal_audio_remove_sound_from_list(disp->sounds_list, "pixel_beats-tired.ogg");
	if (p)
		disp->sounds_list = p;

	kal_audio_play_current_sound(disp->sounds_list);


   emscripten_request_animation_frame_loop(&main_loop, (void *)&gl_kal_em_loop_ud);

   emscripten_exit_with_live_runtime();

	return 0;
}

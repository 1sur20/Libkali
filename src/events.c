/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "events.h"


static void kal_events_scale_aspect_ratio (kal_display_infos *disp_infos, kal_aspect_ratio *r) {
	r->x = (int)(r->x * disp_infos->ratio_w) + disp_infos->background_xdiff;
	r->y = (int)(r->y * disp_infos->ratio_h) + disp_infos->background_ydiff;
	r->w = (int)(r->w * disp_infos->ratio_w);
	r->h = (int)(r->h * disp_infos->ratio_h);
}



/*
   Verify if touchscreen or mouse event correspond to the coordinate of an hitbox.
   If so, return it.
*/

static kal_hitbox * kal_events_hitbox_check (kal_scene **pscene, SDL_Event *e) {
   SDL_TouchFingerEvent *finger_ev = (SDL_TouchFingerEvent *)e;
   SDL_MouseButtonEvent *mouse_ev = (SDL_MouseButtonEvent *)e;
   kal_aspect_ratio r_hit, r_target = {'\x00'};
   kal_scene *scene = *pscene;
   SDL_Rect *rect;
   int i;

   switch (e->type) {
      case SDL_MOUSEBUTTONDOWN:
         // Disable MOUSEBUTTONDOWN event if we have detected a touchscreen compatible device
         if (scene->display->infos.is_touchscreen)
            return NULL;

         /*
            Mouse events are in absolute coordinate and automaticaly adjusted by SDL to the size of the window or monitor 
            (window and screen are the same in fullscreen)
         */
         r_hit.x = mouse_ev->x;
         r_hit.y = mouse_ev->y;
         break;
      case SDL_FINGERDOWN:
         // If we trigger a FINGERDOWN event, we are on a device with a touchscreen. It's the only reliable way to detect it.
         scene->display->infos.is_touchscreen = 1;

         // Set fingerdown event to absolute coordinate using the window's size (same as monitor in fullscreen)
         r_hit.x = (int)(finger_ev->x * scene->display->infos.window_width);
         r_hit.y = (int)(finger_ev->y * scene->display->infos.window_height);
         break;
   }

  /*
      All hitboxes are given in absolute coordinate and correspond to the default application size.
      So, adjust hit to the current real size of background and center it with kal_events_scale_aspect_ratio

      If we found a hitbox that match user mouse/touchscreen event, return it.
      Else, return NULL.
  */

   for (i = 0; i < scene->tot_hitboxes; i++) {
      if (!scene->hitboxes[i].is_ev_up) {
         rect = &scene->hitboxes[i].dest_select;
         r_target.x = rect->x;
         r_target.y = rect->y;
         r_target.w = rect->w;
         r_target.h = rect->h;
         kal_events_scale_aspect_ratio(&scene->display->infos, &r_target);

         if (r_hit.x >= r_target.x && r_hit.x <= (r_target.x + r_target.w) \
         && r_hit.y >= r_target.y && r_hit.y <= (r_target.y + r_target.h)) {
            return &scene->hitboxes[i];
         } 
      }
   }

   return NULL;
}




/*
   Verify if touchscreen or mouse event correspond to the coordinate of an hitbox.
   If so, return it.
*/

static kal_hitbox * kal_events_hitbox_check_up (kal_scene **pscene, SDL_Event *e) {
   SDL_TouchFingerEvent *finger_ev = (SDL_TouchFingerEvent *)e;
   SDL_MouseButtonEvent *mouse_ev = (SDL_MouseButtonEvent *)e;
   kal_aspect_ratio r_hit, r_target = {'\x00'};
   kal_scene *scene = *pscene;
   SDL_Rect *rect;
   int i;

   switch (e->type) {
      case SDL_MOUSEBUTTONUP:
         // Disable MOUSEBUTTONDOWN event if we have detected a touchscreen compatible device
         if (scene->display->infos.is_touchscreen)
            return NULL;

         /*
            Mouse events are in absolute coordinate and automaticaly adjusted by SDL to the size of the window or monitor 
            (window and screen are the same in fullscreen)
         */
         r_hit.x = mouse_ev->x;
         r_hit.y = mouse_ev->y;
         break;
      case SDL_FINGERUP:
         // If we trigger a FINGERDOWN event, we are on a device with a touchscreen. It's the only reliable way to detect it.
         scene->display->infos.is_touchscreen = 1;

         // Set fingerdown event to absolute coordinate using the window's size (same as monitor in fullscreen)
         r_hit.x = (int)(finger_ev->x * scene->display->infos.window_width);
         r_hit.y = (int)(finger_ev->y * scene->display->infos.window_height);
         break;
   }

  /*
      All hitboxes are given in absolute coordinate and correspond to the default application size.
      So, adjust hit to the current real size of background and center it with kal_events_scale_aspect_ratio

      If we found a hitbox that match user mouse/touchscreen event, return it.
      Else, return NULL.
  */

   for (i = 0; i < scene->tot_hitboxes; i++) {
      if (scene->hitboxes[i].is_ev_up) {
         rect = &scene->hitboxes[i].dest_select;
         r_target.x = rect->x;
         r_target.y = rect->y;
         r_target.w = rect->w;
         r_target.h = rect->h;
         kal_events_scale_aspect_ratio(&scene->display->infos, &r_target);

         if (r_hit.x >= r_target.x && r_hit.x <= (r_target.x + r_target.w) \
         && r_hit.y >= r_target.y && r_hit.y <= (r_target.y + r_target.h)) {
            return &scene->hitboxes[i];
         } 
      }
   }

   return NULL;
}






/*
   Read SDL events queue (mouse, touchscreen, keyboard, etc) and call appropriate functions / user's callbacks
*/

void kal_events_read_sdl_queue (kal_scene **pscene, int *cont, int *req_restart) {
   kal_hitbox *hitbox;
   SDL_Event e;

   while (SDL_PollEvent(&e)) {
      if (e.type == SDL_KEYDOWN) {
         if (((kal_scene*)*pscene)->keypress_callback)
            ((kal_scene*)*pscene)->keypress_callback(pscene, e.key.keysym.sym, NULL);
         //kal_event_get_keyboard_input(pscene, cont, req_restart, e.key.keysym.sym);
      } else if (e.type == SDL_MOUSEBUTTONUP || e.type == SDL_FINGERUP) {
         //If mouse button up (button was down, mr de la palisse), verify if user clicked on a defined hitbox

         ((kal_scene *)*pscene)->display->infos.mouse_is_down = 0;

         hitbox = kal_events_hitbox_check_up(pscene, &e);
         if (hitbox && ((kal_scene*)*pscene)->hitbox_event_callback)
               ((kal_scene*)*pscene)->hitbox_event_callback(pscene, hitbox, NULL);
         

      } else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_FINGERDOWN) {
         //If mouse button down, verify if user clicked on a defined hitbox

         ((kal_scene *)*pscene)->display->infos.mouse_is_down = 1;

         hitbox = kal_events_hitbox_check(pscene, &e);
         if (hitbox && ((kal_scene*)*pscene)->hitbox_event_callback)
               ((kal_scene*)*pscene)->hitbox_event_callback(pscene, hitbox, NULL);


      } else if (e.type == SDL_FINGERMOTION) {
         //kal_event_get_touch_screen_input(pscene, cont, req_restart, e.type, &e.tfinger);
         if (((kal_scene *)*pscene)->touchscreen_callback)
            ((kal_scene *)*pscene)->touchscreen_callback(pscene, e.type, &e.tfinger, NULL);
      }
   }

}


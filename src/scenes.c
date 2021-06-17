/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "scenes.h"




/*
	Read json file, self explanatory if you write C code..
*/

static char * kal_scene_read_json_file (char *jpath) {
   int fd; int bytes_read;
   char *res, *buffer; 

   fd = open(jpath, O_RDONLY);
   if (fd == -1)
		kal_error_exit(strerror(errno));

   res = malloc(16384*sizeof(char *));
   if (!res)
      kal_error_exit(strerror(errno));
   memset(res, 0, 16384);

   buffer = res;
   while ((bytes_read = read(fd, buffer, 4096)) != 0) {
      if (bytes_read == -1) {
         if (errno != EINTR)
            kal_error_exit(strerror(errno));
         continue;
      }
      if (bytes_read + (buffer - res) + 4096 >= 16384) {
         close(fd);
         kal_error_exit("file too big"); //should be non fatal ! (change me WTF)
      }
      buffer += bytes_read;
   }
   close(fd);

   return res;
}





/*
	Free allocated buffer for json scene file
*/


static void kal_scene_free_json_string (char *json_str) {
   free(json_str);
}




/*
	Parse default scene attributes : background, sprites_path, flags, scene_name and scene_id
*/

static void kal_scene_parse_default_scene_description (kal_scene *scene, json_t *root) {
	json_t *background, *sprites_path, *flags, *scene_name, *scene_id;



   // Parse json scene description and retrieve scene infos

	background = json_object_get(root, "background");
   if (!json_is_string(background)) {
      json_decref(root);
		kal_error_exit("invalid json type for background");
   }
	scene->background = kal_display_load_texture_from_file(scene, (char *)json_string_value(background));


   sprites_path = json_object_get(root, "sprites_path");
   if (!json_is_string(sprites_path)) {
      json_decref(root);
		kal_error_exit("invalid json type for sprites path");
   }
	scene->assets = kal_display_load_texture_from_file(scene, (char *)json_string_value(sprites_path));

	
	scene_name = json_object_get(root, "name");
	if (!json_is_string(scene_name)) {
		json_decref(root);
		kal_error_exit("invalid json type for scene name");
	}
	strncpy(scene->name, json_string_value(scene_name), KAL_SCENE_NAME_MAX-1);
	scene->name[KAL_SCENE_NAME_MAX-1] = '\x00';


	scene_id = json_object_get(root, "id");
	if (!json_is_integer(scene_id)) {
		json_decref(root);
		kal_error_exit("invalid json type for scene id");
	}
	scene->id = json_integer_value(scene_id);

}




/*
	Parse scene charmap infos and flags (if charmap/assets exist)
*/

static void kal_scene_parse_scene_charmap (kal_scene *scene, json_t *root) {
	json_t *charmap, *flags, *console_attr;


	// Optional, charmap imply flag and, reciprocaly, flag imply charmap 

   charmap = json_object_get(root, "charmap");
	flags = json_object_get(root, "flags");
	console_attr = json_object_get(root, "console_attr");
	if (charmap && flags) {
		if (!json_is_string(charmap) || !json_is_integer(flags)) {
      	json_decref(root);
			kal_error_exit("invalid json type for charmap or flags");
   	}
		scene->flags = json_integer_value(flags);
		switch (scene->flags) {
			case KAL_CONSOLE:
				scene->text_console = kal_create_console_interface(scene, (char *)json_string_value(charmap));
				scene->speech_bubble = NULL;
				scene->text_console->active = 1;
				if (console_attr && json_is_integer(console_attr))
					scene->text_console->queue.left_align = json_integer_value(console_attr);
				break;
			case KAL_BUBBLE:
				scene->speech_bubble = NULL;
				scene->text_console = NULL;
				break;
			case (KAL_CONSOLE|KAL_BUBBLE):
				scene->text_console = kal_create_console_interface(scene, (char *)json_string_value(charmap));
				scene->speech_bubble = NULL;
				scene->text_console->active = 1;
				if (console_attr && json_is_integer(console_attr))
					scene->text_console->queue.left_align = json_integer_value(console_attr);
				break;
		}
	}
}



/*
	Parse scene sprites array
*/

static void kal_scene_parse_scene_sprites (kal_scene *scene, json_t *root) {
	char *sprite_infos_keys[] = {
   	"sel_x", "sel_y", "sel_w", "sel_h", "start_frame", "curr_frame",
      "end_frame", "loop_frame", "frame_delay", "x", "y",
      "w", "h", "active_hover", "active", "click_frame" 
	};
   json_t *sprites, *sprite, *sprite_infos;
	int *sprite_infos_value[16];
	int i, j;


	// Parse each sprite data (position, frame delay, etc)

   sprites = json_object_get(root, "sprites");
   if (!json_is_array(sprites)) {
      json_decref(root);
		kal_error_exit("invalid json type for sprites array");
   }

	(json_array_size(sprites) <= KAL_MAX_SCENE_SPRITES) ?\
		(scene->tot_sprites = json_array_size(sprites)) : (scene->tot_sprites = KAL_MAX_SCENE_SPRITES);

   for (i = 0; i < scene->tot_sprites; i++) {
      sprite = json_array_get(sprites, i);
      if (!json_is_object(sprite)) {
         json_decref(root);
			kal_error_exit("invalid json array (sprite) entry");
      }

		sprite_infos_value[0] = &scene->sprites[i].src_select.x;
		sprite_infos_value[1] = &scene->sprites[i].src_select.y;
		sprite_infos_value[2] = &scene->sprites[i].src_select.w;
		sprite_infos_value[3] = &scene->sprites[i].src_select.h;
		sprite_infos_value[4] = &scene->sprites[i].start_frame_idx;
		sprite_infos_value[5] = &scene->sprites[i].curr_frame_idx;
		sprite_infos_value[6] = &scene->sprites[i].end_frame_idx;
		sprite_infos_value[7] = &scene->sprites[i].loop_on_frame_idx;
		sprite_infos_value[8] = &scene->sprites[i].frame_delay;
		sprite_infos_value[9] = &scene->sprites[i].dest_select.x;
		sprite_infos_value[10] = &scene->sprites[i].dest_select.y;
		sprite_infos_value[11] = &scene->sprites[i].dest_select.w;
		sprite_infos_value[12] = &scene->sprites[i].dest_select.h;
		sprite_infos_value[13] = &scene->sprites[i].active_hover;
		sprite_infos_value[14] = &scene->sprites[i].active;
		sprite_infos_value[15] = &scene->sprites[i].click_frame_idx; //optional


      for (j = 0; j < 16; j++) {
         sprite_infos = json_object_get(sprite, sprite_infos_keys[j]);
         if (!json_is_integer(sprite_infos)) {
				if (j == 15) {
					//click_frame_idx is optional, so it's not an eror : continue
					//it will be presents only if user request a specific sprite frame on click
					*sprite_infos_value[j] = -1;
					continue;
				}
            json_decref(root);
				kal_error_exit("invalid json type for sprite attribute (not an integer)");
         } 
			*sprite_infos_value[j] = json_integer_value(sprite_infos);
      }
		//set frame_time_elasped for each sprite to default value of requested delay beteween frame 
		//and set change_on_click value
      scene->sprites[i].frame_time_elapsed = scene->sprites[i].frame_delay;
		(scene->sprites[i].click_frame_idx == -1) ? \
			(scene->sprites[i].change_on_click = 0) : (scene->sprites[i].change_on_click = 1);
   }
}




/*
	Parse scene hitboxes array
*/

static void kal_scene_parse_scene_hitboxes (kal_scene *scene, json_t *root) {
	char *hitbox_infos_keys[] = {
		"x", "y", "w", "h", "type", "action_name", "action_arg", "button"
	};
	json_t *hitboxes, *hitbox, *hitbox_infos;
	int *hitbox_infos_value[7];
	int i, j;
	

	//Parse each hitbox data (position, actions, etc)

	hitboxes = json_object_get(root, "hitboxes");
	if (!json_is_array(hitboxes)) {
		json_decref(root);
		kal_error_exit("invalid json type for hiboxes array");
	}

	(json_array_size(hitboxes) <= KAL_MAX_SCENE_HITBOXES) ?\
		(scene->tot_hitboxes = json_array_size(hitboxes)) : (scene->tot_hitboxes = KAL_MAX_SCENE_HITBOXES);

	for (i = 0; i < scene->tot_hitboxes; i++) {

		hitbox = json_array_get(hitboxes, i);
		if (!json_is_object(hitbox)) {
			json_decref(root);
			kal_error_exit("invalid json array (hitbox) entry");
		}

		hitbox_infos_value[0] = &scene->hitboxes[i].dest_select.x;
		hitbox_infos_value[1] = &scene->hitboxes[i].dest_select.y;
		hitbox_infos_value[2] = &scene->hitboxes[i].dest_select.w;
		hitbox_infos_value[3] = &scene->hitboxes[i].dest_select.h;
		hitbox_infos_value[4] = &scene->hitboxes[i].is_ev_up;

		for (j = 0; j < 7; j++) {
			hitbox_infos = json_object_get(hitbox, hitbox_infos_keys[j]);
			if (json_is_integer(hitbox_infos) && j <= 4) {
				//array entry is an integer, just use ptr to assign value to kal_scene struct
				*hitbox_infos_value[j] = json_integer_value(hitbox_infos);
			} else if (json_is_string(hitbox_infos) && j >= 5) {
				switch (j) {
					//array entry is a string : 5 is user defined action name
					//									 6 is user defined action arg, but optional
					//								    7 is user defined button name
					// None of this name should be used here as a key or identifier for some actions
					case 5:
						strncpy(scene->hitboxes[i].action_name, json_string_value(hitbox_infos), KAL_HITBOX_STR_MAX-1);
						scene->hitboxes[i].action_name[KAL_HITBOX_STR_MAX-1] = '\x00';

						break;
					case 6:
						strncpy(scene->hitboxes[i].action_arg, json_string_value(hitbox_infos), KAL_HITBOX_STR_MAX-1);
						scene->hitboxes[i].action_arg[KAL_HITBOX_STR_MAX-1] = '\x00';

						break;
					case 7:
						strncpy(scene->hitboxes[i].button, json_string_value(hitbox_infos), KAL_HITBOX_STR_MAX-1);
						scene->hitboxes[i].button[KAL_HITBOX_STR_MAX-1] = '\x00';

						break;
				}
			} else if (j == 6) {
				//j == 6 and array entry for action arg is not defined, set it to a single null byte
				scene->hitboxes[i].action_arg[0] = '\x00';

			} else {
				//error
				json_decref(root);
				kal_error_exit("invalid json type for hitbox attribute");
			}
		}
	}
}




/*
	Parse scene cursor events (array)
*/

static void kal_scene_parse_cursor_ev_array (kal_scene *scene, json_t *root, json_t *cursor_ev_array) {
	char *cursor_ev_infos_keys[] = {
		"sel_x", "sel_y", "sel_w", "sel_h", "x", "y", "w", "h"
	};
	json_t *cursor_ev, *cursor_ev_data;
	int *cursor_ev_infos_value[8];
	int i, j;
	
	
	(json_array_size(cursor_ev_array) <= KAL_MAX_SCENE_HITBOXES) ?\
		(scene->cursor.tot_events = json_array_size(cursor_ev_array)) : (scene->cursor.tot_events = KAL_MAX_SCENE_HITBOXES);

	for (i = 0; i < scene->cursor.tot_events; i++) {
		cursor_ev = json_array_get(cursor_ev_array, i);
		if (!json_is_object(cursor_ev)) {
			json_decref(root);
			kal_error_exit("invalid json for cursor ev array entry");
		}

		cursor_ev_infos_value[0] = &scene->cursor.events[i].src_rect.x;
		cursor_ev_infos_value[1] = &scene->cursor.events[i].src_rect.y;
		cursor_ev_infos_value[2] = &scene->cursor.events[i].src_rect.w;
		cursor_ev_infos_value[3] = &scene->cursor.events[i].src_rect.h;
		cursor_ev_infos_value[4] = &scene->cursor.events[i].x;
		cursor_ev_infos_value[5] = &scene->cursor.events[i].y;
		cursor_ev_infos_value[6] = &scene->cursor.events[i].w;
		cursor_ev_infos_value[7] = &scene->cursor.events[i].h;

		for (j = 0; j < 8; j++) {
			cursor_ev_data = json_object_get(cursor_ev, cursor_ev_infos_keys[j]);
			if (!json_is_integer(cursor_ev_data)) {
				json_decref(root);
				kal_error_exit("invalid json type for cursor ev data");
			}

			*cursor_ev_infos_value[j] = json_integer_value(cursor_ev_data);
		}
	}
}




/*
	Parse scene cursor data and cursor events (array)
*/

static void kal_scene_parse_scene_cursor_data (kal_scene *scene, json_t *root) {
	char *cursor_ev_infos_keys[] = {
		"sel_x", "sel_y", "sel_w", "sel_h", "x", "y", "w", "h"
	};
	json_t *cursor, *cursor_sprites_path, *cursor_default_infos, 
			 *cursor_ev_array, *cursor_ev, *cursor_ev_data;
	int *cursor_ev_infos_value[8];
	int i, j;


	// Parse event cursor data 

	cursor = json_object_get(root, "cursor");
	if (!json_is_object(cursor)) {
		json_decref(root);
		kal_error_exit("invalid json type for cursor");
	}

	cursor_sprites_path = json_object_get(cursor, "sprites_path");
	if (!json_is_string(cursor_sprites_path)) {
		json_decref(root);
		kal_error_exit("invalid json type for cursor sprites path");
	}
	strncpy(scene->cursor.sprite_path, json_string_value(cursor_sprites_path), KAL_HITBOX_STR_MAX-1);
	scene->cursor.sprite_path[KAL_HITBOX_STR_MAX-1] = '\x00';

	//select default cursor sprite 
	cursor_ev_infos_value[0] = &scene->cursor.cursor_default.x;
	cursor_ev_infos_value[1] = &scene->cursor.cursor_default.y;
	cursor_ev_infos_value[2] = &scene->cursor.cursor_default.w;
	cursor_ev_infos_value[3] = &scene->cursor.cursor_default.h;  

	for (i = 0; i < 4; i++) {
		cursor_default_infos = json_object_get(cursor, cursor_ev_infos_keys[i]);
		if (!json_is_integer(cursor_default_infos)) {
			json_decref(root);
			kal_error_exit("invalid json type for cursor default infos");
		}
		*cursor_ev_infos_value[i] = json_integer_value(cursor_default_infos);
	}
	//set default value to current cursor
	scene->cursor.cursor_select.x = scene->cursor.cursor_default.x;
	scene->cursor.cursor_select.y = scene->cursor.cursor_default.y;
	scene->cursor.cursor_select.w = scene->cursor.cursor_default.w;
	scene->cursor.cursor_select.h = scene->cursor.cursor_default.h;




	//parse array of events (coordinates) for other cursor, if presents (optional)

	cursor_ev_array = json_object_get(cursor, "cursor_events");
	if (json_is_array(cursor_ev_array)) {
		kal_scene_parse_cursor_ev_array(scene, root, cursor_ev_array);
	} else {
		scene->cursor.tot_events = 0;
		json_decref(root);
		return;
	}

	return;
}






/*
	Parse json file and copy current scene informations in kal_scene struct
*/

static void kal_scene_parse_json_string (kal_scene *scene, char *json_str) {
   json_t *root; json_error_t error;


   // Open json string and check if it's valid json, return NULL in case of error or json struct (root)   
   
	root = json_loads(json_str, 0, &error);
   if (!root)
      kal_error_exit(error.text);

   if (!json_is_object(root)) {
      json_decref(root);
		kal_error_exit("invalid json type");
   }


	kal_scene_parse_default_scene_description(scene, root);

	kal_scene_parse_scene_charmap(scene, root);

	kal_scene_parse_scene_sprites(scene, root);

	kal_scene_parse_scene_hitboxes(scene, root);

	kal_scene_parse_scene_cursor_data(scene, root);


   json_decref(root);
}




/*
	Load SDL_Surface * from png (cursor sprites)
*/

static void kal_scene_load_cursor_surface (kal_scene *scene) {
	if (scene->cursor.assets) {
		SDL_FreeSurface(scene->cursor.assets);
		scene->cursor.assets = NULL;
	}

	scene->cursor.assets = IMG_Load(scene->cursor.sprite_path);
	if (!scene->cursor.assets)
		kal_error_exit(SDL_GetError());

}






void kal_scene_free (kal_scene *scene) {
	switch (scene->flags) {
		case KAL_CONSOLE:
			SDL_DestroyTexture(scene->text_console->charmap);
			scene->text_console->charmap = NULL;

			free(scene->text_console);
			scene->text_console = NULL;
			break;
		case KAL_BUBBLE:
			scene->speech_bubble = NULL;
			break;
		case (KAL_CONSOLE|KAL_BUBBLE):
			SDL_DestroyTexture(scene->text_console->charmap);
			scene->text_console->charmap = NULL;

			free(scene->text_console);
			scene->speech_bubble = NULL;
			scene->text_console = NULL;
			break;
	}


	SDL_DestroyTexture(scene->assets);
	SDL_DestroyTexture(scene->background);

	if (scene->cursor.assets) {
		SDL_FreeSurface(scene->cursor.assets);
		scene->cursor.assets = NULL;
	}
	if (scene->cursor.cursor_surf) {
		SDL_FreeSurface(scene->cursor.cursor_surf);
		scene->cursor.cursor_surf = NULL;
	}
	if (scene->cursor.cursor) {
		SDL_FreeCursor(scene->cursor.cursor);
		scene->cursor.cursor = NULL;
	}

	free(scene);
}



/*
	Create a new scene and attach current display to it.

*/

kal_scene * kal_create_new_scene (kal_display *display, char *json_file_path,\
void (*keypress_callback_func)(kal_scene **pscene, SDL_Keycode keycode, void *userdata), \
void (*touchscreen_callback_func)(kal_scene **pscene, Uint32 ev_type, SDL_TouchFingerEvent *finger_ev, void *userdata), \
void (*hitbox_event_callback_func)(kal_scene **pscene, kal_hitbox *hitbox, void *userdata), \
kal_scene *old_scene) {
	kal_scene *scene;
	char *json_str;

	scene = (kal_scene *)malloc(sizeof(kal_scene));
	if (!scene)
		kal_error_exit(strerror(errno));
	memset(scene, 0, sizeof(kal_scene));


	scene->display = display;
	scene->cursor.assets = NULL;
	scene->cursor.cursor_surf = NULL;
	scene->cursor.cursor = NULL;

	//set default brigthness to 0
	display->infos.brightness = 0.F;
	

	//read json file and parse it to retrieve scene's description (sprites, events, hitbox, etc)
	json_str = kal_scene_read_json_file(json_file_path);
	kal_scene_parse_json_string(scene, json_str);
	kal_scene_free_json_string(json_str);


	// load surface from image for cursor and set new default cursor
	kal_scene_load_cursor_surface(scene);
	kal_display_set_curr_cursor(scene);


	//if old scene is not NULL, remove it
	if (old_scene)
		kal_scene_free(old_scene);


	//register user function callbacks
	(keypress_callback_func) ?
		(scene->keypress_callback = keypress_callback_func) : (scene->keypress_callback = NULL);
	(touchscreen_callback_func) ?
		(scene->touchscreen_callback = touchscreen_callback_func) : (scene->touchscreen_callback = NULL);
	(hitbox_event_callback_func) ?
		(scene->hitbox_event_callback = hitbox_event_callback_func) : (scene->hitbox_event_callback = NULL);


	return scene;
}



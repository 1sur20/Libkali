/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "display.h"



static void kal_display_scale_aspect_ratio (kal_display_infos *disp_infos, kal_aspect_ratio *r) {
	r->x = (int)(r->x * disp_infos->ratio_w) + disp_infos->background_xdiff;
	r->y = (int)(r->y * disp_infos->ratio_h) + disp_infos->background_ydiff;
	r->w = (int)(r->w * disp_infos->ratio_w);
	r->h = (int)(r->h * disp_infos->ratio_h);
}




static void kal_display_init_disp_infos (kal_display *disp) {
	SDL_Window *window = disp->window;
	SDL_DisplayMode dm;
	int tmp_width;

	SDL_GetWindowSize(window, &disp->infos.window_width, &disp->infos.window_height);
	SDL_GetCurrentDisplayMode(0, &dm);

	/*
		Because it's hard to say if hardware (smartphone/pad) will really be in 
		landscape primary orientation mode or portrary primary orientation mode 
		or another orientation mode by default (we can't rely on monitor width or height), 
		then we assume it's always in landscape mode and we exchange the values if width 
		is lower than height (portrary orientation mode). Fullscreen while in portrary 
		orientation should also be disabled and/or handled elsewhere.
	*/
	
	if (dm.w >= dm.h) {
		disp->infos.monitor_width = dm.w;
		disp->infos.monitor_height = dm.h;
	} else {
		disp->infos.monitor_width = dm.h;
		disp->infos.monitor_height = dm.w;
	}

	/*
		Same here : windows width and height are unreliable to determine 
		orientation mode (depends on hardware). 
		For the sake of simplicity, we exchange the values.
	*/

	if (disp->infos.window_width < disp->infos.window_height) {
		tmp_width = disp->infos.window_width;
		disp->infos.window_width = disp->infos.window_height;
		disp->infos.window_height = tmp_width;
	}

	disp->infos.refresh_rate = dm.refresh_rate;

	disp->infos.default_width = SCREEN_H;
	disp->infos.default_height = SCREEN_V;
	disp->infos.is_fullscreen = 0;

	disp->infos.ratio_w = 1;
	disp->infos.ratio_h = 1;
	disp->infos.background_w = disp->infos.window_width;
	disp->infos.background_xdiff = 0;
	disp->infos.background_h = disp->infos.window_height;
	disp->infos.background_ydiff = 0;

	disp->infos.is_touchscreen = 0;
}




static void kal_display_update_disp_infos (kal_display *disp) {
	SDL_Window *window = disp->window;
	SDL_DisplayMode dm;
	int tmp_width;

	SDL_GetWindowSize(window, &disp->infos.window_width, &disp->infos.window_height);
	SDL_GetCurrentDisplayMode(0, &dm);

	/*
		Because it's hard to say if hardware (smartphone/pad) will really be in 
		landscape primary orientation mode or portrary primary orientation mode 
		or another orientation mode by default (we can't rely on monitor width or height), 
		then we assume it's always in landscape mode and we exchange the values if width 
		is lower than height (portrary orientation mode). Fullscreen while in portrary 
		orientation should also be disabled and/or handled elsewhere.
	*/
	
	if (dm.w >= dm.h) {
		disp->infos.monitor_width = dm.w;
		disp->infos.monitor_height = dm.h;
	} else {
		disp->infos.monitor_width = dm.h;
		disp->infos.monitor_height = dm.w;
	}

	/*
		Same here : windows width and height are unreliable to determine 
		orientation mode (depends on hardware). 
		For the sake of simplicity, we exchange the values.
	*/

	if (disp->infos.window_width < disp->infos.window_height) {
		tmp_width = disp->infos.window_width;
		disp->infos.window_width = disp->infos.window_height;
		disp->infos.window_height = tmp_width;
	}

	/*
		If monitor (screen size) is a multiple of 320 x 180, which correspond to 
		the minimimum screen size supported (16/9 ratio), then we don't have to
		adjust background, sprites (with ratio_w/h), etc sizes. Same if the application 
		is not in fullscreen : the browser will upscale/downscale the canvas automaticaly
		without breaking aspect ratio.

		If monitor is not a multiple of 320 x 180 or not in windowed mode, then 
		adjust background, sprites (with ratio_w/h), etc sizes to the size to nearest lower multiple of
		320 x 180. xdiff and ydiff are used to center the window.
	*/

	if (!(disp->infos.monitor_width % 320 || disp->infos.monitor_height % 180) \
	|| !disp->infos.is_fullscreen) {
		disp->infos.ratio_w = (double)disp->infos.window_width / (double)disp->infos.default_width;
		disp->infos.ratio_h = (double)disp->infos.window_height / (double)disp->infos.default_height;

		disp->infos.background_w = disp->infos.window_width;
		disp->infos.background_xdiff = 0;
		disp->infos.background_h = disp->infos.window_height;
		disp->infos.background_ydiff = 0;
	} else {
		disp->infos.background_w = ((int)(disp->infos.monitor_width / 320)) * 320;
		disp->infos.background_xdiff = (int)((disp->infos.monitor_width - disp->infos.background_w) / 2);
		disp->infos.background_h = ((int)(disp->infos.monitor_height / 180)) * 180;
		disp->infos.background_ydiff = (int)((disp->infos.monitor_height - disp->infos.background_h) / 2);

		disp->infos.ratio_w = (double)disp->infos.background_w / (double)(disp->infos.default_width);
		disp->infos.ratio_h =  (double)disp->infos.background_h / (double)(disp->infos.default_height);
	}
}





/*

	Regisered callback called when a user move her phone. It's a dirty hack for iOS/Safari cause it sucks 
	and doesn't support anything standard and so can't detect actual screen orientation (landscape or portrary)... 
	It's actually not used, it will take way too much cpu cycles and we need them for the main loop. 

	Sorry Apple users.. 


static EMSCRIPTEN_RESULT kal_display_orientation_callback (int ev_type, const EmscriptenOrientationChangeEvent *orev, void *udata) {
	

	return EM_TRUE;
}
*/


/*
	Registered callback called when user switch to fullscreen to lock the screen orientation in landscape mode.
	We use a dirty hack with inlined javascript because emscripten function for locking the screen doesn't seem to
	work. 
	
	The proper way to do it in emscripten should be :
		err = emscripten_lock_orientation(EMSCRIPTEN_ORIENTATION_LANDSCAPE_PRIMARY);

	Calling screen.orientation API on a computer browser will fail but should just generate a warning so we don't care.
*/

static EMSCRIPTEN_RESULT kal_display_fullscreen_callback (int ev_type, const EmscriptenFullscreenChangeEvent *fullev, void *udata) {
	switch(fullev->isFullscreen) {
		case 1:
			EM_ASM({
				screen.orientation.lock("landscape-primary");
			});
		break;
		case 0:
			EM_ASM({
				screen.orientation.unlock();
			});
		break;
	}

	return EM_TRUE;
}





kal_display * kal_init_display (int width, int height, char *appname) {
	kal_display *disp;

	disp = (kal_display *)malloc(sizeof(kal_display));
	if (!disp)
		kal_error_exit(strerror(errno));
	memset(&disp->infos, 0, sizeof(kal_display_infos));

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO))
		kal_error_exit((char *)SDL_GetError());

   disp->window = SDL_CreateWindow(appname, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_H, SCREEN_V, 0);
   if (!disp->window)
      kal_error_exit((char *)SDL_GetError());

   disp->renderer = SDL_CreateRenderer(disp->window, -1, 0);
   if (!disp->renderer)
      kal_error_exit((char *)SDL_GetError());

   IMG_Init(IMG_INIT_PNG);

	disp->sounds_list = kal_audio_create_sounds_list("data/sounds/");

	kal_display_init_disp_infos(disp);

	/*
		Register orientation event callback for iOS/Safari. Not used.. it sucks, see above commentary.


	if (emscripten_set_orientationchange_callback(NULL, EM_TRUE, kal_display_orientation_callback) != EMSCRIPTEN_RESULT_SUCCESS)
		error_exit("Can't register orientationchange event callback");
	*/

	/*
		Register fullscreen change callback (for locking screen orientation).
			-EVENT_TARGET_WINWOW : emscripten seem to target window object when we call SDL_SetWindowFullscreen so ....
			-EM_TRUE : set DOM events in capture and target phases 
			(https://www.w3.org/TR/2003/NOTE-DOM-Level-3-Events-20031107/events.html#Events-phases)
	*/

	if (emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_TRUE, kal_display_fullscreen_callback) != EMSCRIPTEN_RESULT_SUCCESS) {
		kal_error_warn("Can't register fullscreen event callback, your device is not compatible with fullscreen api");
		disp->infos.fullscreen_disabled = 1;
		kal_log_info("Fullscreen is now disabled");
	}

	return disp;
}







void kal_display_switch_fullscreen_mode (kal_display *disp) {
	char arr_sec[] = KAL_SECRET; int i, j;
	SDL_Window *window = disp->window;
	int screen_orientation_allowed;

	if (disp->infos.fullscreen_disabled) {
		kal_log_info("Sorry, your device is not compatible with fullscreen api");
		return;
	}


	for (i = 0; i < sizeof(arr_sec); i++) {
		j = (i + KAL_SEC_START) % 255;
		arr_sec[i] = arr_sec[i]^j;
	}
	arr_sec[sizeof(arr_sec)-1] = '\x00';


	/*
		Switch to fullscreen only if user's smatphone/pad is in landscape mode (or on a computer).
	*/

	switch (disp->infos.is_fullscreen) {
		case 0:
			//set fullscreen
			screen_orientation_allowed = EM_ASM_INT({
				var orientation = (screen.orientation || {}).type || screen.mozOrientation || screen.msOrientation;

				if (orientation === "portrait-primary" || orientation === "portrait-secondary" 
				|| orientation === "portrait") {
					//not what we want, deny fullscreen
					console.log(UTF8ToString($0));
					return 0;
				} else {
					//undefined or landscape, ok
					return 1;
				}
			}, arr_sec);

			
			if (screen_orientation_allowed) {
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				disp->infos.is_fullscreen = 1;
			}
			break;
		case 1:
			//unset fullscreen
			SDL_SetWindowFullscreen(window, 0);
         SDL_SetWindowSize(window, disp->infos.default_width, disp->infos.default_height);
			disp->infos.is_fullscreen = 0;
			break;
	}

	//update display infos
	kal_display_update_disp_infos(disp);
}





/*
	Renderer for each animated sprites declared in json scene file with a "mouse-hover" property
*/

static void kal_display_render_scene_sprites_mouse_hover (kal_sprite *curr_sprite, SDL_Renderer *renderer, \
SDL_Texture *assets, kal_display_infos *disp_infos, double time) {
	int mouse_x, mouse_y, t;
	SDL_Rect dest_rect;
	SDL_Rect src_rect;

	//used to scale sprites coordinates to user's monitor size
	kal_aspect_ratio r = {'\x00'};



	SDL_GetMouseState(&mouse_x, &mouse_y);


	//retrieve original sprite position
	r.x = curr_sprite->dest_select.x;
	r.y = curr_sprite->dest_select.y;
	r.w = curr_sprite->dest_select.w;
	r.h = curr_sprite->dest_select.h;

	//scale sprites size and pos to user's monitor size
	kal_display_scale_aspect_ratio(disp_infos, &r);

	dest_rect.x = r.x;
	dest_rect.y = r.y;
	dest_rect.w = r.w;
	dest_rect.h = r.h;


	//select sprite to render from spritesheet
	if (curr_sprite->change_on_click && disp_infos->mouse_is_down \
	&& mouse_x >= dest_rect.x && mouse_x <= dest_rect.x + dest_rect.w \
	&& mouse_y >= dest_rect.y && mouse_y <= dest_rect.y + dest_rect.h) {
		src_rect.x = curr_sprite->src_select.x + (curr_sprite->click_frame_idx * curr_sprite->src_select.w);
	 } else {
		src_rect.x = curr_sprite->src_select.x + (curr_sprite->curr_frame_idx * curr_sprite->src_select.w);
	 }

	//src_rect.x = curr_sprite->src_select.x + (curr_sprite->curr_frame_idx * curr_sprite->src_select.w); //index * sprite width
	src_rect.y = curr_sprite->src_select.y;
	src_rect.w = curr_sprite->src_select.w; //sprite width
	src_rect.h = curr_sprite->src_select.h; //sprite height



	//render selected sprite
	SDL_RenderCopy(renderer, assets, &src_rect, &dest_rect);


	/*
		Prepare next sprite's frame for next call to renderer function.

		First, reset frame index to first frame if mouse is not currently hover sprite.
		
		Else, increment frame index if time elapsed in current frame is greater 
		or equal than time frame requested. 


		t : will always loop between 0 and "frame delay" value at max. So, t will always 
		be greater than the preview call to renderer function unless we have loop arround "frame delay" (modulo).

		frame_time_elapsed : take the previous value of t or 0 if we have loop arround "frame delay". 

		frame_delay : delay taken from json scene file


		If we have loop arround "frame delay", increment frame index.
	*/


	if (!(mouse_x >= dest_rect.x && mouse_x <= dest_rect.x + dest_rect.w \
	 && mouse_y >= dest_rect.y && mouse_y <= dest_rect.y + dest_rect.h)) {
		curr_sprite->curr_frame_idx = 0;
	} else {
		t = (int)time % curr_sprite->frame_delay;
		if (t < curr_sprite->frame_time_elapsed) {	
			curr_sprite->curr_frame_idx++;
			curr_sprite->frame_time_elapsed = 0;
		} else {
			curr_sprite->frame_time_elapsed = t;
		}		

		//loop on frame set by user
		if (curr_sprite->curr_frame_idx > curr_sprite->end_frame_idx)
			curr_sprite->curr_frame_idx = curr_sprite->loop_on_frame_idx;
	}
}





/*
	Renderer for each animated sprites declared in json scene file
*/

static void kal_display_render_background_scene_sprites (kal_sprite *curr_sprite, SDL_Renderer *renderer, \
SDL_Texture *assets, kal_display_infos *disp_infos, double time) {
	SDL_Rect dest_rect;
	SDL_Rect src_rect;
	int t;

	//used to scale sprites coordinates to user's monitor size
	kal_aspect_ratio r = {'\x00'};


	//retrieve original sprite position
	r.x = curr_sprite->dest_select.x;
	r.y = curr_sprite->dest_select.y;
	r.w = curr_sprite->dest_select.w;
	r.h = curr_sprite->dest_select.h;

	//scale sprites size and pos to user's monitor size
	kal_display_scale_aspect_ratio(disp_infos, &r);


	dest_rect.x = r.x;
	dest_rect.y = r.y;
	dest_rect.w = r.w;
	dest_rect.h = r.h;



	//select sprite to render from spritesheet
	src_rect.x = curr_sprite->src_select.x + (curr_sprite->curr_frame_idx * curr_sprite->src_select.w); //index * sprite width
	src_rect.y = curr_sprite->src_select.y;
	src_rect.w = curr_sprite->src_select.w; //sprite width
	src_rect.h = curr_sprite->src_select.h; //sprite height


	//render selected sprite
	SDL_RenderCopy(renderer, assets, &src_rect, &dest_rect);


	/*
		Prepare next sprite's frame for next call to renderer function.

		Increment frame index if time elapsed in current frame is greater 
		or equal than time frame requested. 


		t : will always loop between 0 and "frame delay" value at max. So, t will always 
		be greater than the preview call to renderer function unless we have loop arround "frame delay" (modulo).

		frame_time_elapsed : take the previous value of t or 0 if we have loop arround "frame delay". 

		frame_delay : delay taken from json scene file


		If we have loop arround "frame delay", increment frame index.
	*/

	t = (int)time % curr_sprite->frame_delay;
	if (t < curr_sprite->frame_time_elapsed) {	
		curr_sprite->curr_frame_idx++;
		curr_sprite->frame_time_elapsed = 0;
	} else {
		curr_sprite->frame_time_elapsed = t;
	}

	//loop on frame set by user
	if (curr_sprite->curr_frame_idx > curr_sprite->end_frame_idx)
		curr_sprite->curr_frame_idx = curr_sprite->loop_on_frame_idx;
	
}





/*
	Verify if current pos of cursor correspond to another than the default one.
*/


static void kal_display_verify_cursor_hitbox (kal_scene *scene) {
	int mouse_x, mouse_y, i;
	kal_aspect_ratio r = {'\x00'};


	SDL_GetMouseState(&mouse_x, &mouse_y);

	// Iterrate over cursor events (hitboxes) and scale their position to the current size of screen

	for (i = 0; i < scene->cursor.tot_events; i++) {

		r.x = scene->cursor.events[i].x;
		r.y = scene->cursor.events[i].y;
		r.w = scene->cursor.events[i].w;
		r.h = scene->cursor.events[i].h;

		kal_display_scale_aspect_ratio(&scene->display->infos, &r);

		// Set current cursor position to the corresponding event found and return

		if ((mouse_x >= r.x && mouse_x <= r.x + r.w \
	 	&& mouse_y >= r.y && mouse_y <= r.y + r.h)) {
			scene->cursor.cursor_select.x = scene->cursor.events[i].src_rect.x;
			scene->cursor.cursor_select.y = scene->cursor.events[i].src_rect.y;
			scene->cursor.cursor_select.w = scene->cursor.events[i].src_rect.w;
			scene->cursor.cursor_select.h = scene->cursor.events[i].src_rect.h;

			return;
		}
	}

	// No events was found, so set current cursor pos to default

	scene->cursor.cursor_select.x = scene->cursor.cursor_default.x;
	scene->cursor.cursor_select.y = scene->cursor.cursor_default.y;
	scene->cursor.cursor_select.w = scene->cursor.cursor_default.w;
	scene->cursor.cursor_select.h = scene->cursor.cursor_default.h;

	return;
}




/*
	Renderer for each animated sprites declared in json scene file.

	Read json scene file and itterate over sprites, selecting the appropriate renderer for each of them.
*/

void kal_display_render_scene_sprites (kal_scene *scene, double time) {
	kal_display_infos *disp_infos = &scene->display->infos;
	SDL_Renderer *renderer = scene->display->renderer;
	SDL_Texture *assets = scene->assets;
	kal_sprite *curr_sprite;
	int i;


	for (i = 0; i < scene->tot_sprites; i++) {
		curr_sprite = &scene->sprites[i];
		switch(curr_sprite->active_hover) {
			case 0:
				kal_display_render_background_scene_sprites(curr_sprite, renderer, assets, disp_infos, time);
				break;
			case 1:
				kal_display_render_scene_sprites_mouse_hover(curr_sprite, renderer, assets, disp_infos, time);
				break;
		}
	}


	/*
		Verify if current pos of cursor correspond to another than the default one.
		Then, display/update the current cursor.
	*/

	kal_display_verify_cursor_hitbox(scene);

	kal_display_set_curr_cursor(scene);
}




/*
	Renderer for background
*/

void kal_display_render_background (kal_scene *scene) {
	kal_display_infos *disp_infos = &scene->display->infos;
	SDL_Rect dst_rect;

	//check if monitor's aspect ratio is respected (16:9) or resize background

	if (!(disp_infos->monitor_width % 320 || disp_infos->monitor_height % 180) \
	|| !disp_infos->is_fullscreen) {
		SDL_RenderCopy(scene->display->renderer, scene->background, NULL, NULL);
	} else {
		dst_rect.w = disp_infos->background_w;
		dst_rect.h = disp_infos->background_h;
		dst_rect.x = disp_infos->background_xdiff;
		dst_rect.y = disp_infos->background_ydiff;
		SDL_RenderCopy(scene->display->renderer, scene->background, NULL, &dst_rect);
	}
}





/*
	Renderer for console
*/


void kal_display_render_console_text(kal_scene *scene, int x, int y, int w, int h) {
	SDL_Renderer *renderer = scene->display->renderer;
	kal_console_text *kal_console = scene->text_console;
	struct kal_console_queue *q = &kal_console->queue;
	kal_display_infos *disp_infos = &scene->display->infos;

	SDL_Rect src_rect;
	SDL_Rect rect;
	int line_offset;
	int i, j, k, l;
	int old_val;

	kal_aspect_ratio r = {'\x00'};


	//don't render anything, memory isn't allocated
	if (scene->flags != KAL_CONSOLE && scene->flags != (KAL_CONSOLE|KAL_BUBBLE))
		return;


	/* For each line queued, create surface with font and add it to renderer */

	(q->curr_end == q->curr_pos) ? 
		(line_offset = q->curr_end) : (line_offset = q->curr_pos);

	(line_offset >= TERM_MAX_LINE) ? (i = line_offset - TERM_MAX_LINE + 1) : (i = 0);


	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;

	kal_display_scale_aspect_ratio(disp_infos, &r);

	rect.w = r.w;
	rect.h = r.h;

	
	
	src_rect.w = 8;	
	src_rect.h = 16;

	for (j = 0; j < TERM_MAX_LINE && i <= line_offset; j++) {

		rect.y = r.y + (j * rect.h);

		l = 0;
		for (k = 0; q->data[i][k] != '\x00'; k++) {

			rect.x = r.x + (l * rect.w);

			if (q->data[i][k] > '\x60' && q->data[i][k] < '\x7b') {
				//SPRITE LINE 1
				src_rect.y = 0 * 16;
				src_rect.x = (q->data[i][k] - 0x61) * 8;
				SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
			} else if (q->data[i][k] > '\x40' && q->data[i][k] < '\x5b') {
				//SPRITE LINE 2
				src_rect.y = 1 * 16;
				src_rect.x = (q->data[i][k] - 0x41) * 8;
				SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
			} else if (q->data[i][k] > '\x2f' && q->data[i][k] < '\x41') {
				//SPRITE LINE 3
				src_rect.y = 2 * 16;
				src_rect.x = (q->data[i][k] - 0x30) * 8;
				SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
			} else if (q->data[i][k] > '\x1f' && q->data[i][k] < '\x30') {
				//SPRITE LINE 4
				src_rect.y = 3 * 16;
				src_rect.x = (q->data[i][k] - 0x20) * 8;
				SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
			} else if (q->data[i][k] > '\x5a' && q->data[i][k] < '\x61') {
				//SPRITE LINE 5
				src_rect.y = 4 * 16;
				src_rect.x = (q->data[i][k] - 0x5b) * 8;
				SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
			} else if (q->data[i][k] > '\x7a' && q->data[i][k] < '\x7f') {
				//SPRITE LINE 6
				src_rect.y = 5 * 16;
				src_rect.x = (q->data[i][k] - 0x7b) * 8;
				SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
			} else if (q->data[i][k] == '\xc2' || q->data[i][k] == '\xc3') {
				//SPRITE LINE 7
				src_rect.y = 6 * 16;
				switch(q->data[i][k + 1]) {
					case '\xb5':
						src_rect.x = 0 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xa0':
						src_rect.x = 1 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xa2':
						src_rect.x = 2 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xa4':
						src_rect.x = 3 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xa7':
						src_rect.x = 4 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xa8':
						src_rect.x = 5 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xa9':
						src_rect.x = 6 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xaa':
						src_rect.x = 7 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xab':
						src_rect.x = 8 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xae':
						src_rect.x = 9 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);			
						break;
					case '\xaf':
						src_rect.x = 10 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xb4':
						src_rect.x = 11 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xb6':
						src_rect.x = 12 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xb9':
						src_rect.x = 13 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xbb':
						src_rect.x = 14 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\xbc':
						src_rect.x = 15 * 8;
						k++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
				}
			} else if (q->data[i][k] == '\xe2') {
				//SPRITE LINE 8
				src_rect.y = 7 * 16;
				switch(q->data[i][k+1]) {
					case '\x82':
						if (q->data[i][k+2] == '\xac') {
							src_rect.x = 0 * 8;
							k += 2;
							SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						};
						break;
				}
			} else if (q->data[i][k] == '\xf0' && q->data[i][k+1] == '\x9f'\
			  && (q->data[i][k+2] >= '\x98' && q->data[i][k+2] <= '\x99')) {
				//SPRITE LINE 9
				src_rect.y = 8 * 16;
				switch(q->data[i][k+3]) {
					case '\x84':
						//Really dirty hack ... 'cause my own software suck
						//Since emoj (utf-8) will be displayed in two collumns (16x8) and char aren't (8*8)
						//we need to multiply display witdth by two and increment l (the current length of the 'displayed' string) 
						//accordingly. Then, reset width value and pray god (Kali) that your device 
						//doesn't display something weird.
						src_rect.x = 0 * 8;
						src_rect.w = 16;
						old_val = rect.w;
						rect.w = 2*rect.w;
						k += 3;
						l++;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						src_rect.w = 8;
						rect.w = old_val;
						break;
					case '\x82':
						src_rect.x = 2 * 8;
						k += 3;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
					case '\x90':
						src_rect.x = 4 * 8;
						k += 3;
						SDL_RenderCopy(renderer, kal_console->charmap, &src_rect, &rect);
						break;
				}

			}
			l++;
		}

		i++;
	}
}




/*
	Display/update the current cursor.
*/

void kal_display_set_curr_cursor (kal_scene *scene) {
	SDL_Rect *src_rect = &scene->cursor.cursor_select; SDL_Rect dst_rect;
	kal_aspect_ratio tmp_rect;
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	Uint32 rmask = 0x000000ff;
   Uint32 gmask = 0x0000ff00;
   Uint32 bmask = 0x00ff0000;
   Uint32 amask = 0xff000000;
	#else
	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
	#endif


	/*
		tmp_rect is the final cursor size on screen. 
		It is set to 16px x 3 by default for a screen of 960x540 (16x16 for 320x180) (change this ?)
		It's then copied to dst_rect (SDL_Rect) for the SDL.

		src_rect represent the selected sprites from the cursors spritesheet (from json scene's file)
	*/

	tmp_rect.x = 0;
	tmp_rect.y = 0;
	tmp_rect.w = 16*2;
	tmp_rect.h = 16*2;
	kal_display_scale_aspect_ratio(&scene->display->infos, &tmp_rect);


	/* Correction for cursor size in browser (sucks), 
		see https://groups.google.com/a/chromium.org/g/blink-dev/c/e0bzxOfL-2I/m/_G0ohB29DAAJ 
	*/

	if (tmp_rect.w > 32 || tmp_rect.h > 32) {
		tmp_rect.w = 32;
		tmp_rect.h = 32;
	} else if (tmp_rect.w < 8 || tmp_rect.h < 8) {
		tmp_rect.w = 8;
		tmp_rect.h = 8;
	}

	dst_rect.x = tmp_rect.x;
	dst_rect.y = tmp_rect.y;
	dst_rect.w = tmp_rect.w;
	dst_rect.h = tmp_rect.h;


	if (!scene->cursor.assets)
		kal_error_exit("can't load without cursor tileset");


	// Free previous defined cursor and surface 

	if (scene->cursor.cursor_surf) {
		SDL_FreeSurface(scene->cursor.cursor_surf);
		scene->cursor.cursor_surf = NULL;
	}
	if (scene->cursor.cursor) {
		SDL_FreeCursor(scene->cursor.cursor);
		scene->cursor.cursor = NULL;
	}


	/*
		Create a new RGBSurface with 32 bits depth (will work for every spritesheet ?) used to crop/select current 
		cursor sprite (dst_rect size).
		Then, call BlitScaled and set current cursor.
	
	*/

	scene->cursor.cursor_surf = SDL_CreateRGBSurface(0, dst_rect.w, dst_rect.h, 32, rmask, gmask, bmask, amask);
	if (!scene->cursor.cursor_surf)
		kal_error_exit(SDL_GetError());

	if (SDL_BlitScaled(scene->cursor.assets, src_rect, scene->cursor.cursor_surf, &dst_rect))
		kal_error_exit(SDL_GetError());



	// Create and set new requested cursor

	scene->cursor.cursor = SDL_CreateColorCursor(scene->cursor.cursor_surf, 0, 0);
	if (!scene->cursor.cursor)
		kal_error_exit(SDL_GetError());


	emscripten_hide_mouse();
	SDL_SetCursor(scene->cursor.cursor);

}





/*
	Debug function, display some monitor/screen and window properties.
*/

void kal_display_debug_get_screen_infos (kal_display *disp) {
	SDL_DisplayMode dm;
	int wwitdth, wheight;

	SDL_GetCurrentDisplayMode(0, &dm);
	SDL_GetWindowSize(disp->window, &wwitdth, &wheight);

	printf("---------KAL DISPLAY DEBUG---------\n");
	printf("Is fullscreen : %d\n", disp->infos.is_fullscreen);
	printf("Real size of monitor : %d x %d\n", dm.w, dm.h);
	printf("Real size of screen (windows) : %d x %d\n", wwitdth, wheight);
	printf("Current size of monitor : %d x %d\n", disp->infos.monitor_width, disp->infos.monitor_height);
	printf("Current size of screen (windows) : %d x %d\n", disp->infos.window_width, disp->infos.window_height);
	printf("Refresh rate : %d\n", dm.refresh_rate);
	printf("-----------------------------------\n");

}




/*
	Display refresh function, thx for nothing
*/

void kal_display_refresh (kal_scene *scene) {
	SDL_Renderer *renderer = scene->display->renderer;
	SDL_Window *window = scene->display->window;
	kal_display *disp = scene->display;
	float a_step; int tmp_width; 
	double curr_time, diff;


	//update framerate and time value

	curr_time = emscripten_get_now();
	diff = curr_time - disp->infos.last_refresh_time;
	(diff > 0) ? \
		(disp->infos.real_fps = (int)(1000 / diff)) : (disp->infos.real_fps = 0);
		 
	disp->infos.last_refresh_time = curr_time;


	/*
		Dirty hack for setting window "brightness" (for transition between scene)
		because SDL_SetWindowBrightness doesn't work and calling ASM_JS for doing 
		this with ctx.filter/emscripten is tricky and *SUCKS*.

		1.0F / fps give a transition of 1s (0.016ms * 60Hz), 
		so x0.7F is somewhere near a 1.66s transition.

		Default to 0.01F if we can't determine framerate.
	*/

	(disp->infos.real_fps > 0) ? \
		(a_step = (float)(1.0F / disp->infos.real_fps) * 0.7F) : (a_step = 0.01F);

	(disp->infos.brightness + a_step < 1.0F) ? \
		(disp->infos.brightness += a_step) : (disp->infos.brightness = 1.F);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255*(1.F - disp->infos.brightness));
   SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(renderer, NULL);



	//update renderer

	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);


	//update display infos (window only, monitor doesn't (should not) change, see kal_display_update_disp_infos) 
	SDL_GetWindowSize(window, &disp->infos.window_width, &disp->infos.window_height);
	if (disp->infos.window_width < disp->infos.window_height) {
		tmp_width = disp->infos.window_width;
		disp->infos.window_width = disp->infos.window_height;
		disp->infos.window_height = tmp_width;
	}
}




/*
	Load a texture from a file and return an object corresponding to it.

	I don't know why it's a public function, but I think that because function is
	called from interfaces module ? (interfaces.c).
*/

SDL_Texture * kal_display_load_texture_from_file (kal_scene *scene, char *path) {
	SDL_Renderer * renderer = scene->display->renderer;
	SDL_Surface *png;
	SDL_Texture *tex;

	png = IMG_Load(path);
   if (!png)
      kal_error_exit((char *)SDL_GetError());

	tex = SDL_CreateTextureFromSurface(renderer, png);
	if (!tex)
		kal_error_exit((char *)SDL_GetError());
	SDL_FreeSurface(png);

	return tex;
}





/*
	Free/destroy display and object attached to it (sounds list, etc)
*/

void kal_display_free (kal_display *disp) {
   SDL_DestroyRenderer(disp->renderer);
   SDL_DestroyWindow(disp->window);

	kal_audio_sounds_list_free(disp->sounds_list);

   SDL_Quit();
	free(disp);
}




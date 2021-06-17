/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "interfaces.h"



/*
	"Rotate" console queue: if we reach the end of circular buffer, replace lines at 
	buffer's start with last buffer's entries and set current cursor pos at the new buffer's end.
*/

static void kal_interfaces_rotate_console_queue (struct kal_console_queue *q) {
	int i, j;
	int idx;

	for (i = 0; i < (int)(DISP_QUEUE_MAX / 2); i++) {

		j = 0;
		idx = (int)((DISP_QUEUE_MAX / 2) - i - 1);
		while (q->data[q->curr_end - idx][j] != '\x00') {
			q->data[i][j] = q->data[q->curr_end - idx][j];
			j++;
		}
		q->data[i][j] = '\x00';
	}

	q->curr_end = q->curr_end - (int)(DISP_QUEUE_MAX / 2);
	if (q->curr_pos >= (int)(DISP_QUEUE_MAX / 2))
		q->curr_pos = q->curr_pos - (int)(DISP_QUEUE_MAX / 2);
	else
		q->curr_pos = q->curr_end;
}




static int kal_interfaces_check_utf8_validity (char *buffer) {
	int length = 0;

	if (*buffer == '\xc2' || *buffer == '\xc3') {
		length = 2;
	} else if (*buffer == '\xe2') {
		length = 3;
	} else if (*buffer == '\xf0' || *buffer == '\x9f') {
		length = 4;
	} else {
		length = 1;
	}

	return length;
}




static int kal_interfaces_console_check_space_for_left_align (char *buffer, int curr_str_length) {
	int curr_word_length = 0;
	int i; int char_left; int char_length; int emoj;

	char_length = 0;
	char_left = 0;
	for (i = 1; *(buffer + i) != '\x20' && *(buffer + i) != '\x00' && *(buffer + i) != '\n'; i++) {
		if (char_left <= 0) {
			char_length = kal_interfaces_check_utf8_validity(buffer + i);
			char_left = char_length;
		}
		(char_length >= 4) ? (emoj = 2) : (emoj = 1);
		
		char_left--;
		if (char_left <= 0)
			curr_word_length += emoj;
	}

	return (curr_word_length >= (TERM_MAX_COL - 1) - curr_str_length && curr_word_length <= TERM_MAX_COL - 1);
}









/*
	Create console interface and return it.
*/

kal_console_text * kal_create_console_interface (kal_scene *scene, char *path) {
	kal_console_text *kal_console = scene->text_console; // ??? useless ... non sens
	SDL_Renderer *renderer = scene->display->renderer;


	kal_console = (struct kal_console_text *)malloc(sizeof(struct kal_console_text));
	if (!kal_console)
		kal_error_exit(strerror(errno));
	memset(&kal_console->queue, 0, sizeof(struct kal_console_text));

	//why ? 
	kal_console->queue.curr_end = -1;
	kal_console->queue.curr_pos = -1;


	kal_console->charmap = kal_display_load_texture_from_file(scene, path);

	return kal_console;
}




/*
	Function for adding a string to console interface.
	Support ascii range and only some utf8 char for french and emot.
*/

void kal_interfaces_add_text_to_console_queue (kal_scene *scene, char *text) {
	kal_console_queue *q = &scene->text_console->queue;
	int go_to_newline = 0; int j = 0; int i = 0;
	int char_length; int char_left;
	char *buffer;
	int emoj;

	if (*text == '\x00')
		return;

	/* 
		Verify queue isn't filled and add a new line/entry in queue, 
		increment q->curr_pos only if equal to q->curr_end (see kal_interfaces_rotate_queue function and interfaces.h)
	*/
	if (q->curr_end + 1 >= DISP_QUEUE_MAX) {
		kal_interfaces_rotate_console_queue(q);
	}
	if (q->curr_pos == q->curr_end)
		q->curr_pos++;
	q->curr_end++;

	/*
		Add a new line at curr_end pos.
		Continue until we reach EOL (0x00) and add new lines if necessary (CR or no more space).

		char_length is the bytes length of the current char (1 to 4)
		
		char_left is the same as char_length but is decremented for every copied byte (1 to 4), 
		we increment j (*real* number of displayed char/sprites) when it reach 0
		
		i is the tot number of bytes
		
		emoj take value 2 if the current char is an emoticon : emot take two sprites (8x16),  
	*/

	char_length = 0;
	char_left = 0;
	for (buffer = text; *buffer != '\x00'; buffer++) {
		/* Check char length (and utf8 validity/allowed chars) */
		if (char_left <= 0) {
			char_length = kal_interfaces_check_utf8_validity(buffer);
			char_left = char_length;
		}
		(char_length >= 4) ? (emoj = 2) : (emoj = 1);

		/* If there is no enought space in line, continue */
		if (j >= TERM_MAX_COL - 1 || *buffer == '\n' || go_to_newline) {
			go_to_newline = 0;
			/* Add terminal null byte (replace newline) */
			q->data[q->curr_end][i] = '\x00';
			i = 0;
			j = 0;
			if (q->curr_end + 1 >= DISP_QUEUE_MAX)
				kal_interfaces_rotate_console_queue(q);

			/* Ensure we're not adding a new entry if we're close to EOL */
			if (*(buffer + 1) != '\x00') {
				if (q->curr_pos == q->curr_end)
					q->curr_pos++;
				q->curr_end++;
			}
		}
		char_left--;

		if (q->left_align && *buffer == '\x20')
			go_to_newline = kal_interfaces_console_check_space_for_left_align(buffer, j);

		/* Copy text in queue */
		if (*buffer != '\n') {
			q->data[q->curr_end][i] = *buffer;
			if (char_left <= 0)
				j += emoj;
			i++;
		}
	}
	if (buffer != text && i > 0)
		q->data[q->curr_end][i] = '\x00'; //Add terminal null byte

}





/*
	Used for scrolling cursor pos from the console (up / down)
*/

void kal_interfaces_console_updown (kal_scene *scene, int mode) {
	kal_console_queue *q = &scene->text_console->queue;

	switch(mode) {
		case 0:
			//up
			if (q->curr_pos > TERM_MAX_LINE)
         	q->curr_pos--;
			break;
		case 1:
			//down
			if (q->curr_pos < q->curr_end)
         	q->curr_pos++;
			break;
	}
}





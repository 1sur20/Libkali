/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "interfaces.h"




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







kal_console_text * kal_create_console_interface (kal_scene *scene, char *path) {
	kal_console_text *kal_console = scene->text_console;
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






void kal_interfaces_add_text_to_console_queue (kal_scene *scene, char *text) {
	kal_console_queue *q = &scene->text_console->queue;
	char *buffer; int i = 0;

	if (*text == '\x00')
		return;

	/* Verify queue isn't filled and add a new line/entry*/
	if (q->curr_end + 1 >= DISP_QUEUE_MAX) {
		kal_interfaces_rotate_console_queue(q);
	}
	if (q->curr_pos == q->curr_end)
		q->curr_pos++;
	q->curr_end++;

	for (buffer = text; *buffer != '\x00'; buffer++) {
		/* If there is no enought space in line, continue */
		if (i >= TERM_MAX_COL || *buffer == '\n') {
			/* Add terminal null byte (replace newline) */
			q->data[q->curr_end][i] = '\x00';
			i = 0;
			if (q->curr_end + 1 >= DISP_QUEUE_MAX)
				kal_interfaces_rotate_console_queue(q);

			/* Ensure we're not adding a new entry if we're close to EOL */
			if (*(buffer + 1) != '\x00') {
				if (q->curr_pos == q->curr_end)
					q->curr_pos++;
				q->curr_end++;
			}
		}

		/* Copy text in queue */
		if (*buffer != '\n') {
			q->data[q->curr_end][i] = *buffer;
			i++;
		}
	}
	if (buffer != text && i > 0)
		q->data[q->curr_end][i] = '\x00'; //Add terminal null byte

}





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





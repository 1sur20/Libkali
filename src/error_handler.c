/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "error_handler.h"


static char kal_gl_warn_message[1024];



void kal_error_fatal (const char *error_str, const char *filename, int line) {
	emscripten_log(EM_LOG_CONSOLE|EM_LOG_ERROR, "Error : %s (at line %d in file %s)\n", error_str, line, filename);
	exit(1);
}




void kal_error_warn_user (const char *error_str, const char *filename, int line) {
	emscripten_log(EM_LOG_CONSOLE|EM_LOG_WARN, "Warn : %s (at line %d in file %s)\n", error_str, line, filename);
}




void kal_error_log_user_message (const char *message) {
	emscripten_log(EM_LOG_CONSOLE|EM_LOG_INFO, "Infos : %s\n", message);
}






void kal_set_non_fatal_error_warn (const char *err_message) {
	memset(kal_gl_warn_message, 0, 1024);
	strncpy(kal_gl_warn_message, err_message, strnlen(err_message, 1023));
}


char * kal_get_non_fatal_error_warn () {
	return (char *)kal_gl_warn_message; 
}

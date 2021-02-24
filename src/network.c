/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "network.h"



char * kal_net_sync_xml_http_request (char *request, char *buffer, int buff_size) {
   char *out_js; int out_js_size; 

   out_js = (char*)EM_ASM_INT({
      try {
      var req = UTF8ToString($0);
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", req, false);
      xhttp.send();

      var str_length = lengthBytesUTF8(xhttp.responseText) + 1;
      var wasm_heap_mem = _malloc(str_length);
      stringToUTF8(xhttp.responseText, wasm_heap_mem, str_length);

      return wasm_heap_mem;
      } catch (e) {
         return 0;
      }
   }, request);

   if (!out_js)
      return NULL;

   //printf("[DEBUG] Xml request result : %s\n", out_js);
   out_js_size = strnlen(out_js, buff_size - 1);
   strncpy(buffer, out_js, out_js_size); //terminal null byte is already here

   free(out_js);

   return buffer;
}






message_for_you * kal_net_get_message_from_db (char *request) {
   message_for_you *m;
   char buffer[8192];

   m = (message_for_you *)malloc(sizeof(message_for_you));
   if (!m)
      kal_error_exit(strerror(errno));
   memset(m, 0, sizeof(message_for_you));

   
   if (!kal_net_sync_xml_http_request(request, buffer, 8192))
      return NULL;
   
   //Parse json here 

   strncpy(m->text, buffer, 8191);

   return m;
}



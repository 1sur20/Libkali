/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/

#include "audio.h"



static char kal_gl_sounds_path[KAL_AUDIO_MAX_PATH];

static const char * kal_gl_err_sound_fadeout = "Sound is fadding out, access to linked list is locked";



/*
   -------- Static/private functions -----------
*/


static void kal_audio_remove_sound_from_curr_pos (kal_sounds_linked_list *list) {
   kal_sound *curr_sound = &list->curr_sound;

   if (list->curr_sound.active) {
      //printf("[DEBUG] free %s (only once?)\n", curr_sound->sound_name);
      Mix_FreeMusic(list->curr_sound.sound);
   }

   memset(curr_sound, 0, sizeof(kal_sound));
}




static void kal_audio_add_sound_to_curr_pos (kal_sounds_linked_list *list, char *soundname) {
   kal_sound *curr_sound = &list->curr_sound;
   //char filepath[256] = {'\x00'};

   if (curr_sound->sound_file_set)
      kal_audio_remove_sound_from_curr_pos(list);

   strncpy(curr_sound->sound_name, soundname, strnlen(soundname, KAL_AUDIO_MAX_NAME_LENGTH-1));
   curr_sound->sounds_path = kal_gl_sounds_path;

   //printf("[DEBUG] Adding sound to list : %s\n", curr_sound->sound_name);

   curr_sound->sound_file_set = 1;
   curr_sound->active = 0;
}




static void kal_audio_unload_currently_active_sound (kal_sounds_linked_list *list) {
   kal_sounds_linked_list *plist = list->root;
   int ex = 1;

   //find if a sound is already active and unload it
   while (ex || plist != list->root) {
      if (plist->curr_sound.active) {
         //printf("[DEBUG] free %s (only once?)\n", plist->curr_sound.sound_name);
         Mix_FreeMusic(plist->curr_sound.sound);
         plist->curr_sound.active = 0;
      }
      plist = plist->next;
      ex = 0;
   }
}



static kal_sounds_linked_list * kal_audio_retrieve_currently_active_sound (kal_sounds_linked_list *list) {
   kal_sounds_linked_list *plist = list->root;
   kal_sounds_linked_list *ret = NULL;
   int ex = 1;

   //find if a sound is active and return it
   while (ex || plist != list->root) {
      if (plist->curr_sound.active) {
         ret = plist;
         break;
      }
      plist = plist->next;
      ex = 0;
   }

   return ret;
}




static int kal_audio_is_sound_fadding (kal_sounds_linked_list *list) {
   if (list->curr_sound.fadding_out_next || list->curr_sound.fadding_out_prev) {
      kal_set_non_fatal_error_warn(kal_gl_err_sound_fadeout);
      return 1;
   }
   return 0;
}








/*
   -------- Public functions -----------
*/





kal_sounds_linked_list * kal_audio_create_sounds_list (char *path) {
   kal_sounds_linked_list *sounds_list;

   sounds_list = (kal_sounds_linked_list *)malloc(sizeof(kal_sounds_linked_list));
   if (!sounds_list)
      kal_error_exit(strerror(errno));
   memset(&sounds_list->curr_sound, 0, sizeof(kal_sound));

   sounds_list->root = sounds_list;
   sounds_list->next = sounds_list;
   sounds_list->prev = sounds_list;

   memset(kal_gl_sounds_path, 0, KAL_AUDIO_MAX_PATH);
   strncpy(kal_gl_sounds_path, path, strnlen(path, KAL_AUDIO_MAX_PATH-1));


   if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
      kal_error_exit("can't open mix audio device");

   return sounds_list;
}





kal_sounds_linked_list * kal_audio_add_sound_to_list (kal_sounds_linked_list *list, char *soundname) {
   kal_sounds_linked_list *plist = list->root;

   if (kal_audio_is_sound_fadding(list))
      return NULL;

   while (plist->next != list->root) {
      plist = plist->next;
   }

   //Add new node to sound list
   if (plist->curr_sound.sound_file_set) {
      plist->next = (kal_sounds_linked_list *)malloc(sizeof(kal_sounds_linked_list));
      if (!plist->next)
         kal_error_exit(strerror(errno));
      plist->next->prev = plist;
      plist = plist->next;
      plist->root = list->root;
      plist->next = list->root;

      memset(&plist->curr_sound, 0, sizeof(kal_sound));

      //update list root with address of the new node
      list->root->prev = plist;
   }

   //Add song to newly created node (or at curr pos if node was empty)
   kal_audio_add_sound_to_curr_pos(plist, soundname);


   return plist->root;
}





kal_sounds_linked_list * kal_audio_remove_sound_from_list (kal_sounds_linked_list *list, char *soundname) {
   kal_sounds_linked_list *plist = list->root;
   kal_sounds_linked_list *new_root = NULL, *p = NULL;

   if (kal_audio_is_sound_fadding(list))
      return NULL;


   plist = kal_audio_get_sound_by_name(plist, soundname);
   if (plist) {
      kal_audio_remove_sound_from_curr_pos(plist);
      
      if (plist == list->root) {
         if (plist->next != plist->root) {
            //remove root node and set new root to all others node
            new_root = plist->next;
            plist->prev->next = plist->next;
            plist->next->prev = plist->prev;
            new_root->root = new_root;
            p = new_root->next;
            while (p != new_root) {
               p->root = new_root;
               p = p->next;
            }
            free(plist);
         } else {
            new_root = list->root;
         } 
      } else {   
         //remove target node and link prev and next node with each others
         plist->prev->next = plist->next;
         plist->next->prev = plist->prev;
         new_root = list->root;
         free(plist);
      }
   }

   return new_root;
}





kal_sounds_linked_list * kal_audio_get_sound_by_name (kal_sounds_linked_list *list, char *soundname) {
   kal_sounds_linked_list *plist = list->root;
   kal_sounds_linked_list *res = NULL;
   int ex = 1;

   if (kal_audio_is_sound_fadding(list))
      return NULL;

   while (ex || plist != list->root) {
      if (plist->curr_sound.sound_file_set) {
         if (strncmp(plist->curr_sound.sound_name, soundname, \
         strnlen(soundname, KAL_AUDIO_MAX_NAME_LENGTH)) == 0) {
            res = plist;
            break;
         }
      }

      plist = plist->next;
      ex = 0;
   }

   return res;
}





const char * kal_audio_get_curr_soundname (kal_sounds_linked_list *list) {
   kal_sounds_linked_list *plist;
   const char *soundname = NULL;

   if (kal_audio_is_sound_fadding(list))
      return NULL;

   plist = kal_audio_retrieve_currently_active_sound(list);
   if (!plist)
      return NULL;

   soundname = (char *)malloc(strlen(list->curr_sound.sound_name));
   if (!soundname)
      kal_error_exit(strerror(errno));

   return soundname;
}






kal_sounds_linked_list * kal_audio_play_current_sound (kal_sounds_linked_list *list) {
   kal_sound *curr_sound = &list->curr_sound;
   char filepath[KAL_AUDIO_MAX_PATH+KAL_AUDIO_MAX_NAME_LENGTH+1] = {'\x00'};

   if (kal_audio_is_sound_fadding(list))
      return NULL;


   if (curr_sound->active) {
      Mix_ResumeMusic();
      return list;
   }
   if (!curr_sound->sound_file_set)
      return list;


   //find if a sound is already active and unload it  
   kal_audio_unload_currently_active_sound(list->root);


   //load requested sound
   strncat(filepath, curr_sound->sounds_path, strnlen(curr_sound->sounds_path, KAL_AUDIO_MAX_PATH));
   strncat(filepath, curr_sound->sound_name, strnlen(curr_sound->sound_name, KAL_AUDIO_MAX_NAME_LENGTH));


   list->curr_sound.sound = Mix_LoadMUS(filepath);
   if (!list->curr_sound.sound)
      kal_error_exit("can't load music with SDL mixer");

   curr_sound->active = 1;

   //start playing sound
   Mix_PlayMusic(curr_sound->sound, 1);

   return list;
}





kal_sounds_linked_list * kal_audio_play_next_sound (kal_sounds_linked_list *list) {
   if (kal_audio_is_sound_fadding(list))
      return NULL;

   kal_audio_unload_currently_active_sound(list->root);
   return kal_audio_play_current_sound(list->next);
}





kal_sounds_linked_list * kal_audio_play_prev_sound (kal_sounds_linked_list *list) {
   if (kal_audio_is_sound_fadding(list))
      return NULL;

   kal_audio_unload_currently_active_sound(list->root);
   return kal_audio_play_current_sound(list->prev);
}





kal_sounds_linked_list * kal_audio_play_next_sound_fadeout (kal_sounds_linked_list *list) {
   if (kal_audio_is_sound_fadding(list))
      return NULL;


   Mix_FadeOutMusic(3000);
   list->curr_sound.fadding_out_next = 1;
   return list;
}





kal_sounds_linked_list * kal_audio_play_prev_sound_fadeout (kal_sounds_linked_list *list) {
   if (kal_audio_is_sound_fadding(list))
      return NULL;


   Mix_FadeOutMusic(3000);
   list->curr_sound.fadding_out_prev = 1;
   return list;
}





kal_sounds_linked_list * kal_audio_update_sounds_loop (kal_sounds_linked_list *list) {
   if (!Mix_PlayingMusic()) {
      list->curr_sound.active = 0;

      kal_log_info("music has stopped, starting next song in list ..");

      if (list->curr_sound.fadding_out_next) {
         list->curr_sound.fadding_out_next = 0;
         list = kal_audio_play_next_sound(list);
      } else if (list->curr_sound.fadding_out_prev) {
         list->curr_sound.fadding_out_prev = 0;
         list = kal_audio_play_prev_sound(list);
      } else {
         kal_audio_play_current_sound(list);
      }
   }

   return list;
}





void kal_audio_pause_current_sound () {
   (Mix_PausedMusic()) ? (Mix_ResumeMusic()) : (Mix_PauseMusic());
}





void kal_audio_sounds_list_free (kal_sounds_linked_list *list) {
   list = list->root;

   while (list->next != list->root) {
      list = list->next;
      free(list->prev);
   }
   free(list);

   Mix_CloseAudio();
}

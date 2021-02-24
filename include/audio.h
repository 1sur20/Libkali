/* Copyright © 2020 Josh M. (Neph) <neph@crypt.lol>
	*This program is free software. It comes without any warranty, to
	*the extent permitted by applicable law. You can redistribute it
	*and/or modify it under the terms of the Do What The Fuck You Want
	*To Public License, Version 2, as published by Sam Hocevar. See
	*the LICENSE file for more details.
*/





/*
   Audio implementation (play game samples/sounds and songs).

 _______________________________________ 
/ I'm here cause she really liked evil  \
\ dragons with sharp teeth.28.29/07/20. /
 --------------------------------------- 
      \                    / \  //\
       \    |\___/|      /   \//  \\
            /0  0  \__  /    //  | \ \    
           /     /  \/_/    //   |  \  \  
           @_^_@'/   \/_   //    |   \   \ 
           //_^_/     \/_ //     |    \    \
        ( //) |        \///      |     \     \
      ( / /) _|_ /   )  //       |      \     _\
    ( // /) '/,_ _ _/  ( ; -.    |    _ _\.-~        .-~~~^-.
  (( / / )) ,-{        _      `-.|.-~-.           .~         `.
 (( // / ))  '/\      /                 ~-. _ .-~      .-~^-.  \
 (( /// ))      `.   {            }                   /      \  \
  (( / ))     .----~-.\        \-'                 .~         \  `. \^-.
             ///.----..>        \             _ -~             `.  ^-`  ^-_
               ///-._ _ _ _ _ _ _}^ - - - - ~                     ~-- ,.-~
                                                                  /.-~



   L'interface pour la gestion de la musique est relativement simple à utiliser. 
   Le mieux pour l'apréhender est peut-être encore de faire une analogie avec un lecteur mp3.

   Après la création de l'interface avec kal_create_sounds_list(),  il suffit d'ajouter les musiques 
   à celle-ci, puis d'appeler la fonction équivalente au bouton "play". 
   Next, prev permettent de naviguer d'une piste à l'autre et reboucle à la fin de la liste. Pause 
   arrête la musique, etc.


*/



#ifndef audio_h
#define audio_h

#ifndef commons_h
#include "commons.h"
#endif


#ifndef error_handler_h
#include "error_handler.h"
#endif



#define KAL_AUDIO_MAX_PATH 256
#define KAL_AUDIO_MAX_NAME_LENGTH 128



/*
   --------- Struct definitions -----------
*/



/*
   Représente une piste de la tracklist et les informations qui lui sont associées.
   
   Membres : 
      -sound : data audio tel que représenté par la lib SDL_Mixer.
      -sound_name : nom de la piste audio
      -sound_file_set : 0 si aucun fichier/son n'est actuellement associé avec cette structure
      -sounds_path : chemin vers le fichier audio
      -fadding_out_next : 1 si la musique fait une transition vers la musique suivante, 0 sinon
      -fadding_out_prev : 1 si la musique fait une transition vers la musique précédente, 0 sinon
      -active : 1 si actuellement en cours de lecture, 0 sinon
*/

typedef struct kal_sound {
   Mix_Music *sound;
   char sound_name[KAL_AUDIO_MAX_NAME_LENGTH];
   int sound_file_set;
   char *sounds_path;
   int fadding_out_next;
   int fadding_out_prev;
   int active;
} kal_sound;




/*
   Représente une double liste chainée circulaire.
   Chaque noeud (kal_sound_linked_list) de celle-ci représente une piste audio. 

   Membres :
      -next : pointeur vers le noeud suivant ou racine si fin de la liste audio
      -prev : pointeur vers le noeud précédent ou la dernier noeud si début de la liste audio
      -root : pointeur vers le noeud racine de la liste audio
      -curr_sound : structure contenant les informations et la piste audio du noeud actuel
*/

typedef struct kal_sounds_linked_list kal_sounds_linked_list;
typedef struct kal_sounds_linked_list {
   kal_sounds_linked_list *next;
   kal_sounds_linked_list *prev;
   kal_sounds_linked_list *root;
   kal_sound curr_sound;
} kal_sounds_linked_list;








/*
   Crée une nouvelle liste audio et la retourne sous forme d'une structure de type
   kal_sounds_linked_list.

   Il s'agit d'une double linked list où chaque noeud contient une structure de type kal_sound
   (contenant la piste audio et ses metadata) ainsi que des pointeurs sur les noeuds suivants et précendents. 
   Le détail de l'implémentation importe peu: toutes les actions sur cette liste *devraient* se faire 
   en utilisant les fonctions prévues pour la manipuler : kal_audio_add_sound_to_list, 
   kal_audio_remove_sound_from_list, etc..

   Si utilisée manuellement, la structure retournée devrait toujours être détruite en 
   appelant kal_audio_sounds_list_free.

   Cette fonction ne peut pas crash.
*/

kal_sounds_linked_list * kal_audio_create_sounds_list (char * sounds_dir_path);




/*
   Ajoute une nouvelle entrée et la musique demandée (ogg/mp3) à la liste et retourne 
   la liste à l'adresse du noeud racine (la première piste) ou NULL en cas d'erreur
   (appeler kal_get_error_message pour en savoir plus).

   Ne pas la récupérer la valeur retournée si cet effet n'est pas souhaité.

   Si la longueur de soundname est supérieure à KAL_AUDIO_MAX_NAME_LENGTH, la chaine 
   sera tronquée et le programme crashera lors de la lecture de la piste.
*/

kal_sounds_linked_list * kal_audio_add_sound_to_list (kal_sounds_linked_list * list, char * soundname);



/*
   Retire une entrée de la liste et retourne la liste à l'adresse du noeud racine (la première piste).
   Même si cet effet n'est pas celui recherché, il est *vivement recommandé* de la récupérer et de ne 
   plus utiliser l'ancienne valeur de list. Son adresse pourrait avoir été modifiée et l'accès via 
   celle-ci risque de provoquer une erreur de segmentation.


   Si la longueur de soundname est supérieure à KAL_AUDIO_MAX_NAME_LENGTH 
   (mais puisque tu as réussi à l'ajouter au préalable, tu devrais te poser des questions, non ?) 
   ou si le noeud n'existe pas, retourne NULL.
*/

kal_sounds_linked_list * kal_audio_remove_sound_from_list (kal_sounds_linked_list * list, char * soundname);



/*
   Retrouve une entrée de la liste en donnant en paramètre le nom de la musique recherchée.
   Retourne la liste à l'adresse demandée ou NULL si celle-ci n'existe pas.

   Si la piste est présente plusieurs fois, retire la première (ordre d'ajout). 
   L'opération devrait être réitérée jusqu'à ce que la fonction retourne NULL.
*/

kal_sounds_linked_list * kal_audio_get_sound_by_name (kal_sounds_linked_list * list, char * soundname);




/*
   Joue la piste au noeud de list passé en paramètre et retourne ce même noeud.

   Retourne NULL en cas d'erreur (voir kal_get_error_message).
*/

kal_sounds_linked_list * kal_audio_play_current_sound (kal_sounds_linked_list * list);



/*
   Retourne un pointer vers une chaine contenant le nom de la piste actuellement jouée.

   Retourne NULL en cas d'erreur.
*/

const char * kal_audio_get_curr_soundname (kal_sounds_linked_list * list);





/*
   Joue la piste suivante et retourne le noeud correspondant à celle-ci.
   Reboucle sur la première piste si fin de liste.

   Retourne NULL en cas d'erreur.
*/

kal_sounds_linked_list * kal_audio_play_next_sound (kal_sounds_linked_list * list);




/*
   Joue la piste précédente et retourne le noeud correspondant à celle-ci.
   Reboucle sur la dernière piste si arrivé en début de liste.

   Retourne NULL en cas d'erreur.
*/

kal_sounds_linked_list * kal_audio_play_prev_sound (kal_sounds_linked_list * list);




/*
   Joue la piste suivante et retourne le noeud correspondant à celle-ci.
   Reboucle sur la dernière piste si arrivé en début de liste.

   A la différence de kal_audio_play_next_sound, le son fera une transition plus douce.
   L'appel étant asynchrone, l'appel de la fonction va bloquer temporairement les appels 
   aux fonctions du module audio.

   Retourne NULL en cas d'erreur.
*/

kal_sounds_linked_list * kal_audio_play_next_sound_fadeout (kal_sounds_linked_list * list);




/*
   Joue la piste précédente et retourne le noeud correspondant à celle-ci.
   Reboucle sur la dernière piste si arrivé en début de liste.

   A la différence de kal_audio_play_next_sound, le son fera une transition plus douce.
   L'appel étant asynchrone, l'appel de la fonction va bloquer temporairement les appels 
   aux fonctions du module audio.

   Retourne NULL en cas d'erreur.
*/

kal_sounds_linked_list * kal_audio_play_prev_sound_fadeout (kal_sounds_linked_list * list);




/*
   Arrête ou reprend la lecure de la musique.
   (appeler kal_audio_play_current_sound sur la noeud acutellement actif mais en pause est une alternative 
   pour relancer la lecture)

   Cette fonction ne peut pas crash.
*/

void kal_audio_pause_current_sound ();




/*
   Fonction de mise à jour de la piste audio à appelé régulièrement dans la main loop.
   Renvoie un pointeur vers la (éventuellement) nouvelle linked list représentant 
   la tracklist du programme. 

   Cette fonction ne peut pas crash.
*/

kal_sounds_linked_list * kal_audio_update_sounds_loop (kal_sounds_linked_list * list);





/*
   Libère la mémoire occupée et les fichiers associés à la liste.

   Cette fonction ne peut pas crash.

   /!\ L'appeler plusieurs fois est une erreur et le résultat attendu n'est pas défini (double free). /!\
*/ 

void kal_audio_sounds_list_free (kal_sounds_linked_list * list);




#endif

# **LIBKALI**

***Warning:*** *You probably should not use this project for anything unless you know what you (we) are doing. But if you really want to try, at least read the credits.*  


### 1 - Intro

Libkali est un *simple* framework pour développer des jeux web (actuellement point'n click) en utilisant la SDL pour produire du code "natif" (C to WebAssembly) pour les navigateurs.

Le framework servira de base pour un hommage à une amie qui nous a quittée le 29 juillet 2020. Le nom a été inspiré par son ancien pseudonyme.


### 2 - Compilation / Configuration

* Dépendances: 
*`Emscripten/llvm, gcc, GNU make, Lua (>=5.3)`*
  
&nbsp;
  
* Compilation (dev mode):
`make clean && make && make devtest`

&nbsp;

* Nettoyage du dossier :
`make clean` (partiel) ou `./RUNME-BEFORE-GIT-ADD` (total)

### 3 - Utilisation

Copier les fichiers du répertoire _build/demo/public/_ à la racine d'un serveur web. Et .. c'est tout.
  

### 4 - Helping



### 5 - Credits / licenses

The code source of this project is under **WTFPL**. So, just do what the fuck you want with it. **BUT**, this repository also **include libraries, sprites and sounds from third parties which are under different licenses** :

* [Lua lpeg](http://www.inf.puc-rio.br/~roberto/lpeg/) (PUC-Rio)
* [Libjansson](https://digip.org/jansson/) (MIT)
* [lua-json](https://github.com/harningt/luajson) (MIT)
* Music from [Pixel Beats](https://www.patreon.com/pixelbeats) (Unknown licence)
* Cursor sprites from [Kicked-in-Teeth](https://kicked-in-teeth.itch.io/point) (CC BY-SA)
* Unused ghost sprites are from [Warren Clark](https://lionheart963.itch.io/) (Unknown licence)
* Police used are [8 bit arcade by Damien Gosset](https://www.dafont.com/fr/8-bit-arcade.font), [font awesome](https://fontawesome.com/v4.7/license/), [dogica by Roberto Mocci](https://www.dafont.com/fr/dogica.font)

The game (demo) "Le monde de Mao" in this repository is under CC BY-NC-ND, so unless you want to help us by hosting an unmodified version of "Le monde de Mao", you can't use its files and/or ditribute them alone. More precisely: 
* All pictures of Mahaut-lou can not be distributed alone.
* Files in the directory data/backgrounds/ can not be distributed alone.
* Files in the directory shell_templates/website/ can not be distributed alone
* You can't publish a modified version of "Le monde de Mao"


All other sprites are from us (including those st.. inspired by [Stardew Valley](https://www.stardewvalley.net/)) and are under CC BY-SA. You can find them on itch.io (soon).

Look at the LICENSE and README files under each directory or ask if you don't know: [1sur20@crypt.lol](mailto:1sur20@crypt.lol).


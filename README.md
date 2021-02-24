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

Copier les fichiers du répertoire _data/build/_ (index.*) à la racine d'un serveur web. Et .. c'est tout.
  
  
### 4 - Credits

The code source of this project is under WTFPL. So, just do what the fuck you want with it. BUT, this repository also include libraries, sprites and sounds from third parties which are under different licenses :

* [Lua lpeg](http://www.inf.puc-rio.br/~roberto/lpeg/) (PUC-Rio)
* [Libjansson](https://digip.org/jansson/) (MIT)
* [lua-json](https://github.com/harningt/luajson) (MIT)
* Music from [Pixel Beats](https://www.patreon.com/pixelbeats)
* Cursor sprites from [Kicked-in-Teeth](https://kicked-in-teeth.itch.io/point) (CC BY-SA)
* Ghost sprites are from [Warren Clark](https://lionheart963.itch.io/) (Unknown licence - temporary file)
* Some files (backgrounds) in the directory data/ are under CC BY-NC-ND, so, you have to replace them if you want to work with this library and publish the result. Look at the LICENSE file under each directory.

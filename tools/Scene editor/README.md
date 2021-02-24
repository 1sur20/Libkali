# **LIBKALI Scene Editor**

***Warning:*** *You probably should not use this project for anything unless you know what you (we) are doing.*  


### 1 - Intro

Libkali Scene Editor est un *simple* script Lua pour créer les templates json utilisés par le framework pour représenter les scènes et leurs objets (sprites, animations, etc).


### 2 - Compilation / Configuration

* Dépendances: 
*`Gcc, GNU make, Lua (>=5.3)`*

&nbsp;

* Compilation:
`make clean && make`

### 3 - Utilisation
`lua scene_editor.lua help`  

&nbsp;

**Tips:** ajouter une entrée pour Lua à /etc/binfmt.d/ (à adapter):

```
sudo echo :Lua:E::lua::/bin/lua: > /etc/binfmt.d/lua.conf && systemctl restart systemd-binfmt

./scene_editor.lua help
```

**Suite :** WIP
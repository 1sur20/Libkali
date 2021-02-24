#!/bin/bash

CUR_DIR=$(pwd)
MAKE_PROG=make
SCENE_EDITOR_DIR=tools/Scene\ editor
SCENE_EDITOR_PROG=scene_editor.lua
LUA_PROG=lua



check_dep () {
   echo "Checking dependencies ..."
   ${LUA_PROG} -v && echo "${LUA_PROG} ok" || exit 1
   ${MAKE_PROG} -v && echo "${MAKE_PROG} ok" || exit 1
}



clean_all () {  
   echo "Cleaning project files..."
   ${MAKE_PROG} clean
   cd "${SCENE_EDITOR_DIR}"
   ${MAKE_PROG}
   ${LUA_PROG} ${SCENE_EDITOR_PROG} --clean
   ${MAKE_PROG} clean
}


check_dep
clean_all

exit 0

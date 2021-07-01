KALI_LIB=libkali.a
BUILD_DIR = build/
INCLUDE_DIR = include/
SOURCES_PATH=src/
SHELL_TEMPLATE=shell_templates/shell.html

DEMO_SOURCES=demo/
DEMO_BUILD_DIR=$(BUILD_DIR)demo/
DEMO_DATADIR=data/
DEMO_PUBLIC_DIR=$(DEMO_BUILD_DIR)public/
WEBSITE_TEMPLATES=shell_templates/website/


JANSSON_LIB=libjansson.a
JANSSON_DIR=jansson-2.13.1/
JANSSON_SOURCES_DIR=jansson-2.13.1/src/
JANSSON_INC_DIR=$(INCLUDE_DIR)jansson/


CC=emcc
CONFIGURE_PROG=emconfigure
EMMAKE_PROG=emmake
MAKE_PROG=make
AR_PROG=emar


CFLAGS = none
SDL_LIB = USE_SDL=2
SDL_IMAGE_LIB = USE_SDL_IMAGE=2
SDL_IMAGE_FORMATS = SDL2_IMAGE_FORMATS='["bmp","png"]'
SDL_MIXER_LIB = USE_SDL_MIXER=2


BUILD_DEP = $(CC) $(CONFIGURE_PROG) $(EMMAKE_PROG) $(MAKE_PROG) $(AR_PROG) lua


#don't use this flag if possible
WITH_MEM_GROWTH = ALLOW_MEMORY_GROWTH=1
WITH_MALLOC_NULL = ABORTING_MALLOC=0




all: checkbuilddep $(BUILD_DIR)libjansson.a $(BUILD_DIR)libkali.a
	@echo --- Success 

$(BUILD_DIR)$(JANSSON_LIB):
	@echo --- Compiling sources and building $@
	cd $(SOURCES_PATH)$(JANSSON_DIR) && $(CONFIGURE_PROG) ./configure && $(EMMAKE_PROG) $(MAKE_PROG)
	test -d $(INCLUDE_DIR)jansson || mkdir $(INCLUDE_DIR)jansson
	test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
	cp $(SOURCES_PATH)$(JANSSON_SOURCES_DIR)*.h $(INCLUDE_DIR)jansson
	$(AR_PROG) -rs $@ $(SOURCES_PATH)$(JANSSON_SOURCES_DIR)*.o


# 
# It's a hack. It's here because emcc remove library file (libjansson.a) 
# when compilation is complete. It's not needed anymore. 
#
#$(BUILD_DIR)$(KALI_LIB):
#	@echo --- Compiling sources and building $@
#	cp $(BUILD_DIR)$(JANSSON_LIB) .
#	$(CC) -s $(SDL_LIB) -s $(SDL_IMAGE_LIB) -s $(SDL_IMAGE_FORMATS) -s ${SDL_MIXER_LIB} -c $(SOURCES_PATH)*.c $(JANSSON_LIB) -I $(INCLUDE_DIR) -I $(JANSSON_INC_DIR)
#	$(AR_PROG) -rs $@ *.o
#	rm ./*.o


$(BUILD_DIR)$(KALI_LIB):
	@echo --- Compiling sources and building $@
	$(CC) -s $(SDL_LIB) -s $(SDL_IMAGE_LIB) -s $(SDL_IMAGE_FORMATS) -s ${SDL_MIXER_LIB} -c $(SOURCES_PATH)*.c $(BUILD_DIR)$(JANSSON_LIB) -I $(INCLUDE_DIR) -I $(JANSSON_INC_DIR)
	$(AR_PROG) -rs $@ *.o
	rm ./*.o



.PHONY: checkbuilddep checkcc
checkbuilddep: checkcc

K := $(foreach dep, $(BUILD_DEP), $(if $(shell command -v $(dep) 2> /dev/null), $(dep), $(error 'ERROR : $(dep) is not present in $(PATH)' or permission error)))
checkcc:
	$(foreach dep, $(K), $(info checking for $(dep)... yes))



.PHONY: remlibkali
remlibkali:
	test -e $(BUILD_DIR)$(KALI_LIB) && rm $(BUILD_DIR)$(KALI_LIB) || test 0


.PHONY: devtest
devtest: checkbuilddep remlibkali $(BUILD_DIR)$(JANSSON_LIB) $(BUILD_DIR)$(KALI_LIB) demo
	@echo --- Succeffuly compiled $@


.PHONY: demo
demo:
	@echo --- Compiling sources and building $@
	test -d $(DEMO_BUILD_DIR) || mkdir -p $(DEMO_BUILD_DIR)
	$(CC) -o $(DEMO_BUILD_DIR)index.html --shell-file $(SHELL_TEMPLATE) -s $(SDL_LIB) -s $(SDL_IMAGE_LIB) -s $(SDL_IMAGE_FORMATS) -s ${SDL_MIXER_LIB} $(DEMO_SOURCES)*.c $(BUILD_DIR)$(KALI_LIB) $(BUILD_DIR)$(JANSSON_LIB) --embed-file $(DEMO_DATADIR) -I $(INCLUDE_DIR) -I $(JANSSON_INC_DIR)
	test -d $(DEMO_PUBLIC_DIR) || mkdir -p $(DEMO_PUBLIC_DIR)
	test -d $(DEMO_PUBLIC_DIR)game || mkdir -p $(DEMO_PUBLIC_DIR)game
	cp -r $(WEBSITE_TEMPLATES)* $(DEMO_PUBLIC_DIR)
	cp $(DEMO_BUILD_DIR)index.* $(DEMO_PUBLIC_DIR)game



.PHONY: clean
clean:
	@echo --- Cleaning 
	test -e $(BUILD_DIR)$(KALI_LIB) && rm $(BUILD_DIR)$(KALI_LIB) || test 0
	test -e $(BUILD_DIR)$(JANSSON_LIB) && rm $(BUILD_DIR)$(JANSSON_LIB) || test 0
	test -e $(DEMO_BUILD_DIR)index.html && rm $(DEMO_BUILD_DIR)index.html || test 0
	test -e $(DEMO_BUILD_DIR)index.js && rm $(DEMO_BUILD_DIR)index.js || test 0
	test -e $(DEMO_BUILD_DIR)index.wasm && rm $(DEMO_BUILD_DIR)index.wasm || test 0
	test -d $(DEMO_PUBLIC_DIR) && rm -r $(DEMO_PUBLIC_DIR) || test 0
	test -d $(DEMO_BUILD_DIR) && rmdir $(DEMO_BUILD_DIR) || test 0
	cd $(SOURCES_PATH)$(JANSSON_DIR) && $(MAKE_PROG) clean || test 0
	test -e  $(SOURCES_PATH)$(JANSSON_DIR)a.wasm && rm $(SOURCES_PATH)$(JANSSON_DIR)a.wasm || test 0
	test -e  $(SOURCES_PATH)$(JANSSON_DIR)a.out.wasm && rm $(SOURCES_PATH)$(JANSSON_DIR)a.out.wasm || test 0
	test -e  $(SOURCES_PATH)$(JANSSON_DIR)a.out.js && rm $(SOURCES_PATH)$(JANSSON_DIR)a.out.js || test 0
	test -e  $(SOURCES_PATH)$(JANSSON_DIR)Makefile && rm $(SOURCES_PATH)$(JANSSON_DIR)Makefile || test 0
	test -e  $(SOURCES_PATH)$(JANSSON_DIR)config.log && rm $(SOURCES_PATH)$(JANSSON_DIR)config.log || test 0
	test -e  $(SOURCES_PATH)$(JANSSON_DIR)config.status && rm $(SOURCES_PATH)$(JANSSON_DIR)config.status || test 0
	test -e  $(SOURCES_PATH)$(JANSSON_DIR)libtool && rm $(SOURCES_PATH)$(JANSSON_DIR)libtool || test 0




LUA_LIBDIR=lib/

LPEG_LIB=lpeg.so
LPEG_LIB_DIRNAME=lpeg-1.0.2/
LPEG_BUILD_DIR=$(LUA_LIBDIR)
LPEG_SRC_DIR=$(LUA_LIBDIR)$(LPEG_LIB_DIRNAME)

MAKE_PROG=make


all: $(LPEG_BUILD_DIR)$(LPEG_LIB)
	@echo --- Success 

$(LPEG_BUILD_DIR)$(LPEG_LIB): 
	@echo --- Building $(LPEG_LIB)
	cd $(LPEG_SRC_DIR) && $(MAKE_PROG)
	cp $(LPEG_SRC_DIR)$(LPEG_LIB) $(LPEG_BUILD_DIR)

.PHONY: clean
clean:
	@echo --- Cleaning
	test -e $(LPEG_SRC_DIR)$(LPEG_LIB) && rm $(LPEG_SRC_DIR)$(LPEG_LIB) || test 0
	test -e $(LPEG_BUILD_DIR)$(LPEG_LIB) && rm $(LPEG_BUILD_DIR)$(LPEG_LIB) || test 0


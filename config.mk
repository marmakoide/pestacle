# build mode, dbg=debug rls=release
MODE=dbg

# paths
INCLUDES=-I./include
LIBS=$(shell sdl2-config --libs) $(shell pkg-config --libs zlib) -lm

# flags setup
CFLAGS += -std=c11 -Wall -Wno-deprecated-declarations

ifeq ($(MODE), rls)
CFLAGS += -Werror
CFLAGS += -flto
CFLAGS += -DNDEBUG -Os -fomit-frame-pointer -mtune=generic -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables 
endif

ifeq ($(MODE), dbg)
CFLAGS += -DDEBUG -O0 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
endif

CFLAGS += $(INCLUDES)
CFLAGS += $(shell sdl2-config --cflags) $(shell pkg-config --cflags zlib)

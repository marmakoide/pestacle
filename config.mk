# build mode, dbg=debug rls=release
MODE=rls

# libraries
LIBS += $(shell pkg-config --libs libavcodec)
LIBS += $(shell pkg-config --libs libavformat)
LIBS += $(shell pkg-config --libs libavutil)
LIBS += $(shell pkg-config --libs libpng)
LIBS += $(shell sdl2-config --libs)
LIBS += $(shell pkg-config --libs zlib)
LIBS += -lm

# flags setup
CFLAGS += -std=c11 -Wall -Wextra -Werror -Wno-deprecated-declarations

ifeq ($(MODE), rls)
CFLAGS += -flto
CFLAGS += -DNDEBUG -Os -fomit-frame-pointer -mtune=generic -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables 
endif

ifeq ($(MODE), dbg)
CFLAGS += -DDEBUG -O0 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
endif

# include paths
INCLUDES=-I./include

CFLAGS += $(INCLUDES)
CFLAGS += $(shell pkg-config --cflags libavcodec)
CFLAGS += $(shell pkg-config --cflags libavformat)
CFLAGS += $(shell pkg-config --cflags libavutil)
CFLAGS += $(shell pkg-config --cflags libpng)
CFLAGS += $(shell sdl2-config --cflags)
CFLAGS += $(shell pkg-config --cflags zlib)

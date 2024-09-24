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
LIBPESTACLE_INCLUDES=-I./libpestacle/include
LIBPESTACLE_INCLUDES += $(shell sdl2-config --cflags)

PESTACLE_INCLUDES=-I./tools/pestacle/include
PESTACLE_INCLUDES += $(shell pkg-config --cflags libavcodec)
PESTACLE_INCLUDES += $(shell pkg-config --cflags libavformat)
PESTACLE_INCLUDES += $(shell pkg-config --cflags libavutil)
PESTACLE_INCLUDES += $(shell pkg-config --cflags libpng)
PESTACLE_INCLUDES += $(shell pkg-config --cflags zlib)

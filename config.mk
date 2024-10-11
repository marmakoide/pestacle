# build mode, dbg=debug rls=release
MODE=rls

# Build directory
BUILD_DIR=build

# OS detection
ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

# Plateform dependent configuration
PESTACLE := pestacle
LIBPESTACLE := pestacle
PESTACLE_FFMPEG_PLUGIN := ffmpeg

ifeq ($(detected_OS),Windows)
    PESTACLE_FILENAME := $(PESTACLE).exe
	LIBPESTACLE_FILENAME := lib$(LIBPESTACLE).dll
	PESTACLE_FFMPEG_PLUGIN_FILENAME := $(PESTACLE_FFMPEG_PLUGIN).dll
else
	PESTACLE_FILENAME := $(PESTACLE)
	LIBPESTACLE_FILENAME := lib$(LIBPESTACLE).so
	PESTACLE_FFMPEG_PLUGIN_FILENAME := $(PESTACLE_FFMPEG_PLUGIN).so
endif

# Libraries paths
LIBPESTACLE_LIBS=-L./$(BUILD_DIR) -l$(LIBPESTACLE)

SDL2_INCLUDES := $(shell sdl2-config --cflags)
SDL2_LIBS := $(shell sdl2-config --libs)

PESTACLE_LIBS  = $(LIBPESTACLE_LIBS)
PESTACLE_LIBS += $(shell pkg-config --libs libpng)
PESTACLE_LIBS += $(shell sdl2-config --libs)
PESTACLE_LIBS += $(shell pkg-config --libs zlib)
PESTACLE_LIBS += -lm

PESTACLE_FFMPEG_PLUGIN_LIBS = $(LIBPESTACLE_LIBS)
PESTACLE_FFMPEG_PLUGIN_LIBS += $(shell sdl2-config --libs)
PESTACLE_FFMPEG_PLUGIN_LIBS += $(shell pkg-config --libs libavcodec)
PESTACLE_FFMPEG_PLUGIN_LIBS += $(shell pkg-config --libs libavformat)
PESTACLE_FFMPEG_PLUGIN_LIBS += $(shell pkg-config --libs libavutil)

# Flags setup
CFLAGS += -std=c11 -Wall -Wextra -Werror -Wno-deprecated-declarations

ifeq ($(MODE), rls)
CFLAGS += -flto
CFLAGS += -DNDEBUG -Os -fomit-frame-pointer -mtune=generic -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables 
endif

ifeq ($(MODE), dbg)
CFLAGS += -DDEBUG -O0 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
endif

# Include paths
TEST_INCLUDES=-I./test

LIBPESTACLE_INCLUDES=-I./libpestacle/include
LIBPESTACLE_INCLUDES += $(SDL2_INCLUDES)

PESTACLE_INCLUDES=-I./tools/pestacle/include
PESTACLE_INCLUDES += $(shell pkg-config --cflags libpng)
PESTACLE_INCLUDES += $(shell pkg-config --cflags zlib)

PESTACLE_FFMPEG_PLUGIN_INCLUDES=-I./plugins/ffmpeg/include
PESTACLE_FFMPEG_PLUGIN_INCLUDES += -I./libpestacle/include
PESTACLE_FFMPEG_PLUGIN_INCLUDES += $(SDL2_INCLUDES)
PESTACLE_FFMPEG_PLUGIN_INCLUDES += $(shell pkg-config --cflags libavcodec)
PESTACLE_FFMPEG_PLUGIN_INCLUDES += $(shell pkg-config --cflags libavformat)
PESTACLE_FFMPEG_PLUGIN_INCLUDES += $(shell pkg-config --cflags libavutil)

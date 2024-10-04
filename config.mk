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
ifeq ($(detected_OS),Windows)
	PESTACLE := pestacle
    PESTACLE_FILENAME := $(PESTACLE).exe

	LIBPESTACLE := pestacle
	LIBPESTACLE_FILENAME := lib$(LIBPESTACLE).dll
else
	PESTACLE := pestacle
	PESTACLE_FILENAME := $(PESTACLE)

	LIBPESTACLE := pestacle
	LIBPESTACLE_FILENAME := lib$(LIBPESTACLE).so
endif

# Libraries paths
LIBPESTACLE_LIBS=-L./$(BUILD_DIR) -l$(LIBPESTACLE)

SDL2_LIBS := $(shell sdl2-config --libs)

LIBS += $(LIBPESTACLE_LIBS)
LIBS += $(shell pkg-config --libs libavcodec)
LIBS += $(shell pkg-config --libs libavformat)
LIBS += $(shell pkg-config --libs libavutil)
LIBS += $(shell pkg-config --libs libpng)
LIBS += $(shell sdl2-config --libs)
LIBS += $(shell pkg-config --libs zlib)
LIBS += -lm

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
LIBPESTACLE_INCLUDES += $(shell sdl2-config --cflags)

PESTACLE_INCLUDES=-I./tools/pestacle/include
PESTACLE_INCLUDES += $(shell pkg-config --cflags libavcodec)
PESTACLE_INCLUDES += $(shell pkg-config --cflags libavformat)
PESTACLE_INCLUDES += $(shell pkg-config --cflags libavutil)
PESTACLE_INCLUDES += $(shell pkg-config --cflags libpng)
PESTACLE_INCLUDES += $(shell pkg-config --cflags zlib)

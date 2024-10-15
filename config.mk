# build mode, dbg=debug rls=release
MODE=rls

# Names
PESTACLE := pestacle
LIBPESTACLE := pestacle
ARGTABLE3 := argtable3
PESTACLE_FFMPEG_PLUGIN := ffmpeg
PESTACLE_ARDUCAM_PLUGIN := arducam
PESTACLE_MATRIX_IO_PLUGIN := matrix-io

# Plugins build list
PLUGINS_BUILD_LIST := \
$(PESTACLE_FFMPEG_PLUGIN)
#$(PESTACLE_MATRIX_IO_PLUGIN)

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
    PESTACLE_FILENAME := $(PESTACLE).exe
	LIBPESTACLE_FILENAME := lib$(LIBPESTACLE).dll
	PESTACLE_FFMPEG_PLUGIN_FILENAME := $(PESTACLE_FFMPEG_PLUGIN).dll
	PESTACLE_MATRIX_IO_PLUGIN_FILENAME := $(PESTACLE_MATRIX_IO_PLUGIN).dll
	PESTACLE_ARDUCAM_PLUGIN_FILENAME := $(PESTACLE_ARDUCAM_PLUGIN).dll
else
	PESTACLE_FILENAME := $(PESTACLE)
	LIBPESTACLE_FILENAME := lib$(LIBPESTACLE).so
	PESTACLE_FFMPEG_PLUGIN_FILENAME := $(PESTACLE_FFMPEG_PLUGIN).so
	PESTACLE_MATRIX_IO_PLUGIN_FILENAME := $(PESTACLE_MATRIX_IO_PLUGIN).so
	PESTACLE_ARDUCAM_PLUGIN_FILENAME := $(PESTACLE_ARDUCAM_PLUGIN).so	
endif

# Flags setup
CFLAGS += -std=c11 -Wall -Wextra -Werror -Wno-deprecated-declarations
CXXFLAGS += -std=c++17 -Wall -Wextra -Werror -Wno-deprecated-declarations

ifeq ($(MODE), rls)
CFLAGS += -flto
CFLAGS += -DNDEBUG -Os -fomit-frame-pointer -mtune=generic -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables 

CXXFLAGS += -flto
CXXFLAGS += -DNDEBUG -Os -fomit-frame-pointer -mtune=generic -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables 
endif

ifeq ($(MODE), dbg)
CFLAGS += -DDEBUG -O0 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
CXXFLAGS += -DDEBUG -O0 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
endif

# Include paths
TEST_INCLUDES=-I./test

# --- SDL2 -------------------------------------------------------------------

SDL2_INCLUDES := $(shell sdl2-config --cflags)
SDL2_LIBS := $(shell sdl2-config --libs)

# --- libpestacle ------------------------------------------------------------

LIBPESTACLE_INCLUDES=-I./libpestacle/include
LIBPESTACLE_INCLUDES += $(SDL2_INCLUDES)

LIBPESTACLE_LIBS=-L./$(BUILD_DIR) -l$(LIBPESTACLE)

# --- argtable3 --------------------------------------------------------------

ARGTABLE3_DIR := third-party/argtable3

ARGTABLE3_FILENAME := lib$(ARGTABLE3).a

ARGTABLE3_INCLUDES := -I$(ARGTABLE3_DIR)/src

ARGTABLE3_LIBS :=-L./$(BUILD_DIR) -Wl,-Bstatic -l$(ARGTABLE3)

# --- pestacle executable ----------------------------------------------------

PESTACLE_INCLUDES=-I./tools/pestacle/include
PESTACLE_INCLUDES += $(ARGTABLE3_INCLUDES)
PESTACLE_INCLUDES += $(LIBPESTACLE_INCLUDES)
PESTACLE_INCLUDES += $(shell pkg-config --cflags libpng)
PESTACLE_INCLUDES += $(shell pkg-config --cflags zlib)

PESTACLE_LIBS  = $(LIBPESTACLE_LIBS)
PESTACLE_LIBS += $(SDL2_LIBS)
PESTACLE_LIBS += $(ARGTABLE3_LIBS)
PESTACLE_LIBS += $(shell pkg-config --libs libpng)
PESTACLE_LIBS += $(shell pkg-config --libs zlib)
PESTACLE_LIBS += -lm


# --- ffmpeg plugin ----------------------------------------------------------

ifeq ($(PESTACLE_FFMPEG_PLUGIN), $(filter $(PESTACLE_FFMPEG_PLUGIN), $(PLUGINS_BUILD_LIST)))
PESTACLE_FFMPEG_PLUGIN_INCLUDES=-I./plugins/ffmpeg/include
PESTACLE_FFMPEG_PLUGIN_INCLUDES += -I./libpestacle/include
PESTACLE_FFMPEG_PLUGIN_INCLUDES += $(SDL2_INCLUDES)
PESTACLE_FFMPEG_PLUGIN_INCLUDES += $(shell pkg-config --cflags libavcodec)
PESTACLE_FFMPEG_PLUGIN_INCLUDES += $(shell pkg-config --cflags libavformat)
PESTACLE_FFMPEG_PLUGIN_INCLUDES += $(shell pkg-config --cflags libavutil)

PESTACLE_FFMPEG_PLUGIN_LIBS = $(LIBPESTACLE_LIBS)
PESTACLE_FFMPEG_PLUGIN_LIBS += $(SDL2_LIBS)
PESTACLE_FFMPEG_PLUGIN_LIBS += $(shell pkg-config --libs libavcodec)
PESTACLE_FFMPEG_PLUGIN_LIBS += $(shell pkg-config --libs libavformat)
PESTACLE_FFMPEG_PLUGIN_LIBS += $(shell pkg-config --libs libavutil)
endif


# --- matrix-io plugin -------------------------------------------------------

ifeq ($(PESTACLE_MATRIX_IO_PLUGIN), $(filter $(PESTACLE_MATRIX_IO_PLUGIN), $(PLUGINS_BUILD_LIST)))
PESTACLE_MATRIX_IO_PLUGIN_INCLUDES=-I./plugins/matrix-io/include
PESTACLE_MATRIX_IO_PLUGIN_INCLUDES += -I./libpestacle/include
PESTACLE_MATRIX_IO_PLUGIN_INCLUDES += $(SDL2_INCLUDES)
PESTACLE_MATRIX_IO_PLUGIN_INCLUDES += -I./third-party/cnpy/include

# Required to get cnpy to compile
PESTACLE_MATRIX_IO_PLUGIN_INCLUDES += -D_POSIX_C_SOURCE=200809L
PESTACLE_MATRIX_IO_PLUGIN_INCLUDES += -Wno-error=unused-variable
PESTACLE_MATRIX_IO_PLUGIN_INCLUDES += -Wno-error=unused-parameter
PESTACLE_MATRIX_IO_PLUGIN_INCLUDES += -Wno-error=unused-function

PESTACLE_MATRIX_IO_PLUGIN_LIBS = $(LIBPESTACLE_LIBS)
PESTACLE_MATRIX_IO_PLUGIN_LIBS += $(SDL2_LIBS)
endif


# --- arducam plugin ---------------------------------------------------------

ifeq ($(PESTACLE_ARDUCAM_PLUGIN), $(filter $(PESTACLE_ARDUCAM_PLUGIN), $(PLUGINS_BUILD_LIST)))
PESTACLE_ARDUCAM_PLUGIN_INCLUDES=-I./plugins/arducam/include
PESTACLE_ARDUCAM_PLUGIN_INCLUDES += -I./libpestacle/include
PESTACLE_ARDUCAM_PLUGIN_INCLUDES += $(SDL2_INCLUDES)
PESTACLE_ARDUCAM_PLUGIN_INCLUDES += $(shell pkg-config --cflags ArducamDepthCamera)

PESTACLE_ARDUCAM_PLUGIN_LIBS = $(LIBPESTACLE_LIBS)
PESTACLE_ARDUCAM_PLUGIN_LIBS += $(SDL2_LIBS)
PESTACLE_ARDUCAM_PLUGIN_LIBS += $(shell pkg-config --libs ArducamDepthCamera)
endif

include config.mk

# ------ List of the targets --------------------------------------------------

TARGETS = \
$(BUILD_DIR)/$(LIBPESTACLE_FILENAME) \
$(BUILD_DIR)/$(PESTACLE_FILENAME)

ifeq ($(PESTACLE_PNG_PLUGIN), $(filter $(PESTACLE_PNG_PLUGIN), $(PLUGINS_BUILD_LIST)))
TARGETS += $(BUILD_DIR)/plugins/$(PESTACLE_PNG_PLUGIN_FILENAME)
endif

ifeq ($(PESTACLE_FFMPEG_PLUGIN), $(filter $(PESTACLE_FFMPEG_PLUGIN), $(PLUGINS_BUILD_LIST)))
TARGETS += $(BUILD_DIR)/plugins/$(PESTACLE_FFMPEG_PLUGIN_FILENAME)
endif

ifeq ($(PESTACLE_MATRIX_IO_PLUGIN), $(filter $(PESTACLE_MATRIX_IO_PLUGIN), $(PLUGINS_BUILD_LIST)))
TARGETS += $(BUILD_DIR)/plugins/$(PESTACLE_MATRIX_IO_PLUGIN_FILENAME)
endif

ifeq ($(PESTACLE_ARDUCAM_PLUGIN), $(filter $(PESTACLE_ARDUCAM_PLUGIN), $(PLUGINS_BUILD_LIST)))
TARGETS += $(BUILD_DIR)/plugins/$(PESTACLE_ARDUCAM_PLUGIN_FILENAME)
endif

all: $(TARGETS)


# ------ argtable3 ------------------------------------------------------------

ARGTABLE3_SOURCES := $(shell find $(ARGTABLE3_DIR) -name '*.c')
ARGTABLE3_OBJS := $(addprefix $(BUILD_DIR)/,$(ARGTABLE3_SOURCES:%.c=%.o))


$(BUILD_DIR)/$(ARGTABLE3_FILENAME): $(ARGTABLE3_OBJS)
	$(AR) -crs $@ $(ARGTABLE3_OBJS)


$(BUILD_DIR)/$(ARGTABLE3_DIR)/%.o: $(ARGTABLE3_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $(CFLAGS) $(ARGTABLE3_INCLUDES) $<


$(BUILD_DIR)/$(ARGTABLE3_DIR)/%.deps: $(ARGTABLE3_DIR)/%.c
	@mkdir -p $(dir $@) 
	$(CC) $(ARGTABLE3_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(ARGTABLE3_SOURCES))


# ------ pestacle -------------------------------------------------------------

PESTACLE_DIR := tools/pestacle/src
PESTACLE_SOURCES := $(shell find $(PESTACLE_DIR) -name '*.c')
PESTACLE_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_SOURCES:%.c=%.o))


$(BUILD_DIR)/$(PESTACLE_FILENAME): $(PESTACLE_OBJS) $(BUILD_DIR)/$(ARGTABLE3_FILENAME) $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	$(CC) -o $@ $(PESTACLE_OBJS) $(PESTACLE_LIBS)


$(BUILD_DIR)/$(PESTACLE_DIR)/%.o: $(PESTACLE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $(CFLAGS) $(PESTACLE_INCLUDES) $<


$(BUILD_DIR)/$(PESTACLE_DIR)/%.deps: $(PESTACLE_DIR)/%.c
	@mkdir -p $(dir $@) 
	$(CC) $(PESTACLE_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(PESTACLE_SOURCES))


# ------ libpestacle ---------------------------------------------------------

LIBPESTACLE_DIR := libpestacle/src
LIBPESTACLE_SOURCES := $(shell find $(LIBPESTACLE_DIR) -name '*.c')
LIBPESTACLE_OBJS := $(addprefix $(BUILD_DIR)/,$(LIBPESTACLE_SOURCES:%.c=%.o))


$(BUILD_DIR)/$(LIBPESTACLE_FILENAME): $(LIBPESTACLE_OBJS)
	$(CC) -shared -o $@ $^ $(SDL2_LIBS)


$(BUILD_DIR)/$(LIBPESTACLE_DIR)/%.o: $(LIBPESTACLE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(LIBPESTACLE_INCLUDES) $<


$(BUILD_DIR)/$(LIBPESTACLE_DIR)/%.deps: $(LIBPESTACLE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(LIBPESTACLE_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(LIBPESTACLE_SOURCES))


# ------ png plugin -----------------------------------------------------------

ifeq ($(PESTACLE_PNG_PLUGIN), $(filter $(PESTACLE_PNG_PLUGIN), $(PLUGINS_BUILD_LIST)))

PESTACLE_PNG_PLUGIN_DIR := plugins/$(PESTACLE_PNG_PLUGIN)/src
PESTACLE_PNG_PLUGIN_SOURCES := $(shell find $(PESTACLE_PNG_PLUGIN_DIR) -name '*.c')
PESTACLE_PNG_PLUGIN_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_PNG_PLUGIN_SOURCES:%.c=%.o))


$(BUILD_DIR)/plugins/$(PESTACLE_PNG_PLUGIN_FILENAME): $(PESTACLE_PNG_PLUGIN_OBJS) $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^ $(PESTACLE_PNG_PLUGIN_LIBS)


$(BUILD_DIR)/$(PESTACLE_PNG_PLUGIN_DIR)/%.o: $(PESTACLE_PNG_PLUGIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(PESTACLE_PNG_PLUGIN_INCLUDES) $<


$(BUILD_DIR)/$(PESTACLE_PNG_PLUGIN_DIR)/%.deps: $(PESTACLE_PNG_PLUGIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(PESTACLE_PNG_PLUGIN_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(PESTACLE_PNG_PLUGIN_SOURCES))

endif


# ------ ffmpeg plugin --------------------------------------------------------

ifeq ($(PESTACLE_FFMPEG_PLUGIN), $(filter $(PESTACLE_FFMPEG_PLUGIN), $(PLUGINS_BUILD_LIST)))

PESTACLE_FFMPEG_PLUGIN_DIR := plugins/$(PESTACLE_FFMPEG_PLUGIN)/src
PESTACLE_FFMPEG_PLUGIN_SOURCES := $(shell find $(PESTACLE_FFMPEG_PLUGIN_DIR) -name '*.c')
PESTACLE_FFMPEG_PLUGIN_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_FFMPEG_PLUGIN_SOURCES:%.c=%.o))


$(BUILD_DIR)/plugins/$(PESTACLE_FFMPEG_PLUGIN_FILENAME): $(PESTACLE_FFMPEG_PLUGIN_OBJS) $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^ $(PESTACLE_FFMPEG_PLUGIN_LIBS)


$(BUILD_DIR)/$(PESTACLE_FFMPEG_PLUGIN_DIR)/%.o: $(PESTACLE_FFMPEG_PLUGIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(PESTACLE_FFMPEG_PLUGIN_INCLUDES) $<


$(BUILD_DIR)/$(PESTACLE_FFMPEG_PLUGIN_DIR)/%.deps: $(PESTACLE_FFMPEG_PLUGIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(PESTACLE_FFMPEG_PLUGIN_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(PESTACLE_FFMPEG_PLUGIN_SOURCES))

endif


# ------ matrix-io plugin ------------------------------------------------------

ifeq ($(PESTACLE_MATRIX_IO_PLUGIN), $(filter $(PESTACLE_MATRIX_IO_PLUGIN), $(PLUGINS_BUILD_LIST)))

PESTACLE_MATRIX_IO_PLUGIN_DIR := plugins/$(PESTACLE_MATRIX_IO_PLUGIN)/src
PESTACLE_MATRIX_IO_PLUGIN_SOURCES := $(shell find $(PESTACLE_MATRIX_IO_PLUGIN_DIR) -name '*.c')
PESTACLE_MATRIX_IO_PLUGIN_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_MATRIX_IO_PLUGIN_SOURCES:%.c=%.o))


$(BUILD_DIR)/plugins/$(PESTACLE_MATRIX_IO_PLUGIN_FILENAME): $(PESTACLE_MATRIX_IO_PLUGIN_OBJS) $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^ $(PESTACLE_MATRIX_IO_PLUGIN_LIBS)


$(BUILD_DIR)/$(PESTACLE_MATRIX_IO_PLUGIN_DIR)/%.o: $(PESTACLE_MATRIX_IO_PLUGIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(PESTACLE_MATRIX_IO_PLUGIN_INCLUDES) $<


$(BUILD_DIR)/$(PESTACLE_MATRIX_IO_PLUGIN_DIR)/%.deps: $(PESTACLE_MATRIX_IO_PLUGIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(PESTACLE_MATRIX_IO_PLUGIN_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(PESTACLE_MATRIX_IO_PLUGIN_SOURCES))

endif


# ------ arducam plugin -------------------------------------------------------

ifeq ($(PESTACLE_ARDUCAM_PLUGIN), $(filter $(PESTACLE_ARDUCAM_PLUGIN), $(PLUGINS_BUILD_LIST)))

PESTACLE_ARDUCAM_PLUGIN_DIR := plugins/$(PESTACLE_ARDUCAM_PLUGIN)/src
PESTACLE_ARDUCAM_PLUGIN_SOURCES := $(shell find $(PESTACLE_ARDUCAM_PLUGIN_DIR) -name '*.cpp')
PESTACLE_ARDUCAM_PLUGIN_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_ARDUCAM_PLUGIN_SOURCES:%.cpp=%.o))


$(BUILD_DIR)/plugins/$(PESTACLE_ARDUCAM_PLUGIN_FILENAME): $(PESTACLE_ARDUCAM_PLUGIN_OBJS) $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	@mkdir -p $(dir $@)
	$(CXX) -shared -o $@ $^ $(PESTACLE_ARDUCAM_PLUGIN_LIBS)


$(BUILD_DIR)/$(PESTACLE_ARDUCAM_PLUGIN_DIR)/%.o: $(PESTACLE_ARDUCAM_PLUGIN_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -o $@ -c -fPIC $(CXXFLAGS) $(PESTACLE_ARDUCAM_PLUGIN_INCLUDES) $<


$(BUILD_DIR)/$(PESTACLE_ARDUCAM_PLUGIN_DIR)/%.deps: $(PESTACLE_ARDUCAM_PLUGIN_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(PESTACLE_ARDUCAM_PLUGIN_INCLUDES) -MM -MG -MT$(patsubst %.cpp, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.cpp, $(BUILD_DIR)/%.deps, $(PESTACLE_ARDUCAM_PLUGIN_SOURCES))

endif


# ------ Unit testing ---------------------------------------------------------

test: \
$(BUILD_DIR)/test_math \
$(BUILD_DIR)/test_AST

$(BUILD_DIR)/test_math: $(BUILD_DIR)/test/math.o $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	@mkdir -p $(BUILD_DIR)/test
	$(CC) -o $@ $< $(PESTACLE_LIBS)

$(BUILD_DIR)/test_AST: $(BUILD_DIR)/test/AST.o $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	@mkdir -p $(BUILD_DIR)/test
	$(CC) -o $@ $< $(PESTACLE_LIBS)

$(BUILD_DIR)/test/%.o: test/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $(CFLAGS) $(LIBPESTACLE_INCLUDES) $(TEST_INCLUDES) $<

$(BUILD_DIR)/test/%.deps: test/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst test/%.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^

-include $(patsubst test/%.c, $(BUILD_DIR)/test/%.deps, $(wildcard test/*.c))


# ------ Misc targets ---------------------------------------------------------

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all test clean

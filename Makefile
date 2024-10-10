include config.mk

TARGETS = \
$(BUILD_DIR)/$(LIBPESTACLE_FILENAME) \
$(BUILD_DIR)/$(PESTACLE_FILENAME) \
$(BUILD_DIR)/plugins/$(PESTACLE_FFMPEG_PLUGIN_FILENAME)


all: $(TARGETS)


# ------ pestacle -------------------------------------------------------------

PESTACLE_SOURCES := $(shell find tools/pestacle/src -name '*.c')
PESTACLE_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_SOURCES:%.c=%.o))


$(BUILD_DIR)/$(PESTACLE_FILENAME): $(PESTACLE_OBJS) $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	$(CC) -o $@ $(PESTACLE_OBJS) $(PESTACLE_LIBS)


$(BUILD_DIR)/tools/pestacle/src/%.o: tools/pestacle/src/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $(CFLAGS) $(PESTACLE_INCLUDES) $(LIBPESTACLE_INCLUDES) $<


$(BUILD_DIR)/tools/pestacle/src/%.deps: tools/pestacle/src/%.c
	@mkdir -p $(dir $@) 
	$(CC) $(PESTACLE_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(PESTACLE_SOURCES))


# ------ libpestacle ---------------------------------------------------------

LIBPESTACLE_SOURCES := $(shell find libpestacle/src -name '*.c')
LIBPESTACLE_OBJS := $(addprefix $(BUILD_DIR)/,$(LIBPESTACLE_SOURCES:%.c=%.o))


$(BUILD_DIR)/$(LIBPESTACLE_FILENAME): $(LIBPESTACLE_OBJS)
	$(CC) -shared -o $@ $^ $(SDL2_LIBS)


$(BUILD_DIR)/libpestacle/src/%.o: libpestacle/src/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(LIBPESTACLE_INCLUDES) $<


$(BUILD_DIR)/libpestacle/src/%.deps: libpestacle/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(LIBPESTACLE_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(LIBPESTACLE_SOURCES))


# ------ ffmpeg plugin --------------------------------------------------------

PESTACLE_FFMPEG_PLUGIN_SOURCES := $(shell find plugins/ffmpeg/src -name '*.c')
PESTACLE_FFMPEG_PLUGIN_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_FFMPEG_PLUGIN_SOURCES:%.c=%.o))


$(BUILD_DIR)/plugins/$(PESTACLE_FFMPEG_PLUGIN_FILENAME): $(PESTACLE_FFMPEG_PLUGIN_OBJS)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^ $(PESTACLE_FFMPEG_PLUGIN_LIBS)


$(BUILD_DIR)/plugins/ffmpeg/src/%.o: plugins/ffmpeg/src/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(PESTACLE_FFMPEG_PLUGIN_INCLUDES) $<


$(BUILD_DIR)/plugins/ffmpeg/src/%.deps: plugins/ffmpeg/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(PESTACLE_FFMPEG_PLUGIN_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(PESTACLE_FFMPEG_PLUGIN_SOURCES))


# ------ Unit testing ---------------------------------------------------------

test: \
$(BUILD_DIR)/test_math

$(BUILD_DIR)/test_math: $(BUILD_DIR)/test/math.o $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
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

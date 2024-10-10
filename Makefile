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


$(BUILD_DIR)/tools/pestacle/%.o: tools/pestacle/%.c
	@mkdir -p $(shell basename $(dir $(abspath $(dir $@))))
	$(CC) -o $@ -c $(CFLAGS) $(PESTACLE_INCLUDES) $(LIBPESTACLE_INCLUDES) $<


$(BUILD_DIR)/tools/pestacle/src/%.deps: tools/pestacle/src/%.c
	@mkdir -p $(BUILD_DIR)/tools/pestacle/src
	$(CC) $(PESTACLE_INCLUDES) -MM -MG -MT$(patsubst tools/pestacle/src/%.c, $(BUILD_DIR)/tools/pestacle/src/%.o, $^) -MF $@ $^

$(BUILD_DIR)/tools/pestacle/src/nodes/%.deps: tools/pestacle/src/nodes/%.c
	@mkdir -p $(BUILD_DIR)/tools/pestacle/src/nodes
	$(CC) $(PESTACLE_INCLUDES) -MM -MG -MT$(patsubst tools/pestacle/src/nodes/%.c, $(BUILD_DIR)/tools/pestacle/src/nodes/%.o, $^) -MF $@ $^

$(BUILD_DIR)/tools/pestacle/src/scopes/%.deps: tools/pestacle/src/scopes/%.c
	@mkdir -p $(BUILD_DIR)/tools/pestacle/src/scopes
	$(CC) $(PESTACLE_INCLUDES) -MM -MG -MT$(patsubst tools/pestacle/src/scopes/%.c, $(BUILD_DIR)/tools/pestacle/src/scopes/%.o, $^) -MF $@ $^


-include $(patsubst tools/pestacle/src/%.c, $(BUILD_DIR)/%.deps, $(wildcard tools/pestacle/src/*.c))
-include $(patsubst tools/pestacle/src/nodes/%.c, $(BUILD_DIR)/tools/pestacle/src/nodes/%.deps, $(wildcard tools/pestacle/src/nodes/*.c))
-include $(patsubst tools/pestacle/src/scopes/%.c, $(BUILD_DIR)/tools/pestacle/src/scopes/%.deps, $(wildcard tools/pestacle/src/scopes/*.c))


# ------ libpestacle ---------------------------------------------------------

LIBPESTACLE_SOURCES := $(shell find libpestacle/src -name '*.c')
LIBPESTACLE_OBJS := $(addprefix $(BUILD_DIR)/,$(LIBPESTACLE_SOURCES:%.c=%.o))


$(BUILD_DIR)/$(LIBPESTACLE_FILENAME): $(LIBPESTACLE_OBJS)
	$(CC) -shared -o $@ $^ $(SDL2_LIBS)


$(BUILD_DIR)/libpestacle/src/%.o: libpestacle/src/%.c
	@mkdir -p $(shell basename $(dir $(abspath $(dir $@))))
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(LIBPESTACLE_INCLUDES) $<


$(BUILD_DIR)/libpestacle/src/%.deps: libpestacle/src/%.c
	@mkdir -p $(BUILD_DIR)/libpestacle/src
	$(CC) $(LIBPESTACLE_INCLUDES) -MM -MG -MT$(patsubst libpestacle/src/%.c, $(BUILD_DIR)/libpestacle/src/%.o, $^) -MF $@ $^

$(BUILD_DIR)/libpestacle/src/math/%.deps: libpestacle/src/math/%.c
	@mkdir -p $(BUILD_DIR)/libpestacle/src/math
	$(CC) $(LIBPESTACLE_INCLUDES) -MM -MG -MT$(patsubst libpestacle/src/math/%.c, $(BUILD_DIR)/libpestacle/src/math/%.o, $^) -MF $@ $^

$(BUILD_DIR)/libpestacle/src/parser/%.deps: libpestacle/src/parser/%.c
	@mkdir -p $(BUILD_DIR)/libpestacle/src/parser
	$(CC) $(LIBPESTACLE_INCLUDES) -MM -MG -MT$(patsubst libpestacle/src/parser/%.c, $(BUILD_DIR)/libpestacle/src/parser/%.o, $^) -MF $@ $^


-include $(patsubst libpestacle/src/%.c, $(BUILD_DIR)/libpestacle/src/%.deps, $(wildcard libpestacle/src/*.c))
-include $(patsubst libpestacle/src/math/%.c, $(BUILD_DIR)/libpestacle/src/math/%.deps, $(wildcard libpestacle/src/math/*.c))
-include $(patsubst libpestacle/src/parser/%.c, $(BUILD_DIR)/libpestacle/src/parser/%.deps, $(wildcard libpestacle/src/parser/*.c))


# ------ ffmpeg plugin --------------------------------------------------------

PESTACLE_FFMPEG_PLUGIN_SOURCES := $(shell find plugins/ffmpeg/src -name '*.c')
PESTACLE_FFMPEG_PLUGIN_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_FFMPEG_PLUGIN_SOURCES:%.c=%.o))


$(BUILD_DIR)/plugins/$(PESTACLE_FFMPEG_PLUGIN_FILENAME): $(PESTACLE_FFMPEG_PLUGIN_OBJS)
	@mkdir -p $(BUILD_DIR)/plugins
	$(CC) -shared -o $@ $^ $(PESTACLE_FFMPEG_PLUGIN_LIBS)


$(BUILD_DIR)/plugins/ffmpeg/%.o: plugins/ffmpeg/%.c
	@mkdir -p $(shell basename $(dir $(abspath $(dir $@))))
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(PESTACLE_FFMPEG_PLUGIN_INCLUDES) $<


$(BUILD_DIR)/plugins/ffmpeg/src/%.deps: plugins/ffmpeg/src/%.c
	@mkdir -p $(BUILD_DIR)/plugins/ffmpeg/src
	$(CC) $(PESTACLE_FFMPEG_PLUGIN_INCLUDES) -MM -MG -MT$(patsubst plugins/ffmpeg/src/%.c, $(BUILD_DIR)/plugins/ffmpeg/src/%.o, $^) -MF $@ $^


-include $(patsubst plugins/ffmpeg/src/%.c, $(BUILD_DIR)/plugins/ffmpeg/src/%.deps, $(wildcard plugins/ffmpeg/src/*.c))


# ------ Unit testing ---------------------------------------------------------

test: \
$(BUILD_DIR)/test_math

$(BUILD_DIR)/test_math: $(BUILD_DIR)/test/math.o $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	@mkdir -p $(BUILD_DIR)/test
	$(CC) -o $@ $< $(PESTACLE_LIBS)

$(BUILD_DIR)/test/%.o: test/%.c
	@mkdir -p $(BUILD_DIR)/test
	$(CC) -o $@ -c $(CFLAGS) $(LIBPESTACLE_INCLUDES) $(TEST_INCLUDES) $<


$(BUILD_DIR)/test/%.deps: test/%.c
	@mkdir -p $(BUILD_DIR)/test
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst test/%.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^

-include $(patsubst test/%.c, $(BUILD_DIR)/test/%.deps, $(wildcard test/*.c))


# ------ Misc targets ---------------------------------------------------------

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all test clean

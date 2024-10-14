include config.mk

TARGETS = \
$(BUILD_DIR)/$(LIBPESTACLE_FILENAME) \
$(BUILD_DIR)/$(PESTACLE_FILENAME) \
$(BUILD_DIR)/plugins/$(PESTACLE_FFMPEG_PLUGIN_FILENAME) \
$(BUILD_DIR)/plugins/$(PESTACLE_ARDUCAM_PLUGIN_FILENAME)

all: $(TARGETS)


# ------ pestacle -------------------------------------------------------------

PESTACLE_DIR := tools/pestacle/src
PESTACLE_SOURCES := $(shell find $(PESTACLE_DIR) -name '*.c')
PESTACLE_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_SOURCES:%.c=%.o))


$(BUILD_DIR)/$(PESTACLE_FILENAME): $(PESTACLE_OBJS) $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	$(CC) -o $@ $(PESTACLE_OBJS) $(PESTACLE_LIBS)


$(BUILD_DIR)/$(PESTACLE_DIR)/%.o: $(PESTACLE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $(CFLAGS) $(PESTACLE_INCLUDES) $(LIBPESTACLE_INCLUDES) $<


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


# ------ ffmpeg plugin --------------------------------------------------------

PESTACLE_FFMPEG_PLUGIN_DIR := plugins/ffmpeg/src
PESTACLE_FFMPEG_PLUGIN_SOURCES := $(shell find $(PESTACLE_FFMPEG_PLUGIN_DIR) -name '*.c')
PESTACLE_FFMPEG_PLUGIN_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_FFMPEG_PLUGIN_SOURCES:%.c=%.o))


$(BUILD_DIR)/plugins/$(PESTACLE_FFMPEG_PLUGIN_FILENAME): $(PESTACLE_FFMPEG_PLUGIN_OBJS)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^ $(PESTACLE_FFMPEG_PLUGIN_LIBS)


$(BUILD_DIR)/$(PESTACLE_FFMPEG_PLUGIN_DIR)/%.o: $(PESTACLE_FFMPEG_PLUGIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(PESTACLE_FFMPEG_PLUGIN_INCLUDES) $<


$(BUILD_DIR)/$(PESTACLE_FFMPEG_PLUGIN_DIR)/%.deps: $(PESTACLE_FFMPEG_PLUGIN_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(PESTACLE_FFMPEG_PLUGIN_INCLUDES) -MM -MG -MT$(patsubst %.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.c, $(BUILD_DIR)/%.deps, $(PESTACLE_FFMPEG_PLUGIN_SOURCES))


# ------ arducam plugin -------------------------------------------------------

PESTACLE_ARDUCAM_PLUGIN_DIR := plugins/arducam/src
PESTACLE_ARDUCAM_PLUGIN_SOURCES := $(shell find $(PESTACLE_ARDUCAM_PLUGIN_DIR) -name '*.cpp')
PESTACLE_ARDUCAM_PLUGIN_OBJS := $(addprefix $(BUILD_DIR)/,$(PESTACLE_ARDUCAM_PLUGIN_SOURCES:%.cpp=%.o))


$(BUILD_DIR)/plugins/$(PESTACLE_ARDUCAM_PLUGIN_FILENAME): $(PESTACLE_ARDUCAM_PLUGIN_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) -shared -o $@ $^ $(PESTACLE_ARDUCAM_PLUGIN_LIBS)


$(BUILD_DIR)/$(PESTACLE_ARDUCAM_PLUGIN_DIR)/%.o: $(PESTACLE_ARDUCAM_PLUGIN_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -o $@ -c -fPIC $(CXXFLAGS) $(PESTACLE_ARDUCAM_PLUGIN_INCLUDES) $<


$(BUILD_DIR)/$(PESTACLE_ARDUCAM_PLUGIN_DIR)/%.deps: $(PESTACLE_ARDUCAM_PLUGIN_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(PESTACLE_ARDUCAM_PLUGIN_INCLUDES) -MM -MG -MT$(patsubst %.cpp, $(BUILD_DIR)/%.o, $^) -MF $@ $^


-include $(patsubst %.cpp, $(BUILD_DIR)/%.deps, $(PESTACLE_ARDUCAM_PLUGIN_SOURCES))


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

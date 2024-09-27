include config.mk

TARGETS = \
$(BUILD_DIR)/$(LIBPESTACLE_FILENAME) \
$(BUILD_DIR)/$(PESTACLE_FILENAME)


all: $(TARGETS)


# ------ pestacle -------------------------------------------------------------

PESTACLE_OBJS = \
$(BUILD_DIR)/tools/pestacle/main.o \
$(BUILD_DIR)/tools/pestacle/argtable3.o \
$(BUILD_DIR)/tools/pestacle/picture.o \
$(BUILD_DIR)/tools/pestacle/nodes/gradient_map.o \
$(BUILD_DIR)/tools/pestacle/nodes/heat_diffusion.o \
$(BUILD_DIR)/tools/pestacle/nodes/lightness.o \
$(BUILD_DIR)/tools/pestacle/nodes/matrix_resize.o \
$(BUILD_DIR)/tools/pestacle/nodes/picture.o \
$(BUILD_DIR)/tools/pestacle/nodes/surface_blend.o \
$(BUILD_DIR)/tools/pestacle/nodes/surface_resize.o \
$(BUILD_DIR)/tools/pestacle/nodes/video.o \
$(BUILD_DIR)/tools/pestacle/scopes/root.o \
$(BUILD_DIR)/tools/pestacle/scopes/window.o

$(BUILD_DIR)/$(PESTACLE_FILENAME): $(PESTACLE_OBJS) $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	$(CC) -o $@ $(PESTACLE_OBJS) $(LIBS)


$(BUILD_DIR)/tools/pestacle/%.o: tools/pestacle/src/%.c
	@mkdir -p $(BUILD_DIR)/tools/pestacle
	$(CC) -o $@ -c $(CFLAGS) $(PESTACLE_INCLUDES) $(LIBPESTACLE_INCLUDES) $<

$(BUILD_DIR)/tools/pestacle/nodes/%.o: tools/pestacle/src/nodes/%.c
	@mkdir -p $(BUILD_DIR)/tools/pestacle/nodes
	$(CC) -o $@ -c $(CFLAGS) $(PESTACLE_INCLUDES) $(LIBPESTACLE_INCLUDES) $<

$(BUILD_DIR)/tools/pestacle/scopes/%.o: tools/pestacle/src/scopes/%.c
	@mkdir -p $(BUILD_DIR)/tools/pestacle/scopes
	$(CC) -o $@ -c $(CFLAGS) $(PESTACLE_INCLUDES) $(LIBPESTACLE_INCLUDES) $<


-include $(patsubst tools/pestacle/src/%.c, $(BUILD_DIR)/%.deps, $(wildcard tools/pestacle/src/*.c))
-include $(patsubst tools/pestacle/src/nodes/%.c, $(BUILD_DIR)/tools/pestacle/src/nodes/%.deps, $(wildcard tools/pestacle/src/nodes/*.c))
-include $(patsubst tools/pestacle/src/scopes/%.c, $(BUILD_DIR)/tools/pestacle/src/scopes/%.deps, $(wildcard tools/pestacle/src/scopes/*.c))


# ------ libpestacle ---------------------------------------------------------

$(BUILD_DIR)/$(LIBPESTACLE_FILENAME): \
$(BUILD_DIR)/libpestacle/errors.o \
$(BUILD_DIR)/libpestacle/memory.o \
$(BUILD_DIR)/libpestacle/strings.o \
$(BUILD_DIR)/libpestacle/string_list.o \
$(BUILD_DIR)/libpestacle/input_buffer.o \
$(BUILD_DIR)/libpestacle/dict.o \
$(BUILD_DIR)/libpestacle/stack.o \
$(BUILD_DIR)/libpestacle/array_ops.o \
$(BUILD_DIR)/libpestacle/vector.o \
$(BUILD_DIR)/libpestacle/matrix.o \
$(BUILD_DIR)/libpestacle/parameter.o \
$(BUILD_DIR)/libpestacle/node.o \
$(BUILD_DIR)/libpestacle/scope.o \
$(BUILD_DIR)/libpestacle/graph.o \
$(BUILD_DIR)/libpestacle/graph_profile.o \
$(BUILD_DIR)/libpestacle/window_manager.o \
$(BUILD_DIR)/libpestacle/parser/lexer.o \
$(BUILD_DIR)/libpestacle/parser/parser.o
	$(CC) -shared -o $@ $^ $(SDL2_LIBS)


$(BUILD_DIR)/libpestacle/%.o: libpestacle/src/%.c
	@mkdir -p $(BUILD_DIR)/libpestacle
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(LIBPESTACLE_INCLUDES) $<

$(BUILD_DIR)/libpestacle/parser/%.o: libpestacle/src/parser/%.c
	@mkdir -p $(BUILD_DIR)/libpestacle/parser
	$(CC) -o $@ -c -fPIC $(CFLAGS) $(SDL2_CFLAGS) $(LIBPESTACLE_INCLUDES) $<


$(BUILD_DIR)/libpestacle/%.deps: libpestacle/src/%.c
	@mkdir -p $(BUILD_DIR)/libpestacle
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst libpestacle/src/%.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^

$(BUILD_DIR)/libpestacle/parser/%.deps: libpestacle/src/parser/%.c
	@mkdir -p $(BUILD_DIR)/libpestacle/parser
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst libpestacle/src/parser/%.c, $(BUILD_DIR)/libpestacle/parser/%.o, $^) -MF $@ $^


-include $(patsubst libpestacle/src/%.c, $(BUILD_DIR)/%.deps, $(wildcard libpestacle/src/*.c))
-include $(patsubst libpestacle/src/parser/%.c, $(BUILD_DIR)/libpestacle/parser/%.deps, $(wildcard libpestacle/src/parser/*.c))


# ------ Unit testing ---------------------------------------------------------

test: \
$(BUILD_DIR)/test/math

$(BUILD_DIR)/test/math: $(BUILD_DIR)/test/math.o $(BUILD_DIR)/$(LIBPESTACLE_FILENAME)
	@mkdir -p $(BUILD_DIR)/test
	$(CC) -o $@ $< $(LIBS)

$(BUILD_DIR)/test/%.o: test/%.c
	@mkdir -p $(BUILD_DIR)/test
	$(CC) -o $@ -c $(CFLAGS) $(LIBPESTACLE_INCLUDES) $(TEST_INCLUDES) $<

-include $(patsubst test/%.c, $(BUILD_DIR)/test/%.deps, $(wildcard test/*.c))


# ------ Misc targets ---------------------------------------------------------

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all clean

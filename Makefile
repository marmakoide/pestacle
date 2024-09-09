include config.mk

TARGET = pestacle
BUILD_DIR = ./build


all: $(TARGET)

$(TARGET): \
$(BUILD_DIR)/errors.o \
$(BUILD_DIR)/memory.o \
$(BUILD_DIR)/strings.o \
$(BUILD_DIR)/input_buffer.o \
$(BUILD_DIR)/parser/lexer.o \
$(BUILD_DIR)/parser/parser.o \
$(BUILD_DIR)/dict.o \
$(BUILD_DIR)/stack.o \
$(BUILD_DIR)/array_ops.o \
$(BUILD_DIR)/vector.o \
$(BUILD_DIR)/matrix.o \
$(BUILD_DIR)/root-domain.o \
$(BUILD_DIR)/event.o \
$(BUILD_DIR)/spng.o \
$(BUILD_DIR)/picture.o \
$(BUILD_DIR)/parameter.o \
$(BUILD_DIR)/node.o \
$(BUILD_DIR)/domain.o \
$(BUILD_DIR)/graph.o \
$(BUILD_DIR)/window_manager.o \
$(BUILD_DIR)/domains/window.o \
$(BUILD_DIR)/nodes/gradient_map.o \
$(BUILD_DIR)/nodes/heat_diffusion.o \
$(BUILD_DIR)/nodes/lightness.o \
$(BUILD_DIR)/nodes/matrix_resize.o \
$(BUILD_DIR)/nodes/mouse_motion.o \
$(BUILD_DIR)/nodes/picture.o \
$(BUILD_DIR)/nodes/surface_blend.o \
$(BUILD_DIR)/nodes/surface_resize.o \
$(BUILD_DIR)/nodes/video.o \
$(BUILD_DIR)/main.o
	$(CC) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.deps: src/%.c
	@mkdir --parents $(BUILD_DIR)
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst src/%.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^

$(BUILD_DIR)/parser/%.deps: src/parser/%.c
	@mkdir --parents $(BUILD_DIR)/parser
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst src/parser/%.c, $(BUILD_DIR)/parser/%.o, $^) -MF $@ $^

$(BUILD_DIR)/nodes/%.deps: src/nodes/%.c
	@mkdir --parents $(BUILD_DIR)/nodes
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst src/nodes/%.c, $(BUILD_DIR)/nodes/%.o, $^) -MF $@ $^

$(BUILD_DIR)/domains/%.deps: src/domains/%.c
	@mkdir --parents $(BUILD_DIR)/domains
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst src/domains/%.c, $(BUILD_DIR)/domains/%.o, $^) -MF $@ $^

$(BUILD_DIR)/%.o: src/%.c
	@mkdir --parents $(BUILD_DIR)
	$(CC) -o $@ -c $(CFLAGS) $<

$(BUILD_DIR)/parser/%.o: src/parser/%.c
	@mkdir --parents $(BUILD_DIR)/parser
	$(CC) -o $@ -c $(CFLAGS) $<

$(BUILD_DIR)/nodes/%.o: src/nodes/%.c
	@mkdir --parents $(BUILD_DIR)/nodes
	$(CC) -o $@ -c $(CFLAGS) $<

$(BUILD_DIR)/domains/%.o: src/domains/%.c
	@mkdir --parents $(BUILD_DIR)/domains
	$(CC) -o $@ -c $(CFLAGS) $<

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET)

.PHONY: all clean

-include $(patsubst src/%.c, $(BUILD_DIR)/%.deps, $(wildcard src/*.c))
-include $(patsubst src/parser/%.c, $(BUILD_DIR)/parser/%.deps, $(wildcard src/parser/*.c))
-include $(patsubst src/nodes/%.c, $(BUILD_DIR)/nodes/%.deps, $(wildcard src/nodes/*.c))
-include $(patsubst src/domains/%.c, $(BUILD_DIR)/domains/%.deps, $(wildcard src/domains/*.c))

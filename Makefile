include config.mk

TARGET = pestacle
BUILD_DIR = ./build


all: $(TARGET)

$(TARGET): \
$(BUILD_DIR)/array_ops.o \
$(BUILD_DIR)/vector.o \
$(BUILD_DIR)/matrix.o \
$(BUILD_DIR)/event.o \
$(BUILD_DIR)/spng.o \
$(BUILD_DIR)/image.o \
$(BUILD_DIR)/source.o \
$(BUILD_DIR)/renderer.o \
$(BUILD_DIR)/renderers/gradient.o \
$(BUILD_DIR)/renderers/linear_blend.o \
$(BUILD_DIR)/sources/heat_diffusion.o \
$(BUILD_DIR)/animation.o \
$(BUILD_DIR)/main.o
	$(CC) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.deps: src/%.c
	@mkdir --parents $(BUILD_DIR)
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst src/%.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^

$(BUILD_DIR)/sources/%.deps: src/sources/%.c
	@mkdir --parents $(BUILD_DIR)/sources
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst src/sources/%.c, $(BUILD_DIR)/sources/%.o, $^) -MF $@ $^

$(BUILD_DIR)/renderers/%.deps: src/renderers/%.c
	@mkdir --parents $(BUILD_DIR)/renderers
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst src/renderers/%.c, $(BUILD_DIR)/renderers/%.o, $^) -MF $@ $^

$(BUILD_DIR)/%.o: src/%.c
	@mkdir --parents $(BUILD_DIR)
	$(CC) -o $@ -c $(CFLAGS) $<

$(BUILD_DIR)/sources/%.o: src/sources/%.c
	@mkdir --parents $(BUILD_DIR)/sources
	$(CC) -o $@ -c $(CFLAGS) $<

$(BUILD_DIR)/renderers/%.o: src/renderers/%.c
	@mkdir --parents $(BUILD_DIR)/renderers
	$(CC) -o $@ -c $(CFLAGS) $<

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET)

.PHONY: all clean

-include $(patsubst src/%.c, $(BUILD_DIR)/%.deps, $(wildcard src/*.c))
-include $(patsubst src/sources/%.c, $(BUILD_DIR)/sources/%.deps, $(wildcard src/sources/*.c))
-include $(patsubst src/renderers/%.c, $(BUILD_DIR)/renderers/%.deps, $(wildcard src/renderers/*.c))

include config.mk

TARGET = pestacle
BUILD_DIR = ./build


all: $(TARGET)

$(TARGET): \
$(BUILD_DIR)/array_ops.o \
$(BUILD_DIR)/vector.o \
$(BUILD_DIR)/matrix.o \
$(BUILD_DIR)/spng.o \
$(BUILD_DIR)/image.o \
$(BUILD_DIR)/animation.o \
$(BUILD_DIR)/main.o
	$(CC) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.deps: src/%.c
	@mkdir --parents $(BUILD_DIR)
	$(CC) $(INCLUDES) -MM -MG -MT$(patsubst src/%.c, $(BUILD_DIR)/%.o, $^) -MF $@ $^

$(BUILD_DIR)/%.o: src/%.c
	@mkdir --parents $(BUILD_DIR)
	$(CC) -o $@ -c $(CFLAGS) $<

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET)

.PHONY: all clean

-include $(patsubst src/%.c, $(BUILD_DIR)/%.deps, $(wildcard src/*.c))

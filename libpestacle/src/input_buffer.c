#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pestacle/errors.h>
#include <pestacle/input_buffer.h>


void
InputBuffer_init(
	InputBuffer* self,
    FILE* file
) {
	self->file = file;
	self->head = self->buffer.value;
	self->end = self->buffer.value;

	InputBuffer_char* item = self->buffer.value;
	for(size_t i = INPUT_BUFFER_SIZE; i != 0; --i, ++item)
		*item = 0;

	InputBuffer_next(self);
}


void
InputBuffer_next(
	InputBuffer* self
) {
	if (self->head >= self->end) {
		// Read data
		size_t read_char_count =
			fread(self->buffer.c, sizeof(char), INPUT_BUFFER_SIZE, self->file);

		// Convert input buffer data from char to InputBuffer_char
		if (read_char_count > 0) {
			char* src = self->buffer.c + read_char_count - 1;
			InputBuffer_char* dst = self->buffer.value + read_char_count - 1;
			for(size_t i = read_char_count; i != 0; --i, --src, --dst)
				*dst = *src;
		}

		// Check if we reached the end of the file
		if (read_char_count != INPUT_BUFFER_SIZE) {
			if (ferror(self->file))
				handle_input_read_error();

			self->buffer.value[read_char_count++] = EOF;
		}

		// Reset head and end pointers
		self->head = self->buffer.value;
		self->end = self->buffer.value + read_char_count - 1;
	}
	else
		self->head += 1;
}


InputBuffer_char
InputBuffer_get(
	const InputBuffer* self
) {
	assert(self != 0);

	return *(self->head);
}

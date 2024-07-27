#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "errors.h"
#include "input_buffer.h"


void
InputBuffer_init(InputBuffer* self,
                 FILE* file) {
	self->file = file;
    self->head = self->buffer;
    self->end = self->buffer;
    memset(self->buffer, 0, INPUT_BUFFER_SIZE);

    InputBuffer_next(self);
}


void
InputBuffer_next(InputBuffer* self) {
	if (self->head >= self->end) {
    	size_t read_char_count =
        	fread(self->buffer, sizeof(char), INPUT_BUFFER_SIZE, self->file);

		if (read_char_count != INPUT_BUFFER_SIZE) {
			if (ferror(self->file))
				handle_input_read_error();
				
			self->buffer[read_char_count++] = EOF;
		}
		
		self->head = self->buffer;
		self->end = self->buffer + read_char_count - 1;
	}
	else
		self->head += 1;
}


char
InputBuffer_get(const InputBuffer* self) {
	assert(self != 0);

	return *(self->head);
}
#ifndef PESTACLE_INPUT_BUFFER_H
#define PESTACLE_INPUT_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
  Buffered file reader : provides bytes from a file one by one, but the
  the file is read page per page.

  After initialization, InputBuffer_get will returns the 1st byte of the file.

  On x86 architecture, char type is signed, while it's unsigned on ARM 
  architecture. The implementation allow to use InputBuffer without any
  differences.
 *****************************************************************************/


#include <stdio.h>

typedef int InputBuffer_char;

#define INPUT_BUFFER_SIZE 4096 // Ideally, the size of a memory page

typedef union {
	char c[INPUT_BUFFER_SIZE];
	InputBuffer_char value[INPUT_BUFFER_SIZE];
} InputBufferData;

typedef struct {
	FILE* file;
	InputBuffer_char* head;
	InputBuffer_char* end;
	InputBufferData buffer;
} InputBuffer;


extern void
InputBuffer_init(
	InputBuffer* self,
	FILE* file
);


extern void
InputBuffer_next(
	InputBuffer* self
);


extern InputBuffer_char
InputBuffer_get(
	const InputBuffer* self
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_INPUT_BUFFER_H */
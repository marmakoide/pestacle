#ifndef PESTACLE_INPUT_BUFFER_H
#define PESTACLE_INPUT_BUFFER_H

/******************************************************************************
  Buffered file reader : provides bytes from a file one by one, but the
  the file is read page per page.

  After initialization, InputBuffer_get will returns the 1st byte of the file.
 *****************************************************************************/


#include <stdio.h>

#define INPUT_BUFFER_SIZE 4096 // Ideally, the size of a memory page

typedef struct {
    FILE* file;
    char* head;
    char* end;
    char buffer[INPUT_BUFFER_SIZE];
} InputBuffer;


extern void
InputBuffer_init(InputBuffer* self,
                 FILE* file);


extern void
InputBuffer_next(InputBuffer* self);


extern char
InputBuffer_get(const InputBuffer* self);


#endif /* PESTACLE_INPUT_BUFFER_H */
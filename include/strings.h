#ifndef PESTACLE_STRING_H
#define PESTACLE_STRING_H

/******************************************************************************
  Implementation of strings.
  The main reason to not just use raw char arrays is to avoid using functions
  like strcpy which makes buffer overuns very easy to run into.
 *****************************************************************************/


#include <stddef.h>
#include <stdint.h>


typedef struct {
	char* data;
	size_t len;
} String;


extern void
String_clone(
	String* self,
	const String* other
);


extern void
String_destroy(
	String* self
);


extern uint32_t
String_djb_hash(
	const String *restrict str
);


extern int
String_equals(
	const String *restrict self,
	const String *restrict other
);


#endif /* PESTACLE_STRING_H */
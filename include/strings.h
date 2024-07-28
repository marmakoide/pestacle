#ifndef PESTACLE_STRING_H
#define PESTACLE_STRING_H

/******************************************************************************
  Implementation of strings.
  The main reason to not just use raw char arrays is to avoid using functions
  like strcpy which makes buffer overuns very easy to run into.
 *****************************************************************************/


#include <stddef.h>


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


extern int
String_equals(
	const String* self,
	const String* other
);


#endif /* PESTACLE_STRING_H */
#ifndef PESTACLE_STRING_H
#define PESTACLE_STRING_H

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
#ifndef PESTACLE_STRING_H
#define PESTACLE_STRING_H

#include <stddef.h>


typedef struct {
	char* data;
	size_t len;
} String;


extern int
string_equals(
	const String* self,
	const String* other
);


#endif /* PESTACLE_STRING_H */
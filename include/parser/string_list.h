#ifndef PESTACLE_STRING_LIST_H
#define PESTACLE_STRING_LIST_H

#include "strings.h"


typedef struct {
	size_t logical_len;
	size_t physical_len;
	String* items;
} StringList;


extern void
StringList_init(
	StringList* self
);


extern void
StringList_destroy(
	StringList* self
);


extern void
StringList_clear(
	StringList* self
);


extern bool
StringList_empty(
	const StringList* self
);


extern size_t
StringList_length(
	const StringList* self
);


extern const String*
StringList_at(
	const StringList* self,
	size_t i
);


extern void
StringList_append(
	StringList* self,
	const String* str
);


#endif /* PESTACLE_STRING_LIST_H */
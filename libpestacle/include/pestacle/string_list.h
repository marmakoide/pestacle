#ifndef PESTACLE_STRING_LIST_H
#define PESTACLE_STRING_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>


typedef struct {
	size_t logical_len;
	size_t physical_len;
	char** items;
} StringList;


extern void
StringList_print(
	const StringList* self,
	FILE* out
);


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


extern void
StringList_copy(
	StringList* self,
	const StringList* src
);


extern bool
StringList_empty(
	const StringList* self
);


extern size_t
StringList_length(
	const StringList* self
);


extern const char**
StringList_items(
	const StringList* self
);


extern const char*
StringList_at(
	const StringList* self,
	size_t i
);


extern void
StringList_append(
	StringList* self,
	const char* str
);


extern char*
StringList_join(
	const StringList* self,
	char c
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_STRING_LIST_H */

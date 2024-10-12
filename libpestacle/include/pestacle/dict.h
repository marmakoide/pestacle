#ifndef PESTACLE_DICT_H
#define PESTACLE_DICT_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
  Implementation of a dictionary aka hashmap with strings as keys
    - Linear probing
    - Open addressing
    - Lazy deletion
 *****************************************************************************/

#include <stddef.h>
#include <stdbool.h>

#define DICT_ENTRY_TOMBSTONE_VALUE ((void*)-1)

typedef struct {
    const char* key;
    void* value;
} DictEntry;


typedef struct {
    size_t size;
    size_t key_count;
    DictEntry* entries;
} Dict;


typedef struct {
	size_t key_count;
	DictEntry* entry;
} DictIterator;


extern void
DictIterator_init(
	DictIterator* self,
	Dict* dict
);


extern void
Dict_destroy(
	Dict* self
);


extern void
DictIterator_next(
	DictIterator* self
);


extern bool
DictIterator_has_next(
	const DictIterator* self
);


extern void
Dict_init(
	Dict* self
);


extern void
Dict_clear(
	Dict* self
);


extern void
Dict_clone(
	Dict* dst,
	Dict* src
);


extern DictEntry*
Dict_pick(
	Dict* self
);


extern DictEntry*
Dict_find(
	Dict* self,
	const char* key
);


extern DictEntry*
Dict_insert(
	Dict* self,
	const char* key
);


extern void
Dict_erase(
	Dict* self,
	DictEntry* entry
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_DICT_H */

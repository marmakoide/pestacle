#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <pestacle/strings.h>
#include <pestacle/memory.h>
#include <pestacle/dict.h>


void
DictIterator_next(
	DictIterator* self
) {
	assert(self);
	
	if (self->key_count == 0)
		self->entry = 0;
	else {
		for(self->entry += 1; self->entry->key == 0; self->entry += 1);
		self->key_count	-= 1;
	}
}


void
DictIterator_init(
	DictIterator* self,
    Dict* dict
) {
	assert(self);
	assert(dict);
    
	self->key_count = dict->key_count;

	if (self->key_count == 0)
		self->entry = 0;
	else {
		for(self->entry = dict->entries; self->entry->key == 0; self->entry += 1);
		self->key_count -= 1;
	}
}


bool
DictIterator_has_next(
	const DictIterator* self
) {
	assert(self);
	
	return (self->key_count != 0) || (self->entry != 0);
}


void
DictEntry_init(
	DictEntry* self
) {
	assert(self);
	
	self->key = 0;
	self->value = 0;
}


void
Dict_init(
	Dict* self
) {
	assert(self);

	self->size = 16;
	self->key_count = 0;

	self->entries =
		(DictEntry*)checked_calloc(self->size, sizeof(DictEntry));

	DictEntry* entry = self->entries;
	for(size_t i = self->size; i != 0; --i, ++entry)
		DictEntry_init(entry);
}


void
Dict_clear(
	Dict* self
) {
	assert(self);

	self->key_count = 0;

	DictEntry* entry = self->entries;
	for(size_t i = self->size; i != 0; --i, ++entry)
		DictEntry_init(entry);
}


void
Dict_clone(
	Dict* dst,
    Dict* src
) {
	assert(dst);
	assert(src);

	dst->size = src->size;
	dst->key_count = src->key_count;

	dst->entries = (DictEntry*)checked_calloc(dst->size, sizeof(DictEntry));
	memcpy(dst->entries, src->entries, dst->size * sizeof(DictEntry));
}


void
Dict_destroy(
	Dict* self
) {
	assert(self);
	
	self->size = 0;
	self->key_count = 0;

	if (self->entries) {
		free(self->entries);
		self->entries = 0;
	}
}


static DictEntry*
Dict_probe(
	Dict* self,
    const char* key
) {
	assert(self);
	assert(key);

	// Initial search locus
	size_t i = (size_t)djb_hash(key);

	// Linear search
	for(size_t j = 0; j < self->size; ++j) {
		DictEntry* entry = self->entries + ((i + j) % self->size);

		// If the entry is not a tombstome
		if (entry->value != DICT_ENTRY_TOMBSTONE_VALUE)
			// Check if the entry is unused or matching the key
			if ((entry->key == 0) || (strcmp(entry->key, key) == 0))
				return entry;
	}

	// Job done
	return 0;
}


static void
Dict_resize(
	Dict* self,
    size_t new_size
) {
	assert(self);

	DictEntry* entry;

	// Pointer on current dict entries
	size_t old_size = self->size;
	DictEntry* old_entries = self->entries;
    
	// Allocate and initialize the new entries
	self->size = new_size;
	self->entries = (DictEntry*)checked_calloc(self->size, sizeof(DictEntry));

	entry = self->entries;
	for(size_t i = self->size; i != 0; --i, ++entry)
		DictEntry_init(entry);
    
	// Populate the new entries
	entry = old_entries;
	for(size_t i = old_size; i != 0; --i, ++entry)
		if (entry->key) {
			DictEntry* dst = Dict_probe(self, entry->key);
			dst->key = entry->key;
			dst->value = entry->value;
		}
    
	// Job done
	free(old_entries);
}


DictEntry*
Dict_pick(
	Dict* self
) {
	assert(self);

	DictEntry* entry = self->entries;
	for(size_t i = self->size; i != 0; --i, ++entry)
		if (entry->key)
			return entry;

	return 0;
}


DictEntry*
Dict_find(
	Dict* self,
    const char* key
) {
	assert(self);
	assert(key);

	DictEntry* entry = Dict_probe(self, key);

	if ((!entry) || (entry->key != 0))
		return entry;

	return 0;
}


DictEntry*
Dict_insert(
	Dict* self,
    const char* key
) {
	assert(self);
	assert(key);

	// Stretch the dictionary to enforce the load factor
	size_t new_size = self->size;
	while(new_size / 2 < self->key_count + 1)
		new_size *= 2;
        
	if (new_size != self->size)
		Dict_resize(self, new_size);

	// Probe the dictionary
	DictEntry* entry = Dict_probe(self, key);

    // If no entry found, it means the dictionary is filled with tombstone
	if (!entry) {
		// Rebuild the dictionary
		Dict_resize(self, self->size);
		
		// Probe the dictionary again
		entry = Dict_probe(self, key);
	}

	// Return 0 if entry already occupied
	if (entry->key)
		return 0;

	// Setup the entry
	entry->key = key;
	entry->value = 0;

	// Job done
	self->key_count += 1;
	return entry;
}


void
Dict_erase(
	Dict* self,
    DictEntry* entry
) {
	assert(self);
	assert(entry);
	assert(self->key_count > 0);

	self->key_count -= 1;
	entry->key = 0;
	entry->value = DICT_ENTRY_TOMBSTONE_VALUE;
}

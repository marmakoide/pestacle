#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pestacle/strings.h>
#include <pestacle/memory.h>


uint32_t
djb_hash(
	const char* str
) {
	// Classic DJB hash, by Daniel J. Berstein
	assert(str);
	
	uint32_t hash = 5381;
	for(; *str != '\0'; ++str)
		hash = ((hash << 5) + hash) + *str; // equivalent to hash * 33 + c

	return hash;
}


char*
strclone(
	const char* str
) {
	size_t len = strlen(str) + 1;
	char* ret = (char*)checked_malloc(sizeof(char) * len);
	memcpy(ret, str, sizeof(char) * len);
	return ret;
}

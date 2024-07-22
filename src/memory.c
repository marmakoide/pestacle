#include <stdlib.h>
#include "memory.h"
#include "errors.h"


void*
checked_malloc(size_t size) {
	void* ret = malloc(size);
	if (!ret)
		handle_out_of_memory_error();

	return ret;
}


void*
checked_calloc(size_t nmemb, size_t size) {
	void* ret = calloc(nmemb, size);
	if (!ret)
		handle_out_of_memory_error();

	return ret;
}

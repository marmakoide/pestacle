#ifndef PESTACLE_MEMORY_H
#define PESTACLE_MEMORY_H

#include <stddef.h>


extern void*
checked_malloc(size_t size);


extern void*
checked_calloc(size_t nmemb, size_t size);


#endif /* PESTACLE_MEMORY_H */
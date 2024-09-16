#ifndef PESTACLE_MEMORY_H
#define PESTACLE_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>


extern void*
checked_malloc(size_t size);


extern void*
checked_calloc(size_t nmemb, size_t size);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_MEMORY_H */
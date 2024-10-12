#ifndef PESTACLE_STRINGS_H
#define PESTACLE_STRINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


extern uint32_t
djb_hash(
	const char* str
);


extern char*
strclone(
	const char* str
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_STRINGS_H */
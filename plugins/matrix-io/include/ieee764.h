#ifndef PESTACLE_PLUGIN_MATRIX_IEEE764_H
#define PESTACLE_PLUGIN_MATRIX_IEEE764_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


union
ieee764_float32 {
	struct {
		#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		unsigned int mantissa : 23;
		unsigned int exponent :  8;
		unsigned int sign     :  1;
		#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		unsigned int sign     :  1;
		unsigned int exponent :  8;
		unsigned int mantissa : 23;
		#else
			#error Unsupported byte order
		#endif
	} ieee764;
	uint32_t uint32;
}; // ieee764_float32


extern void
ieee764_float32_encode(
	union ieee764_float32* self,
	float value
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_PLUGIN_MATRIX_IEEE764_H */

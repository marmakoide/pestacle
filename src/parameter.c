#include <assert.h>
#include "parameter.h"


void
ParameterValue_copy(
	ParameterValue* dst,
	const ParameterValue* src,
	enum ParameterType type
) {
	assert(dst != 0);
	assert(src != 0);

	switch(type) {
		case ParameterType__invalid:
		case ParameterType__last:
			assert(0);
			break;
		case ParameterType__integer:
			dst->int64_value = src->int64_value;
			break;
		case ParameterType__real:
			dst->real_value = src->real_value;
			break;
		case ParameterType__string:
			String_clone(&(dst->string_value), &(src->string_value));
			break;
	}
}
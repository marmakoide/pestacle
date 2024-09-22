#include <assert.h>
#include "memory.h"
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
		case ParameterType__bool:
			dst->bool_value = src->bool_value;
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


static bool
ParameterDefinition_has_parameters(
	const ParameterDefinition* array
) {
	assert(array != 0);

	return array->type != ParameterType__last;
}


static size_t
ParameterDefinition_parameter_count(
	const ParameterDefinition* array
) {
	assert(array != 0);

	size_t count = 0;
	for( ; array->type != ParameterType__last; ++array, ++count);

	return count;
}


ParameterValue*
ParameterValue_new(
	const ParameterDefinition* param_defs
) {
	assert(param_defs != 0);

	if (!ParameterDefinition_has_parameters(param_defs))
		return 0;

	size_t parameter_count = ParameterDefinition_parameter_count(param_defs);
		
	ParameterValue* ret =
		(ParameterValue*)checked_malloc(parameter_count * sizeof(ParameterValue));

	ParameterValue* param = ret;
	for( ; param_defs->type != ParameterType__last; ++param, ++param_defs)
		ParameterValue_copy(
			param,
			&(param_defs->default_value),
			param_defs->type
		);

	return ret;
}


void
ParameterValue_destroy(
	ParameterValue* self,
	const ParameterDefinition* param_defs
) {
	assert(self != 0);
	assert(param_defs != 0);

	for( ; param_defs->type != ParameterType__last; ++self, ++param_defs)
		if (param_defs->type == ParameterType__string)
			String_destroy(&(self->string_value));
}

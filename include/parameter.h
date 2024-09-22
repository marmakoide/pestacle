#ifndef PESTACLE_PARAMETER_H
#define PESTACLE_PARAMETER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "real.h"
#include "strings.h"


enum ParameterType {
	ParameterType__invalid = 0, // Used as a debugging help
	ParameterType__bool,
	ParameterType__integer,
	ParameterType__real,
	ParameterType__string,
	ParameterType__last         // Used to mark the end of an array of ParameterType
}; // enum ParameterType


typedef union {
	bool bool_value;
	int64_t int64_value;
	real_t real_value;
	String string_value;
} ParameterValue;


typedef struct {
	enum ParameterType type;
	String name;
	ParameterValue default_value;
} ParameterDefinition;


#define PARAMETER_DEFINITION_END \
{ \
	ParameterType__last, \
	{ 0, 0 }, \
	{ .int64_value = 0 } \
}

extern void
ParameterValue_copy(
	ParameterValue* dst,
	const ParameterValue* src,
	enum ParameterType type
);


extern ParameterValue*
ParameterValue_new(
	const ParameterDefinition* param_defs
);


extern void
ParameterValue_destroy(
	ParameterValue* self,
	const ParameterDefinition* param_defs
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_PARAMETER_H */

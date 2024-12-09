#ifndef PESTACLE_PARSER_AST_H
#define PESTACLE_PARSER_AST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <pestacle/dict.h>
#include <pestacle/math/real.h>
#include <pestacle/file_location.h>
#include <pestacle/string_list.h>


// --- AST_AtomicValue -------------------------------------------------------

enum AST_AtomicValueType {
	AST_AtomicValueType__invalid = 0, // Used as a debugging help
	AST_AtomicValueType__bool,
	AST_AtomicValueType__integer,
	AST_AtomicValueType__real,
	AST_AtomicValueType__string
}; // enum AST_AtomicValueType


typedef struct {
	enum AST_AtomicValueType type;
	union {
		bool bool_value;
		int64_t int64_value;
		real_t real_value;
		char* string_value;
	};
	FileLocation location;
} AST_AtomicValue;


extern void
AST_AtomicValue_init_bool(
	AST_AtomicValue* self,
	bool value
);


extern void
AST_AtomicValue_init_int64(
	AST_AtomicValue* self,
	int64_t value
);


extern void
AST_AtomicValue_init_real(
	AST_AtomicValue* self,
	real_t value
);


extern void
AST_AtomicValue_init_string(
	AST_AtomicValue* self,
	const char* value
);


extern void
AST_AtomicValue_destroy(
	AST_AtomicValue* self
);


// --- AST_Parameter ---------------------------------------------------------

typedef struct {
	char* name;
	AST_AtomicValue value;
	FileLocation location;
} AST_Parameter;


extern void
AST_Parameter_init(
	AST_Parameter* self,
	const char* name
);


extern void
AST_Parameter_destroy(
	AST_Parameter* self
);


// --- AST_Path --------------------------------------------------------------

typedef struct {
	StringList string_list;
	FileLocation location;
} AST_Path;


extern void
AST_Path_init(
	AST_Path* self
);


extern void
AST_Path_destroy(
	AST_Path* self
);


extern void
AST_Path_copy(
	AST_Path* self,
	const AST_Path* src
);


// --- AST_NodeInstanciation -------------------------------------------------

typedef struct {
	AST_Path src;
	AST_Path dst;
	Dict parameters;
} AST_NodeInstanciation;


extern bool
AST_NodeInstanciation_add_parameter(
	AST_NodeInstanciation* self,
	AST_Parameter* parameter
);


// --- AST_SlotAssignment ---------------------------------------------------0

typedef struct {
	AST_Path src;
	AST_Path dst;
} AST_SlotAssignment;


// --- AST_Statement ---------------------------------------------------------

enum AST_StatementType {
	AST_StatementType__invalid = 0,         // Used as a debugging help
	AST_StatementType__node_instanciation,
	AST_StatementType__slot_assignment
}; // enum AST_StatementType


struct s_AST_Statement;
typedef struct s_AST_Statement AST_Statement;


struct s_AST_Statement {
	enum AST_StatementType type;
	union {
		AST_NodeInstanciation node_instanciation;
        AST_SlotAssignment slot_assignment;
    };
	struct s_AST_Statement* next;
	FileLocation location;
}; // struct s_AST_Statement


// --- AST_Unit --------------------------------------------------------------

typedef struct {
	AST_Statement* head;
	AST_Statement* tail;	
} AST_Unit;


extern void
AST_Unit_init(
	AST_Unit* self
);


extern void
AST_Unit_destroy(
	AST_Unit* self
);


extern AST_Statement*
AST_Unit_append_node_instanciation(
	AST_Unit* self,
	const AST_Path* src,
	const AST_Path* dst
);


extern AST_Statement*
AST_Unit_append_slot_assignment(
	AST_Unit* self,
	const AST_Path* src,
	const AST_Path* dst
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_PARSER_PARSER_H */

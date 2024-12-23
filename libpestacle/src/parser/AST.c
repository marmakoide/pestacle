#include <assert.h>
#include <stdlib.h>

#include <pestacle/memory.h>
#include <pestacle/strings.h>
#include <pestacle/parser/AST.h>


// --- AST_AtomicValue -------------------------------------------------------

static void
AST_AtomicValue_init(
	AST_AtomicValue* self
) {
	assert(self);

	self->type = AST_AtomicValueType__invalid;
	self->location.line = 0;
}


void
AST_AtomicValue_init_bool(
	AST_AtomicValue* self,
	bool value
) {
	assert(self);

	self->type = AST_AtomicValueType__bool;
	self->bool_value = value;
	self->location.line = 0;
}


void
AST_AtomicValue_init_int64(
	AST_AtomicValue* self,
	int64_t value
) {
	assert(self);

	self->type = AST_AtomicValueType__integer;
	self->int64_value = value;
	self->location.line = 0;
}


void
AST_AtomicValue_init_real(
	AST_AtomicValue* self,
	real_t value
) {
	assert(self);

	self->type = AST_AtomicValueType__real;
	self->real_value = value;
	self->location.line = 0;
}


void
AST_AtomicValue_init_string(
	AST_AtomicValue* self,
	const char* value
) {
	assert(self);

	self->type = AST_AtomicValueType__string;
	self->string_value = strclone(value);
	self->location.line = 0;
}


void
AST_AtomicValue_destroy(
	AST_AtomicValue* self
) {
	assert(self);
	assert(self->type != AST_AtomicValueType__invalid);

	if (self->type == AST_AtomicValueType__string) {
		assert(self->string_value);
		free(self->string_value);
		self->string_value = 0;
	}
}


static void
AST_AtomicValue_print(
	AST_AtomicValue* self,
	FILE* out
) {
	assert(self);
	assert(out);
	assert(self->type != AST_AtomicValueType__invalid);

	switch(self->type) {
		case AST_AtomicValueType__bool:
			if (self->bool_value)
				fputs("true", out);
			else
				fputs("false", out);
			break;
		
		case AST_AtomicValueType__integer:
			fprintf(out, "%ld", self->int64_value);
			break;
		
		case AST_AtomicValueType__real:
			fprintf(out, "%f", self->real_value);
			break;
		
		case AST_AtomicValueType__string:
			fputc('"', out);
			fputs(self->string_value, out);
			fputc('"', out);
			break;
		
		default:
			assert(0);
	}
}


// --- AST_Parameter ---------------------------------------------------------

void
AST_Parameter_init(
	AST_Parameter* self,
	const char* name
) {
	assert(self);
	assert(name);

	AST_AtomicValue_init(&(self->value));
	self->name = strclone(name);
	self->location.line = 0;
}


void
AST_Parameter_destroy(
	AST_Parameter* self
) {
	assert(self);

	if (self->name)
		free(self->name);
}


static void
AST_Parameter_print(
	AST_Parameter* self,
	FILE* out
) {
	assert(self);
	assert(out);

	fputs(self->name, out);
	fputs(" = ", out);
	AST_AtomicValue_print(&(self->value), out);
}


// --- AST_Path --------------------------------------------------------------

void
AST_Path_init(
	AST_Path* self
) {
	assert(self);

	self->location.line = 0;
	StringList_init(&(self->string_list));
}


void
AST_Path_destroy(
	AST_Path* self
) {
	assert(self);

	StringList_destroy(&(self->string_list));
}


void
AST_Path_copy(
	AST_Path* self,
	const AST_Path* src
) {
	assert(self);
	assert(src);

	self->location = src->location;
	StringList_copy(&(self->string_list), &(src->string_list));
}


static void
AST_Path_print(
	AST_Path* self,
	FILE* out
) {
	assert(self);
	assert(out);

	for(size_t i = 0; i < StringList_length(&(self->string_list)); ++i) {
		if (i > 0)
			fputc('.', out);
		
		fputs(StringList_at(&(self->string_list), i), out);
	}
}


// --- AST_Instanciation -----------------------------------------------------

static void
AST_Instanciation_init(
	AST_Instanciation* self,
	const AST_Path* src,
	const AST_Path* dst
) {
	assert(self);

	AST_Path_init(&(self->src));
	AST_Path_init(&(self->dst));
	Dict_init(&(self->parameters));

	AST_Path_copy(&(self->src), src);
	AST_Path_copy(&(self->dst), dst);
}


static void
AST_Instanciation_destroy(
	AST_Instanciation* self
) {
	assert(self);

	AST_Path_destroy(&(self->src));
	AST_Path_destroy(&(self->dst));

	DictIterator it;
	DictIterator_init(&it, &(self->parameters));
	for( ; DictIterator_has_next(&it); DictIterator_next(&it)) {
		AST_Parameter* parameter = (AST_Parameter*)it.entry->value;
		AST_Parameter_destroy(parameter);
		free(parameter);
	}


	Dict_destroy(&(self->parameters));
}


bool
AST_Instanciation_add_parameter(
	AST_Instanciation* self,
	AST_Parameter* parameter
) {
	assert(self);
	assert(parameter);
	assert(parameter->name);

	DictEntry* entry = Dict_insert(&(self->parameters), parameter->name);
	if (!entry)
		return false;

	entry->value = parameter;

	return true;
}


static void
AST_Instanciation_print(
	AST_Instanciation* self,
	FILE* out
) {
	assert(self);
	assert(out);

	AST_Path_print(&(self->dst), out);
	fputs(" = ", out);
	AST_Path_print(&(self->src), out);

	fputc('(', out);

	DictIterator it;
	DictIterator_init(&it, &(self->parameters));
	for(size_t i = 0; DictIterator_has_next(&it); DictIterator_next(&it), ++i) {
		AST_Parameter* parameter = (AST_Parameter*)it.entry->value;

		if (i > 0)
			fputc(',', out);
		fputc('\n', out);

		fputs("  ", out);
		AST_Parameter_print(parameter, out);
	}

	fputs("\n)", out);
}


// --- AST_SlotAssignment ----------------------------------------------------

static void
AST_SlotAssignment_init(
	AST_SlotAssignment* self,
	const AST_Path* src,
	const AST_Path* dst	
) {
	assert(self);

	AST_Path_init(&(self->src));
	AST_Path_init(&(self->dst));

	AST_Path_copy(&(self->src), src);
	AST_Path_copy(&(self->dst), dst);
}


static void
AST_SlotAssignment_destroy(
	AST_SlotAssignment* self
) {
	assert(self);

	AST_Path_destroy(&(self->src));
	AST_Path_destroy(&(self->dst));
}


static void
AST_SlotAssignment_print(
	AST_SlotAssignment* self,
	FILE* out
) {
	assert(self);
	assert(out);

	AST_Path_print(&(self->dst), out);
	fputs(" = ", out);
	AST_Path_print(&(self->src), out);
}


// --- AST_Statement ---------------------------------------------------------

static void
AST_Statement_init(
	AST_Statement* self
) {
	assert(self);

	self->type = AST_StatementType__invalid;
	self->next = 0;
	self->location.line = 0;
}


static void
AST_Statement_destroy(
	AST_Statement* self
) {
	assert(self);

	switch(self->type) {
		case AST_StatementType__invalid:
			assert(0);
			break;

		case AST_StatementType__instanciation:
			AST_Instanciation_destroy(&(self->instanciation));
			break;

		case AST_StatementType__slot_assignment:
			AST_SlotAssignment_destroy(&(self->slot_assignment));
			break;
	}
}


// --- AST_Unit --------------------------------------------------------------

void
AST_Unit_init(
	AST_Unit* self
) {
	assert(self);

	self->head = 0;
	self->tail = 0;
}


void
AST_Unit_destroy(
	AST_Unit* self
) {
	assert(self);

	for(AST_Statement* it = self->head; it; ) {
		AST_Statement* next = it->next;
		AST_Statement_destroy(it);
		free(it);
		it = next;
	}

	self->head = 0;
	self->tail = 0;
}


static AST_Statement*
AST_Unit_append_statement(
	AST_Unit* self
) {
	assert(self);

	// Allocate the new statement
	AST_Statement* ret = (AST_Statement*)checked_malloc(sizeof(AST_Statement));
	AST_Statement_init(ret);

	// Update the linked list
	if (self->head == 0)
		self->head = ret;
	else
		self->tail->next = ret;

	self->tail = ret;

	// Job done
	return ret;
}


AST_Statement*
AST_Unit_append_instanciation(
	AST_Unit* self,
	const AST_Path* src,
	const AST_Path* dst
) {
	assert(self);
	assert(src);
	assert(dst);

	AST_Statement* ret = AST_Unit_append_statement(self);
	ret->type = AST_StatementType__instanciation;
	AST_Instanciation_init(&(ret->instanciation), src, dst);

	return ret;
}


AST_Statement*
AST_Unit_append_slot_assignment(
	AST_Unit* self,
	const AST_Path* src,
	const AST_Path* dst
) {
	assert(self);
	assert(src);
	assert(dst);

	AST_Statement* ret = AST_Unit_append_statement(self);
	ret->type = AST_StatementType__slot_assignment;
	AST_SlotAssignment_init(&(ret->slot_assignment), src, dst);

	return ret;
}


void
AST_Unit_print(
	AST_Unit* self,
	FILE* out
) {
	assert(self);
	assert(out);

	// For each statement
	size_t i = 0;
	for(AST_Statement* stat = self->head; stat; stat = stat->next, ++i) {
		if (i > 0)
			fputc('\n', out);

		switch(stat->type) {
			case AST_StatementType__instanciation:
				AST_Instanciation_print(&(stat->instanciation), out);
				break;

			case AST_StatementType__slot_assignment:
				AST_SlotAssignment_print(&(stat->slot_assignment), out);
				break;

			default:
				assert(0);
		}
	}
}
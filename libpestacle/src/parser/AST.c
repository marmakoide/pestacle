#include <assert.h>
#include <stdlib.h>

#include <pestacle/memory.h>
#include <pestacle/strings.h>
#include <pestacle/parser/AST.h>


// --- AST_AtomicValue -------------------------------------------------------

void
AST_AtomicValue_init_bool(
	AST_AtomicValue* self,
	bool value
) {
	assert(self);

	self->type = AST_AtomicValueType__bool;
	self->bool_value = value;
}


void
AST_AtomicValue_init_int64(
	AST_AtomicValue* self,
	int64_t value
) {
	assert(self);

	self->type = AST_AtomicValueType__integer;
	self->int64_value = value;
}


void
AST_AtomicValue_init_real(
	AST_AtomicValue* self,
	real_t value
) {
	assert(self);

	self->type = AST_AtomicValueType__real;
	self->real_value = value;
}


void
AST_AtomicValue_init_string(
	AST_AtomicValue* self,
	const char* value
) {
	assert(self);

	self->type = AST_AtomicValueType__string;
	self->string_value = strclone(value);
}


void
AST_AtomicValue_destroy(
	AST_AtomicValue* self
) {
	assert(self);
	assert(self->type != AtomicValueType__invalid);

	if (self->type == AST_AtomicValueType__string) {
		assert(self->string_value);
		free(self->string_value);
		self->string_value = 0;
	}
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


// --- AST_NodeInstanciation -------------------------------------------------

static void
AST_NodeInstanciation_init(
	AST_NodeInstanciation* self,
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
AST_NodeInstanciation_destroy(
	AST_NodeInstanciation* self
) {
	assert(self);

	AST_Path_destroy(&(self->src));
	AST_Path_destroy(&(self->dst));
	Dict_destroy(&(self->parameters));
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

		case AST_StatementType__node_instanciation:
			AST_NodeInstanciation_destroy(&(self->node_instanciation));
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
AST_Unit_append_node_instanciation(
	AST_Unit* self,
	const AST_Path* src,
	const AST_Path* dst
) {
	assert(self);
	assert(src);
	assert(dst);

	AST_Statement* ret = AST_Unit_append_statement(self);
	ret->type = AST_StatementType__node_instanciation;
	AST_NodeInstanciation_init(&(ret->node_instanciation), src, dst);

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
#ifndef PESTACLE_SCOPE_H
#define PESTACLE_SCOPE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <pestacle/dict.h>
#include <pestacle/node.h>
#include <pestacle/stack.h>
#include <pestacle/string_list.h>


struct s_Scope;
typedef struct s_Scope Scope;


struct s_ScopeDelegate;
typedef struct s_ScopeDelegate ScopeDelegate;


enum ScopeMemberType {
	ScopeMemberType__invalid = 0, // Used as a debugging help
	ScopeMemberType__node,
	ScopeMemberType__scope,
	ScopeMemberType__node_delegate,
	ScopeMemberType__scope_delegate,
	ScopeMemberType__last
}; // enum ScopeMemberType


typedef struct {
	enum ScopeMemberType type;
	union {
		Node* node;
		Scope* scope;
		const NodeDelegate* node_delegate;
		const ScopeDelegate* scope_delegate;
	};
	Scope* parent;
} ScopeMember;


typedef struct {
	bool (*setup)(   // setup method (optional, can be 0)
		Scope*
	);

	void (*destroy)( // destroy method (optional, can be 0)
		Scope*
	);
} ScopeDelegateMethods;


struct s_ScopeDelegate {
	const char* name;
	const ParameterDefinition* parameter_defs;
	ScopeDelegateMethods methods;
}; // struct s_ScopeDelegate


struct s_Scope {
	void* data;
	char* name;

	const ScopeDelegate* delegate;
	struct s_Scope* delegate_scope; // Scope owning the delegate
	
	ParameterValue* parameters;
	Dict members;
}; // struct s_Scope


extern void
Scope_print(
	Scope* self,
	FILE* out
);


extern Scope*
Scope_new(
	const char* name,
	const ScopeDelegate* delegate,
	Scope* delegate_scope
);


extern bool
Scope_setup(
	Scope* self
);


extern void
Scope_destroy(
	Scope* self
);


extern bool
Scope_get_parameter_by_name(
	Scope* self,
	const char* name,
	const ParameterDefinition** param_def_ptr,
	ParameterValue** param_value_ptr
);


extern ScopeMember*
Scope_get_member(
	Scope* self,
	const StringListView* path
);


extern bool
Scope_add_node(
	Scope* self,
	Node* node
);


extern bool
Scope_add_scope(
	Scope* self,
	Scope* scope
);


extern bool
Scope_add_node_delegate(
	Scope* self,
	const NodeDelegate* node_delegate
);


extern bool
Scope_add_scope_delegate(
	Scope* self,
	const ScopeDelegate* scope_delegate
);


extern bool
Scope_instanciate_scope(
	Scope* self,
	const ScopeDelegate* scope_delegate
);


extern bool
Scope_populate(
	Scope* self,
	const NodeDelegate** node_delegate_list,
	const ScopeDelegate** scope_delegate_list,
	const ScopeDelegate** scope_instance_delegate_list
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_SCOPE_H */

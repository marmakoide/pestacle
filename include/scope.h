#ifndef PESTACLE_SCOPE_H
#define PESTACLE_SCOPE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "dict.h"
#include "node.h"
#include "stack.h"
#include "window_manager.h"


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
} ScopeMember;


typedef struct {
	bool (*setup)(   // setup method (optional, can be 0)
		Scope*,
		WindowManager*
	);

	void (*destroy)( // destroy method (optional, can be 0)
		Scope*
	);
} ScopeDelegateMethods;


struct s_ScopeDelegate {
	String name;
	const ParameterDefinition* parameter_defs;
	ScopeDelegateMethods methods;
}; // struct s_ScopeDelegate


struct s_Scope {
	void* data;
	String name;

	struct s_Scope* parent_scope;   // Scope owning the scope, 0 if any

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
	const String* name,
	const ScopeDelegate* delegate,
	Scope* delegate_scope
);


extern bool
Scope_setup(
	Scope* self,
	WindowManager* window_manager
);


extern void
Scope_destroy(
	Scope* self
);


extern bool
Scope_get_parameter_by_name(
	Scope* self,
	const String* name,
	const ParameterDefinition** param_def_ptr,
	ParameterValue** param_value_ptr
);


extern ScopeMember*
Scope_get_member(
	Scope* self,
	const String* path,
	size_t path_len
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


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_SCOPE_H */

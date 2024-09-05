#ifndef PESTACLE_DOMAIN_H
#define PESTACLE_DOMAIN_H

#include "dict.h"
#include "node.h"


struct s_Domain;
typedef struct s_Domain Domain;


struct s_DomainDelegate;
typedef struct s_DomainDelegate DomainDelegate;


enum DomainMemberType {
	DomainMemberType__invalid = 0, // Used as a debugging help
	DomainMemberType__node_delegate,
	DomainMemberType__domain_delegate,
	DomainMemberType__last
}; // enum DomainMemberType


typedef struct {
	enum DomainMemberType type;
	union {
		const NodeDelegate* node_delegate;
		const DomainDelegate* domain_delegate;
	};
} DomainMember;


typedef struct {
	bool (*setup)(   // setup method (optional, can be 0)
		Domain*
	);

	void (*destroy)( // destroy method (optional, can be 0)
		Domain*
	);
} DomainDelegateMethods;


struct s_DomainDelegate {
	String name;
	const ParameterDefinition* parameter_defs;
	DomainDelegateMethods methods;
}; // struct s_DomainDelegate


struct s_Domain {
	void* data;
	const DomainDelegate* delegate;
	String name;
	ParameterValue* parameters;
	Dict members;
}; // struct s_Domain


extern Domain*
Domain_new(
	const String* name,
	const DomainDelegate* delegate
);


extern bool
Domain_setup(
	Domain* self
);


extern void
Domain_destroy(
	Domain* self
);


extern const DomainMember*
Domain_get_member_by_name(
	Domain* self,
	const String* name
);


extern bool
Domain_add_node_delegate(
	Domain* self,
	const NodeDelegate* node_delegate
);


#endif /* PESTACLE_DOMAIN_H */
#ifndef PESTACLE_SOURCE_H
#define PESTACLE_SOURCE_H

#include <SDL.h>
#include "event.h"
#include "matrix.h"
#include "strings.h"


struct s_Source;
typedef struct s_Source Source;


typedef struct {
	String name;
} SourceInputSlotDefinition;


typedef struct {
	int (*setup)(              // setup method (optional, can be 0)
		Source*,
		int width,
		int height
	);

	void (*destroy)(           // destroy method (optional, can be 0)
		Source*
	);

	void (*update)(            // update method (optional, can be 0)
		Source*
	);

	void (*handle_event)(      // handle_event (optional, can be 0)
		Source*,
		const Event* event	
	);

	const Matrix* (*get)(      // get method
		const Source*
	);
} SourceDelegateMethods;


typedef struct {
	String name;
	size_t input_count;
	const SourceInputSlotDefinition* input_defs;
	SourceDelegateMethods methods;
} SourceDelegate;


struct s_Source {
	void* data;
	const SourceDelegate* delegate;
	Source** inputs;
}; // struct s_Source


extern Source*
source_allocate();


extern void
source_init(
	Source* self,
	const SourceDelegate* delegate,
	void* data
);


extern int
source_set_input_slot(
	Source* self,
	const String* name,
	Source* other
);


extern int
source_setup(
	Source* self,
	int width,
	int height
);


extern void
source_destroy(
	Source* self
);


extern void
source_update(
	Source* self
);


extern void
source_handle_event(
	Source* self,
	const Event* event
);


extern const Matrix*
source_get(
	Source* self
);


#endif /* PESTACLE_SOURCE_H */
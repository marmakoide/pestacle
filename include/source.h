#ifndef PESTACLE_SOURCE_H
#define PESTACLE_SOURCE_H

#include <SDL.h>
#include "event.h"
#include "matrix.h"


struct s_Source;
typedef struct s_Source Source;

typedef struct {
	const char* name;

	int (*setup)(
		Source*,
		int width,
		int height
	);

	void (*destroy)(
		Source*
	);

	void (*update)(
		Source*
	);

	void (*handle_event)(
		Source*,
		const Event* event	
	);

	const struct Matrix* (*get)(
		const Source*
	);
} SourceDelegate;


struct s_Source {
	void* data;
	const SourceDelegate* delegate;
}; // struct s_Source


extern Source*
source_allocate();


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


extern const struct Matrix*
source_get(
	Source* self
);


#endif /* PESTACLE_SOURCE_H */
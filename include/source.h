#ifndef PESTACLE_SOURCE_H
#define PESTACLE_SOURCE_H

#include <SDL.h>
#include "event.h"
#include "matrix.h"


struct Source;

struct SourceDelegate {
	const char* name;

	int (*setup)(
		struct Source*,
		int width,
		int height
	);

	void (*destroy)(
		struct Source*
	);

	void (*update)(
		struct Source*
	);

	void (*handle_event)(
		struct Source*,
		const Event* event	
	);

	const struct Matrix* (*get)(
		const struct Source*
	);
}; // struct Source


struct Source {
	void* data;
	const struct SourceDelegate* delegate;
}; // struct Source


extern struct Source*
source_allocate();


extern int
source_setup(
	struct Source* self,
	int width,
	int height
);


extern void
source_destroy(
	struct Source* self
);


extern void
source_update(
	struct Source* self
);


extern void
source_handle_event(
	struct Source* self,
	const Event* event
);


extern const struct Matrix*
source_get(
	struct Source* self
);


#endif /* PESTACLE_SOURCE_H */
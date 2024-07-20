#ifndef PESTACLE_RENDERER_H
#define PESTACLE_RENDERER_H

#include <SDL.h>
#include "matrix.h"


/*
 * Interface defining a way to render a matrix to a SDL buffer
 */


struct Renderer;

typedef struct {
	const char* name;

	int (*setup)(
		struct Renderer*,
		int width,
		int height
	);

	void (*destroy)(
		struct Renderer*
	);

	void (*render)(
		struct Renderer*,
		const struct Matrix* src,
		SDL_Surface* dst
	);
} RendererDelegate;


struct Renderer {
	void* data;
	const RendererDelegate* delegate;
}; // struct Renderer


extern struct Renderer*
renderer_allocate();


extern int
renderer_setup(
	struct Renderer* self,
	int width,
	int height
);


extern void
renderer_destroy(
	struct Renderer* self
);


extern void
renderer_render(
	struct Renderer* self,
	const struct Matrix* src,
	SDL_Surface* dst
);


#endif /* PESTACLE_RENDERER_H */
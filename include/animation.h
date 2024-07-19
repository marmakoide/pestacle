#ifndef PESTACLE_ANIMATION_H
#define PESTACLE_ANIMATION_H

#include <SDL.h>
#include <vector.h>
#include <matrix.h>
#include "source.h"
#include "renderer.h"


struct Animation {
	struct Source* source;
	struct Renderer* renderer;
}; // struct Animation


extern int
animation_init(
	struct Animation* self,
	int screen_width,
	int screen_height
);


extern void
animation_destroy(
	struct Animation* self
);


extern void
animation_handle_event(
	struct Animation* self,
	const union Event* event
);


extern void
animation_render(
	const struct Animation* self,
	SDL_Surface* surface
);


extern void
animation_update(
	struct Animation* self
);


#endif /* PESTACLE_ANIMATION_H */ 
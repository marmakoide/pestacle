#ifndef PESTACLE_ANIMATION_H
#define PESTACLE_ANIMATION_H

#include <SDL.h>
#include "vector.h"
#include "matrix.h"
#include "source.h"
#include "renderer.h"


typedef struct {
	Source* source_a;
	Source* source_b;
	Renderer* renderer;
} Animation;


extern int
animation_init(
	Animation* self,
	int screen_width,
	int screen_height
);


extern void
animation_destroy(
	Animation* self
);


extern void
animation_handle_event(
	Animation* self,
	const Event* event
);


extern void
animation_render(
	const Animation* self,
	SDL_Surface* surface
);


extern void
animation_update(
	Animation* self
);


#endif /* PESTACLE_ANIMATION_H */ 
#ifndef PESTACLE_ANIMATION_H
#define PESTACLE_ANIMATION_H

#include <SDL.h>
#include "vector.h"
#include "matrix.h"
#include "node.h"
#include "renderer.h"
#include "dict.h"


typedef struct {
	Dict node_instance_dict;
	size_t sorted_node_count;
	Node** sorted_nodes;

	Renderer* renderer;
} Animation;


extern void
Animation_init(
	Animation* self
);


extern bool
Animation_setup(
	Animation* self,
	int screen_width,
	int screen_height
);


extern void
Animation_destroy(
	Animation* self
);


extern Node*
Animation_get_node_instance(
	Animation* self,
	const String* instance_name
);


extern bool
Animation_add_node_instance(
	Animation* self,
	const String* instance_name,
	const String* delegate_name
);


extern void
Animation_handle_event(
	Animation* self,
	const Event* event
);


extern void
Animation_render(
	const Animation* self,
	SDL_Surface* surface
);


extern void
Animation_update(
	Animation* self
);


#endif /* PESTACLE_ANIMATION_H */ 
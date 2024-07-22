#include "image.h"
#include "animation.h"

#include "nodes/heat_diffusion.h"
#include "nodes/mouse_motion.h"
#include "renderers/gradient.h"
#include "renderers/linear_blend.h"


int
animation_init(
	Animation* self,
	int screen_width,
	int screen_height
) {
	self->node_a = 0;
	self->node_b = 0;
	self->renderer = 0;

	// Setup the sources
	self->node_a = mouse_motion_node_new();
	if (!node_setup(self->node_a, screen_width, screen_height))
		goto failure;

	const String param_name = { "value", 6 };
	node_get_parameter_by_name(self->node_a, &param_name)->value = (real_t)32;

	self->node_b = heat_diffusion_node_new();
	if (!node_setup(self->node_b, screen_width, screen_height))
		goto failure;

	const String slot_name = { "input", 6 };
	if (!node_set_input_slot_by_name(self->node_b, &slot_name, self->node_a)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Could not find slot '%s' for '%s' source type\n",
			slot_name.data,
			self->node_b->delegate->name.data
		);
		goto failure;
	}

	// Setup the renderer
	//self->renderer = gradient_renderer_new();

	self->renderer =
		linear_blend_renderer_new(
			"assets/paysage-vignoble_200x100.png",
			"assets/soif_200x100.png"
		);

	if (!renderer_setup(self->renderer, screen_width, screen_height))
		goto failure;

	// Job done
	return 1;

	// Failure handling
failure:
	animation_destroy(self);
	return 0;
}


void
animation_destroy(
	Animation* self
) {
	if (self->node_a)
		node_destroy(self->node_a);

	if (self->node_b)
		node_destroy(self->node_b);

	if (self->renderer)
		renderer_destroy(self->renderer);

	#ifdef DEBUG
	self->node_a = 0;
	self->node_b = 0;
	self->renderer = 0;
	#endif
}


void
animation_handle_event(
	Animation* self,
	const Event* event
) {
	node_handle_event(self->node_a, event);
	node_handle_event(self->node_b, event);
}


void
animation_render(
	const Animation* self,
	SDL_Surface* dst
) {
	renderer_render(self->renderer, node_get(self->node_b), dst);
}


void
animation_update(
	Animation* self
) {
	node_update(self->node_b);
	node_update(self->node_a);
}
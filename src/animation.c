#include "image.h"
#include "animation.h"

#include "sources/heat_diffusion.h"
#include "sources/mouse_motion.h"
#include "renderers/gradient.h"
#include "renderers/linear_blend.h"


int
animation_init(
	Animation* self,
	int screen_width,
	int screen_height
) {
	self->source_a = 0;
	self->source_b = 0;
	self->renderer = 0;

	// Setup the sources
	self->source_a = mouse_motion_source_new((real_t)32);
	if (!source_setup(self->source_a, screen_width, screen_height))
		goto failure;

	self->source_b = heat_diffusion_source_new();
	if (!source_setup(self->source_b, screen_width, screen_height))
		goto failure;

	const String slot_name = { "input", 6 };
	if (!source_set_input_slot(self->source_b, &slot_name, self->source_a)) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Could not find slot '%s' for '%s' source type\n",
			slot_name.data,
			self->source_b->delegate->name.data
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
	if (self->source_a) {
		source_destroy(self->source_a);
		self->source_a = 0;
	}

	if (self->source_b) {
		source_destroy(self->source_b);
		self->source_b = 0;
	}

	if (self->renderer) {
		renderer_destroy(self->renderer);
		self->renderer = 0;
	}

	return 0;
}


void
animation_destroy(
	Animation* self
) {
	if (self->source_a)
		source_destroy(self->source_a);

	if (self->source_b)
		source_destroy(self->source_b);

	if (self->renderer)
		renderer_destroy(self->renderer);

	#ifdef DEBUG
	self->source_a = 0;
	self->source_b = 0;
	self->renderer = 0;
	#endif
}


void
animation_handle_event(
	Animation* self,
	const Event* event
) {
	source_handle_event(self->source_a, event);
	source_handle_event(self->source_b, event);
}


void
animation_render(
	const Animation* self,
	SDL_Surface* dst
) {
	renderer_render(self->renderer, source_get(self->source_b), dst);
}


void
animation_update(
	Animation* self
) {
	source_update(self->source_b);
	source_update(self->source_a);
}
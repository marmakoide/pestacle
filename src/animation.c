#include "image.h"
#include "animation.h"

#include "sources/heat_diffusion.h"
#include "renderers/gradient.h"
#include "renderers/linear_blend.h"


int
animation_init(
	struct Animation* self,
	int screen_width,
	int screen_height
) {
	self->source = 0;
	self->renderer = 0;

	// Setup the source
	self->source =
		heat_diffusion_source_new();

	if (!source_setup(
		self->source,
		screen_width,
		screen_height
	))
		return 0;
	
	// Setup the renderer
	//self->renderer = gradient_renderer_new();
	
	self->renderer =
		linear_blend_renderer_new(
			"assets/paysage-vignoble_200x100.png",
			"assets/soif_200x100.png"
		);
		
	if (!renderer_setup(
		self->renderer,
		screen_width,
		screen_height
	))
		return 0;

	// Job done
	return 1;
}


void
animation_destroy(
	struct Animation* self
) {
	source_destroy(self->source);
	renderer_destroy(self->renderer);

	#ifdef DEBUG
	self->source = 0;
	self->renderer = 0;
	#endif
}


void
animation_handle_event(
	struct Animation* self,
	const Event* event
) {
	source_handle_event(
		self->source,
		event
	);
}


void
animation_render(
	const struct Animation* self,
	SDL_Surface* dst
) {
	renderer_render(
		self->renderer,
		source_get(self->source),
		dst
	);
}


void
animation_update(
	struct Animation* self
) {
	source_update(
		self->source
	);
}
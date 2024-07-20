#include "renderers/gradient.h"


int
gradient_renderer_setup(
	struct Renderer* self,
	int width,
	int height
) {
	return 1;
}


void
gradient_renderer_destroy(
	struct Renderer* self
) {

}


void
gradient_renderer_render(
	struct Renderer* self,
	const struct Matrix* src,
	SDL_Surface* dst
) {
	const real_t* coeff = src->data;
	uint8_t* pixel_row = (uint8_t*)dst->pixels;
	for(int i = dst->h; i != 0; --i, pixel_row += dst->pitch) {
		uint8_t* pixel = pixel_row;
		for(int j = dst->w; j != 0; --j, pixel += 3, ++coeff) {
			uint8_t level = (uint8_t)fmaxf(fminf(255.f * (*coeff), 255.f), 0.f);

			for(int k = 0; k < 3; ++k)
				pixel[k] = 0xff - level;
		}
	}
}


static const RendererDelegate
gradient_renderer_delegate = {
	"gradient-renderer",
	gradient_renderer_setup,
	gradient_renderer_destroy,
	gradient_renderer_render
};



struct Renderer*
gradient_renderer_new() {
	// Allocation
	struct Renderer* ret = renderer_allocate();
	if (!ret)
		return ret;

	// Delegate setup
	ret->data = 0;
	ret->delegate = &gradient_renderer_delegate;

	// Job done
	return ret;
}
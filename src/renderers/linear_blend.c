#include "image.h"
#include "renderers/gradient.h"


struct LinearBlendData {
	const char* picture_path_a;
	SDL_Surface* picture_a;

	const char* picture_path_b;
	SDL_Surface* picture_b;
};


int
linear_blend_renderer_setup(
	struct Renderer* self,
	int width,
	int height
) {
	struct LinearBlendData* data =
		(struct LinearBlendData*)self->data;

	// Load background picture
	data->picture_a = load_png(data->picture_path_a);
	if (!data->picture_a)
		return 0;

	data->picture_b = load_png(data->picture_path_b);
	if (!data->picture_b)
		return 0;

	// Job done
	return 1;
}


void
linear_blend_renderer_destroy(
	struct Renderer* self
) {
	struct LinearBlendData* data =
		(struct LinearBlendData*)self->data;

	if (data->picture_a)
		SDL_FreeSurface(data->picture_a);
	data->picture_a = 0;

	if (data->picture_b)
		SDL_FreeSurface(data->picture_b);
	data->picture_b = 0;
}


void
linear_blend_renderer_render(
	struct Renderer* self,
	const struct Matrix* src,
	SDL_Surface* dst
) {
	struct LinearBlendData* data =
		(struct LinearBlendData*)self->data;

	const real_t* coeff = src->data;
	uint8_t* dst_pixel_row = (uint8_t*)dst->pixels;
	const uint8_t* src_a_pixel_row = (const uint8_t*)data->picture_a->pixels;
	const uint8_t* src_b_pixel_row = (const uint8_t*)data->picture_b->pixels;
	
	for(int i = dst->h; i != 0; --i, dst_pixel_row += dst->pitch, src_a_pixel_row += data->picture_a->pitch, src_b_pixel_row += data->picture_b->pitch) {
		uint8_t* dst_pixel = dst_pixel_row;
		const uint8_t* src_a_pixel = src_a_pixel_row;
		const uint8_t* src_b_pixel = src_b_pixel_row;

		for(int j = dst->w; j != 0; --j, dst_pixel += 3, src_a_pixel += 4, src_b_pixel += 4, ++coeff) {
			float level = fmaxf(fminf(*coeff, 1.f), 0.f);
			
			for(int k = 0; k < 3; ++k)
				dst_pixel[k] = (uint8_t)(src_b_pixel[k] * level + src_a_pixel[k] * (1.f - level));
		}
	}
}


static const RendererDelegate
linear_blend_renderer_delegate = {
	"linear-blend-renderer",
	linear_blend_renderer_setup,
	linear_blend_renderer_destroy,
	linear_blend_renderer_render
};



struct Renderer*
linear_blend_renderer_new(
	const char* picture_path_a,
	const char* picture_path_b
) {
	// Allocation
	struct Renderer* ret = renderer_allocate();
	if (!ret)
		return ret;

	struct LinearBlendData* data =
		(struct LinearBlendData*)malloc(sizeof(struct LinearBlendData));

	data->picture_a = 0;
	data->picture_path_a = picture_path_a;

	data->picture_b = 0;
	data->picture_path_b = picture_path_b;

	// Delegate setup
	ret->data = data;
	ret->delegate = &linear_blend_renderer_delegate;

	// Job done
	return ret;
}
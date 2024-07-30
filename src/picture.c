#include <spng.h>
#include <errno.h>
#include "picture.h"


SDL_Surface*
load_png(const char* path) {
	SDL_Surface* surface = 0;

	// Open file
	FILE* f = fopen(path, "rb");
	if (!f) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to open file '%s': %s", path, strerror(errno));
		goto termination;
	}

	// Setup a spng context
	spng_ctx* ctx = spng_ctx_new(0);
	if (!ctx) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to setup PNG I/O context");
		goto termination;
	}
	
    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
	spng_set_png_file(ctx, f);

	// Read file header
	struct spng_ihdr ihdr;
	int ret = spng_get_ihdr(ctx, &ihdr);
	if (ret) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to read PNG header");
		goto termination;
	}

	// Compute decoded data size
	size_t out_size;
	ret = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &out_size);
	if (ret) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to compute PNG data decoded size");
		goto termination;
	}

	// Create the surface
	surface = SDL_CreateRGBSurfaceWithFormat(
		0,
		ihdr.width,
		ihdr.height,
		32,
		SDL_PIXELFORMAT_RGBA32
	);

	// Decode the PNG data
	ret = spng_decode_image(ctx, surface->pixels, out_size, SPNG_FMT_RGBA8, 0);
	if (ret) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unable to read PNG data");
		
		SDL_FreeSurface(surface);
		surface = 0;
	}
	
	// Free ressources
termination:
	if (ctx)
		spng_ctx_free(ctx);

	if (f)
		fclose(f);

	// Job done
	return surface;
}

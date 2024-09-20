#include <png.h>
#include <errno.h>
#include "memory.h"
#include "picture.h"


static void
on_png_error(
	png_structp png_ptr,
	png_const_charp error_message
) {
	const char* path = (const char*)png_get_error_ptr(png_ptr);

	SDL_LogError(
		SDL_LOG_CATEGORY_SYSTEM,
		"While reading file '%s': %s",
		path,
		error_message
	);
}


static void
on_png_warning(
	png_structp png_ptr,
	png_const_charp error_message
) {
	const char* path = (const char*)png_get_error_ptr(png_ptr);

	SDL_LogWarn(
		SDL_LOG_CATEGORY_SYSTEM,
		"While reading file '%s': %s",
		path,
		error_message
	);
}


SDL_Surface*
load_png(const char* path) {
	bool success = true;
	SDL_Surface* surface = 0;
	png_bytep* row_pointers = 0;
	png_structp png = 0;
	png_infop info = 0;

	// Open file
	FILE* fp = fopen(path, "rb");
	if (!fp) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"Unable to open file '%s': %s",
			path,
			strerror(errno)
		);
		goto termination;
	}

	// Allocate libpng data
	png = png_create_read_struct(
		PNG_LIBPNG_VER_STRING,
		(png_voidp)path,
		on_png_error,
		on_png_warning
	);
	if (!png)
		goto termination;
	
	info = png_create_info_struct(png);
	if (!info)
		goto termination;

 	if (setjmp(png_jmpbuf(png))) {
 		success = false;
 		goto termination;
	}

	// Read PNG header
	png_init_io(png, fp);
	png_read_info(png, info);

	int width           = png_get_image_width(png, info);
	int height          = png_get_image_height(png, info);
	png_byte color_type = png_get_color_type(png, info);
	png_byte bit_depth  = png_get_bit_depth(png, info);

	// Create the surface
	surface = SDL_CreateRGBSurfaceWithFormat(
		0,
		width,
		height,
		32,
		SDL_PIXELFORMAT_RGBA32
	);

	// Setup libpng to get picture data in RGBA format
	if (bit_depth == 16)
		png_set_strip_16(png);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (
		color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE
	)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if (
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA
	)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	// Generate point to each row
	row_pointers = (png_bytep*)checked_malloc(sizeof(png_bytep) * height);

	png_bytep row_ptr = (png_bytep)surface->pixels;
	for(int i = 0; i < height; ++i, row_ptr += surface->pitch)
		row_pointers[i] = row_ptr;

	png_read_image(png, row_pointers);

	// Free ressources
termination:
	if (fp)
		fclose(fp);

	png_destroy_read_struct(&png, &info, 0);

	if (row_pointers)
		free(row_pointers);

	if ((surface != 0) && (!success)) {
		SDL_FreeSurface(surface);
		surface = 0;
	}

	// Job done
	return surface;
}

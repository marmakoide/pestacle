#ifndef PESTACLE_PLUGIN_PICTURE_H
#define PESTACLE_PLUGIN_PICTURE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <SDL.h>


extern SDL_Surface*
load_png(
	const char* path
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_PICTURE_H */

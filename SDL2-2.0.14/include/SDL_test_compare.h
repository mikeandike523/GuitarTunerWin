 
#ifndef SDL_test_compare_h_
#define SDL_test_compare_h_
#include "SDL.h"
#include "SDL_test_images.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif
int SDLTest_CompareSurfaces(SDL_Surface *surface, SDL_Surface *referenceSurface, int allowable_error);
#ifdef __cplusplus
}
#endif
#include "close_code.h"
#endif  

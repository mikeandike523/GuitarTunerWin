 
#ifndef SDL_test_font_h_
#define SDL_test_font_h_
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FONT_CHARACTER_SIZE  8
int SDLTest_DrawCharacter(SDL_Renderer *renderer, int x, int y, char c);
int SDLTest_DrawString(SDL_Renderer *renderer, int x, int y, const char *s);
void SDLTest_CleanupTextDrawing(void);
#ifdef __cplusplus
}
#endif
#include "close_code.h"
#endif  

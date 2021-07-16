 
#ifndef SDL_rect_h_
#define SDL_rect_h_
#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_pixels.h"
#include "SDL_rwops.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_Point
{
int x;
int y;
} SDL_Point;
typedef struct SDL_FPoint
{
float x;
float y;
} SDL_FPoint;
typedef struct SDL_Rect
{
int x, y;
int w, h;
} SDL_Rect;
typedef struct SDL_FRect
{
float x;
float y;
float w;
float h;
} SDL_FRect;
SDL_FORCE_INLINE SDL_bool SDL_PointInRect(const SDL_Point *p, const SDL_Rect *r)
{
return ( (p->x >= r->x) && (p->x < (r->x + r->w)) &&
(p->y >= r->y) && (p->y < (r->y + r->h)) ) ? SDL_TRUE : SDL_FALSE;
}
SDL_FORCE_INLINE SDL_bool SDL_RectEmpty(const SDL_Rect *r)
{
return ((!r) || (r->w <= 0) || (r->h <= 0)) ? SDL_TRUE : SDL_FALSE;
}
SDL_FORCE_INLINE SDL_bool SDL_RectEquals(const SDL_Rect *a, const SDL_Rect *b)
{
return (a && b && (a->x == b->x) && (a->y == b->y) &&
(a->w == b->w) && (a->h == b->h)) ? SDL_TRUE : SDL_FALSE;
}
extern DECLSPEC SDL_bool SDLCALL SDL_HasIntersection(const SDL_Rect * A,
const SDL_Rect * B);
extern DECLSPEC SDL_bool SDLCALL SDL_IntersectRect(const SDL_Rect * A,
const SDL_Rect * B,
SDL_Rect * result);
extern DECLSPEC void SDLCALL SDL_UnionRect(const SDL_Rect * A,
const SDL_Rect * B,
SDL_Rect * result);
extern DECLSPEC SDL_bool SDLCALL SDL_EnclosePoints(const SDL_Point * points,
int count,
const SDL_Rect * clip,
SDL_Rect * result);
extern DECLSPEC SDL_bool SDLCALL SDL_IntersectRectAndLine(const SDL_Rect *
rect, int *X1,
int *Y1, int *X2,
int *Y2);
#ifdef __cplusplus
}
#endif
#include "close_code.h"
#endif  

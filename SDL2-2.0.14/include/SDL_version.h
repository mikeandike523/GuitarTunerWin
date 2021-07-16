 
#ifndef SDL_version_h_
#define SDL_version_h_
#include "SDL_stdinc.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_version
{
Uint8 major;         
Uint8 minor;         
Uint8 patch;         
} SDL_version;
#define SDL_MAJOR_VERSION   2
#define SDL_MINOR_VERSION   0
#define SDL_PATCHLEVEL      14
#define SDL_VERSION(x)                          \
{                                   \
(x)->major = SDL_MAJOR_VERSION;                 \
(x)->minor = SDL_MINOR_VERSION;                 \
(x)->patch = SDL_PATCHLEVEL;                    \
}
#define SDL_VERSIONNUM(X, Y, Z)                     \
((X)*1000 + (Y)*100 + (Z))
#define SDL_COMPILEDVERSION \
SDL_VERSIONNUM(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL)
#define SDL_VERSION_ATLEAST(X, Y, Z) \
(SDL_COMPILEDVERSION >= SDL_VERSIONNUM(X, Y, Z))
extern DECLSPEC void SDLCALL SDL_GetVersion(SDL_version * ver);
extern DECLSPEC const char *SDLCALL SDL_GetRevision(void);
extern DECLSPEC int SDLCALL SDL_GetRevisionNumber(void);
#ifdef __cplusplus
}
#endif
#include "close_code.h"
#endif  

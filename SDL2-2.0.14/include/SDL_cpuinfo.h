 
#ifndef SDL_cpuinfo_h_
#define SDL_cpuinfo_h_
#include "SDL_stdinc.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1500) && (defined(_M_IX86) || defined(_M_X64))
#ifdef __clang__
#undef __MMX__
#undef __SSE__
#undef __SSE2__
#else
#include <intrin.h>
#ifndef _WIN64
#ifndef __MMX__
#define __MMX__
#endif
#ifndef __3dNOW__
#define __3dNOW__
#endif
#endif
#ifndef __SSE__
#define __SSE__
#endif
#ifndef __SSE2__
#define __SSE2__
#endif
#endif  
#elif defined(__MINGW64_VERSION_MAJOR)
#include <intrin.h>
#else
#if defined(HAVE_ALTIVEC_H) && defined(__ALTIVEC__) && !defined(__APPLE_ALTIVEC__) && defined(SDL_ENABLE_ALTIVEC_H)
#include <altivec.h>
#endif
#if !defined(SDL_DISABLE_ARM_NEON_H)
#  if defined(__ARM_NEON)
#    include <arm_neon.h>
#  elif defined(__WINDOWS__) || defined(__WINRT__)
#    if defined(_M_ARM)
#      include <armintr.h>
#      include <arm_neon.h>
#      define __ARM_NEON 1  
#    endif
#    if defined (_M_ARM64)
#      include <arm64intr.h>
#      include <arm64_neon.h>
#      define __ARM_NEON 1  
#    endif
#  endif
#endif
#if defined(__3dNOW__) && !defined(SDL_DISABLE_MM3DNOW_H)
#include <mm3dnow.h>
#endif
#if defined(HAVE_IMMINTRIN_H) && !defined(SDL_DISABLE_IMMINTRIN_H)
#include <immintrin.h>
#else
#if defined(__MMX__) && !defined(SDL_DISABLE_MMINTRIN_H)
#include <mmintrin.h>
#endif
#if defined(__SSE__) && !defined(SDL_DISABLE_XMMINTRIN_H)
#include <xmmintrin.h>
#endif
#if defined(__SSE2__) && !defined(SDL_DISABLE_EMMINTRIN_H)
#include <emmintrin.h>
#endif
#if defined(__SSE3__) && !defined(SDL_DISABLE_PMMINTRIN_H)
#include <pmmintrin.h>
#endif
#endif  
#endif  
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif
#define SDL_CACHELINE_SIZE  128
extern DECLSPEC int SDLCALL SDL_GetCPUCount(void);
extern DECLSPEC int SDLCALL SDL_GetCPUCacheLineSize(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasRDTSC(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasAltiVec(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasMMX(void);
extern DECLSPEC SDL_bool SDLCALL SDL_Has3DNow(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasSSE(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasSSE2(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasSSE3(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasSSE41(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasSSE42(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasAVX(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasAVX2(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasAVX512F(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasARMSIMD(void);
extern DECLSPEC SDL_bool SDLCALL SDL_HasNEON(void);
extern DECLSPEC int SDLCALL SDL_GetSystemRAM(void);
extern DECLSPEC size_t SDLCALL SDL_SIMDGetAlignment(void);
extern DECLSPEC void * SDLCALL SDL_SIMDAlloc(const size_t len);
extern DECLSPEC void * SDLCALL SDL_SIMDRealloc(void *mem, const size_t len);
extern DECLSPEC void SDLCALL SDL_SIMDFree(void *ptr);
#ifdef __cplusplus
}
#endif
#include "close_code.h"
#endif  

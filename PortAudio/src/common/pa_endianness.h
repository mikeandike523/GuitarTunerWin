#ifndef PA_ENDIANNESS_H
#define PA_ENDIANNESS_H
#ifdef __cplusplus
extern "C"
{
#endif  
#if defined(__APPLE__)
#if defined(__LITTLE_ENDIAN__)
#if !defined( PA_LITTLE_ENDIAN )
#define PA_LITTLE_ENDIAN
#endif
#if defined( PA_BIG_ENDIAN )
#undef PA_BIG_ENDIAN
#endif
#else
#if !defined( PA_BIG_ENDIAN )
#define PA_BIG_ENDIAN
#endif
#if defined( PA_LITTLE_ENDIAN )
#undef PA_LITTLE_ENDIAN
#endif
#endif
#else
#if defined(PA_LITTLE_ENDIAN) || defined(PA_BIG_ENDIAN)
#if defined(PA_LITTLE_ENDIAN) && defined(PA_BIG_ENDIAN)
#error both PA_LITTLE_ENDIAN and PA_BIG_ENDIAN have been defined externally to pa_endianness.h - only one endianness at a time please
#endif
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(LITTLE_ENDIAN) || defined(__i386) || defined(_M_IX86) || defined(__x86_64__)
#define PA_LITTLE_ENDIAN  
#else
#define PA_BIG_ENDIAN
#endif
#endif
#if !defined(PA_LITTLE_ENDIAN) && !defined(PA_BIG_ENDIAN)
#error pa_endianness.h was unable to automatically determine the endianness of the target platform
#endif
#endif
#if defined(NDEBUG)
#define PA_VALIDATE_ENDIANNESS
#else
#if defined(PA_LITTLE_ENDIAN)
#define PA_VALIDATE_ENDIANNESS \
{ \
const long nativeOne = 1; \
assert( "PortAudio: compile time and runtime endianness don't match" && (((char *)&nativeOne)[0]) == 1 ); \
}
#elif defined(PA_BIG_ENDIAN)
#define PA_VALIDATE_ENDIANNESS \
{ \
const long nativeOne = 1; \
assert( "PortAudio: compile time and runtime endianness don't match" && (((char *)&nativeOne)[0]) == 0 ); \
}
#endif
#endif
#ifdef __cplusplus
}
#endif  
#endif  

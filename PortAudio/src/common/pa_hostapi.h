#ifndef PA_HOSTAPI_H
#define PA_HOSTAPI_H
#include "portaudio.h"
#ifndef PA_USE_SKELETON
#define PA_USE_SKELETON 0
#elif (PA_USE_SKELETON != 0) && (PA_USE_SKELETON != 1)
#undef PA_USE_SKELETON
#define PA_USE_SKELETON 1
#endif
#if defined(PA_NO_ASIO) || defined(PA_NO_DS) || defined(PA_NO_WMME) || defined(PA_NO_WASAPI) || defined(PA_NO_WDMKS)
#error "Portaudio: PA_NO_<APINAME> is no longer supported, please remove definition and use PA_USE_<APINAME> instead"
#endif
#ifndef PA_USE_ASIO
#define PA_USE_ASIO 0
#elif (PA_USE_ASIO != 0) && (PA_USE_ASIO != 1)
#undef PA_USE_ASIO
#define PA_USE_ASIO 1
#endif
#ifndef PA_USE_DS
#define PA_USE_DS 0
#elif (PA_USE_DS != 0) && (PA_USE_DS != 1)
#undef PA_USE_DS
#define PA_USE_DS 1
#endif
#ifndef PA_USE_WMME
#define PA_USE_WMME 0
#elif (PA_USE_WMME != 0) && (PA_USE_WMME != 1)
#undef PA_USE_WMME
#define PA_USE_WMME 1
#endif
#ifndef PA_USE_WASAPI
#define PA_USE_WASAPI 0
#elif (PA_USE_WASAPI != 0) && (PA_USE_WASAPI != 1)
#undef PA_USE_WASAPI
#define PA_USE_WASAPI 1
#endif
#ifndef PA_USE_WDMKS
#define PA_USE_WDMKS 0
#elif (PA_USE_WDMKS != 0) && (PA_USE_WDMKS != 1)
#undef PA_USE_WDMKS
#define PA_USE_WDMKS 1
#endif
#if defined(PA_NO_OSS) || defined(PA_NO_ALSA) || defined(PA_NO_JACK) || defined(PA_NO_COREAUDIO) || defined(PA_NO_SGI) || defined(PA_NO_ASIHPI)
#error "Portaudio: PA_NO_<APINAME> is no longer supported, please remove definition and use PA_USE_<APINAME> instead"
#endif
#ifndef PA_USE_OSS
#define PA_USE_OSS 0
#elif (PA_USE_OSS != 0) && (PA_USE_OSS != 1)
#undef PA_USE_OSS
#define PA_USE_OSS 1
#endif
#ifndef PA_USE_ALSA
#define PA_USE_ALSA 0
#elif (PA_USE_ALSA != 0) && (PA_USE_ALSA != 1)
#undef PA_USE_ALSA
#define PA_USE_ALSA 1
#endif
#ifndef PA_USE_JACK
#define PA_USE_JACK 0
#elif (PA_USE_JACK != 0) && (PA_USE_JACK != 1)
#undef PA_USE_JACK
#define PA_USE_JACK 1
#endif
#ifndef PA_USE_SGI
#define PA_USE_SGI 0
#elif (PA_USE_SGI != 0) && (PA_USE_SGI != 1)
#undef PA_USE_SGI
#define PA_USE_SGI 1
#endif
#ifndef PA_USE_COREAUDIO
#define PA_USE_COREAUDIO 0
#elif (PA_USE_COREAUDIO != 0) && (PA_USE_COREAUDIO != 1)
#undef PA_USE_COREAUDIO
#define PA_USE_COREAUDIO 1
#endif
#ifndef PA_USE_ASIHPI
#define PA_USE_ASIHPI 0
#elif (PA_USE_ASIHPI != 0) && (PA_USE_ASIHPI != 1)
#undef PA_USE_ASIHPI
#define PA_USE_ASIHPI 1
#endif
#ifdef __cplusplus
extern "C"
{
#endif  
typedef struct PaUtilPrivatePaFrontHostApiInfo {
unsigned long baseDeviceIndex;
}PaUtilPrivatePaFrontHostApiInfo;
typedef struct PaUtilHostApiSpecificStreamInfoHeader
{
unsigned long size;              
PaHostApiTypeId hostApiType;     
unsigned long version;           
} PaUtilHostApiSpecificStreamInfoHeader;
typedef struct PaUtilHostApiRepresentation {
PaUtilPrivatePaFrontHostApiInfo privatePaFrontInfo;
PaHostApiInfo info;
PaDeviceInfo** deviceInfos;
void (*Terminate)( struct PaUtilHostApiRepresentation *hostApi );
PaError (*OpenStream)( struct PaUtilHostApiRepresentation *hostApi,
PaStream** stream,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerCallback,
PaStreamFlags streamFlags,
PaStreamCallback *streamCallback,
void *userData );
PaError (*IsFormatSupported)( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate );
} PaUtilHostApiRepresentation;
typedef PaError PaUtilHostApiInitializer( PaUtilHostApiRepresentation**, PaHostApiIndex );
extern PaUtilHostApiInitializer *paHostApiInitializers[];
#ifdef __cplusplus
}
#endif  
#endif  

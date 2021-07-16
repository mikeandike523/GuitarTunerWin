 
#ifdef PORTAUDIO_CMAKE_GENERATED
#include "options_cmake.h"
#endif
#include "pa_hostapi.h"
#ifdef __cplusplus
extern "C"
{
#endif  
PaError PaSkeleton_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaWinMme_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaWinDs_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaAsio_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaWinWdm_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaWasapi_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
#ifdef __cplusplus
}
#endif  
PaUtilHostApiInitializer *paHostApiInitializers[] =
{
#if PA_USE_WMME
PaWinMme_Initialize,
#endif
#if PA_USE_DS
PaWinDs_Initialize,
#endif
#if PA_USE_ASIO
PaAsio_Initialize,
#endif
#if PA_USE_WASAPI
PaWasapi_Initialize,
#endif
#if PA_USE_WDMKS
PaWinWdm_Initialize,
#endif
#if PA_USE_SKELETON
PaSkeleton_Initialize,  
#endif
0    
};

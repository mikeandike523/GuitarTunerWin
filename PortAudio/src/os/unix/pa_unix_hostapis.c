 
#include "pa_hostapi.h"
PaError PaJack_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaAlsa_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaOSS_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaSGI_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaAsiHpi_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaMacCore_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaSkeleton_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaUtilHostApiInitializer *paHostApiInitializers[] =
{
#ifdef __linux__
#if PA_USE_ALSA
PaAlsa_Initialize,
#endif
#if PA_USE_OSS
PaOSS_Initialize,
#endif
#else    
#if PA_USE_OSS
PaOSS_Initialize,
#endif
#if PA_USE_ALSA
PaAlsa_Initialize,
#endif
#endif   
#if PA_USE_JACK
PaJack_Initialize,
#endif
#if PA_USE_SGI
PaSGI_Initialize,
#endif
#if PA_USE_ASIHPI
PaAsiHpi_Initialize,
#endif
#if PA_USE_COREAUDIO
PaMacCore_Initialize,
#endif
#if PA_USE_SKELETON
PaSkeleton_Initialize,
#endif
0    
};

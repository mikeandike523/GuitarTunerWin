#ifndef PA_UTIL_H
#define PA_UTIL_H
#include "portaudio.h"
#ifdef __cplusplus
extern "C"
{
#endif  
struct PaUtilHostApiRepresentation;
PaError PaUtil_GetHostApiRepresentation( struct PaUtilHostApiRepresentation **hostApi,
PaHostApiTypeId type );
PaError PaUtil_DeviceIndexToHostApiDeviceIndex(
PaDeviceIndex *hostApiDevice, PaDeviceIndex device,
struct PaUtilHostApiRepresentation *hostApi );
void PaUtil_SetLastHostErrorInfo( PaHostApiTypeId hostApiType, long errorCode,
const char *errorText );
void *PaUtil_AllocateMemory( long size );
void PaUtil_FreeMemory( void *block );
int PaUtil_CountCurrentlyAllocatedBlocks( void );
void PaUtil_InitializeClock( void );
double PaUtil_GetTime( void );
#ifdef __cplusplus
}
#endif  
#endif  

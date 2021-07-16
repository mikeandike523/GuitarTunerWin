 
#ifndef PA_MAC_CORE_UTILITIES_H__
#define PA_MAC_CORE_UTILITIES_H__
#include <pthread.h>
#include "portaudio.h"
#include "pa_util.h"
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>
#ifndef MIN
#define MIN(a, b)  (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a, b)  (((a)<(b))?(b):(a))
#endif
#define ERR(mac_error) PaMacCore_SetError(mac_error, __LINE__, 1 )
#define WARNING(mac_error) PaMacCore_SetError(mac_error, __LINE__, 0 )
#define INPUT_ELEMENT  (1)
#define OUTPUT_ELEMENT (0)
#define MAC_CORE_DEBUG
#ifdef MAC_CORE_DEBUG
# define DBUG(MSG) do { printf("||PaMacCore (AUHAL)|| "); printf MSG ; fflush(stdout); } while(0)
#else
# define DBUG(MSG)
#endif
#ifdef MAC_CORE_VERBOSE_DEBUG
# define VDBUG(MSG) do { printf("||PaMacCore (v )|| "); printf MSG ; fflush(stdout); } while(0)
#else
# define VDBUG(MSG)
#endif
#ifdef MAC_CORE_VERY_VERBOSE_DEBUG
# define VVDBUG(MSG) do { printf("||PaMacCore (vv)|| "); printf MSG ; fflush(stdout); } while(0)
#else
# define VVDBUG(MSG)
#endif
OSStatus PaMacCore_AudioHardwareGetProperty(
AudioHardwarePropertyID inPropertyID,
UInt32*                 ioPropertyDataSize,
void*                   outPropertyData );
OSStatus PaMacCore_AudioHardwareGetPropertySize(
AudioHardwarePropertyID inPropertyID,
UInt32*                 outSize );
OSStatus PaMacCore_AudioDeviceGetProperty(
AudioDeviceID         inDevice,
UInt32                inChannel,
Boolean               isInput,
AudioDevicePropertyID inPropertyID,
UInt32*               ioPropertyDataSize,
void*                 outPropertyData );
OSStatus PaMacCore_AudioDeviceSetProperty(
AudioDeviceID         inDevice,
const AudioTimeStamp* inWhen,
UInt32                inChannel,
Boolean               isInput,
AudioDevicePropertyID inPropertyID,
UInt32                inPropertyDataSize,
const void*           inPropertyData );
OSStatus PaMacCore_AudioDeviceGetPropertySize(
AudioDeviceID         inDevice,
UInt32                inChannel,
Boolean               isInput,
AudioDevicePropertyID inPropertyID,
UInt32*               outSize );
OSStatus PaMacCore_AudioDeviceAddPropertyListener(
AudioDeviceID                   inDevice,
UInt32                          inChannel,
Boolean                         isInput,
AudioDevicePropertyID           inPropertyID,
AudioObjectPropertyListenerProc inProc,
void*                           inClientData );
OSStatus PaMacCore_AudioDeviceRemovePropertyListener(
AudioDeviceID                   inDevice,
UInt32                          inChannel,
Boolean                         isInput,
AudioDevicePropertyID           inPropertyID,
AudioObjectPropertyListenerProc inProc,
void*                           inClientData );
OSStatus PaMacCore_AudioStreamGetProperty(
AudioStreamID         inStream,
UInt32                inChannel,
AudioDevicePropertyID inPropertyID,
UInt32*               ioPropertyDataSize,
void*                 outPropertyData );
#define UNIX_ERR(err) PaMacCore_SetUnixError( err, __LINE__ )
PaError PaMacCore_SetUnixError( int err, int line );
PaError PaMacCore_SetError(OSStatus error, int line, int isError);
long computeRingBufferSize( const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
long inputFramesPerBuffer,
long outputFramesPerBuffer,
double sampleRate );
OSStatus propertyProc(
AudioObjectID inObjectID,
UInt32 inNumberAddresses,
const AudioObjectPropertyAddress* inAddresses,
void* inClientData );
PaError AudioDeviceSetPropertyNowAndWaitForChange(
AudioDeviceID inDevice,
UInt32 inChannel,
Boolean isInput,
AudioDevicePropertyID inPropertyID,
UInt32 inPropertyDataSize,
const void *inPropertyData,
void *outPropertyData );
PaError setBestSampleRateForDevice( const AudioDeviceID device,
const bool isOutput,
const bool requireExact,
const Float64 desiredSrate );
PaError setBestFramesPerBuffer( const AudioDeviceID device,
const bool isOutput,
UInt32 requestedFramesPerBuffer,
UInt32 *actualFramesPerBuffer );
OSStatus xrunCallback(
AudioObjectID inObjectID,
UInt32 inNumberAddresses,
const AudioObjectPropertyAddress* inAddresses,
void * inClientData );
int initializeXRunListenerList( void );
int destroyXRunListenerList( void );
void *addToXRunListenerList( void *stream );
int removeFromXRunListenerList( void *stream );
#endif  

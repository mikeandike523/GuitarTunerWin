 
#include "pa_mac_core_utilities.h"
#include "pa_mac_core_internal.h"
#include <libkern/OSAtomic.h>
#include <strings.h>
#include <pthread.h>
#include <sys/time.h>
OSStatus PaMacCore_AudioHardwareGetProperty(
AudioHardwarePropertyID inPropertyID,
UInt32*                 ioPropertyDataSize,
void*                   outPropertyData )
{
AudioObjectPropertyAddress address = { inPropertyID, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
return AudioObjectGetPropertyData(kAudioObjectSystemObject, &address, 0, NULL, ioPropertyDataSize, outPropertyData);
}
OSStatus PaMacCore_AudioHardwareGetPropertySize(
AudioHardwarePropertyID inPropertyID,
UInt32*                 outSize )
{
AudioObjectPropertyAddress address = { inPropertyID, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
return AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &address, 0, NULL, outSize);
}
OSStatus PaMacCore_AudioDeviceGetProperty(
AudioDeviceID         inDevice,
UInt32                inChannel,
Boolean               isInput,
AudioDevicePropertyID inPropertyID,
UInt32*               ioPropertyDataSize,
void*                 outPropertyData )
{
AudioObjectPropertyScope scope = isInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
AudioObjectPropertyAddress address = { inPropertyID, scope, inChannel };
return AudioObjectGetPropertyData(inDevice, &address, 0, NULL, ioPropertyDataSize, outPropertyData);
}
OSStatus PaMacCore_AudioDeviceSetProperty(
AudioDeviceID         inDevice,
const AudioTimeStamp* inWhen,
UInt32                inChannel,
Boolean               isInput,
AudioDevicePropertyID inPropertyID,
UInt32                inPropertyDataSize,
const void*           inPropertyData )
{
AudioObjectPropertyScope scope = isInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
AudioObjectPropertyAddress address = { inPropertyID, scope, inChannel };
return AudioObjectSetPropertyData(inDevice, &address, 0, NULL, inPropertyDataSize, inPropertyData);
}
OSStatus PaMacCore_AudioDeviceGetPropertySize(
AudioDeviceID         inDevice,
UInt32                inChannel,
Boolean               isInput,
AudioDevicePropertyID inPropertyID,
UInt32*               outSize )
{
AudioObjectPropertyScope scope = isInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
AudioObjectPropertyAddress address = { inPropertyID, scope, inChannel };
return AudioObjectGetPropertyDataSize(inDevice, &address, 0, NULL, outSize);
}
OSStatus PaMacCore_AudioDeviceAddPropertyListener(
AudioDeviceID                   inDevice,
UInt32                          inChannel,
Boolean                         isInput,
AudioDevicePropertyID           inPropertyID,
AudioObjectPropertyListenerProc inProc,
void*                           inClientData )
{
AudioObjectPropertyScope scope = isInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
AudioObjectPropertyAddress address = { inPropertyID, scope, inChannel };
return AudioObjectAddPropertyListener(inDevice, &address, inProc, inClientData);
}
OSStatus PaMacCore_AudioDeviceRemovePropertyListener(
AudioDeviceID                   inDevice,
UInt32                          inChannel,
Boolean                         isInput,
AudioDevicePropertyID           inPropertyID,
AudioObjectPropertyListenerProc inProc,
void*                           inClientData )
{
AudioObjectPropertyScope scope = isInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
AudioObjectPropertyAddress address = { inPropertyID, scope, inChannel };
return AudioObjectRemovePropertyListener(inDevice, &address, inProc, inClientData);
}
OSStatus PaMacCore_AudioStreamGetProperty(
AudioStreamID         inStream,
UInt32                inChannel,
AudioDevicePropertyID inPropertyID,
UInt32*               ioPropertyDataSize,
void*                 outPropertyData )
{
AudioObjectPropertyAddress address = { inPropertyID, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
return AudioObjectGetPropertyData(inStream, &address, 0, NULL, ioPropertyDataSize, outPropertyData);
}
PaError PaMacCore_SetUnixError( int err, int line )
{
PaError ret;
const char *errorText;
if( err == 0 )
{
return paNoError;
}
ret = paNoError;
errorText = strerror( err );
if( err == ENOMEM )
ret = paInsufficientMemory;
else
ret = paInternalError;
DBUG(("%d on line %d: msg='%s'\n", err, line, errorText));
PaUtil_SetLastHostErrorInfo( paCoreAudio, err, errorText );
return ret;
}
PaError PaMacCore_SetError(OSStatus error, int line, int isError)
{
PaError result;
const char *errorType;
const char *errorText;
switch (error) {
case kAudioHardwareNoError:
return paNoError;
case kAudioHardwareNotRunningError:
errorText = "Audio Hardware Not Running";
result = paInternalError;
break;
case kAudioHardwareUnspecifiedError:
errorText = "Unspecified Audio Hardware Error";
result = paInternalError;
break;
case kAudioHardwareUnknownPropertyError:
errorText = "Audio Hardware: Unknown Property";
result = paInternalError;
break;
case kAudioHardwareBadPropertySizeError:
errorText = "Audio Hardware: Bad Property Size";
result = paInternalError;
break;
case kAudioHardwareIllegalOperationError:
errorText = "Audio Hardware: Illegal Operation";
result = paInternalError;
break;
case kAudioHardwareBadDeviceError:
errorText = "Audio Hardware: Bad Device";
result = paInvalidDevice;
break;
case kAudioHardwareBadStreamError:
errorText = "Audio Hardware: BadStream";
result = paBadStreamPtr;
break;
case kAudioHardwareUnsupportedOperationError:
errorText = "Audio Hardware: Unsupported Operation";
result = paInternalError;
break;
case kAudioDeviceUnsupportedFormatError:
errorText = "Audio Device: Unsupported Format";
result = paSampleFormatNotSupported;
break;
case kAudioDevicePermissionsError:
errorText = "Audio Device: Permissions Error";
result = paDeviceUnavailable;
break;
case kAudioUnitErr_InvalidProperty:
errorText = "Audio Unit: Invalid Property";
result = paInternalError;
break;
case kAudioUnitErr_InvalidParameter:
errorText = "Audio Unit: Invalid Parameter";
result = paInternalError;
break;
case kAudioUnitErr_NoConnection:
errorText = "Audio Unit: No Connection";
result = paInternalError;
break;
case kAudioUnitErr_FailedInitialization:
errorText = "Audio Unit: Initialization Failed";
result = paInternalError;
break;
case kAudioUnitErr_TooManyFramesToProcess:
errorText = "Audio Unit: Too Many Frames";
result = paInternalError;
break;
case kAudioUnitErr_InvalidFile:
errorText = "Audio Unit: Invalid File";
result = paInternalError;
break;
case kAudioUnitErr_UnknownFileType:
errorText = "Audio Unit: Unknown File Type";
result = paInternalError;
break;
case kAudioUnitErr_FileNotSpecified:
errorText = "Audio Unit: File Not Specified";
result = paInternalError;
break;
case kAudioUnitErr_FormatNotSupported:
errorText = "Audio Unit: Format Not Supported";
result = paInternalError;
break;
case kAudioUnitErr_Uninitialized:
errorText = "Audio Unit: Uninitialized";
result = paInternalError;
break;
case kAudioUnitErr_InvalidScope:
errorText = "Audio Unit: Invalid Scope";
result = paInternalError;
break;
case kAudioUnitErr_PropertyNotWritable:
errorText = "Audio Unit: PropertyNotWritable";
result = paInternalError;
break;
case kAudioUnitErr_InvalidPropertyValue:
errorText = "Audio Unit: Invalid Property Value";
result = paInternalError;
break;
case kAudioUnitErr_PropertyNotInUse:
errorText = "Audio Unit: Property Not In Use";
result = paInternalError;
break;
case kAudioUnitErr_Initialized:
errorText = "Audio Unit: Initialized";
result = paInternalError;
break;
case kAudioUnitErr_InvalidOfflineRender:
errorText = "Audio Unit: Invalid Offline Render";
result = paInternalError;
break;
case kAudioUnitErr_Unauthorized:
errorText = "Audio Unit: Unauthorized";
result = paInternalError;
break;
case kAudioUnitErr_CannotDoInCurrentContext:
errorText = "Audio Unit: cannot do in current context";
result = paInternalError;
break;
default:
errorText = "Unknown Error";
result = paInternalError;
}
if (isError)
errorType = "Error";
else
errorType = "Warning";
char str[20];
*(UInt32 *)(str + 1) = CFSwapInt32HostToBig(error);
if (isprint(str[1]) && isprint(str[2]) && isprint(str[3]) && isprint(str[4]))
{
str[0] = str[5] = '\'';
str[6] = '\0';
} else {
sprintf(str, "%d", (int)error);
}
DBUG(("%s on line %d: err='%s', msg=%s\n", errorType, line, str, errorText));
PaUtil_SetLastHostErrorInfo( paCoreAudio, error, errorText );
return result;
}
long computeRingBufferSize( const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
long inputFramesPerBuffer,
long outputFramesPerBuffer,
double sampleRate )
{
long ringSize;
int index;
int i;
double latency ;
long framesPerBuffer ;
VVDBUG(( "computeRingBufferSize()\n" ));
assert( inputParameters || outputParameters );
if( outputParameters && inputParameters )
{
latency = MAX( inputParameters->suggestedLatency, outputParameters->suggestedLatency );
framesPerBuffer = MAX( inputFramesPerBuffer, outputFramesPerBuffer );
}
else if( outputParameters )
{
latency = outputParameters->suggestedLatency;
framesPerBuffer = outputFramesPerBuffer ;
}
else  
{
latency = inputParameters->suggestedLatency;
framesPerBuffer = inputFramesPerBuffer ;
}
ringSize = (long) ( latency * sampleRate * 2 + .5);
VDBUG( ( "suggested latency : %d\n", (int) (latency*sampleRate) ) );
if( ringSize < framesPerBuffer * 3 )
ringSize = framesPerBuffer * 3 ;
VDBUG(("framesPerBuffer:%d\n",(int)framesPerBuffer));
VDBUG(("Ringbuffer size (1): %d\n", (int)ringSize ));
ringSize = MAX( ringSize, 4 );
index = -1;
for( i=0; i<sizeof(long)*8; ++i )
if( ringSize >> i & 0x01 )
index = i;
assert( index > 0 );
if( ringSize <= ( 0x01 << index ) )
ringSize = 0x01 << index ;
else
ringSize = 0x01 << ( index + 1 );
VDBUG(( "Final Ringbuffer size (2): %d\n", (int)ringSize ));
return ringSize;
}
OSStatus propertyProc(
AudioObjectID inObjectID,
UInt32 inNumberAddresses,
const AudioObjectPropertyAddress* inAddresses,
void* inClientData )
{
return noErr;
}
PaError AudioDeviceSetPropertyNowAndWaitForChange(
AudioDeviceID inDevice,
UInt32 inChannel,
Boolean isInput,
AudioDevicePropertyID inPropertyID,
UInt32 inPropertyDataSize,
const void *inPropertyData,
void *outPropertyData )
{
OSStatus macErr;
UInt32 outPropertyDataSize = inPropertyDataSize;
macErr = PaMacCore_AudioDeviceGetProperty( inDevice, inChannel,
isInput, inPropertyID,
&outPropertyDataSize, outPropertyData );
if( macErr ) {
memset( outPropertyData, 0, inPropertyDataSize );
goto failMac;
}
if( inPropertyDataSize!=outPropertyDataSize )
return paInternalError;
if( 0==memcmp( outPropertyData, inPropertyData, outPropertyDataSize ) )
return paNoError;
macErr = PaMacCore_AudioDeviceAddPropertyListener( inDevice, inChannel, isInput,
inPropertyID, propertyProc,
NULL );
if( macErr )
goto failMac;
macErr = PaMacCore_AudioDeviceSetProperty( inDevice, NULL, inChannel,
isInput, inPropertyID,
inPropertyDataSize, inPropertyData );
if( macErr )
goto failMac;
struct timeval tv1, tv2;
gettimeofday( &tv1, NULL );
memcpy( &tv2, &tv1, sizeof( struct timeval ) );
while( tv2.tv_sec - tv1.tv_sec < 30 ) {
macErr = PaMacCore_AudioDeviceGetProperty( inDevice, inChannel,
isInput, inPropertyID,
&outPropertyDataSize, outPropertyData );
if( macErr ) {
memset( outPropertyData, 0, inPropertyDataSize );
goto failMac;
}
if( 0==memcmp( outPropertyData, inPropertyData, outPropertyDataSize ) ) {
PaMacCore_AudioDeviceRemovePropertyListener( inDevice, inChannel, isInput, inPropertyID, propertyProc, NULL);
return paNoError;
}
Pa_Sleep( 100 );
gettimeofday( &tv2, NULL );
}
DBUG( ("Timeout waiting for device setting.\n" ) );
PaMacCore_AudioDeviceRemovePropertyListener( inDevice, inChannel, isInput, inPropertyID, propertyProc, NULL );
return paNoError;
failMac:
PaMacCore_AudioDeviceRemovePropertyListener( inDevice, inChannel, isInput, inPropertyID, propertyProc, NULL );
return ERR( macErr );
}
PaError setBestSampleRateForDevice( const AudioDeviceID device,
const bool isOutput,
const bool requireExact,
const Float64 desiredSrate )
{
const bool isInput = isOutput ? 0 : 1;
Float64 srate;
UInt32 propsize = sizeof( Float64 );
OSErr err;
AudioValueRange *ranges;
int i=0;
Float64 max  = -1;  
Float64 best = -1;  
VDBUG(("Setting sample rate for device %ld to %g.\n",(long)device,(float)desiredSrate));
srate = 0;
err = AudioDeviceSetPropertyNowAndWaitForChange(
device, 0, isInput,
kAudioDevicePropertyNominalSampleRate,
propsize, &desiredSrate, &srate );
if( srate != 0 && srate == desiredSrate )
return paNoError;
if( !err && srate == desiredSrate )
return paNoError;
if( requireExact )
return paInvalidSampleRate;
err = PaMacCore_AudioDeviceGetPropertySize( device, 0, isInput,
kAudioDevicePropertyAvailableNominalSampleRates,
&propsize );
if( err )
return ERR( err );
ranges = (AudioValueRange *)calloc( 1, propsize );
if( !ranges )
return paInsufficientMemory;
err = PaMacCore_AudioDeviceGetProperty( device, 0, isInput,
kAudioDevicePropertyAvailableNominalSampleRates,
&propsize, ranges );
if( err )
{
free( ranges );
return ERR( err );
}
VDBUG(("Requested sample rate of %g was not available.\n", (float)desiredSrate));
VDBUG(("%lu Available Sample Rates are:\n",propsize/sizeof(AudioValueRange)));
#ifdef MAC_CORE_VERBOSE_DEBUG
for( i=0; i<propsize/sizeof(AudioValueRange); ++i )
VDBUG( ("\t%g-%g\n",
(float) ranges[i].mMinimum,
(float) ranges[i].mMaximum ) );
#endif
VDBUG(("-----\n"));
for( i=0; i<propsize/sizeof(AudioValueRange); ++i )
{
if( ranges[i].mMaximum > max ) max = ranges[i].mMaximum;
if( ranges[i].mMinimum > desiredSrate ) {
if( best < 0 )
best = ranges[i].mMinimum;
else if( ranges[i].mMinimum < best )
best = ranges[i].mMinimum;
}
}
if( best < 0 )
best = max;
VDBUG( ("Maximum Rate %g. best is %g.\n", max, best ) );
free( ranges );
propsize = sizeof( best );
srate = 0;
err = AudioDeviceSetPropertyNowAndWaitForChange(
device, 0, isInput,
kAudioDevicePropertyNominalSampleRate,
propsize, &best, &srate );
if( srate != 0 && srate == best )
return paNoError;
if( err )
return ERR( err );
return paInternalError;
}
PaError setBestFramesPerBuffer( const AudioDeviceID device,
const bool isOutput,
UInt32 requestedFramesPerBuffer,
UInt32 *actualFramesPerBuffer )
{
UInt32 afpb;
const bool isInput = !isOutput;
UInt32 propsize = sizeof(UInt32);
OSErr err;
AudioValueRange range;
if( actualFramesPerBuffer == NULL )
{
actualFramesPerBuffer = &afpb;
}
err = PaMacCore_AudioDeviceSetProperty( device, NULL, 0, isInput,
kAudioDevicePropertyBufferFrameSize,
propsize, &requestedFramesPerBuffer);
err = PaMacCore_AudioDeviceGetProperty( device, 0, isInput,
kAudioDevicePropertyBufferFrameSize,
&propsize, actualFramesPerBuffer);
if( err )
{
return ERR( err );
}
if( *actualFramesPerBuffer == requestedFramesPerBuffer )
{
return paNoError;  
}
propsize = sizeof(AudioValueRange);
err = PaMacCore_AudioDeviceGetProperty( device, 0, isInput,
kAudioDevicePropertyBufferFrameSizeRange,
&propsize, &range );
if( err )
{
return ERR( err );
}
if( requestedFramesPerBuffer < range.mMinimum )
{
requestedFramesPerBuffer = range.mMinimum;
}
else if( requestedFramesPerBuffer > range.mMaximum )
{
requestedFramesPerBuffer = range.mMaximum;
}
propsize = sizeof( UInt32 );
err = PaMacCore_AudioDeviceSetProperty( device, NULL, 0, isInput,
kAudioDevicePropertyBufferFrameSize,
propsize, &requestedFramesPerBuffer );
err = PaMacCore_AudioDeviceGetProperty( device, 0, isInput,
kAudioDevicePropertyBufferFrameSize,
&propsize, actualFramesPerBuffer );
if( err )
return ERR( err );
return paNoError;
}
struct PaMacXRunListNode_s {
PaMacCoreStream *stream;
struct PaMacXRunListNode_s *next;
} ;
typedef struct PaMacXRunListNode_s PaMacXRunListNode;
static PaMacXRunListNode firstXRunListNode;
static int xRunListSize;
static pthread_mutex_t xrunMutex;
OSStatus xrunCallback(
AudioObjectID inDevice,
UInt32 inNumberAddresses,
const AudioObjectPropertyAddress* inAddresses,
void * inClientData)
{
PaMacXRunListNode *node = (PaMacXRunListNode *) inClientData;
bool isInput = inAddresses->mScope == kAudioDevicePropertyScopeInput;
int ret = pthread_mutex_trylock( &xrunMutex ) ;
if( ret == 0 ) {
node = node->next ;  
for( ; node; node=node->next ) {
PaMacCoreStream *stream = node->stream;
if( stream->state != ACTIVE )
continue;  
if( isInput ) {
if( stream->inputDevice == inDevice )
OSAtomicOr32( paInputOverflow, &stream->xrunFlags );
} else {
if( stream->outputDevice == inDevice )
OSAtomicOr32( paOutputUnderflow, &stream->xrunFlags );
}
}
pthread_mutex_unlock( &xrunMutex );
}
return 0;
}
int initializeXRunListenerList( void )
{
xRunListSize = 0;
bzero( (void *) &firstXRunListNode, sizeof(firstXRunListNode) );
return pthread_mutex_init( &xrunMutex, NULL );
}
int destroyXRunListenerList( void )
{
PaMacXRunListNode *node;
node = firstXRunListNode.next;
while( node ) {
PaMacXRunListNode *tmp = node;
node = node->next;
free( tmp );
}
xRunListSize = 0;
return pthread_mutex_destroy( &xrunMutex );
}
void *addToXRunListenerList( void *stream )
{
pthread_mutex_lock( &xrunMutex );
PaMacXRunListNode *newNode;
newNode = (PaMacXRunListNode *) malloc( sizeof( PaMacXRunListNode ) );
newNode->stream = (PaMacCoreStream *) stream;
newNode->next = firstXRunListNode.next;
firstXRunListNode.next = newNode;
pthread_mutex_unlock( &xrunMutex );
return &firstXRunListNode;
}
int removeFromXRunListenerList( void *stream )
{
pthread_mutex_lock( &xrunMutex );
PaMacXRunListNode *node, *prev;
prev = &firstXRunListNode;
node = firstXRunListNode.next;
while( node ) {
if( node->stream == stream ) {
--xRunListSize;
prev->next = node->next;
free( node );
pthread_mutex_unlock( &xrunMutex );
return xRunListSize;
}
prev = prev->next;
node = node->next;
}
pthread_mutex_unlock( &xrunMutex );
return xRunListSize;
}

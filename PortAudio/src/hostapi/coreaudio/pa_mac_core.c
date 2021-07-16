 
#include "pa_mac_core_internal.h"
#include <string.h>  
#include <libkern/OSAtomic.h>
#include "pa_mac_core.h"
#include "pa_mac_core_utilities.h"
#include "pa_mac_core_blocking.h"
#ifdef __cplusplus
extern "C"
{
#endif  
#define PA_MAC_SMALL_BUFFER_SIZE    (64)
PaError PaMacCore_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
void PaMacCore_SetupStreamInfo(  PaMacCoreStreamInfo *data, const unsigned long flags )
{
bzero( data, sizeof( PaMacCoreStreamInfo ) );
data->size = sizeof( PaMacCoreStreamInfo );
data->hostApiType = paCoreAudio;
data->version = 0x01;
data->flags = flags;
data->channelMap = NULL;
data->channelMapSize = 0;
}
void PaMacCore_SetupChannelMap( PaMacCoreStreamInfo *data, const SInt32 * const channelMap, const unsigned long channelMapSize )
{
data->channelMap = channelMap;
data->channelMapSize = channelMapSize;
}
static char *channelName = NULL;
static int channelNameSize = 0;
static bool ensureChannelNameSize( int size )
{
if( size >= channelNameSize ) {
free( channelName );
channelName = (char *) malloc( ( channelNameSize = size ) + 1 );
if( !channelName ) {
channelNameSize = 0;
return false;
}
}
return true;
}
const char *PaMacCore_GetChannelName( int device, int channelIndex, bool input )
{
struct PaUtilHostApiRepresentation *hostApi;
PaError err;
OSStatus error;
err = PaUtil_GetHostApiRepresentation( &hostApi, paCoreAudio );
assert(err == paNoError);
if( err != paNoError )
return NULL;
PaMacAUHAL *macCoreHostApi = (PaMacAUHAL*)hostApi;
AudioDeviceID hostApiDevice = macCoreHostApi->devIds[device];
CFStringRef nameRef;
UInt32 size = sizeof(nameRef);
error = PaMacCore_AudioDeviceGetProperty( hostApiDevice,
channelIndex + 1,
input,
kAudioDevicePropertyChannelNameCFString,
&size,
&nameRef );
if( error )
{
size = 0;
error = PaMacCore_AudioDeviceGetPropertySize( hostApiDevice,
channelIndex + 1,
input,
kAudioDevicePropertyChannelName,
&size );
if( !error )
{
if( !ensureChannelNameSize( size ) )
return NULL;
error = PaMacCore_AudioDeviceGetProperty( hostApiDevice,
channelIndex + 1,
input,
kAudioDevicePropertyChannelName,
&size,
channelName );
if( !error )
return channelName;
}
nameRef = CFStringCreateWithFormat( NULL, NULL, CFSTR( "%s: %d"), hostApi->deviceInfos[device]->name, channelIndex + 1 );
size = CFStringGetMaximumSizeForEncoding(CFStringGetLength(nameRef), kCFStringEncodingUTF8);;
if( !ensureChannelNameSize( size ) )
{
CFRelease( nameRef );
return NULL;
}
CFStringGetCString( nameRef, channelName, size+1, kCFStringEncodingUTF8 );
CFRelease( nameRef );
}
else
{
size = CFStringGetMaximumSizeForEncoding(CFStringGetLength(nameRef), kCFStringEncodingUTF8);;
if( !ensureChannelNameSize( size ) )
{
CFRelease( nameRef );
return NULL;
}
CFStringGetCString( nameRef, channelName, size+1, kCFStringEncodingUTF8 );
CFRelease( nameRef );
}
return channelName;
}
PaError PaMacCore_GetBufferSizeRange( PaDeviceIndex device,
long *minBufferSizeFrames, long *maxBufferSizeFrames )
{
PaError result;
PaUtilHostApiRepresentation *hostApi;
result = PaUtil_GetHostApiRepresentation( &hostApi, paCoreAudio );
if( result == paNoError )
{
PaDeviceIndex hostApiDeviceIndex;
result = PaUtil_DeviceIndexToHostApiDeviceIndex( &hostApiDeviceIndex, device, hostApi );
if( result == paNoError )
{
PaMacAUHAL *macCoreHostApi = (PaMacAUHAL*)hostApi;
AudioDeviceID macCoreDeviceId = macCoreHostApi->devIds[hostApiDeviceIndex];
AudioValueRange audioRange;
UInt32 propSize = sizeof( audioRange );
Boolean isInput = 0;
if( macCoreHostApi->inheritedHostApiRep.deviceInfos[hostApiDeviceIndex]->maxOutputChannels == 0 )
isInput = 1;
result = WARNING(PaMacCore_AudioDeviceGetProperty( macCoreDeviceId, 0, isInput, kAudioDevicePropertyBufferFrameSizeRange, &propSize, &audioRange ) );
*minBufferSizeFrames = audioRange.mMinimum;
*maxBufferSizeFrames = audioRange.mMaximum;
}
}
return result;
}
AudioDeviceID PaMacCore_GetStreamInputDevice( PaStream* s )
{
PaMacCoreStream *stream = (PaMacCoreStream*)s;
VVDBUG(("PaMacCore_GetStreamInputHandle()\n"));
return ( stream->inputDevice );
}
AudioDeviceID PaMacCore_GetStreamOutputDevice( PaStream* s )
{
PaMacCoreStream *stream = (PaMacCoreStream*)s;
VVDBUG(("PaMacCore_GetStreamOutputHandle()\n"));
return ( stream->outputDevice );
}
#ifdef __cplusplus
}
#endif  
#define RING_BUFFER_ADVANCE_DENOMINATOR (4)
static void Terminate( struct PaUtilHostApiRepresentation *hostApi );
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate );
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream** s,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback *streamCallback,
void *userData );
static PaError CloseStream( PaStream* stream );
static PaError StartStream( PaStream *stream );
static PaError StopStream( PaStream *stream );
static PaError AbortStream( PaStream *stream );
static PaError IsStreamStopped( PaStream *s );
static PaError IsStreamActive( PaStream *stream );
static PaTime GetStreamTime( PaStream *stream );
static OSStatus AudioIOProc( void *inRefCon,
AudioUnitRenderActionFlags *ioActionFlags,
const AudioTimeStamp *inTimeStamp,
UInt32 inBusNumber,
UInt32 inNumberFrames,
AudioBufferList *ioData );
static double GetStreamCpuLoad( PaStream* stream );
static PaError GetChannelInfo( PaMacAUHAL *auhalHostApi,
PaDeviceInfo *deviceInfo,
AudioDeviceID macCoreDeviceId,
int isInput);
static PaError OpenAndSetupOneAudioUnit( const PaMacCoreStream *stream,
const PaStreamParameters *inStreamParams,
const PaStreamParameters *outStreamParams,
const UInt32 requestedFramesPerBuffer,
UInt32 *actualInputFramesPerBuffer,
UInt32 *actualOutputFramesPerBuffer,
const PaMacAUHAL *auhalHostApi,
AudioUnit *audioUnit,
AudioConverterRef *srConverter,
AudioDeviceID *audioDevice,
const double sampleRate,
void *refCon );
#define PA_AUHAL_SET_LAST_HOST_ERROR( errorCode, errorText ) \
PaUtil_SetLastHostErrorInfo( paCoreAudio, errorCode, errorText )
static void startStopCallback(
void *               inRefCon,
AudioUnit            ci,
AudioUnitPropertyID  inID,
AudioUnitScope       inScope,
AudioUnitElement     inElement )
{
PaMacCoreStream *stream = (PaMacCoreStream *) inRefCon;
UInt32 isRunning;
UInt32 size = sizeof( isRunning );
OSStatus err;
err = AudioUnitGetProperty( ci, kAudioOutputUnitProperty_IsRunning, inScope, inElement, &isRunning, &size );
assert( !err );
if( err )
isRunning = false;  
if( isRunning )
return;  
if( stream->inputUnit && stream->outputUnit && stream->inputUnit != stream->outputUnit && ci == stream->inputUnit )
return;
PaStreamFinishedCallback *sfc = stream->streamRepresentation.streamFinishedCallback;
if( stream->state == STOPPING )
stream->state = STOPPED ;
if( sfc )
sfc( stream->streamRepresentation.userData );
}
static PaError gatherDeviceInfo(PaMacAUHAL *auhalHostApi)
{
UInt32 size;
UInt32 propsize;
VVDBUG(("gatherDeviceInfo()\n"));
if( auhalHostApi->devIds )
PaUtil_GroupFreeMemory(auhalHostApi->allocations, auhalHostApi->devIds);
auhalHostApi->devIds = NULL;
PaMacCore_AudioHardwareGetPropertySize( kAudioHardwarePropertyDevices,
&propsize);
auhalHostApi->devCount = propsize / sizeof( AudioDeviceID );
VDBUG( ( "Found %ld device(s).\n", auhalHostApi->devCount ) );
auhalHostApi->devIds = (AudioDeviceID *)PaUtil_GroupAllocateMemory(
auhalHostApi->allocations,
propsize );
if( !auhalHostApi->devIds )
return paInsufficientMemory;
PaMacCore_AudioHardwareGetProperty( kAudioHardwarePropertyDevices,
&propsize,
auhalHostApi->devIds );
#ifdef MAC_CORE_VERBOSE_DEBUG
{
int i;
for( i=0; i<auhalHostApi->devCount; ++i )
printf( "Device %d\t: %ld\n", i, (long)auhalHostApi->devIds[i] );
}
#endif
size = sizeof(AudioDeviceID);
auhalHostApi->defaultIn  = kAudioDeviceUnknown;
auhalHostApi->defaultOut = kAudioDeviceUnknown;
if( 0 != PaMacCore_AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice,
&size,
&auhalHostApi->defaultIn) ) {
int i;
auhalHostApi->defaultIn  = kAudioDeviceUnknown;
VDBUG(("Failed to get default input device from OS."));
VDBUG((" I will substitute the first available input Device."));
for( i=0; i<auhalHostApi->devCount; ++i ) {
PaDeviceInfo devInfo;
if( 0 != GetChannelInfo( auhalHostApi, &devInfo,
auhalHostApi->devIds[i], TRUE ) )
if( devInfo.maxInputChannels ) {
auhalHostApi->defaultIn = auhalHostApi->devIds[i];
break;
}
}
}
if( 0 != PaMacCore_AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,
&size,
&auhalHostApi->defaultOut) ) {
int i;
auhalHostApi->defaultIn  = kAudioDeviceUnknown;
VDBUG(("Failed to get default output device from OS."));
VDBUG((" I will substitute the first available output Device."));
for( i=0; i<auhalHostApi->devCount; ++i ) {
PaDeviceInfo devInfo;
if( 0 != GetChannelInfo( auhalHostApi, &devInfo,
auhalHostApi->devIds[i], FALSE ) )
if( devInfo.maxOutputChannels ) {
auhalHostApi->defaultOut = auhalHostApi->devIds[i];
break;
}
}
}
VDBUG( ( "Default in : %ld\n", (long)auhalHostApi->defaultIn  ) );
VDBUG( ( "Default out: %ld\n", (long)auhalHostApi->defaultOut ) );
return paNoError;
}
static PaError ClipToDeviceBufferSize( AudioDeviceID macCoreDeviceId,
int isInput, UInt32 desiredSize, UInt32 *allowedSize )
{
UInt32 resultSize = desiredSize;
AudioValueRange audioRange;
UInt32 propSize = sizeof( audioRange );
PaError err = WARNING(PaMacCore_AudioDeviceGetProperty( macCoreDeviceId, 0, isInput, kAudioDevicePropertyBufferFrameSizeRange, &propSize, &audioRange ) );
resultSize = MAX( resultSize, audioRange.mMinimum );
resultSize = MIN( resultSize, audioRange.mMaximum );
*allowedSize = resultSize;
return err;
}
#if 0
static void DumpDeviceProperties( AudioDeviceID macCoreDeviceId,
int isInput )
{
PaError err;
int i;
UInt32 propSize;
UInt32 deviceLatency;
UInt32 streamLatency;
UInt32 bufferFrames;
UInt32 safetyOffset;
AudioStreamID streamIDs[128];
AudioValueRange audioRange;
printf("\n======= latency query : macCoreDeviceId = %d, isInput %d =======\n", (int)macCoreDeviceId, isInput );
propSize = sizeof(UInt32);
bufferFrames = 0;
err = WARNING(AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertyBufferFrameSize, &propSize, &bufferFrames));
printf("kAudioDevicePropertyBufferFrameSize: err = %d, propSize = %d, value = %d\n", err, propSize, bufferFrames );
propSize = sizeof(UInt32);
safetyOffset = 0;
err = WARNING(AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertySafetyOffset, &propSize, &safetyOffset));
printf("kAudioDevicePropertySafetyOffset: err = %d, propSize = %d, value = %d\n", err, propSize, safetyOffset );
propSize = sizeof(UInt32);
deviceLatency = 0;
err = WARNING(AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertyLatency, &propSize, &deviceLatency));
printf("kAudioDevicePropertyLatency: err = %d, propSize = %d, value = %d\n", err, propSize, deviceLatency );
propSize = sizeof( audioRange );
err = WARNING(AudioDeviceGetProperty( macCoreDeviceId, 0, isInput, kAudioDevicePropertyBufferFrameSizeRange, &propSize, &audioRange ) );
printf("kAudioDevicePropertyBufferFrameSizeRange: err = %d, propSize = %u, minimum = %g\n", err, propSize, audioRange.mMinimum);
printf("kAudioDevicePropertyBufferFrameSizeRange: err = %d, propSize = %u, maximum = %g\n", err, propSize, audioRange.mMaximum );
propSize = sizeof(streamIDs);
err  = WARNING(AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertyStreams, &propSize, &streamIDs[0]));
int numStreams = propSize / sizeof(AudioStreamID);
for( i=0; i<numStreams; i++ )
{
printf("Stream #%d = %d---------------------- \n", i, streamIDs[i] );
propSize = sizeof(UInt32);
streamLatency = 0;
err  = WARNING(PaMacCore_AudioStreamGetProperty(streamIDs[i], 0, kAudioStreamPropertyLatency, &propSize, &streamLatency));
printf("  kAudioStreamPropertyLatency: err = %d, propSize = %d, value = %d\n", err, propSize, streamLatency );
}
}
#endif
static PaError CalculateFixedDeviceLatency( AudioDeviceID macCoreDeviceId, int isInput, UInt32 *fixedLatencyPtr )
{
PaError err;
UInt32 propSize;
UInt32 deviceLatency;
UInt32 streamLatency;
UInt32 safetyOffset;
AudioStreamID streamIDs[1];
propSize = sizeof(streamIDs);
err  = WARNING(PaMacCore_AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertyStreams, &propSize, &streamIDs[0]));
if( err != paNoError ) goto error;
streamLatency = 0;
if( propSize == sizeof(AudioStreamID) )
{
propSize = sizeof(UInt32);
err  = WARNING(PaMacCore_AudioStreamGetProperty(streamIDs[0], 0, kAudioStreamPropertyLatency, &propSize, &streamLatency));
}
propSize = sizeof(UInt32);
safetyOffset = 0;
err = WARNING(PaMacCore_AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertySafetyOffset, &propSize, &safetyOffset));
if( err != paNoError ) goto error;
propSize = sizeof(UInt32);
deviceLatency = 0;
err = WARNING(PaMacCore_AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertyLatency, &propSize, &deviceLatency));
if( err != paNoError ) goto error;
*fixedLatencyPtr = deviceLatency + streamLatency + safetyOffset;
return err;
error:
return err;
}
static PaError CalculateDefaultDeviceLatencies( AudioDeviceID macCoreDeviceId,
int isInput, UInt32 *lowLatencyFramesPtr,
UInt32 *highLatencyFramesPtr )
{
UInt32 propSize;
UInt32 bufferFrames = 0;
UInt32 fixedLatency = 0;
UInt32 clippedMinBufferSize = 0;
PaError err = CalculateFixedDeviceLatency( macCoreDeviceId, isInput, &fixedLatency );
if( err != paNoError ) goto error;
err = ClipToDeviceBufferSize( macCoreDeviceId, isInput, PA_MAC_SMALL_BUFFER_SIZE, &clippedMinBufferSize );
if( err != paNoError ) goto error;
propSize = sizeof(UInt32);
err = WARNING(PaMacCore_AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertyBufferFrameSize, &propSize, &bufferFrames));
if( err != paNoError ) goto error;
*lowLatencyFramesPtr = fixedLatency + clippedMinBufferSize;
*highLatencyFramesPtr = fixedLatency + bufferFrames;
return err;
error:
return err;
}
static PaError GetChannelInfo( PaMacAUHAL *auhalHostApi,
PaDeviceInfo *deviceInfo,
AudioDeviceID macCoreDeviceId,
int isInput)
{
UInt32 propSize;
PaError err = paNoError;
UInt32 i;
int numChannels = 0;
AudioBufferList *buflist = NULL;
VVDBUG(("GetChannelInfo()\n"));
err = ERR(PaMacCore_AudioDeviceGetPropertySize(macCoreDeviceId, 0, isInput, kAudioDevicePropertyStreamConfiguration, &propSize));
if (err)
return err;
buflist = PaUtil_AllocateMemory(propSize);
if( !buflist )
return paInsufficientMemory;
err = ERR(PaMacCore_AudioDeviceGetProperty(macCoreDeviceId, 0, isInput, kAudioDevicePropertyStreamConfiguration, &propSize, buflist));
if (err)
goto error;
for (i = 0; i < buflist->mNumberBuffers; ++i)
numChannels += buflist->mBuffers[i].mNumberChannels;
if (isInput)
deviceInfo->maxInputChannels = numChannels;
else
deviceInfo->maxOutputChannels = numChannels;
if (numChannels > 0)  
{
deviceInfo->defaultLowInputLatency = .01;
deviceInfo->defaultHighInputLatency = .10;
deviceInfo->defaultLowOutputLatency = .01;
deviceInfo->defaultHighOutputLatency = .10;
UInt32 lowLatencyFrames = 0;
UInt32 highLatencyFrames = 0;
err = CalculateDefaultDeviceLatencies( macCoreDeviceId, isInput, &lowLatencyFrames, &highLatencyFrames );
if( err == 0 )
{
double lowLatencySeconds = lowLatencyFrames / deviceInfo->defaultSampleRate;
double highLatencySeconds = highLatencyFrames / deviceInfo->defaultSampleRate;
if (isInput)
{
deviceInfo->defaultLowInputLatency = lowLatencySeconds;
deviceInfo->defaultHighInputLatency = highLatencySeconds;
}
else
{
deviceInfo->defaultLowOutputLatency = lowLatencySeconds;
deviceInfo->defaultHighOutputLatency = highLatencySeconds;
}
}
}
PaUtil_FreeMemory( buflist );
return paNoError;
error:
PaUtil_FreeMemory( buflist );
return err;
}
static PaError InitializeDeviceInfo( PaMacAUHAL *auhalHostApi,
PaDeviceInfo *deviceInfo,
AudioDeviceID macCoreDeviceId,
PaHostApiIndex hostApiIndex )
{
Float64 sampleRate;
char *name;
PaError err = paNoError;
CFStringRef nameRef;
UInt32 propSize;
VVDBUG(("InitializeDeviceInfo(): macCoreDeviceId=%ld\n", macCoreDeviceId));
memset(deviceInfo, 0, sizeof(PaDeviceInfo));
deviceInfo->structVersion = 2;
deviceInfo->hostApi = hostApiIndex;
propSize = sizeof(nameRef);
err = ERR(PaMacCore_AudioDeviceGetProperty(macCoreDeviceId, 0, 0, kAudioDevicePropertyDeviceNameCFString, &propSize, &nameRef));
if (err)
{
err = ERR(PaMacCore_AudioDeviceGetPropertySize(macCoreDeviceId, 0, 0, kAudioDevicePropertyDeviceName, &propSize));
if (err)
return err;
name = PaUtil_GroupAllocateMemory(auhalHostApi->allocations,propSize+1);
if ( !name )
return paInsufficientMemory;
err = ERR(PaMacCore_AudioDeviceGetProperty(macCoreDeviceId, 0, 0, kAudioDevicePropertyDeviceName, &propSize, name));
if (err)
return err;
}
else
{
propSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(nameRef), kCFStringEncodingUTF8);
name = PaUtil_GroupAllocateMemory(auhalHostApi->allocations, propSize+1);
if ( !name )
{
CFRelease(nameRef);
return paInsufficientMemory;
}
CFStringGetCString(nameRef, name, propSize+1, kCFStringEncodingUTF8);
CFRelease(nameRef);
}
deviceInfo->name = name;
propSize = sizeof(Float64);
err = ERR(PaMacCore_AudioDeviceGetProperty(macCoreDeviceId, 0, 0, kAudioDevicePropertyNominalSampleRate, &propSize, &sampleRate));
if (err)
deviceInfo->defaultSampleRate = 0.0;
else
deviceInfo->defaultSampleRate = sampleRate;
err = GetChannelInfo(auhalHostApi, deviceInfo, macCoreDeviceId, 1);
if (err)
return err;
err = GetChannelInfo(auhalHostApi, deviceInfo, macCoreDeviceId, 0);
if (err)
return err;
return paNoError;
}
PaError PaMacCore_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result = paNoError;
int i;
PaMacAUHAL *auhalHostApi = NULL;
PaDeviceInfo *deviceInfoArray;
int unixErr;
VVDBUG(("PaMacCore_Initialize(): hostApiIndex=%d\n", hostApiIndex));
CFRunLoopRef theRunLoop = NULL;
AudioObjectPropertyAddress theAddress = { kAudioHardwarePropertyRunLoop, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
OSStatus osErr = AudioObjectSetPropertyData (kAudioObjectSystemObject, &theAddress, 0, NULL, sizeof(CFRunLoopRef), &theRunLoop);
if (osErr != noErr) {
goto error;
}
unixErr = initializeXRunListenerList();
if( 0 != unixErr ) {
return UNIX_ERR(unixErr);
}
auhalHostApi = (PaMacAUHAL*)PaUtil_AllocateMemory( sizeof(PaMacAUHAL) );
if( !auhalHostApi )
{
result = paInsufficientMemory;
goto error;
}
auhalHostApi->allocations = PaUtil_CreateAllocationGroup();
if( !auhalHostApi->allocations )
{
result = paInsufficientMemory;
goto error;
}
auhalHostApi->devIds = NULL;
auhalHostApi->devCount = 0;
result = gatherDeviceInfo( auhalHostApi );
if( result != paNoError )
goto error;
*hostApi = &auhalHostApi->inheritedHostApiRep;
(*hostApi)->info.structVersion = 1;
(*hostApi)->info.type = paCoreAudio;
(*hostApi)->info.name = "Core Audio";
(*hostApi)->info.defaultInputDevice = paNoDevice;
(*hostApi)->info.defaultOutputDevice = paNoDevice;
(*hostApi)->info.deviceCount = 0;
if( auhalHostApi->devCount > 0 )
{
(*hostApi)->deviceInfos = (PaDeviceInfo**)PaUtil_GroupAllocateMemory(
auhalHostApi->allocations, sizeof(PaDeviceInfo*) * auhalHostApi->devCount);
if( !(*hostApi)->deviceInfos )
{
result = paInsufficientMemory;
goto error;
}
deviceInfoArray = (PaDeviceInfo*)PaUtil_GroupAllocateMemory(
auhalHostApi->allocations, sizeof(PaDeviceInfo) * auhalHostApi->devCount );
if( !deviceInfoArray )
{
result = paInsufficientMemory;
goto error;
}
for( i=0; i < auhalHostApi->devCount; ++i )
{
int err;
err = InitializeDeviceInfo( auhalHostApi, &deviceInfoArray[i],
auhalHostApi->devIds[i],
hostApiIndex );
if (err == paNoError)
{    
(*hostApi)->deviceInfos[(*hostApi)->info.deviceCount] = &deviceInfoArray[i];
if (auhalHostApi->devIds[i] == auhalHostApi->defaultIn)
(*hostApi)->info.defaultInputDevice = (*hostApi)->info.deviceCount;
if (auhalHostApi->devIds[i] == auhalHostApi->defaultOut)
(*hostApi)->info.defaultOutputDevice = (*hostApi)->info.deviceCount;
(*hostApi)->info.deviceCount++;
}
else
{    
int j;
auhalHostApi->devCount--;
for( j=i; j<auhalHostApi->devCount; ++j )
auhalHostApi->devIds[j] = auhalHostApi->devIds[j+1];
i--;
}
}
}
(*hostApi)->Terminate = Terminate;
(*hostApi)->OpenStream = OpenStream;
(*hostApi)->IsFormatSupported = IsFormatSupported;
PaUtil_InitializeStreamInterface( &auhalHostApi->callbackStreamInterface,
CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped,
IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable,
PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &auhalHostApi->blockingStreamInterface,
CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped,
IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream,
GetStreamReadAvailable,
GetStreamWriteAvailable );
return result;
error:
if( auhalHostApi )
{
if( auhalHostApi->allocations )
{
PaUtil_FreeAllAllocations( auhalHostApi->allocations );
PaUtil_DestroyAllocationGroup( auhalHostApi->allocations );
}
PaUtil_FreeMemory( auhalHostApi );
}
return result;
}
static void Terminate( struct PaUtilHostApiRepresentation *hostApi )
{
int unixErr;
PaMacAUHAL *auhalHostApi = (PaMacAUHAL*)hostApi;
VVDBUG(("Terminate()\n"));
unixErr = destroyXRunListenerList();
if( 0 != unixErr )
UNIX_ERR(unixErr);
if( auhalHostApi->allocations )
{
PaUtil_FreeAllAllocations( auhalHostApi->allocations );
PaUtil_DestroyAllocationGroup( auhalHostApi->allocations );
}
PaUtil_FreeMemory( auhalHostApi );
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate )
{
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
VVDBUG(("IsFormatSupported(): in chan=%d, in fmt=%ld, out chan=%d, out fmt=%ld sampleRate=%g\n",
inputParameters  ? inputParameters->channelCount  : -1,
inputParameters  ? inputParameters->sampleFormat  : -1,
outputParameters ? outputParameters->channelCount : -1,
outputParameters ? outputParameters->sampleFormat : -1,
(float) sampleRate ));
if( inputParameters )
{
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
if( inputSampleFormat & paCustomFormat )
return paSampleFormatNotSupported;
if( inputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( inputChannelCount > hostApi->deviceInfos[ inputParameters->device ]->maxInputChannels )
return paInvalidChannelCount;
}
else
{
inputChannelCount = 0;
}
if( outputParameters )
{
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
if( outputSampleFormat & paCustomFormat )
return paSampleFormatNotSupported;
if( outputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( outputChannelCount > hostApi->deviceInfos[ outputParameters->device ]->maxOutputChannels )
return paInvalidChannelCount;
}
else
{
outputChannelCount = 0;
}
{
PaError err;
PaStream *s;
err = OpenStream( hostApi, &s, inputParameters, outputParameters,
sampleRate, 1024, 0, (PaStreamCallback *)1, NULL );
if( err != paNoError && err != paInvalidSampleRate )
DBUG( ( "OpenStream @ %g returned: %d: %s\n",
(float) sampleRate, err, Pa_GetErrorText( err ) ) );
if( err )
return err;
err = CloseStream( s );
if( err ) {
DBUG( ( "WARNING: could not close Stream. %d: %s\n",
err, Pa_GetErrorText( err ) ) );
}
}
return paFormatIsSupported;
}
static void InitializeDeviceProperties( PaMacCoreDeviceProperties *deviceProperties )
{
memset( deviceProperties, 0, sizeof(PaMacCoreDeviceProperties) );
deviceProperties->sampleRate = 1.0;  
deviceProperties->samplePeriod = 1.0 / deviceProperties->sampleRate;
}
static Float64 CalculateSoftwareLatencyFromProperties( PaMacCoreStream *stream, PaMacCoreDeviceProperties *deviceProperties )
{
UInt32 latencyFrames = deviceProperties->bufferFrameSize + deviceProperties->deviceLatency + deviceProperties->safetyOffset;
return latencyFrames * deviceProperties->samplePeriod;  
}
static Float64 CalculateHardwareLatencyFromProperties( PaMacCoreStream *stream, PaMacCoreDeviceProperties *deviceProperties )
{
return deviceProperties->deviceLatency * deviceProperties->samplePeriod;  
}
static void UpdateTimeStampOffsets( PaMacCoreStream *stream )
{
Float64 inputSoftwareLatency = 0.0;
Float64 inputHardwareLatency = 0.0;
Float64 outputSoftwareLatency = 0.0;
Float64 outputHardwareLatency = 0.0;
if( stream->inputUnit != NULL )
{
inputSoftwareLatency = CalculateSoftwareLatencyFromProperties( stream, &stream->inputProperties );
inputHardwareLatency = CalculateHardwareLatencyFromProperties( stream, &stream->inputProperties );
}
if( stream->outputUnit != NULL )
{
outputSoftwareLatency = CalculateSoftwareLatencyFromProperties( stream, &stream->outputProperties );
outputHardwareLatency = CalculateHardwareLatencyFromProperties( stream, &stream->outputProperties );
}
pthread_mutex_lock( &stream->timingInformationMutex );
stream->timestampOffsetCombined = inputSoftwareLatency + outputSoftwareLatency;
stream->timestampOffsetInputDevice = inputHardwareLatency;
stream->timestampOffsetOutputDevice = outputHardwareLatency;
pthread_mutex_unlock( &stream->timingInformationMutex );
}
static OSStatus UpdateSampleRateFromDeviceProperty( PaMacCoreStream *stream, AudioDeviceID deviceID,
Boolean isInput, AudioDevicePropertyID sampleRatePropertyID )
{
PaMacCoreDeviceProperties * deviceProperties = isInput ? &stream->inputProperties : &stream->outputProperties;
Float64 sampleRate = 0.0;
UInt32 propSize = sizeof(Float64);
OSStatus osErr = PaMacCore_AudioDeviceGetProperty( deviceID, 0, isInput, sampleRatePropertyID, &propSize, &sampleRate);
if( (osErr == noErr) && (sampleRate > 1000.0) )  
{
deviceProperties->sampleRate = sampleRate;
deviceProperties->samplePeriod = 1.0 / sampleRate;
}
return osErr;
}
static OSStatus AudioDevicePropertyActualSampleRateListenerProc( AudioObjectID inDevice, UInt32 inNumberAddresses,
const AudioObjectPropertyAddress * inAddresses, void * inClientData )
{
PaMacCoreStream *stream = (PaMacCoreStream*)inClientData;
bool isInput = inAddresses->mScope == kAudioDevicePropertyScopeInput;
assert( stream->streamRepresentation.magic == PA_STREAM_MAGIC );
OSStatus osErr = UpdateSampleRateFromDeviceProperty( stream, inDevice, isInput, kAudioDevicePropertyActualSampleRate );
if( osErr == noErr )
{
UpdateTimeStampOffsets( stream );
}
return osErr;
}
static OSStatus QueryUInt32DeviceProperty( AudioDeviceID deviceID, Boolean isInput, AudioDevicePropertyID propertyID, UInt32 *outValue )
{
UInt32 propertyValue = 0;
UInt32 propertySize = sizeof(UInt32);
OSStatus osErr = PaMacCore_AudioDeviceGetProperty( deviceID, 0, isInput, propertyID, &propertySize, &propertyValue);
if( osErr == noErr )
{
*outValue = propertyValue;
}
return osErr;
}
static OSStatus AudioDevicePropertyGenericListenerProc( AudioObjectID inDevice, UInt32 inNumberAddresses,
const AudioObjectPropertyAddress * inAddresses, void * inClientData )
{
OSStatus osErr = noErr;
PaMacCoreStream *stream = (PaMacCoreStream*)inClientData;
bool isInput = inAddresses->mScope == kAudioDevicePropertyScopeInput;
AudioDevicePropertyID inPropertyID = inAddresses->mSelector;
assert( stream->streamRepresentation.magic == PA_STREAM_MAGIC );
PaMacCoreDeviceProperties *deviceProperties = isInput ? &stream->inputProperties : &stream->outputProperties;
UInt32 *valuePtr = NULL;
switch( inPropertyID )
{
case kAudioDevicePropertySafetyOffset:
valuePtr = &deviceProperties->safetyOffset;
break;
case kAudioDevicePropertyLatency:
valuePtr = &deviceProperties->deviceLatency;
break;
case kAudioDevicePropertyBufferFrameSize:
valuePtr = &deviceProperties->bufferFrameSize;
break;
}
if( valuePtr != NULL )
{
osErr = QueryUInt32DeviceProperty( inDevice, isInput, inPropertyID, valuePtr );
if( osErr == noErr )
{
UpdateTimeStampOffsets( stream );
}
}
return osErr;
}
static OSStatus SetupDevicePropertyListeners( PaMacCoreStream *stream, AudioDeviceID deviceID, Boolean isInput )
{
OSStatus osErr = noErr;
PaMacCoreDeviceProperties *deviceProperties = isInput ? &stream->inputProperties : &stream->outputProperties;
if( (osErr = QueryUInt32DeviceProperty( deviceID, isInput,
kAudioDevicePropertyLatency, &deviceProperties->deviceLatency )) != noErr ) return osErr;
if( (osErr = QueryUInt32DeviceProperty( deviceID, isInput,
kAudioDevicePropertyBufferFrameSize, &deviceProperties->bufferFrameSize )) != noErr ) return osErr;
if( (osErr = QueryUInt32DeviceProperty( deviceID, isInput,
kAudioDevicePropertySafetyOffset, &deviceProperties->safetyOffset )) != noErr ) return osErr;
PaMacCore_AudioDeviceAddPropertyListener( deviceID, 0, isInput, kAudioDevicePropertyActualSampleRate,
AudioDevicePropertyActualSampleRateListenerProc, stream );
PaMacCore_AudioDeviceAddPropertyListener( deviceID, 0, isInput, kAudioStreamPropertyLatency,
AudioDevicePropertyGenericListenerProc, stream );
PaMacCore_AudioDeviceAddPropertyListener( deviceID, 0, isInput, kAudioDevicePropertyBufferFrameSize,
AudioDevicePropertyGenericListenerProc, stream );
PaMacCore_AudioDeviceAddPropertyListener( deviceID, 0, isInput, kAudioDevicePropertySafetyOffset,
AudioDevicePropertyGenericListenerProc, stream );
return osErr;
}
static void CleanupDevicePropertyListeners( PaMacCoreStream *stream, AudioDeviceID deviceID, Boolean isInput )
{
PaMacCore_AudioDeviceRemovePropertyListener( deviceID, 0, isInput, kAudioDevicePropertyActualSampleRate,
AudioDevicePropertyActualSampleRateListenerProc, stream );
PaMacCore_AudioDeviceRemovePropertyListener( deviceID, 0, isInput, kAudioDevicePropertyLatency,
AudioDevicePropertyGenericListenerProc, stream );
PaMacCore_AudioDeviceRemovePropertyListener( deviceID, 0, isInput, kAudioDevicePropertyBufferFrameSize,
AudioDevicePropertyGenericListenerProc, stream );
PaMacCore_AudioDeviceRemovePropertyListener( deviceID, 0, isInput, kAudioDevicePropertySafetyOffset,
AudioDevicePropertyGenericListenerProc, stream );
}
static PaError OpenAndSetupOneAudioUnit(
const PaMacCoreStream *stream,
const PaStreamParameters *inStreamParams,
const PaStreamParameters *outStreamParams,
const UInt32 requestedFramesPerBuffer,
UInt32 *actualInputFramesPerBuffer,
UInt32 *actualOutputFramesPerBuffer,
const PaMacAUHAL *auhalHostApi,
AudioUnit *audioUnit,
AudioConverterRef *srConverter,
AudioDeviceID *audioDevice,
const double sampleRate,
void *refCon )
{
AudioComponentDescription desc;
AudioComponent comp;
AudioStreamBasicDescription desiredFormat;
OSStatus result = noErr;
PaError paResult = paNoError;
int line = 0;
UInt32 callbackKey;
AURenderCallbackStruct rcbs;
unsigned long macInputStreamFlags  = paMacCorePlayNice;
unsigned long macOutputStreamFlags = paMacCorePlayNice;
SInt32 const *inChannelMap = NULL;
SInt32 const *outChannelMap = NULL;
unsigned long inChannelMapSize = 0;
unsigned long outChannelMapSize = 0;
VVDBUG(("OpenAndSetupOneAudioUnit(): in chan=%d, in fmt=%ld, out chan=%d, out fmt=%ld, requestedFramesPerBuffer=%ld\n",
inStreamParams  ? inStreamParams->channelCount  : -1,
inStreamParams  ? inStreamParams->sampleFormat  : -1,
outStreamParams ? outStreamParams->channelCount : -1,
outStreamParams ? outStreamParams->sampleFormat : -1,
requestedFramesPerBuffer ));
if( !inStreamParams && !outStreamParams ) {
*audioUnit = NULL;
*audioDevice = kAudioDeviceUnknown;
return paNoError;
}
if( inStreamParams && inStreamParams->hostApiSpecificStreamInfo )
{
macInputStreamFlags=
((PaMacCoreStreamInfo*)inStreamParams->hostApiSpecificStreamInfo)
->flags;
inChannelMap = ((PaMacCoreStreamInfo*)inStreamParams->hostApiSpecificStreamInfo)->channelMap;
inChannelMapSize = ((PaMacCoreStreamInfo*)inStreamParams->hostApiSpecificStreamInfo)->channelMapSize;
}
if( outStreamParams && outStreamParams->hostApiSpecificStreamInfo )
{
macOutputStreamFlags=
((PaMacCoreStreamInfo*)outStreamParams->hostApiSpecificStreamInfo)
->flags;
outChannelMap = ((PaMacCoreStreamInfo*)outStreamParams->hostApiSpecificStreamInfo)->channelMap;
outChannelMapSize = ((PaMacCoreStreamInfo*)outStreamParams->hostApiSpecificStreamInfo)->channelMapSize;
}
desc.componentType         = kAudioUnitType_Output;
desc.componentSubType      = kAudioUnitSubType_HALOutput;
desc.componentManufacturer = kAudioUnitManufacturer_Apple;
desc.componentFlags        = 0;
desc.componentFlagsMask    = 0;
comp = AudioComponentFindNext( NULL, &desc );
if( !comp )
{
DBUG( ( "AUHAL component not found." ) );
*audioUnit = NULL;
*audioDevice = kAudioDeviceUnknown;
return paUnanticipatedHostError;
}
result = AudioComponentInstanceNew( comp, audioUnit );
if( result )
{
DBUG( ( "Failed to open AUHAL component." ) );
*audioUnit = NULL;
*audioDevice = kAudioDeviceUnknown;
return ERR( result );
}
#define ERR_WRAP(mac_err) do { result = mac_err ; line = __LINE__ ; if ( result != noErr ) goto error ; } while(0)
if( inStreamParams )
{
UInt32 enableIO = 1;
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioOutputUnitProperty_EnableIO,
kAudioUnitScope_Input,
INPUT_ELEMENT,
&enableIO,
sizeof(enableIO) ) );
}
if( !outStreamParams )
{
UInt32 enableIO = 0;
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioOutputUnitProperty_EnableIO,
kAudioUnitScope_Output,
OUTPUT_ELEMENT,
&enableIO,
sizeof(enableIO) ) );
}
if( inStreamParams && outStreamParams )
{
assert( outStreamParams->device == inStreamParams->device );
}
if( inStreamParams )
{
*audioDevice = auhalHostApi->devIds[inStreamParams->device] ;
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioOutputUnitProperty_CurrentDevice,
kAudioUnitScope_Global,
INPUT_ELEMENT,
audioDevice,
sizeof(AudioDeviceID) ) );
}
if( outStreamParams && outStreamParams != inStreamParams )
{
*audioDevice = auhalHostApi->devIds[outStreamParams->device] ;
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioOutputUnitProperty_CurrentDevice,
kAudioUnitScope_Global,
OUTPUT_ELEMENT,
audioDevice,
sizeof(AudioDeviceID) ) );
}
result = PaMacCore_AudioDeviceAddPropertyListener( *audioDevice,
0,
outStreamParams ? false : true,
kAudioDeviceProcessorOverload,
xrunCallback,
addToXRunListenerList( (void *)stream ) ) ;
if( result == kAudioHardwareIllegalOperationError ) {
} else {
ERR_WRAP( result );
}
ERR_WRAP( AudioUnitAddPropertyListener( *audioUnit,
kAudioOutputUnitProperty_IsRunning,
startStopCallback,
(void *)stream ) );
bzero( &desiredFormat, sizeof(desiredFormat) );
desiredFormat.mFormatID         = kAudioFormatLinearPCM ;
desiredFormat.mFormatFlags      = kAudioFormatFlagsNativeFloatPacked;
desiredFormat.mFramesPerPacket  = 1;
desiredFormat.mBitsPerChannel   = sizeof( float ) * 8;
result = 0;
if( inStreamParams ) {
paResult = setBestFramesPerBuffer( *audioDevice, FALSE,
requestedFramesPerBuffer,
actualInputFramesPerBuffer );
if( paResult ) goto error;
if( macInputStreamFlags & paMacCoreChangeDeviceParameters ) {
bool requireExact;
requireExact=macInputStreamFlags & paMacCoreFailIfConversionRequired;
paResult = setBestSampleRateForDevice( *audioDevice, FALSE,
requireExact, sampleRate );
if( paResult ) goto error;
}
if( actualInputFramesPerBuffer && actualOutputFramesPerBuffer )
*actualOutputFramesPerBuffer = *actualInputFramesPerBuffer ;
}
if( outStreamParams && !inStreamParams ) {
paResult = setBestFramesPerBuffer( *audioDevice, TRUE,
requestedFramesPerBuffer,
actualOutputFramesPerBuffer );
if( paResult ) goto error;
if( macOutputStreamFlags & paMacCoreChangeDeviceParameters ) {
bool requireExact;
requireExact=macOutputStreamFlags & paMacCoreFailIfConversionRequired;
paResult = setBestSampleRateForDevice( *audioDevice, TRUE,
requireExact, sampleRate );
if( paResult ) goto error;
}
}
if( outStreamParams ) {
UInt32 value = kAudioConverterQuality_Max;
switch( macOutputStreamFlags & 0x0700 ) {
case 0x0100:  
value=kRenderQuality_Min;
break;
case 0x0200:  
value=kRenderQuality_Low;
break;
case 0x0300:  
value=kRenderQuality_Medium;
break;
case 0x0400:  
value=kRenderQuality_High;
break;
}
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioUnitProperty_RenderQuality,
kAudioUnitScope_Global,
OUTPUT_ELEMENT,
&value,
sizeof(value) ) );
}
if( outStreamParams )
{
desiredFormat.mSampleRate    =sampleRate;
desiredFormat.mBytesPerPacket=sizeof(float)*outStreamParams->channelCount;
desiredFormat.mBytesPerFrame =sizeof(float)*outStreamParams->channelCount;
desiredFormat.mChannelsPerFrame = outStreamParams->channelCount;
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioUnitProperty_StreamFormat,
kAudioUnitScope_Input,
OUTPUT_ELEMENT,
&desiredFormat,
sizeof(AudioStreamBasicDescription) ) );
}
if( inStreamParams )
{
AudioStreamBasicDescription sourceFormat;
UInt32 size = sizeof( AudioStreamBasicDescription );
ERR_WRAP( AudioUnitGetProperty( *audioUnit,
kAudioUnitProperty_StreamFormat,
kAudioUnitScope_Input,
INPUT_ELEMENT,
&sourceFormat,
&size ) );
desiredFormat.mSampleRate = sourceFormat.mSampleRate;
desiredFormat.mBytesPerPacket=sizeof(float)*inStreamParams->channelCount;
desiredFormat.mBytesPerFrame =sizeof(float)*inStreamParams->channelCount;
desiredFormat.mChannelsPerFrame = inStreamParams->channelCount;
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioUnitProperty_StreamFormat,
kAudioUnitScope_Output,
INPUT_ELEMENT,
&desiredFormat,
sizeof(AudioStreamBasicDescription) ) );
}
if( outStreamParams ) {
UInt32 size = sizeof( *actualOutputFramesPerBuffer );
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioUnitProperty_MaximumFramesPerSlice,
kAudioUnitScope_Input,
OUTPUT_ELEMENT,
actualOutputFramesPerBuffer,
sizeof(*actualOutputFramesPerBuffer) ) );
ERR_WRAP( AudioUnitGetProperty( *audioUnit,
kAudioUnitProperty_MaximumFramesPerSlice,
kAudioUnitScope_Global,
OUTPUT_ELEMENT,
actualOutputFramesPerBuffer,
&size ) );
}
if( inStreamParams ) {
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioUnitProperty_MaximumFramesPerSlice,
kAudioUnitScope_Output,
INPUT_ELEMENT,
actualInputFramesPerBuffer,
sizeof(*actualInputFramesPerBuffer) ) );
}
if( inStreamParams ) {
AudioStreamBasicDescription desiredFormat;
AudioStreamBasicDescription sourceFormat;
UInt32 sourceSize = sizeof( sourceFormat );
bzero( &desiredFormat, sizeof(desiredFormat) );
desiredFormat.mSampleRate       = sampleRate;
desiredFormat.mFormatID         = kAudioFormatLinearPCM ;
desiredFormat.mFormatFlags      = kAudioFormatFlagsNativeFloatPacked;
desiredFormat.mFramesPerPacket  = 1;
desiredFormat.mBitsPerChannel   = sizeof( float ) * 8;
desiredFormat.mBytesPerPacket=sizeof(float)*inStreamParams->channelCount;
desiredFormat.mBytesPerFrame =sizeof(float)*inStreamParams->channelCount;
desiredFormat.mChannelsPerFrame = inStreamParams->channelCount;
ERR_WRAP( AudioUnitGetProperty( *audioUnit,
kAudioUnitProperty_StreamFormat,
kAudioUnitScope_Output,
INPUT_ELEMENT,
&sourceFormat,
&sourceSize ) );
if( desiredFormat.mSampleRate != sourceFormat.mSampleRate )
{
UInt32 value = kAudioConverterQuality_Max;
switch( macInputStreamFlags & 0x0700 ) {
case 0x0100:  
value=kAudioConverterQuality_Min;
break;
case 0x0200:  
value=kAudioConverterQuality_Low;
break;
case 0x0300:  
value=kAudioConverterQuality_Medium;
break;
case 0x0400:  
value=kAudioConverterQuality_High;
break;
}
VDBUG(( "Creating sample rate converter for input"
" to convert from %g to %g\n",
(float)sourceFormat.mSampleRate,
(float)desiredFormat.mSampleRate ) );
ERR_WRAP( AudioConverterNew( &sourceFormat,
&desiredFormat,
srConverter ) );
ERR_WRAP( AudioConverterSetProperty( *srConverter,
kAudioConverterSampleRateConverterQuality,
sizeof( value ),
&value ) );
}
}
callbackKey = outStreamParams ? kAudioUnitProperty_SetRenderCallback
: kAudioOutputUnitProperty_SetInputCallback ;
rcbs.inputProc = AudioIOProc;
rcbs.inputProcRefCon = refCon;
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
callbackKey,
kAudioUnitScope_Output,
outStreamParams ? OUTPUT_ELEMENT : INPUT_ELEMENT,
&rcbs,
sizeof(rcbs)) );
if( inStreamParams && outStreamParams && *srConverter )
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioOutputUnitProperty_SetInputCallback,
kAudioUnitScope_Output,
INPUT_ELEMENT,
&rcbs,
sizeof(rcbs)) );
if(inChannelMap)
{
UInt32 mapSize = inChannelMapSize *sizeof(SInt32);
ERR_WRAP( AudioUnitSetProperty( *audioUnit,
kAudioOutputUnitProperty_ChannelMap,
kAudioUnitScope_Output,
INPUT_ELEMENT,
inChannelMap,
mapSize) );
}
if(outChannelMap)
{
UInt32 mapSize = outChannelMapSize *sizeof(SInt32);
ERR_WRAP(AudioUnitSetProperty( *audioUnit,
kAudioOutputUnitProperty_ChannelMap,
kAudioUnitScope_Output,
OUTPUT_ELEMENT,
outChannelMap,
mapSize) );
}
ERR_WRAP( AudioUnitInitialize(*audioUnit) );
if( inStreamParams && outStreamParams )
{
VDBUG( ("Opened device %ld for input and output.\n", (long)*audioDevice ) );
}
else if( inStreamParams )
{
VDBUG( ("Opened device %ld for input.\n", (long)*audioDevice ) );
}
else if( outStreamParams )
{
VDBUG( ("Opened device %ld for output.\n", (long)*audioDevice ) );
}
return paNoError;
#undef ERR_WRAP
error:
AudioComponentInstanceDispose( *audioUnit );
*audioUnit = NULL;
if( result )
return PaMacCore_SetError( result, line, 1 );
return paResult;
}
static UInt32 CalculateOptimalBufferSize( PaMacAUHAL *auhalHostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
UInt32 fixedInputLatency,
UInt32 fixedOutputLatency,
double sampleRate,
UInt32 requestedFramesPerBuffer )
{
UInt32 resultBufferSizeFrames = 0;
if( inputParameters )
{
UInt32 suggestedLatencyFrames = inputParameters->suggestedLatency * sampleRate;
SInt32 variableLatencyFrames = suggestedLatencyFrames - fixedInputLatency;
variableLatencyFrames = MAX( variableLatencyFrames, 0 );
resultBufferSizeFrames = MAX( resultBufferSizeFrames, (UInt32) variableLatencyFrames );
}
if( outputParameters )
{
UInt32 suggestedLatencyFrames = outputParameters->suggestedLatency * sampleRate;
SInt32 variableLatencyFrames = suggestedLatencyFrames - fixedOutputLatency;
variableLatencyFrames = MAX( variableLatencyFrames, 0 );
resultBufferSizeFrames = MAX( resultBufferSizeFrames, (UInt32) variableLatencyFrames );
}
resultBufferSizeFrames = MAX( resultBufferSizeFrames, 1 );
if( requestedFramesPerBuffer != paFramesPerBufferUnspecified )
{
UInt32 n = (resultBufferSizeFrames + requestedFramesPerBuffer - 1) / requestedFramesPerBuffer;
resultBufferSizeFrames = n * requestedFramesPerBuffer;
} else {
VDBUG( ("Block Size unspecified. Based on Latency, the user wants a Block Size near: %ld.\n",
(long)resultBufferSizeFrames ) );
}
if( inputParameters )
{
ClipToDeviceBufferSize( auhalHostApi->devIds[inputParameters->device],
true,  
resultBufferSizeFrames, &resultBufferSizeFrames );
}
if( outputParameters )
{
ClipToDeviceBufferSize( auhalHostApi->devIds[outputParameters->device],
false, resultBufferSizeFrames, &resultBufferSizeFrames );
}
VDBUG(("After querying hardware, setting block size to %ld.\n", (long)resultBufferSizeFrames));
return resultBufferSizeFrames;
}
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream** s,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long requestedFramesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback *streamCallback,
void *userData )
{
PaError result = paNoError;
PaMacAUHAL *auhalHostApi = (PaMacAUHAL*)hostApi;
PaMacCoreStream *stream = 0;
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaSampleFormat hostInputSampleFormat, hostOutputSampleFormat;
UInt32 fixedInputLatency = 0;
UInt32 fixedOutputLatency = 0;
UInt32 inputLatencyFrames = 0;
UInt32 outputLatencyFrames = 0;
UInt32 suggestedLatencyFramesPerBuffer = requestedFramesPerBuffer;
VVDBUG(("OpenStream(): in chan=%d, in fmt=%ld, out chan=%d, out fmt=%ld SR=%g, FPB=%ld\n",
inputParameters  ? inputParameters->channelCount  : -1,
inputParameters  ? inputParameters->sampleFormat  : -1,
outputParameters ? outputParameters->channelCount : -1,
outputParameters ? outputParameters->sampleFormat : -1,
(float) sampleRate,
requestedFramesPerBuffer ));
VDBUG( ("Opening Stream.\n") );
if( inputParameters )
{
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
if( !streamCallback && inputSampleFormat & paNonInterleaved )
{
return paSampleFormatNotSupported;
}
if( inputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( inputChannelCount > hostApi->deviceInfos[ inputParameters->device ]->maxInputChannels )
return paInvalidChannelCount;
hostInputSampleFormat = paFloat32;
}
else
{
inputChannelCount = 0;
inputSampleFormat = hostInputSampleFormat = paFloat32;  
}
if( outputParameters )
{
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
if( !streamCallback && outputSampleFormat & paNonInterleaved )
{
return paSampleFormatNotSupported;
}
if( outputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( outputChannelCount > hostApi->deviceInfos[ outputParameters->device ]->maxOutputChannels )
return paInvalidChannelCount;
hostOutputSampleFormat = paFloat32;
}
else
{
outputChannelCount = 0;
outputSampleFormat = hostOutputSampleFormat = paFloat32;  
}
if( (streamFlags & paPlatformSpecificFlags) != 0 )
return paInvalidFlag;  
stream = (PaMacCoreStream*)PaUtil_AllocateMemory( sizeof(PaMacCoreStream) );
if( !stream )
{
result = paInsufficientMemory;
goto error;
}
bzero( stream, sizeof( PaMacCoreStream ) );
if( streamCallback )
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&auhalHostApi->callbackStreamInterface,
streamCallback, userData );
}
else
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&auhalHostApi->blockingStreamInterface,
BlioCallback, &stream->blio );
}
PaUtil_InitializeCpuLoadMeasurer( &stream->cpuLoadMeasurer, sampleRate );
if( inputParameters )
{
CalculateFixedDeviceLatency( auhalHostApi->devIds[inputParameters->device], true, &fixedInputLatency );
inputLatencyFrames += fixedInputLatency;
}
if( outputParameters )
{
CalculateFixedDeviceLatency( auhalHostApi->devIds[outputParameters->device], false, &fixedOutputLatency );
outputLatencyFrames += fixedOutputLatency;
}
suggestedLatencyFramesPerBuffer = CalculateOptimalBufferSize( auhalHostApi, inputParameters, outputParameters,
fixedInputLatency, fixedOutputLatency,
sampleRate, requestedFramesPerBuffer );
if( requestedFramesPerBuffer == paFramesPerBufferUnspecified )
{
requestedFramesPerBuffer = suggestedLatencyFramesPerBuffer;
}
if( inputParameters && outputParameters && outputParameters->device == inputParameters->device )
{    
UInt32 inputFramesPerBuffer  = (UInt32) stream->inputFramesPerBuffer;
UInt32 outputFramesPerBuffer = (UInt32) stream->outputFramesPerBuffer;
result = OpenAndSetupOneAudioUnit( stream,
inputParameters,
outputParameters,
suggestedLatencyFramesPerBuffer,
&inputFramesPerBuffer,
&outputFramesPerBuffer,
auhalHostApi,
&(stream->inputUnit),
&(stream->inputSRConverter),
&(stream->inputDevice),
sampleRate,
stream );
stream->inputFramesPerBuffer = inputFramesPerBuffer;
stream->outputFramesPerBuffer = outputFramesPerBuffer;
stream->outputUnit = stream->inputUnit;
stream->outputDevice = stream->inputDevice;
if( result != paNoError )
goto error;
}
else
{    
UInt32 outputFramesPerBuffer = (UInt32) stream->outputFramesPerBuffer;
UInt32 inputFramesPerBuffer  = (UInt32) stream->inputFramesPerBuffer;
result = OpenAndSetupOneAudioUnit( stream,
NULL,
outputParameters,
suggestedLatencyFramesPerBuffer,
NULL,
&outputFramesPerBuffer,
auhalHostApi,
&(stream->outputUnit),
NULL,
&(stream->outputDevice),
sampleRate,
stream );
if( result != paNoError )
goto error;
result = OpenAndSetupOneAudioUnit( stream,
inputParameters,
NULL,
suggestedLatencyFramesPerBuffer,
&inputFramesPerBuffer,
NULL,
auhalHostApi,
&(stream->inputUnit),
&(stream->inputSRConverter),
&(stream->inputDevice),
sampleRate,
stream );
if( result != paNoError )
goto error;
stream->inputFramesPerBuffer = inputFramesPerBuffer;
stream->outputFramesPerBuffer = outputFramesPerBuffer;
}
inputLatencyFrames += stream->inputFramesPerBuffer;
outputLatencyFrames += stream->outputFramesPerBuffer;
if( stream->inputUnit ) {
const size_t szfl = sizeof(float);
bzero( &stream->inputAudioBufferList, sizeof( AudioBufferList ) );
stream->inputAudioBufferList.mNumberBuffers = 1;
stream->inputAudioBufferList.mBuffers[0].mNumberChannels
= inputChannelCount;
stream->inputAudioBufferList.mBuffers[0].mDataByteSize
= stream->inputFramesPerBuffer*inputChannelCount*szfl;
stream->inputAudioBufferList.mBuffers[0].mData
= (float *) calloc(
stream->inputFramesPerBuffer*inputChannelCount,
szfl );
if( !stream->inputAudioBufferList.mBuffers[0].mData )
{
result = paInsufficientMemory;
goto error;
}
if( (stream->outputUnit && (stream->inputUnit != stream->outputUnit))
|| stream->inputSRConverter )
{
void *data;
long ringSize;
ringSize = computeRingBufferSize( inputParameters,
outputParameters,
stream->inputFramesPerBuffer,
stream->outputFramesPerBuffer,
sampleRate );
data = calloc( ringSize, szfl*inputParameters->channelCount );
if( !data )
{
result = paInsufficientMemory;
goto error;
}
result = PaUtil_InitializeRingBuffer( &stream->inputRingBuffer, szfl*inputParameters->channelCount, ringSize, data );
if( result != 0 )
{
result = paUnanticipatedHostError;
free(data);
goto error;
}
if( stream->outputUnit )
PaUtil_AdvanceRingBufferWriteIndex( &stream->inputRingBuffer, ringSize / RING_BUFFER_ADVANCE_DENOMINATOR );
inputLatencyFrames += ringSize;
}
}
if( !streamCallback )
{
long ringSize;
ringSize = computeRingBufferSize( inputParameters,
outputParameters,
stream->inputFramesPerBuffer,
stream->outputFramesPerBuffer,
sampleRate );
result = initializeBlioRingBuffers( &stream->blio,
inputParameters ? inputParameters->sampleFormat : 0,
outputParameters ? outputParameters->sampleFormat : 0,
ringSize,
inputParameters ? inputChannelCount : 0,
outputParameters ? outputChannelCount : 0 );
if( result != paNoError )
goto error;
inputLatencyFrames += ringSize;
outputLatencyFrames += ringSize;
}
{
unsigned long maxHostFrames = stream->inputFramesPerBuffer;
if( stream->outputFramesPerBuffer > maxHostFrames )
maxHostFrames = stream->outputFramesPerBuffer;
result = PaUtil_InitializeBufferProcessor( &stream->bufferProcessor,
inputChannelCount, inputSampleFormat,
hostInputSampleFormat,
outputChannelCount, outputSampleFormat,
hostOutputSampleFormat,
sampleRate,
streamFlags,
requestedFramesPerBuffer,
maxHostFrames,
stream->inputSRConverter
? paUtilUnknownHostBufferSize
: paUtilBoundedHostBufferSize,
streamCallback ? streamCallback : BlioCallback,
streamCallback ? userData : &stream->blio );
if( result != paNoError )
goto error;
}
stream->bufferProcessorIsInitialized = TRUE;
if( inputParameters )
{
inputLatencyFrames += PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor);
stream->streamRepresentation.streamInfo.inputLatency = inputLatencyFrames / sampleRate;
}
else
{
stream->streamRepresentation.streamInfo.inputLatency = 0.0;
}
if( outputParameters )
{
outputLatencyFrames += PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor);
stream->streamRepresentation.streamInfo.outputLatency = outputLatencyFrames / sampleRate;
}
else
{
stream->streamRepresentation.streamInfo.outputLatency = 0.0;
}
stream->streamRepresentation.streamInfo.sampleRate = sampleRate;
stream->sampleRate = sampleRate;
stream->userInChan  = inputChannelCount;
stream->userOutChan = outputChannelCount;
pthread_mutex_init( &stream->timingInformationMutex, NULL );
stream->timingInformationMutexIsInitialized = 1;
InitializeDeviceProperties( &stream->inputProperties );      
InitializeDeviceProperties( &stream->outputProperties );     
if( stream->outputUnit )
{
Boolean isInput = FALSE;
result = ERR( UpdateSampleRateFromDeviceProperty(
stream, stream->outputDevice, isInput, kAudioDevicePropertyNominalSampleRate )  );
if( result )
goto error;  
UpdateSampleRateFromDeviceProperty( stream, stream->outputDevice, isInput, kAudioDevicePropertyActualSampleRate );
SetupDevicePropertyListeners( stream, stream->outputDevice, isInput );
}
if( stream->inputUnit )
{
Boolean isInput = TRUE;
result = ERR( UpdateSampleRateFromDeviceProperty(
stream, stream->inputDevice, isInput, kAudioDevicePropertyNominalSampleRate )  );
if( result )
goto error;
UpdateSampleRateFromDeviceProperty( stream, stream->inputDevice, isInput, kAudioDevicePropertyActualSampleRate );
SetupDevicePropertyListeners( stream, stream->inputDevice, isInput );
}
UpdateTimeStampOffsets( stream );
stream->timestampOffsetCombined_ioProcCopy = stream->timestampOffsetCombined;
stream->timestampOffsetInputDevice_ioProcCopy = stream->timestampOffsetInputDevice;
stream->timestampOffsetOutputDevice_ioProcCopy = stream->timestampOffsetOutputDevice;
stream->state = STOPPED;
stream->xrunFlags = 0;
*s = (PaStream*)stream;
return result;
error:
CloseStream( stream );
return result;
}
#define HOST_TIME_TO_PA_TIME( x ) ( AudioConvertHostTimeToNanos( (x) ) * 1.0E-09)  
PaTime GetStreamTime( PaStream *s )
{
return HOST_TIME_TO_PA_TIME( AudioGetCurrentHostTime() );
}
#define RING_BUFFER_EMPTY (1000)
static OSStatus ringBufferIOProc(
AudioConverterRef              inAudioConverter,
UInt32*                        ioNumberDataPackets,
AudioBufferList*               ioData,
AudioStreamPacketDescription** outDataPacketDescription,
void*                          inUserData)
{
VVDBUG(("ringBufferIOProc()\n"));
PaUtilRingBuffer *rb = (PaUtilRingBuffer *) inUserData;
if( PaUtil_GetRingBufferReadAvailable( rb ) == 0 ) {
ioData->mBuffers[0].mData = NULL;
ioData->mBuffers[0].mDataByteSize = 0;
*ioNumberDataPackets = 0;
VVDBUG(("Ring buffer empty!\n"));
return RING_BUFFER_EMPTY;
}
UInt32 packetSize = sizeof(float) * ioData->mBuffers[0].mNumberChannels;
UInt32 dataSize = *ioNumberDataPackets * packetSize;
assert(dataSize % rb->elementSizeBytes == 0);
UInt32 rbElements = dataSize / rb->elementSizeBytes;
ring_buffer_size_t rbElementsRead = rbElements;
void *dummyData;
ring_buffer_size_t dummySize;
PaUtil_GetRingBufferReadRegions( rb, rbElements,
&ioData->mBuffers[0].mData, &rbElementsRead,
&dummyData, &dummySize );
assert(rbElementsRead > 0);
VVDBUG(("RingBuffer read elements %u of %u\n", rbElementsRead, rbElements));
PaUtil_AdvanceRingBufferReadIndex( rb, rbElementsRead );
UInt32 bytesRead = rbElementsRead * rb->elementSizeBytes;
ioData->mBuffers[0].mDataByteSize = bytesRead;
*ioNumberDataPackets = bytesRead / packetSize;
return noErr;
}
static OSStatus AudioIOProc( void *inRefCon,
AudioUnitRenderActionFlags *ioActionFlags,
const AudioTimeStamp *inTimeStamp,
UInt32 inBusNumber,
UInt32 inNumberFrames,
AudioBufferList *ioData )
{
unsigned long framesProcessed     = 0;
PaStreamCallbackTimeInfo timeInfo = {0,0,0};
PaMacCoreStream *stream           = (PaMacCoreStream*)inRefCon;
const bool isRender               = inBusNumber == OUTPUT_ELEMENT;
int callbackResult                = paContinue ;
double hostTimeStampInPaTime      = HOST_TIME_TO_PA_TIME(inTimeStamp->mHostTime);
VVDBUG(("AudioIOProc()\n"));
PaUtil_BeginCpuLoadMeasurement( &stream->cpuLoadMeasurer );
if( pthread_mutex_trylock( &stream->timingInformationMutex ) == 0 ) {
stream->timestampOffsetCombined_ioProcCopy = stream->timestampOffsetCombined;
stream->timestampOffsetInputDevice_ioProcCopy = stream->timestampOffsetInputDevice;
stream->timestampOffsetOutputDevice_ioProcCopy = stream->timestampOffsetOutputDevice;
pthread_mutex_unlock( &stream->timingInformationMutex );
}
timeInfo.currentTime = HOST_TIME_TO_PA_TIME( AudioGetCurrentHostTime() );
if( isRender )
{
if( stream->inputUnit )  
{
if( stream->inputUnit == stream->outputUnit )  
{
timeInfo.inputBufferAdcTime = hostTimeStampInPaTime -
(stream->timestampOffsetCombined_ioProcCopy + stream->timestampOffsetInputDevice_ioProcCopy);
timeInfo.outputBufferDacTime = hostTimeStampInPaTime + stream->timestampOffsetOutputDevice_ioProcCopy;
}
else  
{
timeInfo.inputBufferAdcTime = hostTimeStampInPaTime -
(stream->timestampOffsetCombined_ioProcCopy + stream->timestampOffsetInputDevice_ioProcCopy);
timeInfo.outputBufferDacTime = hostTimeStampInPaTime + stream->timestampOffsetOutputDevice_ioProcCopy;
}
}
else  
{
timeInfo.inputBufferAdcTime = 0;
timeInfo.outputBufferDacTime = hostTimeStampInPaTime + stream->timestampOffsetOutputDevice_ioProcCopy;
}
}
else  
{
timeInfo.inputBufferAdcTime = hostTimeStampInPaTime - stream->timestampOffsetInputDevice_ioProcCopy;
timeInfo.outputBufferDacTime = 0;
}
if( isRender && stream->inputUnit == stream->outputUnit
&& !stream->inputSRConverter )
{
OSStatus err = 0;
unsigned long frames;
long bytesPerFrame = sizeof( float ) * ioData->mBuffers[0].mNumberChannels;
PaUtil_BeginBufferProcessing( &(stream->bufferProcessor),
&timeInfo,
stream->xrunFlags );
stream->xrunFlags = 0;  
assert( ioData->mNumberBuffers == 1 );
assert( ioData->mBuffers[0].mNumberChannels == stream->userOutChan );
frames = ioData->mBuffers[0].mDataByteSize / bytesPerFrame;
err= AudioUnitRender( stream->inputUnit,
ioActionFlags,
inTimeStamp,
INPUT_ELEMENT,
inNumberFrames,
&stream->inputAudioBufferList );
if(err != noErr)
{
goto stop_stream;
}
PaUtil_SetInputFrameCount( &(stream->bufferProcessor), frames );
PaUtil_SetInterleavedInputChannels( &(stream->bufferProcessor),
0,
stream->inputAudioBufferList.mBuffers[0].mData,
stream->inputAudioBufferList.mBuffers[0].mNumberChannels );
PaUtil_SetOutputFrameCount( &(stream->bufferProcessor), frames );
PaUtil_SetInterleavedOutputChannels( &(stream->bufferProcessor),
0,
ioData->mBuffers[0].mData,
ioData->mBuffers[0].mNumberChannels );
framesProcessed =
PaUtil_EndBufferProcessing( &(stream->bufferProcessor),
&callbackResult );
}
else if( isRender )
{
unsigned long frames;
long bytesPerFrame = sizeof( float ) * ioData->mBuffers[0].mNumberChannels;
int xrunFlags = stream->xrunFlags;
if( stream->state == STOPPING || stream->state == CALLBACK_STOPPED )
xrunFlags = 0;
PaUtil_BeginBufferProcessing( &(stream->bufferProcessor),
&timeInfo,
xrunFlags );
stream->xrunFlags = 0;  
assert( ioData->mNumberBuffers == 1 );
frames = ioData->mBuffers[0].mDataByteSize / bytesPerFrame;
assert( ioData->mBuffers[0].mNumberChannels == stream->userOutChan );
PaUtil_SetOutputFrameCount( &(stream->bufferProcessor), frames );
PaUtil_SetInterleavedOutputChannels( &(stream->bufferProcessor),
0,
ioData->mBuffers[0].mData,
ioData->mBuffers[0].mNumberChannels );
if( stream->inputUnit ) {
const int flsz = sizeof( float );
int inChan = stream->inputAudioBufferList.mBuffers[0].mNumberChannels;
long bytesPerFrame = flsz * inChan;
if( stream->inputSRConverter )
{
OSStatus err;
float data[ inChan * frames ];
AudioBufferList bufferList;
bufferList.mNumberBuffers = 1;
bufferList.mBuffers[0].mNumberChannels = inChan;
bufferList.mBuffers[0].mDataByteSize = sizeof( data );
bufferList.mBuffers[0].mData = data;
UInt32 packets = frames;
err = AudioConverterFillComplexBuffer(
stream->inputSRConverter,
ringBufferIOProc,
&stream->inputRingBuffer,
&packets,
&bufferList,
NULL);
if( err == RING_BUFFER_EMPTY )
{    
err = 0;
UInt32 size = packets * bytesPerFrame;
bzero( ((char *)data) + size, sizeof(data)-size );
if( stream->state == ACTIVE )
{
stream->xrunFlags |= paInputUnderflow;
}
}
ERR( err );
if(err != noErr)
{
goto stop_stream;
}
PaUtil_SetInputFrameCount( &(stream->bufferProcessor), frames );
PaUtil_SetInterleavedInputChannels( &(stream->bufferProcessor),
0,
data,
inChan );
framesProcessed =
PaUtil_EndBufferProcessing( &(stream->bufferProcessor),
&callbackResult );
}
else
{
void *data1, *data2;
ring_buffer_size_t size1, size2;
ring_buffer_size_t framesReadable = PaUtil_GetRingBufferReadRegions( &stream->inputRingBuffer,
frames,
&data1, &size1,
&data2, &size2 );
if( size1 == frames ) {
PaUtil_SetInputFrameCount( &(stream->bufferProcessor), frames );
PaUtil_SetInterleavedInputChannels( &(stream->bufferProcessor),
0,
data1,
inChan );
framesProcessed =
PaUtil_EndBufferProcessing( &(stream->bufferProcessor),
&callbackResult );
PaUtil_AdvanceRingBufferReadIndex(&stream->inputRingBuffer, size1 );
} else if( framesReadable < frames ) {
long sizeBytes1 = size1 * bytesPerFrame;
long sizeBytes2 = size2 * bytesPerFrame;
unsigned char data[ frames * bytesPerFrame ];
if( size1 > 0 )
{
memcpy( data, data1, sizeBytes1 );
}
if( size2 > 0 )
{
memcpy( data+sizeBytes1, data2, sizeBytes2 );
}
bzero( data+sizeBytes1+sizeBytes2, (frames*bytesPerFrame) - sizeBytes1 - sizeBytes2 );
PaUtil_SetInputFrameCount( &(stream->bufferProcessor), frames );
PaUtil_SetInterleavedInputChannels( &(stream->bufferProcessor),
0,
data,
inChan );
framesProcessed =
PaUtil_EndBufferProcessing( &(stream->bufferProcessor),
&callbackResult );
PaUtil_AdvanceRingBufferReadIndex( &stream->inputRingBuffer,
framesReadable );
stream->xrunFlags |= paInputUnderflow;
} else {
PaUtil_SetInputFrameCount( &(stream->bufferProcessor), size1 );
PaUtil_SetInterleavedInputChannels( &(stream->bufferProcessor),
0,
data1,
inChan );
PaUtil_Set2ndInputFrameCount( &(stream->bufferProcessor), size2 );
PaUtil_Set2ndInterleavedInputChannels( &(stream->bufferProcessor),
0,
data2,
inChan );
framesProcessed =
PaUtil_EndBufferProcessing( &(stream->bufferProcessor),
&callbackResult );
PaUtil_AdvanceRingBufferReadIndex(&stream->inputRingBuffer, framesReadable );
}
}
} else {
framesProcessed =
PaUtil_EndBufferProcessing( &(stream->bufferProcessor),
&callbackResult );
}
}
else
{
OSStatus err = 0;
int chan = stream->inputAudioBufferList.mBuffers[0].mNumberChannels ;
do {
err= AudioUnitRender( stream->inputUnit,
ioActionFlags,
inTimeStamp,
INPUT_ELEMENT,
inNumberFrames,
&stream->inputAudioBufferList );
if( err == -10874 )
inNumberFrames /= 2;
} while( err == -10874 && inNumberFrames > 1 );
ERR( err );
if(err != noErr)
{
goto stop_stream;
}
if( stream->inputSRConverter || stream->outputUnit )
{
ring_buffer_size_t framesWritten = PaUtil_WriteRingBuffer( &stream->inputRingBuffer,
stream->inputAudioBufferList.mBuffers[0].mData,
inNumberFrames );
if( framesWritten != inNumberFrames )
{
stream->xrunFlags |= paInputOverflow ;
}
}
else
{
PaUtil_BeginBufferProcessing( &(stream->bufferProcessor),
&timeInfo,
stream->xrunFlags );
stream->xrunFlags = 0;
PaUtil_SetInputFrameCount( &(stream->bufferProcessor), inNumberFrames);
PaUtil_SetInterleavedInputChannels( &(stream->bufferProcessor),
0,
stream->inputAudioBufferList.mBuffers[0].mData,
chan );
framesProcessed =
PaUtil_EndBufferProcessing( &(stream->bufferProcessor),
&callbackResult );
}
if( !stream->outputUnit && stream->inputSRConverter )
{
float data[ chan * inNumberFrames ];
OSStatus err;
do
{    
AudioBufferList bufferList;
bufferList.mNumberBuffers = 1;
bufferList.mBuffers[0].mNumberChannels = chan;
bufferList.mBuffers[0].mDataByteSize = sizeof( data );
bufferList.mBuffers[0].mData = data;
UInt32 packets = inNumberFrames;
err = AudioConverterFillComplexBuffer(
stream->inputSRConverter,
ringBufferIOProc,
&stream->inputRingBuffer,
&packets,
&bufferList,
NULL);
if( err != RING_BUFFER_EMPTY )
ERR( err );
if( err != noErr && err != RING_BUFFER_EMPTY )
{
goto stop_stream;
}
PaUtil_SetInputFrameCount( &(stream->bufferProcessor), packets );
if( packets > 0 )
{
PaUtil_BeginBufferProcessing( &(stream->bufferProcessor),
&timeInfo,
stream->xrunFlags );
stream->xrunFlags = 0;
PaUtil_SetInterleavedInputChannels( &(stream->bufferProcessor),
0,
data,
chan );
framesProcessed =
PaUtil_EndBufferProcessing( &(stream->bufferProcessor),
&callbackResult );
}
} while( callbackResult == paContinue && !err );
}
}
if( callbackResult == paContinue )
{
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, framesProcessed );
return noErr;
}
stop_stream:
stream->state = CALLBACK_STOPPED ;
if( stream->outputUnit )
AudioOutputUnitStop(stream->outputUnit);
if( stream->inputUnit )
AudioOutputUnitStop(stream->inputUnit);
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, framesProcessed );
return noErr;
}
static PaError CloseStream( PaStream* s )
{
PaError result = paNoError;
PaMacCoreStream *stream = (PaMacCoreStream*)s;
VVDBUG(("CloseStream()\n"));
VDBUG( ( "Closing stream.\n" ) );
if( stream ) {
if( stream->outputUnit )
{
Boolean isInput = FALSE;
CleanupDevicePropertyListeners( stream, stream->outputDevice, isInput );
}
if( stream->inputUnit )
{
Boolean isInput = TRUE;
CleanupDevicePropertyListeners( stream, stream->inputDevice, isInput );
}
if( stream->outputUnit ) {
int count = removeFromXRunListenerList( stream );
if( count == 0 )
PaMacCore_AudioDeviceRemovePropertyListener( stream->outputDevice,
0,
false,
kAudioDeviceProcessorOverload,
xrunCallback, NULL );  
}
if( stream->inputUnit && stream->outputUnit != stream->inputUnit ) {
int count = removeFromXRunListenerList( stream );
if( count == 0 )
PaMacCore_AudioDeviceRemovePropertyListener( stream->inputDevice,
0,
true,
kAudioDeviceProcessorOverload,
xrunCallback, NULL );  
}
if( stream->outputUnit && stream->outputUnit != stream->inputUnit ) {
AudioUnitUninitialize( stream->outputUnit );
AudioComponentInstanceDispose( stream->outputUnit );
}
stream->outputUnit = NULL;
if( stream->inputUnit )
{
AudioUnitUninitialize( stream->inputUnit );
AudioComponentInstanceDispose( stream->inputUnit );
stream->inputUnit = NULL;
}
if( stream->inputRingBuffer.buffer )
free( (void *) stream->inputRingBuffer.buffer );
stream->inputRingBuffer.buffer = NULL;
if( stream->inputSRConverter )
ERR( AudioConverterDispose( stream->inputSRConverter ) );
stream->inputSRConverter = NULL;
if( stream->inputAudioBufferList.mBuffers[0].mData )
free( stream->inputAudioBufferList.mBuffers[0].mData );
stream->inputAudioBufferList.mBuffers[0].mData = NULL;
result = destroyBlioRingBuffers( &stream->blio );
if( result )
return result;
if( stream->bufferProcessorIsInitialized )
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
if( stream->timingInformationMutexIsInitialized )
pthread_mutex_destroy( &stream->timingInformationMutex );
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
PaUtil_FreeMemory( stream );
}
return result;
}
static PaError StartStream( PaStream *s )
{
PaMacCoreStream *stream = (PaMacCoreStream*)s;
OSStatus result = noErr;
VVDBUG(("StartStream()\n"));
VDBUG( ( "Starting stream.\n" ) );
#define ERR_WRAP(mac_err) do { result = mac_err ; if ( result != noErr ) return ERR(result) ; } while(0)
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );
if(  stream->inputSRConverter )
ERR_WRAP( AudioConverterReset( stream->inputSRConverter ) );
stream->state = ACTIVE;
if( stream->inputUnit ) {
ERR_WRAP( AudioOutputUnitStart(stream->inputUnit) );
}
if( stream->outputUnit && stream->outputUnit != stream->inputUnit ) {
ERR_WRAP( AudioOutputUnitStart(stream->outputUnit) );
}
return paNoError;
#undef ERR_WRAP
}
static ComponentResult BlockWhileAudioUnitIsRunning( AudioUnit audioUnit, AudioUnitElement element )
{
Boolean isRunning = 1;
while( isRunning ) {
UInt32 s = sizeof( isRunning );
ComponentResult err = AudioUnitGetProperty( audioUnit, kAudioOutputUnitProperty_IsRunning, kAudioUnitScope_Global, element,  &isRunning, &s );
if( err )
return err;
Pa_Sleep( 100 );
}
return noErr;
}
static PaError FinishStoppingStream( PaMacCoreStream *stream )
{
OSStatus result = noErr;
PaError paErr;
#define ERR_WRAP(mac_err) do { result = mac_err ; if ( result != noErr ) return ERR(result) ; } while(0)
if( stream->inputUnit == stream->outputUnit && stream->inputUnit )
{
ERR_WRAP( AudioOutputUnitStop(stream->inputUnit) );
ERR_WRAP( BlockWhileAudioUnitIsRunning(stream->inputUnit,0) );
ERR_WRAP( BlockWhileAudioUnitIsRunning(stream->inputUnit,1) );
ERR_WRAP( AudioUnitReset(stream->inputUnit, kAudioUnitScope_Global, 1) );
ERR_WRAP( AudioUnitReset(stream->inputUnit, kAudioUnitScope_Global, 0) );
}
else
{
if( stream->inputUnit )
{
ERR_WRAP(AudioOutputUnitStop(stream->inputUnit) );
ERR_WRAP( BlockWhileAudioUnitIsRunning(stream->inputUnit,1) );
ERR_WRAP(AudioUnitReset(stream->inputUnit,kAudioUnitScope_Global,1));
}
if( stream->outputUnit )
{
ERR_WRAP(AudioOutputUnitStop(stream->outputUnit));
ERR_WRAP( BlockWhileAudioUnitIsRunning(stream->outputUnit,0) );
ERR_WRAP(AudioUnitReset(stream->outputUnit,kAudioUnitScope_Global,0));
}
}
if( stream->inputRingBuffer.buffer ) {
PaUtil_FlushRingBuffer( &stream->inputRingBuffer );
bzero( (void *)stream->inputRingBuffer.buffer,
stream->inputRingBuffer.bufferSize );
if( stream->outputUnit )
PaUtil_AdvanceRingBufferWriteIndex( &stream->inputRingBuffer,
stream->inputRingBuffer.bufferSize
/ RING_BUFFER_ADVANCE_DENOMINATOR );
}
stream->xrunFlags = 0;
stream->state = STOPPED;
paErr = resetBlioRingBuffers( &stream->blio );
if( paErr )
return paErr;
VDBUG( ( "Stream Stopped.\n" ) );
return paNoError;
#undef ERR_WRAP
}
static PaError StopStream( PaStream *s )
{
PaError paErr;
PaMacCoreStream *stream = (PaMacCoreStream*)s;
VVDBUG(("StopStream()\n"));
stream->state = STOPPING;
if( stream->userOutChan > 0 )  
{
size_t maxHostFrames = MAX( stream->inputFramesPerBuffer, stream->outputFramesPerBuffer );
VDBUG( ("Waiting for write buffer to be drained.\n") );
paErr = waitUntilBlioWriteBufferIsEmpty( &stream->blio, stream->sampleRate,
maxHostFrames );
VDBUG( ( "waitUntilBlioWriteBufferIsEmpty returned %d\n", paErr ) );
}
return FinishStoppingStream( stream );
}
static PaError AbortStream( PaStream *s )
{
PaMacCoreStream *stream = (PaMacCoreStream*)s;
VDBUG( ( "AbortStream()\n" ) );
stream->state = STOPPING;
return FinishStoppingStream( stream );
}
static PaError IsStreamStopped( PaStream *s )
{
PaMacCoreStream *stream = (PaMacCoreStream*)s;
VVDBUG(("IsStreamStopped()\n"));
return stream->state == STOPPED ? 1 : 0;
}
static PaError IsStreamActive( PaStream *s )
{
PaMacCoreStream *stream = (PaMacCoreStream*)s;
VVDBUG(("IsStreamActive()\n"));
return ( stream->state == ACTIVE || stream->state == STOPPING );
}
static double GetStreamCpuLoad( PaStream* s )
{
PaMacCoreStream *stream = (PaMacCoreStream*)s;
VVDBUG(("GetStreamCpuLoad()\n"));
return PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer );
}

 
#include <string.h>  
#include "pa_util.h"
#include "pa_allocation.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_cpuload.h"
#include "pa_process.h"
#ifdef __cplusplus
extern "C"
{
#endif  
PaError PaSkeleton_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
#ifdef __cplusplus
}
#endif  
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
static double GetStreamCpuLoad( PaStream* stream );
static PaError ReadStream( PaStream* stream, void *buffer, unsigned long frames );
static PaError WriteStream( PaStream* stream, const void *buffer, unsigned long frames );
static signed long GetStreamReadAvailable( PaStream* stream );
static signed long GetStreamWriteAvailable( PaStream* stream );
#define PA_SKELETON_SET_LAST_HOST_ERROR( errorCode, errorText ) \
PaUtil_SetLastHostErrorInfo( paInDevelopment, errorCode, errorText )
typedef struct
{
PaUtilHostApiRepresentation inheritedHostApiRep;
PaUtilStreamInterface callbackStreamInterface;
PaUtilStreamInterface blockingStreamInterface;
PaUtilAllocationGroup *allocations;
}
PaSkeletonHostApiRepresentation;   
PaError PaSkeleton_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result = paNoError;
int i, deviceCount;
PaSkeletonHostApiRepresentation *skeletonHostApi;
PaDeviceInfo *deviceInfoArray;
skeletonHostApi = (PaSkeletonHostApiRepresentation*)PaUtil_AllocateMemory( sizeof(PaSkeletonHostApiRepresentation) );
if( !skeletonHostApi )
{
result = paInsufficientMemory;
goto error;
}
skeletonHostApi->allocations = PaUtil_CreateAllocationGroup();
if( !skeletonHostApi->allocations )
{
result = paInsufficientMemory;
goto error;
}
*hostApi = &skeletonHostApi->inheritedHostApiRep;
(*hostApi)->info.structVersion = 1;
(*hostApi)->info.type = paInDevelopment;             
(*hostApi)->info.name = "skeleton implementation";   
(*hostApi)->info.defaultInputDevice = paNoDevice;   
(*hostApi)->info.defaultOutputDevice = paNoDevice;  
(*hostApi)->info.deviceCount = 0;
deviceCount = 0;  
if( deviceCount > 0 )
{
(*hostApi)->deviceInfos = (PaDeviceInfo**)PaUtil_GroupAllocateMemory(
skeletonHostApi->allocations, sizeof(PaDeviceInfo*) * deviceCount );
if( !(*hostApi)->deviceInfos )
{
result = paInsufficientMemory;
goto error;
}
deviceInfoArray = (PaDeviceInfo*)PaUtil_GroupAllocateMemory(
skeletonHostApi->allocations, sizeof(PaDeviceInfo) * deviceCount );
if( !deviceInfoArray )
{
result = paInsufficientMemory;
goto error;
}
for( i=0; i < deviceCount; ++i )
{
PaDeviceInfo *deviceInfo = &deviceInfoArray[i];
deviceInfo->structVersion = 2;
deviceInfo->hostApi = hostApiIndex;
deviceInfo->name = 0;  
deviceInfo->maxInputChannels = 0;   
deviceInfo->maxOutputChannels = 0;   
deviceInfo->defaultLowInputLatency = 0.;   
deviceInfo->defaultLowOutputLatency = 0.;   
deviceInfo->defaultHighInputLatency = 0.;   
deviceInfo->defaultHighOutputLatency = 0.;   
deviceInfo->defaultSampleRate = 0.;  
(*hostApi)->deviceInfos[i] = deviceInfo;
++(*hostApi)->info.deviceCount;
}
}
(*hostApi)->Terminate = Terminate;
(*hostApi)->OpenStream = OpenStream;
(*hostApi)->IsFormatSupported = IsFormatSupported;
PaUtil_InitializeStreamInterface( &skeletonHostApi->callbackStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable, PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &skeletonHostApi->blockingStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream, GetStreamReadAvailable, GetStreamWriteAvailable );
return result;
error:
if( skeletonHostApi )
{
if( skeletonHostApi->allocations )
{
PaUtil_FreeAllAllocations( skeletonHostApi->allocations );
PaUtil_DestroyAllocationGroup( skeletonHostApi->allocations );
}
PaUtil_FreeMemory( skeletonHostApi );
}
return result;
}
static void Terminate( struct PaUtilHostApiRepresentation *hostApi )
{
PaSkeletonHostApiRepresentation *skeletonHostApi = (PaSkeletonHostApiRepresentation*)hostApi;
if( skeletonHostApi->allocations )
{
PaUtil_FreeAllAllocations( skeletonHostApi->allocations );
PaUtil_DestroyAllocationGroup( skeletonHostApi->allocations );
}
PaUtil_FreeMemory( skeletonHostApi );
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate )
{
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
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
if( inputParameters->hostApiSpecificStreamInfo )
return paIncompatibleHostApiSpecificStreamInfo;  
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
if( outputParameters->hostApiSpecificStreamInfo )
return paIncompatibleHostApiSpecificStreamInfo;  
}
else
{
outputChannelCount = 0;
}
(void) sampleRate;
return paFormatIsSupported;
}
typedef struct PaSkeletonStream
{  
PaUtilStreamRepresentation streamRepresentation;
PaUtilCpuLoadMeasurer cpuLoadMeasurer;
PaUtilBufferProcessor bufferProcessor;
unsigned long framesPerHostCallback;  
}
PaSkeletonStream;
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream** s,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback *streamCallback,
void *userData )
{
PaError result = paNoError;
PaSkeletonHostApiRepresentation *skeletonHostApi = (PaSkeletonHostApiRepresentation*)hostApi;
PaSkeletonStream *stream = 0;
unsigned long framesPerHostBuffer = framesPerBuffer;  
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaSampleFormat hostInputSampleFormat, hostOutputSampleFormat;
if( inputParameters )
{
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
if( inputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( inputChannelCount > hostApi->deviceInfos[ inputParameters->device ]->maxInputChannels )
return paInvalidChannelCount;
if( inputParameters->hostApiSpecificStreamInfo )
return paIncompatibleHostApiSpecificStreamInfo;  
hostInputSampleFormat =
PaUtil_SelectClosestAvailableFormat( paInt16  , inputSampleFormat );
}
else
{
inputChannelCount = 0;
inputSampleFormat = hostInputSampleFormat = paInt16;  
}
if( outputParameters )
{
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
if( outputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( outputChannelCount > hostApi->deviceInfos[ outputParameters->device ]->maxOutputChannels )
return paInvalidChannelCount;
if( outputParameters->hostApiSpecificStreamInfo )
return paIncompatibleHostApiSpecificStreamInfo;  
hostOutputSampleFormat =
PaUtil_SelectClosestAvailableFormat( paInt16  , outputSampleFormat );
}
else
{
outputChannelCount = 0;
outputSampleFormat = hostOutputSampleFormat = paInt16;  
}
if( (streamFlags & paPlatformSpecificFlags) != 0 )
return paInvalidFlag;  
stream = (PaSkeletonStream*)PaUtil_AllocateMemory( sizeof(PaSkeletonStream) );
if( !stream )
{
result = paInsufficientMemory;
goto error;
}
if( streamCallback )
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&skeletonHostApi->callbackStreamInterface, streamCallback, userData );
}
else
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&skeletonHostApi->blockingStreamInterface, streamCallback, userData );
}
PaUtil_InitializeCpuLoadMeasurer( &stream->cpuLoadMeasurer, sampleRate );
result =  PaUtil_InitializeBufferProcessor( &stream->bufferProcessor,
inputChannelCount, inputSampleFormat, hostInputSampleFormat,
outputChannelCount, outputSampleFormat, hostOutputSampleFormat,
sampleRate, streamFlags, framesPerBuffer,
framesPerHostBuffer, paUtilFixedHostBufferSize,
streamCallback, userData );
if( result != paNoError )
goto error;
stream->streamRepresentation.streamInfo.inputLatency =
(PaTime)PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor) / sampleRate;  
stream->streamRepresentation.streamInfo.outputLatency =
(PaTime)PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor) / sampleRate;  
stream->streamRepresentation.streamInfo.sampleRate = sampleRate;
stream->framesPerHostCallback = framesPerHostBuffer;
*s = (PaStream*)stream;
return result;
error:
if( stream )
PaUtil_FreeMemory( stream );
return result;
}
static void ExampleHostProcessingLoop( void *inputBuffer, void *outputBuffer, void *userData )
{
PaSkeletonStream *stream = (PaSkeletonStream*)userData;
PaStreamCallbackTimeInfo timeInfo = {0,0,0};  
int callbackResult;
unsigned long framesProcessed;
PaUtil_BeginCpuLoadMeasurement( &stream->cpuLoadMeasurer );
PaUtil_BeginBufferProcessing( &stream->bufferProcessor, &timeInfo, 0   );
PaUtil_SetInputFrameCount( &stream->bufferProcessor, 0   );
PaUtil_SetInterleavedInputChannels( &stream->bufferProcessor,
0,  
inputBuffer,
0 );  
PaUtil_SetOutputFrameCount( &stream->bufferProcessor, 0   );
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor,
0,  
outputBuffer,
0 );  
callbackResult = paContinue;
framesProcessed = PaUtil_EndBufferProcessing( &stream->bufferProcessor, &callbackResult );
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, framesProcessed );
if( callbackResult == paContinue )
{
}
else if( callbackResult == paAbort )
{
if( stream->streamRepresentation.streamFinishedCallback != 0 )
stream->streamRepresentation.streamFinishedCallback( stream->streamRepresentation.userData );
}
else
{
if( stream->streamRepresentation.streamFinishedCallback != 0 )
stream->streamRepresentation.streamFinishedCallback( stream->streamRepresentation.userData );
}
}
static PaError CloseStream( PaStream* s )
{
PaError result = paNoError;
PaSkeletonStream *stream = (PaSkeletonStream*)s;
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
PaUtil_FreeMemory( stream );
return result;
}
static PaError StartStream( PaStream *s )
{
PaError result = paNoError;
PaSkeletonStream *stream = (PaSkeletonStream*)s;
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );
(void) ExampleHostProcessingLoop;
return result;
}
static PaError StopStream( PaStream *s )
{
PaError result = paNoError;
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) stream;
return result;
}
static PaError AbortStream( PaStream *s )
{
PaError result = paNoError;
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) stream;
return result;
}
static PaError IsStreamStopped( PaStream *s )
{
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) stream;
return 0;
}
static PaError IsStreamActive( PaStream *s )
{
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) stream;
return 0;
}
static PaTime GetStreamTime( PaStream *s )
{
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) stream;
return 0;
}
static double GetStreamCpuLoad( PaStream* s )
{
PaSkeletonStream *stream = (PaSkeletonStream*)s;
return PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer );
}
static PaError ReadStream( PaStream* s,
void *buffer,
unsigned long frames )
{
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) buffer;
(void) frames;
(void) stream;
return paNoError;
}
static PaError WriteStream( PaStream* s,
const void *buffer,
unsigned long frames )
{
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) buffer;
(void) frames;
(void) stream;
return paNoError;
}
static signed long GetStreamReadAvailable( PaStream* s )
{
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) stream;
return 0;
}
static signed long GetStreamWriteAvailable( PaStream* s )
{
PaSkeletonStream *stream = (PaSkeletonStream*)s;
(void) stream;
return 0;
}

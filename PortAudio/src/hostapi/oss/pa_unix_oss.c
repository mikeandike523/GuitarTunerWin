 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <limits.h>
#include <semaphore.h>
#ifdef HAVE_SYS_SOUNDCARD_H
# include <sys/soundcard.h>
# ifdef __NetBSD__
#  define DEVICE_NAME_BASE           "/dev/audio"
# else
#  define DEVICE_NAME_BASE           "/dev/dsp"
# endif
#elif defined(HAVE_LINUX_SOUNDCARD_H)
# include <linux/soundcard.h>
# define DEVICE_NAME_BASE            "/dev/dsp"
#elif defined(HAVE_MACHINE_SOUNDCARD_H)
# include <machine/soundcard.h>  
# define DEVICE_NAME_BASE            "/dev/audio"
#else
# error No sound card header file
#endif
#include "portaudio.h"
#include "pa_util.h"
#include "pa_allocation.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_cpuload.h"
#include "pa_process.h"
#include "pa_unix_util.h"
#include "pa_debugprint.h"
static int sysErr_;
static pthread_t mainThread_;
#define ENSURE_(expr, code) \
do { \
if( UNLIKELY( (sysErr_ = (expr)) < 0 ) ) \
{ \
\
if( (code) == paUnanticipatedHostError && pthread_self() == mainThread_ ) \
{ \
PaUtil_SetLastHostErrorInfo( paOSS, sysErr_, strerror( errno ) ); \
} \
\
PaUtil_DebugPrint(( "Expression '" #expr "' failed in '" __FILE__ "', line: " STRINGIZE( __LINE__ ) "\n" )); \
result = (code); \
goto error; \
} \
} while( 0 );
#ifndef AFMT_S16_NE
#define AFMT_S16_NE  Get_AFMT_S16_NE()
static int Get_AFMT_S16_NE( void )
{
long testData = 1;
char *ptr = (char *) &testData;
int isLittle = ( *ptr == 1 );  
return isLittle ? AFMT_S16_LE : AFMT_S16_BE;
}
#endif
typedef struct
{
PaUtilHostApiRepresentation inheritedHostApiRep;
PaUtilStreamInterface callbackStreamInterface;
PaUtilStreamInterface blockingStreamInterface;
PaUtilAllocationGroup *allocations;
PaHostApiIndex hostApiIndex;
}
PaOSSHostApiRepresentation;
typedef struct
{
int fd;
const char *devName;
int userChannelCount, hostChannelCount;
int userInterleaved;
void *buffer;
PaSampleFormat userFormat, hostFormat;
double latency;
unsigned long hostFrames, numBufs;
void **userBuffers;  
} PaOssStreamComponent;
typedef struct PaOssStream
{
PaUtilStreamRepresentation streamRepresentation;
PaUtilCpuLoadMeasurer cpuLoadMeasurer;
PaUtilBufferProcessor bufferProcessor;
PaUtilThreading threading;
int sharedDevice;
unsigned long framesPerHostBuffer;
int triggered;   
int isActive;
int isStopped;
int lastPosPtr;
double lastStreamBytes;
int framesProcessed;
double sampleRate;
int callbackMode;
volatile int callbackStop, callbackAbort;
PaOssStreamComponent *capture, *playback;
unsigned long pollTimeout;
sem_t semaphore;
}
PaOssStream;
typedef enum {
StreamMode_In,
StreamMode_Out
} StreamMode;
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
static PaError BuildDeviceList( PaOSSHostApiRepresentation *hostApi );
PaError PaOSS_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result = paNoError;
PaOSSHostApiRepresentation *ossHostApi = NULL;
PA_UNLESS( ossHostApi = (PaOSSHostApiRepresentation*)PaUtil_AllocateMemory( sizeof(PaOSSHostApiRepresentation) ),
paInsufficientMemory );
PA_UNLESS( ossHostApi->allocations = PaUtil_CreateAllocationGroup(), paInsufficientMemory );
ossHostApi->hostApiIndex = hostApiIndex;
*hostApi = &ossHostApi->inheritedHostApiRep;
(*hostApi)->info.structVersion = 1;
(*hostApi)->info.type = paOSS;
(*hostApi)->info.name = "OSS";
(*hostApi)->Terminate = Terminate;
(*hostApi)->OpenStream = OpenStream;
(*hostApi)->IsFormatSupported = IsFormatSupported;
PA_ENSURE( BuildDeviceList( ossHostApi ) );
PaUtil_InitializeStreamInterface( &ossHostApi->callbackStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable,
PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &ossHostApi->blockingStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream, GetStreamReadAvailable, GetStreamWriteAvailable );
mainThread_ = pthread_self();
return result;
error:
if( ossHostApi )
{
if( ossHostApi->allocations )
{
PaUtil_FreeAllAllocations( ossHostApi->allocations );
PaUtil_DestroyAllocationGroup( ossHostApi->allocations );
}
PaUtil_FreeMemory( ossHostApi );
}
return result;
}
PaError PaUtil_InitializeDeviceInfo( PaDeviceInfo *deviceInfo, const char *name, PaHostApiIndex hostApiIndex, int maxInputChannels,
int maxOutputChannels, PaTime defaultLowInputLatency, PaTime defaultLowOutputLatency, PaTime defaultHighInputLatency,
PaTime defaultHighOutputLatency, double defaultSampleRate, PaUtilAllocationGroup *allocations  )
{
PaError result = paNoError;
deviceInfo->structVersion = 2;
if( allocations )
{
size_t len = strlen( name ) + 1;
PA_UNLESS( deviceInfo->name = PaUtil_GroupAllocateMemory( allocations, len ), paInsufficientMemory );
strncpy( (char *)deviceInfo->name, name, len );
}
else
deviceInfo->name = name;
deviceInfo->hostApi = hostApiIndex;
deviceInfo->maxInputChannels = maxInputChannels;
deviceInfo->maxOutputChannels = maxOutputChannels;
deviceInfo->defaultLowInputLatency = defaultLowInputLatency;
deviceInfo->defaultLowOutputLatency = defaultLowOutputLatency;
deviceInfo->defaultHighInputLatency = defaultHighInputLatency;
deviceInfo->defaultHighOutputLatency = defaultHighOutputLatency;
deviceInfo->defaultSampleRate = defaultSampleRate;
error:
return result;
}
static int CalcHigherLogTwo( int n )
{
int log2 = 0;
while( (1<<log2) < n ) log2++;
return log2;
}
static PaError QueryDirection( const char *deviceName, StreamMode mode, double *defaultSampleRate, int *maxChannelCount,
double *defaultLowLatency, double *defaultHighLatency )
{
PaError result = paNoError;
int numChannels, maxNumChannels;
int busy = 0;
int devHandle = -1;
int sr;
*maxChannelCount = 0;   
int temp, frgmt;
unsigned long fragFrames;
if ( (devHandle = open( deviceName, (mode == StreamMode_In ? O_RDONLY : O_WRONLY) | O_NONBLOCK ))  < 0 )
{
if( errno == EBUSY || errno == EAGAIN )
{
PA_DEBUG(( "%s: Device %s busy\n", __FUNCTION__, deviceName ));
}
else
{
if( errno != ENOENT )
{
PA_DEBUG(( "%s: Can't access device %s: %s\n", __FUNCTION__, deviceName, strerror( errno ) ));
}
}
return paDeviceUnavailable;
}
maxNumChannels = 0;
for( numChannels = 1; numChannels <= 16; numChannels++ )
{
temp = numChannels;
if( ioctl( devHandle, SNDCTL_DSP_CHANNELS, &temp ) < 0 )
{
busy = EAGAIN == errno || EBUSY == errno;
if( maxNumChannels >= 2 )
break;
}
else
{
if( (numChannels > 2) && (temp != numChannels) )
break;
if( temp > maxNumChannels )
maxNumChannels = temp;  
}
}
if( 0 == maxNumChannels && busy )
{
result = paDeviceUnavailable;
goto error;
}
if( maxNumChannels < 1 )
{
int stereo = 1;
if( ioctl( devHandle, SNDCTL_DSP_STEREO, &stereo ) < 0 )
{
maxNumChannels = 1;
}
else
{
maxNumChannels = (stereo) ? 2 : 1;
}
PA_DEBUG(( "%s: use SNDCTL_DSP_STEREO, maxNumChannels = %d\n", __FUNCTION__, maxNumChannels ));
}
{
numChannels = PA_MIN( maxNumChannels, 2 );
ENSURE_( ioctl( devHandle, SNDCTL_DSP_CHANNELS, &numChannels ), paUnanticipatedHostError );
}
if( *defaultSampleRate < 0 )
{
sr = 44100;
ENSURE_( ioctl( devHandle, SNDCTL_DSP_SPEED, &sr ), paUnanticipatedHostError );
*defaultSampleRate = sr;
}
*maxChannelCount = maxNumChannels;
fragFrames = 128;
frgmt = (4 << 16) + (CalcHigherLogTwo( fragFrames * numChannels * 2 ) & 0xffff);
ENSURE_( ioctl( devHandle, SNDCTL_DSP_SETFRAGMENT, &frgmt ), paUnanticipatedHostError );
fragFrames = pow( 2, frgmt & 0xffff ) / (numChannels * 2);
*defaultLowLatency = ((frgmt >> 16) - 1) * fragFrames / *defaultSampleRate;
temp = (fragFrames < 256) ? 4 : (fragFrames < 512) ? 2 : 1;
*defaultHighLatency = temp * *defaultLowLatency;
error:
if( devHandle >= 0 )
close( devHandle );
return result;
}
static PaError QueryDevice( char *deviceName, PaOSSHostApiRepresentation *ossApi, PaDeviceInfo **deviceInfo )
{
PaError result = paNoError;
double sampleRate = -1.;
int maxInputChannels, maxOutputChannels;
PaTime defaultLowInputLatency, defaultLowOutputLatency, defaultHighInputLatency, defaultHighOutputLatency;
PaError tmpRes = paNoError;
int busy = 0;
*deviceInfo = NULL;
if( (tmpRes = QueryDirection( deviceName, StreamMode_In, &sampleRate, &maxInputChannels, &defaultLowInputLatency,
&defaultHighInputLatency )) != paNoError )
{
if( tmpRes != paDeviceUnavailable )
{
PA_DEBUG(( "%s: Querying device %s for capture failed!\n", __FUNCTION__, deviceName ));
}
++busy;
}
if( (tmpRes = QueryDirection( deviceName, StreamMode_Out, &sampleRate, &maxOutputChannels, &defaultLowOutputLatency,
&defaultHighOutputLatency )) != paNoError )
{
if( tmpRes != paDeviceUnavailable )
{
PA_DEBUG(( "%s: Querying device %s for playback failed!\n", __FUNCTION__, deviceName ));
}
++busy;
}
assert( 0 <= busy && busy <= 2 );
if( 2 == busy )      
{
result = paDeviceUnavailable;
goto error;
}
PA_UNLESS( *deviceInfo = PaUtil_GroupAllocateMemory( ossApi->allocations, sizeof (PaDeviceInfo) ), paInsufficientMemory );
PA_ENSURE( PaUtil_InitializeDeviceInfo( *deviceInfo, deviceName, ossApi->hostApiIndex, maxInputChannels, maxOutputChannels,
defaultLowInputLatency, defaultLowOutputLatency, defaultHighInputLatency, defaultHighOutputLatency, sampleRate,
ossApi->allocations ) );
error:
return result;
}
static PaError BuildDeviceList( PaOSSHostApiRepresentation *ossApi )
{
PaError result = paNoError;
PaUtilHostApiRepresentation *commonApi = &ossApi->inheritedHostApiRep;
int i;
int numDevices = 0, maxDeviceInfos = 1;
PaDeviceInfo **deviceInfos = NULL;
commonApi->info.defaultInputDevice = paNoDevice;
commonApi->info.defaultOutputDevice = paNoDevice;
for( i = -1; i < 100; i++ )
{
char deviceName[32];
PaDeviceInfo *deviceInfo;
int testResult;
if( i == -1 )
snprintf(deviceName, sizeof (deviceName), "%s", DEVICE_NAME_BASE);
else
snprintf(deviceName, sizeof (deviceName), "%s%d", DEVICE_NAME_BASE, i);
if( (testResult = QueryDevice( deviceName, ossApi, &deviceInfo )) != paNoError )
{
if( testResult != paDeviceUnavailable )
PA_ENSURE( testResult );
continue;
}
++numDevices;
if( !deviceInfos || numDevices > maxDeviceInfos )
{
maxDeviceInfos *= 2;
PA_UNLESS( deviceInfos = (PaDeviceInfo **) realloc( deviceInfos, maxDeviceInfos * sizeof (PaDeviceInfo *) ),
paInsufficientMemory );
}
{
int devIdx = numDevices - 1;
deviceInfos[devIdx] = deviceInfo;
if( commonApi->info.defaultInputDevice == paNoDevice && deviceInfo->maxInputChannels > 0 )
commonApi->info.defaultInputDevice = devIdx;
if( commonApi->info.defaultOutputDevice == paNoDevice && deviceInfo->maxOutputChannels > 0 )
commonApi->info.defaultOutputDevice = devIdx;
}
}
PA_DEBUG(("PaOSS %s: Total number of devices found: %d\n", __FUNCTION__, numDevices));
commonApi->deviceInfos = (PaDeviceInfo**)PaUtil_GroupAllocateMemory(
ossApi->allocations, sizeof(PaDeviceInfo*) * numDevices );
memcpy( commonApi->deviceInfos, deviceInfos, numDevices * sizeof (PaDeviceInfo *) );
commonApi->info.deviceCount = numDevices;
error:
free( deviceInfos );
return result;
}
static void Terminate( struct PaUtilHostApiRepresentation *hostApi )
{
PaOSSHostApiRepresentation *ossHostApi = (PaOSSHostApiRepresentation*)hostApi;
if( ossHostApi->allocations )
{
PaUtil_FreeAllAllocations( ossHostApi->allocations );
PaUtil_DestroyAllocationGroup( ossHostApi->allocations );
}
PaUtil_FreeMemory( ossHostApi );
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate )
{
PaError result = paNoError;
PaDeviceIndex device;
PaDeviceInfo *deviceInfo;
char *deviceName;
int inputChannelCount, outputChannelCount;
int tempDevHandle = -1;
int flags;
PaSampleFormat inputSampleFormat, outputSampleFormat;
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
}
else
{
inputChannelCount = 0;
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
}
else
{
outputChannelCount = 0;
}
if (inputChannelCount == 0 && outputChannelCount == 0)
return paInvalidChannelCount;
if (inputChannelCount > 0 && outputChannelCount > 0 &&
inputParameters->device != outputParameters->device)
return paInvalidDevice;
if (inputChannelCount > 0 && outputChannelCount > 0 &&
inputChannelCount != outputChannelCount)
return paInvalidChannelCount;
if( inputChannelCount > 0 )
{
result = PaUtil_DeviceIndexToHostApiDeviceIndex(&device, inputParameters->device, hostApi);
if (result != paNoError)
return result;
}
else
{
result = PaUtil_DeviceIndexToHostApiDeviceIndex(&device, outputParameters->device, hostApi);
if (result != paNoError)
return result;
}
deviceInfo = hostApi->deviceInfos[device];
deviceName = (char *)deviceInfo->name;
flags = O_NONBLOCK;
if (inputChannelCount > 0 && outputChannelCount > 0)
flags |= O_RDWR;
else if (inputChannelCount > 0)
flags |= O_RDONLY;
else
flags |= O_WRONLY;
ENSURE_( tempDevHandle = open( deviceInfo->name, flags ), paDeviceUnavailable );
error:
if( tempDevHandle >= 0 )
close( tempDevHandle );
return result;
}
static PaError ValidateParameters( const PaStreamParameters *parameters, const PaDeviceInfo *deviceInfo, StreamMode mode )
{
int maxChans;
assert( parameters );
if( parameters->device == paUseHostApiSpecificDeviceSpecification )
{
return paInvalidDevice;
}
maxChans = (mode == StreamMode_In ? deviceInfo->maxInputChannels : deviceInfo->maxOutputChannels);
if( parameters->channelCount > maxChans )
{
return paInvalidChannelCount;
}
return paNoError;
}
static PaError PaOssStreamComponent_Initialize( PaOssStreamComponent *component, const PaStreamParameters *parameters,
int callbackMode, int fd, const char *deviceName )
{
PaError result = paNoError;
assert( component );
memset( component, 0, sizeof (PaOssStreamComponent) );
component->fd = fd;
component->devName = deviceName;
component->userChannelCount = parameters->channelCount;
component->userFormat = parameters->sampleFormat;
component->latency = parameters->suggestedLatency;
component->userInterleaved = !(parameters->sampleFormat & paNonInterleaved);
if( !callbackMode && !component->userInterleaved )
{
PA_UNLESS( component->userBuffers = PaUtil_AllocateMemory( sizeof (void *) * component->userChannelCount ),
paInsufficientMemory );
}
error:
return result;
}
static void PaOssStreamComponent_Terminate( PaOssStreamComponent *component )
{
assert( component );
if( component->fd >= 0 )
close( component->fd );
if( component->buffer )
PaUtil_FreeMemory( component->buffer );
if( component->userBuffers )
PaUtil_FreeMemory( component->userBuffers );
PaUtil_FreeMemory( component );
}
static PaError ModifyBlocking( int fd, int blocking )
{
PaError result = paNoError;
int fflags;
ENSURE_( fflags = fcntl( fd, F_GETFL ), paUnanticipatedHostError );
if( blocking )
fflags &= ~O_NONBLOCK;
else
fflags |= O_NONBLOCK;
ENSURE_( fcntl( fd, F_SETFL, fflags ), paUnanticipatedHostError );
error:
return result;
}
static PaError OpenDevices( const char *idevName, const char *odevName, int *idev, int *odev )
{
PaError result = paNoError;
int flags = O_NONBLOCK, duplex = 0;
*idev = *odev = -1;
if( idevName && odevName )
{
duplex = 1;
flags |= O_RDWR;
}
else if( idevName )
flags |= O_RDONLY;
else
flags |= O_WRONLY;
assert( flags & O_NONBLOCK );
if( idevName )
{
ENSURE_( *idev = open( idevName, flags ), paDeviceUnavailable );
PA_ENSURE( ModifyBlocking( *idev, 1 ) );  
}
if( odevName )
{
if( !idevName )
{
ENSURE_( *odev = open( odevName, flags ), paDeviceUnavailable );
PA_ENSURE( ModifyBlocking( *odev, 1 ) );  
}
else
{
ENSURE_( *odev = dup( *idev ), paUnanticipatedHostError );
}
}
error:
return result;
}
static PaError PaOssStream_Initialize( PaOssStream *stream, const PaStreamParameters *inputParameters, const PaStreamParameters *outputParameters,
PaStreamCallback callback, void *userData, PaStreamFlags streamFlags,
PaOSSHostApiRepresentation *ossApi )
{
PaError result = paNoError;
int idev, odev;
PaUtilHostApiRepresentation *hostApi = &ossApi->inheritedHostApiRep;
const char *idevName = NULL, *odevName = NULL;
assert( stream );
memset( stream, 0, sizeof (PaOssStream) );
stream->isStopped = 1;
PA_ENSURE( PaUtil_InitializeThreading( &stream->threading ) );
if( inputParameters && outputParameters )
{
if( inputParameters->device == outputParameters->device )
stream->sharedDevice = 1;
}
if( inputParameters )
idevName = hostApi->deviceInfos[inputParameters->device]->name;
if( outputParameters )
odevName = hostApi->deviceInfos[outputParameters->device]->name;
PA_ENSURE( OpenDevices( idevName, odevName, &idev, &odev ) );
if( inputParameters )
{
PA_UNLESS( stream->capture = PaUtil_AllocateMemory( sizeof (PaOssStreamComponent) ), paInsufficientMemory );
PA_ENSURE( PaOssStreamComponent_Initialize( stream->capture, inputParameters, callback != NULL, idev, idevName ) );
}
if( outputParameters )
{
PA_UNLESS( stream->playback = PaUtil_AllocateMemory( sizeof (PaOssStreamComponent) ), paInsufficientMemory );
PA_ENSURE( PaOssStreamComponent_Initialize( stream->playback, outputParameters, callback != NULL, odev, odevName ) );
}
if( callback != NULL )
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&ossApi->callbackStreamInterface, callback, userData );
stream->callbackMode = 1;
}
else
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&ossApi->blockingStreamInterface, callback, userData );
}
ENSURE_( sem_init( &stream->semaphore, 0, 0 ), paInternalError );
error:
return result;
}
static void PaOssStream_Terminate( PaOssStream *stream )
{
assert( stream );
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
PaUtil_TerminateThreading( &stream->threading );
if( stream->capture )
PaOssStreamComponent_Terminate( stream->capture );
if( stream->playback )
PaOssStreamComponent_Terminate( stream->playback );
sem_destroy( &stream->semaphore );
PaUtil_FreeMemory( stream );
}
static PaError Pa2OssFormat( PaSampleFormat paFormat, int *ossFormat )
{
switch( paFormat )
{
case paUInt8:
*ossFormat = AFMT_U8;
break;
case paInt8:
*ossFormat = AFMT_S8;
break;
case paInt16:
*ossFormat = AFMT_S16_NE;
break;
#ifdef AFMT_S32_NE
case paInt32:
*ossFormat = AFMT_S32_NE;
break;
#endif
default:
return paInternalError;      
}
return paNoError;
}
static PaError GetAvailableFormats( PaOssStreamComponent *component, PaSampleFormat *availableFormats )
{
PaError result = paNoError;
int mask = 0;
PaSampleFormat frmts = 0;
ENSURE_( ioctl( component->fd, SNDCTL_DSP_GETFMTS, &mask ), paUnanticipatedHostError );
if( mask & AFMT_U8 )
frmts |= paUInt8;
if( mask & AFMT_S8 )
frmts |= paInt8;
if( mask & AFMT_S16_NE )
frmts |= paInt16;
#ifdef AFMT_S32_NE
if( mask & AFMT_S32_NE )
frmts |= paInt32;
#endif
if( frmts == 0 )
result = paSampleFormatNotSupported;
*availableFormats = frmts;
error:
return result;
}
static unsigned int PaOssStreamComponent_FrameSize( PaOssStreamComponent *component )
{
return Pa_GetSampleSize( component->hostFormat ) * component->hostChannelCount;
}
static unsigned long PaOssStreamComponent_BufferSize( PaOssStreamComponent *component )
{
return PaOssStreamComponent_FrameSize( component ) * component->hostFrames * component->numBufs;
}
static PaError PaOssStreamComponent_Configure( PaOssStreamComponent *component, double sampleRate, unsigned long
framesPerBuffer, StreamMode streamMode, PaOssStreamComponent *master )
{
PaError result = paNoError;
int temp, nativeFormat;
int sr = (int)sampleRate;
PaSampleFormat availableFormats = 0, hostFormat = 0;
int chans = component->userChannelCount;
int frgmt;
int numBufs;
int bytesPerBuf;
unsigned long bufSz;
unsigned long fragSz;
audio_buf_info bufInfo;
if( !master )
{
if( framesPerBuffer == paFramesPerBufferUnspecified )
{
fragSz = (unsigned long)(component->latency * sampleRate / 3);
bufSz = fragSz * 4;
}
else
{
fragSz = framesPerBuffer;
bufSz = (unsigned long)(component->latency * sampleRate) + fragSz;  
}
PA_ENSURE( GetAvailableFormats( component, &availableFormats ) );
hostFormat = PaUtil_SelectClosestAvailableFormat( availableFormats, component->userFormat );
numBufs = (int)PA_MAX( bufSz / fragSz, 2 );
bytesPerBuf = PA_MAX( fragSz * Pa_GetSampleSize( hostFormat ) * chans, 16 );
frgmt = (numBufs << 16) + (CalcHigherLogTwo( bytesPerBuf ) & 0xffff);
ENSURE_( ioctl( component->fd, SNDCTL_DSP_SETFRAGMENT, &frgmt ), paUnanticipatedHostError );
PA_ENSURE( Pa2OssFormat( hostFormat, &temp ) );
nativeFormat = temp;
ENSURE_( ioctl( component->fd, SNDCTL_DSP_SETFMT, &temp ), paUnanticipatedHostError );
PA_UNLESS( temp == nativeFormat, paInternalError );
ENSURE_( ioctl( component->fd, SNDCTL_DSP_CHANNELS, &chans ), paSampleFormatNotSupported );    
PA_UNLESS( chans >= component->userChannelCount, paInvalidChannelCount );
ENSURE_( ioctl( component->fd, SNDCTL_DSP_SPEED, &sr ), paInvalidSampleRate );
if( (fabs( sampleRate - sr ) / sampleRate) > 0.01 )
{
PA_DEBUG(("%s: Wanted %f, closest sample rate was %d\n", __FUNCTION__, sampleRate, sr ));
PA_ENSURE( paInvalidSampleRate );
}
ENSURE_( ioctl( component->fd, streamMode == StreamMode_In ? SNDCTL_DSP_GETISPACE : SNDCTL_DSP_GETOSPACE, &bufInfo ),
paUnanticipatedHostError );
component->numBufs = bufInfo.fragstotal;
ENSURE_( ioctl( component->fd, SNDCTL_DSP_GETBLKSIZE, &bytesPerBuf ), paUnanticipatedHostError );
component->hostFrames = bytesPerBuf / Pa_GetSampleSize( hostFormat ) / chans;
component->hostChannelCount = chans;
component->hostFormat = hostFormat;
}
else
{
component->hostFormat = master->hostFormat;
component->hostFrames = master->hostFrames;
component->hostChannelCount = master->hostChannelCount;
component->numBufs = master->numBufs;
}
PA_UNLESS( component->buffer = PaUtil_AllocateMemory( PaOssStreamComponent_BufferSize( component ) ),
paInsufficientMemory );
error:
return result;
}
static PaError PaOssStreamComponent_Read( PaOssStreamComponent *component, unsigned long *frames )
{
PaError result = paNoError;
size_t len = *frames * PaOssStreamComponent_FrameSize( component );
ssize_t bytesRead;
ENSURE_( bytesRead = read( component->fd, component->buffer, len ), paUnanticipatedHostError );
*frames = bytesRead / PaOssStreamComponent_FrameSize( component );
error:
return result;
}
static PaError PaOssStreamComponent_Write( PaOssStreamComponent *component, unsigned long *frames )
{
PaError result = paNoError;
size_t len = *frames * PaOssStreamComponent_FrameSize( component );
ssize_t bytesWritten;
ENSURE_( bytesWritten = write( component->fd, component->buffer, len ), paUnanticipatedHostError );
*frames = bytesWritten / PaOssStreamComponent_FrameSize( component );
error:
return result;
}
static PaError PaOssStream_Configure( PaOssStream *stream, double sampleRate, unsigned long framesPerBuffer,
double *inputLatency, double *outputLatency )
{
PaError result = paNoError;
int duplex = stream->capture && stream->playback;
unsigned long framesPerHostBuffer = 0;
if( duplex && stream->sharedDevice )
ENSURE_( ioctl( stream->capture->fd, SNDCTL_DSP_SETDUPLEX, 0 ), paUnanticipatedHostError );
if( stream->capture )
{
PaOssStreamComponent *component = stream->capture;
PA_ENSURE( PaOssStreamComponent_Configure( component, sampleRate, framesPerBuffer, StreamMode_In,
NULL ) );
assert( component->hostChannelCount > 0 );
assert( component->hostFrames > 0 );
*inputLatency = (component->hostFrames * (component->numBufs - 1)) / sampleRate;
}
if( stream->playback )
{
PaOssStreamComponent *component = stream->playback, *master = stream->sharedDevice ? stream->capture : NULL;
PA_ENSURE( PaOssStreamComponent_Configure( component, sampleRate, framesPerBuffer, StreamMode_Out,
master ) );
assert( component->hostChannelCount > 0 );
assert( component->hostFrames > 0 );
*outputLatency = (component->hostFrames * (component->numBufs - 1)) / sampleRate;
}
if( duplex )
framesPerHostBuffer = PA_MIN( stream->capture->hostFrames, stream->playback->hostFrames );
else if( stream->capture )
framesPerHostBuffer = stream->capture->hostFrames;
else if( stream->playback )
framesPerHostBuffer = stream->playback->hostFrames;
stream->framesPerHostBuffer = framesPerHostBuffer;
stream->pollTimeout = (int) ceil( 1e6 * framesPerHostBuffer / sampleRate );     
stream->sampleRate = stream->streamRepresentation.streamInfo.sampleRate = sampleRate;
error:
return result;
}
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
PaOSSHostApiRepresentation *ossHostApi = (PaOSSHostApiRepresentation*)hostApi;
PaOssStream *stream = NULL;
int inputChannelCount = 0, outputChannelCount = 0;
PaSampleFormat inputSampleFormat = 0, outputSampleFormat = 0, inputHostFormat = 0, outputHostFormat = 0;
const PaDeviceInfo *inputDeviceInfo = 0, *outputDeviceInfo = 0;
int bpInitialized = 0;
double inLatency = 0., outLatency = 0.;
int i = 0;
if( (streamFlags & paPlatformSpecificFlags) != 0 )
return paInvalidFlag;  
if( inputParameters )
{
inputDeviceInfo = hostApi->deviceInfos[inputParameters->device];
PA_ENSURE( ValidateParameters( inputParameters, inputDeviceInfo, StreamMode_In ) );
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
}
if( outputParameters )
{
outputDeviceInfo = hostApi->deviceInfos[outputParameters->device];
PA_ENSURE( ValidateParameters( outputParameters, outputDeviceInfo, StreamMode_Out ) );
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
}
if( inputChannelCount > 0 && outputChannelCount > 0 )
{
if( inputParameters->device == outputParameters->device )
{
if( inputParameters->channelCount != outputParameters->channelCount )
return paInvalidChannelCount;
}
}
if( framesPerBuffer != paFramesPerBufferUnspecified )
{
framesPerBuffer &= INT_MAX;
for (i = 1; framesPerBuffer > i; i <<= 1) ;
framesPerBuffer = i;
}
PA_UNLESS( stream = (PaOssStream*)PaUtil_AllocateMemory( sizeof(PaOssStream) ), paInsufficientMemory );
PA_ENSURE( PaOssStream_Initialize( stream, inputParameters, outputParameters, streamCallback, userData, streamFlags, ossHostApi ) );
PA_ENSURE( PaOssStream_Configure( stream, sampleRate, framesPerBuffer, &inLatency, &outLatency ) );
PaUtil_InitializeCpuLoadMeasurer( &stream->cpuLoadMeasurer, sampleRate );
if( inputParameters )
{
inputHostFormat = stream->capture->hostFormat;
stream->streamRepresentation.streamInfo.inputLatency = inLatency +
PaUtil_GetBufferProcessorInputLatencyFrames( &stream->bufferProcessor ) / sampleRate;
}
if( outputParameters )
{
outputHostFormat = stream->playback->hostFormat;
stream->streamRepresentation.streamInfo.outputLatency = outLatency +
PaUtil_GetBufferProcessorOutputLatencyFrames( &stream->bufferProcessor ) / sampleRate;
}
PA_ENSURE( PaUtil_InitializeBufferProcessor( &stream->bufferProcessor,
inputChannelCount, inputSampleFormat, inputHostFormat, outputChannelCount, outputSampleFormat,
outputHostFormat, sampleRate, streamFlags, framesPerBuffer, stream->framesPerHostBuffer,
paUtilFixedHostBufferSize, streamCallback, userData ) );
bpInitialized = 1;
*s = (PaStream*)stream;
return result;
error:
if( bpInitialized )
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
if( stream )
PaOssStream_Terminate( stream );
return result;
}
static PaError PaOssStream_WaitForFrames( PaOssStream *stream, unsigned long *frames )
{
PaError result = paNoError;
int pollPlayback = 0, pollCapture = 0;
int captureAvail = INT_MAX, playbackAvail = INT_MAX, commonAvail;
audio_buf_info bufInfo;
fd_set readFds, writeFds;
int nfds = 0;
struct timeval selectTimeval = {0, 0};
unsigned long timeout = stream->pollTimeout;     
int captureFd = -1, playbackFd = -1;
assert( stream );
assert( frames );
if( stream->capture )
{
pollCapture = 1;
captureFd = stream->capture->fd;
}
if( stream->playback )
{
pollPlayback = 1;
playbackFd = stream->playback->fd;
}
FD_ZERO( &readFds );
FD_ZERO( &writeFds );
while( pollPlayback || pollCapture )
{
#ifdef PTHREAD_CANCELED
pthread_testcancel();
#else
if( stream->callbackStop || stream->callbackAbort )
{
PA_DEBUG(( "Cancelling PaOssStream_WaitForFrames\n" ));
(*frames) = 0;
return paNoError;
}
#endif
selectTimeval.tv_usec = timeout;
nfds = 0;
if( pollCapture )
{
FD_SET( captureFd, &readFds );
nfds = captureFd + 1;
}
if( pollPlayback )
{
FD_SET( playbackFd, &writeFds );
nfds = PA_MAX( nfds, playbackFd + 1 );
}
ENSURE_( select( nfds, &readFds, &writeFds, NULL, &selectTimeval ), paUnanticipatedHostError );
#ifdef PTHREAD_CANCELED
pthread_testcancel();
#else
if( stream->callbackStop || stream->callbackAbort )
{
PA_DEBUG(( "Cancelling PaOssStream_WaitForFrames\n" ));
(*frames) = 0;
return paNoError;
}
#endif
if( pollCapture )
{
if( FD_ISSET( captureFd, &readFds ) )
{
FD_CLR( captureFd, &readFds );
pollCapture = 0;
}
else if( stream->playback )  
{
}
}
if( pollPlayback )
{
if( FD_ISSET( playbackFd, &writeFds ) )
{
FD_CLR( playbackFd, &writeFds );
pollPlayback = 0;
}
else if( stream->capture )   
{
}
}
}
if( stream->capture )
{
ENSURE_( ioctl( captureFd, SNDCTL_DSP_GETISPACE, &bufInfo ), paUnanticipatedHostError );
captureAvail = bufInfo.fragments * stream->capture->hostFrames;
if( !captureAvail )
PA_DEBUG(( "%s: captureAvail: 0\n", __FUNCTION__ ));
captureAvail = captureAvail == 0 ? INT_MAX : captureAvail;       
}
if( stream->playback )
{
ENSURE_( ioctl( playbackFd, SNDCTL_DSP_GETOSPACE, &bufInfo ), paUnanticipatedHostError );
playbackAvail = bufInfo.fragments * stream->playback->hostFrames;
if( !playbackAvail )
{
PA_DEBUG(( "%s: playbackAvail: 0\n", __FUNCTION__ ));
}
playbackAvail = playbackAvail == 0 ? INT_MAX : playbackAvail;       
}
commonAvail = PA_MIN( captureAvail, playbackAvail );
if( commonAvail == INT_MAX )
commonAvail = 0;
commonAvail -= commonAvail % stream->framesPerHostBuffer;
assert( commonAvail != INT_MAX );
assert( commonAvail >= 0 );
*frames = commonAvail;
error:
return result;
}
static PaError PaOssStream_Prepare( PaOssStream *stream )
{
PaError result = paNoError;
int enableBits = 0;
if( stream->triggered )
return result;
if( stream->playback )
ENSURE_( ioctl( stream->playback->fd, SNDCTL_DSP_SETTRIGGER, &enableBits ), paUnanticipatedHostError );
if( stream->capture )
ENSURE_( ioctl( stream->capture->fd, SNDCTL_DSP_SETTRIGGER, &enableBits ), paUnanticipatedHostError );
if( stream->playback )
{
size_t bufSz = PaOssStreamComponent_BufferSize( stream->playback );
memset( stream->playback->buffer, 0, bufSz );
PA_ENSURE( ModifyBlocking( stream->playback->fd, 0 ) );
while (1)
{
if( write( stream->playback->fd, stream->playback->buffer, bufSz ) < 0 )
break;
}
PA_ENSURE( ModifyBlocking( stream->playback->fd, 1 ) );
}
if( stream->sharedDevice )
{
enableBits = PCM_ENABLE_INPUT | PCM_ENABLE_OUTPUT;
ENSURE_( ioctl( stream->capture->fd, SNDCTL_DSP_SETTRIGGER, &enableBits ), paUnanticipatedHostError );
}
else
{
if( stream->capture )
{
enableBits = PCM_ENABLE_INPUT;
ENSURE_( ioctl( stream->capture->fd, SNDCTL_DSP_SETTRIGGER, &enableBits ), paUnanticipatedHostError );
}
if( stream->playback )
{
enableBits = PCM_ENABLE_OUTPUT;
ENSURE_( ioctl( stream->playback->fd, SNDCTL_DSP_SETTRIGGER, &enableBits ), paUnanticipatedHostError );
}
}
stream->triggered = 1;
error:
return result;
}
static PaError PaOssStream_Stop( PaOssStream *stream, int abort )
{
PaError result = paNoError;
int captureErr = 0, playbackErr = 0;
if( stream->capture )
{
if( (captureErr = ioctl( stream->capture->fd, SNDCTL_DSP_POST, 0 )) < 0 )
{
PA_DEBUG(( "%s: Failed to stop capture device, error: %d\n", __FUNCTION__, captureErr ));
}
}
if( stream->playback && !stream->sharedDevice )
{
if( (playbackErr = ioctl( stream->playback->fd, SNDCTL_DSP_POST, 0 )) < 0 )
{
PA_DEBUG(( "%s: Failed to stop playback device, error: %d\n", __FUNCTION__, playbackErr ));
}
}
if( captureErr || playbackErr )
{
result = paUnanticipatedHostError;
}
return result;
}
static void OnExit( void *data )
{
PaOssStream *stream = (PaOssStream *) data;
assert( data );
PaUtil_ResetCpuLoadMeasurer( &stream->cpuLoadMeasurer );
PaOssStream_Stop( stream, stream->callbackAbort );
PA_DEBUG(( "OnExit: Stoppage\n" ));
if( stream->streamRepresentation.streamFinishedCallback )
stream->streamRepresentation.streamFinishedCallback( stream->streamRepresentation.userData );
stream->callbackAbort = 0;       
stream->isActive = 0;
}
static PaError SetUpBuffers( PaOssStream *stream, unsigned long framesAvail )
{
PaError result = paNoError;
if( stream->capture )
{
PaUtil_SetInterleavedInputChannels( &stream->bufferProcessor, 0, stream->capture->buffer,
stream->capture->hostChannelCount );
PaUtil_SetInputFrameCount( &stream->bufferProcessor, framesAvail );
}
if( stream->playback )
{
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor, 0, stream->playback->buffer,
stream->playback->hostChannelCount );
PaUtil_SetOutputFrameCount( &stream->bufferProcessor, framesAvail );
}
return result;
}
static void *PaOSS_AudioThreadProc( void *userData )
{
PaError result = paNoError;
PaOssStream *stream = (PaOssStream*)userData;
unsigned long framesAvail = 0, framesProcessed = 0;
int callbackResult = paContinue;
int triggered = stream->triggered;   
int initiateProcessing = triggered;     
PaStreamCallbackFlags cbFlags = 0;   
PaStreamCallbackTimeInfo timeInfo = {0,0,0};  
assert( stream );
pthread_cleanup_push( &OnExit, stream );     
PA_ENSURE( PaOssStream_Prepare( stream ) );
if( initiateProcessing )
{
if( stream->capture )
ModifyBlocking( stream->capture->fd, 1 );
if( stream->playback )
ModifyBlocking( stream->playback->fd, 1 );
}
while( 1 )
{
#ifdef PTHREAD_CANCELED
pthread_testcancel();
#else
if( stream->callbackAbort )  
{
PA_DEBUG(( "Aborting callback thread\n" ));
break;
}
#endif
if( stream->callbackStop && callbackResult == paContinue )
{
PA_DEBUG(( "Setting callbackResult to paComplete\n" ));
callbackResult = paComplete;
}
if( !initiateProcessing )
{
PA_ENSURE( PaOssStream_WaitForFrames( stream, &framesAvail ) );
assert( framesAvail % stream->framesPerHostBuffer == 0 );
}
else
{
framesAvail = stream->framesPerHostBuffer;
}
while( framesAvail > 0 )
{
unsigned long frames = framesAvail;
#ifdef PTHREAD_CANCELED
pthread_testcancel();
#else
if( stream->callbackStop )
{
PA_DEBUG(( "Setting callbackResult to paComplete\n" ));
callbackResult = paComplete;
}
if( stream->callbackAbort )  
{
PA_DEBUG(( "Aborting callback thread\n" ));
break;
}
#endif
PaUtil_BeginCpuLoadMeasurement( &stream->cpuLoadMeasurer );
if ( stream->capture )
{
PA_ENSURE( PaOssStreamComponent_Read( stream->capture, &frames ) );
if( frames < framesAvail )
{
PA_DEBUG(( "Read %lu less frames than requested\n", framesAvail - frames ));
framesAvail = frames;
}
}
#if ( SOUND_VERSION >= 0x030904 )
#endif
PaUtil_BeginBufferProcessing( &stream->bufferProcessor, &timeInfo,
cbFlags );
cbFlags = 0;
PA_ENSURE( SetUpBuffers( stream, framesAvail ) );
framesProcessed = PaUtil_EndBufferProcessing( &stream->bufferProcessor,
&callbackResult );
assert( framesProcessed == framesAvail );
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, framesProcessed );
if ( stream->playback )
{
frames = framesAvail;
PA_ENSURE( PaOssStreamComponent_Write( stream->playback, &frames ) );
if( frames < framesAvail )
{
PA_DEBUG(( "Wrote %lu less frames than requested\n", framesAvail - frames ));
}
}
framesAvail -= framesProcessed;
stream->framesProcessed += framesProcessed;
if( callbackResult != paContinue )
break;
}
if( initiateProcessing || !triggered )
{
if( stream->capture )
PA_ENSURE( ModifyBlocking( stream->capture->fd, 0 ) );
if( stream->playback && !stream->sharedDevice )
PA_ENSURE( ModifyBlocking( stream->playback->fd, 0 ) );
initiateProcessing = 0;
sem_post( &stream->semaphore );
}
if( callbackResult != paContinue )
{
stream->callbackAbort = callbackResult == paAbort;
if( stream->callbackAbort || PaUtil_IsBufferProcessorOutputEmpty( &stream->bufferProcessor ) )
break;
}
}
pthread_cleanup_pop( 1 );
error:
pthread_exit( NULL );
}
static PaError CloseStream( PaStream* s )
{
PaError result = paNoError;
PaOssStream *stream = (PaOssStream*)s;
assert( stream );
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
PaOssStream_Terminate( stream );
return result;
}
static PaError StartStream( PaStream *s )
{
PaError result = paNoError;
PaOssStream *stream = (PaOssStream*)s;
stream->isActive = 1;
stream->isStopped = 0;
stream->lastPosPtr = 0;
stream->lastStreamBytes = 0;
stream->framesProcessed = 0;
if( stream->bufferProcessor.streamCallback )
{
PA_ENSURE( PaUtil_StartThreading( &stream->threading, &PaOSS_AudioThreadProc, stream ) );
sem_wait( &stream->semaphore );
}
else
PA_ENSURE( PaOssStream_Prepare( stream ) );
error:
return result;
}
static PaError RealStop( PaOssStream *stream, int abort )
{
PaError result = paNoError;
if( stream->callbackMode )
{
if( abort )
stream->callbackAbort = 1;
else
stream->callbackStop = 1;
PA_ENSURE( PaUtil_CancelThreading( &stream->threading, !abort, NULL ) );
stream->callbackStop = stream->callbackAbort = 0;
}
else
PA_ENSURE( PaOssStream_Stop( stream, abort ) );
stream->isStopped = 1;
error:
return result;
}
static PaError StopStream( PaStream *s )
{
return RealStop( (PaOssStream *)s, 0 );
}
static PaError AbortStream( PaStream *s )
{
return RealStop( (PaOssStream *)s, 1 );
}
static PaError IsStreamStopped( PaStream *s )
{
PaOssStream *stream = (PaOssStream*)s;
return (stream->isStopped);
}
static PaError IsStreamActive( PaStream *s )
{
PaOssStream *stream = (PaOssStream*)s;
return (stream->isActive);
}
static PaTime GetStreamTime( PaStream *s )
{
PaOssStream *stream = (PaOssStream*)s;
count_info info;
int delta;
if( stream->playback ) {
if( ioctl( stream->playback->fd, SNDCTL_DSP_GETOPTR, &info) == 0 ) {
delta = ( info.bytes - stream->lastPosPtr )  ;
return (float)(stream->lastStreamBytes + delta) / PaOssStreamComponent_FrameSize( stream->playback ) / stream->sampleRate;
}
}
else {
if (ioctl( stream->capture->fd, SNDCTL_DSP_GETIPTR, &info) == 0) {
delta = (info.bytes - stream->lastPosPtr)  ;
return (float)(stream->lastStreamBytes + delta) / PaOssStreamComponent_FrameSize( stream->capture ) / stream->sampleRate;
}
}
return stream->framesProcessed / stream->sampleRate;
}
static double GetStreamCpuLoad( PaStream* s )
{
PaOssStream *stream = (PaOssStream*)s;
return PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer );
}
static PaError ReadStream( PaStream* s,
void *buffer,
unsigned long frames )
{
PaError result = paNoError;
PaOssStream *stream = (PaOssStream*)s;
int bytesRequested, bytesRead;
unsigned long framesRequested;
void *userBuffer;
if( stream->bufferProcessor.userInputIsInterleaved )
userBuffer = buffer;
else  
{
userBuffer = stream->capture->userBuffers;
memcpy( (void *)userBuffer, buffer, sizeof (void *) * stream->capture->userChannelCount );
}
while( frames )
{
framesRequested = PA_MIN( frames, stream->capture->hostFrames );
bytesRequested = framesRequested * PaOssStreamComponent_FrameSize( stream->capture );
ENSURE_( (bytesRead = read( stream->capture->fd, stream->capture->buffer, bytesRequested )),
paUnanticipatedHostError );
if ( bytesRequested != bytesRead )
{
PA_DEBUG(( "Requested %d bytes, read %d\n", bytesRequested, bytesRead ));
return paUnanticipatedHostError;
}
PaUtil_SetInputFrameCount( &stream->bufferProcessor, stream->capture->hostFrames );
PaUtil_SetInterleavedInputChannels( &stream->bufferProcessor, 0, stream->capture->buffer, stream->capture->hostChannelCount );
PaUtil_CopyInput( &stream->bufferProcessor, &userBuffer, framesRequested );
frames -= framesRequested;
}
error:
return result;
}
static PaError WriteStream( PaStream *s, const void *buffer, unsigned long frames )
{
PaError result = paNoError;
PaOssStream *stream = (PaOssStream*)s;
int bytesRequested, bytesWritten;
unsigned long framesConverted;
const void *userBuffer;
if( stream->bufferProcessor.userOutputIsInterleaved )
userBuffer = buffer;
else
{
userBuffer = stream->playback->userBuffers;
memcpy( (void *)userBuffer, buffer, sizeof (void *) * stream->playback->userChannelCount );
}
while( frames )
{
PaUtil_SetOutputFrameCount( &stream->bufferProcessor, stream->playback->hostFrames );
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor, 0, stream->playback->buffer, stream->playback->hostChannelCount );
framesConverted = PaUtil_CopyOutput( &stream->bufferProcessor, &userBuffer, frames );
frames -= framesConverted;
bytesRequested = framesConverted * PaOssStreamComponent_FrameSize( stream->playback );
ENSURE_( (bytesWritten = write( stream->playback->fd, stream->playback->buffer, bytesRequested )),
paUnanticipatedHostError );
if ( bytesRequested != bytesWritten )
{
PA_DEBUG(( "Requested %d bytes, wrote %d\n", bytesRequested, bytesWritten ));
return paUnanticipatedHostError;
}
}
error:
return result;
}
static signed long GetStreamReadAvailable( PaStream* s )
{
PaError result = paNoError;
PaOssStream *stream = (PaOssStream*)s;
audio_buf_info info;
ENSURE_( ioctl( stream->capture->fd, SNDCTL_DSP_GETISPACE, &info ), paUnanticipatedHostError );
return info.fragments * stream->capture->hostFrames;
error:
return result;
}
static signed long GetStreamWriteAvailable( PaStream* s )
{
PaError result = paNoError;
PaOssStream *stream = (PaOssStream*)s;
int delay = 0;
#ifdef SNDCTL_DSP_GETODELAY
ENSURE_( ioctl( stream->playback->fd, SNDCTL_DSP_GETODELAY, &delay ), paUnanticipatedHostError );
#endif
return (PaOssStreamComponent_BufferSize( stream->playback ) - delay) / PaOssStreamComponent_FrameSize( stream->playback );
#ifdef SNDCTL_DSP_GETODELAY
error:
return result;
#endif
}

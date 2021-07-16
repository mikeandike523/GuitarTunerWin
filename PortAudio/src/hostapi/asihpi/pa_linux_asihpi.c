 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>           
#include <pthread.h>          
#include <assert.h>           
#include <math.h>             
#include <asihpi/hpi.h>       
#include "portaudio.h"        
#include "pa_util.h"          
#include "pa_unix_util.h"     
#include "pa_allocation.h"    
#include "pa_hostapi.h"       
#include "pa_stream.h"        
#include "pa_cpuload.h"       
#include "pa_process.h"       
#include "pa_converters.h"    
#include "pa_debugprint.h"
#define PA_ENSURE_(expr) \
do { \
PaError paError = (expr); \
if( UNLIKELY( paError < paNoError ) ) \
{ \
PA_DEBUG(( "Expression '" #expr "' failed in '" __FILE__ "', line: " STRINGIZE( __LINE__ ) "\n" )); \
result = paError; \
goto error; \
} \
} while (0);
#define PA_UNLESS_(expr, paError) \
do { \
if( UNLIKELY( (expr) == 0 ) ) \
{ \
PA_DEBUG(( "Expression '" #expr "' failed in '" __FILE__ "', line: " STRINGIZE( __LINE__ ) "\n" )); \
result = (paError); \
goto error; \
} \
} while( 0 );
#define PA_ASIHPI_UNLESS_(expr, paError) \
do { \
hpi_err_t hpiError = (expr); \
\
if( UNLIKELY( hpiError ) ) \
{ \
char szError[256]; \
HPI_GetErrorText( hpiError, szError ); \
PA_DEBUG(( "HPI error %d occurred: %s\n", hpiError, szError )); \
\
PA_DEBUG(( "Expression '" #expr "' failed in '" __FILE__ "', line: " STRINGIZE( __LINE__ ) "\n" )); \
if( (paError) == paUnanticipatedHostError ) \
{ \
PA_DEBUG(( "Host error description: %s\n", szError )); \
\
if( pthread_equal( pthread_self(), paUnixMainThread ) ) \
{ \
PaUtil_SetLastHostErrorInfo( paInDevelopment, hpiError, szError ); \
} \
} \
\
\
if( (paError) < 0 ) \
{ \
result = (paError); \
goto error; \
} \
} \
} while( 0 );
#define PA_ASIHPI_REPORT_ERROR_(hpiErrorCode) \
do { \
char szError[256]; \
HPI_GetErrorText( hpiError, szError ); \
PA_DEBUG(( "HPI error %d occurred: %s\n", hpiError, szError )); \
\
if( pthread_equal( pthread_self(), paUnixMainThread ) ) \
{ \
PaUtil_SetLastHostErrorInfo( paInDevelopment, (hpiErrorCode), szError ); \
} \
} while( 0 );
#define PA_ASIHPI_AVAILABLE_FORMATS_ (paFloat32 | paInt32 | paInt24 | paInt16 | paUInt8)
#define PA_ASIHPI_USE_BBM_ 1
#define PA_ASIHPI_MIN_FRAMES_ 1152
#define PA_ASIHPI_MIN_POLLING_INTERVAL_ 10
typedef struct PaAsiHpiHostApiRepresentation
{
PaUtilHostApiRepresentation baseHostApiRep;
PaUtilStreamInterface callbackStreamInterface;
PaUtilStreamInterface blockingStreamInterface;
PaUtilAllocationGroup *allocations;
PaHostApiIndex hostApiIndex;
}
PaAsiHpiHostApiRepresentation;
typedef struct PaAsiHpiDeviceInfo
{
PaDeviceInfo baseDeviceInfo;
uint16_t adapterIndex;
uint16_t adapterType;
uint16_t adapterVersion;
uint32_t adapterSerialNumber;
uint16_t streamIndex;
uint16_t streamIsOutput;
}
PaAsiHpiDeviceInfo;
typedef enum PaAsiHpiStreamState
{
paAsiHpiStoppedState=0,
paAsiHpiActiveState=1,
paAsiHpiCallbackFinishedState=2
}
PaAsiHpiStreamState;
typedef struct PaAsiHpiStreamComponent
{
PaAsiHpiDeviceInfo *hpiDevice;
hpi_handle_t hpiStream;
struct hpi_format hpiFormat;
uint32_t bytesPerFrame;
uint32_t hardwareBufferSize;
uint32_t hostBufferSize;
uint32_t outputBufferCap;
uint8_t *tempBuffer;
uint32_t tempBufferSize;
}
PaAsiHpiStreamComponent;
typedef struct PaAsiHpiStream
{
PaUtilStreamRepresentation baseStreamRep;
PaUtilCpuLoadMeasurer cpuLoadMeasurer;
PaUtilBufferProcessor bufferProcessor;
PaUtilAllocationGroup *allocations;
PaAsiHpiStreamComponent *input, *output;
uint32_t pollingInterval;
int callbackMode;
unsigned long maxFramesPerHostBuffer;
int neverDropInput;
void **blockingUserBufferCopy;
PaUnixThread thread;
volatile sig_atomic_t state;
volatile sig_atomic_t callbackAbort;
volatile sig_atomic_t callbackFinished;
}
PaAsiHpiStream;
typedef struct PaAsiHpiStreamInfo
{
uint16_t state;
uint32_t bufferSize;
uint32_t dataSize;
uint32_t frameCounter;
uint32_t auxDataSize;
uint32_t totalBufferedData;
uint32_t availableFrames;
int overflow;
int underflow;
}
PaAsiHpiStreamInfo;
#ifdef __cplusplus
extern "C"
{
#endif  
PaError PaAsiHpi_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
#ifdef __cplusplus
}
#endif  
static void Terminate( struct PaUtilHostApiRepresentation *hostApi );
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate );
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream **s,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback *streamCallback,
void *userData );
static PaError CloseStream( PaStream *s );
static PaError StartStream( PaStream *s );
static PaError StopStream( PaStream *s );
static PaError AbortStream( PaStream *s );
static PaError IsStreamStopped( PaStream *s );
static PaError IsStreamActive( PaStream *s );
static PaTime GetStreamTime( PaStream *s );
static double GetStreamCpuLoad( PaStream *s );
static PaError ReadStream( PaStream *s, void *buffer, unsigned long frames );
static PaError WriteStream( PaStream *s, const void *buffer, unsigned long frames );
static signed long GetStreamReadAvailable( PaStream *s );
static signed long GetStreamWriteAvailable( PaStream *s );
static void *CallbackThreadFunc( void *userData );
static PaError PaAsiHpi_BuildDeviceList( PaAsiHpiHostApiRepresentation *hpiHostApi );
static uint16_t PaAsiHpi_PaToHpiFormat( PaSampleFormat paFormat );
static PaSampleFormat PaAsiHpi_HpiToPaFormat( uint16_t hpiFormat );
static PaError PaAsiHpi_CreateFormat( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *parameters, double sampleRate,
PaAsiHpiDeviceInfo **hpiDevice, struct hpi_format *hpiFormat );
static PaError PaAsiHpi_OpenInput( struct PaUtilHostApiRepresentation *hostApi,
const PaAsiHpiDeviceInfo *hpiDevice, const struct hpi_format *hpiFormat,
hpi_handle_t *hpiStream );
static PaError PaAsiHpi_OpenOutput( struct PaUtilHostApiRepresentation *hostApi,
const PaAsiHpiDeviceInfo *hpiDevice, const struct hpi_format *hpiFormat,
hpi_handle_t *hpiStream );
static PaError PaAsiHpi_GetStreamInfo( PaAsiHpiStreamComponent *streamComp, PaAsiHpiStreamInfo *info );
static void PaAsiHpi_StreamComponentDump( PaAsiHpiStreamComponent *streamComp, PaAsiHpiStream *stream );
static void PaAsiHpi_StreamDump( PaAsiHpiStream *stream );
static PaError PaAsiHpi_SetupBuffers( PaAsiHpiStreamComponent *streamComp, uint32_t pollingInterval,
unsigned long framesPerPaHostBuffer, PaTime suggestedLatency );
static PaError PaAsiHpi_PrimeOutputWithSilence( PaAsiHpiStream *stream );
static PaError PaAsiHpi_StartStream( PaAsiHpiStream *stream, int outputPrimed );
static PaError PaAsiHpi_StopStream( PaAsiHpiStream *stream, int abort );
static PaError PaAsiHpi_ExplicitStop( PaAsiHpiStream *stream, int abort );
static void PaAsiHpi_OnThreadExit( void *userData );
static PaError PaAsiHpi_WaitForFrames( PaAsiHpiStream *stream, unsigned long *framesAvail,
PaStreamCallbackFlags *cbFlags );
static void PaAsiHpi_CalculateTimeInfo( PaAsiHpiStream *stream, PaStreamCallbackTimeInfo *timeInfo );
static PaError PaAsiHpi_BeginProcessing( PaAsiHpiStream* stream, unsigned long* numFrames,
PaStreamCallbackFlags *cbFlags );
static PaError PaAsiHpi_EndProcessing( PaAsiHpiStream *stream, unsigned long numFrames,
PaStreamCallbackFlags *cbFlags );
static PaError PaAsiHpi_BuildDeviceList( PaAsiHpiHostApiRepresentation *hpiHostApi )
{
PaError result = paNoError;
PaUtilHostApiRepresentation *hostApi = &hpiHostApi->baseHostApiRep;
PaHostApiInfo *baseApiInfo = &hostApi->info;
PaAsiHpiDeviceInfo *hpiDeviceList;
int numAdapters;
hpi_err_t hpiError = 0;
int i, j, deviceCount = 0, deviceIndex = 0;
assert( hpiHostApi );
PA_ASIHPI_UNLESS_( HPI_SubSysGetNumAdapters( NULL, &numAdapters), paNoError );
for( i=0; i < numAdapters; ++i )
{
uint16_t inStreams, outStreams;
uint16_t version;
uint32_t serial;
uint16_t type;
uint32_t idx;
hpiError = HPI_SubSysGetAdapter(NULL, i, &idx, &type);
if (hpiError)
continue;
hpiError = HPI_AdapterOpen( NULL, idx );
if( hpiError )
{
PA_ASIHPI_REPORT_ERROR_( hpiError );
continue;
}
hpiError = HPI_AdapterGetInfo( NULL, idx, &outStreams, &inStreams,
&version, &serial, &type );
if( hpiError )
{
PA_ASIHPI_REPORT_ERROR_( hpiError );
continue;
}
else
{
if( (baseApiInfo->defaultInputDevice == paNoDevice) && (inStreams > 0) )
baseApiInfo->defaultInputDevice = deviceCount;
deviceCount += inStreams;
if( (baseApiInfo->defaultOutputDevice == paNoDevice) && (outStreams > 0) )
baseApiInfo->defaultOutputDevice = deviceCount;
deviceCount += outStreams;
}
}
if( deviceCount > 0 )
{
PA_UNLESS_( hostApi->deviceInfos = (PaDeviceInfo**) PaUtil_GroupAllocateMemory(
hpiHostApi->allocations, sizeof(PaDeviceInfo*) * deviceCount ),
paInsufficientMemory );
PA_UNLESS_( hpiDeviceList = (PaAsiHpiDeviceInfo*) PaUtil_GroupAllocateMemory(
hpiHostApi->allocations, sizeof(PaAsiHpiDeviceInfo) * deviceCount ),
paInsufficientMemory );
for( i=0; i < numAdapters; ++i )
{
uint16_t inStreams, outStreams;
uint16_t version;
uint32_t serial;
uint16_t type;
uint32_t idx;
hpiError = HPI_SubSysGetAdapter( NULL, i, &idx, &type );
if (hpiError)
continue;
hpiError = HPI_AdapterGetInfo( NULL, idx,
&outStreams, &inStreams, &version, &serial, &type );
if( hpiError )
{
PA_ASIHPI_REPORT_ERROR_( hpiError );
continue;
}
else
{
PA_DEBUG(( "Found HPI Adapter ID=%4X Idx=%d #In=%d #Out=%d S/N=%d HWver=%c%d DSPver=%03d\n",
type, idx, inStreams, outStreams, serial,
((version>>3)&0xf)+'A',                   
version&0x7,                              
((version>>13)*100)+((version>>7)&0x3f)   
));
}
for( j=0; j < inStreams; ++j )
{
PaAsiHpiDeviceInfo *hpiDevice = &hpiDeviceList[deviceIndex];
PaDeviceInfo *baseDeviceInfo = &hpiDevice->baseDeviceInfo;
char srcName[72];
char *deviceName;
memset( hpiDevice, 0, sizeof(PaAsiHpiDeviceInfo) );
hpiDevice->adapterIndex = idx;
hpiDevice->adapterType = type;
hpiDevice->adapterVersion = version;
hpiDevice->adapterSerialNumber = serial;
hpiDevice->streamIndex = j;
hpiDevice->streamIsOutput = 0;
baseDeviceInfo->structVersion = 2;
sprintf( srcName,
"Adapter %d (%4X) - Input Stream %d", i+1, type, j+1 );
PA_UNLESS_( deviceName = (char *) PaUtil_GroupAllocateMemory(
hpiHostApi->allocations, strlen(srcName) + 1 ), paInsufficientMemory );
strcpy( deviceName, srcName );
baseDeviceInfo->name = deviceName;
baseDeviceInfo->hostApi = hpiHostApi->hostApiIndex;
baseDeviceInfo->maxInputChannels = HPI_MAX_CHANNELS;
baseDeviceInfo->maxOutputChannels = 0;
baseDeviceInfo->defaultLowInputLatency = 0.01;
baseDeviceInfo->defaultLowOutputLatency = -1.0;
baseDeviceInfo->defaultHighInputLatency = 0.2;
baseDeviceInfo->defaultHighOutputLatency = -1.0;
baseDeviceInfo->defaultSampleRate = 44100;
hostApi->deviceInfos[deviceIndex++] = (PaDeviceInfo *) hpiDevice;
}
for( j=0; j < outStreams; ++j )
{
PaAsiHpiDeviceInfo *hpiDevice = &hpiDeviceList[deviceIndex];
PaDeviceInfo *baseDeviceInfo = &hpiDevice->baseDeviceInfo;
char srcName[72];
char *deviceName;
memset( hpiDevice, 0, sizeof(PaAsiHpiDeviceInfo) );
hpiDevice->adapterIndex = idx;
hpiDevice->adapterType = type;
hpiDevice->adapterVersion = version;
hpiDevice->adapterSerialNumber = serial;
hpiDevice->streamIndex = j;
hpiDevice->streamIsOutput = 1;
baseDeviceInfo->structVersion = 2;
sprintf( srcName,
"Adapter %d (%4X) - Output Stream %d", i+1, type, j+1 );
PA_UNLESS_( deviceName = (char *) PaUtil_GroupAllocateMemory(
hpiHostApi->allocations, strlen(srcName) + 1 ), paInsufficientMemory );
strcpy( deviceName, srcName );
baseDeviceInfo->name = deviceName;
baseDeviceInfo->hostApi = hpiHostApi->hostApiIndex;
baseDeviceInfo->maxInputChannels = 0;
baseDeviceInfo->maxOutputChannels = HPI_MAX_CHANNELS;
baseDeviceInfo->defaultLowInputLatency = -1.0;
baseDeviceInfo->defaultLowOutputLatency = 0.01;
baseDeviceInfo->defaultHighInputLatency = -1.0;
baseDeviceInfo->defaultHighOutputLatency = 0.2;
baseDeviceInfo->defaultSampleRate = 44100;
hostApi->deviceInfos[deviceIndex++] = (PaDeviceInfo *) hpiDevice;
}
}
}
baseApiInfo->deviceCount = deviceIndex;
error:
return result;
}
PaError PaAsiHpi_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result = paNoError;
PaAsiHpiHostApiRepresentation *hpiHostApi = NULL;
PaHostApiInfo *baseApiInfo;
if (!HPI_SubSysCreate())
{
PA_DEBUG(( "Could not open HPI interface\n" ));
*hostApi = NULL;
return paNoError;
}
else
{
uint32_t hpiVersion;
PA_ASIHPI_UNLESS_( HPI_SubSysGetVersionEx( NULL, &hpiVersion ), paUnanticipatedHostError );
PA_DEBUG(( "HPI interface v%d.%02d.%02d\n",
hpiVersion >> 16,  (hpiVersion >> 8) & 0x0F, (hpiVersion & 0x0F) ));
}
PA_UNLESS_( hpiHostApi = (PaAsiHpiHostApiRepresentation*) PaUtil_AllocateMemory(
sizeof(PaAsiHpiHostApiRepresentation) ), paInsufficientMemory );
PA_UNLESS_( hpiHostApi->allocations = PaUtil_CreateAllocationGroup(), paInsufficientMemory );
hpiHostApi->hostApiIndex = hostApiIndex;
*hostApi = &hpiHostApi->baseHostApiRep;
baseApiInfo = &((*hostApi)->info);
baseApiInfo->structVersion = 1;
baseApiInfo->type = paAudioScienceHPI;
baseApiInfo->name = "AudioScience HPI";
baseApiInfo->deviceCount = 0;
baseApiInfo->defaultInputDevice = paNoDevice;
baseApiInfo->defaultOutputDevice = paNoDevice;
PA_ENSURE_( PaAsiHpi_BuildDeviceList( hpiHostApi ) );
(*hostApi)->Terminate = Terminate;
(*hostApi)->OpenStream = OpenStream;
(*hostApi)->IsFormatSupported = IsFormatSupported;
PaUtil_InitializeStreamInterface( &hpiHostApi->callbackStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable, PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &hpiHostApi->blockingStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream, GetStreamReadAvailable, GetStreamWriteAvailable );
PA_ENSURE_( PaUnixThreading_Initialize() );
return result;
error:
if (hpiHostApi)
PaUtil_FreeMemory( hpiHostApi );
return result;
}
static void Terminate( struct PaUtilHostApiRepresentation *hostApi )
{
PaAsiHpiHostApiRepresentation *hpiHostApi = (PaAsiHpiHostApiRepresentation*)hostApi;
int i;
PaError result = paNoError;
if( hpiHostApi )
{
uint16_t lastAdapterIndex = HPI_MAX_ADAPTERS;
for( i=0; i < hostApi->info.deviceCount; ++i )
{
PaAsiHpiDeviceInfo *hpiDevice = (PaAsiHpiDeviceInfo *) hostApi->deviceInfos[ i ];
if( hpiDevice->adapterIndex != lastAdapterIndex )
{
PA_ASIHPI_UNLESS_( HPI_AdapterClose( NULL,
hpiDevice->adapterIndex ), paNoError );
lastAdapterIndex = hpiDevice->adapterIndex;
}
}
HPI_SubSysFree( NULL );
if( hpiHostApi->allocations )
{
PaUtil_FreeAllAllocations( hpiHostApi->allocations );
PaUtil_DestroyAllocationGroup( hpiHostApi->allocations );
}
PaUtil_FreeMemory( hpiHostApi );
}
error:
return;
}
static uint16_t PaAsiHpi_PaToHpiFormat( PaSampleFormat paFormat )
{
switch( paFormat & ~paNonInterleaved )
{
case paFloat32:
return HPI_FORMAT_PCM32_FLOAT;
case paInt32:
return HPI_FORMAT_PCM32_SIGNED;
case paInt24:
return HPI_FORMAT_PCM24_SIGNED;
case paInt16:
return HPI_FORMAT_PCM16_SIGNED;
case paUInt8:
return HPI_FORMAT_PCM8_UNSIGNED;
case paInt8:
default:
return HPI_FORMAT_PCM16_SIGNED;
}
}
static PaSampleFormat PaAsiHpi_HpiToPaFormat( uint16_t hpiFormat )
{
switch( hpiFormat )
{
case HPI_FORMAT_PCM32_FLOAT:
return paFloat32;
case HPI_FORMAT_PCM32_SIGNED:
return paInt32;
case HPI_FORMAT_PCM24_SIGNED:
return paInt24;
case HPI_FORMAT_PCM16_SIGNED:
return paInt16;
case HPI_FORMAT_PCM8_UNSIGNED:
return paUInt8;
default:
return paCustomFormat;
}
}
static PaError PaAsiHpi_CreateFormat( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *parameters, double sampleRate,
PaAsiHpiDeviceInfo **hpiDevice, struct hpi_format *hpiFormat )
{
int maxChannelCount = 0;
PaSampleFormat hostSampleFormat = 0;
hpi_err_t hpiError = 0;
if( parameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
else
{
assert( parameters->device < hostApi->info.deviceCount );
*hpiDevice = (PaAsiHpiDeviceInfo*) hostApi->deviceInfos[ parameters->device ];
}
if( parameters->hostApiSpecificStreamInfo )
return paIncompatibleHostApiSpecificStreamInfo;
if( (*hpiDevice)->streamIsOutput )
{
maxChannelCount = (*hpiDevice)->baseDeviceInfo.maxOutputChannels;
}
else
{
maxChannelCount = (*hpiDevice)->baseDeviceInfo.maxInputChannels;
}
if( (maxChannelCount == 0) || (parameters->channelCount > maxChannelCount) )
return paInvalidChannelCount;
if( parameters->sampleFormat & paCustomFormat )
return paSampleFormatNotSupported;
hostSampleFormat = PaUtil_SelectClosestAvailableFormat(PA_ASIHPI_AVAILABLE_FORMATS_,
parameters->sampleFormat );
hpiError = HPI_FormatCreate( hpiFormat, (uint16_t)parameters->channelCount,
PaAsiHpi_PaToHpiFormat( hostSampleFormat ),
(uint32_t)sampleRate, 0, 0 );
if( hpiError )
{
PA_ASIHPI_REPORT_ERROR_( hpiError );
switch( hpiError )
{
case HPI_ERROR_INVALID_FORMAT:
return paSampleFormatNotSupported;
case HPI_ERROR_INVALID_SAMPLERATE:
case HPI_ERROR_INCOMPATIBLE_SAMPLERATE:
return paInvalidSampleRate;
case HPI_ERROR_INVALID_CHANNELS:
return paInvalidChannelCount;
}
}
return paNoError;
}
static PaError PaAsiHpi_OpenInput( struct PaUtilHostApiRepresentation *hostApi,
const PaAsiHpiDeviceInfo *hpiDevice, const struct hpi_format *hpiFormat,
hpi_handle_t *hpiStream )
{
PaAsiHpiHostApiRepresentation *hpiHostApi = (PaAsiHpiHostApiRepresentation*)hostApi;
PaError result = paNoError;
hpi_err_t hpiError = 0;
PA_UNLESS_( !hpiDevice->streamIsOutput, paInvalidChannelCount );
PA_ASIHPI_UNLESS_( HPI_InStreamOpen( NULL, hpiDevice->adapterIndex,
hpiDevice->streamIndex, hpiStream ), paDeviceUnavailable );
hpiError = HPI_InStreamSetFormat( NULL, *hpiStream, (struct hpi_format*)hpiFormat );
if( hpiError )
{
PA_ASIHPI_REPORT_ERROR_( hpiError );
PA_ASIHPI_UNLESS_( HPI_InStreamClose( NULL, *hpiStream ), paNoError );
switch( hpiError )
{
case HPI_ERROR_INVALID_FORMAT:
return paSampleFormatNotSupported;
case HPI_ERROR_INVALID_SAMPLERATE:
case HPI_ERROR_INCOMPATIBLE_SAMPLERATE:
return paInvalidSampleRate;
case HPI_ERROR_INVALID_CHANNELS:
return paInvalidChannelCount;
default:
return paInvalidDevice;
}
}
error:
return result;
}
static PaError PaAsiHpi_OpenOutput( struct PaUtilHostApiRepresentation *hostApi,
const PaAsiHpiDeviceInfo *hpiDevice, const struct hpi_format *hpiFormat,
hpi_handle_t *hpiStream )
{
PaAsiHpiHostApiRepresentation *hpiHostApi = (PaAsiHpiHostApiRepresentation*)hostApi;
PaError result = paNoError;
hpi_err_t hpiError = 0;
PA_UNLESS_( hpiDevice->streamIsOutput, paInvalidChannelCount );
PA_ASIHPI_UNLESS_( HPI_OutStreamOpen( NULL, hpiDevice->adapterIndex,
hpiDevice->streamIndex, hpiStream ), paDeviceUnavailable );
hpiError = HPI_OutStreamQueryFormat( NULL, *hpiStream, (struct hpi_format*)hpiFormat );
if( hpiError )
{
PA_ASIHPI_REPORT_ERROR_( hpiError );
PA_ASIHPI_UNLESS_( HPI_OutStreamClose( NULL, *hpiStream ), paNoError );
switch( hpiError )
{
case HPI_ERROR_INVALID_FORMAT:
return paSampleFormatNotSupported;
case HPI_ERROR_INVALID_SAMPLERATE:
case HPI_ERROR_INCOMPATIBLE_SAMPLERATE:
return paInvalidSampleRate;
case HPI_ERROR_INVALID_CHANNELS:
return paInvalidChannelCount;
default:
return paInvalidDevice;
}
}
error:
return result;
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate )
{
PaError result = paFormatIsSupported;
PaAsiHpiHostApiRepresentation *hpiHostApi = (PaAsiHpiHostApiRepresentation*)hostApi;
PaAsiHpiDeviceInfo *hpiDevice = NULL;
struct hpi_format hpiFormat;
if( inputParameters )
{
hpi_handle_t hpiStream;
PA_DEBUG(( "%s: Checking input params: dev=%d, sr=%d, chans=%d, fmt=%d\n",
__FUNCTION__, inputParameters->device, (int)sampleRate,
inputParameters->channelCount, inputParameters->sampleFormat ));
PA_ENSURE_( PaAsiHpi_CreateFormat( hostApi, inputParameters, sampleRate,
&hpiDevice, &hpiFormat ) );
PA_ENSURE_( PaAsiHpi_OpenInput( hostApi, hpiDevice, &hpiFormat, &hpiStream ) );
PA_ASIHPI_UNLESS_( HPI_InStreamClose( NULL, hpiStream ), paNoError );
}
if( outputParameters )
{
hpi_handle_t hpiStream;
PA_DEBUG(( "%s: Checking output params: dev=%d, sr=%d, chans=%d, fmt=%d\n",
__FUNCTION__, outputParameters->device, (int)sampleRate,
outputParameters->channelCount, outputParameters->sampleFormat ));
PA_ENSURE_( PaAsiHpi_CreateFormat( hostApi, outputParameters, sampleRate,
&hpiDevice, &hpiFormat ) );
PA_ENSURE_( PaAsiHpi_OpenOutput( hostApi, hpiDevice, &hpiFormat, &hpiStream ) );
PA_ASIHPI_UNLESS_( HPI_OutStreamClose( NULL, hpiStream ), paNoError );
}
error:
return result;
}
static PaError PaAsiHpi_GetStreamInfo( PaAsiHpiStreamComponent *streamComp, PaAsiHpiStreamInfo *info )
{
PaError result = paDeviceUnavailable;
uint16_t state;
uint32_t bufferSize, dataSize, frameCounter, auxDataSize, threshold;
uint32_t hwBufferSize, hwDataSize;
assert( streamComp );
assert( info );
info->state = 0;
info->bufferSize = 0;
info->dataSize = 0;
info->frameCounter = 0;
info->auxDataSize = 0;
info->totalBufferedData = 0;
info->availableFrames = 0;
info->underflow = 0;
info->overflow = 0;
if( streamComp->hpiDevice && streamComp->hpiStream )
{
if( streamComp->hpiDevice->streamIsOutput )
{
PA_ASIHPI_UNLESS_( HPI_OutStreamGetInfoEx( NULL,
streamComp->hpiStream,
&state, &bufferSize, &dataSize, &frameCounter,
&auxDataSize ), paUnanticipatedHostError );
}
else
{
PA_ASIHPI_UNLESS_( HPI_InStreamGetInfoEx( NULL,
streamComp->hpiStream,
&state, &bufferSize, &dataSize, &frameCounter,
&auxDataSize ), paUnanticipatedHostError );
}
info->state = state;
info->bufferSize = bufferSize;
info->dataSize = dataSize;
info->frameCounter = frameCounter;
info->auxDataSize = auxDataSize;
info->totalBufferedData = dataSize;
if( streamComp->hostBufferSize > 0 )
info->totalBufferedData += auxDataSize;
info->totalBufferedData /= streamComp->bytesPerFrame;
info->availableFrames = streamComp->hpiDevice->streamIsOutput ?
bufferSize - dataSize : dataSize;
info->availableFrames /= streamComp->bytesPerFrame;
threshold = PA_MIN( streamComp->tempBufferSize,
streamComp->bytesPerFrame * PA_ASIHPI_MIN_FRAMES_ );
hwBufferSize = streamComp->hardwareBufferSize;
hwDataSize = streamComp->hostBufferSize > 0 ? auxDataSize : dataSize;
info->underflow = streamComp->hpiDevice->streamIsOutput ?
(frameCounter > 0) && ( (state == HPI_STATE_DRAINED) || (hwDataSize == 0) ) :
(state != HPI_STATE_STOPPED) && (dataSize < threshold);
info->overflow = (state != HPI_STATE_STOPPED) && (hwBufferSize - hwDataSize < threshold);
return paNoError;
}
error:
return result;
}
static void PaAsiHpi_StreamComponentDump( PaAsiHpiStreamComponent *streamComp,
PaAsiHpiStream *stream )
{
PaAsiHpiStreamInfo streamInfo;
assert( streamComp );
assert( stream );
PA_DEBUG(( "device: %s\n", streamComp->hpiDevice->baseDeviceInfo.name ));
if( streamComp->hpiDevice->streamIsOutput )
{
PA_DEBUG(( "user: %d-bit, %d ",
8*stream->bufferProcessor.bytesPerUserOutputSample,
stream->bufferProcessor.outputChannelCount));
if( stream->bufferProcessor.userOutputIsInterleaved )
{
PA_DEBUG(( "interleaved channels, " ));
}
else
{
PA_DEBUG(( "non-interleaved channels, " ));
}
PA_DEBUG(( "%d frames/buffer, latency = %5.1f ms\n",
stream->bufferProcessor.framesPerUserBuffer,
1000*stream->baseStreamRep.streamInfo.outputLatency ));
PA_DEBUG(( "host: %d-bit, %d interleaved channels, %d frames/buffer ",
8*stream->bufferProcessor.bytesPerHostOutputSample,
stream->bufferProcessor.outputChannelCount,
stream->bufferProcessor.framesPerHostBuffer ));
}
else
{
PA_DEBUG(( "user: %d-bit, %d ",
8*stream->bufferProcessor.bytesPerUserInputSample,
stream->bufferProcessor.inputChannelCount));
if( stream->bufferProcessor.userInputIsInterleaved )
{
PA_DEBUG(( "interleaved channels, " ));
}
else
{
PA_DEBUG(( "non-interleaved channels, " ));
}
PA_DEBUG(( "%d frames/buffer, latency = %5.1f ms\n",
stream->bufferProcessor.framesPerUserBuffer,
1000*stream->baseStreamRep.streamInfo.inputLatency ));
PA_DEBUG(( "host: %d-bit, %d interleaved channels, %d frames/buffer ",
8*stream->bufferProcessor.bytesPerHostInputSample,
stream->bufferProcessor.inputChannelCount,
stream->bufferProcessor.framesPerHostBuffer ));
}
switch( stream->bufferProcessor.hostBufferSizeMode )
{
case paUtilFixedHostBufferSize:
PA_DEBUG(( "[fixed] " ));
break;
case paUtilBoundedHostBufferSize:
PA_DEBUG(( "[bounded] " ));
break;
case paUtilUnknownHostBufferSize:
PA_DEBUG(( "[unknown] " ));
break;
case paUtilVariableHostBufferSizePartialUsageAllowed:
PA_DEBUG(( "[variable] " ));
break;
}
PA_DEBUG(( "(%d max)\n", streamComp->tempBufferSize / streamComp->bytesPerFrame ));
PA_DEBUG(( "HPI: adapter %d stream %d, %d-bit, %d-channel, %d Hz\n",
streamComp->hpiDevice->adapterIndex, streamComp->hpiDevice->streamIndex,
8 * streamComp->bytesPerFrame / streamComp->hpiFormat.wChannels,
streamComp->hpiFormat.wChannels,
streamComp->hpiFormat.dwSampleRate ));
PA_DEBUG(( "HPI: " ));
PaAsiHpi_GetStreamInfo( streamComp, &streamInfo );
switch( streamInfo.state )
{
case HPI_STATE_STOPPED:
PA_DEBUG(( "[STOPPED] " ));
break;
case HPI_STATE_PLAYING:
PA_DEBUG(( "[PLAYING] " ));
break;
case HPI_STATE_RECORDING:
PA_DEBUG(( "[RECORDING] " ));
break;
case HPI_STATE_DRAINED:
PA_DEBUG(( "[DRAINED] " ));
break;
default:
PA_DEBUG(( "[unknown state] " ));
break;
}
if( streamComp->hostBufferSize )
{
PA_DEBUG(( "host = %d/%d B, ", streamInfo.dataSize, streamComp->hostBufferSize ));
PA_DEBUG(( "hw = %d/%d (%d) B, ", streamInfo.auxDataSize,
streamComp->hardwareBufferSize, streamComp->outputBufferCap ));
}
else
{
PA_DEBUG(( "hw = %d/%d B, ", streamInfo.dataSize, streamComp->hardwareBufferSize ));
}
PA_DEBUG(( "count = %d", streamInfo.frameCounter ));
if( streamInfo.overflow )
{
PA_DEBUG(( " [overflow]" ));
}
else if( streamInfo.underflow )
{
PA_DEBUG(( " [underflow]" ));
}
PA_DEBUG(( "\n" ));
}
static void PaAsiHpi_StreamDump( PaAsiHpiStream *stream )
{
assert( stream );
PA_DEBUG(( "\n------------------------- STREAM INFO FOR %p ---------------------------\n", stream ));
if( stream->baseStreamRep.streamCallback )
{
PA_DEBUG(( "[callback] " ));
}
else
{
PA_DEBUG(( "[blocking] " ));
}
PA_DEBUG(( "sr=%d Hz, poll=%d ms, max %d frames/buf ",
(int)stream->baseStreamRep.streamInfo.sampleRate,
stream->pollingInterval, stream->maxFramesPerHostBuffer ));
switch( stream->state )
{
case paAsiHpiStoppedState:
PA_DEBUG(( "[stopped]\n" ));
break;
case paAsiHpiActiveState:
PA_DEBUG(( "[active]\n" ));
break;
case paAsiHpiCallbackFinishedState:
PA_DEBUG(( "[cb fin]\n" ));
break;
default:
PA_DEBUG(( "[unknown state]\n" ));
break;
}
if( stream->callbackMode )
{
PA_DEBUG(( "cb info: thread=%p, cbAbort=%d, cbFinished=%d\n",
stream->thread.thread, stream->callbackAbort, stream->callbackFinished ));
}
PA_DEBUG(( "----------------------------------- Input  ------------------------------------\n" ));
if( stream->input )
{
PaAsiHpi_StreamComponentDump( stream->input, stream );
}
else
{
PA_DEBUG(( "*none*\n" ));
}
PA_DEBUG(( "----------------------------------- Output ------------------------------------\n" ));
if( stream->output )
{
PaAsiHpi_StreamComponentDump( stream->output, stream );
}
else
{
PA_DEBUG(( "*none*\n" ));
}
PA_DEBUG(( "-------------------------------------------------------------------------------\n\n" ));
}
static PaError PaAsiHpi_SetupBuffers( PaAsiHpiStreamComponent *streamComp, uint32_t pollingInterval,
unsigned long framesPerPaHostBuffer, PaTime suggestedLatency )
{
PaError result = paNoError;
PaAsiHpiStreamInfo streamInfo;
unsigned long hpiBufferSize = 0, paHostBufferSize = 0;
assert( streamComp );
assert( streamComp->hpiDevice );
PA_ENSURE_( PaAsiHpi_GetStreamInfo( streamComp, &streamInfo ) );
streamComp->hardwareBufferSize = streamInfo.bufferSize;
hpiBufferSize = streamInfo.bufferSize;
if( PA_ASIHPI_USE_BBM_ )
{
uint32_t bbmBufferSize = 0, preLatencyBufferSize = 0;
hpi_err_t hpiError = 0;
PaTime pollingOverhead;
pollingOverhead = PaUtil_GetTime();
Pa_Sleep( 0 );
pollingOverhead = 1000*(PaUtil_GetTime() - pollingOverhead);
PA_DEBUG(( "polling overhead = %f ms (length of 0-second sleep)\n", pollingOverhead ));
PA_ASIHPI_UNLESS_( HPI_StreamEstimateBufferSize( &streamComp->hpiFormat,
pollingInterval + (uint32_t)ceil( pollingOverhead ),
&bbmBufferSize ), paUnanticipatedHostError );
bbmBufferSize *= 3;
if( bbmBufferSize < 3 * streamComp->bytesPerFrame * framesPerPaHostBuffer )
bbmBufferSize = 3 * streamComp->bytesPerFrame * framesPerPaHostBuffer;
if( suggestedLatency > 0.0 )
{
PaTime bufferDuration = ((PaTime)bbmBufferSize) / streamComp->bytesPerFrame
/ streamComp->hpiFormat.dwSampleRate;
if( bufferDuration < suggestedLatency )
{
preLatencyBufferSize = bbmBufferSize;
bbmBufferSize = (uint32_t)ceil( suggestedLatency * streamComp->bytesPerFrame
* streamComp->hpiFormat.dwSampleRate );
}
}
bbmBufferSize = ((uint32_t)ceil((bbmBufferSize + 20)/4096.0))*4096 - 20;
streamComp->hostBufferSize = bbmBufferSize;
if( streamComp->hpiDevice->streamIsOutput )
hpiError = HPI_OutStreamHostBufferAllocate( NULL,
streamComp->hpiStream,
bbmBufferSize );
else
hpiError = HPI_InStreamHostBufferAllocate( NULL,
streamComp->hpiStream,
bbmBufferSize );
if( hpiError )
{
streamComp->hostBufferSize = 0;
if( hpiError == HPI_ERROR_INVALID_DATASIZE )
{
if( preLatencyBufferSize > 0 )
{
PA_DEBUG(( "Retrying BBM allocation with smaller size (%d vs. %d bytes)\n",
preLatencyBufferSize, bbmBufferSize ));
bbmBufferSize = preLatencyBufferSize;
if( streamComp->hpiDevice->streamIsOutput )
hpiError = HPI_OutStreamHostBufferAllocate( NULL,
streamComp->hpiStream,
bbmBufferSize );
else
hpiError = HPI_InStreamHostBufferAllocate( NULL,
streamComp->hpiStream,
bbmBufferSize );
if( hpiError )
{
PA_ASIHPI_REPORT_ERROR_( hpiError );
if( hpiError == HPI_ERROR_INVALID_DATASIZE )
{
result = paBufferTooBig;
goto error;
}
else if( hpiError != HPI_ERROR_INVALID_OPERATION )
{
result = paUnanticipatedHostError;
goto error;
}
}
else
{
streamComp->hostBufferSize = bbmBufferSize;
hpiBufferSize = bbmBufferSize;
}
}
else
{
result = paBufferTooBig;
goto error;
}
}
else if (( hpiError != HPI_ERROR_INVALID_OPERATION ) &&
( hpiError != HPI_ERROR_INVALID_FUNC ))
{
PA_ASIHPI_REPORT_ERROR_( hpiError );
result = paUnanticipatedHostError;
goto error;
}
}
else
{
hpiBufferSize = bbmBufferSize;
}
}
paHostBufferSize = streamComp->bytesPerFrame * framesPerPaHostBuffer;
if( hpiBufferSize < 3*paHostBufferSize )
{
result = paBufferTooBig;
goto error;
}
if( streamComp->hpiDevice->streamIsOutput )
{
PaTime latency = suggestedLatency > 0.0 ? suggestedLatency :
streamComp->hpiDevice->baseDeviceInfo.defaultHighOutputLatency;
streamComp->outputBufferCap =
(uint32_t)ceil( latency * streamComp->bytesPerFrame * streamComp->hpiFormat.dwSampleRate );
if( streamComp->outputBufferCap < 4*paHostBufferSize )
streamComp->outputBufferCap = 4*paHostBufferSize;
}
else
{
streamComp->outputBufferCap = 0;
}
streamComp->tempBufferSize = paHostBufferSize;
PA_UNLESS_( streamComp->tempBuffer = (uint8_t *)PaUtil_AllocateMemory( streamComp->tempBufferSize ),
paInsufficientMemory );
error:
return result;
}
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream **s,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback *streamCallback,
void *userData )
{
PaError result = paNoError;
PaAsiHpiHostApiRepresentation *hpiHostApi = (PaAsiHpiHostApiRepresentation*)hostApi;
PaAsiHpiStream *stream = NULL;
unsigned long framesPerHostBuffer = framesPerBuffer;
int inputChannelCount = 0, outputChannelCount = 0;
PaSampleFormat inputSampleFormat = 0, outputSampleFormat = 0;
PaSampleFormat hostInputSampleFormat = 0, hostOutputSampleFormat = 0;
PaTime maxSuggestedLatency = 0.0;
if( (streamFlags & paPlatformSpecificFlags) != 0 )
return paInvalidFlag;  
PA_UNLESS_( stream = (PaAsiHpiStream *)PaUtil_AllocateMemory( sizeof(PaAsiHpiStream) ),
paInsufficientMemory );
memset( stream, 0, sizeof(PaAsiHpiStream) );
if( framesPerHostBuffer == paFramesPerBufferUnspecified )
{
if( inputParameters )
maxSuggestedLatency = inputParameters->suggestedLatency;
if( outputParameters && (outputParameters->suggestedLatency > maxSuggestedLatency) )
maxSuggestedLatency = outputParameters->suggestedLatency;
if( maxSuggestedLatency > 0.0 )
framesPerHostBuffer = (unsigned long)ceil( maxSuggestedLatency * sampleRate );
else
framesPerHostBuffer = 4096;
}
if( 1000.0*framesPerHostBuffer/sampleRate < PA_ASIHPI_MIN_POLLING_INTERVAL_ )
framesPerHostBuffer = (unsigned long)ceil( sampleRate * PA_ASIHPI_MIN_POLLING_INTERVAL_ / 1000.0 );
if( framesPerBuffer > 0 )
framesPerHostBuffer = (unsigned long)ceil( (double)framesPerHostBuffer / framesPerBuffer ) * framesPerBuffer;
framesPerHostBuffer = (framesPerHostBuffer / 4) * 4;
stream->pollingInterval = (uint32_t)ceil( 1000.0*framesPerHostBuffer/sampleRate );
assert( framesPerHostBuffer > 0 );
if( inputParameters )
{
PA_UNLESS_( stream->input = (PaAsiHpiStreamComponent *)PaUtil_AllocateMemory( sizeof(PaAsiHpiStreamComponent) ),
paInsufficientMemory );
memset( stream->input, 0, sizeof(PaAsiHpiStreamComponent) );
PA_ENSURE_( PaAsiHpi_CreateFormat( hostApi, inputParameters, sampleRate,
&stream->input->hpiDevice, &stream->input->hpiFormat ) );
PA_ENSURE_( PaAsiHpi_OpenInput( hostApi, stream->input->hpiDevice, &stream->input->hpiFormat,
&stream->input->hpiStream ) );
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
hostInputSampleFormat = PaAsiHpi_HpiToPaFormat( stream->input->hpiFormat.wFormat );
stream->input->bytesPerFrame = inputChannelCount * Pa_GetSampleSize( hostInputSampleFormat );
assert( stream->input->bytesPerFrame > 0 );
PA_ENSURE_( PaAsiHpi_SetupBuffers( stream->input, stream->pollingInterval,
framesPerHostBuffer, inputParameters->suggestedLatency ) );
}
if( outputParameters )
{
PA_UNLESS_( stream->output = (PaAsiHpiStreamComponent *)PaUtil_AllocateMemory( sizeof(PaAsiHpiStreamComponent) ),
paInsufficientMemory );
memset( stream->output, 0, sizeof(PaAsiHpiStreamComponent) );
PA_ENSURE_( PaAsiHpi_CreateFormat( hostApi, outputParameters, sampleRate,
&stream->output->hpiDevice, &stream->output->hpiFormat ) );
PA_ENSURE_( PaAsiHpi_OpenOutput( hostApi, stream->output->hpiDevice,
&stream->output->hpiFormat,
&stream->output->hpiStream ) );
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
hostOutputSampleFormat = PaAsiHpi_HpiToPaFormat( stream->output->hpiFormat.wFormat );
stream->output->bytesPerFrame = outputChannelCount * Pa_GetSampleSize( hostOutputSampleFormat );
PA_ENSURE_( PaAsiHpi_SetupBuffers( stream->output, stream->pollingInterval,
framesPerHostBuffer, outputParameters->suggestedLatency ) );
}
if( inputParameters && outputParameters )
{
stream->maxFramesPerHostBuffer = PA_MIN( stream->input->tempBufferSize / stream->input->bytesPerFrame,
stream->output->tempBufferSize / stream->output->bytesPerFrame );
}
else
{
stream->maxFramesPerHostBuffer = inputParameters ? stream->input->tempBufferSize / stream->input->bytesPerFrame
: stream->output->tempBufferSize / stream->output->bytesPerFrame;
}
assert( stream->maxFramesPerHostBuffer > 0 );
stream->neverDropInput = streamFlags & paNeverDropInput;
stream->state = paAsiHpiStoppedState;
if( streamCallback )
{
PaUtil_InitializeStreamRepresentation( &stream->baseStreamRep,
&hpiHostApi->callbackStreamInterface,
streamCallback, userData );
stream->callbackMode = 1;
}
else
{
PaUtil_InitializeStreamRepresentation( &stream->baseStreamRep,
&hpiHostApi->blockingStreamInterface,
streamCallback, userData );
PA_UNLESS_( stream->blockingUserBufferCopy =
PaUtil_AllocateMemory( sizeof(void *) * PA_MAX( inputChannelCount, outputChannelCount ) ),
paInsufficientMemory );
stream->callbackMode = 0;
}
PaUtil_InitializeCpuLoadMeasurer( &stream->cpuLoadMeasurer, sampleRate );
PA_ENSURE_( PaUtil_InitializeBufferProcessor( &stream->bufferProcessor,
inputChannelCount, inputSampleFormat, hostInputSampleFormat,
outputChannelCount, outputSampleFormat, hostOutputSampleFormat,
sampleRate, streamFlags,
framesPerBuffer, framesPerHostBuffer, paUtilFixedHostBufferSize,
streamCallback, userData ) );
stream->baseStreamRep.streamInfo.structVersion = 1;
stream->baseStreamRep.streamInfo.sampleRate = sampleRate;
if( stream->input )
{
PaTime bufferDuration = ( stream->input->hostBufferSize + stream->input->hardwareBufferSize )
/ sampleRate / stream->input->bytesPerFrame;
stream->baseStreamRep.streamInfo.inputLatency =
bufferDuration +
((PaTime)PaUtil_GetBufferProcessorInputLatencyFrames( &stream->bufferProcessor ) -
stream->maxFramesPerHostBuffer) / sampleRate;
assert( stream->baseStreamRep.streamInfo.inputLatency > 0.0 );
}
if( stream->output )
{
PaTime bufferDuration = ( stream->output->hostBufferSize + stream->output->hardwareBufferSize )
/ sampleRate / stream->output->bytesPerFrame;
if( !stream->input && (stream->output->outputBufferCap > 0) )
{
bufferDuration = PA_MIN( bufferDuration,
stream->output->outputBufferCap / sampleRate / stream->output->bytesPerFrame );
}
stream->baseStreamRep.streamInfo.outputLatency =
bufferDuration +
((PaTime)PaUtil_GetBufferProcessorOutputLatencyFrames( &stream->bufferProcessor ) -
stream->maxFramesPerHostBuffer) / sampleRate;
assert( stream->baseStreamRep.streamInfo.outputLatency > 0.0 );
}
PaAsiHpi_StreamDump( stream );
*s = (PaStream*)stream;
return result;
error:
CloseStream( (PaStream*)stream );
return result;
}
static PaError CloseStream( PaStream *s )
{
PaError result = paNoError;
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
if( stream == NULL )
return paNoError;
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
PaUtil_TerminateStreamRepresentation( &stream->baseStreamRep );
if( stream->input )
{
if( stream->input->hpiStream )
{
PA_ASIHPI_UNLESS_( HPI_InStreamClose( NULL,
stream->input->hpiStream ), paUnanticipatedHostError );
}
PaUtil_FreeMemory( stream->input->tempBuffer );
PaUtil_FreeMemory( stream->input );
}
if( stream->output )
{
if( stream->output->hpiStream )
{
PA_ASIHPI_UNLESS_( HPI_OutStreamClose( NULL,
stream->output->hpiStream ), paUnanticipatedHostError );
}
PaUtil_FreeMemory( stream->output->tempBuffer );
PaUtil_FreeMemory( stream->output );
}
PaUtil_FreeMemory( stream->blockingUserBufferCopy );
PaUtil_FreeMemory( stream );
error:
return result;
}
static PaError PaAsiHpi_PrimeOutputWithSilence( PaAsiHpiStream *stream )
{
PaError result = paNoError;
PaAsiHpiStreamComponent *out;
PaUtilZeroer *zeroer;
PaSampleFormat outputFormat;
assert( stream );
out = stream->output;
if( !out )
return result;
assert( out->tempBuffer );
PA_ASIHPI_UNLESS_( HPI_OutStreamReset( NULL,
out->hpiStream ), paUnanticipatedHostError );
outputFormat = PaAsiHpi_HpiToPaFormat( out->hpiFormat.wFormat );
zeroer = PaUtil_SelectZeroer( outputFormat );
zeroer(out->tempBuffer, 1, out->tempBufferSize / Pa_GetSampleSize(outputFormat) );
PA_ASIHPI_UNLESS_( HPI_OutStreamWriteBuf( NULL, out->hpiStream,
out->tempBuffer, out->tempBufferSize, &out->hpiFormat),
paUnanticipatedHostError );
PA_ASIHPI_UNLESS_( HPI_OutStreamWriteBuf( NULL, out->hpiStream,
out->tempBuffer, out->tempBufferSize, &out->hpiFormat),
paUnanticipatedHostError );
error:
return result;
}
static PaError PaAsiHpi_StartStream( PaAsiHpiStream *stream, int outputPrimed )
{
PaError result = paNoError;
if( stream->input )
{
PA_ASIHPI_UNLESS_( HPI_InStreamStart( NULL,
stream->input->hpiStream ), paUnanticipatedHostError );
}
if( stream->output )
{
if( !outputPrimed )
{
PA_ENSURE_( PaAsiHpi_PrimeOutputWithSilence( stream ) );
}
PA_ASIHPI_UNLESS_( HPI_OutStreamStart( NULL,
stream->output->hpiStream ), paUnanticipatedHostError );
}
stream->state = paAsiHpiActiveState;
stream->callbackFinished = 0;
error:
return result;
}
static PaError StartStream( PaStream *s )
{
PaError result = paNoError;
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
assert( stream );
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );
if( stream->callbackMode )
{
PA_ENSURE_( PaUnixThread_New( &stream->thread, &CallbackThreadFunc, stream, 1., 0   ) );
}
else
{
PA_ENSURE_( PaAsiHpi_StartStream( stream, 0 ) );
}
error:
return result;
}
static PaError PaAsiHpi_StopStream( PaAsiHpiStream *stream, int abort )
{
PaError result = paNoError;
assert( stream );
if( stream->input )
{
PA_ASIHPI_UNLESS_( HPI_InStreamReset( NULL,
stream->input->hpiStream ), paUnanticipatedHostError );
}
if( stream->output )
{
if( !abort )
{
while( 1 )
{
PaAsiHpiStreamInfo streamInfo;
PaTime timeLeft;
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->output, &streamInfo ) );
if( (streamInfo.state != HPI_STATE_PLAYING) &&
(streamInfo.dataSize < stream->output->bytesPerFrame * PA_ASIHPI_MIN_FRAMES_) )
break;
timeLeft = 1000.0 * streamInfo.dataSize / stream->output->bytesPerFrame
/ stream->baseStreamRep.streamInfo.sampleRate;
Pa_Sleep( (long)ceil( timeLeft ) );
}
}
PA_ASIHPI_UNLESS_( HPI_OutStreamReset( NULL,
stream->output->hpiStream ), paUnanticipatedHostError );
}
error:
return result;
}
static PaError PaAsiHpi_ExplicitStop( PaAsiHpiStream *stream, int abort )
{
PaError result = paNoError;
if( stream->callbackMode )
{
PaError threadRes;
stream->callbackAbort = abort;
if( abort )
{
PA_DEBUG(( "Aborting callback\n" ));
}
else
{
PA_DEBUG(( "Stopping callback\n" ));
}
PA_ENSURE_( PaUnixThread_Terminate( &stream->thread, !abort, &threadRes ) );
if( threadRes != paNoError )
{
PA_DEBUG(( "Callback thread returned: %d\n", threadRes ));
}
}
else
{
PA_ENSURE_( PaAsiHpi_StopStream( stream, abort ) );
}
stream->state = paAsiHpiStoppedState;
error:
return result;
}
static PaError StopStream( PaStream *s )
{
return PaAsiHpi_ExplicitStop( (PaAsiHpiStream *) s, 0 );
}
static PaError AbortStream( PaStream *s )
{
return PaAsiHpi_ExplicitStop( (PaAsiHpiStream * ) s, 1 );
}
static PaError IsStreamStopped( PaStream *s )
{
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
assert( stream );
return stream->state == paAsiHpiStoppedState ? 1 : 0;
}
static PaError IsStreamActive( PaStream *s )
{
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
assert( stream );
return stream->state == paAsiHpiActiveState ? 1 : 0;
}
static PaTime GetStreamTime( PaStream *s )
{
return PaUtil_GetTime();
}
static double GetStreamCpuLoad( PaStream *s )
{
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
return stream->callbackMode ? PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer ) : 0.0;
}
static void PaAsiHpi_OnThreadExit( void *userData )
{
PaAsiHpiStream *stream = (PaAsiHpiStream *) userData;
assert( stream );
PaUtil_ResetCpuLoadMeasurer( &stream->cpuLoadMeasurer );
PA_DEBUG(( "%s: Stopping HPI streams\n", __FUNCTION__ ));
PaAsiHpi_StopStream( stream, stream->callbackAbort );
PA_DEBUG(( "%s: Stoppage\n", __FUNCTION__ ));
if( stream->baseStreamRep.streamFinishedCallback )
{
stream->baseStreamRep.streamFinishedCallback( stream->baseStreamRep.userData );
}
if( stream->callbackFinished )
stream->state = paAsiHpiCallbackFinishedState;
}
static PaError PaAsiHpi_WaitForFrames( PaAsiHpiStream *stream, unsigned long *framesAvail,
PaStreamCallbackFlags *cbFlags )
{
PaError result = paNoError;
double sampleRate;
unsigned long framesTarget;
uint32_t outputData = 0, outputSpace = 0, inputData = 0, framesLeft = 0;
assert( stream );
assert( stream->input || stream->output );
sampleRate = stream->baseStreamRep.streamInfo.sampleRate;
framesTarget = stream->bufferProcessor.framesPerHostBuffer;
assert( framesTarget > 0 );
while( 1 )
{
PaAsiHpiStreamInfo info;
if( stream->output )
{
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->output, &info ) );
if( info.availableFrames < framesTarget )
{
framesLeft = framesTarget - info.availableFrames;
Pa_Sleep( (long)ceil( 1000 * framesLeft / sampleRate ) );
continue;
}
if( !stream->input && (stream->output->outputBufferCap > 0) &&
( info.totalBufferedData > stream->output->outputBufferCap / stream->output->bytesPerFrame ) )
{
framesLeft = info.totalBufferedData - stream->output->outputBufferCap / stream->output->bytesPerFrame;
Pa_Sleep( (long)ceil( 1000 * framesLeft / sampleRate ) );
continue;
}
outputData = info.totalBufferedData;
outputSpace = info.availableFrames;
if( info.underflow )
{
*cbFlags |= paOutputUnderflow;
}
}
if( stream->input )
{
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->input, &info ) );
if( info.availableFrames < framesTarget )
{
framesLeft = framesTarget - info.availableFrames;
if( !stream->output || (outputData > framesLeft) )
{
Pa_Sleep( (long)ceil( 1000 * framesLeft / sampleRate ) );
continue;
}
}
inputData = info.availableFrames;
if( info.overflow )
{
*cbFlags |= paInputOverflow;
}
}
break;
}
if( stream->input && stream->output )
{
if( outputSpace >= framesTarget )
*framesAvail = outputSpace;
if( (inputData >= framesTarget) && (inputData < outputSpace) )
*framesAvail = inputData;
}
else
{
*framesAvail = stream->input ? inputData : outputSpace;
}
error:
return result;
}
static void PaAsiHpi_CalculateTimeInfo( PaAsiHpiStream *stream, PaStreamCallbackTimeInfo *timeInfo )
{
PaAsiHpiStreamInfo streamInfo;
double sampleRate;
assert( stream );
assert( timeInfo );
sampleRate = stream->baseStreamRep.streamInfo.sampleRate;
timeInfo->currentTime = GetStreamTime( (PaStream *)stream );
timeInfo->inputBufferAdcTime = timeInfo->currentTime;
if( stream->input )
{
PaAsiHpi_GetStreamInfo( stream->input, &streamInfo );
timeInfo->inputBufferAdcTime -= streamInfo.totalBufferedData / sampleRate;
}
timeInfo->outputBufferDacTime = timeInfo->currentTime;
if( stream->output )
{
PaAsiHpi_GetStreamInfo( stream->output, &streamInfo );
timeInfo->outputBufferDacTime += streamInfo.totalBufferedData / sampleRate;
}
}
static PaError PaAsiHpi_BeginProcessing( PaAsiHpiStream *stream, unsigned long *numFrames,
PaStreamCallbackFlags *cbFlags )
{
PaError result = paNoError;
assert( stream );
if( *numFrames > stream->maxFramesPerHostBuffer )
*numFrames = stream->maxFramesPerHostBuffer;
if( stream->input )
{
PaAsiHpiStreamInfo info;
uint32_t framesToGet = *numFrames;
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->input, &info ) );
if( info.overflow )
{
*cbFlags |= paInputOverflow;
}
if( framesToGet > info.availableFrames )
{
PaUtilZeroer *zeroer;
PaSampleFormat inputFormat;
if( stream->output )
*cbFlags |= paInputUnderflow;
framesToGet = info.availableFrames;
inputFormat = PaAsiHpi_HpiToPaFormat( stream->input->hpiFormat.wFormat );
zeroer = PaUtil_SelectZeroer( inputFormat );
zeroer(stream->input->tempBuffer, 1,
stream->input->tempBufferSize / Pa_GetSampleSize(inputFormat) );
}
PA_ASIHPI_UNLESS_( HPI_InStreamReadBuf( NULL,
stream->input->hpiStream,
stream->input->tempBuffer,
framesToGet * stream->input->bytesPerFrame),
paUnanticipatedHostError );
PaUtil_SetInputFrameCount( &stream->bufferProcessor, *numFrames );
PaUtil_SetInterleavedInputChannels( &stream->bufferProcessor,
0, stream->input->tempBuffer,
stream->input->hpiFormat.wChannels );
}
if( stream->output )
{
PaUtil_SetOutputFrameCount( &stream->bufferProcessor, *numFrames );
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor,
0, stream->output->tempBuffer,
stream->output->hpiFormat.wChannels );
}
error:
return result;
}
static PaError PaAsiHpi_EndProcessing( PaAsiHpiStream *stream, unsigned long numFrames,
PaStreamCallbackFlags *cbFlags )
{
PaError result = paNoError;
assert( stream );
if( stream->output )
{
PaAsiHpiStreamInfo info;
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->output, &info ) );
if( info.underflow )
{
*cbFlags |= paOutputUnderflow;
}
PA_ASIHPI_UNLESS_( HPI_OutStreamWriteBuf( NULL,
stream->output->hpiStream,
stream->output->tempBuffer,
numFrames * stream->output->bytesPerFrame,
&stream->output->hpiFormat),
paUnanticipatedHostError );
}
error:
return result;
}
static void *CallbackThreadFunc( void *userData )
{
PaError result = paNoError;
PaAsiHpiStream *stream = (PaAsiHpiStream *) userData;
int callbackResult = paContinue;
assert( stream );
pthread_cleanup_push( &PaAsiHpi_OnThreadExit, stream );
PA_ENSURE_( PaUnixThread_PrepareNotify( &stream->thread ) );
PA_ENSURE_( PaAsiHpi_StartStream( stream, 0 ) );
PA_ENSURE_( PaUnixThread_NotifyParent( &stream->thread ) );
while( 1 )
{
PaStreamCallbackFlags cbFlags = 0;
unsigned long framesAvail, framesGot;
pthread_testcancel();
if( PaUnixThread_StopRequested( &stream->thread ) && (callbackResult == paContinue) )
{
PA_DEBUG(( "Setting callbackResult to paComplete\n" ));
callbackResult = paComplete;
}
if( callbackResult != paContinue )
{
stream->callbackAbort = (callbackResult == paAbort);
if( stream->callbackAbort ||
PaUtil_IsBufferProcessorOutputEmpty( &stream->bufferProcessor ) )
{
goto end;
}
PA_DEBUG(( "%s: Flushing buffer processor\n", __FUNCTION__ ));
}
PA_ENSURE_( PaAsiHpi_WaitForFrames( stream, &framesAvail, &cbFlags ) );
while( framesAvail > 0 )
{
PaStreamCallbackTimeInfo timeInfo = {0, 0, 0};
pthread_testcancel();
framesGot = framesAvail;
if( stream->bufferProcessor.hostBufferSizeMode == paUtilFixedHostBufferSize )
{
framesGot = framesGot >= stream->maxFramesPerHostBuffer ? stream->maxFramesPerHostBuffer : 0;
}
else
{
assert( stream->bufferProcessor.hostBufferSizeMode == paUtilBoundedHostBufferSize );
framesGot = PA_MIN( framesGot, stream->maxFramesPerHostBuffer );
}
PaAsiHpi_CalculateTimeInfo( stream, &timeInfo );
PaUtil_BeginBufferProcessing( &stream->bufferProcessor, &timeInfo, cbFlags );
PaUtil_BeginCpuLoadMeasurement( &stream->cpuLoadMeasurer );
if( framesGot > 0 )
{
PA_ENSURE_( PaAsiHpi_BeginProcessing( stream, &framesGot, &cbFlags ) );
if( stream->input && stream->output && (cbFlags & paInputOverflow) )
{
if( stream->neverDropInput )
{
PaUtil_SetNoOutput( &stream->bufferProcessor );
cbFlags |= paOutputOverflow;
}
}
PaUtil_EndBufferProcessing( &stream->bufferProcessor, &callbackResult );
cbFlags = 0;
PA_ENSURE_( PaAsiHpi_EndProcessing( stream, framesGot, &cbFlags ) );
framesAvail -= framesGot;
}
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, framesGot );
if( framesGot == 0 )
{
break;
}
if( callbackResult != paContinue )
break;
}
}
pthread_cleanup_pop( 1 );
end:
stream->callbackFinished = 1;
PA_DEBUG(( "%s: Thread %d exiting (callbackResult = %d)\n ",
__FUNCTION__, pthread_self(), callbackResult ));
PaUnixThreading_EXIT( result );
error:
goto end;
}
static PaError ReadStream( PaStream *s,
void *buffer,
unsigned long frames )
{
PaError result = paNoError;
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
PaAsiHpiStreamInfo info;
void *userBuffer;
assert( stream );
PA_UNLESS_( stream->input, paCanNotReadFromAnOutputOnlyStream );
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->input, &info ) );
if( info.overflow )
{
result = paInputOverflowed;
}
if( stream->bufferProcessor.userInputIsInterleaved )
{
userBuffer = buffer;
}
else
{
userBuffer = stream->blockingUserBufferCopy;
memcpy( userBuffer, buffer, sizeof (void *) * stream->input->hpiFormat.wChannels );
}
while( frames > 0 )
{
unsigned long framesGot, framesAvail;
PaStreamCallbackFlags cbFlags = 0;
PA_ENSURE_( PaAsiHpi_WaitForFrames( stream, &framesAvail, &cbFlags ) );
framesGot = PA_MIN( framesAvail, frames );
PA_ENSURE_( PaAsiHpi_BeginProcessing( stream, &framesGot, &cbFlags ) );
if( framesGot > 0 )
{
framesGot = PaUtil_CopyInput( &stream->bufferProcessor, &userBuffer, framesGot );
PA_ENSURE_( PaAsiHpi_EndProcessing( stream, framesGot, &cbFlags ) );
frames -= framesGot;
}
}
error:
return result;
}
static PaError WriteStream( PaStream *s,
const void *buffer,
unsigned long frames )
{
PaError result = paNoError;
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
PaAsiHpiStreamInfo info;
const void *userBuffer;
assert( stream );
PA_UNLESS_( stream->output, paCanNotWriteToAnInputOnlyStream );
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->output, &info ) );
if( info.underflow )
{
result = paOutputUnderflowed;
}
if( stream->bufferProcessor.userOutputIsInterleaved )
{
userBuffer = buffer;
}
else
{
userBuffer = stream->blockingUserBufferCopy;
memcpy( (void *)userBuffer, buffer, sizeof (void *) * stream->output->hpiFormat.wChannels );
}
while( frames > 0 )
{
unsigned long framesGot, framesAvail;
PaStreamCallbackFlags cbFlags = 0;
PA_ENSURE_( PaAsiHpi_WaitForFrames( stream, &framesAvail, &cbFlags ) );
framesGot = PA_MIN( framesAvail, frames );
PA_ENSURE_( PaAsiHpi_BeginProcessing( stream, &framesGot, &cbFlags ) );
if( framesGot > 0 )
{
framesGot = PaUtil_CopyOutput( &stream->bufferProcessor, &userBuffer, framesGot );
PA_ENSURE_( PaAsiHpi_EndProcessing( stream, framesGot, &cbFlags ) );
frames -= framesGot;
}
}
error:
return result;
}
static signed long GetStreamReadAvailable( PaStream *s )
{
PaError result = paNoError;
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
PaAsiHpiStreamInfo info;
assert( stream );
PA_UNLESS_( stream->input, paCanNotReadFromAnOutputOnlyStream );
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->input, &info ) );
result = (info.availableFrames / stream->maxFramesPerHostBuffer) * stream->maxFramesPerHostBuffer;
if( info.overflow )
{
result = paInputOverflowed;
}
error:
return result;
}
static signed long GetStreamWriteAvailable( PaStream *s )
{
PaError result = paNoError;
PaAsiHpiStream *stream = (PaAsiHpiStream*)s;
PaAsiHpiStreamInfo info;
assert( stream );
PA_UNLESS_( stream->output, paCanNotWriteToAnInputOnlyStream );
PA_ENSURE_( PaAsiHpi_GetStreamInfo( stream->output, &info ) );
result = (info.availableFrames / stream->maxFramesPerHostBuffer) * stream->maxFramesPerHostBuffer;
if( info.underflow )
{
result = paOutputUnderflowed;
}
error:
return result;
}

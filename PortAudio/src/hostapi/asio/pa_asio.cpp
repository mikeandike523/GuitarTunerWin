 
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <new>
#include <windows.h>
#include <mmsystem.h>
#include "portaudio.h"
#include "pa_asio.h"
#include "pa_util.h"
#include "pa_allocation.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_cpuload.h"
#include "pa_process.h"
#include "pa_debugprint.h"
#include "pa_ringbuffer.h"
#include "pa_win_coinitialize.h"
#include "pa_win_util.h"
#ifndef WIN32
#define WIN32
#endif
#include "asiosys.h"
#include "asio.h"
#include "asiodrivers.h"
#include "iasiothiscallresolver.h"
#if (defined(WIN32) && (defined(_MSC_VER) && (_MSC_VER >= 1200)))  
#pragma comment(lib, "winmm.lib")
#endif
extern AsioDrivers* asioDrivers;
#define CARBON_COMPATIBLE  (0)
extern "C" PaError PaAsio_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex );
static void Terminate( struct PaUtilHostApiRepresentation *hostApi );
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream** s,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback *streamCallback,
void *userData );
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate );
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
static int BlockingIoPaCallback(const void                     *inputBuffer    ,
void                     *outputBuffer   ,
unsigned long             framesPerBuffer,
const PaStreamCallbackTimeInfo *timeInfo       ,
PaStreamCallbackFlags     statusFlags    ,
void                     *userData       );
static void bufferSwitch(long index, ASIOBool processNow);
static ASIOTime *bufferSwitchTimeInfo(ASIOTime *timeInfo, long index, ASIOBool processNow);
static void sampleRateChanged(ASIOSampleRate sRate);
static long asioMessages(long selector, long value, void* message, double* opt);
static ASIOCallbacks asioCallbacks_ =
{ bufferSwitch, sampleRateChanged, asioMessages, bufferSwitchTimeInfo };
#define PA_ASIO_SET_LAST_HOST_ERROR( errorCode, errorText ) \
PaUtil_SetLastHostErrorInfo( paASIO, errorCode, errorText )
static void PaAsio_SetLastSystemError( DWORD errorCode )
{
PaWinUtil_SetLastSystemErrorInfo( paASIO, errorCode );
}
#define PA_ASIO_SET_LAST_SYSTEM_ERROR( errorCode ) \
PaAsio_SetLastSystemError( errorCode )
static const char* PaAsio_GetAsioErrorText( ASIOError asioError )
{
const char *result;
switch( asioError ){
case ASE_OK:
case ASE_SUCCESS:           result = "Success"; break;
case ASE_NotPresent:        result = "Hardware input or output is not present or available"; break;
case ASE_HWMalfunction:     result = "Hardware is malfunctioning"; break;
case ASE_InvalidParameter:  result = "Input parameter invalid"; break;
case ASE_InvalidMode:       result = "Hardware is in a bad mode or used in a bad mode"; break;
case ASE_SPNotAdvancing:    result = "Hardware is not running when sample position is inquired"; break;
case ASE_NoClock:           result = "Sample clock or rate cannot be determined or is not present"; break;
case ASE_NoMemory:          result = "Not enough memory for completing the request"; break;
default:                    result = "Unknown ASIO error"; break;
}
return result;
}
#define PA_ASIO_SET_LAST_ASIO_ERROR( asioError ) \
PaUtil_SetLastHostErrorInfo( paASIO, asioError, PaAsio_GetAsioErrorText( asioError ) )
#if MAC
inline long PaAsio_AtomicIncrement(volatile long* v) {return ++(*const_cast<long*>(v));}
inline long PaAsio_AtomicDecrement(volatile long* v) {return --(*const_cast<long*>(v));}
#elif WINDOWS
inline long PaAsio_AtomicIncrement(volatile long* v) {return InterlockedIncrement(const_cast<long*>(v));}
inline long PaAsio_AtomicDecrement(volatile long* v) {return InterlockedDecrement(const_cast<long*>(v));}
#endif
typedef struct PaAsioDriverInfo
{
ASIODriverInfo asioDriverInfo;
long inputChannelCount, outputChannelCount;
long bufferMinSize, bufferMaxSize, bufferPreferredSize, bufferGranularity;
bool postOutput;
}
PaAsioDriverInfo;
typedef struct
{
PaUtilHostApiRepresentation inheritedHostApiRep;
PaUtilStreamInterface callbackStreamInterface;
PaUtilStreamInterface blockingStreamInterface;
PaUtilAllocationGroup *allocations;
PaWinUtilComInitializationResult comInitializationResult;
AsioDrivers *asioDrivers;
void *systemSpecific;
PaDeviceIndex openAsioDeviceIndex;
PaAsioDriverInfo openAsioDriverInfo;
}
PaAsioHostApiRepresentation;
static char **GetAsioDriverNames( PaAsioHostApiRepresentation *asioHostApi, PaUtilAllocationGroup *group, long driverCount )
{
char **result = 0;
int i;
result =(char**)PaUtil_GroupAllocateMemory(
group, sizeof(char*) * driverCount );
if( !result )
goto error;
result[0] = (char*)PaUtil_GroupAllocateMemory(
group, 32 * driverCount );
if( !result[0] )
goto error;
for( i=0; i<driverCount; ++i )
result[i] = result[0] + (32 * i);
asioHostApi->asioDrivers->getDriverNames( result, driverCount );
error:
return result;
}
static PaSampleFormat AsioSampleTypeToPaNativeSampleFormat(ASIOSampleType type)
{
switch (type) {
case ASIOSTInt16MSB:
case ASIOSTInt16LSB:
return paInt16;
case ASIOSTFloat32MSB:
case ASIOSTFloat32LSB:
case ASIOSTFloat64MSB:
case ASIOSTFloat64LSB:
return paFloat32;
case ASIOSTInt32MSB:
case ASIOSTInt32LSB:
case ASIOSTInt32MSB16:
case ASIOSTInt32LSB16:
case ASIOSTInt32MSB18:
case ASIOSTInt32MSB20:
case ASIOSTInt32MSB24:
case ASIOSTInt32LSB18:
case ASIOSTInt32LSB20:
case ASIOSTInt32LSB24:
return paInt32;
case ASIOSTInt24MSB:
case ASIOSTInt24LSB:
return paInt24;
default:
return paCustomFormat;
}
}
void AsioSampleTypeLOG(ASIOSampleType type)
{
switch (type) {
case ASIOSTInt16MSB:  PA_DEBUG(("ASIOSTInt16MSB\n"));  break;
case ASIOSTInt16LSB:  PA_DEBUG(("ASIOSTInt16LSB\n"));  break;
case ASIOSTFloat32MSB:PA_DEBUG(("ASIOSTFloat32MSB\n"));break;
case ASIOSTFloat32LSB:PA_DEBUG(("ASIOSTFloat32LSB\n"));break;
case ASIOSTFloat64MSB:PA_DEBUG(("ASIOSTFloat64MSB\n"));break;
case ASIOSTFloat64LSB:PA_DEBUG(("ASIOSTFloat64LSB\n"));break;
case ASIOSTInt32MSB:  PA_DEBUG(("ASIOSTInt32MSB\n"));  break;
case ASIOSTInt32LSB:  PA_DEBUG(("ASIOSTInt32LSB\n"));  break;
case ASIOSTInt32MSB16:PA_DEBUG(("ASIOSTInt32MSB16\n"));break;
case ASIOSTInt32LSB16:PA_DEBUG(("ASIOSTInt32LSB16\n"));break;
case ASIOSTInt32MSB18:PA_DEBUG(("ASIOSTInt32MSB18\n"));break;
case ASIOSTInt32MSB20:PA_DEBUG(("ASIOSTInt32MSB20\n"));break;
case ASIOSTInt32MSB24:PA_DEBUG(("ASIOSTInt32MSB24\n"));break;
case ASIOSTInt32LSB18:PA_DEBUG(("ASIOSTInt32LSB18\n"));break;
case ASIOSTInt32LSB20:PA_DEBUG(("ASIOSTInt32LSB20\n"));break;
case ASIOSTInt32LSB24:PA_DEBUG(("ASIOSTInt32LSB24\n"));break;
case ASIOSTInt24MSB:  PA_DEBUG(("ASIOSTInt24MSB\n"));  break;
case ASIOSTInt24LSB:  PA_DEBUG(("ASIOSTInt24LSB\n"));  break;
default:              PA_DEBUG(("Custom Format%d\n",type));break;
}
}
static int BytesPerAsioSample( ASIOSampleType sampleType )
{
switch (sampleType) {
case ASIOSTInt16MSB:
case ASIOSTInt16LSB:
return 2;
case ASIOSTFloat64MSB:
case ASIOSTFloat64LSB:
return 8;
case ASIOSTFloat32MSB:
case ASIOSTFloat32LSB:
case ASIOSTInt32MSB:
case ASIOSTInt32LSB:
case ASIOSTInt32MSB16:
case ASIOSTInt32LSB16:
case ASIOSTInt32MSB18:
case ASIOSTInt32MSB20:
case ASIOSTInt32MSB24:
case ASIOSTInt32LSB18:
case ASIOSTInt32LSB20:
case ASIOSTInt32LSB24:
return 4;
case ASIOSTInt24MSB:
case ASIOSTInt24LSB:
return 3;
default:
return 0;
}
}
static void Swap16( void *buffer, long shift, long count )
{
unsigned short *p = (unsigned short*)buffer;
unsigned short temp;
(void) shift;  
while( count-- )
{
temp = *p;
*p++ = (unsigned short)((temp<<8) | (temp>>8));
}
}
static void Swap24( void *buffer, long shift, long count )
{
unsigned char *p = (unsigned char*)buffer;
unsigned char temp;
(void) shift;  
while( count-- )
{
temp = *p;
*p = *(p+2);
*(p+2) = temp;
p += 3;
}
}
#define PA_SWAP32_( x ) ((x>>24) | ((x>>8)&0xFF00) | ((x<<8)&0xFF0000) | (x<<24));
static void Swap32( void *buffer, long shift, long count )
{
unsigned long *p = (unsigned long*)buffer;
unsigned long temp;
(void) shift;  
while( count-- )
{
temp = *p;
*p++ = PA_SWAP32_( temp);
}
}
static void SwapShiftLeft32( void *buffer, long shift, long count )
{
unsigned long *p = (unsigned long*)buffer;
unsigned long temp;
while( count-- )
{
temp = *p;
temp = PA_SWAP32_( temp);
*p++ = temp << shift;
}
}
static void ShiftRightSwap32( void *buffer, long shift, long count )
{
unsigned long *p = (unsigned long*)buffer;
unsigned long temp;
while( count-- )
{
temp = *p >> shift;
*p++ = PA_SWAP32_( temp);
}
}
static void ShiftLeft32( void *buffer, long shift, long count )
{
unsigned long *p = (unsigned long*)buffer;
unsigned long temp;
while( count-- )
{
temp = *p;
*p++ = temp << shift;
}
}
static void ShiftRight32( void *buffer, long shift, long count )
{
unsigned long *p = (unsigned long*)buffer;
unsigned long temp;
while( count-- )
{
temp = *p;
*p++ = temp >> shift;
}
}
#define PA_SWAP_( x, y ) temp=x; x = y; y = temp;
static void Swap64ConvertFloat64ToFloat32( void *buffer, long shift, long count )
{
double *in = (double*)buffer;
float *out = (float*)buffer;
unsigned char *p;
unsigned char temp;
(void) shift;  
while( count-- )
{
p = (unsigned char*)in;
PA_SWAP_( p[0], p[7] );
PA_SWAP_( p[1], p[6] );
PA_SWAP_( p[2], p[5] );
PA_SWAP_( p[3], p[4] );
*out++ = (float) (*in++);
}
}
static void ConvertFloat64ToFloat32( void *buffer, long shift, long count )
{
double *in = (double*)buffer;
float *out = (float*)buffer;
(void) shift;  
while( count-- )
*out++ = (float) (*in++);
}
static void ConvertFloat32ToFloat64Swap64( void *buffer, long shift, long count )
{
float *in = ((float*)buffer) + (count-1);
double *out = ((double*)buffer) + (count-1);
unsigned char *p;
unsigned char temp;
(void) shift;  
while( count-- )
{
*out = *in--;
p = (unsigned char*)out;
PA_SWAP_( p[0], p[7] );
PA_SWAP_( p[1], p[6] );
PA_SWAP_( p[2], p[5] );
PA_SWAP_( p[3], p[4] );
out--;
}
}
static void ConvertFloat32ToFloat64( void *buffer, long shift, long count )
{
float *in = ((float*)buffer) + (count-1);
double *out = ((double*)buffer) + (count-1);
(void) shift;  
while( count-- )
*out-- = *in--;
}
#ifdef MAC
#define PA_MSB_IS_NATIVE_
#undef PA_LSB_IS_NATIVE_
#endif
#ifdef WINDOWS
#undef PA_MSB_IS_NATIVE_
#define PA_LSB_IS_NATIVE_
#endif
typedef void PaAsioBufferConverter( void *, long, long );
static void SelectAsioToPaConverter( ASIOSampleType type, PaAsioBufferConverter **converter, long *shift )
{
*shift = 0;
*converter = 0;
switch (type) {
case ASIOSTInt16MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap16;
#endif
break;
case ASIOSTInt16LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap16;
#endif
break;
case ASIOSTFloat32MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap32;
#endif
break;
case ASIOSTFloat32LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap32;
#endif
break;
case ASIOSTFloat64MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap64ConvertFloat64ToFloat32;
#else
*converter = ConvertFloat64ToFloat32;
#endif
break;
case ASIOSTFloat64LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap64ConvertFloat64ToFloat32;
#else
*converter = ConvertFloat64ToFloat32;
#endif
break;
case ASIOSTInt32MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap32;
#endif
break;
case ASIOSTInt32LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap32;
#endif
break;
case ASIOSTInt32MSB16:
#ifdef PA_LSB_IS_NATIVE_
*converter = SwapShiftLeft32;
#else
*converter = ShiftLeft32;
#endif
*shift = 16;
break;
case ASIOSTInt32MSB18:
#ifdef PA_LSB_IS_NATIVE_
*converter = SwapShiftLeft32;
#else
*converter = ShiftLeft32;
#endif
*shift = 14;
break;
case ASIOSTInt32MSB20:
#ifdef PA_LSB_IS_NATIVE_
*converter = SwapShiftLeft32;
#else
*converter = ShiftLeft32;
#endif
*shift = 12;
break;
case ASIOSTInt32MSB24:
#ifdef PA_LSB_IS_NATIVE_
*converter = SwapShiftLeft32;
#else
*converter = ShiftLeft32;
#endif
*shift = 8;
break;
case ASIOSTInt32LSB16:
#ifdef PA_MSB_IS_NATIVE_
*converter = SwapShiftLeft32;
#else
*converter = ShiftLeft32;
#endif
*shift = 16;
break;
case ASIOSTInt32LSB18:
#ifdef PA_MSB_IS_NATIVE_
*converter = SwapShiftLeft32;
#else
*converter = ShiftLeft32;
#endif
*shift = 14;
break;
case ASIOSTInt32LSB20:
#ifdef PA_MSB_IS_NATIVE_
*converter = SwapShiftLeft32;
#else
*converter = ShiftLeft32;
#endif
*shift = 12;
break;
case ASIOSTInt32LSB24:
#ifdef PA_MSB_IS_NATIVE_
*converter = SwapShiftLeft32;
#else
*converter = ShiftLeft32;
#endif
*shift = 8;
break;
case ASIOSTInt24MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap24;
#endif
break;
case ASIOSTInt24LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap24;
#endif
break;
}
}
static void SelectPaToAsioConverter( ASIOSampleType type, PaAsioBufferConverter **converter, long *shift )
{
*shift = 0;
*converter = 0;
switch (type) {
case ASIOSTInt16MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap16;
#endif
break;
case ASIOSTInt16LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap16;
#endif
break;
case ASIOSTFloat32MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap32;
#endif
break;
case ASIOSTFloat32LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap32;
#endif
break;
case ASIOSTFloat64MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = ConvertFloat32ToFloat64Swap64;
#else
*converter = ConvertFloat32ToFloat64;
#endif
break;
case ASIOSTFloat64LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = ConvertFloat32ToFloat64Swap64;
#else
*converter = ConvertFloat32ToFloat64;
#endif
break;
case ASIOSTInt32MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap32;
#endif
break;
case ASIOSTInt32LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap32;
#endif
break;
case ASIOSTInt32MSB16:
#ifdef PA_LSB_IS_NATIVE_
*converter = ShiftRightSwap32;
#else
*converter = ShiftRight32;
#endif
*shift = 16;
break;
case ASIOSTInt32MSB18:
#ifdef PA_LSB_IS_NATIVE_
*converter = ShiftRightSwap32;
#else
*converter = ShiftRight32;
#endif
*shift = 14;
break;
case ASIOSTInt32MSB20:
#ifdef PA_LSB_IS_NATIVE_
*converter = ShiftRightSwap32;
#else
*converter = ShiftRight32;
#endif
*shift = 12;
break;
case ASIOSTInt32MSB24:
#ifdef PA_LSB_IS_NATIVE_
*converter = ShiftRightSwap32;
#else
*converter = ShiftRight32;
#endif
*shift = 8;
break;
case ASIOSTInt32LSB16:
#ifdef PA_MSB_IS_NATIVE_
*converter = ShiftRightSwap32;
#else
*converter = ShiftRight32;
#endif
*shift = 16;
break;
case ASIOSTInt32LSB18:
#ifdef PA_MSB_IS_NATIVE_
*converter = ShiftRightSwap32;
#else
*converter = ShiftRight32;
#endif
*shift = 14;
break;
case ASIOSTInt32LSB20:
#ifdef PA_MSB_IS_NATIVE_
*converter = ShiftRightSwap32;
#else
*converter = ShiftRight32;
#endif
*shift = 12;
break;
case ASIOSTInt32LSB24:
#ifdef PA_MSB_IS_NATIVE_
*converter = ShiftRightSwap32;
#else
*converter = ShiftRight32;
#endif
*shift = 8;
break;
case ASIOSTInt24MSB:
#ifdef PA_LSB_IS_NATIVE_
*converter = Swap24;
#endif
break;
case ASIOSTInt24LSB:
#ifdef PA_MSB_IS_NATIVE_
*converter = Swap24;
#endif
break;
}
}
typedef struct PaAsioDeviceInfo
{
PaDeviceInfo commonDeviceInfo;
long minBufferSize;
long maxBufferSize;
long preferredBufferSize;
long bufferGranularity;
ASIOChannelInfo *asioChannelInfos;
}
PaAsioDeviceInfo;
PaError PaAsio_GetAvailableBufferSizes( PaDeviceIndex device,
long *minBufferSizeFrames, long *maxBufferSizeFrames, long *preferredBufferSizeFrames, long *granularity )
{
PaError result;
PaUtilHostApiRepresentation *hostApi;
PaDeviceIndex hostApiDevice;
result = PaUtil_GetHostApiRepresentation( &hostApi, paASIO );
if( result == paNoError )
{
result = PaUtil_DeviceIndexToHostApiDeviceIndex( &hostApiDevice, device, hostApi );
if( result == paNoError )
{
PaAsioDeviceInfo *asioDeviceInfo =
(PaAsioDeviceInfo*)hostApi->deviceInfos[hostApiDevice];
*minBufferSizeFrames = asioDeviceInfo->minBufferSize;
*maxBufferSizeFrames = asioDeviceInfo->maxBufferSize;
*preferredBufferSizeFrames = asioDeviceInfo->preferredBufferSize;
*granularity = asioDeviceInfo->bufferGranularity;
}
}
return result;
}
static void UnloadAsioDriver( void )
{
ASIOExit();
}
static PaError LoadAsioDriver( PaAsioHostApiRepresentation *asioHostApi, const char *driverName,
PaAsioDriverInfo *driverInfo, void *systemSpecific )
{
PaError result = paNoError;
ASIOError asioError;
int asioIsInitialized = 0;
if( !asioHostApi->asioDrivers->loadDriver( const_cast<char*>(driverName) ) )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_HOST_ERROR( 0, "Failed to load ASIO driver" );
goto error;
}
memset( &driverInfo->asioDriverInfo, 0, sizeof(ASIODriverInfo) );
driverInfo->asioDriverInfo.asioVersion = 2;
driverInfo->asioDriverInfo.sysRef = systemSpecific;
if( (asioError = ASIOInit( &driverInfo->asioDriverInfo )) != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
goto error;
}
else
{
asioIsInitialized = 1;
}
if( (asioError = ASIOGetChannels(&driverInfo->inputChannelCount,
&driverInfo->outputChannelCount)) != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
goto error;
}
if( (asioError = ASIOGetBufferSize(&driverInfo->bufferMinSize,
&driverInfo->bufferMaxSize, &driverInfo->bufferPreferredSize,
&driverInfo->bufferGranularity)) != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
goto error;
}
if( ASIOOutputReady() == ASE_OK )
driverInfo->postOutput = true;
else
driverInfo->postOutput = false;
return result;
error:
if( asioIsInitialized )
{
ASIOExit();
}
return result;
}
#define PA_DEFAULTSAMPLERATESEARCHORDER_COUNT_     13    
static ASIOSampleRate defaultSampleRateSearchOrder_[]
= {44100.0, 48000.0, 32000.0, 24000.0, 22050.0, 88200.0, 96000.0,
192000.0, 16000.0, 12000.0, 11025.0, 9600.0, 8000.0 };
static PaError InitPaDeviceInfoFromAsioDriver( PaAsioHostApiRepresentation *asioHostApi,
const char *driverName, int driverIndex,
PaDeviceInfo *deviceInfo, PaAsioDeviceInfo *asioDeviceInfo )
{
PaError result = paNoError;
union _tag_local {
PaAsioDriverInfo info;
char _padding[4096];
} paAsioDriver;
asioDeviceInfo->asioChannelInfos = 0;  
result = LoadAsioDriver( asioHostApi, driverName, &paAsioDriver.info, asioHostApi->systemSpecific );
if( result == paNoError )
{
PA_DEBUG(("PaAsio_Initialize: drv:%d name = %s\n",  driverIndex,deviceInfo->name));
PA_DEBUG(("PaAsio_Initialize: drv:%d inputChannels       = %d\n", driverIndex, paAsioDriver.info.inputChannelCount));
PA_DEBUG(("PaAsio_Initialize: drv:%d outputChannels      = %d\n", driverIndex, paAsioDriver.info.outputChannelCount));
PA_DEBUG(("PaAsio_Initialize: drv:%d bufferMinSize       = %d\n", driverIndex, paAsioDriver.info.bufferMinSize));
PA_DEBUG(("PaAsio_Initialize: drv:%d bufferMaxSize       = %d\n", driverIndex, paAsioDriver.info.bufferMaxSize));
PA_DEBUG(("PaAsio_Initialize: drv:%d bufferPreferredSize = %d\n", driverIndex, paAsioDriver.info.bufferPreferredSize));
PA_DEBUG(("PaAsio_Initialize: drv:%d bufferGranularity   = %d\n", driverIndex, paAsioDriver.info.bufferGranularity));
deviceInfo->maxInputChannels  = paAsioDriver.info.inputChannelCount;
deviceInfo->maxOutputChannels = paAsioDriver.info.outputChannelCount;
deviceInfo->defaultSampleRate = 0.;
bool foundDefaultSampleRate = false;
for( int j=0; j < PA_DEFAULTSAMPLERATESEARCHORDER_COUNT_; ++j )
{
ASIOError asioError = ASIOCanSampleRate( defaultSampleRateSearchOrder_[j] );
if( asioError != ASE_NoClock && asioError != ASE_NotPresent )
{
deviceInfo->defaultSampleRate = defaultSampleRateSearchOrder_[j];
foundDefaultSampleRate = true;
break;
}
}
PA_DEBUG(("PaAsio_Initialize: drv:%d defaultSampleRate = %f\n", driverIndex, deviceInfo->defaultSampleRate));
if( foundDefaultSampleRate ){
double defaultLowLatency =
paAsioDriver.info.bufferPreferredSize / deviceInfo->defaultSampleRate;
deviceInfo->defaultLowInputLatency = defaultLowLatency;
deviceInfo->defaultLowOutputLatency = defaultLowLatency;
double defaultHighLatency =
paAsioDriver.info.bufferMaxSize / deviceInfo->defaultSampleRate;
if( defaultHighLatency < defaultLowLatency )
defaultHighLatency = defaultLowLatency;  
deviceInfo->defaultHighInputLatency = defaultHighLatency;
deviceInfo->defaultHighOutputLatency = defaultHighLatency;
}else{
deviceInfo->defaultLowInputLatency = 0.;
deviceInfo->defaultLowOutputLatency = 0.;
deviceInfo->defaultHighInputLatency = 0.;
deviceInfo->defaultHighOutputLatency = 0.;
}
PA_DEBUG(("PaAsio_Initialize: drv:%d defaultLowInputLatency = %f\n", driverIndex, deviceInfo->defaultLowInputLatency));
PA_DEBUG(("PaAsio_Initialize: drv:%d defaultLowOutputLatency = %f\n", driverIndex, deviceInfo->defaultLowOutputLatency));
PA_DEBUG(("PaAsio_Initialize: drv:%d defaultHighInputLatency = %f\n", driverIndex, deviceInfo->defaultHighInputLatency));
PA_DEBUG(("PaAsio_Initialize: drv:%d defaultHighOutputLatency = %f\n", driverIndex, deviceInfo->defaultHighOutputLatency));
asioDeviceInfo->minBufferSize = paAsioDriver.info.bufferMinSize;
asioDeviceInfo->maxBufferSize = paAsioDriver.info.bufferMaxSize;
asioDeviceInfo->preferredBufferSize = paAsioDriver.info.bufferPreferredSize;
asioDeviceInfo->bufferGranularity = paAsioDriver.info.bufferGranularity;
asioDeviceInfo->asioChannelInfos = (ASIOChannelInfo*)PaUtil_GroupAllocateMemory(
asioHostApi->allocations,
sizeof(ASIOChannelInfo) * (deviceInfo->maxInputChannels
+ deviceInfo->maxOutputChannels) );
if( !asioDeviceInfo->asioChannelInfos )
{
result = paInsufficientMemory;
goto error_unload;
}
int a;
for( a=0; a < deviceInfo->maxInputChannels; ++a ){
asioDeviceInfo->asioChannelInfos[a].channel = a;
asioDeviceInfo->asioChannelInfos[a].isInput = ASIOTrue;
ASIOError asioError = ASIOGetChannelInfo( &asioDeviceInfo->asioChannelInfos[a] );
if( asioError != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
goto error_unload;
}
}
for( a=0; a < deviceInfo->maxOutputChannels; ++a ){
int b = deviceInfo->maxInputChannels + a;
asioDeviceInfo->asioChannelInfos[b].channel = a;
asioDeviceInfo->asioChannelInfos[b].isInput = ASIOFalse;
ASIOError asioError = ASIOGetChannelInfo( &asioDeviceInfo->asioChannelInfos[b] );
if( asioError != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
goto error_unload;
}
}
UnloadAsioDriver();
}
return result;
error_unload:
UnloadAsioDriver();
if( asioDeviceInfo->asioChannelInfos ){
PaUtil_GroupFreeMemory( asioHostApi->allocations, asioDeviceInfo->asioChannelInfos );
asioDeviceInfo->asioChannelInfos = 0;
}
return result;
}
typedef BOOL (WINAPI *IsDebuggerPresentPtr)(VOID);
IsDebuggerPresentPtr IsDebuggerPresent_ = 0;
PaError PaAsio_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result = paNoError;
int i, driverCount;
PaAsioHostApiRepresentation *asioHostApi;
PaAsioDeviceInfo *deviceInfoArray;
char **names;
asioHostApi = (PaAsioHostApiRepresentation*)PaUtil_AllocateMemory( sizeof(PaAsioHostApiRepresentation) );
if( !asioHostApi )
{
result = paInsufficientMemory;
goto error;
}
memset( asioHostApi, 0, sizeof(PaAsioHostApiRepresentation) );  
result = PaWinUtil_CoInitialize( paASIO, &asioHostApi->comInitializationResult );
if( result != paNoError )
{
goto error;
}
asioHostApi->asioDrivers = 0;  
asioHostApi->allocations = PaUtil_CreateAllocationGroup();
if( !asioHostApi->allocations )
{
result = paInsufficientMemory;
goto error;
}
try
{
asioHostApi->asioDrivers = new AsioDrivers();  
}
catch (std::bad_alloc)
{
asioHostApi->asioDrivers = 0;
}
if( asioHostApi->asioDrivers == 0 )
{
result = paInsufficientMemory;
goto error;
}
asioDrivers = asioHostApi->asioDrivers;  
asioHostApi->systemSpecific = 0;
asioHostApi->openAsioDeviceIndex = paNoDevice;
*hostApi = &asioHostApi->inheritedHostApiRep;
(*hostApi)->info.structVersion = 1;
(*hostApi)->info.type = paASIO;
(*hostApi)->info.name = "ASIO";
(*hostApi)->info.deviceCount = 0;
#ifdef WINDOWS
asioHostApi->systemSpecific = GetDesktopWindow();
#endif
#if MAC
driverCount = asioHostApi->asioDrivers->getNumFragments();
#elif WINDOWS
driverCount = asioHostApi->asioDrivers->asioGetNumDev();
#endif
if( driverCount > 0 )
{
names = GetAsioDriverNames( asioHostApi, asioHostApi->allocations, driverCount );
if( !names )
{
result = paInsufficientMemory;
goto error;
}
(*hostApi)->deviceInfos = (PaDeviceInfo**)PaUtil_GroupAllocateMemory(
asioHostApi->allocations, sizeof(PaDeviceInfo*) * driverCount );
if( !(*hostApi)->deviceInfos )
{
result = paInsufficientMemory;
goto error;
}
deviceInfoArray = (PaAsioDeviceInfo*)PaUtil_GroupAllocateMemory(
asioHostApi->allocations, sizeof(PaAsioDeviceInfo) * driverCount );
if( !deviceInfoArray )
{
result = paInsufficientMemory;
goto error;
}
IsDebuggerPresent_ = (IsDebuggerPresentPtr)GetProcAddress( LoadLibraryA( "Kernel32.dll" ), "IsDebuggerPresent" );
for( i=0; i < driverCount; ++i )
{
PA_DEBUG(("ASIO names[%d]:%s\n",i,names[i]));
if (   strcmp (names[i],"ASIO DirectX Full Duplex Driver") == 0
|| strcmp (names[i],"ASIO Multimedia Driver")          == 0
|| strncmp(names[i],"Premiere",8)                      == 0    
|| strncmp(names[i],"Adobe",5)                         == 0    
)
{
PA_DEBUG(("BLACKLISTED!!!\n"));
continue;
}
if( IsDebuggerPresent_ && IsDebuggerPresent_() )
{
if( strcmp(names[i], "ASIO Digidesign Driver") == 0 )
{
PA_DEBUG(("BLACKLISTED!!! ASIO Digidesign Driver would quit the debugger\n"));
continue;
}
}
{
PaAsioDeviceInfo *asioDeviceInfo = &deviceInfoArray[ (*hostApi)->info.deviceCount ];
PaDeviceInfo *deviceInfo = &asioDeviceInfo->commonDeviceInfo;
deviceInfo->structVersion = 2;
deviceInfo->hostApi = hostApiIndex;
deviceInfo->name = names[i];
if( InitPaDeviceInfoFromAsioDriver( asioHostApi, names[i], i, deviceInfo, asioDeviceInfo ) == paNoError )
{
(*hostApi)->deviceInfos[ (*hostApi)->info.deviceCount ] = deviceInfo;
++(*hostApi)->info.deviceCount;
}
else
{
PA_DEBUG(("Skipping ASIO device:%s\n",names[i]));
continue;
}
}
}
}
if( (*hostApi)->info.deviceCount > 0 )
{
(*hostApi)->info.defaultInputDevice = 0;
(*hostApi)->info.defaultOutputDevice = 0;
}
else
{
(*hostApi)->info.defaultInputDevice = paNoDevice;
(*hostApi)->info.defaultOutputDevice = paNoDevice;
}
(*hostApi)->Terminate = Terminate;
(*hostApi)->OpenStream = OpenStream;
(*hostApi)->IsFormatSupported = IsFormatSupported;
PaUtil_InitializeStreamInterface( &asioHostApi->callbackStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable, PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &asioHostApi->blockingStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream, GetStreamReadAvailable, GetStreamWriteAvailable );
return result;
error:
if( asioHostApi )
{
if( asioHostApi->allocations )
{
PaUtil_FreeAllAllocations( asioHostApi->allocations );
PaUtil_DestroyAllocationGroup( asioHostApi->allocations );
}
delete asioHostApi->asioDrivers;
asioDrivers = 0;  
PaWinUtil_CoUninitialize( paASIO, &asioHostApi->comInitializationResult );
PaUtil_FreeMemory( asioHostApi );
}
return result;
}
static void Terminate( struct PaUtilHostApiRepresentation *hostApi )
{
PaAsioHostApiRepresentation *asioHostApi = (PaAsioHostApiRepresentation*)hostApi;
if( asioHostApi->allocations )
{
PaUtil_FreeAllAllocations( asioHostApi->allocations );
PaUtil_DestroyAllocationGroup( asioHostApi->allocations );
}
delete asioHostApi->asioDrivers;
asioDrivers = 0;  
PaWinUtil_CoUninitialize( paASIO, &asioHostApi->comInitializationResult );
PaUtil_FreeMemory( asioHostApi );
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate )
{
PaError result = paNoError;
PaAsioHostApiRepresentation *asioHostApi = (PaAsioHostApiRepresentation*)hostApi;
PaAsioDriverInfo *driverInfo = &asioHostApi->openAsioDriverInfo;
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaDeviceIndex asioDeviceIndex;
ASIOError asioError;
if( inputParameters && outputParameters )
{
if( inputParameters->device != outputParameters->device )
return paBadIODeviceCombination;
}
if( inputParameters )
{
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
if( inputSampleFormat & paCustomFormat )
return paSampleFormatNotSupported;
if( inputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
asioDeviceIndex = inputParameters->device;
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
asioDeviceIndex = outputParameters->device;
}
else
{
outputChannelCount = 0;
}
if( asioHostApi->openAsioDeviceIndex != paNoDevice
&& asioHostApi->openAsioDeviceIndex != asioDeviceIndex )
{
return paDeviceUnavailable;
}
if( asioHostApi->openAsioDeviceIndex == paNoDevice )
{
result = LoadAsioDriver( asioHostApi, asioHostApi->inheritedHostApiRep.deviceInfos[ asioDeviceIndex ]->name,
driverInfo, asioHostApi->systemSpecific );
if( result != paNoError )
return result;
}
if( inputChannelCount > 0 )
{
if( inputChannelCount > driverInfo->inputChannelCount )
{
result = paInvalidChannelCount;
goto done;
}
}
if( outputChannelCount )
{
if( outputChannelCount > driverInfo->outputChannelCount )
{
result = paInvalidChannelCount;
goto done;
}
}
asioError = ASIOCanSampleRate( sampleRate );
if( asioError == ASE_NoClock || asioError == ASE_NotPresent )
{
result = paInvalidSampleRate;
goto done;
}
done:
if( asioHostApi->openAsioDeviceIndex == paNoDevice )
{
UnloadAsioDriver();  
}
if( result == paNoError )
return paFormatIsSupported;
else
return result;
}
typedef struct PaAsioStreamBlockingState
{
int stopFlag;  
unsigned long writeBuffersRequested;  
unsigned long readFramesRequested;    
int writeBuffersRequestedFlag;  
int readFramesRequestedFlag;    
HANDLE writeBuffersReadyEvent;  
HANDLE readFramesReadyEvent;    
void *writeRingBufferData;  
void *readRingBufferData;   
PaUtilRingBuffer writeRingBuffer;  
PaUtilRingBuffer readRingBuffer;   
long writeRingBufferInitialFrames;  
const void **writeStreamBuffer;  
void **readStreamBuffer;  
PaUtilBufferProcessor bufferProcessor;  
int outputUnderflowFlag;  
int inputOverflowFlag;  
}
PaAsioStreamBlockingState;
typedef struct PaAsioStream
{
PaUtilStreamRepresentation streamRepresentation;
PaUtilCpuLoadMeasurer cpuLoadMeasurer;
PaUtilBufferProcessor bufferProcessor;
PaAsioHostApiRepresentation *asioHostApi;
unsigned long framesPerHostCallback;
ASIOBufferInfo *asioBufferInfos;
ASIOChannelInfo *asioChannelInfos;
long asioInputLatencyFrames, asioOutputLatencyFrames;  
long inputChannelCount, outputChannelCount;
bool postOutput;
void **bufferPtrs;  
void **inputBufferPtrs[2];
void **outputBufferPtrs[2];
PaAsioBufferConverter *inputBufferConverter;
long inputShift;
PaAsioBufferConverter *outputBufferConverter;
long outputShift;
volatile bool stopProcessing;
int stopPlayoutCount;
HANDLE completedBuffersPlayedEvent;
bool streamFinishedCallbackCalled;
int isStopped;
volatile int isActive;
volatile bool zeroOutput;  
volatile long reenterCount;
volatile long reenterError;
PaStreamCallbackFlags callbackFlags;
PaAsioStreamBlockingState *blockingState;  
}
PaAsioStream;
static PaAsioStream *theAsioStream = 0;  
static void ZeroOutputBuffers( PaAsioStream *stream, long index )
{
int i;
for( i=0; i < stream->outputChannelCount; ++i )
{
void *buffer = stream->asioBufferInfos[ i + stream->inputChannelCount ].buffers[index];
int bytesPerSample = BytesPerAsioSample( stream->asioChannelInfos[ i + stream->inputChannelCount ].type );
memset( buffer, 0, stream->framesPerHostCallback * bytesPerSample );
}
}
static unsigned long NextPowerOfTwo( unsigned long x )
{
--x;
x |= x >> 1;
x |= x >> 2;
x |= x >> 4;
x |= x >> 8;
x |= x >> 16;
return x + 1;
}
static unsigned long SelectHostBufferSizeForUnspecifiedUserFramesPerBuffer(
unsigned long targetBufferingLatencyFrames, PaAsioDriverInfo *driverInfo )
{
unsigned long result;
if( targetBufferingLatencyFrames <= (unsigned long)driverInfo->bufferMinSize )
{
result = driverInfo->bufferMinSize;
}
else if( targetBufferingLatencyFrames >= (unsigned long)driverInfo->bufferMaxSize )
{
result = driverInfo->bufferMaxSize;
}
else
{
if( driverInfo->bufferGranularity == 0 )  
{
result = driverInfo->bufferPreferredSize;
}
else if( driverInfo->bufferGranularity == -1 )  
{
result = NextPowerOfTwo( targetBufferingLatencyFrames );
if( result < (unsigned long)driverInfo->bufferMinSize )
result = driverInfo->bufferMinSize;
if( result > (unsigned long)driverInfo->bufferMaxSize )
result = driverInfo->bufferMaxSize;
}
else  
{
unsigned long n = (targetBufferingLatencyFrames + driverInfo->bufferGranularity - 1)
/ driverInfo->bufferGranularity;
result = n * driverInfo->bufferGranularity;
if( result < (unsigned long)driverInfo->bufferMinSize )
result = driverInfo->bufferMinSize;
if( result > (unsigned long)driverInfo->bufferMaxSize )
result = driverInfo->bufferMaxSize;
}
}
return result;
}
static unsigned long SelectHostBufferSizeForSpecifiedUserFramesPerBuffer(
unsigned long targetBufferingLatencyFrames, unsigned long userFramesPerBuffer,
PaAsioDriverInfo *driverInfo )
{
unsigned long result = 0;
assert( userFramesPerBuffer != 0 );
if( driverInfo->bufferGranularity == 0 )  
{
if( (driverInfo->bufferPreferredSize % userFramesPerBuffer) == 0 )
result = driverInfo->bufferPreferredSize;
}
else if( driverInfo->bufferGranularity == -1 )  
{
unsigned long x = (unsigned long)driverInfo->bufferMinSize;
do {
if( (x % userFramesPerBuffer) == 0 )
{
result = x;
if( result >= targetBufferingLatencyFrames )
break;  
}
x *= 2;
} while( x <= (unsigned long)driverInfo->bufferMaxSize );
}
else  
{
unsigned long x = (unsigned long)driverInfo->bufferMinSize;
do {
if( (x % userFramesPerBuffer) == 0 )
{
result = x;
if( result >= targetBufferingLatencyFrames )
break;  
}
x += driverInfo->bufferGranularity;
} while( x <= (unsigned long)driverInfo->bufferMaxSize );
}
return result;
}
static unsigned long SelectHostBufferSize(
unsigned long targetBufferingLatencyFrames,
unsigned long userFramesPerBuffer, PaAsioDriverInfo *driverInfo )
{
unsigned long result = 0;
if( userFramesPerBuffer != 0 )
{
result = SelectHostBufferSizeForSpecifiedUserFramesPerBuffer(
targetBufferingLatencyFrames, userFramesPerBuffer, driverInfo );
}
if( result == 0 )
{
result = SelectHostBufferSizeForUnspecifiedUserFramesPerBuffer(
targetBufferingLatencyFrames, driverInfo );
}
return result;
}
static PaError ValidateAsioSpecificStreamInfo(
const PaStreamParameters *streamParameters,
const PaAsioStreamInfo *streamInfo,
int deviceChannelCount,
int **channelSelectors )
{
if( streamInfo )
{
if( streamInfo->size != sizeof( PaAsioStreamInfo )
|| streamInfo->version != 1 )
{
return paIncompatibleHostApiSpecificStreamInfo;
}
if( streamInfo->flags & paAsioUseChannelSelectors )
*channelSelectors = streamInfo->channelSelectors;
if( !(*channelSelectors) )
return paIncompatibleHostApiSpecificStreamInfo;
for( int i=0; i < streamParameters->channelCount; ++i ){
if( (*channelSelectors)[i] < 0
|| (*channelSelectors)[i] >= deviceChannelCount ){
return paInvalidChannelCount;
}
}
}
return paNoError;
}
static bool IsUsingExternalClockSource()
{
bool result = false;
ASIOError asioError;
ASIOClockSource clocks[32];
long numSources=32;
asioError = ASIOGetClockSources(clocks, &numSources);
if( asioError != ASE_OK ){
PA_DEBUG(("ERROR: ASIOGetClockSources: %s\n", PaAsio_GetAsioErrorText(asioError) ));
}else{
PA_DEBUG(("INFO ASIOGetClockSources listing %d clocks\n", numSources ));
for (int i=0;i<numSources;++i){
PA_DEBUG(("ASIOClockSource%d %s current:%d\n", i, clocks[i].name, clocks[i].isCurrentSource ));
if (clocks[i].isCurrentSource)
result = true;
}
}
return result;
}
static PaError ValidateAndSetSampleRate( double sampleRate )
{
PaError result = paNoError;
ASIOError asioError;
asioError = ASIOCanSampleRate( sampleRate );
PA_DEBUG(("ASIOCanSampleRate(%f):%d\n", sampleRate, asioError ));
if( asioError != ASE_OK )
{
result = paInvalidSampleRate;
PA_DEBUG(("ERROR: ASIOCanSampleRate: %s\n", PaAsio_GetAsioErrorText(asioError) ));
goto error;
}
ASIOSampleRate oldRate;
asioError = ASIOGetSampleRate(&oldRate);
if( asioError != ASE_OK )
{
result = paInvalidSampleRate;
PA_DEBUG(("ERROR: ASIOGetSampleRate: %s\n", PaAsio_GetAsioErrorText(asioError) ));
goto error;
}
PA_DEBUG(("ASIOGetSampleRate:%f\n",oldRate));
if (oldRate != sampleRate){
PA_DEBUG(("before ASIOSetSampleRate(%f)\n",sampleRate));
if( false ){
asioError = ASIOSetSampleRate( 0 );
}else{
asioError = ASIOSetSampleRate( sampleRate );
}
if( asioError != ASE_OK )
{
result = paInvalidSampleRate;
PA_DEBUG(("ERROR: ASIOSetSampleRate: %s\n", PaAsio_GetAsioErrorText(asioError) ));
goto error;
}
PA_DEBUG(("after ASIOSetSampleRate(%f)\n",sampleRate));
}
else
{
PA_DEBUG(("No Need to change SR\n"));
}
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
PaAsioHostApiRepresentation *asioHostApi = (PaAsioHostApiRepresentation*)hostApi;
PaAsioStream *stream = 0;
PaAsioStreamInfo *inputStreamInfo, *outputStreamInfo;
unsigned long framesPerHostBuffer;
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaSampleFormat hostInputSampleFormat, hostOutputSampleFormat;
unsigned long suggestedInputLatencyFrames;
unsigned long suggestedOutputLatencyFrames;
PaDeviceIndex asioDeviceIndex;
ASIOError asioError;
int asioIsInitialized = 0;
int asioBuffersCreated = 0;
int completedBuffersPlayedEventInited = 0;
int i;
PaAsioDriverInfo *driverInfo;
int *inputChannelSelectors = 0;
int *outputChannelSelectors = 0;
int usingBlockingIo = ( !streamCallback ) ? TRUE : FALSE;
long lBlockingBufferSize     = 0;  
long lBlockingBufferSizePow2 = 0;  
long lBytesPerFrame          = 0;  
int blockingWriteBuffersReadyEventInitialized = 0;  
int blockingReadFramesReadyEventInitialized   = 0;  
int callbackBufferProcessorInited = FALSE;
int blockingBufferProcessorInited = FALSE;
if( asioHostApi->openAsioDeviceIndex != paNoDevice )
{
PA_DEBUG(("OpenStream paDeviceUnavailable\n"));
return paDeviceUnavailable;
}
assert( theAsioStream == 0 );
if( inputParameters && outputParameters )
{
if( inputParameters->device != outputParameters->device )
{
PA_DEBUG(("OpenStream paBadIODeviceCombination\n"));
return paBadIODeviceCombination;
}
}
if( inputParameters )
{
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
suggestedInputLatencyFrames = (unsigned long)((inputParameters->suggestedLatency * sampleRate)+0.5f);
if( inputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
asioDeviceIndex = inputParameters->device;
PaAsioDeviceInfo *asioDeviceInfo = (PaAsioDeviceInfo*)hostApi->deviceInfos[asioDeviceIndex];
inputStreamInfo = (PaAsioStreamInfo*)inputParameters->hostApiSpecificStreamInfo;
result = ValidateAsioSpecificStreamInfo( inputParameters, inputStreamInfo,
asioDeviceInfo->commonDeviceInfo.maxInputChannels,
&inputChannelSelectors
);
if( result != paNoError ) return result;
}
else
{
inputChannelCount = 0;
inputSampleFormat = 0;
suggestedInputLatencyFrames = 0;
}
if( outputParameters )
{
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
suggestedOutputLatencyFrames = (unsigned long)((outputParameters->suggestedLatency * sampleRate)+0.5f);
if( outputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
asioDeviceIndex = outputParameters->device;
PaAsioDeviceInfo *asioDeviceInfo = (PaAsioDeviceInfo*)hostApi->deviceInfos[asioDeviceIndex];
outputStreamInfo = (PaAsioStreamInfo*)outputParameters->hostApiSpecificStreamInfo;
result = ValidateAsioSpecificStreamInfo( outputParameters, outputStreamInfo,
asioDeviceInfo->commonDeviceInfo.maxOutputChannels,
&outputChannelSelectors
);
if( result != paNoError ) return result;
}
else
{
outputChannelCount = 0;
outputSampleFormat = 0;
suggestedOutputLatencyFrames = 0;
}
driverInfo = &asioHostApi->openAsioDriverInfo;
result = LoadAsioDriver( asioHostApi, asioHostApi->inheritedHostApiRep.deviceInfos[ asioDeviceIndex ]->name,
driverInfo, asioHostApi->systemSpecific );
if( result == paNoError )
asioIsInitialized = 1;
else{
PA_DEBUG(("OpenStream ERROR1 - LoadAsioDriver returned %d\n", result));
goto error;
}
if( inputChannelCount > 0 )
{
if( inputChannelCount > driverInfo->inputChannelCount )
{
result = paInvalidChannelCount;
PA_DEBUG(("OpenStream ERROR2\n"));
goto error;
}
}
if( outputChannelCount )
{
if( outputChannelCount > driverInfo->outputChannelCount )
{
result = paInvalidChannelCount;
PA_DEBUG(("OpenStream ERROR3\n"));
goto error;
}
}
result = ValidateAndSetSampleRate( sampleRate );
if( result != paNoError )
goto error;
if( (streamFlags & paPlatformSpecificFlags) != 0 ){
PA_DEBUG(("OpenStream invalid flags!!\n"));
return paInvalidFlag;  
}
stream = (PaAsioStream*)PaUtil_AllocateMemory( sizeof(PaAsioStream) );
if( !stream )
{
result = paInsufficientMemory;
PA_DEBUG(("OpenStream ERROR5\n"));
goto error;
}
stream->blockingState = NULL;  
stream->completedBuffersPlayedEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
if( stream->completedBuffersPlayedEvent == NULL )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
PA_DEBUG(("OpenStream ERROR6\n"));
goto error;
}
completedBuffersPlayedEventInited = 1;
stream->asioBufferInfos = 0;  
stream->asioChannelInfos = 0;  
stream->bufferPtrs = 0;  
if( usingBlockingIo )
{
streamCallback = BlockingIoPaCallback;  
userData       = &theAsioStream;        
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&asioHostApi->blockingStreamInterface, streamCallback, userData );
}
else  
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&asioHostApi->callbackStreamInterface, streamCallback, userData );
}
PaUtil_InitializeCpuLoadMeasurer( &stream->cpuLoadMeasurer, sampleRate );
stream->asioBufferInfos = (ASIOBufferInfo*)PaUtil_AllocateMemory(
sizeof(ASIOBufferInfo) * (inputChannelCount + outputChannelCount) );
if( !stream->asioBufferInfos )
{
result = paInsufficientMemory;
PA_DEBUG(("OpenStream ERROR7\n"));
goto error;
}
for( i=0; i < inputChannelCount; ++i )
{
ASIOBufferInfo *info = &stream->asioBufferInfos[i];
info->isInput = ASIOTrue;
if( inputChannelSelectors ){
info->channelNum = inputChannelSelectors[i];
}else{
info->channelNum = i;
}
info->buffers[0] = info->buffers[1] = 0;
}
for( i=0; i < outputChannelCount; ++i ){
ASIOBufferInfo *info = &stream->asioBufferInfos[inputChannelCount+i];
info->isInput = ASIOFalse;
if( outputChannelSelectors ){
info->channelNum = outputChannelSelectors[i];
}else{
info->channelNum = i;
}
info->buffers[0] = info->buffers[1] = 0;
}
if( usingBlockingIo )
{
framesPerHostBuffer = SelectHostBufferSize( 0, framesPerBuffer, driverInfo );
}
else  
{
unsigned long targetBufferingLatencyFrames =
(( suggestedInputLatencyFrames > suggestedOutputLatencyFrames )
? suggestedInputLatencyFrames
: suggestedOutputLatencyFrames);
framesPerHostBuffer = SelectHostBufferSize( targetBufferingLatencyFrames,
framesPerBuffer, driverInfo );
}
PA_DEBUG(("PaAsioOpenStream: framesPerHostBuffer :%d\n",  framesPerHostBuffer));
asioError = ASIOCreateBuffers( stream->asioBufferInfos,
inputChannelCount+outputChannelCount,
framesPerHostBuffer, &asioCallbacks_ );
if( asioError != ASE_OK
&& framesPerHostBuffer != (unsigned long)driverInfo->bufferPreferredSize )
{
PA_DEBUG(("ERROR: ASIOCreateBuffers: %s\n", PaAsio_GetAsioErrorText(asioError) ));
framesPerHostBuffer = driverInfo->bufferPreferredSize;
PA_DEBUG(("PaAsioOpenStream: CORRECTED framesPerHostBuffer :%d\n",  framesPerHostBuffer));
ASIOError asioError2 = ASIOCreateBuffers( stream->asioBufferInfos,
inputChannelCount+outputChannelCount,
framesPerHostBuffer, &asioCallbacks_ );
if( asioError2 == ASE_OK )
asioError = ASE_OK;
}
if( asioError != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
PA_DEBUG(("OpenStream ERROR9\n"));
goto error;
}
asioBuffersCreated = 1;
stream->asioChannelInfos = (ASIOChannelInfo*)PaUtil_AllocateMemory(
sizeof(ASIOChannelInfo) * (inputChannelCount + outputChannelCount) );
if( !stream->asioChannelInfos )
{
result = paInsufficientMemory;
PA_DEBUG(("OpenStream ERROR10\n"));
goto error;
}
for( i=0; i < inputChannelCount + outputChannelCount; ++i )
{
stream->asioChannelInfos[i].channel = stream->asioBufferInfos[i].channelNum;
stream->asioChannelInfos[i].isInput = stream->asioBufferInfos[i].isInput;
asioError = ASIOGetChannelInfo( &stream->asioChannelInfos[i] );
if( asioError != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
PA_DEBUG(("OpenStream ERROR11\n"));
goto error;
}
}
stream->bufferPtrs = (void**)PaUtil_AllocateMemory(
2 * sizeof(void*) * (inputChannelCount + outputChannelCount) );
if( !stream->bufferPtrs )
{
result = paInsufficientMemory;
PA_DEBUG(("OpenStream ERROR12\n"));
goto error;
}
if( inputChannelCount > 0 )
{
stream->inputBufferPtrs[0] = stream-> bufferPtrs;
stream->inputBufferPtrs[1] = &stream->bufferPtrs[inputChannelCount];
for( i=0; i<inputChannelCount; ++i )
{
stream->inputBufferPtrs[0][i] = stream->asioBufferInfos[i].buffers[0];
stream->inputBufferPtrs[1][i] = stream->asioBufferInfos[i].buffers[1];
}
}
else
{
stream->inputBufferPtrs[0] = 0;
stream->inputBufferPtrs[1] = 0;
}
if( outputChannelCount > 0 )
{
stream->outputBufferPtrs[0] = &stream->bufferPtrs[inputChannelCount*2];
stream->outputBufferPtrs[1] = &stream->bufferPtrs[inputChannelCount*2 + outputChannelCount];
for( i=0; i<outputChannelCount; ++i )
{
stream->outputBufferPtrs[0][i] = stream->asioBufferInfos[inputChannelCount+i].buffers[0];
stream->outputBufferPtrs[1][i] = stream->asioBufferInfos[inputChannelCount+i].buffers[1];
}
}
else
{
stream->outputBufferPtrs[0] = 0;
stream->outputBufferPtrs[1] = 0;
}
if( inputChannelCount > 0 )
{
ASIOSampleType inputType = stream->asioChannelInfos[0].type;
PA_DEBUG(("ASIO Input  type:%d",inputType));
AsioSampleTypeLOG(inputType);
hostInputSampleFormat = AsioSampleTypeToPaNativeSampleFormat( inputType );
SelectAsioToPaConverter( inputType, &stream->inputBufferConverter, &stream->inputShift );
}
else
{
hostInputSampleFormat = 0;
stream->inputBufferConverter = 0;
}
if( outputChannelCount > 0 )
{
ASIOSampleType outputType = stream->asioChannelInfos[inputChannelCount].type;
PA_DEBUG(("ASIO Output type:%d",outputType));
AsioSampleTypeLOG(outputType);
hostOutputSampleFormat = AsioSampleTypeToPaNativeSampleFormat( outputType );
SelectPaToAsioConverter( outputType, &stream->outputBufferConverter, &stream->outputShift );
}
else
{
hostOutputSampleFormat = 0;
stream->outputBufferConverter = 0;
}
ASIOGetLatencies( &stream->asioInputLatencyFrames, &stream->asioOutputLatencyFrames );
if( usingBlockingIo )
{
stream->blockingState = (PaAsioStreamBlockingState*)PaUtil_AllocateMemory( sizeof(PaAsioStreamBlockingState) );
if( !stream->blockingState )
{
result = paInsufficientMemory;
PA_DEBUG(("ERROR! Blocking i/o interface struct allocation failed in OpenStream()\n"));
goto error;
}
stream->blockingState->readFramesReadyEvent   = NULL;  
stream->blockingState->writeBuffersReadyEvent = NULL;  
stream->blockingState->readRingBufferData     = NULL;  
stream->blockingState->writeRingBufferData    = NULL;  
stream->blockingState->readStreamBuffer       = NULL;  
stream->blockingState->writeStreamBuffer      = NULL;  
stream->blockingState->stopFlag               = TRUE;  
if( framesPerBuffer == paFramesPerBufferUnspecified )
{
framesPerBuffer = framesPerHostBuffer;
}
result = PaUtil_InitializeBufferProcessor( &stream->bufferProcessor               ,
inputChannelCount                     ,
inputSampleFormat & ~paNonInterleaved ,  
(hostInputSampleFormat | paNonInterleaved),  
outputChannelCount                    ,
outputSampleFormat & ~paNonInterleaved,  
(hostOutputSampleFormat | paNonInterleaved),  
sampleRate                            ,
streamFlags                           ,
framesPerBuffer                       ,  
framesPerHostBuffer                   ,  
paUtilFixedHostBufferSize             ,
streamCallback                        ,
userData                               );
if( result != paNoError ){
PA_DEBUG(("OpenStream ERROR13\n"));
goto error;
}
callbackBufferProcessorInited = TRUE;
result = PaUtil_InitializeBufferProcessor(&stream->blockingState->bufferProcessor,
inputChannelCount                     ,
inputSampleFormat                     ,  
inputSampleFormat & ~paNonInterleaved ,  
outputChannelCount                    ,
outputSampleFormat                    ,  
outputSampleFormat & ~paNonInterleaved,  
sampleRate                            ,
paClipOff | paDitherOff               ,  
framesPerBuffer                       ,  
framesPerBuffer                       ,  
paUtilBoundedHostBufferSize           ,
NULL, NULL                            ); 
if( result != paNoError ){
PA_DEBUG(("ERROR! Blocking i/o buffer processor initialization failed in OpenStream()\n"));
goto error;
}
blockingBufferProcessorInited = TRUE;
if( inputChannelCount )
{
stream->blockingState->readFramesReadyEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if( stream->blockingState->readFramesReadyEvent == NULL )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
PA_DEBUG(("ERROR! Blocking i/o \"read frames ready\" event creation failed in OpenStream()\n"));
goto error;
}
blockingReadFramesReadyEventInitialized = 1;
stream->blockingState->readStreamBuffer = (void**)PaUtil_AllocateMemory( sizeof(void*) * inputChannelCount );
if( !stream->blockingState->readStreamBuffer )
{
result = paInsufficientMemory;
PA_DEBUG(("ERROR! Blocking i/o read stream buffer allocation failed in OpenStream()\n"));
goto error;
}
lBlockingBufferSize = suggestedInputLatencyFrames - stream->asioInputLatencyFrames;
lBlockingBufferSize = (lBlockingBufferSize > 0) ? lBlockingBufferSize : 1;
lBlockingBufferSize = (lBlockingBufferSize + framesPerBuffer - 1) / framesPerBuffer;
lBlockingBufferSize = (lBlockingBufferSize + 1) * framesPerBuffer;
lBlockingBufferSizePow2 = 1;
while( lBlockingBufferSize > (lBlockingBufferSizePow2<<=1) );
lBlockingBufferSize = lBlockingBufferSizePow2;
stream->streamRepresentation.streamInfo.inputLatency =
(double)( PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor               )
+ PaUtil_GetBufferProcessorInputLatencyFrames(&stream->blockingState->bufferProcessor)
+ (lBlockingBufferSize / framesPerBuffer - 1) * framesPerBuffer
+ stream->asioInputLatencyFrames )
/ sampleRate;
PA_DEBUG(("PaAsio : ASIO InputLatency = %ld (%ld ms),\n         added buffProc:%ld (%ld ms),\n         added blocking:%ld (%ld ms)\n",
stream->asioInputLatencyFrames,
(long)( stream->asioInputLatencyFrames * (1000.0 / sampleRate) ),
PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor),
(long)( PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor) * (1000.0 / sampleRate) ),
PaUtil_GetBufferProcessorInputLatencyFrames(&stream->blockingState->bufferProcessor) + (lBlockingBufferSize / framesPerBuffer - 1) * framesPerBuffer,
(long)( (PaUtil_GetBufferProcessorInputLatencyFrames(&stream->blockingState->bufferProcessor) + (lBlockingBufferSize / framesPerBuffer - 1) * framesPerBuffer) * (1000.0 / sampleRate) )
));
lBytesPerFrame = inputChannelCount * Pa_GetSampleSize(inputSampleFormat );
stream->blockingState->readRingBufferData = (void*)PaUtil_AllocateMemory( lBlockingBufferSize * lBytesPerFrame );
if( !stream->blockingState->readRingBufferData )
{
result = paInsufficientMemory;
PA_DEBUG(("ERROR! Blocking i/o input ring buffer allocation failed in OpenStream()\n"));
goto error;
}
PaUtil_InitializeRingBuffer( &stream->blockingState->readRingBuffer    ,
lBytesPerFrame                           ,
lBlockingBufferSize                      ,
stream->blockingState->readRingBufferData );
}
if( outputChannelCount )
{
stream->blockingState->writeBuffersReadyEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if( stream->blockingState->writeBuffersReadyEvent == NULL )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
PA_DEBUG(("ERROR! Blocking i/o \"write buffers ready\" event creation failed in OpenStream()\n"));
goto error;
}
blockingWriteBuffersReadyEventInitialized = 1;
stream->blockingState->writeStreamBuffer = (const void**)PaUtil_AllocateMemory( sizeof(const void*) * outputChannelCount );
if( !stream->blockingState->writeStreamBuffer )
{
result = paInsufficientMemory;
PA_DEBUG(("ERROR! Blocking i/o write stream buffer allocation failed in OpenStream()\n"));
goto error;
}
lBlockingBufferSize = suggestedOutputLatencyFrames - stream->asioOutputLatencyFrames;
lBlockingBufferSize = (lBlockingBufferSize > 0) ? lBlockingBufferSize : 1;
lBlockingBufferSize = (lBlockingBufferSize + framesPerBuffer - 1) / framesPerBuffer;
lBlockingBufferSize = (lBlockingBufferSize + 1) * framesPerBuffer;
stream->blockingState->writeRingBufferInitialFrames = lBlockingBufferSize - framesPerBuffer;
lBlockingBufferSizePow2 = 1;
while( lBlockingBufferSize > (lBlockingBufferSizePow2<<=1) );
lBlockingBufferSize = lBlockingBufferSizePow2;
stream->streamRepresentation.streamInfo.outputLatency =
(double)( PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor)
+ PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->blockingState->bufferProcessor)
+ (lBlockingBufferSize / framesPerBuffer - 1) * framesPerBuffer
+ stream->asioOutputLatencyFrames )
/ sampleRate;
PA_DEBUG(("PaAsio : ASIO OutputLatency = %ld (%ld ms),\n         added buffProc:%ld (%ld ms),\n         added blocking:%ld (%ld ms)\n",
stream->asioOutputLatencyFrames,
(long)( stream->asioOutputLatencyFrames * (1000.0 / sampleRate) ),
PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor),
(long)( PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor) * (1000.0 / sampleRate) ),
PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->blockingState->bufferProcessor) + (lBlockingBufferSize / framesPerBuffer - 1) * framesPerBuffer,
(long)( (PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->blockingState->bufferProcessor) + (lBlockingBufferSize / framesPerBuffer - 1) * framesPerBuffer) * (1000.0 / sampleRate) )
));
lBytesPerFrame = outputChannelCount * Pa_GetSampleSize(outputSampleFormat);
stream->blockingState->writeRingBufferData = (void*)PaUtil_AllocateMemory( lBlockingBufferSize * lBytesPerFrame );
if( !stream->blockingState->writeRingBufferData )
{
result = paInsufficientMemory;
PA_DEBUG(("ERROR! Blocking i/o output ring buffer allocation failed in OpenStream()\n"));
goto error;
}
PaUtil_InitializeRingBuffer( &stream->blockingState->writeRingBuffer    ,
lBytesPerFrame                            ,
lBlockingBufferSize                       ,
stream->blockingState->writeRingBufferData );
}
stream->streamRepresentation.streamInfo.sampleRate = sampleRate;
}
else  
{
result =  PaUtil_InitializeBufferProcessor( &stream->bufferProcessor,
inputChannelCount, inputSampleFormat, (hostInputSampleFormat | paNonInterleaved),
outputChannelCount, outputSampleFormat, (hostOutputSampleFormat | paNonInterleaved),
sampleRate, streamFlags, framesPerBuffer,
framesPerHostBuffer, paUtilFixedHostBufferSize,
streamCallback, userData );
if( result != paNoError ){
PA_DEBUG(("OpenStream ERROR13\n"));
goto error;
}
callbackBufferProcessorInited = TRUE;
stream->streamRepresentation.streamInfo.inputLatency =
(double)( PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor)
+ stream->asioInputLatencyFrames) / sampleRate;    
stream->streamRepresentation.streamInfo.outputLatency =
(double)( PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor)
+ stream->asioOutputLatencyFrames) / sampleRate;  
stream->streamRepresentation.streamInfo.sampleRate = sampleRate;
PA_DEBUG(("PaAsio : ASIO InputLatency = %ld (%ld ms), added buffProc:%ld (%ld ms)\n",
stream->asioInputLatencyFrames,
(long)((stream->asioInputLatencyFrames*1000)/ sampleRate),
PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor),
(long)((PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor)*1000)/ sampleRate)
));
PA_DEBUG(("PaAsio : ASIO OuputLatency = %ld (%ld ms), added buffProc:%ld (%ld ms)\n",
stream->asioOutputLatencyFrames,
(long)((stream->asioOutputLatencyFrames*1000)/ sampleRate),
PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor),
(long)((PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor)*1000)/ sampleRate)
));
}
stream->asioHostApi = asioHostApi;
stream->framesPerHostCallback = framesPerHostBuffer;
stream->inputChannelCount = inputChannelCount;
stream->outputChannelCount = outputChannelCount;
stream->postOutput = driverInfo->postOutput;
stream->isStopped = 1;
stream->isActive = 0;
asioHostApi->openAsioDeviceIndex = asioDeviceIndex;
theAsioStream = stream;
*s = (PaStream*)stream;
return result;
error:
PA_DEBUG(("goto errored\n"));
if( stream )
{
if( stream->blockingState )
{
if( blockingBufferProcessorInited )
PaUtil_TerminateBufferProcessor( &stream->blockingState->bufferProcessor );
if( stream->blockingState->writeRingBufferData )
PaUtil_FreeMemory( stream->blockingState->writeRingBufferData );
if( stream->blockingState->writeStreamBuffer )
PaUtil_FreeMemory( stream->blockingState->writeStreamBuffer );
if( blockingWriteBuffersReadyEventInitialized )
CloseHandle( stream->blockingState->writeBuffersReadyEvent );
if( stream->blockingState->readRingBufferData )
PaUtil_FreeMemory( stream->blockingState->readRingBufferData );
if( stream->blockingState->readStreamBuffer )
PaUtil_FreeMemory( stream->blockingState->readStreamBuffer );
if( blockingReadFramesReadyEventInitialized )
CloseHandle( stream->blockingState->readFramesReadyEvent );
PaUtil_FreeMemory( stream->blockingState );
}
if( callbackBufferProcessorInited )
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
if( completedBuffersPlayedEventInited )
CloseHandle( stream->completedBuffersPlayedEvent );
if( stream->asioBufferInfos )
PaUtil_FreeMemory( stream->asioBufferInfos );
if( stream->asioChannelInfos )
PaUtil_FreeMemory( stream->asioChannelInfos );
if( stream->bufferPtrs )
PaUtil_FreeMemory( stream->bufferPtrs );
PaUtil_FreeMemory( stream );
}
if( asioBuffersCreated )
ASIODisposeBuffers();
if( asioIsInitialized )
{
UnloadAsioDriver();
}
return result;
}
static PaError CloseStream( PaStream* s )
{
PaError result = paNoError;
PaAsioStream *stream = (PaAsioStream*)s;
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
stream->asioHostApi->openAsioDeviceIndex = paNoDevice;
CloseHandle( stream->completedBuffersPlayedEvent );
if( stream->blockingState )
{
PaUtil_TerminateBufferProcessor( &stream->blockingState->bufferProcessor );
if( stream->inputChannelCount ) {
PaUtil_FreeMemory( stream->blockingState->readRingBufferData );
PaUtil_FreeMemory( stream->blockingState->readStreamBuffer  );
CloseHandle( stream->blockingState->readFramesReadyEvent );
}
if( stream->outputChannelCount ) {
PaUtil_FreeMemory( stream->blockingState->writeRingBufferData );
PaUtil_FreeMemory( stream->blockingState->writeStreamBuffer );
CloseHandle( stream->blockingState->writeBuffersReadyEvent );
}
PaUtil_FreeMemory( stream->blockingState );
}
PaUtil_FreeMemory( stream->asioBufferInfos );
PaUtil_FreeMemory( stream->asioChannelInfos );
PaUtil_FreeMemory( stream->bufferPtrs );
PaUtil_FreeMemory( stream );
ASIODisposeBuffers();
UnloadAsioDriver();
theAsioStream = 0;
return result;
}
static void bufferSwitch(long index, ASIOBool directProcess)
{
ASIOTime  timeInfo;
memset( &timeInfo, 0, sizeof (timeInfo) );
if( ASIOGetSamplePosition(&timeInfo.timeInfo.samplePosition, &timeInfo.timeInfo.systemTime) == ASE_OK)
timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
bufferSwitchTimeInfo( &timeInfo, index, directProcess );
}
#if NATIVE_INT64
#define ASIO64toDouble(a)  (a)
#else
const double twoRaisedTo32 = 4294967296.;
#define ASIO64toDouble(a)  ((a).lo + (a).hi * twoRaisedTo32)
#endif
static ASIOTime *bufferSwitchTimeInfo( ASIOTime *timeInfo, long index, ASIOBool directProcess )
{
(void) directProcess;  
#if 0
asioDriverInfo.tInfo = *timeInfo;
if (timeInfo->timeInfo.flags & kSystemTimeValid)
asioDriverInfo.nanoSeconds = ASIO64toDouble(timeInfo->timeInfo.systemTime);
else
asioDriverInfo.nanoSeconds = 0;
if (timeInfo->timeInfo.flags & kSamplePositionValid)
asioDriverInfo.samples = ASIO64toDouble(timeInfo->timeInfo.samplePosition);
else
asioDriverInfo.samples = 0;
if (timeInfo->timeCode.flags & kTcValid)
asioDriverInfo.tcSamples = ASIO64toDouble(timeInfo->timeCode.timeCodeSamples);
else
asioDriverInfo.tcSamples = 0;
asioDriverInfo.sysRefTime = get_sys_reference_time();
#endif
#if 0
static double last_samples = 0;
char tmp[128];
sprintf (tmp, "diff: %d / %d ms / %d ms / %d samples                 \n", asioDriverInfo.sysRefTime - (long)(asioDriverInfo.nanoSeconds / 1000000.0), asioDriverInfo.sysRefTime, (long)(asioDriverInfo.nanoSeconds / 1000000.0), (long)(asioDriverInfo.samples - last_samples));
OutputDebugString (tmp);
last_samples = asioDriverInfo.samples;
#endif
if( !theAsioStream )
return 0L;
if( PaAsio_AtomicIncrement(&theAsioStream->reenterCount) )
{
theAsioStream->reenterError++;
return 0L;
}
int buffersDone = 0;
do
{
if( buffersDone > 0 )
{
if( theAsioStream->inputChannelCount > 0 )
theAsioStream->callbackFlags |= paInputOverflow;
if( theAsioStream->outputChannelCount > 0 )
theAsioStream->callbackFlags |= paOutputUnderflow;
}
else
{
if( theAsioStream->zeroOutput )
{
ZeroOutputBuffers( theAsioStream, index );
if( theAsioStream->postOutput )
ASIOOutputReady();
if( theAsioStream->stopProcessing )
{
if( theAsioStream->stopPlayoutCount < 2 )
{
++theAsioStream->stopPlayoutCount;
if( theAsioStream->stopPlayoutCount == 2 )
{
theAsioStream->isActive = 0;
if( theAsioStream->streamRepresentation.streamFinishedCallback != 0 )
theAsioStream->streamRepresentation.streamFinishedCallback( theAsioStream->streamRepresentation.userData );
theAsioStream->streamFinishedCallbackCalled = true;
SetEvent( theAsioStream->completedBuffersPlayedEvent );
}
}
}
}
else
{
#if 0
static double last_samples = -512;
double samples;
samples = ASIO64toDouble(timeInfo->timeInfo.samplePosition);
int delta = samples - last_samples;
last_samples = samples;
if( delta > theAsioStream->framesPerHostCallback )
{
if( theAsioStream->inputChannelCount > 0 )
theAsioStream->callbackFlags |= paInputOverflow;
if( theAsioStream->outputChannelCount > 0 )
theAsioStream->callbackFlags |= paOutputUnderflow;
}
static int previousIndex = 1;
if( index == previousIndex )
{
if( theAsioStream->inputChannelCount > 0 )
theAsioStream->callbackFlags |= paInputOverflow;
if( theAsioStream->outputChannelCount > 0 )
theAsioStream->callbackFlags |= paOutputUnderflow;
}
previousIndex = index;
#endif
int i;
PaUtil_BeginCpuLoadMeasurement( &theAsioStream->cpuLoadMeasurer );
PaStreamCallbackTimeInfo paTimeInfo;
paTimeInfo.currentTime = (ASIO64toDouble( timeInfo->timeInfo.systemTime ) * .000000001);
paTimeInfo.inputBufferAdcTime = paTimeInfo.currentTime -
((double)theAsioStream->asioInputLatencyFrames/theAsioStream->streamRepresentation.streamInfo.sampleRate);
paTimeInfo.outputBufferDacTime = paTimeInfo.currentTime +
((double)theAsioStream->asioOutputLatencyFrames/theAsioStream->streamRepresentation.streamInfo.sampleRate);
#if 0
static double previousTime = -1;
if( previousTime > 0 ){
double delta = paTimeInfo.currentTime - previousTime;
if( delta >= 2. * (theAsioStream->framesPerHostCallback / theAsioStream->streamRepresentation.streamInfo.sampleRate) ){
if( theAsioStream->inputChannelCount > 0 )
theAsioStream->callbackFlags |= paInputOverflow;
if( theAsioStream->outputChannelCount > 0 )
theAsioStream->callbackFlags |= paOutputUnderflow;
}
}
previousTime = paTimeInfo.currentTime;
#endif
if( theAsioStream->inputBufferConverter )
{
for( i=0; i<theAsioStream->inputChannelCount; i++ )
{
theAsioStream->inputBufferConverter( theAsioStream->inputBufferPtrs[index][i],
theAsioStream->inputShift, theAsioStream->framesPerHostCallback );
}
}
PaUtil_BeginBufferProcessing( &theAsioStream->bufferProcessor, &paTimeInfo, theAsioStream->callbackFlags );
theAsioStream->callbackFlags = 0;
PaUtil_SetInputFrameCount( &theAsioStream->bufferProcessor, 0   );
for( i=0; i<theAsioStream->inputChannelCount; ++i )
PaUtil_SetNonInterleavedInputChannel( &theAsioStream->bufferProcessor, i, theAsioStream->inputBufferPtrs[index][i] );
PaUtil_SetOutputFrameCount( &theAsioStream->bufferProcessor, 0   );
for( i=0; i<theAsioStream->outputChannelCount; ++i )
PaUtil_SetNonInterleavedOutputChannel( &theAsioStream->bufferProcessor, i, theAsioStream->outputBufferPtrs[index][i] );
int callbackResult;
if( theAsioStream->stopProcessing )
callbackResult = paComplete;
else
callbackResult = paContinue;
unsigned long framesProcessed = PaUtil_EndBufferProcessing( &theAsioStream->bufferProcessor, &callbackResult );
if( theAsioStream->outputBufferConverter )
{
for( i=0; i<theAsioStream->outputChannelCount; i++ )
{
theAsioStream->outputBufferConverter( theAsioStream->outputBufferPtrs[index][i],
theAsioStream->outputShift, theAsioStream->framesPerHostCallback );
}
}
PaUtil_EndCpuLoadMeasurement( &theAsioStream->cpuLoadMeasurer, framesProcessed );
if( theAsioStream->postOutput )
ASIOOutputReady();
if( callbackResult == paContinue )
{
}
else if( callbackResult == paAbort )
{
theAsioStream->isActive = 0;
if( theAsioStream->streamRepresentation.streamFinishedCallback != 0 )
theAsioStream->streamRepresentation.streamFinishedCallback( theAsioStream->streamRepresentation.userData );
theAsioStream->streamFinishedCallbackCalled = true;
SetEvent( theAsioStream->completedBuffersPlayedEvent );
theAsioStream->zeroOutput = true;
}
else  
{
theAsioStream->stopProcessing = true;
if( PaUtil_IsBufferProcessorOutputEmpty( &theAsioStream->bufferProcessor ) )
{
theAsioStream->zeroOutput = true;
theAsioStream->stopPlayoutCount = 0;
}
}
}
}
++buffersDone;
}while( PaAsio_AtomicDecrement(&theAsioStream->reenterCount) >= 0 );
return 0L;
}
static void sampleRateChanged(ASIOSampleRate sRate)
{
(void) sRate;  
PA_DEBUG( ("sampleRateChanged : %d \n", sRate));
}
static long asioMessages(long selector, long value, void* message, double* opt)
{
long ret = 0;
(void) message;  
(void) opt;
PA_DEBUG( ("asioMessages : %d , %d \n", selector, value));
switch(selector)
{
case kAsioSelectorSupported:
if(value == kAsioResetRequest
|| value == kAsioEngineVersion
|| value == kAsioResyncRequest
|| value == kAsioLatenciesChanged
|| value == kAsioSupportsTimeInfo
|| value == kAsioSupportsTimeCode
|| value == kAsioSupportsInputMonitor)
ret = 1L;
break;
case kAsioBufferSizeChange:
break;
case kAsioResetRequest:
ret = 1L;
break;
case kAsioResyncRequest:
ret = 1L;
break;
case kAsioLatenciesChanged:
ret = 1L;
break;
case kAsioEngineVersion:
ret = 2L;
break;
case kAsioSupportsTimeInfo:
ret = 1;
break;
case kAsioSupportsTimeCode:
ret = 0;
break;
}
return ret;
}
static PaError StartStream( PaStream *s )
{
PaError result = paNoError;
PaAsioStream *stream = (PaAsioStream*)s;
PaAsioStreamBlockingState *blockingState = stream->blockingState;
ASIOError asioError;
if( stream->outputChannelCount > 0 )
{
ZeroOutputBuffers( stream, 0 );
ZeroOutputBuffers( stream, 1 );
}
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );
stream->stopProcessing = false;
stream->zeroOutput = false;
stream->reenterCount = -1;
stream->reenterError = 0;
stream->callbackFlags = 0;
if( ResetEvent( stream->completedBuffersPlayedEvent ) == 0 )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
}
if( blockingState )
{
PaUtil_ResetBufferProcessor( &blockingState->bufferProcessor );
if( stream->inputChannelCount )
{
if( ResetEvent( blockingState->readFramesReadyEvent ) == 0 )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
}
PaUtil_FlushRingBuffer( &blockingState->readRingBuffer );
(*blockingState->bufferProcessor.inputZeroer)( blockingState->readRingBuffer.buffer, 1, blockingState->bufferProcessor.inputChannelCount * blockingState->readRingBuffer.bufferSize );
}
if( stream->outputChannelCount )
{
if( ResetEvent( blockingState->writeBuffersReadyEvent ) == 0 )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
}
PaUtil_FlushRingBuffer( &blockingState->writeRingBuffer );
(*blockingState->bufferProcessor.outputZeroer)( blockingState->writeRingBuffer.buffer, 1, blockingState->bufferProcessor.outputChannelCount * blockingState->writeRingBuffer.bufferSize );
PaUtil_AdvanceRingBufferWriteIndex( &blockingState->writeRingBuffer, blockingState->writeRingBufferInitialFrames );
}
blockingState->writeBuffersRequested     = 0;
blockingState->readFramesRequested       = 0;
blockingState->writeBuffersRequestedFlag = FALSE;
blockingState->readFramesRequestedFlag   = FALSE;
blockingState->outputUnderflowFlag       = FALSE;
blockingState->inputOverflowFlag         = FALSE;
blockingState->stopFlag                  = FALSE;
}
if( result == paNoError )
{
assert( theAsioStream == stream );  
stream->isStopped = 0;
stream->isActive = 1;
stream->streamFinishedCallbackCalled = false;
asioError = ASIOStart();
if( asioError != ASE_OK )
{
stream->isStopped = 1;
stream->isActive = 0;
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
}
}
return result;
}
static void EnsureCallbackHasCompleted( PaAsioStream *stream )
{
int count = 2000;   
while( stream->reenterCount != -1 && count > 0 )
{
Sleep(1);
--count;
}
}
static PaError StopStream( PaStream *s )
{
PaError result = paNoError;
PaAsioStream *stream = (PaAsioStream*)s;
PaAsioStreamBlockingState *blockingState = stream->blockingState;
ASIOError asioError;
if( stream->isActive )
{
if( blockingState && stream->outputChannelCount )
{
blockingState->writeBuffersRequested = blockingState->writeRingBuffer.bufferSize;
blockingState->writeBuffersRequestedFlag = TRUE;
blockingState->stopFlag = TRUE;
DWORD timeout = (DWORD)( 2 * blockingState->writeRingBuffer.bufferSize * 1000
/ stream->streamRepresentation.streamInfo.sampleRate );
DWORD waitResult = WaitForSingleObject( blockingState->writeBuffersReadyEvent, timeout );
if( waitResult == WAIT_FAILED )
{
PA_DEBUG(("WaitForSingleObject() failed in StopStream()\n"));
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
}
else if( waitResult == WAIT_TIMEOUT )
{
PA_DEBUG(("WaitForSingleObject() timed out in StopStream()\n"));
result = paTimedOut;
}
}
stream->stopProcessing = true;
if( WaitForSingleObject( stream->completedBuffersPlayedEvent,
(DWORD)(stream->streamRepresentation.streamInfo.outputLatency * 1000. * 4.) )
== WAIT_TIMEOUT )
{
PA_DEBUG(("WaitForSingleObject() timed out in StopStream()\n" ));
}
}
asioError = ASIOStop();
if( asioError == ASE_OK )
{
EnsureCallbackHasCompleted( stream );
}
else
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
}
stream->isStopped = 1;
stream->isActive = 0;
if( !stream->streamFinishedCallbackCalled )
{
if( stream->streamRepresentation.streamFinishedCallback != 0 )
stream->streamRepresentation.streamFinishedCallback( stream->streamRepresentation.userData );
}
return result;
}
static PaError AbortStream( PaStream *s )
{
PaError result = paNoError;
PaAsioStream *stream = (PaAsioStream*)s;
ASIOError asioError;
stream->zeroOutput = true;
asioError = ASIOStop();
if( asioError == ASE_OK )
{
EnsureCallbackHasCompleted( stream );
}
else
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
}
stream->isStopped = 1;
stream->isActive = 0;
if( !stream->streamFinishedCallbackCalled )
{
if( stream->streamRepresentation.streamFinishedCallback != 0 )
stream->streamRepresentation.streamFinishedCallback( stream->streamRepresentation.userData );
}
return result;
}
static PaError IsStreamStopped( PaStream *s )
{
PaAsioStream *stream = (PaAsioStream*)s;
return stream->isStopped;
}
static PaError IsStreamActive( PaStream *s )
{
PaAsioStream *stream = (PaAsioStream*)s;
return stream->isActive;
}
static PaTime GetStreamTime( PaStream *s )
{
(void) s;  
return (double)timeGetTime() * .001;
}
static double GetStreamCpuLoad( PaStream* s )
{
PaAsioStream *stream = (PaAsioStream*)s;
return PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer );
}
static PaError ReadStream( PaStream      *s     ,
void          *buffer,
unsigned long  frames )
{
PaError result = paNoError;  
PaAsioStream *stream = (PaAsioStream*)s;  
PaAsioStreamBlockingState *blockingState = stream->blockingState;
PaUtilBufferProcessor *pBp = &blockingState->bufferProcessor;
PaUtilRingBuffer      *pRb = &blockingState->readRingBuffer;
void *pRingBufferData1st = NULL;  
void *pRingBufferData2nd = NULL;  
long lRingBufferSize1st = 0;  
long lRingBufferSize2nd = 0;  
unsigned long lFramesPerBlock = stream->bufferProcessor.framesPerUserBuffer;
unsigned long lFramesCopied = 0;
unsigned long lFramesRemaining = frames;
const void *userBuffer;
unsigned int i;  
DWORD timeout = (DWORD)( 8 * lFramesPerBlock * 1000 / stream->streamRepresentation.streamInfo.sampleRate );
DWORD waitResult = 0;
if( blockingState->stopFlag || !stream->isActive )
{
PA_DEBUG(("Warning! Stream no longer available for reading in ReadStream()\n"));
result = paStreamIsStopped;
return result;
}
if( stream->inputChannelCount )
{
if( !pBp->userOutputIsInterleaved )
{
userBuffer = blockingState->readStreamBuffer;
for( i = 0; i<pBp->inputChannelCount; ++i )
{
((void**)userBuffer)[i] = ((void**)buffer)[i];
}
}  
else { userBuffer = buffer; }
do  
{
lFramesPerBlock =(lFramesPerBlock < lFramesRemaining)
? lFramesPerBlock : lFramesRemaining;
if( PaUtil_GetRingBufferReadAvailable(pRb) < (long) lFramesPerBlock )
{
blockingState->readFramesRequested = lFramesPerBlock;
blockingState->readFramesRequestedFlag = TRUE;
waitResult = WaitForSingleObject( blockingState->readFramesReadyEvent, timeout );
if( waitResult == WAIT_FAILED )
{
PA_DEBUG(("WaitForSingleObject() failed in ReadStream()\n"));
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
return result;
}
else if( waitResult == WAIT_TIMEOUT )
{
PA_DEBUG(("WaitForSingleObject() timed out in ReadStream()\n"));
if( blockingState->stopFlag ) { return result = paStreamIsStopped; }
return result = paTimedOut;
}
}
PaUtil_GetRingBufferReadRegions(pRb                ,
lFramesPerBlock    ,
&pRingBufferData1st,
&lRingBufferSize1st,
&pRingBufferData2nd,
&lRingBufferSize2nd);
PaUtil_SetInputFrameCount( pBp, lRingBufferSize1st );
PaUtil_SetInterleavedInputChannels(pBp               ,   
0                 ,   
pRingBufferData1st,   
0                 );  
if( lRingBufferSize2nd ) {
PaUtil_Set2ndInputFrameCount( pBp, lRingBufferSize2nd );
PaUtil_Set2ndInterleavedInputChannels(pBp               ,   
0                 ,   
pRingBufferData2nd,   
0                 );  
}
lFramesCopied = PaUtil_CopyInput( pBp, &buffer, lFramesPerBlock );
PaUtil_AdvanceRingBufferReadIndex( pRb, lFramesCopied );
lFramesRemaining -= lFramesCopied;
}  
while( lFramesRemaining );
if( blockingState->inputOverflowFlag )
{
blockingState->inputOverflowFlag = FALSE;
result = paInputOverflowed;
}
}  
else {
result = paCanNotReadFromAnOutputOnlyStream;
}
return result;
}
static PaError WriteStream( PaStream      *s     ,
const void    *buffer,
unsigned long  frames )
{
PaError result = paNoError;  
PaAsioStream *stream = (PaAsioStream*)s;  
PaAsioStreamBlockingState *blockingState = stream->blockingState;
PaUtilBufferProcessor *pBp = &blockingState->bufferProcessor;
PaUtilRingBuffer      *pRb = &blockingState->writeRingBuffer;
void *pRingBufferData1st = NULL;  
void *pRingBufferData2nd = NULL;  
long lRingBufferSize1st = 0;  
long lRingBufferSize2nd = 0;  
unsigned long lFramesPerBlock = stream->bufferProcessor.framesPerUserBuffer;
unsigned long lFramesCopied = 0;
unsigned long lFramesRemaining = frames;
DWORD timeout = (DWORD)( 8 * lFramesPerBlock * 1000 / stream->streamRepresentation.streamInfo.sampleRate );
DWORD waitResult = 0;
const void *userBuffer;
unsigned int i;  
if( blockingState->stopFlag || !stream->isActive )
{
PA_DEBUG(("Warning! Stream no longer available for writing in WriteStream()\n"));
result = paStreamIsStopped;
return result;
}
if( stream->outputChannelCount )
{
if( !pBp->userOutputIsInterleaved )
{
userBuffer = blockingState->writeStreamBuffer;
for( i = 0; i<pBp->outputChannelCount; ++i )
{
((const void**)userBuffer)[i] = ((const void**)buffer)[i];
}
}  
else { userBuffer = buffer; }
do  
{
lFramesPerBlock =(lFramesPerBlock < lFramesRemaining)
? lFramesPerBlock : lFramesRemaining;
if( PaUtil_GetRingBufferWriteAvailable(pRb) < (long) lFramesPerBlock )
{
blockingState->writeBuffersRequested = lFramesPerBlock;
blockingState->writeBuffersRequestedFlag = TRUE;
waitResult = WaitForSingleObject( blockingState->writeBuffersReadyEvent, timeout );
if( waitResult == WAIT_FAILED )
{
PA_DEBUG(("WaitForSingleObject() failed in WriteStream()\n"));
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_SYSTEM_ERROR( GetLastError() );
return result;
}
else if( waitResult == WAIT_TIMEOUT )
{
PA_DEBUG(("WaitForSingleObject() timed out in WriteStream()\n"));
if( blockingState->stopFlag ) { return result = paStreamIsStopped; }
return result = paTimedOut;
}
}
PaUtil_GetRingBufferWriteRegions(pRb                ,
lFramesPerBlock    ,
&pRingBufferData1st,
&lRingBufferSize1st,
&pRingBufferData2nd,
&lRingBufferSize2nd);
PaUtil_SetOutputFrameCount( pBp, lRingBufferSize1st );
PaUtil_SetInterleavedOutputChannels(pBp               ,   
0                 ,   
pRingBufferData1st,   
0                 );  
if( lRingBufferSize2nd ) {
PaUtil_Set2ndOutputFrameCount( pBp, lRingBufferSize2nd );
PaUtil_Set2ndInterleavedOutputChannels(pBp               ,   
0                 ,   
pRingBufferData2nd,   
0                 );  
}
lFramesCopied = PaUtil_CopyOutput( pBp, &userBuffer, lFramesPerBlock );
PaUtil_AdvanceRingBufferWriteIndex( pRb, lFramesCopied );
lFramesRemaining -= lFramesCopied;
}  
while( lFramesRemaining );
if( blockingState->outputUnderflowFlag )
{
blockingState->outputUnderflowFlag = FALSE;
result = paOutputUnderflowed;
}
}  
else
{
result = paCanNotWriteToAnInputOnlyStream;
}
return result;
}
static signed long GetStreamReadAvailable( PaStream* s )
{
PaAsioStream *stream = (PaAsioStream*)s;
return PaUtil_GetRingBufferReadAvailable( &stream->blockingState->readRingBuffer );
}
static signed long GetStreamWriteAvailable( PaStream* s )
{
PaAsioStream *stream = (PaAsioStream*)s;
return PaUtil_GetRingBufferWriteAvailable( &stream->blockingState->writeRingBuffer );
}
static int BlockingIoPaCallback(const void                     *inputBuffer    ,
void                     *outputBuffer   ,
unsigned long             framesPerBuffer,
const PaStreamCallbackTimeInfo *timeInfo       ,
PaStreamCallbackFlags     statusFlags    ,
void                     *userData       )
{
PaError result = paNoError;  
PaAsioStream *stream = *(PaAsioStream**)userData;  
PaAsioStreamBlockingState *blockingState = stream->blockingState;  
PaUtilBufferProcessor *pBp = &blockingState->bufferProcessor;
PaUtilRingBuffer      *pRb = NULL;
if( stream->outputChannelCount )
{
if( statusFlags & paOutputUnderflowed ) {
blockingState->outputUnderflowFlag = TRUE;
}
pRb = &blockingState->writeRingBuffer;
if( PaUtil_GetRingBufferReadAvailable(pRb) >= (long) framesPerBuffer )
{
PaUtil_ReadRingBuffer( pRb, outputBuffer, framesPerBuffer );
}
else  
{
blockingState->outputUnderflowFlag = TRUE;
(*pBp->outputZeroer)( outputBuffer, 1, pBp->outputChannelCount * framesPerBuffer );
if( blockingState->stopFlag && PaUtil_GetRingBufferReadAvailable(pRb) < (long) framesPerBuffer )
{
PaUtil_ReadRingBuffer( pRb, outputBuffer, PaUtil_GetRingBufferReadAvailable(pRb) );
}
}
if( blockingState->writeBuffersRequestedFlag && PaUtil_GetRingBufferWriteAvailable(pRb) >= (long) blockingState->writeBuffersRequested )
{
blockingState->writeBuffersRequestedFlag = FALSE;
blockingState->writeBuffersRequested     = 0;
SetEvent( blockingState->writeBuffersReadyEvent );
}
}
if( stream->inputChannelCount )
{
if( statusFlags & paInputOverflowed ) {
blockingState->inputOverflowFlag = TRUE;
}
pRb = &blockingState->readRingBuffer;
if( PaUtil_GetRingBufferWriteAvailable(pRb) < (long) framesPerBuffer )
{
blockingState->inputOverflowFlag = TRUE;
PaUtil_AdvanceRingBufferReadIndex( pRb, framesPerBuffer );
}
PaUtil_WriteRingBuffer( pRb, inputBuffer, framesPerBuffer );
if( blockingState->readFramesRequestedFlag && PaUtil_GetRingBufferReadAvailable(pRb) >= (long) blockingState->readFramesRequested )
{
blockingState->readFramesRequestedFlag = FALSE;
blockingState->readFramesRequested     = 0;
SetEvent( blockingState->readFramesReadyEvent );
}
}
return paContinue;
}
PaError PaAsio_ShowControlPanel( PaDeviceIndex device, void* systemSpecific )
{
PaError result = paNoError;
PaUtilHostApiRepresentation *hostApi;
PaDeviceIndex hostApiDevice;
ASIODriverInfo asioDriverInfo;
ASIOError asioError;
int asioIsInitialized = 0;
PaAsioHostApiRepresentation *asioHostApi;
PaAsioDeviceInfo *asioDeviceInfo;
PaWinUtilComInitializationResult comInitializationResult;
result = PaWinUtil_CoInitialize( paASIO, &comInitializationResult );
if( result != paNoError )
return result;
result = PaUtil_GetHostApiRepresentation( &hostApi, paASIO );
if( result != paNoError )
goto error;
result = PaUtil_DeviceIndexToHostApiDeviceIndex( &hostApiDevice, device, hostApi );
if( result != paNoError )
goto error;
asioHostApi = (PaAsioHostApiRepresentation*)hostApi;
if( asioHostApi->openAsioDeviceIndex != paNoDevice )
{
result = paDeviceUnavailable;
goto error;
}
asioDeviceInfo = (PaAsioDeviceInfo*)hostApi->deviceInfos[hostApiDevice];
if( !asioHostApi->asioDrivers->loadDriver( const_cast<char*>(asioDeviceInfo->commonDeviceInfo.name) ) )
{
result = paUnanticipatedHostError;
goto error;
}
memset( &asioDriverInfo, 0, sizeof(ASIODriverInfo) );
asioDriverInfo.asioVersion = 2;
asioDriverInfo.sysRef = systemSpecific;
asioError = ASIOInit( &asioDriverInfo );
if( asioError != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
goto error;
}
else
{
asioIsInitialized = 1;
}
PA_DEBUG(("PaAsio_ShowControlPanel: ASIOInit(): %s\n", PaAsio_GetAsioErrorText(asioError) ));
PA_DEBUG(("asioVersion: ASIOInit(): %ld\n",   asioDriverInfo.asioVersion ));
PA_DEBUG(("driverVersion: ASIOInit(): %ld\n", asioDriverInfo.driverVersion ));
PA_DEBUG(("Name: ASIOInit(): %s\n",           asioDriverInfo.name ));
PA_DEBUG(("ErrorMessage: ASIOInit(): %s\n",   asioDriverInfo.errorMessage ));
asioError = ASIOControlPanel();
if( asioError != ASE_OK )
{
PA_DEBUG(("PaAsio_ShowControlPanel: ASIOControlPanel(): %s\n", PaAsio_GetAsioErrorText(asioError) ));
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
goto error;
}
PA_DEBUG(("PaAsio_ShowControlPanel: ASIOControlPanel(): %s\n", PaAsio_GetAsioErrorText(asioError) ));
asioError = ASIOExit();
if( asioError != ASE_OK )
{
result = paUnanticipatedHostError;
PA_ASIO_SET_LAST_ASIO_ERROR( asioError );
asioIsInitialized = 0;
goto error;
}
PA_DEBUG(("PaAsio_ShowControlPanel: ASIOExit(): %s\n", PaAsio_GetAsioErrorText(asioError) ));
return result;
error:
if( asioIsInitialized )
{
ASIOExit();
}
PaWinUtil_CoUninitialize( paASIO, &comInitializationResult );
return result;
}
PaError PaAsio_GetInputChannelName( PaDeviceIndex device, int channelIndex,
const char** channelName )
{
PaError result = paNoError;
PaUtilHostApiRepresentation *hostApi;
PaDeviceIndex hostApiDevice;
PaAsioDeviceInfo *asioDeviceInfo;
result = PaUtil_GetHostApiRepresentation( &hostApi, paASIO );
if( result != paNoError )
goto error;
result = PaUtil_DeviceIndexToHostApiDeviceIndex( &hostApiDevice, device, hostApi );
if( result != paNoError )
goto error;
asioDeviceInfo = (PaAsioDeviceInfo*)hostApi->deviceInfos[hostApiDevice];
if( channelIndex < 0 || channelIndex >= asioDeviceInfo->commonDeviceInfo.maxInputChannels ){
result = paInvalidChannelCount;
goto error;
}
*channelName = asioDeviceInfo->asioChannelInfos[channelIndex].name;
return paNoError;
error:
return result;
}
PaError PaAsio_GetOutputChannelName( PaDeviceIndex device, int channelIndex,
const char** channelName )
{
PaError result = paNoError;
PaUtilHostApiRepresentation *hostApi;
PaDeviceIndex hostApiDevice;
PaAsioDeviceInfo *asioDeviceInfo;
result = PaUtil_GetHostApiRepresentation( &hostApi, paASIO );
if( result != paNoError )
goto error;
result = PaUtil_DeviceIndexToHostApiDeviceIndex( &hostApiDevice, device, hostApi );
if( result != paNoError )
goto error;
asioDeviceInfo = (PaAsioDeviceInfo*)hostApi->deviceInfos[hostApiDevice];
if( channelIndex < 0 || channelIndex >= asioDeviceInfo->commonDeviceInfo.maxOutputChannels ){
result = paInvalidChannelCount;
goto error;
}
*channelName = asioDeviceInfo->asioChannelInfos[
asioDeviceInfo->commonDeviceInfo.maxInputChannels + channelIndex].name;
return paNoError;
error:
return result;
}
static PaError GetAsioStreamPointer( PaAsioStream **stream, PaStream *s )
{
PaError result;
PaUtilHostApiRepresentation *hostApi;
PaAsioHostApiRepresentation *asioHostApi;
result = PaUtil_ValidateStreamPointer( s );
if( result != paNoError )
return result;
result = PaUtil_GetHostApiRepresentation( &hostApi, paASIO );
if( result != paNoError )
return result;
asioHostApi = (PaAsioHostApiRepresentation*)hostApi;
if( PA_STREAM_REP( s )->streamInterface == &asioHostApi->callbackStreamInterface
|| PA_STREAM_REP( s )->streamInterface == &asioHostApi->blockingStreamInterface )
{
*stream = (PaAsioStream *)s;
return paNoError;
}
else
{
return paIncompatibleStreamHostApi;
}
}
PaError PaAsio_SetStreamSampleRate( PaStream* s, double sampleRate )
{
PaAsioStream *stream;
PaError result = GetAsioStreamPointer( &stream, s );
if( result != paNoError )
return result;
if( stream != theAsioStream )
return paBadStreamPtr;
return ValidateAndSetSampleRate( sampleRate );
}

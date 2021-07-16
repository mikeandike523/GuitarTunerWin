 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#ifndef UNDER_CE
#include <process.h>
#endif
#include <assert.h>
#ifndef __MWERKS__
#include <malloc.h>
#include <memory.h>
#endif  
#include "portaudio.h"
#include "pa_trace.h"
#include "pa_util.h"
#include "pa_allocation.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_cpuload.h"
#include "pa_process.h"
#include "pa_debugprint.h"
#include "pa_win_wmme.h"
#include "pa_win_waveformat.h"
#include "pa_win_util.h"
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
#include "pa_win_wdmks_utils.h"
#ifndef DRV_QUERYDEVICEINTERFACE
#define DRV_QUERYDEVICEINTERFACE     (DRV_RESERVED + 12)
#endif
#ifndef DRV_QUERYDEVICEINTERFACESIZE
#define DRV_QUERYDEVICEINTERFACESIZE (DRV_RESERVED + 13)
#endif
#endif  
#if !defined(__CYGWIN__) && !defined(_WIN32_WCE)
#define CREATE_THREAD (HANDLE)_beginthreadex( 0, 0, ProcessingThreadProc, stream, 0, &stream->processingThreadId )
#define PA_THREAD_FUNC static unsigned WINAPI
#define PA_THREAD_ID unsigned
#else
#define CREATE_THREAD CreateThread( 0, 0, ProcessingThreadProc, stream, 0, &stream->processingThreadId )
#define PA_THREAD_FUNC static DWORD WINAPI
#define PA_THREAD_ID DWORD
#endif
#if (defined(_WIN32_WCE))
#pragma comment(lib, "Coredll.lib")
#elif (defined(WIN32) && (defined(_MSC_VER) && (_MSC_VER >= 1200)))  
#pragma comment(lib, "winmm.lib")
#endif
#ifndef DWORD_PTR
#if defined(_WIN64)
#define DWORD_PTR unsigned __int64
#else
#define DWORD_PTR unsigned long
#endif
#endif
#define PA_MME_USE_HIGH_DEFAULT_LATENCY_    (0)   
#if PA_MME_USE_HIGH_DEFAULT_LATENCY_
#define PA_MME_WIN_9X_DEFAULT_LATENCY_                              (0.4)
#define PA_MME_MIN_HOST_OUTPUT_BUFFER_COUNT_                        (4)
#define PA_MME_MIN_HOST_INPUT_BUFFER_COUNT_FULL_DUPLEX_             (4)
#define PA_MME_MIN_HOST_INPUT_BUFFER_COUNT_HALF_DUPLEX_             (4)
#define PA_MME_HOST_BUFFER_GRANULARITY_FRAMES_WHEN_UNSPECIFIED_     (16)
#define PA_MME_MAX_HOST_BUFFER_SECS_                                (0.3)        
#define PA_MME_MAX_HOST_BUFFER_BYTES_                               (32 * 1024)  
#else
#define PA_MME_WIN_9X_DEFAULT_LATENCY_                              (0.2)
#define PA_MME_MIN_HOST_OUTPUT_BUFFER_COUNT_                        (2)
#define PA_MME_MIN_HOST_INPUT_BUFFER_COUNT_FULL_DUPLEX_             (3)          
#define PA_MME_MIN_HOST_INPUT_BUFFER_COUNT_HALF_DUPLEX_             (2)
#define PA_MME_HOST_BUFFER_GRANULARITY_FRAMES_WHEN_UNSPECIFIED_     (16)
#define PA_MME_MAX_HOST_BUFFER_SECS_                                (0.1)        
#define PA_MME_MAX_HOST_BUFFER_BYTES_                               (32 * 1024)  
#endif
#define PA_MME_WIN_NT_DEFAULT_LATENCY_                              (0.4)
#define PA_MME_WIN_WDM_DEFAULT_LATENCY_                             (0.090)
#define PA_MME_TARGET_HOST_BUFFER_COUNT_    8
#define PA_MME_MIN_TIMEOUT_MSEC_        (1000)
static const char constInputMapperSuffix_[] = " - Input";
static const char constOutputMapperSuffix_[] = " - Output";
static char *CopyWCharStringToUtf8CString(char *destination, size_t destLengthBytes, const WCHAR *source)
{
int intDestLengthBytes;  
if (destLengthBytes < INT_MAX)
{
#pragma warning (disable : 4267)  
intDestLengthBytes = (int)destLengthBytes;  
#pragma warning (default : 4267)
}
else
{
intDestLengthBytes = INT_MAX;
}
if (WideCharToMultiByte(CP_UTF8, 0, source, -1, destination,  intDestLengthBytes, NULL, NULL) == 0)
return NULL;
return destination;
}
static size_t WCharStringLen(const WCHAR *str)
{
return WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
}
typedef struct PaWinMmeStream PaWinMmeStream;      
#ifdef __cplusplus
extern "C"
{
#endif  
PaError PaWinMme_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
#ifdef __cplusplus
}
#endif  
static void Terminate( struct PaUtilHostApiRepresentation *hostApi );
static PaError OpenStream( struct PaUtilHostApiRepresentation *hostApi,
PaStream** stream,
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
#define PA_MME_SET_LAST_WAVEIN_ERROR( mmresult )                              \
{                                                                         \
wchar_t mmeErrorTextWide[ MAXERRORLENGTH ];                           \
char mmeErrorText[ MAXERRORLENGTH ];                                  \
waveInGetErrorTextW( mmresult, mmeErrorTextWide, MAXERRORLENGTH );    \
WideCharToMultiByte( CP_UTF8, 0, mmeErrorTextWide, -1,                \
mmeErrorText, MAXERRORLENGTH, NULL, NULL );                       \
PaUtil_SetLastHostErrorInfo( paMME, mmresult, mmeErrorText );         \
}
#define PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult )                             \
{                                                                         \
wchar_t mmeErrorTextWide[ MAXERRORLENGTH ];                           \
char mmeErrorText[ MAXERRORLENGTH ];                                  \
waveOutGetErrorTextW( mmresult, mmeErrorTextWide, MAXERRORLENGTH );   \
WideCharToMultiByte( CP_UTF8, 0, mmeErrorTextWide, -1,                \
mmeErrorText, MAXERRORLENGTH, NULL, NULL );                       \
PaUtil_SetLastHostErrorInfo( paMME, mmresult, mmeErrorText );         \
}
static void PaMme_SetLastSystemError( DWORD errorCode )
{
PaWinUtil_SetLastSystemErrorInfo( paMME, errorCode );
}
#define PA_MME_SET_LAST_SYSTEM_ERROR( errorCode ) \
PaMme_SetLastSystemError( errorCode )
static PaError CreateEventWithPaError( HANDLE *handle,
LPSECURITY_ATTRIBUTES lpEventAttributes,
BOOL bManualReset,
BOOL bInitialState,
LPCTSTR lpName )
{
PaError result = paNoError;
*handle = NULL;
*handle = CreateEvent( lpEventAttributes, bManualReset, bInitialState, lpName );
if( *handle == NULL )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_SYSTEM_ERROR( GetLastError() );
}
return result;
}
static PaError ResetEventWithPaError( HANDLE handle )
{
PaError result = paNoError;
if( handle )
{
if( ResetEvent( handle ) == 0 )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_SYSTEM_ERROR( GetLastError() );
}
}
return result;
}
static PaError CloseHandleWithPaError( HANDLE handle )
{
PaError result = paNoError;
if( handle )
{
if( CloseHandle( handle ) == 0 )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_SYSTEM_ERROR( GetLastError() );
}
}
return result;
}
typedef struct
{
PaUtilHostApiRepresentation inheritedHostApiRep;
PaUtilStreamInterface callbackStreamInterface;
PaUtilStreamInterface blockingStreamInterface;
PaUtilAllocationGroup *allocations;
int inputDeviceCount, outputDeviceCount;
UINT *winMmeDeviceIds;
}
PaWinMmeHostApiRepresentation;
typedef struct
{
PaDeviceInfo inheritedDeviceInfo;
DWORD dwFormats;  
char deviceInputChannelCountIsKnown;  
char deviceOutputChannelCountIsKnown;  
}
PaWinMmeDeviceInfo;
#define PA_ENV_BUF_SIZE_  (32)
#define PA_REC_IN_DEV_ENV_NAME_  ("PA_RECOMMENDED_INPUT_DEVICE")
#define PA_REC_OUT_DEV_ENV_NAME_  ("PA_RECOMMENDED_OUTPUT_DEVICE")
static PaDeviceIndex GetEnvDefaultDeviceID( char *envName )
{
PaDeviceIndex recommendedIndex = paNoDevice;
DWORD   hresult;
char    envbuf[PA_ENV_BUF_SIZE_];
#ifndef WIN32_PLATFORM_PSPC  
hresult = GetEnvironmentVariableA( envName, envbuf, PA_ENV_BUF_SIZE_ );
if( (hresult > 0) && (hresult < PA_ENV_BUF_SIZE_) )
{
recommendedIndex = atoi( envbuf );
}
#endif
return recommendedIndex;
}
static void InitializeDefaultDeviceIdsFromEnv( PaWinMmeHostApiRepresentation *hostApi )
{
PaDeviceIndex device;
device = GetEnvDefaultDeviceID( PA_REC_IN_DEV_ENV_NAME_ );
if( device != paNoDevice &&
( device >= 0 && device < hostApi->inheritedHostApiRep.info.deviceCount ) &&
hostApi->inheritedHostApiRep.deviceInfos[ device ]->maxInputChannels > 0 )
{
hostApi->inheritedHostApiRep.info.defaultInputDevice = device;
}
device = GetEnvDefaultDeviceID( PA_REC_OUT_DEV_ENV_NAME_ );
if( device != paNoDevice &&
( device >= 0 && device < hostApi->inheritedHostApiRep.info.deviceCount ) &&
hostApi->inheritedHostApiRep.deviceInfos[ device ]->maxOutputChannels > 0 )
{
hostApi->inheritedHostApiRep.info.defaultOutputDevice = device;
}
}
static UINT LocalDeviceIndexToWinMmeDeviceId( PaWinMmeHostApiRepresentation *hostApi, PaDeviceIndex device )
{
assert( device >= 0 && device < hostApi->inputDeviceCount + hostApi->outputDeviceCount );
return hostApi->winMmeDeviceIds[ device ];
}
static int SampleFormatAndWinWmmeSpecificFlagsToLinearWaveFormatTag( PaSampleFormat sampleFormat, unsigned long winMmeSpecificFlags )
{
int waveFormatTag = 0;
if( winMmeSpecificFlags & paWinMmeWaveFormatDolbyAc3Spdif )
waveFormatTag = PAWIN_WAVE_FORMAT_DOLBY_AC3_SPDIF;
else if( winMmeSpecificFlags & paWinMmeWaveFormatWmaSpdif )
waveFormatTag = PAWIN_WAVE_FORMAT_WMA_SPDIF;
else
waveFormatTag = PaWin_SampleFormatToLinearWaveFormatTag( sampleFormat );
return waveFormatTag;
}
static PaError QueryInputWaveFormatEx( int deviceId, WAVEFORMATEX *waveFormatEx )
{
MMRESULT mmresult;
switch( mmresult = waveInOpen( NULL, deviceId, waveFormatEx, 0, 0, WAVE_FORMAT_QUERY ) )
{
case MMSYSERR_NOERROR:
return paNoError;
case MMSYSERR_ALLOCATED:     
return paDeviceUnavailable;
case MMSYSERR_NODRIVER:      
return paDeviceUnavailable;
case MMSYSERR_NOMEM:         
return paInsufficientMemory;
case WAVERR_BADFORMAT:       
return paSampleFormatNotSupported;
case MMSYSERR_BADDEVICEID:   
default:
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
return paUnanticipatedHostError;
}
}
static PaError QueryOutputWaveFormatEx( int deviceId, WAVEFORMATEX *waveFormatEx )
{
MMRESULT mmresult;
switch( mmresult = waveOutOpen( NULL, deviceId, waveFormatEx, 0, 0, WAVE_FORMAT_QUERY ) )
{
case MMSYSERR_NOERROR:
return paNoError;
case MMSYSERR_ALLOCATED:     
return paDeviceUnavailable;
case MMSYSERR_NODRIVER:      
return paDeviceUnavailable;
case MMSYSERR_NOMEM:         
return paInsufficientMemory;
case WAVERR_BADFORMAT:       
return paSampleFormatNotSupported;
case MMSYSERR_BADDEVICEID:   
default:
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
return paUnanticipatedHostError;
}
}
static PaError QueryFormatSupported( PaDeviceInfo *deviceInfo,
PaError (*waveFormatExQueryFunction)(int, WAVEFORMATEX*),
int winMmeDeviceId, int channels, double sampleRate, unsigned long winMmeSpecificFlags )
{
PaWinMmeDeviceInfo *winMmeDeviceInfo = (PaWinMmeDeviceInfo*)deviceInfo;
PaWinWaveFormat waveFormat;
PaSampleFormat sampleFormat;
int waveFormatTag;
sampleFormat = paInt16;
waveFormatTag = SampleFormatAndWinWmmeSpecificFlagsToLinearWaveFormatTag( sampleFormat, winMmeSpecificFlags );
if( waveFormatTag == PaWin_SampleFormatToLinearWaveFormatTag( paInt16 ) ){
if( sampleRate == 11025.0
&& ( (channels == 1 && (winMmeDeviceInfo->dwFormats & WAVE_FORMAT_1M16))
|| (channels == 2 && (winMmeDeviceInfo->dwFormats & WAVE_FORMAT_1S16)) ) ){
return paNoError;
}
if( sampleRate == 22050.0
&& ( (channels == 1 && (winMmeDeviceInfo->dwFormats & WAVE_FORMAT_2M16))
|| (channels == 2 && (winMmeDeviceInfo->dwFormats & WAVE_FORMAT_2S16)) ) ){
return paNoError;
}
if( sampleRate == 44100.0
&& ( (channels == 1 && (winMmeDeviceInfo->dwFormats & WAVE_FORMAT_4M16))
|| (channels == 2 && (winMmeDeviceInfo->dwFormats & WAVE_FORMAT_4S16)) ) ){
return paNoError;
}
}
PaWin_InitializeWaveFormatExtensible( &waveFormat, channels, sampleFormat, waveFormatTag,
sampleRate, PAWIN_SPEAKER_DIRECTOUT );
if( waveFormatExQueryFunction( winMmeDeviceId, (WAVEFORMATEX*)&waveFormat ) == paNoError )
return paNoError;
PaWin_InitializeWaveFormatEx( &waveFormat, channels, sampleFormat, waveFormatTag, sampleRate );
return waveFormatExQueryFunction( winMmeDeviceId, (WAVEFORMATEX*)&waveFormat );
}
#define PA_DEFAULTSAMPLERATESEARCHORDER_COUNT_  (13)  
static double defaultSampleRateSearchOrder_[] =
{ 44100.0, 48000.0, 32000.0, 24000.0, 22050.0, 88200.0, 96000.0, 192000.0,
16000.0, 12000.0, 11025.0, 9600.0, 8000.0 };
static void DetectDefaultSampleRate( PaWinMmeDeviceInfo *winMmeDeviceInfo, int winMmeDeviceId,
PaError (*waveFormatExQueryFunction)(int, WAVEFORMATEX*), int maxChannels )
{
PaDeviceInfo *deviceInfo = &winMmeDeviceInfo->inheritedDeviceInfo;
int i;
deviceInfo->defaultSampleRate = 0.;
for( i=0; i < PA_DEFAULTSAMPLERATESEARCHORDER_COUNT_; ++i )
{
double sampleRate = defaultSampleRateSearchOrder_[ i ];
PaError paerror = QueryFormatSupported( deviceInfo, waveFormatExQueryFunction, winMmeDeviceId, maxChannels, sampleRate, 0 );
if( paerror == paNoError )
{
deviceInfo->defaultSampleRate = sampleRate;
break;
}
}
}
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
static int QueryWaveInKSFilterMaxChannels( UINT waveInDeviceId, int *maxChannels )
{
void *devicePath;
DWORD devicePathSize;
int result = 0;
HWAVEIN hDeviceId = (HWAVEIN)((UINT_PTR)waveInDeviceId);
if( waveInMessage(hDeviceId, DRV_QUERYDEVICEINTERFACESIZE,
(DWORD_PTR)&devicePathSize, 0 ) != MMSYSERR_NOERROR )
return 0;
devicePath = PaUtil_AllocateMemory( devicePathSize );
if( !devicePath )
return 0;
if( waveInMessage(hDeviceId, DRV_QUERYDEVICEINTERFACE,
(DWORD_PTR)devicePath, devicePathSize ) == MMSYSERR_NOERROR )
{
int count = PaWin_WDMKS_QueryFilterMaximumChannelCount( devicePath,   1  );
if( count > 0 )
{
*maxChannels = count;
result = 1;
}
}
PaUtil_FreeMemory( devicePath );
return result;
}
#endif  
static PaError InitializeInputDeviceInfo( PaWinMmeHostApiRepresentation *winMmeHostApi,
PaWinMmeDeviceInfo *winMmeDeviceInfo, UINT winMmeInputDeviceId, int *success )
{
PaError result = paNoError;
char *deviceName;  
MMRESULT mmresult;
WAVEINCAPSW wic;
PaDeviceInfo *deviceInfo = &winMmeDeviceInfo->inheritedDeviceInfo;
size_t len;
*success = 0;
mmresult = waveInGetDevCapsW( winMmeInputDeviceId, &wic, sizeof( WAVEINCAPSW ) );
if( mmresult == MMSYSERR_NOMEM )
{
result = paInsufficientMemory;
goto error;
}
else if( mmresult != MMSYSERR_NOERROR )
{
return paNoError;
}
if( winMmeInputDeviceId == WAVE_MAPPER )
{
len = WCharStringLen( wic.szPname ) + 1 + sizeof(constInputMapperSuffix_);
deviceName = (char*)PaUtil_GroupAllocateMemory(
winMmeHostApi->allocations,
(long)len );
if( !deviceName )
{
result = paInsufficientMemory;
goto error;
}
CopyWCharStringToUtf8CString( deviceName, len, wic.szPname );
strcat( deviceName, constInputMapperSuffix_ );
}
else
{
len = WCharStringLen( wic.szPname ) + 1;
deviceName = (char*)PaUtil_GroupAllocateMemory(
winMmeHostApi->allocations,
(long)len );
if( !deviceName )
{
result = paInsufficientMemory;
goto error;
}
CopyWCharStringToUtf8CString( deviceName, len, wic.szPname  );
}
deviceInfo->name = deviceName;
if( wic.wChannels == 0xFFFF || wic.wChannels < 1 || wic.wChannels > 255 ){
PA_DEBUG(("Pa_GetDeviceInfo: Num input channels reported as %d! Changed to 2.\n", wic.wChannels ));
deviceInfo->maxInputChannels = 2;
winMmeDeviceInfo->deviceInputChannelCountIsKnown = 0;
}else{
deviceInfo->maxInputChannels = wic.wChannels;
winMmeDeviceInfo->deviceInputChannelCountIsKnown = 1;
}
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
winMmeDeviceInfo->deviceInputChannelCountIsKnown =
QueryWaveInKSFilterMaxChannels( winMmeInputDeviceId, &deviceInfo->maxInputChannels );
#endif  
winMmeDeviceInfo->dwFormats = wic.dwFormats;
DetectDefaultSampleRate( winMmeDeviceInfo, winMmeInputDeviceId,
QueryInputWaveFormatEx, deviceInfo->maxInputChannels );
*success = 1;
error:
return result;
}
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
static int QueryWaveOutKSFilterMaxChannels( UINT waveOutDeviceId, int *maxChannels )
{
void *devicePath;
DWORD devicePathSize;
int result = 0;
HWAVEOUT hDeviceId = (HWAVEOUT)((UINT_PTR)waveOutDeviceId);
if( waveOutMessage(hDeviceId, DRV_QUERYDEVICEINTERFACESIZE,
(DWORD_PTR)&devicePathSize, 0 ) != MMSYSERR_NOERROR )
return 0;
devicePath = PaUtil_AllocateMemory( devicePathSize );
if( !devicePath )
return 0;
if( waveOutMessage(hDeviceId, DRV_QUERYDEVICEINTERFACE,
(DWORD_PTR)devicePath, devicePathSize ) == MMSYSERR_NOERROR )
{
int count = PaWin_WDMKS_QueryFilterMaximumChannelCount( devicePath,   0  );
if( count > 0 )
{
*maxChannels = count;
result = 1;
}
}
PaUtil_FreeMemory( devicePath );
return result;
}
#endif  
static PaError InitializeOutputDeviceInfo( PaWinMmeHostApiRepresentation *winMmeHostApi,
PaWinMmeDeviceInfo *winMmeDeviceInfo, UINT winMmeOutputDeviceId, int *success )
{
PaError result = paNoError;
char *deviceName;  
MMRESULT mmresult;
WAVEOUTCAPSW woc;
PaDeviceInfo *deviceInfo = &winMmeDeviceInfo->inheritedDeviceInfo;
size_t len;
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
int wdmksDeviceOutputChannelCountIsKnown;
#endif
*success = 0;
mmresult = waveOutGetDevCapsW( winMmeOutputDeviceId, &woc, sizeof( WAVEOUTCAPSW ) );
if( mmresult == MMSYSERR_NOMEM )
{
result = paInsufficientMemory;
goto error;
}
else if( mmresult != MMSYSERR_NOERROR )
{
return paNoError;
}
if( winMmeOutputDeviceId == WAVE_MAPPER )
{
len = WCharStringLen( woc.szPname ) + 1 + sizeof(constOutputMapperSuffix_);
deviceName = (char*)PaUtil_GroupAllocateMemory(
winMmeHostApi->allocations,
(long)len );
if( !deviceName )
{
result = paInsufficientMemory;
goto error;
}
CopyWCharStringToUtf8CString( deviceName, len, woc.szPname );
strcat( deviceName, constOutputMapperSuffix_ );
}
else
{
len = WCharStringLen( woc.szPname ) + 1;
deviceName = (char*)PaUtil_GroupAllocateMemory(
winMmeHostApi->allocations,
(long)len );
if( !deviceName )
{
result = paInsufficientMemory;
goto error;
}
CopyWCharStringToUtf8CString( deviceName, len, woc.szPname );
}
deviceInfo->name = deviceName;
if( woc.wChannels == 0xFFFF || woc.wChannels < 1 || woc.wChannels > 255 ){
PA_DEBUG(("Pa_GetDeviceInfo: Num output channels reported as %d! Changed to 2.\n", woc.wChannels ));
deviceInfo->maxOutputChannels = 2;
winMmeDeviceInfo->deviceOutputChannelCountIsKnown = 0;
}else{
deviceInfo->maxOutputChannels = woc.wChannels;
winMmeDeviceInfo->deviceOutputChannelCountIsKnown = 1;
}
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
wdmksDeviceOutputChannelCountIsKnown = QueryWaveOutKSFilterMaxChannels(
winMmeOutputDeviceId, &deviceInfo->maxOutputChannels );
if( wdmksDeviceOutputChannelCountIsKnown && !winMmeDeviceInfo->deviceOutputChannelCountIsKnown )
winMmeDeviceInfo->deviceOutputChannelCountIsKnown = 1;
#endif  
winMmeDeviceInfo->dwFormats = woc.dwFormats;
DetectDefaultSampleRate( winMmeDeviceInfo, winMmeOutputDeviceId,
QueryOutputWaveFormatEx, deviceInfo->maxOutputChannels );
*success = 1;
error:
return result;
}
static void GetDefaultLatencies( PaTime *defaultLowLatency, PaTime *defaultHighLatency )
{
#pragma warning (disable : 4996)  
OSVERSIONINFO osvi;
osvi.dwOSVersionInfoSize = sizeof( osvi );
GetVersionEx( &osvi );
if( (osvi.dwMajorVersion == 4) && (osvi.dwPlatformId == 2) )
{
*defaultLowLatency = PA_MME_WIN_NT_DEFAULT_LATENCY_;
}
else if(osvi.dwMajorVersion >= 5)
{
*defaultLowLatency = PA_MME_WIN_WDM_DEFAULT_LATENCY_;
}
else
{
*defaultLowLatency = PA_MME_WIN_9X_DEFAULT_LATENCY_;
}
*defaultHighLatency = *defaultLowLatency * 2;
#pragma warning (default : 4996)
}
PaError PaWinMme_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result = paNoError;
int i;
PaWinMmeHostApiRepresentation *winMmeHostApi;
int inputDeviceCount, outputDeviceCount, maximumPossibleDeviceCount;
PaWinMmeDeviceInfo *deviceInfoArray;
int deviceInfoInitializationSucceeded;
PaTime defaultLowLatency, defaultHighLatency;
DWORD waveInPreferredDevice, waveOutPreferredDevice;
DWORD preferredDeviceStatusFlags;
winMmeHostApi = (PaWinMmeHostApiRepresentation*)PaUtil_AllocateMemory( sizeof(PaWinMmeHostApiRepresentation) );
if( !winMmeHostApi )
{
result = paInsufficientMemory;
goto error;
}
winMmeHostApi->allocations = PaUtil_CreateAllocationGroup();
if( !winMmeHostApi->allocations )
{
result = paInsufficientMemory;
goto error;
}
*hostApi = &winMmeHostApi->inheritedHostApiRep;
(*hostApi)->info.structVersion = 1;
(*hostApi)->info.type = paMME;
(*hostApi)->info.name = "MME";
(*hostApi)->info.deviceCount = 0;
(*hostApi)->info.defaultInputDevice = paNoDevice;
(*hostApi)->info.defaultOutputDevice = paNoDevice;
winMmeHostApi->inputDeviceCount = 0;
winMmeHostApi->outputDeviceCount = 0;
#if !defined(DRVM_MAPPER_PREFERRED_GET)
#define DRVM_MAPPER_PREFERRED_GET    (0x2000+21)
#endif
preferredDeviceStatusFlags = 0;
waveInPreferredDevice = -1;
waveInMessage( (HWAVEIN)((UINT_PTR)WAVE_MAPPER), DRVM_MAPPER_PREFERRED_GET, (DWORD_PTR)&waveInPreferredDevice, (DWORD_PTR)&preferredDeviceStatusFlags );
preferredDeviceStatusFlags = 0;
waveOutPreferredDevice = -1;
waveOutMessage( (HWAVEOUT)((UINT_PTR)WAVE_MAPPER), DRVM_MAPPER_PREFERRED_GET, (DWORD_PTR)&waveOutPreferredDevice, (DWORD_PTR)&preferredDeviceStatusFlags );
maximumPossibleDeviceCount = 0;
inputDeviceCount = waveInGetNumDevs();
if( inputDeviceCount > 0 )
maximumPossibleDeviceCount += inputDeviceCount + 1;      
outputDeviceCount = waveOutGetNumDevs();
if( outputDeviceCount > 0 )
maximumPossibleDeviceCount += outputDeviceCount + 1;     
if( maximumPossibleDeviceCount > 0 ){
(*hostApi)->deviceInfos = (PaDeviceInfo**)PaUtil_GroupAllocateMemory(
winMmeHostApi->allocations, sizeof(PaDeviceInfo*) * maximumPossibleDeviceCount );
if( !(*hostApi)->deviceInfos )
{
result = paInsufficientMemory;
goto error;
}
deviceInfoArray = (PaWinMmeDeviceInfo*)PaUtil_GroupAllocateMemory(
winMmeHostApi->allocations, sizeof(PaWinMmeDeviceInfo) * maximumPossibleDeviceCount );
if( !deviceInfoArray )
{
result = paInsufficientMemory;
goto error;
}
winMmeHostApi->winMmeDeviceIds = (UINT*)PaUtil_GroupAllocateMemory(
winMmeHostApi->allocations, sizeof(int) * maximumPossibleDeviceCount );
if( !winMmeHostApi->winMmeDeviceIds )
{
result = paInsufficientMemory;
goto error;
}
GetDefaultLatencies( &defaultLowLatency, &defaultHighLatency );
if( inputDeviceCount > 0 ){
for( i = -1; i < inputDeviceCount; ++i ){
UINT winMmeDeviceId = (UINT)((i==-1) ? WAVE_MAPPER : i);
PaWinMmeDeviceInfo *wmmeDeviceInfo = &deviceInfoArray[ (*hostApi)->info.deviceCount ];
PaDeviceInfo *deviceInfo = &wmmeDeviceInfo->inheritedDeviceInfo;
deviceInfo->structVersion = 2;
deviceInfo->hostApi = hostApiIndex;
deviceInfo->maxInputChannels = 0;
wmmeDeviceInfo->deviceInputChannelCountIsKnown = 1;
deviceInfo->maxOutputChannels = 0;
wmmeDeviceInfo->deviceOutputChannelCountIsKnown = 1;
deviceInfo->defaultLowInputLatency = defaultLowLatency;
deviceInfo->defaultLowOutputLatency = defaultLowLatency;
deviceInfo->defaultHighInputLatency = defaultHighLatency;
deviceInfo->defaultHighOutputLatency = defaultHighLatency;
result = InitializeInputDeviceInfo( winMmeHostApi, wmmeDeviceInfo,
winMmeDeviceId, &deviceInfoInitializationSucceeded );
if( result != paNoError )
goto error;
if( deviceInfoInitializationSucceeded ){
if( (*hostApi)->info.defaultInputDevice == paNoDevice ){
(*hostApi)->info.defaultInputDevice = (*hostApi)->info.deviceCount;
}else if( winMmeDeviceId == waveInPreferredDevice ){
(*hostApi)->info.defaultInputDevice = (*hostApi)->info.deviceCount;
}
winMmeHostApi->winMmeDeviceIds[ (*hostApi)->info.deviceCount ] = winMmeDeviceId;
(*hostApi)->deviceInfos[ (*hostApi)->info.deviceCount ] = deviceInfo;
winMmeHostApi->inputDeviceCount++;
(*hostApi)->info.deviceCount++;
}
}
}
if( outputDeviceCount > 0 ){
for( i = -1; i < outputDeviceCount; ++i ){
UINT winMmeDeviceId = (UINT)((i==-1) ? WAVE_MAPPER : i);
PaWinMmeDeviceInfo *wmmeDeviceInfo = &deviceInfoArray[ (*hostApi)->info.deviceCount ];
PaDeviceInfo *deviceInfo = &wmmeDeviceInfo->inheritedDeviceInfo;
deviceInfo->structVersion = 2;
deviceInfo->hostApi = hostApiIndex;
deviceInfo->maxInputChannels = 0;
wmmeDeviceInfo->deviceInputChannelCountIsKnown = 1;
deviceInfo->maxOutputChannels = 0;
wmmeDeviceInfo->deviceOutputChannelCountIsKnown = 1;
deviceInfo->defaultLowInputLatency = defaultLowLatency;
deviceInfo->defaultLowOutputLatency = defaultLowLatency;
deviceInfo->defaultHighInputLatency = defaultHighLatency;
deviceInfo->defaultHighOutputLatency = defaultHighLatency;
result = InitializeOutputDeviceInfo( winMmeHostApi, wmmeDeviceInfo,
winMmeDeviceId, &deviceInfoInitializationSucceeded );
if( result != paNoError )
goto error;
if( deviceInfoInitializationSucceeded ){
if( (*hostApi)->info.defaultOutputDevice == paNoDevice ){
(*hostApi)->info.defaultOutputDevice = (*hostApi)->info.deviceCount;
}else if( winMmeDeviceId == waveOutPreferredDevice ){
(*hostApi)->info.defaultOutputDevice = (*hostApi)->info.deviceCount;
}
winMmeHostApi->winMmeDeviceIds[ (*hostApi)->info.deviceCount ] = winMmeDeviceId;
(*hostApi)->deviceInfos[ (*hostApi)->info.deviceCount ] = deviceInfo;
winMmeHostApi->outputDeviceCount++;
(*hostApi)->info.deviceCount++;
}
}
}
}
InitializeDefaultDeviceIdsFromEnv( winMmeHostApi );
(*hostApi)->Terminate = Terminate;
(*hostApi)->OpenStream = OpenStream;
(*hostApi)->IsFormatSupported = IsFormatSupported;
PaUtil_InitializeStreamInterface( &winMmeHostApi->callbackStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable, PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &winMmeHostApi->blockingStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream, GetStreamReadAvailable, GetStreamWriteAvailable );
return result;
error:
if( winMmeHostApi )
{
if( winMmeHostApi->allocations )
{
PaUtil_FreeAllAllocations( winMmeHostApi->allocations );
PaUtil_DestroyAllocationGroup( winMmeHostApi->allocations );
}
PaUtil_FreeMemory( winMmeHostApi );
}
return result;
}
static void Terminate( struct PaUtilHostApiRepresentation *hostApi )
{
PaWinMmeHostApiRepresentation *winMmeHostApi = (PaWinMmeHostApiRepresentation*)hostApi;
if( winMmeHostApi->allocations )
{
PaUtil_FreeAllAllocations( winMmeHostApi->allocations );
PaUtil_DestroyAllocationGroup( winMmeHostApi->allocations );
}
PaUtil_FreeMemory( winMmeHostApi );
}
static PaError IsInputChannelCountSupported( PaWinMmeDeviceInfo* deviceInfo, int channelCount )
{
PaError result = paNoError;
if( channelCount > 0
&& deviceInfo->deviceInputChannelCountIsKnown
&& channelCount > deviceInfo->inheritedDeviceInfo.maxInputChannels ){
result = paInvalidChannelCount;
}
return result;
}
static PaError IsOutputChannelCountSupported( PaWinMmeDeviceInfo* deviceInfo, int channelCount )
{
PaError result = paNoError;
if( channelCount > 0
&& deviceInfo->deviceOutputChannelCountIsKnown
&& channelCount > deviceInfo->inheritedDeviceInfo.maxOutputChannels ){
result = paInvalidChannelCount;
}
return result;
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate )
{
PaWinMmeHostApiRepresentation *winMmeHostApi = (PaWinMmeHostApiRepresentation*)hostApi;
PaDeviceInfo *inputDeviceInfo, *outputDeviceInfo;
int inputChannelCount, outputChannelCount;
int inputMultipleDeviceChannelCount, outputMultipleDeviceChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaWinMmeStreamInfo *inputStreamInfo, *outputStreamInfo;
UINT winMmeInputDeviceId, winMmeOutputDeviceId;
unsigned int i;
PaError paerror;
if( inputParameters )
{
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
inputStreamInfo = inputParameters->hostApiSpecificStreamInfo;
if( inputSampleFormat & paCustomFormat )
return paSampleFormatNotSupported;
if( inputParameters->device == paUseHostApiSpecificDeviceSpecification
&& inputStreamInfo && (inputStreamInfo->flags & paWinMmeUseMultipleDevices) )
{
inputMultipleDeviceChannelCount = 0;
for( i=0; i< inputStreamInfo->deviceCount; ++i )
{
inputMultipleDeviceChannelCount += inputStreamInfo->devices[i].channelCount;
inputDeviceInfo = hostApi->deviceInfos[ inputStreamInfo->devices[i].device ];
if( inputStreamInfo->devices[i].channelCount < 1 )
return paInvalidChannelCount;
paerror = IsInputChannelCountSupported( (PaWinMmeDeviceInfo*)inputDeviceInfo,
inputStreamInfo->devices[i].channelCount );
if( paerror != paNoError )
return paerror;
winMmeInputDeviceId = LocalDeviceIndexToWinMmeDeviceId( winMmeHostApi, inputStreamInfo->devices[i].device );
paerror = QueryFormatSupported( inputDeviceInfo, QueryInputWaveFormatEx,
winMmeInputDeviceId, inputStreamInfo->devices[i].channelCount, sampleRate,
((inputStreamInfo) ? inputStreamInfo->flags : 0) );
if( paerror != paNoError )
return paInvalidSampleRate;
}
if( inputMultipleDeviceChannelCount != inputChannelCount )
return paIncompatibleHostApiSpecificStreamInfo;
}
else
{
if( inputStreamInfo && (inputStreamInfo->flags & paWinMmeUseMultipleDevices) )
return paIncompatibleHostApiSpecificStreamInfo;  
inputDeviceInfo = hostApi->deviceInfos[ inputParameters->device ];
paerror = IsInputChannelCountSupported( (PaWinMmeDeviceInfo*)inputDeviceInfo, inputChannelCount );
if( paerror != paNoError )
return paerror;
winMmeInputDeviceId = LocalDeviceIndexToWinMmeDeviceId( winMmeHostApi, inputParameters->device );
paerror = QueryFormatSupported( inputDeviceInfo, QueryInputWaveFormatEx,
winMmeInputDeviceId, inputChannelCount, sampleRate,
((inputStreamInfo) ? inputStreamInfo->flags : 0) );
if( paerror != paNoError )
return paInvalidSampleRate;
}
}
if( outputParameters )
{
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
outputStreamInfo = outputParameters->hostApiSpecificStreamInfo;
if( outputSampleFormat & paCustomFormat )
return paSampleFormatNotSupported;
if( outputParameters->device == paUseHostApiSpecificDeviceSpecification
&& outputStreamInfo && (outputStreamInfo->flags & paWinMmeUseMultipleDevices) )
{
outputMultipleDeviceChannelCount = 0;
for( i=0; i< outputStreamInfo->deviceCount; ++i )
{
outputMultipleDeviceChannelCount += outputStreamInfo->devices[i].channelCount;
outputDeviceInfo = hostApi->deviceInfos[ outputStreamInfo->devices[i].device ];
if( outputStreamInfo->devices[i].channelCount < 1 )
return paInvalidChannelCount;
paerror = IsOutputChannelCountSupported( (PaWinMmeDeviceInfo*)outputDeviceInfo,
outputStreamInfo->devices[i].channelCount );
if( paerror != paNoError )
return paerror;
winMmeOutputDeviceId = LocalDeviceIndexToWinMmeDeviceId( winMmeHostApi, outputStreamInfo->devices[i].device );
paerror = QueryFormatSupported( outputDeviceInfo, QueryOutputWaveFormatEx,
winMmeOutputDeviceId, outputStreamInfo->devices[i].channelCount, sampleRate,
((outputStreamInfo) ? outputStreamInfo->flags : 0) );
if( paerror != paNoError )
return paInvalidSampleRate;
}
if( outputMultipleDeviceChannelCount != outputChannelCount )
return paIncompatibleHostApiSpecificStreamInfo;
}
else
{
if( outputStreamInfo && (outputStreamInfo->flags & paWinMmeUseMultipleDevices) )
return paIncompatibleHostApiSpecificStreamInfo;  
outputDeviceInfo = hostApi->deviceInfos[ outputParameters->device ];
paerror = IsOutputChannelCountSupported( (PaWinMmeDeviceInfo*)outputDeviceInfo, outputChannelCount );
if( paerror != paNoError )
return paerror;
winMmeOutputDeviceId = LocalDeviceIndexToWinMmeDeviceId( winMmeHostApi, outputParameters->device );
paerror = QueryFormatSupported( outputDeviceInfo, QueryOutputWaveFormatEx,
winMmeOutputDeviceId, outputChannelCount, sampleRate,
((outputStreamInfo) ? outputStreamInfo->flags : 0) );
if( paerror != paNoError )
return paInvalidSampleRate;
}
}
return paFormatIsSupported;
}
static unsigned long ComputeHostBufferCountForFixedBufferSizeFrames(
unsigned long suggestedLatencyFrames,
unsigned long hostBufferSizeFrames,
unsigned long minimumBufferCount )
{
unsigned long resultBufferCount = ((suggestedLatencyFrames + (hostBufferSizeFrames - 1)) / hostBufferSizeFrames);
resultBufferCount += 1;
if( resultBufferCount < minimumBufferCount )  
resultBufferCount = minimumBufferCount;
return resultBufferCount;
}
static unsigned long ComputeHostBufferSizeGivenHardUpperLimit(
unsigned long userFramesPerBuffer,
unsigned long absoluteMaximumBufferSizeFrames )
{
static unsigned long primes_[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23,
29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 0 };  
unsigned long result = userFramesPerBuffer;
int i;
assert( absoluteMaximumBufferSizeFrames > 67 );  
while( result > absoluteMaximumBufferSizeFrames ){
for( i=0; primes_[i] != 0; ++i )
{
unsigned long p = primes_[i];
unsigned long divided = result / p;
if( divided*p == result )
{
result = divided;
break;  
}
}
if( primes_[i] == 0 )
{  
unsigned long d = (userFramesPerBuffer + (absoluteMaximumBufferSizeFrames-1))
/ absoluteMaximumBufferSizeFrames;
return userFramesPerBuffer / d;
}
}
return result;
}
static PaError SelectHostBufferSizeFramesAndHostBufferCount(
unsigned long suggestedLatencyFrames,
unsigned long userFramesPerBuffer,
unsigned long minimumBufferCount,
unsigned long preferredMaximumBufferSizeFrames,  
unsigned long absoluteMaximumBufferSizeFrames,   
unsigned long *hostBufferSizeFrames,
unsigned long *hostBufferCount )
{
unsigned long effectiveUserFramesPerBuffer;
unsigned long numberOfUserBuffersPerHostBuffer;
if( userFramesPerBuffer == paFramesPerBufferUnspecified ){
effectiveUserFramesPerBuffer = PA_MME_HOST_BUFFER_GRANULARITY_FRAMES_WHEN_UNSPECIFIED_;
}else{
if( userFramesPerBuffer > absoluteMaximumBufferSizeFrames ){
effectiveUserFramesPerBuffer = ComputeHostBufferSizeGivenHardUpperLimit( userFramesPerBuffer, absoluteMaximumBufferSizeFrames );
assert( effectiveUserFramesPerBuffer <= absoluteMaximumBufferSizeFrames );
if( suggestedLatencyFrames < userFramesPerBuffer )
suggestedLatencyFrames = userFramesPerBuffer;
}else{
effectiveUserFramesPerBuffer = userFramesPerBuffer;
}
}
*hostBufferSizeFrames = effectiveUserFramesPerBuffer;
*hostBufferCount = ComputeHostBufferCountForFixedBufferSizeFrames(
suggestedLatencyFrames, *hostBufferSizeFrames, minimumBufferCount );
if( *hostBufferSizeFrames >= userFramesPerBuffer )
{
numberOfUserBuffersPerHostBuffer = ((*hostBufferCount - 1) + (PA_MME_TARGET_HOST_BUFFER_COUNT_-2)) / (PA_MME_TARGET_HOST_BUFFER_COUNT_ - 1);
if( numberOfUserBuffersPerHostBuffer > 1 )
{
unsigned long maxCoalescedBufferSizeFrames = (absoluteMaximumBufferSizeFrames < preferredMaximumBufferSizeFrames)  
? absoluteMaximumBufferSizeFrames
: preferredMaximumBufferSizeFrames;
unsigned long maxUserBuffersPerHostBuffer = maxCoalescedBufferSizeFrames / effectiveUserFramesPerBuffer;  
if( numberOfUserBuffersPerHostBuffer > maxUserBuffersPerHostBuffer )
numberOfUserBuffersPerHostBuffer = maxUserBuffersPerHostBuffer;
*hostBufferSizeFrames = effectiveUserFramesPerBuffer * numberOfUserBuffersPerHostBuffer;
*hostBufferCount = ComputeHostBufferCountForFixedBufferSizeFrames(
suggestedLatencyFrames, *hostBufferSizeFrames, minimumBufferCount );
}
}
return paNoError;
}
static PaError CalculateMaxHostSampleFrameSizeBytes(
int channelCount,
PaSampleFormat hostSampleFormat,
const PaWinMmeStreamInfo *streamInfo,
int *hostSampleFrameSizeBytes )
{
unsigned int i;
int maxDeviceChannelCount = channelCount;
int hostSampleSizeBytes = Pa_GetSampleSize( hostSampleFormat );
if( hostSampleSizeBytes < 0 )
{
return hostSampleSizeBytes;  
}
if( streamInfo && ( streamInfo->flags & paWinMmeUseMultipleDevices ) )
{
maxDeviceChannelCount = streamInfo->devices[0].channelCount;
for( i=1; i< streamInfo->deviceCount; ++i )
{
if( streamInfo->devices[i].channelCount > maxDeviceChannelCount )
maxDeviceChannelCount = streamInfo->devices[i].channelCount;
}
}
*hostSampleFrameSizeBytes = hostSampleSizeBytes * maxDeviceChannelCount;
return paNoError;
}
static PaError CalculateBufferSettings(
unsigned long *hostFramesPerInputBuffer, unsigned long *hostInputBufferCount,
unsigned long *hostFramesPerOutputBuffer, unsigned long *hostOutputBufferCount,
int inputChannelCount, PaSampleFormat hostInputSampleFormat,
PaTime suggestedInputLatency, const PaWinMmeStreamInfo *inputStreamInfo,
int outputChannelCount, PaSampleFormat hostOutputSampleFormat,
PaTime suggestedOutputLatency, const PaWinMmeStreamInfo *outputStreamInfo,
double sampleRate, unsigned long userFramesPerBuffer )
{
PaError result = paNoError;
if( inputChannelCount > 0 )  
{
int hostInputFrameSizeBytes;
result = CalculateMaxHostSampleFrameSizeBytes(
inputChannelCount, hostInputSampleFormat, inputStreamInfo, &hostInputFrameSizeBytes );
if( result != paNoError )
goto error;
if( inputStreamInfo
&& ( inputStreamInfo->flags & paWinMmeUseLowLevelLatencyParameters ) )
{
if( inputStreamInfo->bufferCount <= 0
|| inputStreamInfo->framesPerBuffer <= 0 )
{
result = paIncompatibleHostApiSpecificStreamInfo;
goto error;
}
*hostFramesPerInputBuffer = inputStreamInfo->framesPerBuffer;
*hostInputBufferCount = inputStreamInfo->bufferCount;
}
else
{
unsigned long minimumBufferCount = (outputChannelCount > 0)
? PA_MME_MIN_HOST_INPUT_BUFFER_COUNT_FULL_DUPLEX_
: PA_MME_MIN_HOST_INPUT_BUFFER_COUNT_HALF_DUPLEX_;
result = SelectHostBufferSizeFramesAndHostBufferCount(
(unsigned long)(suggestedInputLatency * sampleRate),  
userFramesPerBuffer,
minimumBufferCount,
(unsigned long)(PA_MME_MAX_HOST_BUFFER_SECS_ * sampleRate),  
(PA_MME_MAX_HOST_BUFFER_BYTES_ / hostInputFrameSizeBytes),   
hostFramesPerInputBuffer,
hostInputBufferCount );
if( result != paNoError )
goto error;
}
}
else
{
*hostFramesPerInputBuffer = 0;
*hostInputBufferCount = 0;
}
if( outputChannelCount > 0 )  
{
if( outputStreamInfo
&& ( outputStreamInfo->flags & paWinMmeUseLowLevelLatencyParameters ) )
{
if( outputStreamInfo->bufferCount <= 0
|| outputStreamInfo->framesPerBuffer <= 0 )
{
result = paIncompatibleHostApiSpecificStreamInfo;
goto error;
}
*hostFramesPerOutputBuffer = outputStreamInfo->framesPerBuffer;
*hostOutputBufferCount = outputStreamInfo->bufferCount;
if( inputChannelCount > 0 )  
{
if( *hostFramesPerInputBuffer != *hostFramesPerOutputBuffer )
{
if( inputStreamInfo
&& ( inputStreamInfo->flags & paWinMmeUseLowLevelLatencyParameters ) )
{
if( *hostFramesPerInputBuffer < *hostFramesPerOutputBuffer )
{
if( *hostFramesPerOutputBuffer % *hostFramesPerInputBuffer != 0 )
{
result = paIncompatibleHostApiSpecificStreamInfo;
goto error;
}
}
else
{
assert( *hostFramesPerInputBuffer > *hostFramesPerOutputBuffer );
if( *hostFramesPerInputBuffer % *hostFramesPerOutputBuffer != 0 )
{
result = paIncompatibleHostApiSpecificStreamInfo;
goto error;
}
}
}
else
{
*hostFramesPerInputBuffer = *hostFramesPerOutputBuffer;
*hostInputBufferCount = ComputeHostBufferCountForFixedBufferSizeFrames(
(unsigned long)(suggestedInputLatency * sampleRate),
*hostFramesPerInputBuffer,
PA_MME_MIN_HOST_INPUT_BUFFER_COUNT_FULL_DUPLEX_ );
}
}
}
}
else
{
int hostOutputFrameSizeBytes;
result = CalculateMaxHostSampleFrameSizeBytes(
outputChannelCount, hostOutputSampleFormat, outputStreamInfo, &hostOutputFrameSizeBytes );
if( result != paNoError )
goto error;
result = SelectHostBufferSizeFramesAndHostBufferCount(
(unsigned long)(suggestedOutputLatency * sampleRate),  
userFramesPerBuffer,
PA_MME_MIN_HOST_OUTPUT_BUFFER_COUNT_,
(unsigned long)(PA_MME_MAX_HOST_BUFFER_SECS_ * sampleRate),  
(PA_MME_MAX_HOST_BUFFER_BYTES_ / hostOutputFrameSizeBytes),  
hostFramesPerOutputBuffer,
hostOutputBufferCount );
if( result != paNoError )
goto error;
if( inputChannelCount > 0 )  
{
if( *hostFramesPerOutputBuffer != *hostFramesPerInputBuffer )
{
if( *hostFramesPerInputBuffer < *hostFramesPerOutputBuffer )
{
*hostFramesPerOutputBuffer = *hostFramesPerInputBuffer;
*hostOutputBufferCount = ComputeHostBufferCountForFixedBufferSizeFrames(
(unsigned long)(suggestedOutputLatency * sampleRate),
*hostOutputBufferCount,
PA_MME_MIN_HOST_OUTPUT_BUFFER_COUNT_ );
}
else
{
*hostFramesPerInputBuffer = *hostFramesPerOutputBuffer;
*hostInputBufferCount = ComputeHostBufferCountForFixedBufferSizeFrames(
(unsigned long)(suggestedInputLatency * sampleRate),
*hostFramesPerInputBuffer,
PA_MME_MIN_HOST_INPUT_BUFFER_COUNT_FULL_DUPLEX_ );
}
}
}
}
}
else
{
*hostFramesPerOutputBuffer = 0;
*hostOutputBufferCount = 0;
}
error:
return result;
}
typedef struct
{
HANDLE bufferEvent;
void *waveHandles;
unsigned int deviceCount;
WAVEHDR **waveHeaders;                   
unsigned int bufferCount;
unsigned int currentBufferIndex;
unsigned int framesPerBuffer;
unsigned int framesUsedInCurrentBuffer;
}PaWinMmeSingleDirectionHandlesAndBuffers;
static void InitializeSingleDirectionHandlesAndBuffers( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers );
static PaError InitializeWaveHandles( PaWinMmeHostApiRepresentation *winMmeHostApi,
PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers,
unsigned long winMmeSpecificFlags,
unsigned long bytesPerHostSample,
double sampleRate, PaWinMmeDeviceAndChannelCount *devices,
unsigned int deviceCount, PaWinWaveFormatChannelMask channelMask, int isInput );
static PaError TerminateWaveHandles( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers, int isInput, int currentlyProcessingAnError );
static PaError InitializeWaveHeaders( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers,
unsigned long hostBufferCount,
PaSampleFormat hostSampleFormat,
unsigned long framesPerHostBuffer,
PaWinMmeDeviceAndChannelCount *devices,
int isInput );
static void TerminateWaveHeaders( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers, int isInput );
static void InitializeSingleDirectionHandlesAndBuffers( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers )
{
handlesAndBuffers->bufferEvent = 0;
handlesAndBuffers->waveHandles = 0;
handlesAndBuffers->deviceCount = 0;
handlesAndBuffers->waveHeaders = 0;
handlesAndBuffers->bufferCount = 0;
}
static PaError InitializeWaveHandles( PaWinMmeHostApiRepresentation *winMmeHostApi,
PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers,
unsigned long winMmeSpecificFlags,
unsigned long bytesPerHostSample,
double sampleRate, PaWinMmeDeviceAndChannelCount *devices,
unsigned int deviceCount, PaWinWaveFormatChannelMask channelMask, int isInput )
{
PaError result;
MMRESULT mmresult;
signed int i, j;
PaSampleFormat sampleFormat;
int waveFormatTag;
result = CreateEventWithPaError( &handlesAndBuffers->bufferEvent, NULL, FALSE, FALSE, NULL );
if( result != paNoError ) goto error;
if( isInput )
handlesAndBuffers->waveHandles = (void*)PaUtil_AllocateMemory( sizeof(HWAVEIN) * deviceCount );
else
handlesAndBuffers->waveHandles = (void*)PaUtil_AllocateMemory( sizeof(HWAVEOUT) * deviceCount );
if( !handlesAndBuffers->waveHandles )
{
result = paInsufficientMemory;
goto error;
}
handlesAndBuffers->deviceCount = deviceCount;
for( i = 0; i < (signed int)deviceCount; ++i )
{
if( isInput )
((HWAVEIN*)handlesAndBuffers->waveHandles)[i] = 0;
else
((HWAVEOUT*)handlesAndBuffers->waveHandles)[i] = 0;
}
sampleFormat = paInt16;
waveFormatTag = SampleFormatAndWinWmmeSpecificFlagsToLinearWaveFormatTag( sampleFormat, winMmeSpecificFlags );
for( i = 0; i < (signed int)deviceCount; ++i )
{
PaWinWaveFormat waveFormat;
UINT winMmeDeviceId = LocalDeviceIndexToWinMmeDeviceId( winMmeHostApi, devices[i].device );
for( j = 0; j < 2; ++j )
{
switch(j){
case 0:
PaWin_InitializeWaveFormatExtensible( &waveFormat, devices[i].channelCount,
sampleFormat, waveFormatTag, sampleRate, channelMask );
break;
case 1:
PaWin_InitializeWaveFormatEx( &waveFormat, devices[i].channelCount,
sampleFormat, waveFormatTag, sampleRate );
break;
}
if( isInput )
{
mmresult = waveInOpen( &((HWAVEIN*)handlesAndBuffers->waveHandles)[i], winMmeDeviceId,
(WAVEFORMATEX*)&waveFormat,
(DWORD_PTR)handlesAndBuffers->bufferEvent, (DWORD_PTR)0, CALLBACK_EVENT );
}
else
{
mmresult = waveOutOpen( &((HWAVEOUT*)handlesAndBuffers->waveHandles)[i], winMmeDeviceId,
(WAVEFORMATEX*)&waveFormat,
(DWORD_PTR)handlesAndBuffers->bufferEvent, (DWORD_PTR)0, CALLBACK_EVENT );
}
if( mmresult == MMSYSERR_NOERROR )
{
break;  
}
else if( j == 0 )
{
continue;  
}
else
{
switch( mmresult )
{
case MMSYSERR_ALLOCATED:     
result = paDeviceUnavailable;
break;
case MMSYSERR_NODRIVER:      
result = paDeviceUnavailable;
break;
case MMSYSERR_NOMEM:         
result = paInsufficientMemory;
break;
case MMSYSERR_BADDEVICEID:   
case WAVERR_BADFORMAT:       
default:
result = paUnanticipatedHostError;
if( isInput )
{
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
}
else
{
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
}
}
goto error;
}
}
}
return result;
error:
TerminateWaveHandles( handlesAndBuffers, isInput, 1   );
return result;
}
static PaError TerminateWaveHandles( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers, int isInput, int currentlyProcessingAnError )
{
PaError result = paNoError;
MMRESULT mmresult;
signed int i;
if( handlesAndBuffers->waveHandles )
{
for( i = handlesAndBuffers->deviceCount-1; i >= 0; --i )
{
if( isInput )
{
if( ((HWAVEIN*)handlesAndBuffers->waveHandles)[i] )
mmresult = waveInClose( ((HWAVEIN*)handlesAndBuffers->waveHandles)[i] );
else
mmresult = MMSYSERR_NOERROR;
}
else
{
if( ((HWAVEOUT*)handlesAndBuffers->waveHandles)[i] )
mmresult = waveOutClose( ((HWAVEOUT*)handlesAndBuffers->waveHandles)[i] );
else
mmresult = MMSYSERR_NOERROR;
}
if( mmresult != MMSYSERR_NOERROR &&
!currentlyProcessingAnError )  
{
result = paUnanticipatedHostError;
if( isInput )
{
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
}
else
{
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
}
}
}
PaUtil_FreeMemory( handlesAndBuffers->waveHandles );
handlesAndBuffers->waveHandles = 0;
}
if( handlesAndBuffers->bufferEvent )
{
result = CloseHandleWithPaError( handlesAndBuffers->bufferEvent );
handlesAndBuffers->bufferEvent = 0;
}
return result;
}
static PaError InitializeWaveHeaders( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers,
unsigned long hostBufferCount,
PaSampleFormat hostSampleFormat,
unsigned long framesPerHostBuffer,
PaWinMmeDeviceAndChannelCount *devices,
int isInput )
{
PaError result = paNoError;
MMRESULT mmresult;
WAVEHDR *deviceWaveHeaders;
signed int i, j;
handlesAndBuffers->waveHeaders = (WAVEHDR**)PaUtil_AllocateMemory( sizeof(WAVEHDR*) * handlesAndBuffers->deviceCount );
if( !handlesAndBuffers->waveHeaders )
{
result = paInsufficientMemory;
goto error;
}
for( i = 0; i < (signed int)handlesAndBuffers->deviceCount; ++i )
handlesAndBuffers->waveHeaders[i] = 0;
handlesAndBuffers->bufferCount = hostBufferCount;
for( i = 0; i < (signed int)handlesAndBuffers->deviceCount; ++i )
{
int bufferBytes = Pa_GetSampleSize( hostSampleFormat ) *
framesPerHostBuffer * devices[i].channelCount;
if( bufferBytes < 0 )
{
result = paInternalError;
goto error;
}
deviceWaveHeaders = (WAVEHDR *) PaUtil_AllocateMemory( sizeof(WAVEHDR)*hostBufferCount );
if( !deviceWaveHeaders )
{
result = paInsufficientMemory;
goto error;
}
for( j=0; j < (signed int)hostBufferCount; ++j )
deviceWaveHeaders[j].lpData = 0;
handlesAndBuffers->waveHeaders[i] = deviceWaveHeaders;
for( j=0; j < (signed int)hostBufferCount; ++j )
{
deviceWaveHeaders[j].lpData = (char *)PaUtil_AllocateMemory( bufferBytes );
if( !deviceWaveHeaders[j].lpData )
{
result = paInsufficientMemory;
goto error;
}
deviceWaveHeaders[j].dwBufferLength = bufferBytes;
deviceWaveHeaders[j].dwUser = 0xFFFFFFFF;  
if( isInput )
{
mmresult = waveInPrepareHeader( ((HWAVEIN*)handlesAndBuffers->waveHandles)[i], &deviceWaveHeaders[j], sizeof(WAVEHDR) );
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
goto error;
}
}
else  
{
mmresult = waveOutPrepareHeader( ((HWAVEOUT*)handlesAndBuffers->waveHandles)[i], &deviceWaveHeaders[j], sizeof(WAVEHDR) );
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
goto error;
}
}
deviceWaveHeaders[j].dwUser = devices[i].channelCount;
}
}
return result;
error:
TerminateWaveHeaders( handlesAndBuffers, isInput );
return result;
}
static void TerminateWaveHeaders( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers, int isInput )
{
signed int i, j;
WAVEHDR *deviceWaveHeaders;
if( handlesAndBuffers->waveHeaders )
{
for( i = handlesAndBuffers->deviceCount-1; i >= 0 ; --i )
{
deviceWaveHeaders = handlesAndBuffers->waveHeaders[i];   
if( deviceWaveHeaders )
{
for( j = handlesAndBuffers->bufferCount-1; j >= 0; --j )
{
if( deviceWaveHeaders[j].lpData )
{
if( deviceWaveHeaders[j].dwUser != 0xFFFFFFFF )
{
if( isInput )
waveInUnprepareHeader( ((HWAVEIN*)handlesAndBuffers->waveHandles)[i], &deviceWaveHeaders[j], sizeof(WAVEHDR) );
else
waveOutUnprepareHeader( ((HWAVEOUT*)handlesAndBuffers->waveHandles)[i], &deviceWaveHeaders[j], sizeof(WAVEHDR) );
}
PaUtil_FreeMemory( deviceWaveHeaders[j].lpData );
}
}
PaUtil_FreeMemory( deviceWaveHeaders );
}
}
PaUtil_FreeMemory( handlesAndBuffers->waveHeaders );
handlesAndBuffers->waveHeaders = 0;
}
}
struct PaWinMmeStream
{
PaUtilStreamRepresentation streamRepresentation;
PaUtilCpuLoadMeasurer cpuLoadMeasurer;
PaUtilBufferProcessor bufferProcessor;
int primeStreamUsingCallback;
PaWinMmeSingleDirectionHandlesAndBuffers input;
PaWinMmeSingleDirectionHandlesAndBuffers output;
HANDLE abortEvent;
HANDLE processingThread;
PA_THREAD_ID processingThreadId;
char throttleProcessingThreadOnOverload;     
int processingThreadPriority;
int highThreadPriority;
int throttledThreadPriority;
unsigned long throttledSleepMsecs;
int isStopped;
volatile int isActive;
volatile int stopProcessing;     
volatile int abortProcessing;    
DWORD allBuffersDurationMs;      
};
static PaError ValidateWinMmeSpecificStreamInfo(
const PaStreamParameters *streamParameters,
const PaWinMmeStreamInfo *streamInfo,
unsigned long *winMmeSpecificFlags,
char *throttleProcessingThreadOnOverload,
unsigned long *deviceCount )
{
if( streamInfo )
{
if( streamInfo->size != sizeof( PaWinMmeStreamInfo )
|| streamInfo->version != 1 )
{
return paIncompatibleHostApiSpecificStreamInfo;
}
*winMmeSpecificFlags = streamInfo->flags;
if( streamInfo->flags & paWinMmeDontThrottleOverloadedProcessingThread )
*throttleProcessingThreadOnOverload = 0;
if( streamInfo->flags & paWinMmeUseMultipleDevices )
{
if( streamParameters->device != paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
*deviceCount = streamInfo->deviceCount;
}
}
return paNoError;
}
static PaError RetrieveDevicesFromStreamParameters(
struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *streamParameters,
const PaWinMmeStreamInfo *streamInfo,
PaWinMmeDeviceAndChannelCount *devices,
unsigned long deviceCount )
{
PaError result = paNoError;
unsigned int i;
int totalChannelCount;
PaDeviceIndex hostApiDevice;
if( streamInfo && streamInfo->flags & paWinMmeUseMultipleDevices )
{
totalChannelCount = 0;
for( i=0; i < deviceCount; ++i )
{
result = PaUtil_DeviceIndexToHostApiDeviceIndex( &hostApiDevice,
streamInfo->devices[i].device, hostApi );
if( result != paNoError )
return result;
devices[i].device = hostApiDevice;
devices[i].channelCount = streamInfo->devices[i].channelCount;
totalChannelCount += devices[i].channelCount;
}
if( totalChannelCount != streamParameters->channelCount )
{
return paInvalidChannelCount;  
}
}
else
{
devices[0].device = streamParameters->device;
devices[0].channelCount = streamParameters->channelCount;
}
return result;
}
static PaError ValidateInputChannelCounts(
struct PaUtilHostApiRepresentation *hostApi,
PaWinMmeDeviceAndChannelCount *devices,
unsigned long deviceCount )
{
unsigned int i;
PaWinMmeDeviceInfo *inputDeviceInfo;
PaError paerror;
for( i=0; i < deviceCount; ++i )
{
if( devices[i].channelCount < 1 )
return paInvalidChannelCount;
inputDeviceInfo =
(PaWinMmeDeviceInfo*)hostApi->deviceInfos[ devices[i].device ];
paerror = IsInputChannelCountSupported( inputDeviceInfo, devices[i].channelCount );
if( paerror != paNoError )
return paerror;
}
return paNoError;
}
static PaError ValidateOutputChannelCounts(
struct PaUtilHostApiRepresentation *hostApi,
PaWinMmeDeviceAndChannelCount *devices,
unsigned long deviceCount )
{
unsigned int i;
PaWinMmeDeviceInfo *outputDeviceInfo;
PaError paerror;
for( i=0; i < deviceCount; ++i )
{
if( devices[i].channelCount < 1 )
return paInvalidChannelCount;
outputDeviceInfo =
(PaWinMmeDeviceInfo*)hostApi->deviceInfos[ devices[i].device ];
paerror = IsOutputChannelCountSupported( outputDeviceInfo, devices[i].channelCount );
if( paerror != paNoError )
return paerror;
}
return paNoError;
}
#define PA_IS_INPUT_STREAM_( stream ) ( stream ->input.waveHandles )
#define PA_IS_OUTPUT_STREAM_( stream ) ( stream ->output.waveHandles )
#define PA_IS_FULL_DUPLEX_STREAM_( stream ) ( stream ->input.waveHandles && stream ->output.waveHandles )
#define PA_IS_HALF_DUPLEX_STREAM_( stream ) ( !(stream ->input.waveHandles && stream ->output.waveHandles) )
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
PaError result;
PaWinMmeHostApiRepresentation *winMmeHostApi = (PaWinMmeHostApiRepresentation*)hostApi;
PaWinMmeStream *stream = 0;
int bufferProcessorIsInitialized = 0;
int streamRepresentationIsInitialized = 0;
PaSampleFormat hostInputSampleFormat, hostOutputSampleFormat;
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
double suggestedInputLatency, suggestedOutputLatency;
PaWinMmeStreamInfo *inputStreamInfo, *outputStreamInfo;
PaWinWaveFormatChannelMask inputChannelMask, outputChannelMask;
unsigned long framesPerHostInputBuffer;
unsigned long hostInputBufferCount;
unsigned long framesPerHostOutputBuffer;
unsigned long hostOutputBufferCount;
unsigned long framesPerBufferProcessorCall;
PaWinMmeDeviceAndChannelCount *inputDevices = 0;     
unsigned long winMmeSpecificInputFlags = 0;
unsigned long inputDeviceCount = 0;
PaWinMmeDeviceAndChannelCount *outputDevices = 0;
unsigned long winMmeSpecificOutputFlags = 0;
unsigned long outputDeviceCount = 0;                 
char throttleProcessingThreadOnOverload = 1;
if( inputParameters )
{
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
suggestedInputLatency = inputParameters->suggestedLatency;
inputDeviceCount = 1;
inputStreamInfo = (PaWinMmeStreamInfo*)inputParameters->hostApiSpecificStreamInfo;
result = ValidateWinMmeSpecificStreamInfo( inputParameters, inputStreamInfo,
&winMmeSpecificInputFlags,
&throttleProcessingThreadOnOverload,
&inputDeviceCount );
if( result != paNoError ) return result;
inputDevices = (PaWinMmeDeviceAndChannelCount*)alloca( sizeof(PaWinMmeDeviceAndChannelCount) * inputDeviceCount );
if( !inputDevices ) return paInsufficientMemory;
result = RetrieveDevicesFromStreamParameters( hostApi, inputParameters, inputStreamInfo, inputDevices, inputDeviceCount );
if( result != paNoError ) return result;
result = ValidateInputChannelCounts( hostApi, inputDevices, inputDeviceCount );
if( result != paNoError ) return result;
hostInputSampleFormat =
PaUtil_SelectClosestAvailableFormat( paInt16  , inputSampleFormat );
if( inputDeviceCount != 1 ){
inputChannelMask = PAWIN_SPEAKER_DIRECTOUT;
}
else
{
if( inputStreamInfo && inputStreamInfo->flags & paWinMmeUseChannelMask )
inputChannelMask = inputStreamInfo->channelMask;
else
inputChannelMask = PaWin_DefaultChannelMask( inputDevices[0].channelCount );
}
}
else
{
inputChannelCount = 0;
inputSampleFormat = 0;
suggestedInputLatency = 0.;
inputStreamInfo = 0;
hostInputSampleFormat = 0;
}
if( outputParameters )
{
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
suggestedOutputLatency = outputParameters->suggestedLatency;
outputDeviceCount = 1;
outputStreamInfo = (PaWinMmeStreamInfo*)outputParameters->hostApiSpecificStreamInfo;
result = ValidateWinMmeSpecificStreamInfo( outputParameters, outputStreamInfo,
&winMmeSpecificOutputFlags,
&throttleProcessingThreadOnOverload,
&outputDeviceCount );
if( result != paNoError ) return result;
outputDevices = (PaWinMmeDeviceAndChannelCount*)alloca( sizeof(PaWinMmeDeviceAndChannelCount) * outputDeviceCount );
if( !outputDevices ) return paInsufficientMemory;
result = RetrieveDevicesFromStreamParameters( hostApi, outputParameters, outputStreamInfo, outputDevices, outputDeviceCount );
if( result != paNoError ) return result;
result = ValidateOutputChannelCounts( hostApi, outputDevices, outputDeviceCount );
if( result != paNoError ) return result;
hostOutputSampleFormat =
PaUtil_SelectClosestAvailableFormat( paInt16  , outputSampleFormat );
if( outputDeviceCount != 1 ){
outputChannelMask = PAWIN_SPEAKER_DIRECTOUT;
}
else
{
if( outputStreamInfo && outputStreamInfo->flags & paWinMmeUseChannelMask )
outputChannelMask = outputStreamInfo->channelMask;
else
outputChannelMask = PaWin_DefaultChannelMask( outputDevices[0].channelCount );
}
}
else
{
outputChannelCount = 0;
outputSampleFormat = 0;
outputStreamInfo = 0;
hostOutputSampleFormat = 0;
suggestedOutputLatency = 0.;
}
if( (streamFlags & paPlatformSpecificFlags) != 0 )
return paInvalidFlag;  
if( (winMmeSpecificOutputFlags & paWinMmeWaveFormatDolbyAc3Spdif)
|| (winMmeSpecificOutputFlags & paWinMmeWaveFormatWmaSpdif) ){
streamFlags = streamFlags | paClipOff | paDitherOff;
}
result = CalculateBufferSettings( &framesPerHostInputBuffer, &hostInputBufferCount,
&framesPerHostOutputBuffer, &hostOutputBufferCount,
inputChannelCount, hostInputSampleFormat, suggestedInputLatency, inputStreamInfo,
outputChannelCount, hostOutputSampleFormat, suggestedOutputLatency, outputStreamInfo,
sampleRate, framesPerBuffer );
if( result != paNoError ) goto error;
stream = (PaWinMmeStream*)PaUtil_AllocateMemory( sizeof(PaWinMmeStream) );
if( !stream )
{
result = paInsufficientMemory;
goto error;
}
InitializeSingleDirectionHandlesAndBuffers( &stream->input );
InitializeSingleDirectionHandlesAndBuffers( &stream->output );
stream->abortEvent = 0;
stream->processingThread = 0;
stream->throttleProcessingThreadOnOverload = throttleProcessingThreadOnOverload;
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
( (streamCallback)
? &winMmeHostApi->callbackStreamInterface
: &winMmeHostApi->blockingStreamInterface ),
streamCallback, userData );
streamRepresentationIsInitialized = 1;
PaUtil_InitializeCpuLoadMeasurer( &stream->cpuLoadMeasurer, sampleRate );
if( inputParameters && outputParameters )  
{
if( framesPerHostInputBuffer < framesPerHostOutputBuffer )
{
assert( (framesPerHostOutputBuffer % framesPerHostInputBuffer) == 0 );  
framesPerBufferProcessorCall = framesPerHostInputBuffer;
}
else
{
assert( (framesPerHostInputBuffer % framesPerHostOutputBuffer) == 0 );  
framesPerBufferProcessorCall = framesPerHostOutputBuffer;
}
}
else if( inputParameters )
{
framesPerBufferProcessorCall = framesPerHostInputBuffer;
}
else if( outputParameters )
{
framesPerBufferProcessorCall = framesPerHostOutputBuffer;
}
stream->input.framesPerBuffer = framesPerHostInputBuffer;
stream->output.framesPerBuffer = framesPerHostOutputBuffer;
result =  PaUtil_InitializeBufferProcessor( &stream->bufferProcessor,
inputChannelCount, inputSampleFormat, hostInputSampleFormat,
outputChannelCount, outputSampleFormat, hostOutputSampleFormat,
sampleRate, streamFlags, framesPerBuffer,
framesPerBufferProcessorCall, paUtilFixedHostBufferSize,
streamCallback, userData );
if( result != paNoError ) goto error;
bufferProcessorIsInitialized = 1;
stream->streamRepresentation.streamInfo.inputLatency =
(double)(PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor)
+ framesPerHostInputBuffer) / sampleRate;
stream->streamRepresentation.streamInfo.outputLatency =
(double)(PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor)
+ (framesPerHostOutputBuffer * (hostOutputBufferCount-1))) / sampleRate;
stream->streamRepresentation.streamInfo.sampleRate = sampleRate;
stream->primeStreamUsingCallback = ( (streamFlags&paPrimeOutputBuffersUsingStreamCallback) && streamCallback ) ? 1 : 0;
stream->throttledSleepMsecs =
(unsigned long)(stream->bufferProcessor.framesPerHostBuffer *
stream->bufferProcessor.samplePeriod * .25 * 1000);
stream->isStopped = 1;
stream->isActive = 0;
if( inputParameters )
{
result = InitializeWaveHandles( winMmeHostApi, &stream->input,
winMmeSpecificInputFlags,
stream->bufferProcessor.bytesPerHostInputSample, sampleRate,
inputDevices, inputDeviceCount, inputChannelMask, 1   );
if( result != paNoError ) goto error;
}
if( outputParameters )
{
result = InitializeWaveHandles( winMmeHostApi, &stream->output,
winMmeSpecificOutputFlags,
stream->bufferProcessor.bytesPerHostOutputSample, sampleRate,
outputDevices, outputDeviceCount, outputChannelMask, 0   );
if( result != paNoError ) goto error;
}
if( inputParameters )
{
result = InitializeWaveHeaders( &stream->input, hostInputBufferCount,
hostInputSampleFormat, framesPerHostInputBuffer, inputDevices, 1   );
if( result != paNoError ) goto error;
}
if( outputParameters )
{
result = InitializeWaveHeaders( &stream->output, hostOutputBufferCount,
hostOutputSampleFormat, framesPerHostOutputBuffer, outputDevices, 0   );
if( result != paNoError ) goto error;
stream->allBuffersDurationMs = (DWORD) (1000.0 * (framesPerHostOutputBuffer * stream->output.bufferCount) / sampleRate);
}
else
{
stream->allBuffersDurationMs = (DWORD) (1000.0 * (framesPerHostInputBuffer * stream->input.bufferCount) / sampleRate);
}
if( streamCallback )
{
result = CreateEventWithPaError( &stream->abortEvent, NULL, TRUE, FALSE, NULL );
if( result != paNoError ) goto error;
}
*s = (PaStream*)stream;
return result;
error:
if( stream )
{
if( stream->abortEvent )
CloseHandle( stream->abortEvent );
TerminateWaveHeaders( &stream->output, 0   );
TerminateWaveHeaders( &stream->input, 1   );
TerminateWaveHandles( &stream->output, 0  , 1   );
TerminateWaveHandles( &stream->input, 1  , 1   );
if( bufferProcessorIsInitialized )
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
if( streamRepresentationIsInitialized )
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
PaUtil_FreeMemory( stream );
}
return result;
}
static int BuffersAreDone( WAVEHDR **waveHeaders, unsigned int deviceCount, int bufferIndex )
{
unsigned int i;
for( i=0; i < deviceCount; ++i )
{
if( !(waveHeaders[i][ bufferIndex ].dwFlags & WHDR_DONE) )
{
return 0;
}
}
return 1;
}
static int CurrentInputBuffersAreDone( PaWinMmeStream *stream )
{
return BuffersAreDone( stream->input.waveHeaders, stream->input.deviceCount, stream->input.currentBufferIndex );
}
static int CurrentOutputBuffersAreDone( PaWinMmeStream *stream )
{
return BuffersAreDone( stream->output.waveHeaders, stream->output.deviceCount, stream->output.currentBufferIndex );
}
static int NoBuffersAreQueued( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers )
{
unsigned int i, j;
if( handlesAndBuffers->waveHandles )
{
for( i=0; i < handlesAndBuffers->bufferCount; ++i )
{
for( j=0; j < handlesAndBuffers->deviceCount; ++j )
{
if( !( handlesAndBuffers->waveHeaders[ j ][ i ].dwFlags & WHDR_DONE) )
{
return 0;
}
}
}
}
return 1;
}
#define PA_CIRCULAR_INCREMENT_( current, max )\
( (((current) + 1) >= (max)) ? (0) : (current+1) )
#define PA_CIRCULAR_DECREMENT_( current, max )\
( ((current) == 0) ? ((max)-1) : (current-1) )
static signed long GetAvailableFrames( PaWinMmeSingleDirectionHandlesAndBuffers *handlesAndBuffers )
{
signed long result = 0;
unsigned int i;
if( BuffersAreDone( handlesAndBuffers->waveHeaders, handlesAndBuffers->deviceCount, handlesAndBuffers->currentBufferIndex ) )
{
result = handlesAndBuffers->framesPerBuffer - handlesAndBuffers->framesUsedInCurrentBuffer;
i = PA_CIRCULAR_INCREMENT_( handlesAndBuffers->currentBufferIndex, handlesAndBuffers->bufferCount );
while( i != handlesAndBuffers->currentBufferIndex )
{
if( BuffersAreDone( handlesAndBuffers->waveHeaders, handlesAndBuffers->deviceCount, i ) )
{
result += handlesAndBuffers->framesPerBuffer;
i = PA_CIRCULAR_INCREMENT_( i, handlesAndBuffers->bufferCount );
}
else
break;
}
}
return result;
}
static PaError AdvanceToNextInputBuffer( PaWinMmeStream *stream )
{
PaError result = paNoError;
MMRESULT mmresult;
unsigned int i;
for( i=0; i < stream->input.deviceCount; ++i )
{
stream->input.waveHeaders[i][ stream->input.currentBufferIndex ].dwFlags &= ~WHDR_DONE;
mmresult = waveInAddBuffer( ((HWAVEIN*)stream->input.waveHandles)[i],
&stream->input.waveHeaders[i][ stream->input.currentBufferIndex ],
sizeof(WAVEHDR) );
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
}
}
stream->input.currentBufferIndex =
PA_CIRCULAR_INCREMENT_( stream->input.currentBufferIndex, stream->input.bufferCount );
stream->input.framesUsedInCurrentBuffer = 0;
return result;
}
static PaError AdvanceToNextOutputBuffer( PaWinMmeStream *stream )
{
PaError result = paNoError;
MMRESULT mmresult;
unsigned int i;
for( i=0; i < stream->output.deviceCount; ++i )
{
mmresult = waveOutWrite( ((HWAVEOUT*)stream->output.waveHandles)[i],
&stream->output.waveHeaders[i][ stream->output.currentBufferIndex ],
sizeof(WAVEHDR) );
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
}
}
stream->output.currentBufferIndex =
PA_CIRCULAR_INCREMENT_( stream->output.currentBufferIndex, stream->output.bufferCount );
stream->output.framesUsedInCurrentBuffer = 0;
return result;
}
static PaError CatchUpInputBuffers( PaWinMmeStream *stream )
{
PaError result = paNoError;
unsigned int i;
for( i=0; i < stream->input.bufferCount - 1; ++i )
{
result = AdvanceToNextInputBuffer( stream );
if( result != paNoError )
break;
}
return result;
}
static PaError CatchUpOutputBuffers( PaWinMmeStream *stream )
{
PaError result = paNoError;
unsigned int i, j;
unsigned int previousBufferIndex =
PA_CIRCULAR_DECREMENT_( stream->output.currentBufferIndex, stream->output.bufferCount );
for( i=0; i < stream->output.bufferCount - 1; ++i )
{
for( j=0; j < stream->output.deviceCount; ++j )
{
if( stream->output.waveHeaders[j][ stream->output.currentBufferIndex ].lpData
!= stream->output.waveHeaders[j][ previousBufferIndex ].lpData )
{
CopyMemory( stream->output.waveHeaders[j][ stream->output.currentBufferIndex ].lpData,
stream->output.waveHeaders[j][ previousBufferIndex ].lpData,
stream->output.waveHeaders[j][ stream->output.currentBufferIndex ].dwBufferLength );
}
}
result = AdvanceToNextOutputBuffer( stream );
if( result != paNoError )
break;
}
return result;
}
PA_THREAD_FUNC ProcessingThreadProc( void *pArg )
{
PaWinMmeStream *stream = (PaWinMmeStream *)pArg;
HANDLE events[3];
int eventCount = 0;
DWORD result = paNoError;
DWORD waitResult;
DWORD timeout = (unsigned long)(stream->allBuffersDurationMs * 0.5);
int hostBuffersAvailable;
signed int hostInputBufferIndex, hostOutputBufferIndex;
PaStreamCallbackFlags statusFlags;
int callbackResult;
int done = 0;
unsigned int channel, i;
unsigned long framesProcessed;
if( stream->input.bufferEvent )
events[eventCount++] = stream->input.bufferEvent;
if( stream->output.bufferEvent )
events[eventCount++] = stream->output.bufferEvent;
events[eventCount++] = stream->abortEvent;
statusFlags = 0;  
do{
waitResult = WaitForMultipleObjects( eventCount, events, FALSE  , timeout );
if( waitResult == WAIT_FAILED )
{
result = paUnanticipatedHostError;
done = 1;
}
else if( waitResult == WAIT_TIMEOUT )
{
}
if( stream->abortProcessing )
{
done = 1;
}
else if( stream->stopProcessing )
{
if( PA_IS_OUTPUT_STREAM_(stream) )
{
if( NoBuffersAreQueued( &stream->output ) )
done = 1;  
}
else
{
done = 1;  
}
}
else
{
hostBuffersAvailable = 1;
do
{
hostInputBufferIndex = -1;
hostOutputBufferIndex = -1;
if( PA_IS_INPUT_STREAM_(stream) )
{
if( CurrentInputBuffersAreDone( stream ) )
{
if( NoBuffersAreQueued( &stream->input ) )
{
result = CatchUpInputBuffers( stream );
if( result != paNoError )
done = 1;
statusFlags |= paInputOverflow;
}
hostInputBufferIndex = stream->input.currentBufferIndex;
}
}
if( PA_IS_OUTPUT_STREAM_(stream) )
{
if( CurrentOutputBuffersAreDone( stream ) )
{
if( NoBuffersAreQueued( &stream->output ) )
{
result = CatchUpOutputBuffers( stream );
if( result != paNoError )
done = 1;
statusFlags |= paOutputUnderflow;
}
hostOutputBufferIndex = stream->output.currentBufferIndex;
}
}
if( (PA_IS_FULL_DUPLEX_STREAM_(stream) && hostInputBufferIndex != -1 && hostOutputBufferIndex != -1) ||
(PA_IS_HALF_DUPLEX_STREAM_(stream) && ( hostInputBufferIndex != -1 || hostOutputBufferIndex != -1 ) ) )
{
PaStreamCallbackTimeInfo timeInfo = {0,0,0};  
if( PA_IS_OUTPUT_STREAM_(stream) )
{
MMTIME mmtime;
double timeBeforeGetPosition, timeAfterGetPosition;
double time;
long framesInBufferRing;
long writePosition;
long playbackPosition;
HWAVEOUT firstWaveOutDevice = ((HWAVEOUT*)stream->output.waveHandles)[0];
mmtime.wType = TIME_SAMPLES;
timeBeforeGetPosition = PaUtil_GetTime();
waveOutGetPosition( firstWaveOutDevice, &mmtime, sizeof(MMTIME) );
timeAfterGetPosition = PaUtil_GetTime();
timeInfo.currentTime = timeAfterGetPosition;
time = timeBeforeGetPosition + (timeAfterGetPosition - timeBeforeGetPosition) * .5;
framesInBufferRing = stream->output.bufferCount * stream->bufferProcessor.framesPerHostBuffer;
playbackPosition = mmtime.u.sample % framesInBufferRing;
writePosition = stream->output.currentBufferIndex * stream->bufferProcessor.framesPerHostBuffer
+ stream->output.framesUsedInCurrentBuffer;
if( playbackPosition >= writePosition ){
timeInfo.outputBufferDacTime =
time + ((double)( writePosition + (framesInBufferRing - playbackPosition) ) * stream->bufferProcessor.samplePeriod );
}else{
timeInfo.outputBufferDacTime =
time + ((double)( writePosition - playbackPosition ) * stream->bufferProcessor.samplePeriod );
}
}
PaUtil_BeginCpuLoadMeasurement( &stream->cpuLoadMeasurer );
PaUtil_BeginBufferProcessing( &stream->bufferProcessor, &timeInfo, statusFlags  );
statusFlags = 0;
if( PA_IS_INPUT_STREAM_(stream) )
{
PaUtil_SetInputFrameCount( &stream->bufferProcessor, 0   );
channel = 0;
for( i=0; i<stream->input.deviceCount; ++i )
{
int channelCount = (int)stream->input.waveHeaders[i][ hostInputBufferIndex ].dwUser;
PaUtil_SetInterleavedInputChannels( &stream->bufferProcessor, channel,
stream->input.waveHeaders[i][ hostInputBufferIndex ].lpData +
stream->input.framesUsedInCurrentBuffer * channelCount *
stream->bufferProcessor.bytesPerHostInputSample,
channelCount );
channel += channelCount;
}
}
if( PA_IS_OUTPUT_STREAM_(stream) )
{
PaUtil_SetOutputFrameCount( &stream->bufferProcessor, 0   );
channel = 0;
for( i=0; i<stream->output.deviceCount; ++i )
{
int channelCount = (int)stream->output.waveHeaders[i][ hostOutputBufferIndex ].dwUser;
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor, channel,
stream->output.waveHeaders[i][ hostOutputBufferIndex ].lpData +
stream->output.framesUsedInCurrentBuffer * channelCount *
stream->bufferProcessor.bytesPerHostOutputSample,
channelCount );
channel += channelCount;
}
}
callbackResult = paContinue;
framesProcessed = PaUtil_EndBufferProcessing( &stream->bufferProcessor, &callbackResult );
stream->input.framesUsedInCurrentBuffer += framesProcessed;
stream->output.framesUsedInCurrentBuffer += framesProcessed;
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, framesProcessed );
if( callbackResult == paContinue )
{
}
else if( callbackResult == paAbort )
{
stream->abortProcessing = 1;
done = 1;
result = paNoError;
}
else
{
stream->stopProcessing = 1;  
result = paNoError;
}
if( PA_IS_INPUT_STREAM_(stream)
&& stream->stopProcessing == 0 && stream->abortProcessing == 0
&& stream->input.framesUsedInCurrentBuffer == stream->input.framesPerBuffer )
{
if( NoBuffersAreQueued( &stream->input ) )
{
result = CatchUpInputBuffers( stream );
if( result != paNoError )
done = 1;
statusFlags |= paInputOverflow;
}
result = AdvanceToNextInputBuffer( stream );
if( result != paNoError )
done = 1;
}
if( PA_IS_OUTPUT_STREAM_(stream) && !stream->abortProcessing )
{
if( stream->stopProcessing &&
stream->output.framesUsedInCurrentBuffer < stream->output.framesPerBuffer )
{
stream->output.framesUsedInCurrentBuffer += PaUtil_ZeroOutput( &stream->bufferProcessor,
stream->output.framesPerBuffer - stream->output.framesUsedInCurrentBuffer );
}
if( stream->output.framesUsedInCurrentBuffer == stream->output.framesPerBuffer )
{
int outputUnderflow = NoBuffersAreQueued( &stream->output );
result = AdvanceToNextOutputBuffer( stream );
if( result != paNoError )
done = 1;
if( outputUnderflow && !done && !stream->stopProcessing )
{
result = CatchUpOutputBuffers( stream );
if( result != paNoError )
done = 1;
statusFlags |= paOutputUnderflow;
}
}
}
if( stream->throttleProcessingThreadOnOverload != 0 )
{
if( stream->stopProcessing || stream->abortProcessing )
{
if( stream->processingThreadPriority != stream->highThreadPriority )
{
SetThreadPriority( stream->processingThread, stream->highThreadPriority );
stream->processingThreadPriority = stream->highThreadPriority;
}
}
else if( PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer ) > 1. )
{
if( stream->processingThreadPriority != stream->throttledThreadPriority )
{
SetThreadPriority( stream->processingThread, stream->throttledThreadPriority );
stream->processingThreadPriority = stream->throttledThreadPriority;
}
Sleep( stream->throttledSleepMsecs );
}
else
{
if( stream->processingThreadPriority != stream->highThreadPriority )
{
SetThreadPriority( stream->processingThread, stream->highThreadPriority );
stream->processingThreadPriority = stream->highThreadPriority;
}
}
}
}
else
{
hostBuffersAvailable = 0;
}
}
while( hostBuffersAvailable &&
stream->stopProcessing == 0 &&
stream->abortProcessing == 0 &&
!done );
}
}
while( !done );
stream->isActive = 0;
if( stream->streamRepresentation.streamFinishedCallback != 0 )
stream->streamRepresentation.streamFinishedCallback( stream->streamRepresentation.userData );
PaUtil_ResetCpuLoadMeasurer( &stream->cpuLoadMeasurer );
return result;
}
static PaError CloseStream( PaStream* s )
{
PaError result;
PaWinMmeStream *stream = (PaWinMmeStream*)s;
result = CloseHandleWithPaError( stream->abortEvent );
if( result != paNoError ) goto error;
TerminateWaveHeaders( &stream->output, 0   );
TerminateWaveHeaders( &stream->input, 1   );
TerminateWaveHandles( &stream->output, 0  , 0   );
TerminateWaveHandles( &stream->input, 1  , 0   );
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
PaUtil_FreeMemory( stream );
error:
return result;
}
static PaError StartStream( PaStream *s )
{
PaError result;
PaWinMmeStream *stream = (PaWinMmeStream*)s;
MMRESULT mmresult;
unsigned int i, j;
int callbackResult;
unsigned int channel;
unsigned long framesProcessed;
PaStreamCallbackTimeInfo timeInfo = {0,0,0};  
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );
if( PA_IS_INPUT_STREAM_(stream) )
{
for( i=0; i<stream->input.bufferCount; ++i )
{
for( j=0; j<stream->input.deviceCount; ++j )
{
stream->input.waveHeaders[j][i].dwFlags &= ~WHDR_DONE;
mmresult = waveInAddBuffer( ((HWAVEIN*)stream->input.waveHandles)[j], &stream->input.waveHeaders[j][i], sizeof(WAVEHDR) );
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
goto error;
}
}
}
stream->input.currentBufferIndex = 0;
stream->input.framesUsedInCurrentBuffer = 0;
}
if( PA_IS_OUTPUT_STREAM_(stream) )
{
for( i=0; i<stream->output.deviceCount; ++i )
{
if( (mmresult = waveOutPause( ((HWAVEOUT*)stream->output.waveHandles)[i] )) != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
goto error;
}
}
for( i=0; i<stream->output.bufferCount; ++i )
{
if( stream->primeStreamUsingCallback )
{
stream->output.framesUsedInCurrentBuffer = 0;
do{
PaUtil_BeginBufferProcessing( &stream->bufferProcessor,
&timeInfo,
paPrimingOutput | ((stream->input.bufferCount > 0 ) ? paInputUnderflow : 0));
if( stream->input.bufferCount > 0 )
PaUtil_SetNoInput( &stream->bufferProcessor );
PaUtil_SetOutputFrameCount( &stream->bufferProcessor, 0   );
channel = 0;
for( j=0; j<stream->output.deviceCount; ++j )
{
int channelCount = (int)stream->output.waveHeaders[j][i].dwUser;
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor, channel,
stream->output.waveHeaders[j][i].lpData +
stream->output.framesUsedInCurrentBuffer * channelCount *
stream->bufferProcessor.bytesPerHostOutputSample,
channelCount );
channel += channelCount;
}
callbackResult = paContinue;
framesProcessed = PaUtil_EndBufferProcessing( &stream->bufferProcessor, &callbackResult );
stream->output.framesUsedInCurrentBuffer += framesProcessed;
if( callbackResult != paContinue )
{
}
}while( stream->output.framesUsedInCurrentBuffer != stream->output.framesPerBuffer );
}
else
{
for( j=0; j<stream->output.deviceCount; ++j )
{
ZeroMemory( stream->output.waveHeaders[j][i].lpData, stream->output.waveHeaders[j][i].dwBufferLength );
}
}
for( j=0; j<stream->output.deviceCount; ++j )
{
mmresult = waveOutWrite( ((HWAVEOUT*)stream->output.waveHandles)[j], &stream->output.waveHeaders[j][i], sizeof(WAVEHDR) );
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
goto error;
}
}
}
stream->output.currentBufferIndex = 0;
stream->output.framesUsedInCurrentBuffer = 0;
}
stream->isStopped = 0;
stream->isActive = 1;
stream->stopProcessing = 0;
stream->abortProcessing = 0;
result = ResetEventWithPaError( stream->input.bufferEvent );
if( result != paNoError ) goto error;
result = ResetEventWithPaError( stream->output.bufferEvent );
if( result != paNoError ) goto error;
if( stream->streamRepresentation.streamCallback )
{
result = ResetEventWithPaError( stream->abortEvent );
if( result != paNoError ) goto error;
stream->processingThread = CREATE_THREAD;
if( !stream->processingThread )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_SYSTEM_ERROR( GetLastError() );
goto error;
}
stream->highThreadPriority = THREAD_PRIORITY_TIME_CRITICAL;
stream->throttledThreadPriority = THREAD_PRIORITY_NORMAL;
if( !SetThreadPriority( stream->processingThread, stream->highThreadPriority ) )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_SYSTEM_ERROR( GetLastError() );
goto error;
}
stream->processingThreadPriority = stream->highThreadPriority;
}
else
{
}
if( PA_IS_INPUT_STREAM_(stream) )
{
for( i=0; i < stream->input.deviceCount; ++i )
{
mmresult = waveInStart( ((HWAVEIN*)stream->input.waveHandles)[i] );
PA_DEBUG(("Pa_StartStream: waveInStart returned = 0x%X.\n", mmresult));
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
goto error;
}
}
}
if( PA_IS_OUTPUT_STREAM_(stream) )
{
for( i=0; i < stream->output.deviceCount; ++i )
{
if( (mmresult = waveOutRestart( ((HWAVEOUT*)stream->output.waveHandles)[i] )) != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
goto error;
}
}
}
return result;
error:
return result;
}
static PaError StopStream( PaStream *s )
{
PaError result = paNoError;
PaWinMmeStream *stream = (PaWinMmeStream*)s;
int timeout;
DWORD waitResult;
MMRESULT mmresult;
signed int hostOutputBufferIndex;
unsigned int channel, waitCount, i;
if( stream->processingThread )
{
stream->stopProcessing = 1;
timeout = (int)(stream->allBuffersDurationMs * 1.5);
if( timeout < PA_MME_MIN_TIMEOUT_MSEC_ )
timeout = PA_MME_MIN_TIMEOUT_MSEC_;
PA_DEBUG(("WinMME StopStream: waiting for background thread.\n"));
waitResult = WaitForSingleObject( stream->processingThread, timeout );
if( waitResult == WAIT_TIMEOUT )
{
stream->abortProcessing = 1;
SetEvent( stream->abortEvent );
waitResult = WaitForSingleObject( stream->processingThread, timeout );
if( waitResult == WAIT_TIMEOUT )
{
PA_DEBUG(("WinMME StopStream: timed out while waiting for background thread to finish.\n"));
result = paTimedOut;
}
}
CloseHandle( stream->processingThread );
stream->processingThread = NULL;
}
else
{
if( PA_IS_OUTPUT_STREAM_(stream) )
{
if( stream->output.framesUsedInCurrentBuffer > 0 )
{
hostOutputBufferIndex = stream->output.currentBufferIndex;
PaUtil_SetOutputFrameCount( &stream->bufferProcessor,
stream->output.framesPerBuffer - stream->output.framesUsedInCurrentBuffer );
channel = 0;
for( i=0; i<stream->output.deviceCount; ++i )
{
int channelCount = (int)stream->output.waveHeaders[i][ hostOutputBufferIndex ].dwUser;
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor, channel,
stream->output.waveHeaders[i][ hostOutputBufferIndex ].lpData +
stream->output.framesUsedInCurrentBuffer * channelCount *
stream->bufferProcessor.bytesPerHostOutputSample,
channelCount );
channel += channelCount;
}
PaUtil_ZeroOutput( &stream->bufferProcessor,
stream->output.framesPerBuffer - stream->output.framesUsedInCurrentBuffer );
AdvanceToNextOutputBuffer( stream );
}
timeout = (stream->allBuffersDurationMs / stream->output.bufferCount) + 1;
if( timeout < PA_MME_MIN_TIMEOUT_MSEC_ )
timeout = PA_MME_MIN_TIMEOUT_MSEC_;
waitCount = 0;
while( !NoBuffersAreQueued( &stream->output ) && waitCount <= stream->output.bufferCount )
{
waitResult = WaitForSingleObject( stream->output.bufferEvent, timeout );
if( waitResult == WAIT_FAILED )
{
break;
}
else if( waitResult == WAIT_TIMEOUT )
{
}
++waitCount;
}
}
}
if( PA_IS_OUTPUT_STREAM_(stream) )
{
for( i =0; i < stream->output.deviceCount; ++i )
{
mmresult = waveOutReset( ((HWAVEOUT*)stream->output.waveHandles)[i] );
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
}
}
}
if( PA_IS_INPUT_STREAM_(stream) )
{
for( i=0; i < stream->input.deviceCount; ++i )
{
mmresult = waveInReset( ((HWAVEIN*)stream->input.waveHandles)[i] );
if( mmresult != MMSYSERR_NOERROR )
{
result = paUnanticipatedHostError;
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
}
}
}
stream->isStopped = 1;
stream->isActive = 0;
return result;
}
static PaError AbortStream( PaStream *s )
{
PaError result = paNoError;
PaWinMmeStream *stream = (PaWinMmeStream*)s;
int timeout;
DWORD waitResult;
MMRESULT mmresult;
unsigned int i;
if( stream->processingThread )
{
stream->abortProcessing = 1;
SetEvent( stream->abortEvent );
}
if( PA_IS_OUTPUT_STREAM_(stream) )
{
for( i =0; i < stream->output.deviceCount; ++i )
{
mmresult = waveOutReset( ((HWAVEOUT*)stream->output.waveHandles)[i] );
if( mmresult != MMSYSERR_NOERROR )
{
PA_MME_SET_LAST_WAVEOUT_ERROR( mmresult );
return paUnanticipatedHostError;
}
}
}
if( PA_IS_INPUT_STREAM_(stream) )
{
for( i=0; i < stream->input.deviceCount; ++i )
{
mmresult = waveInReset( ((HWAVEIN*)stream->input.waveHandles)[i] );
if( mmresult != MMSYSERR_NOERROR )
{
PA_MME_SET_LAST_WAVEIN_ERROR( mmresult );
return paUnanticipatedHostError;
}
}
}
if( stream->processingThread )
{
PA_DEBUG(("WinMME AbortStream: waiting for background thread.\n"));
timeout = (int)(stream->allBuffersDurationMs * 1.5);
if( timeout < PA_MME_MIN_TIMEOUT_MSEC_ )
timeout = PA_MME_MIN_TIMEOUT_MSEC_;
waitResult = WaitForSingleObject( stream->processingThread, timeout );
if( waitResult == WAIT_TIMEOUT )
{
PA_DEBUG(("WinMME AbortStream: timed out while waiting for background thread to finish.\n"));
return paTimedOut;
}
CloseHandle( stream->processingThread );
stream->processingThread = NULL;
}
stream->isStopped = 1;
stream->isActive = 0;
return result;
}
static PaError IsStreamStopped( PaStream *s )
{
PaWinMmeStream *stream = (PaWinMmeStream*)s;
return stream->isStopped;
}
static PaError IsStreamActive( PaStream *s )
{
PaWinMmeStream *stream = (PaWinMmeStream*)s;
return stream->isActive;
}
static PaTime GetStreamTime( PaStream *s )
{
(void) s;  
return PaUtil_GetTime();
}
static double GetStreamCpuLoad( PaStream* s )
{
PaWinMmeStream *stream = (PaWinMmeStream*)s;
return PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer );
}
static PaError ReadStream( PaStream* s,
void *buffer,
unsigned long frames )
{
PaError result = paNoError;
PaWinMmeStream *stream = (PaWinMmeStream*)s;
void *userBuffer;
unsigned long framesRead = 0;
unsigned long framesProcessed;
signed int hostInputBufferIndex;
DWORD waitResult;
DWORD timeout = (unsigned long)(stream->allBuffersDurationMs * 0.5);
unsigned int channel, i;
if( PA_IS_INPUT_STREAM_(stream) )
{
if( stream->bufferProcessor.userInputIsInterleaved )
{
userBuffer = buffer;
}
else
{
userBuffer = (void*)alloca( sizeof(void*) * stream->bufferProcessor.inputChannelCount );
if( !userBuffer )
return paInsufficientMemory;
for( i = 0; i<stream->bufferProcessor.inputChannelCount; ++i )
((void**)userBuffer)[i] = ((void**)buffer)[i];
}
do{
if( CurrentInputBuffersAreDone( stream ) )
{
if( NoBuffersAreQueued( &stream->input ) )
{
result = paInputOverflowed;
}
hostInputBufferIndex = stream->input.currentBufferIndex;
PaUtil_SetInputFrameCount( &stream->bufferProcessor,
stream->input.framesPerBuffer - stream->input.framesUsedInCurrentBuffer );
channel = 0;
for( i=0; i<stream->input.deviceCount; ++i )
{
int channelCount = (int)stream->input.waveHeaders[i][ hostInputBufferIndex ].dwUser;
PaUtil_SetInterleavedInputChannels( &stream->bufferProcessor, channel,
stream->input.waveHeaders[i][ hostInputBufferIndex ].lpData +
stream->input.framesUsedInCurrentBuffer * channelCount *
stream->bufferProcessor.bytesPerHostInputSample,
channelCount );
channel += channelCount;
}
framesProcessed = PaUtil_CopyInput( &stream->bufferProcessor, &userBuffer, frames - framesRead );
stream->input.framesUsedInCurrentBuffer += framesProcessed;
if( stream->input.framesUsedInCurrentBuffer == stream->input.framesPerBuffer )
{
result = AdvanceToNextInputBuffer( stream );
if( result != paNoError )
break;
}
framesRead += framesProcessed;
}else{
waitResult = WaitForSingleObject( stream->input.bufferEvent, timeout );
if( waitResult == WAIT_FAILED )
{
result = paUnanticipatedHostError;
break;
}
else if( waitResult == WAIT_TIMEOUT )
{
}
}
}while( framesRead < frames );
}
else
{
result = paCanNotReadFromAnOutputOnlyStream;
}
return result;
}
static PaError WriteStream( PaStream* s,
const void *buffer,
unsigned long frames )
{
PaError result = paNoError;
PaWinMmeStream *stream = (PaWinMmeStream*)s;
const void *userBuffer;
unsigned long framesWritten = 0;
unsigned long framesProcessed;
signed int hostOutputBufferIndex;
DWORD waitResult;
DWORD timeout = (unsigned long)(stream->allBuffersDurationMs * 0.5);
unsigned int channel, i;
if( PA_IS_OUTPUT_STREAM_(stream) )
{
if( stream->bufferProcessor.userOutputIsInterleaved )
{
userBuffer = buffer;
}
else
{
userBuffer = (const void*)alloca( sizeof(void*) * stream->bufferProcessor.outputChannelCount );
if( !userBuffer )
return paInsufficientMemory;
for( i = 0; i<stream->bufferProcessor.outputChannelCount; ++i )
((const void**)userBuffer)[i] = ((const void**)buffer)[i];
}
do{
if( CurrentOutputBuffersAreDone( stream ) )
{
if( NoBuffersAreQueued( &stream->output ) )
{
result = paOutputUnderflowed;
}
hostOutputBufferIndex = stream->output.currentBufferIndex;
PaUtil_SetOutputFrameCount( &stream->bufferProcessor,
stream->output.framesPerBuffer - stream->output.framesUsedInCurrentBuffer );
channel = 0;
for( i=0; i<stream->output.deviceCount; ++i )
{
int channelCount = (int)stream->output.waveHeaders[i][ hostOutputBufferIndex ].dwUser;
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor, channel,
stream->output.waveHeaders[i][ hostOutputBufferIndex ].lpData +
stream->output.framesUsedInCurrentBuffer * channelCount *
stream->bufferProcessor.bytesPerHostOutputSample,
channelCount );
channel += channelCount;
}
framesProcessed = PaUtil_CopyOutput( &stream->bufferProcessor, &userBuffer, frames - framesWritten );
stream->output.framesUsedInCurrentBuffer += framesProcessed;
if( stream->output.framesUsedInCurrentBuffer == stream->output.framesPerBuffer )
{
result = AdvanceToNextOutputBuffer( stream );
if( result != paNoError )
break;
}
framesWritten += framesProcessed;
}
else
{
waitResult = WaitForSingleObject( stream->output.bufferEvent, timeout );
if( waitResult == WAIT_FAILED )
{
result = paUnanticipatedHostError;
break;
}
else if( waitResult == WAIT_TIMEOUT )
{
}
}
}while( framesWritten < frames );
}
else
{
result = paCanNotWriteToAnInputOnlyStream;
}
return result;
}
static signed long GetStreamReadAvailable( PaStream* s )
{
PaWinMmeStream *stream = (PaWinMmeStream*)s;
if( PA_IS_INPUT_STREAM_(stream) )
return GetAvailableFrames( &stream->input );
else
return paCanNotReadFromAnOutputOnlyStream;
}
static signed long GetStreamWriteAvailable( PaStream* s )
{
PaWinMmeStream *stream = (PaWinMmeStream*)s;
if( PA_IS_OUTPUT_STREAM_(stream) )
return GetAvailableFrames( &stream->output );
else
return paCanNotWriteToAnInputOnlyStream;
}
static PaError GetWinMMEStreamPointer( PaWinMmeStream **stream, PaStream *s )
{
PaError result;
PaUtilHostApiRepresentation *hostApi;
PaWinMmeHostApiRepresentation *winMmeHostApi;
result = PaUtil_ValidateStreamPointer( s );
if( result != paNoError )
return result;
result = PaUtil_GetHostApiRepresentation( &hostApi, paMME );
if( result != paNoError )
return result;
winMmeHostApi = (PaWinMmeHostApiRepresentation*)hostApi;
if( PA_STREAM_REP( s )->streamInterface == &winMmeHostApi->callbackStreamInterface
|| PA_STREAM_REP( s )->streamInterface == &winMmeHostApi->blockingStreamInterface )
{
*stream = (PaWinMmeStream *)s;
return paNoError;
}
else
{
return paIncompatibleStreamHostApi;
}
}
int PaWinMME_GetStreamInputHandleCount( PaStream* s )
{
PaWinMmeStream *stream;
PaError result = GetWinMMEStreamPointer( &stream, s );
if( result == paNoError )
return (PA_IS_INPUT_STREAM_(stream)) ? stream->input.deviceCount : 0;
else
return result;
}
HWAVEIN PaWinMME_GetStreamInputHandle( PaStream* s, int handleIndex )
{
PaWinMmeStream *stream;
PaError result = GetWinMMEStreamPointer( &stream, s );
if( result == paNoError
&& PA_IS_INPUT_STREAM_(stream)
&& handleIndex >= 0
&& (unsigned int)handleIndex < stream->input.deviceCount )
return ((HWAVEIN*)stream->input.waveHandles)[handleIndex];
else
return 0;
}
int PaWinMME_GetStreamOutputHandleCount( PaStream* s)
{
PaWinMmeStream *stream;
PaError result = GetWinMMEStreamPointer( &stream, s );
if( result == paNoError )
return (PA_IS_OUTPUT_STREAM_(stream)) ? stream->output.deviceCount : 0;
else
return result;
}
HWAVEOUT PaWinMME_GetStreamOutputHandle( PaStream* s, int handleIndex )
{
PaWinMmeStream *stream;
PaError result = GetWinMMEStreamPointer( &stream, s );
if( result == paNoError
&& PA_IS_OUTPUT_STREAM_(stream)
&& handleIndex >= 0
&& (unsigned int)handleIndex < stream->output.deviceCount )
return ((HWAVEOUT*)stream->output.waveHandles)[handleIndex];
else
return 0;
}

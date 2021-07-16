 
#include <assert.h>
#include <stdio.h>
#include <string.h>  
#define _WIN32_WINNT 0x0400  
#include <initguid.h>  
#include <windows.h>
#include <objbase.h>
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
#define DIRECTSOUND_VERSION 0x0800
#else
#define DIRECTSOUND_VERSION 0x0300
#endif
#include <dsound.h>
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
#include <dsconf.h>
#endif  
#ifndef PA_WIN_DS_USE_WMME_TIMER
#ifndef UNDER_CE
#include <process.h>
#endif
#endif
#include "pa_util.h"
#include "pa_allocation.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_cpuload.h"
#include "pa_process.h"
#include "pa_debugprint.h"
#include "pa_win_util.h"
#include "pa_win_ds.h"
#include "pa_win_ds_dynlink.h"
#include "pa_win_waveformat.h"
#include "pa_win_wdmks_utils.h"
#include "pa_win_coinitialize.h"
#if (defined(WIN32) && (defined(_MSC_VER) && (_MSC_VER >= 1200)))  
#pragma comment( lib, "dsound.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "kernel32.lib" )
#endif
#ifndef PA_WIN_DS_USE_WMME_TIMER
#if !defined(__CYGWIN__) && !defined(UNDER_CE)
#define CREATE_THREAD (HANDLE)_beginthreadex
#undef CLOSE_THREAD_HANDLE  
#define PA_THREAD_FUNC static unsigned WINAPI
#define PA_THREAD_ID unsigned
#else
#define CREATE_THREAD CreateThread
#define CLOSE_THREAD_HANDLE CloseHandle
#define PA_THREAD_FUNC static DWORD WINAPI
#define PA_THREAD_ID DWORD
#endif
#if (defined(UNDER_CE))
#pragma comment(lib, "Coredll.lib")
#elif (defined(WIN32) && (defined(_MSC_VER) && (_MSC_VER >= 1200)))  
#pragma comment(lib, "winmm.lib")
#endif
PA_THREAD_FUNC ProcessingThreadProc( void *pArg );
#if !defined(UNDER_CE)
#define PA_WIN_DS_USE_WAITABLE_TIMER_OBJECT  
#endif
#endif  
#ifndef DWORD_PTR
#if defined(_WIN64)
#define DWORD_PTR unsigned __int64
#else
#define DWORD_PTR unsigned long
#endif
#endif
#define PRINT(x) PA_DEBUG(x);
#define ERR_RPT(x) PRINT(x)
#define DBUG(x)   PRINT(x)
#define DBUGX(x)  PRINT(x)
#define PA_USE_HIGH_LATENCY   (0)
#if PA_USE_HIGH_LATENCY
#define PA_DS_WIN_9X_DEFAULT_LATENCY_     (.500)
#define PA_DS_WIN_NT_DEFAULT_LATENCY_     (.600)
#else
#define PA_DS_WIN_9X_DEFAULT_LATENCY_     (.140)
#define PA_DS_WIN_NT_DEFAULT_LATENCY_     (.280)
#endif
#define PA_DS_WIN_WDM_DEFAULT_LATENCY_    (.120)
#define PA_DS_MINIMUM_POLLING_PERIOD_SECONDS    (0.001)  
#define PA_DS_MAXIMUM_POLLING_PERIOD_SECONDS    (0.100)  
#define PA_DS_POLLING_JITTER_SECONDS            (0.001)  
#define SECONDS_PER_MSEC      (0.001)
#define MSECS_PER_SECOND       (1000)
#ifdef __cplusplus
extern "C"
{
#endif  
PaError PaWinDs_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
#ifdef __cplusplus
}
#endif  
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
#if _WIN32_WINNT >= 0x0602  
#define PA_DS_SET_LAST_DIRECTSOUND_ERROR( hr ) \
PaWinUtil_SetLastSystemErrorInfo( paDirectSound, hr )
#else
#define PA_DS_SET_LAST_DIRECTSOUND_ERROR( hr ) \
PaUtil_SetLastHostErrorInfo( paDirectSound, hr, "DirectSound error" )
#endif
static BOOL CALLBACK CollectGUIDsProcW(LPGUID lpGUID,
LPCWSTR lpszDesc,
LPCWSTR lpszDrvName,
LPVOID lpContext );
typedef struct PaWinDsDeviceInfo
{
PaDeviceInfo        inheritedDeviceInfo;
GUID                guid;
GUID                *lpGUID;
double              sampleRates[3];
char deviceInputChannelCountIsKnown;  
char deviceOutputChannelCountIsKnown;  
} PaWinDsDeviceInfo;
typedef struct
{
PaUtilHostApiRepresentation inheritedHostApiRep;
PaUtilStreamInterface    callbackStreamInterface;
PaUtilStreamInterface    blockingStreamInterface;
PaUtilAllocationGroup   *allocations;
PaWinUtilComInitializationResult comInitializationResult;
} PaWinDsHostApiRepresentation;
typedef struct PaWinDsStream
{
PaUtilStreamRepresentation streamRepresentation;
PaUtilCpuLoadMeasurer cpuLoadMeasurer;
PaUtilBufferProcessor bufferProcessor;
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
LPDIRECTSOUNDFULLDUPLEX8 pDirectSoundFullDuplex8;
#endif
LPDIRECTSOUND        pDirectSound;
LPDIRECTSOUNDBUFFER  pDirectSoundPrimaryBuffer;
LPDIRECTSOUNDBUFFER  pDirectSoundOutputBuffer;
DWORD                outputBufferWriteOffsetBytes;      
INT                  outputBufferSizeBytes;
INT                  outputFrameSizeBytes;
LARGE_INTEGER        perfCounterTicksPerBuffer;  
LARGE_INTEGER        previousPlayTime;
DWORD                previousPlayCursor;
UINT                 outputUnderflowCount;
BOOL                 outputIsRunning;
INT                  finalZeroBytesWritten;  
LPDIRECTSOUNDCAPTURE pDirectSoundCapture;
LPDIRECTSOUNDCAPTUREBUFFER   pDirectSoundInputBuffer;
INT                  inputFrameSizeBytes;
UINT                 readOffset;       
UINT                 inputBufferSizeBytes;
int              hostBufferSizeFrames;  
double           framesWritten;
double           secondsPerHostByte;  
double           pollingPeriodSeconds;
PaStreamCallbackFlags callbackFlags;
PaStreamFlags    streamFlags;
int              callbackResult;
HANDLE           processingCompleted;
volatile int     isStarted;
volatile int     isActive;
volatile int     stopProcessing;  
volatile int     abortProcessing;  
UINT             systemTimerResolutionPeriodMs;  
#ifdef PA_WIN_DS_USE_WMME_TIMER
MMRESULT         timerID;
#else
#ifdef PA_WIN_DS_USE_WAITABLE_TIMER_OBJECT
HANDLE           waitableTimer;
#endif
HANDLE           processingThread;
PA_THREAD_ID     processingThreadId;
HANDLE           processingThreadCompleted;
#endif
} PaWinDsStream;
static double PaWinDS_GetMinSystemLatencySeconds( void )
{
#pragma warning (disable : 4996)  
double minLatencySeconds;
OSVERSIONINFO osvi;
osvi.dwOSVersionInfoSize = sizeof( osvi );
GetVersionEx( &osvi );
DBUG(("PA - PlatformId = 0x%x\n", osvi.dwPlatformId ));
DBUG(("PA - MajorVersion = 0x%x\n", osvi.dwMajorVersion ));
DBUG(("PA - MinorVersion = 0x%x\n", osvi.dwMinorVersion ));
if( (osvi.dwMajorVersion == 4) && (osvi.dwPlatformId == 2) )
{
minLatencySeconds = PA_DS_WIN_NT_DEFAULT_LATENCY_;
}
else if(osvi.dwMajorVersion >= 5)
{
minLatencySeconds = PA_DS_WIN_WDM_DEFAULT_LATENCY_;
}
else
{
minLatencySeconds = PA_DS_WIN_9X_DEFAULT_LATENCY_;
}
return minLatencySeconds;
#pragma warning (default : 4996)
}
#define PA_LATENCY_ENV_NAME  ("PA_MIN_LATENCY_MSEC")
#define PA_ENV_BUF_SIZE  (32)
static double PaWinDs_GetMinLatencySeconds( double sampleRate )
{
char      envbuf[PA_ENV_BUF_SIZE];
DWORD     hresult;
double    minLatencySeconds = 0;
hresult = GetEnvironmentVariableA( PA_LATENCY_ENV_NAME, envbuf, PA_ENV_BUF_SIZE );
if( (hresult > 0) && (hresult < PA_ENV_BUF_SIZE) )
{
minLatencySeconds = atoi( envbuf ) * SECONDS_PER_MSEC;
}
else
{
minLatencySeconds = PaWinDS_GetMinSystemLatencySeconds();
#if PA_USE_HIGH_LATENCY
PRINT(("PA - Minimum Latency set to %f msec!\n", minLatencySeconds * MSECS_PER_SECOND ));
#endif
}
return minLatencySeconds;
}
static char *DuplicateDeviceNameString( PaUtilAllocationGroup *allocations, const wchar_t* src )
{
char *result = 0;
if( src != NULL )
{
size_t len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
result = (char*)PaUtil_GroupAllocateMemory( allocations, (long)(len + 1) );
if( result ) {
if (WideCharToMultiByte(CP_UTF8, 0, src, -1, result, (int)len, NULL, NULL) == 0) {
result = 0;
}
}
}
else
{
result = (char*)PaUtil_GroupAllocateMemory( allocations, 1 );
if( result )
result[0] = '\0';
}
return result;
}
typedef struct DSDeviceNameAndGUID{
char *name;  
GUID guid;
LPGUID lpGUID;
void *pnpInterface;   
} DSDeviceNameAndGUID;
typedef struct DSDeviceNameAndGUIDVector{
PaUtilAllocationGroup *allocations;
PaError enumerationError;
int count;
int free;
DSDeviceNameAndGUID *items;  
} DSDeviceNameAndGUIDVector;
typedef struct DSDeviceNamesAndGUIDs{
PaWinDsHostApiRepresentation *winDsHostApi;
DSDeviceNameAndGUIDVector inputNamesAndGUIDs;
DSDeviceNameAndGUIDVector outputNamesAndGUIDs;
} DSDeviceNamesAndGUIDs;
static PaError InitializeDSDeviceNameAndGUIDVector(
DSDeviceNameAndGUIDVector *guidVector, PaUtilAllocationGroup *allocations )
{
PaError result = paNoError;
guidVector->allocations = allocations;
guidVector->enumerationError = paNoError;
guidVector->count = 0;
guidVector->free = 8;
guidVector->items = (DSDeviceNameAndGUID*)LocalAlloc( LMEM_FIXED, sizeof(DSDeviceNameAndGUID) * guidVector->free );
if( guidVector->items == NULL )
result = paInsufficientMemory;
return result;
}
static PaError ExpandDSDeviceNameAndGUIDVector( DSDeviceNameAndGUIDVector *guidVector )
{
PaError result = paNoError;
DSDeviceNameAndGUID *newItems;
int i;
int size = guidVector->count + guidVector->free;
guidVector->free += size;
newItems = (DSDeviceNameAndGUID*)LocalAlloc( LMEM_FIXED, sizeof(DSDeviceNameAndGUID) * size * 2 );
if( newItems == NULL )
{
result = paInsufficientMemory;
}
else
{
for( i=0; i < guidVector->count; ++i )
{
newItems[i].name = guidVector->items[i].name;
if( guidVector->items[i].lpGUID == NULL )
{
newItems[i].lpGUID = NULL;
}
else
{
newItems[i].lpGUID = &newItems[i].guid;
memcpy( &newItems[i].guid, guidVector->items[i].lpGUID, sizeof(GUID) );
}
newItems[i].pnpInterface = guidVector->items[i].pnpInterface;
}
LocalFree( guidVector->items );
guidVector->items = newItems;
}
return result;
}
static PaError TerminateDSDeviceNameAndGUIDVector( DSDeviceNameAndGUIDVector *guidVector )
{
PaError result = paNoError;
if( guidVector->items != NULL )
{
if( LocalFree( guidVector->items ) != NULL )
result = paInsufficientMemory;               
guidVector->items = NULL;
}
return result;
}
static BOOL CALLBACK CollectGUIDsProcW(LPGUID lpGUID,
LPCWSTR lpszDesc,
LPCWSTR lpszDrvName,
LPVOID lpContext )
{
DSDeviceNameAndGUIDVector *namesAndGUIDs = (DSDeviceNameAndGUIDVector*)lpContext;
PaError error;
(void) lpszDrvName;  
if( namesAndGUIDs->free == 0 )
{
error = ExpandDSDeviceNameAndGUIDVector( namesAndGUIDs );
if( error != paNoError )
{
namesAndGUIDs->enumerationError = error;
return FALSE;
}
}
if( lpGUID == NULL )
{
namesAndGUIDs->items[namesAndGUIDs->count].lpGUID = NULL;
}
else
{
namesAndGUIDs->items[namesAndGUIDs->count].lpGUID =
&namesAndGUIDs->items[namesAndGUIDs->count].guid;
memcpy( &namesAndGUIDs->items[namesAndGUIDs->count].guid, lpGUID, sizeof(GUID) );
}
namesAndGUIDs->items[namesAndGUIDs->count].name =
DuplicateDeviceNameString( namesAndGUIDs->allocations, lpszDesc );
if( namesAndGUIDs->items[namesAndGUIDs->count].name == NULL )
{
namesAndGUIDs->enumerationError = paInsufficientMemory;
return FALSE;
}
namesAndGUIDs->items[namesAndGUIDs->count].pnpInterface = 0;
++namesAndGUIDs->count;
--namesAndGUIDs->free;
return TRUE;
}
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
static void *DuplicateWCharString( PaUtilAllocationGroup *allocations, wchar_t *source )
{
size_t len;
wchar_t *result;
len = wcslen( source );
result = (wchar_t*)PaUtil_GroupAllocateMemory( allocations, (long) ((len+1) * sizeof(wchar_t)) );
wcscpy( result, source );
return result;
}
static BOOL CALLBACK KsPropertySetEnumerateCallback( PDSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_W_DATA data, LPVOID context )
{
int i;
DSDeviceNamesAndGUIDs *deviceNamesAndGUIDs = (DSDeviceNamesAndGUIDs*)context;
if( data->Interface )
{
if( data->DataFlow == DIRECTSOUNDDEVICE_DATAFLOW_RENDER )
{
for( i=0; i < deviceNamesAndGUIDs->outputNamesAndGUIDs.count; ++i )
{
if( deviceNamesAndGUIDs->outputNamesAndGUIDs.items[i].lpGUID
&& memcmp( &data->DeviceId, deviceNamesAndGUIDs->outputNamesAndGUIDs.items[i].lpGUID, sizeof(GUID) ) == 0 )
{
deviceNamesAndGUIDs->outputNamesAndGUIDs.items[i].pnpInterface =
(char*)DuplicateWCharString( deviceNamesAndGUIDs->winDsHostApi->allocations, data->Interface );
break;
}
}
}
else if( data->DataFlow == DIRECTSOUNDDEVICE_DATAFLOW_CAPTURE )
{
for( i=0; i < deviceNamesAndGUIDs->inputNamesAndGUIDs.count; ++i )
{
if( deviceNamesAndGUIDs->inputNamesAndGUIDs.items[i].lpGUID
&& memcmp( &data->DeviceId, deviceNamesAndGUIDs->inputNamesAndGUIDs.items[i].lpGUID, sizeof(GUID) ) == 0 )
{
deviceNamesAndGUIDs->inputNamesAndGUIDs.items[i].pnpInterface =
(char*)DuplicateWCharString( deviceNamesAndGUIDs->winDsHostApi->allocations, data->Interface );
break;
}
}
}
}
return TRUE;
}
static GUID pawin_CLSID_DirectSoundPrivate =
{ 0x11ab3ec0, 0x25ec, 0x11d1, 0xa4, 0xd8, 0x00, 0xc0, 0x4f, 0xc2, 0x8a, 0xca };
static GUID pawin_DSPROPSETID_DirectSoundDevice =
{ 0x84624f82, 0x25ec, 0x11d1, 0xa4, 0xd8, 0x00, 0xc0, 0x4f, 0xc2, 0x8a, 0xca };
static GUID pawin_IID_IKsPropertySet =
{ 0x31efac30, 0x515c, 0x11d0, 0xa9, 0xaa, 0x00, 0xaa, 0x00, 0x61, 0xbe, 0x93 };
static void FindDevicePnpInterfaces( DSDeviceNamesAndGUIDs *deviceNamesAndGUIDs )
{
IClassFactory *pClassFactory;
if( paWinDsDSoundEntryPoints.DllGetClassObject(&pawin_CLSID_DirectSoundPrivate, &IID_IClassFactory, (PVOID *) &pClassFactory) == S_OK ){
IKsPropertySet *pPropertySet;
if( pClassFactory->lpVtbl->CreateInstance( pClassFactory, NULL, &pawin_IID_IKsPropertySet, (PVOID *) &pPropertySet) == S_OK ){
DSPROPERTY_DIRECTSOUNDDEVICE_ENUMERATE_W_DATA data;
ULONG bytesReturned;
data.Callback = KsPropertySetEnumerateCallback;
data.Context = deviceNamesAndGUIDs;
IKsPropertySet_Get( pPropertySet,
&pawin_DSPROPSETID_DirectSoundDevice,
DSPROPERTY_DIRECTSOUNDDEVICE_ENUMERATE_W,
NULL,
0,
&data,
sizeof(data),
&bytesReturned
);
IKsPropertySet_Release( pPropertySet );
}
pClassFactory->lpVtbl->Release( pClassFactory );
}
}
#endif  
GUID IID_IRolandVSCEmulated1 = {0xc2ad1800, 0xb243, 0x11ce, 0xa8, 0xa4, 0x00, 0xaa, 0x00, 0x6c, 0x45, 0x01};
GUID IID_IRolandVSCEmulated2 = {0xc2ad1800, 0xb243, 0x11ce, 0xa8, 0xa4, 0x00, 0xaa, 0x00, 0x6c, 0x45, 0x02};
#define PA_DEFAULTSAMPLERATESEARCHORDER_COUNT_  (13)  
static double defaultSampleRateSearchOrder_[] =
{ 44100.0, 48000.0, 32000.0, 24000.0, 22050.0, 88200.0, 96000.0, 192000.0,
16000.0, 12000.0, 11025.0, 9600.0, 8000.0 };
static PaError AddOutputDeviceInfoFromDirectSound(
PaWinDsHostApiRepresentation *winDsHostApi, char *name, LPGUID lpGUID, char *pnpInterface )
{
PaUtilHostApiRepresentation  *hostApi = &winDsHostApi->inheritedHostApiRep;
PaWinDsDeviceInfo            *winDsDeviceInfo = (PaWinDsDeviceInfo*) hostApi->deviceInfos[hostApi->info.deviceCount];
PaDeviceInfo                 *deviceInfo = &winDsDeviceInfo->inheritedDeviceInfo;
HRESULT                       hr;
LPDIRECTSOUND                 lpDirectSound;
DSCAPS                        caps;
int                           deviceOK = TRUE;
PaError                       result = paNoError;
int                           i;
if( lpGUID == NULL )
{
winDsDeviceInfo->lpGUID = NULL;
}
else
{
memcpy( &winDsDeviceInfo->guid, lpGUID, sizeof(GUID) );
winDsDeviceInfo->lpGUID = &winDsDeviceInfo->guid;
}
if( lpGUID )
{
if (IsEqualGUID (&IID_IRolandVSCEmulated1,lpGUID) ||
IsEqualGUID (&IID_IRolandVSCEmulated2,lpGUID) )
{
PA_DEBUG(("BLACKLISTED: %s \n",name));
return paNoError;
}
}
hr = paWinDsDSoundEntryPoints.DirectSoundCreate( lpGUID, &lpDirectSound, NULL );
if( hr != DS_OK )
{
if (hr == DSERR_ALLOCATED)
PA_DEBUG(("AddOutputDeviceInfoFromDirectSound %s DSERR_ALLOCATED\n",name));
DBUG(("Cannot create DirectSound for %s. Result = 0x%x\n", name, hr ));
if (lpGUID)
DBUG(("%s's GUID: {0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x, 0x%x} \n",
name,
lpGUID->Data1,
lpGUID->Data2,
lpGUID->Data3,
lpGUID->Data4[0],
lpGUID->Data4[1],
lpGUID->Data4[2],
lpGUID->Data4[3],
lpGUID->Data4[4],
lpGUID->Data4[5],
lpGUID->Data4[6],
lpGUID->Data4[7]));
deviceOK = FALSE;
}
else
{
memset( &caps, 0, sizeof(caps) );
caps.dwSize = sizeof(caps);
hr = IDirectSound_GetCaps( lpDirectSound, &caps );
if( hr != DS_OK )
{
DBUG(("Cannot GetCaps() for DirectSound device %s. Result = 0x%x\n", name, hr ));
deviceOK = FALSE;
}
else
{
#if PA_USE_WMME
if( caps.dwFlags & DSCAPS_EMULDRIVER )
{
deviceOK = FALSE;
}
#endif
if( deviceOK )
{
deviceInfo->maxInputChannels = 0;
winDsDeviceInfo->deviceInputChannelCountIsKnown = 1;
if( caps.dwFlags & DSCAPS_PRIMARYSTEREO )
{
deviceInfo->maxOutputChannels = 2;
winDsDeviceInfo->deviceOutputChannelCountIsKnown = 0;
}
else
{
deviceInfo->maxOutputChannels = 1;
winDsDeviceInfo->deviceOutputChannelCountIsKnown = 1;
}
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
if( !pnpInterface )
#endif
{
DWORD spkrcfg;
if( SUCCEEDED(IDirectSound_GetSpeakerConfig( lpDirectSound, &spkrcfg )) )
{
int count = 0;
switch (DSSPEAKER_CONFIG(spkrcfg))
{
case DSSPEAKER_HEADPHONE:        count = 2; break;
case DSSPEAKER_MONO:             count = 1; break;
case DSSPEAKER_QUAD:             count = 4; break;
case DSSPEAKER_STEREO:           count = 2; break;
case DSSPEAKER_SURROUND:         count = 4; break;
case DSSPEAKER_5POINT1:          count = 6; break;
#ifndef DSSPEAKER_7POINT1
#define DSSPEAKER_7POINT1 0x00000007
#endif
case DSSPEAKER_7POINT1:          count = 8; break;
#ifndef DSSPEAKER_7POINT1_SURROUND
#define DSSPEAKER_7POINT1_SURROUND 0x00000008
#endif
case DSSPEAKER_7POINT1_SURROUND: count = 8; break;
#ifndef DSSPEAKER_5POINT1_SURROUND
#define DSSPEAKER_5POINT1_SURROUND 0x00000009
#endif
case DSSPEAKER_5POINT1_SURROUND: count = 6; break;
}
if( count )
{
deviceInfo->maxOutputChannels = count;
winDsDeviceInfo->deviceOutputChannelCountIsKnown = 1;
}
}
}
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
if( pnpInterface )
{
int count = PaWin_WDMKS_QueryFilterMaximumChannelCount( pnpInterface,   0  );
if( count > 0 )
{
deviceInfo->maxOutputChannels = count;
winDsDeviceInfo->deviceOutputChannelCountIsKnown = 1;
}
}
#endif  
if( caps.dwFlags & DSCAPS_CONTINUOUSRATE )
{
deviceInfo->defaultSampleRate = caps.dwMaxSecondarySampleRate;
for( i = 0; i < PA_DEFAULTSAMPLERATESEARCHORDER_COUNT_; ++i )
{
if( defaultSampleRateSearchOrder_[i] >= caps.dwMinSecondarySampleRate
&& defaultSampleRateSearchOrder_[i] <= caps.dwMaxSecondarySampleRate )
{
deviceInfo->defaultSampleRate = defaultSampleRateSearchOrder_[i];
break;
}
}
}
else if( caps.dwMinSecondarySampleRate == caps.dwMaxSecondarySampleRate )
{
if( caps.dwMinSecondarySampleRate == 0 )
{
deviceInfo->defaultSampleRate = 48000.0f;   
DBUG(("PA - Reported rates both zero. Setting to fake values for device #%s\n", name ));
}
else
{
deviceInfo->defaultSampleRate = caps.dwMaxSecondarySampleRate;
}
}
else if( (caps.dwMinSecondarySampleRate < 1000.0) && (caps.dwMaxSecondarySampleRate > 50000.0) )
{
deviceInfo->defaultSampleRate = 48000.0f;   
DBUG(("PA - Sample rate range used instead of two odd values for device #%s\n", name ));
}
else deviceInfo->defaultSampleRate = caps.dwMaxSecondarySampleRate;
deviceInfo->defaultLowInputLatency = 0.;
deviceInfo->defaultHighInputLatency = 0.;
deviceInfo->defaultLowOutputLatency = PaWinDs_GetMinLatencySeconds( deviceInfo->defaultSampleRate );
deviceInfo->defaultHighOutputLatency = deviceInfo->defaultLowOutputLatency * 2;
}
}
IDirectSound_Release( lpDirectSound );
}
if( deviceOK )
{
deviceInfo->name = name;
if( lpGUID == NULL )
hostApi->info.defaultOutputDevice = hostApi->info.deviceCount;
hostApi->info.deviceCount++;
}
return result;
}
static PaError AddInputDeviceInfoFromDirectSoundCapture(
PaWinDsHostApiRepresentation *winDsHostApi, char *name, LPGUID lpGUID, char *pnpInterface )
{
PaUtilHostApiRepresentation  *hostApi = &winDsHostApi->inheritedHostApiRep;
PaWinDsDeviceInfo            *winDsDeviceInfo = (PaWinDsDeviceInfo*) hostApi->deviceInfos[hostApi->info.deviceCount];
PaDeviceInfo                 *deviceInfo = &winDsDeviceInfo->inheritedDeviceInfo;
HRESULT                       hr;
LPDIRECTSOUNDCAPTURE          lpDirectSoundCapture;
DSCCAPS                       caps;
int                           deviceOK = TRUE;
PaError                       result = paNoError;
if( lpGUID == NULL )
{
winDsDeviceInfo->lpGUID = NULL;
}
else
{
winDsDeviceInfo->lpGUID = &winDsDeviceInfo->guid;
memcpy( &winDsDeviceInfo->guid, lpGUID, sizeof(GUID) );
}
hr = paWinDsDSoundEntryPoints.DirectSoundCaptureCreate( lpGUID, &lpDirectSoundCapture, NULL );
if( hr != DS_OK )
{
DBUG(("Cannot create Capture for %s. Result = 0x%x\n", name, hr ));
deviceOK = FALSE;
}
else
{
memset( &caps, 0, sizeof(caps) );
caps.dwSize = sizeof(caps);
hr = IDirectSoundCapture_GetCaps( lpDirectSoundCapture, &caps );
if( hr != DS_OK )
{
DBUG(("Cannot GetCaps() for Capture device %s. Result = 0x%x\n", name, hr ));
deviceOK = FALSE;
}
else
{
#if PA_USE_WMME
if( caps.dwFlags & DSCAPS_EMULDRIVER )
{
deviceOK = FALSE;
}
#endif
if( deviceOK )
{
deviceInfo->maxInputChannels = caps.dwChannels;
winDsDeviceInfo->deviceInputChannelCountIsKnown = 1;
deviceInfo->maxOutputChannels = 0;
winDsDeviceInfo->deviceOutputChannelCountIsKnown = 1;
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
if( pnpInterface )
{
int count = PaWin_WDMKS_QueryFilterMaximumChannelCount( pnpInterface,   1  );
if( count > 0 )
{
deviceInfo->maxInputChannels = count;
winDsDeviceInfo->deviceInputChannelCountIsKnown = 1;
}
}
#endif  
#ifndef WAVE_FORMAT_48M08
#define WAVE_FORMAT_48M08      0x00001000     
#define WAVE_FORMAT_48S08      0x00002000     
#define WAVE_FORMAT_48M16      0x00004000     
#define WAVE_FORMAT_48S16      0x00008000     
#define WAVE_FORMAT_96M08      0x00010000     
#define WAVE_FORMAT_96S08      0x00020000     
#define WAVE_FORMAT_96M16      0x00040000     
#define WAVE_FORMAT_96S16      0x00080000     
#endif
if( caps.dwChannels == 2 )
{
if( caps.dwFormats & WAVE_FORMAT_4S16 )
deviceInfo->defaultSampleRate = 44100.0;
else if( caps.dwFormats & WAVE_FORMAT_48S16 )
deviceInfo->defaultSampleRate = 48000.0;
else if( caps.dwFormats & WAVE_FORMAT_2S16 )
deviceInfo->defaultSampleRate = 22050.0;
else if( caps.dwFormats & WAVE_FORMAT_1S16 )
deviceInfo->defaultSampleRate = 11025.0;
else if( caps.dwFormats & WAVE_FORMAT_96S16 )
deviceInfo->defaultSampleRate = 96000.0;
else
deviceInfo->defaultSampleRate = 48000.0;  
}
else if( caps.dwChannels == 1 )
{
if( caps.dwFormats & WAVE_FORMAT_4M16 )
deviceInfo->defaultSampleRate = 44100.0;
else if( caps.dwFormats & WAVE_FORMAT_48M16 )
deviceInfo->defaultSampleRate = 48000.0;
else if( caps.dwFormats & WAVE_FORMAT_2M16 )
deviceInfo->defaultSampleRate = 22050.0;
else if( caps.dwFormats & WAVE_FORMAT_1M16 )
deviceInfo->defaultSampleRate = 11025.0;
else if( caps.dwFormats & WAVE_FORMAT_96M16 )
deviceInfo->defaultSampleRate = 96000.0;
else
deviceInfo->defaultSampleRate = 48000.0;  
}
else deviceInfo->defaultSampleRate = 48000.0;  
deviceInfo->defaultLowInputLatency = PaWinDs_GetMinLatencySeconds( deviceInfo->defaultSampleRate );
deviceInfo->defaultHighInputLatency = deviceInfo->defaultLowInputLatency * 2;
deviceInfo->defaultLowOutputLatency = 0.;
deviceInfo->defaultHighOutputLatency = 0.;
}
}
IDirectSoundCapture_Release( lpDirectSoundCapture );
}
if( deviceOK )
{
deviceInfo->name = name;
if( lpGUID == NULL )
hostApi->info.defaultInputDevice = hostApi->info.deviceCount;
hostApi->info.deviceCount++;
}
return result;
}
PaError PaWinDs_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result = paNoError;
int i, deviceCount;
PaWinDsHostApiRepresentation *winDsHostApi;
DSDeviceNamesAndGUIDs deviceNamesAndGUIDs;
PaWinDsDeviceInfo *deviceInfoArray;
PaWinDs_InitializeDSoundEntryPoints();
deviceNamesAndGUIDs.winDsHostApi = NULL;
deviceNamesAndGUIDs.inputNamesAndGUIDs.items = NULL;
deviceNamesAndGUIDs.outputNamesAndGUIDs.items = NULL;
winDsHostApi = (PaWinDsHostApiRepresentation*)PaUtil_AllocateMemory( sizeof(PaWinDsHostApiRepresentation) );
if( !winDsHostApi )
{
result = paInsufficientMemory;
goto error;
}
memset( winDsHostApi, 0, sizeof(PaWinDsHostApiRepresentation) );  
result = PaWinUtil_CoInitialize( paDirectSound, &winDsHostApi->comInitializationResult );
if( result != paNoError )
{
goto error;
}
winDsHostApi->allocations = PaUtil_CreateAllocationGroup();
if( !winDsHostApi->allocations )
{
result = paInsufficientMemory;
goto error;
}
*hostApi = &winDsHostApi->inheritedHostApiRep;
(*hostApi)->info.structVersion = 1;
(*hostApi)->info.type = paDirectSound;
(*hostApi)->info.name = "Windows DirectSound";
(*hostApi)->info.deviceCount = 0;
(*hostApi)->info.defaultInputDevice = paNoDevice;
(*hostApi)->info.defaultOutputDevice = paNoDevice;
result = InitializeDSDeviceNameAndGUIDVector( &deviceNamesAndGUIDs.inputNamesAndGUIDs, winDsHostApi->allocations );
if( result != paNoError )
goto error;
result = InitializeDSDeviceNameAndGUIDVector( &deviceNamesAndGUIDs.outputNamesAndGUIDs, winDsHostApi->allocations );
if( result != paNoError )
goto error;
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateW( (LPDSENUMCALLBACKW)CollectGUIDsProcW, (void *)&deviceNamesAndGUIDs.inputNamesAndGUIDs );
paWinDsDSoundEntryPoints.DirectSoundEnumerateW( (LPDSENUMCALLBACKW)CollectGUIDsProcW, (void *)&deviceNamesAndGUIDs.outputNamesAndGUIDs );
if( deviceNamesAndGUIDs.inputNamesAndGUIDs.enumerationError != paNoError )
{
result = deviceNamesAndGUIDs.inputNamesAndGUIDs.enumerationError;
goto error;
}
if( deviceNamesAndGUIDs.outputNamesAndGUIDs.enumerationError != paNoError )
{
result = deviceNamesAndGUIDs.outputNamesAndGUIDs.enumerationError;
goto error;
}
deviceCount = deviceNamesAndGUIDs.inputNamesAndGUIDs.count + deviceNamesAndGUIDs.outputNamesAndGUIDs.count;
#ifdef PAWIN_USE_WDMKS_DEVICE_INFO
if( deviceCount > 0 )
{
deviceNamesAndGUIDs.winDsHostApi = winDsHostApi;
FindDevicePnpInterfaces( &deviceNamesAndGUIDs );
}
#endif  
if( deviceCount > 0 )
{
(*hostApi)->deviceInfos = (PaDeviceInfo**)PaUtil_GroupAllocateMemory(
winDsHostApi->allocations, sizeof(PaDeviceInfo*) * deviceCount );
if( !(*hostApi)->deviceInfos )
{
result = paInsufficientMemory;
goto error;
}
deviceInfoArray = (PaWinDsDeviceInfo*)PaUtil_GroupAllocateMemory(
winDsHostApi->allocations, sizeof(PaWinDsDeviceInfo) * deviceCount );
if( !deviceInfoArray )
{
result = paInsufficientMemory;
goto error;
}
for( i=0; i < deviceCount; ++i )
{
PaDeviceInfo *deviceInfo = &deviceInfoArray[i].inheritedDeviceInfo;
deviceInfo->structVersion = 2;
deviceInfo->hostApi = hostApiIndex;
deviceInfo->name = 0;
(*hostApi)->deviceInfos[i] = deviceInfo;
}
for( i=0; i < deviceNamesAndGUIDs.inputNamesAndGUIDs.count; ++i )
{
result = AddInputDeviceInfoFromDirectSoundCapture( winDsHostApi,
deviceNamesAndGUIDs.inputNamesAndGUIDs.items[i].name,
deviceNamesAndGUIDs.inputNamesAndGUIDs.items[i].lpGUID,
deviceNamesAndGUIDs.inputNamesAndGUIDs.items[i].pnpInterface );
if( result != paNoError )
goto error;
}
for( i=0; i < deviceNamesAndGUIDs.outputNamesAndGUIDs.count; ++i )
{
result = AddOutputDeviceInfoFromDirectSound( winDsHostApi,
deviceNamesAndGUIDs.outputNamesAndGUIDs.items[i].name,
deviceNamesAndGUIDs.outputNamesAndGUIDs.items[i].lpGUID,
deviceNamesAndGUIDs.outputNamesAndGUIDs.items[i].pnpInterface );
if( result != paNoError )
goto error;
}
}
result = TerminateDSDeviceNameAndGUIDVector( &deviceNamesAndGUIDs.inputNamesAndGUIDs );
if( result != paNoError )
goto error;
result = TerminateDSDeviceNameAndGUIDVector( &deviceNamesAndGUIDs.outputNamesAndGUIDs );
if( result != paNoError )
goto error;
(*hostApi)->Terminate = Terminate;
(*hostApi)->OpenStream = OpenStream;
(*hostApi)->IsFormatSupported = IsFormatSupported;
PaUtil_InitializeStreamInterface( &winDsHostApi->callbackStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable, PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &winDsHostApi->blockingStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream, GetStreamReadAvailable, GetStreamWriteAvailable );
return result;
error:
TerminateDSDeviceNameAndGUIDVector( &deviceNamesAndGUIDs.inputNamesAndGUIDs );
TerminateDSDeviceNameAndGUIDVector( &deviceNamesAndGUIDs.outputNamesAndGUIDs );
Terminate( (struct PaUtilHostApiRepresentation *)winDsHostApi );
return result;
}
static void Terminate( struct PaUtilHostApiRepresentation *hostApi )
{
PaWinDsHostApiRepresentation *winDsHostApi = (PaWinDsHostApiRepresentation*)hostApi;
if( winDsHostApi ){
if( winDsHostApi->allocations )
{
PaUtil_FreeAllAllocations( winDsHostApi->allocations );
PaUtil_DestroyAllocationGroup( winDsHostApi->allocations );
}
PaWinUtil_CoUninitialize( paDirectSound, &winDsHostApi->comInitializationResult );
PaUtil_FreeMemory( winDsHostApi );
}
PaWinDs_TerminateDSoundEntryPoints();
}
static PaError ValidateWinDirectSoundSpecificStreamInfo(
const PaStreamParameters *streamParameters,
const PaWinDirectSoundStreamInfo *streamInfo )
{
if( streamInfo )
{
if( streamInfo->size != sizeof( PaWinDirectSoundStreamInfo )
|| streamInfo->version != 2 )
{
return paIncompatibleHostApiSpecificStreamInfo;
}
if( streamInfo->flags & paWinDirectSoundUseLowLevelLatencyParameters )
{
if( streamInfo->framesPerBuffer <= 0 )
return paIncompatibleHostApiSpecificStreamInfo;
}
}
return paNoError;
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate )
{
PaError result;
PaWinDsDeviceInfo *inputWinDsDeviceInfo, *outputWinDsDeviceInfo;
PaDeviceInfo *inputDeviceInfo, *outputDeviceInfo;
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaWinDirectSoundStreamInfo *inputStreamInfo, *outputStreamInfo;
if( inputParameters )
{
inputWinDsDeviceInfo = (PaWinDsDeviceInfo*) hostApi->deviceInfos[ inputParameters->device ];
inputDeviceInfo = &inputWinDsDeviceInfo->inheritedDeviceInfo;
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
if( inputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( inputWinDsDeviceInfo->deviceInputChannelCountIsKnown
&& inputChannelCount > inputDeviceInfo->maxInputChannels )
return paInvalidChannelCount;
inputStreamInfo = (PaWinDirectSoundStreamInfo*)inputParameters->hostApiSpecificStreamInfo;
result = ValidateWinDirectSoundSpecificStreamInfo( inputParameters, inputStreamInfo );
if( result != paNoError ) return result;
}
else
{
inputChannelCount = 0;
}
if( outputParameters )
{
outputWinDsDeviceInfo = (PaWinDsDeviceInfo*) hostApi->deviceInfos[ outputParameters->device ];
outputDeviceInfo = &outputWinDsDeviceInfo->inheritedDeviceInfo;
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
if( outputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( outputWinDsDeviceInfo->deviceOutputChannelCountIsKnown
&& outputChannelCount > outputDeviceInfo->maxOutputChannels )
return paInvalidChannelCount;
outputStreamInfo = (PaWinDirectSoundStreamInfo*)outputParameters->hostApiSpecificStreamInfo;
result = ValidateWinDirectSoundSpecificStreamInfo( outputParameters, outputStreamInfo );
if( result != paNoError ) return result;
}
else
{
outputChannelCount = 0;
}
return paFormatIsSupported;
}
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
static HRESULT InitFullDuplexInputOutputBuffers( PaWinDsStream *stream,
PaWinDsDeviceInfo *inputDevice,
PaSampleFormat hostInputSampleFormat,
WORD inputChannelCount,
int bytesPerInputBuffer,
PaWinWaveFormatChannelMask inputChannelMask,
PaWinDsDeviceInfo *outputDevice,
PaSampleFormat hostOutputSampleFormat,
WORD outputChannelCount,
int bytesPerOutputBuffer,
PaWinWaveFormatChannelMask outputChannelMask,
unsigned long nFrameRate
)
{
HRESULT hr;
DSCBUFFERDESC  captureDesc;
PaWinWaveFormat captureWaveFormat;
DSBUFFERDESC   secondaryRenderDesc;
PaWinWaveFormat renderWaveFormat;
LPDIRECTSOUNDBUFFER8 pRenderBuffer8;
LPDIRECTSOUNDCAPTUREBUFFER8 pCaptureBuffer8;
PaWin_InitializeWaveFormatExtensible( &captureWaveFormat, inputChannelCount,
hostInputSampleFormat, PaWin_SampleFormatToLinearWaveFormatTag( hostInputSampleFormat ),
nFrameRate, inputChannelMask );
ZeroMemory(&captureDesc, sizeof(DSCBUFFERDESC));
captureDesc.dwSize = sizeof(DSCBUFFERDESC);
captureDesc.dwFlags = 0;
captureDesc.dwBufferBytes = bytesPerInputBuffer;
captureDesc.lpwfxFormat = (WAVEFORMATEX*)&captureWaveFormat;
PaWin_InitializeWaveFormatExtensible( &renderWaveFormat, outputChannelCount,
hostOutputSampleFormat, PaWin_SampleFormatToLinearWaveFormatTag( hostOutputSampleFormat ),
nFrameRate, outputChannelMask );
ZeroMemory(&secondaryRenderDesc, sizeof(DSBUFFERDESC));
secondaryRenderDesc.dwSize = sizeof(DSBUFFERDESC);
secondaryRenderDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
secondaryRenderDesc.dwBufferBytes = bytesPerOutputBuffer;
secondaryRenderDesc.lpwfxFormat = (WAVEFORMATEX*)&renderWaveFormat;
hr = paWinDsDSoundEntryPoints.DirectSoundFullDuplexCreate8(
inputDevice->lpGUID, outputDevice->lpGUID,
&captureDesc, &secondaryRenderDesc,
GetDesktopWindow(),  
DSSCL_EXCLUSIVE,
&stream->pDirectSoundFullDuplex8,
&pCaptureBuffer8,
&pRenderBuffer8,
NULL  
);
if( hr == DS_OK )
{
PA_DEBUG(("DirectSoundFullDuplexCreate succeeded!\n"));
hr = IUnknown_QueryInterface( pCaptureBuffer8, &IID_IDirectSoundCaptureBuffer, (LPVOID *)&stream->pDirectSoundInputBuffer );
if( hr == DS_OK )
hr = IUnknown_QueryInterface( pRenderBuffer8, &IID_IDirectSoundBuffer, (LPVOID *)&stream->pDirectSoundOutputBuffer );
IUnknown_Release( pCaptureBuffer8 );
IUnknown_Release( pRenderBuffer8 );
if( !stream->pDirectSoundInputBuffer || !stream->pDirectSoundOutputBuffer ){
if( stream->pDirectSoundInputBuffer )
{
IUnknown_Release( stream->pDirectSoundInputBuffer );
stream->pDirectSoundInputBuffer = NULL;
}
if( stream->pDirectSoundOutputBuffer )
{
IUnknown_Release( stream->pDirectSoundOutputBuffer );
stream->pDirectSoundOutputBuffer = NULL;
}
IUnknown_Release( stream->pDirectSoundFullDuplex8 );
stream->pDirectSoundFullDuplex8 = NULL;
}
}
else
{
PA_DEBUG(("DirectSoundFullDuplexCreate failed. hr=%d\n", hr));
}
return hr;
}
#endif  
static HRESULT InitInputBuffer( PaWinDsStream *stream,
PaWinDsDeviceInfo *device,
PaSampleFormat sampleFormat,
unsigned long nFrameRate,
WORD nChannels,
int bytesPerBuffer,
PaWinWaveFormatChannelMask channelMask )
{
DSCBUFFERDESC  captureDesc;
PaWinWaveFormat waveFormat;
HRESULT        result;
if( (result = paWinDsDSoundEntryPoints.DirectSoundCaptureCreate(
device->lpGUID, &stream->pDirectSoundCapture, NULL) ) != DS_OK ){
ERR_RPT(("PortAudio: DirectSoundCaptureCreate() failed!\n"));
return result;
}
ZeroMemory(&captureDesc, sizeof(DSCBUFFERDESC));
captureDesc.dwSize = sizeof(DSCBUFFERDESC);
captureDesc.dwFlags = 0;
captureDesc.dwBufferBytes = bytesPerBuffer;
captureDesc.lpwfxFormat = (WAVEFORMATEX*)&waveFormat;
PaWin_InitializeWaveFormatExtensible( &waveFormat, nChannels,
sampleFormat, PaWin_SampleFormatToLinearWaveFormatTag( sampleFormat ),
nFrameRate, channelMask );
if( IDirectSoundCapture_CreateCaptureBuffer( stream->pDirectSoundCapture,
&captureDesc, &stream->pDirectSoundInputBuffer, NULL) != DS_OK )
{
PaWin_InitializeWaveFormatEx( &waveFormat, nChannels, sampleFormat,
PaWin_SampleFormatToLinearWaveFormatTag( sampleFormat ), nFrameRate );
if ((result = IDirectSoundCapture_CreateCaptureBuffer( stream->pDirectSoundCapture,
&captureDesc, &stream->pDirectSoundInputBuffer, NULL)) != DS_OK) return result;
}
stream->readOffset = 0;   
return DS_OK;
}
static HRESULT InitOutputBuffer( PaWinDsStream *stream, PaWinDsDeviceInfo *device,
PaSampleFormat sampleFormat, unsigned long nFrameRate,
WORD nChannels, int bytesPerBuffer,
PaWinWaveFormatChannelMask channelMask )
{
HRESULT        result;
HWND           hWnd;
HRESULT        hr;
PaWinWaveFormat waveFormat;
DSBUFFERDESC   primaryDesc;
DSBUFFERDESC   secondaryDesc;
if( (hr = paWinDsDSoundEntryPoints.DirectSoundCreate(
device->lpGUID, &stream->pDirectSound, NULL )) != DS_OK ){
ERR_RPT(("PortAudio: DirectSoundCreate() failed!\n"));
return hr;
}
hWnd = GetDesktopWindow();
if ((hr = IDirectSound_SetCooperativeLevel( stream->pDirectSound,
hWnd, DSSCL_EXCLUSIVE)) != DS_OK)
{
return hr;
}
ZeroMemory(&primaryDesc, sizeof(DSBUFFERDESC));
primaryDesc.dwSize        = sizeof(DSBUFFERDESC);
primaryDesc.dwFlags       = DSBCAPS_PRIMARYBUFFER;  
primaryDesc.dwBufferBytes = 0;
primaryDesc.lpwfxFormat   = NULL;
if ((result = IDirectSound_CreateSoundBuffer( stream->pDirectSound,
&primaryDesc, &stream->pDirectSoundPrimaryBuffer, NULL)) != DS_OK)
goto error;
PaWin_InitializeWaveFormatExtensible( &waveFormat, nChannels,
sampleFormat, PaWin_SampleFormatToLinearWaveFormatTag( sampleFormat ),
nFrameRate, channelMask );
if( IDirectSoundBuffer_SetFormat( stream->pDirectSoundPrimaryBuffer, (WAVEFORMATEX*)&waveFormat) != DS_OK )
{
PaWin_InitializeWaveFormatEx( &waveFormat, nChannels, sampleFormat,
PaWin_SampleFormatToLinearWaveFormatTag( sampleFormat ), nFrameRate );
if((result = IDirectSoundBuffer_SetFormat( stream->pDirectSoundPrimaryBuffer, (WAVEFORMATEX*)&waveFormat)) != DS_OK)
goto error;
}
ZeroMemory(&secondaryDesc, sizeof(DSBUFFERDESC));
secondaryDesc.dwSize = sizeof(DSBUFFERDESC);
secondaryDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
secondaryDesc.dwBufferBytes = bytesPerBuffer;
secondaryDesc.lpwfxFormat = (WAVEFORMATEX*)&waveFormat;  
if ((result = IDirectSound_CreateSoundBuffer( stream->pDirectSound,
&secondaryDesc, &stream->pDirectSoundOutputBuffer, NULL)) != DS_OK)
goto error;
return DS_OK;
error:
if( stream->pDirectSoundPrimaryBuffer )
{
IDirectSoundBuffer_Release( stream->pDirectSoundPrimaryBuffer );
stream->pDirectSoundPrimaryBuffer = NULL;
}
return result;
}
static void CalculateBufferSettings( unsigned long *hostBufferSizeFrames,
unsigned long *pollingPeriodFrames,
int isFullDuplex,
unsigned long suggestedInputLatencyFrames,
unsigned long suggestedOutputLatencyFrames,
double sampleRate, unsigned long userFramesPerBuffer )
{
unsigned long minimumPollingPeriodFrames = (unsigned long)(sampleRate * PA_DS_MINIMUM_POLLING_PERIOD_SECONDS);
unsigned long maximumPollingPeriodFrames = (unsigned long)(sampleRate * PA_DS_MAXIMUM_POLLING_PERIOD_SECONDS);
unsigned long pollingJitterFrames = (unsigned long)(sampleRate * PA_DS_POLLING_JITTER_SECONDS);
if( userFramesPerBuffer == paFramesPerBufferUnspecified )
{
unsigned long targetBufferingLatencyFrames = max( suggestedInputLatencyFrames, suggestedOutputLatencyFrames );
*pollingPeriodFrames = targetBufferingLatencyFrames / 4;
if( *pollingPeriodFrames < minimumPollingPeriodFrames )
{
*pollingPeriodFrames = minimumPollingPeriodFrames;
}
else if( *pollingPeriodFrames > maximumPollingPeriodFrames )
{
*pollingPeriodFrames = maximumPollingPeriodFrames;
}
*hostBufferSizeFrames = *pollingPeriodFrames
+ max( *pollingPeriodFrames + pollingJitterFrames, targetBufferingLatencyFrames);
}
else
{
unsigned long targetBufferingLatencyFrames = suggestedInputLatencyFrames;
if( isFullDuplex )
{
if( userFramesPerBuffer < suggestedOutputLatencyFrames )
{
unsigned long adjustedSuggestedOutputLatencyFrames =
suggestedOutputLatencyFrames - userFramesPerBuffer;
if( adjustedSuggestedOutputLatencyFrames > targetBufferingLatencyFrames )
targetBufferingLatencyFrames = adjustedSuggestedOutputLatencyFrames;
}
}
else
{
if( suggestedOutputLatencyFrames > suggestedInputLatencyFrames )
targetBufferingLatencyFrames = suggestedOutputLatencyFrames;
}
*hostBufferSizeFrames = userFramesPerBuffer
+ max( userFramesPerBuffer + pollingJitterFrames, targetBufferingLatencyFrames);
*pollingPeriodFrames = max( max(1, userFramesPerBuffer / 4), targetBufferingLatencyFrames / 16 );
if( *pollingPeriodFrames > maximumPollingPeriodFrames )
{
*pollingPeriodFrames = maximumPollingPeriodFrames;
}
}
}
static void CalculatePollingPeriodFrames( unsigned long hostBufferSizeFrames,
unsigned long *pollingPeriodFrames,
double sampleRate, unsigned long userFramesPerBuffer )
{
unsigned long minimumPollingPeriodFrames = (unsigned long)(sampleRate * PA_DS_MINIMUM_POLLING_PERIOD_SECONDS);
unsigned long maximumPollingPeriodFrames = (unsigned long)(sampleRate * PA_DS_MAXIMUM_POLLING_PERIOD_SECONDS);
unsigned long pollingJitterFrames = (unsigned long)(sampleRate * PA_DS_POLLING_JITTER_SECONDS);
*pollingPeriodFrames = max( max(1, userFramesPerBuffer / 4), hostBufferSizeFrames / 16 );
if( *pollingPeriodFrames > maximumPollingPeriodFrames )
{
*pollingPeriodFrames = maximumPollingPeriodFrames;
}
}
static void SetStreamInfoLatencies( PaWinDsStream *stream,
unsigned long userFramesPerBuffer,
unsigned long pollingPeriodFrames,
double sampleRate )
{
unsigned long effectiveFramesPerBuffer = (userFramesPerBuffer == paFramesPerBufferUnspecified)
? pollingPeriodFrames
: userFramesPerBuffer;
if( stream->bufferProcessor.inputChannelCount > 0 )
{
stream->streamRepresentation.streamInfo.inputLatency =
(double)(PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor)
+ effectiveFramesPerBuffer) / sampleRate;
}
else
{
stream->streamRepresentation.streamInfo.inputLatency = 0;
}
if( stream->bufferProcessor.outputChannelCount > 0 )
{
stream->streamRepresentation.streamInfo.outputLatency =
(double)(PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor)
+ (stream->hostBufferSizeFrames - effectiveFramesPerBuffer)) / sampleRate;
}
else
{
stream->streamRepresentation.streamInfo.outputLatency = 0;
}
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
PaWinDsHostApiRepresentation *winDsHostApi = (PaWinDsHostApiRepresentation*)hostApi;
PaWinDsStream *stream = 0;
int bufferProcessorIsInitialized = 0;
int streamRepresentationIsInitialized = 0;
PaWinDsDeviceInfo *inputWinDsDeviceInfo, *outputWinDsDeviceInfo;
PaDeviceInfo *inputDeviceInfo, *outputDeviceInfo;
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaSampleFormat hostInputSampleFormat, hostOutputSampleFormat;
int userRequestedHostInputBufferSizeFrames = 0;
int userRequestedHostOutputBufferSizeFrames = 0;
unsigned long suggestedInputLatencyFrames, suggestedOutputLatencyFrames;
PaWinDirectSoundStreamInfo *inputStreamInfo, *outputStreamInfo;
PaWinWaveFormatChannelMask inputChannelMask, outputChannelMask;
unsigned long pollingPeriodFrames = 0;
if( inputParameters )
{
inputWinDsDeviceInfo = (PaWinDsDeviceInfo*) hostApi->deviceInfos[ inputParameters->device ];
inputDeviceInfo = &inputWinDsDeviceInfo->inheritedDeviceInfo;
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = inputParameters->sampleFormat;
suggestedInputLatencyFrames = (unsigned long)(inputParameters->suggestedLatency * sampleRate);
if( inputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( inputWinDsDeviceInfo->deviceInputChannelCountIsKnown
&& inputChannelCount > inputDeviceInfo->maxInputChannels )
return paInvalidChannelCount;
inputStreamInfo = (PaWinDirectSoundStreamInfo*)inputParameters->hostApiSpecificStreamInfo;
result = ValidateWinDirectSoundSpecificStreamInfo( inputParameters, inputStreamInfo );
if( result != paNoError ) return result;
if( inputStreamInfo && inputStreamInfo->flags & paWinDirectSoundUseLowLevelLatencyParameters )
userRequestedHostInputBufferSizeFrames = inputStreamInfo->framesPerBuffer;
if( inputStreamInfo && inputStreamInfo->flags & paWinDirectSoundUseChannelMask )
inputChannelMask = inputStreamInfo->channelMask;
else
inputChannelMask = PaWin_DefaultChannelMask( inputChannelCount );
}
else
{
inputChannelCount = 0;
inputSampleFormat = 0;
suggestedInputLatencyFrames = 0;
}
if( outputParameters )
{
outputWinDsDeviceInfo = (PaWinDsDeviceInfo*) hostApi->deviceInfos[ outputParameters->device ];
outputDeviceInfo = &outputWinDsDeviceInfo->inheritedDeviceInfo;
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = outputParameters->sampleFormat;
suggestedOutputLatencyFrames = (unsigned long)(outputParameters->suggestedLatency * sampleRate);
if( outputParameters->device == paUseHostApiSpecificDeviceSpecification )
return paInvalidDevice;
if( outputWinDsDeviceInfo->deviceOutputChannelCountIsKnown
&& outputChannelCount > outputDeviceInfo->maxOutputChannels )
return paInvalidChannelCount;
outputStreamInfo = (PaWinDirectSoundStreamInfo*)outputParameters->hostApiSpecificStreamInfo;
result = ValidateWinDirectSoundSpecificStreamInfo( outputParameters, outputStreamInfo );
if( result != paNoError ) return result;
if( outputStreamInfo && outputStreamInfo->flags & paWinDirectSoundUseLowLevelLatencyParameters )
userRequestedHostOutputBufferSizeFrames = outputStreamInfo->framesPerBuffer;
if( outputStreamInfo && outputStreamInfo->flags & paWinDirectSoundUseChannelMask )
outputChannelMask = outputStreamInfo->channelMask;
else
outputChannelMask = PaWin_DefaultChannelMask( outputChannelCount );
}
else
{
outputChannelCount = 0;
outputSampleFormat = 0;
suggestedOutputLatencyFrames = 0;
}
if( (userRequestedHostInputBufferSizeFrames > 0 && userRequestedHostOutputBufferSizeFrames > 0)
&& userRequestedHostInputBufferSizeFrames != userRequestedHostOutputBufferSizeFrames )
return paIncompatibleHostApiSpecificStreamInfo;
if( (streamFlags & paPlatformSpecificFlags) != 0 )
return paInvalidFlag;  
stream = (PaWinDsStream*)PaUtil_AllocateMemory( sizeof(PaWinDsStream) );
if( !stream )
{
result = paInsufficientMemory;
goto error;
}
memset( stream, 0, sizeof(PaWinDsStream) );  
if( streamCallback )
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&winDsHostApi->callbackStreamInterface, streamCallback, userData );
}
else
{
PaUtil_InitializeStreamRepresentation( &stream->streamRepresentation,
&winDsHostApi->blockingStreamInterface, streamCallback, userData );
}
streamRepresentationIsInitialized = 1;
stream->streamFlags = streamFlags;
PaUtil_InitializeCpuLoadMeasurer( &stream->cpuLoadMeasurer, sampleRate );
if( inputParameters )
{
PaSampleFormat nativeInputFormats = paInt16;
hostInputSampleFormat =
PaUtil_SelectClosestAvailableFormat( nativeInputFormats, inputParameters->sampleFormat );
}
else
{
hostInputSampleFormat = 0;
}
if( outputParameters )
{
PaSampleFormat nativeOutputFormats = paInt16;
hostOutputSampleFormat =
PaUtil_SelectClosestAvailableFormat( nativeOutputFormats, outputParameters->sampleFormat );
}
else
{
hostOutputSampleFormat = 0;
}
result =  PaUtil_InitializeBufferProcessor( &stream->bufferProcessor,
inputChannelCount, inputSampleFormat, hostInputSampleFormat,
outputChannelCount, outputSampleFormat, hostOutputSampleFormat,
sampleRate, streamFlags, framesPerBuffer,
0,  
paUtilVariableHostBufferSizePartialUsageAllowed,
streamCallback, userData );
if( result != paNoError )
goto error;
bufferProcessorIsInitialized = 1;
{
HRESULT          hr;
unsigned long    integerSampleRate = (unsigned long) (sampleRate + 0.5);
stream->processingCompleted = CreateEvent( NULL,   TRUE,   FALSE, NULL );
if( stream->processingCompleted == NULL )
{
result = paInsufficientMemory;
goto error;
}
#ifdef PA_WIN_DS_USE_WMME_TIMER
stream->timerID = 0;
#endif
#ifdef PA_WIN_DS_USE_WAITABLE_TIMER_OBJECT
stream->waitableTimer = (HANDLE)CreateWaitableTimer( 0, FALSE, NULL );
if( stream->waitableTimer == NULL )
{
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( GetLastError() );
goto error;
}
#endif
#ifndef PA_WIN_DS_USE_WMME_TIMER
stream->processingThreadCompleted = CreateEvent( NULL,   TRUE,   FALSE, NULL );
if( stream->processingThreadCompleted == NULL )
{
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( GetLastError() );
goto error;
}
#endif
if( userRequestedHostInputBufferSizeFrames > 0 || userRequestedHostOutputBufferSizeFrames > 0 )
{
stream->hostBufferSizeFrames = max( userRequestedHostInputBufferSizeFrames, userRequestedHostOutputBufferSizeFrames );
CalculatePollingPeriodFrames(
stream->hostBufferSizeFrames, &pollingPeriodFrames,
sampleRate, framesPerBuffer );
}
else
{
CalculateBufferSettings( (unsigned long*)&stream->hostBufferSizeFrames, &pollingPeriodFrames,
(inputParameters && outputParameters),
suggestedInputLatencyFrames,
suggestedOutputLatencyFrames,
sampleRate, framesPerBuffer );
}
stream->pollingPeriodSeconds = pollingPeriodFrames / sampleRate;
DBUG(("DirectSound host buffer size frames: %d, polling period seconds: %f, @ sr: %f\n",
stream->hostBufferSizeFrames, stream->pollingPeriodSeconds, sampleRate ));
if( outputParameters )
{
LARGE_INTEGER  counterFrequency;
int sampleSizeBytes = Pa_GetSampleSize(hostOutputSampleFormat);
stream->outputFrameSizeBytes = outputParameters->channelCount * sampleSizeBytes;
stream->outputBufferSizeBytes = stream->hostBufferSizeFrames * stream->outputFrameSizeBytes;
if( stream->outputBufferSizeBytes < DSBSIZE_MIN )
{
result = paBufferTooSmall;
goto error;
}
else if( stream->outputBufferSizeBytes > DSBSIZE_MAX )
{
result = paBufferTooBig;
goto error;
}
stream->secondsPerHostByte = 1.0 /
(stream->bufferProcessor.bytesPerHostOutputSample *
outputChannelCount * sampleRate);
stream->outputIsRunning = FALSE;
stream->outputUnderflowCount = 0;
if( QueryPerformanceFrequency( &counterFrequency ) )
{
stream->perfCounterTicksPerBuffer.QuadPart = (counterFrequency.QuadPart * stream->hostBufferSizeFrames) / integerSampleRate;
}
else
{
stream->perfCounterTicksPerBuffer.QuadPart = 0;
}
}
if( inputParameters )
{
int sampleSizeBytes = Pa_GetSampleSize(hostInputSampleFormat);
stream->inputFrameSizeBytes = inputParameters->channelCount * sampleSizeBytes;
stream->inputBufferSizeBytes = stream->hostBufferSizeFrames * stream->inputFrameSizeBytes;
if( stream->inputBufferSizeBytes < DSBSIZE_MIN )
{
result = paBufferTooSmall;
goto error;
}
else if( stream->inputBufferSizeBytes > DSBSIZE_MAX )
{
result = paBufferTooBig;
goto error;
}
}
assert( stream->pDirectSoundCapture == NULL );
assert( stream->pDirectSoundInputBuffer == NULL );
assert( stream->pDirectSound == NULL );
assert( stream->pDirectSoundPrimaryBuffer == NULL );
assert( stream->pDirectSoundOutputBuffer == NULL );
if( inputParameters && outputParameters )
{
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
hr = InitFullDuplexInputOutputBuffers( stream,
(PaWinDsDeviceInfo*)hostApi->deviceInfos[inputParameters->device],
hostInputSampleFormat,
(WORD)inputParameters->channelCount, stream->inputBufferSizeBytes,
inputChannelMask,
(PaWinDsDeviceInfo*)hostApi->deviceInfos[outputParameters->device],
hostOutputSampleFormat,
(WORD)outputParameters->channelCount, stream->outputBufferSizeBytes,
outputChannelMask,
integerSampleRate
);
DBUG(("InitFullDuplexInputOutputBuffers() returns %x\n", hr));
#endif  
}
if( outputParameters && !stream->pDirectSoundOutputBuffer )
{
hr = InitOutputBuffer( stream,
(PaWinDsDeviceInfo*)hostApi->deviceInfos[outputParameters->device],
hostOutputSampleFormat,
integerSampleRate,
(WORD)outputParameters->channelCount, stream->outputBufferSizeBytes,
outputChannelMask );
DBUG(("InitOutputBuffer() returns %x\n", hr));
if( hr != DS_OK )
{
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( hr );
goto error;
}
}
if( inputParameters && !stream->pDirectSoundInputBuffer )
{
hr = InitInputBuffer( stream,
(PaWinDsDeviceInfo*)hostApi->deviceInfos[inputParameters->device],
hostInputSampleFormat,
integerSampleRate,
(WORD)inputParameters->channelCount, stream->inputBufferSizeBytes,
inputChannelMask );
DBUG(("InitInputBuffer() returns %x\n", hr));
if( hr != DS_OK )
{
ERR_RPT(("PortAudio: DSW_InitInputBuffer() returns %x\n", hr));
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( hr );
goto error;
}
}
}
SetStreamInfoLatencies( stream, framesPerBuffer, pollingPeriodFrames, sampleRate );
stream->streamRepresentation.streamInfo.sampleRate = sampleRate;
*s = (PaStream*)stream;
return result;
error:
if( stream )
{
if( stream->processingCompleted != NULL )
CloseHandle( stream->processingCompleted );
#ifdef PA_WIN_DS_USE_WAITABLE_TIMER_OBJECT
if( stream->waitableTimer != NULL )
CloseHandle( stream->waitableTimer );
#endif
#ifndef PA_WIN_DS_USE_WMME_TIMER
if( stream->processingThreadCompleted != NULL )
CloseHandle( stream->processingThreadCompleted );
#endif
if( stream->pDirectSoundOutputBuffer )
{
IDirectSoundBuffer_Stop( stream->pDirectSoundOutputBuffer );
IDirectSoundBuffer_Release( stream->pDirectSoundOutputBuffer );
stream->pDirectSoundOutputBuffer = NULL;
}
if( stream->pDirectSoundPrimaryBuffer )
{
IDirectSoundBuffer_Release( stream->pDirectSoundPrimaryBuffer );
stream->pDirectSoundPrimaryBuffer = NULL;
}
if( stream->pDirectSoundInputBuffer )
{
IDirectSoundCaptureBuffer_Stop( stream->pDirectSoundInputBuffer );
IDirectSoundCaptureBuffer_Release( stream->pDirectSoundInputBuffer );
stream->pDirectSoundInputBuffer = NULL;
}
if( stream->pDirectSoundCapture )
{
IDirectSoundCapture_Release( stream->pDirectSoundCapture );
stream->pDirectSoundCapture = NULL;
}
if( stream->pDirectSound )
{
IDirectSound_Release( stream->pDirectSound );
stream->pDirectSound = NULL;
}
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
if( stream->pDirectSoundFullDuplex8 )
{
IDirectSoundFullDuplex_Release( stream->pDirectSoundFullDuplex8 );
stream->pDirectSoundFullDuplex8 = NULL;
}
#endif
if( bufferProcessorIsInitialized )
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
if( streamRepresentationIsInitialized )
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
PaUtil_FreeMemory( stream );
}
return result;
}
static HRESULT QueryOutputSpace( PaWinDsStream *stream, long *bytesEmpty )
{
HRESULT hr;
DWORD   playCursor;
DWORD   writeCursor;
long    numBytesEmpty;
long    playWriteGap;
hr = IDirectSoundBuffer_GetCurrentPosition( stream->pDirectSoundOutputBuffer,
&playCursor, &writeCursor );
if( hr != DS_OK )
{
return hr;
}
playWriteGap = writeCursor - playCursor;
if( playWriteGap < 0 ) playWriteGap += stream->outputBufferSizeBytes;  
if( stream->outputIsRunning && (stream->perfCounterTicksPerBuffer.QuadPart != 0) )
{
LARGE_INTEGER   currentTime;
LARGE_INTEGER   elapsedTime;
long            bytesPlayed;
long            bytesExpected;
long            buffersWrapped;
QueryPerformanceCounter( &currentTime );
elapsedTime.QuadPart = currentTime.QuadPart - stream->previousPlayTime.QuadPart;
stream->previousPlayTime = currentTime;
bytesPlayed = playCursor - stream->previousPlayCursor;
if( bytesPlayed < 0 ) bytesPlayed += stream->outputBufferSizeBytes;  
stream->previousPlayCursor = playCursor;
bytesExpected = (long) ((elapsedTime.QuadPart * stream->outputBufferSizeBytes) / stream->perfCounterTicksPerBuffer.QuadPart);
buffersWrapped = (bytesExpected - bytesPlayed) / stream->outputBufferSizeBytes;
if( buffersWrapped > 0 )
{
playCursor += (buffersWrapped * stream->outputBufferSizeBytes);
bytesPlayed += (buffersWrapped * stream->outputBufferSizeBytes);
}
}
numBytesEmpty = playCursor - stream->outputBufferWriteOffsetBytes;
if( numBytesEmpty < 0 ) numBytesEmpty += stream->outputBufferSizeBytes;  
if( numBytesEmpty > (stream->outputBufferSizeBytes - playWriteGap) )
{
if( stream->outputIsRunning )
{
stream->outputUnderflowCount += 1;
}
stream->outputBufferWriteOffsetBytes = writeCursor;
numBytesEmpty = stream->outputBufferSizeBytes - playWriteGap;
}
*bytesEmpty = numBytesEmpty;
return hr;
}
static int TimeSlice( PaWinDsStream *stream )
{
long              numFrames = 0;
long              bytesEmpty = 0;
long              bytesFilled = 0;
long              bytesToXfer = 0;
long              framesToXfer = 0;  
long              numInFramesReady = 0;
long              numOutFramesReady = 0;
long              bytesProcessed;
HRESULT           hresult;
double            outputLatency = 0;
double            inputLatency = 0;
PaStreamCallbackTimeInfo timeInfo = {0,0,0};
LPBYTE            lpInBuf1 = NULL;
LPBYTE            lpInBuf2 = NULL;
DWORD             dwInSize1 = 0;
DWORD             dwInSize2 = 0;
LPBYTE            lpOutBuf1 = NULL;
LPBYTE            lpOutBuf2 = NULL;
DWORD             dwOutSize1 = 0;
DWORD             dwOutSize2 = 0;
if( stream->bufferProcessor.inputChannelCount > 0 )
{
HRESULT hr;
DWORD capturePos;
DWORD readPos;
long  filled = 0;
hr = IDirectSoundCaptureBuffer_GetCurrentPosition( stream->pDirectSoundInputBuffer, &capturePos, &readPos );
if( hr == DS_OK )
{
filled = readPos - stream->readOffset;
if( filled < 0 ) filled += stream->inputBufferSizeBytes;  
bytesFilled = filled;
inputLatency = ((double)bytesFilled) * stream->secondsPerHostByte;
}
framesToXfer = numInFramesReady = bytesFilled / stream->inputFrameSizeBytes;
}
if( stream->bufferProcessor.outputChannelCount > 0 )
{
UINT previousUnderflowCount = stream->outputUnderflowCount;
QueryOutputSpace( stream, &bytesEmpty );
framesToXfer = numOutFramesReady = bytesEmpty / stream->outputFrameSizeBytes;
if( stream->outputUnderflowCount != previousUnderflowCount )
stream->callbackFlags |= paOutputUnderflow;
outputLatency = ((double)(stream->outputBufferSizeBytes - bytesEmpty)) * stream->secondsPerHostByte;
}
if( stream->bufferProcessor.inputChannelCount > 0 && stream->bufferProcessor.outputChannelCount > 0 )
{
framesToXfer = (numOutFramesReady < numInFramesReady) ? numOutFramesReady : numInFramesReady;
}
if( framesToXfer > 0 )
{
PaUtil_BeginCpuLoadMeasurement( &stream->cpuLoadMeasurer );
timeInfo.currentTime = PaUtil_GetTime();
PaUtil_BeginBufferProcessing( &stream->bufferProcessor, &timeInfo, stream->callbackFlags );
stream->callbackFlags = 0;
if( stream->bufferProcessor.inputChannelCount > 0 )
{
timeInfo.inputBufferAdcTime = timeInfo.currentTime - inputLatency;
bytesToXfer = framesToXfer * stream->inputFrameSizeBytes;
hresult = IDirectSoundCaptureBuffer_Lock ( stream->pDirectSoundInputBuffer,
stream->readOffset, bytesToXfer,
(void **) &lpInBuf1, &dwInSize1,
(void **) &lpInBuf2, &dwInSize2, 0);
if (hresult != DS_OK)
{
ERR_RPT(("DirectSound IDirectSoundCaptureBuffer_Lock failed, hresult = 0x%x\n",hresult));
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );  
stream->callbackResult = paComplete;
goto error2;
}
numFrames = dwInSize1 / stream->inputFrameSizeBytes;
PaUtil_SetInputFrameCount( &stream->bufferProcessor, numFrames );
PaUtil_SetInterleavedInputChannels( &stream->bufferProcessor, 0, lpInBuf1, 0 );
if( dwInSize2 > 0 )
{
numFrames = dwInSize2 / stream->inputFrameSizeBytes;
PaUtil_Set2ndInputFrameCount( &stream->bufferProcessor, numFrames );
PaUtil_Set2ndInterleavedInputChannels( &stream->bufferProcessor, 0, lpInBuf2, 0 );
}
}
if( stream->bufferProcessor.outputChannelCount > 0 )
{
timeInfo.outputBufferDacTime = timeInfo.currentTime;
bytesToXfer = framesToXfer * stream->outputFrameSizeBytes;
hresult = IDirectSoundBuffer_Lock ( stream->pDirectSoundOutputBuffer,
stream->outputBufferWriteOffsetBytes, bytesToXfer,
(void **) &lpOutBuf1, &dwOutSize1,
(void **) &lpOutBuf2, &dwOutSize2, 0);
if (hresult != DS_OK)
{
ERR_RPT(("DirectSound IDirectSoundBuffer_Lock failed, hresult = 0x%x\n",hresult));
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );  
stream->callbackResult = paComplete;
goto error1;
}
numFrames = dwOutSize1 / stream->outputFrameSizeBytes;
PaUtil_SetOutputFrameCount( &stream->bufferProcessor, numFrames );
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor, 0, lpOutBuf1, 0 );
if( dwOutSize2 > 0 )
{
numFrames = dwOutSize2 / stream->outputFrameSizeBytes;
PaUtil_Set2ndOutputFrameCount( &stream->bufferProcessor, numFrames );
PaUtil_Set2ndInterleavedOutputChannels( &stream->bufferProcessor, 0, lpOutBuf2, 0 );
}
}
numFrames = PaUtil_EndBufferProcessing( &stream->bufferProcessor, &stream->callbackResult );
stream->framesWritten += numFrames;
if( stream->bufferProcessor.outputChannelCount > 0 )
{
bytesProcessed = numFrames * stream->outputFrameSizeBytes;
stream->outputBufferWriteOffsetBytes = (stream->outputBufferWriteOffsetBytes + bytesProcessed) % stream->outputBufferSizeBytes;
IDirectSoundBuffer_Unlock( stream->pDirectSoundOutputBuffer, lpOutBuf1, dwOutSize1, lpOutBuf2, dwOutSize2);
}
error1:
if( stream->bufferProcessor.inputChannelCount > 0 )
{
bytesProcessed = numFrames * stream->inputFrameSizeBytes;
stream->readOffset = (stream->readOffset + bytesProcessed) % stream->inputBufferSizeBytes;
IDirectSoundCaptureBuffer_Unlock( stream->pDirectSoundInputBuffer, lpInBuf1, dwInSize1, lpInBuf2, dwInSize2);
}
error2:
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, numFrames );
}
if( stream->callbackResult == paComplete && !PaUtil_IsBufferProcessorOutputEmpty( &stream->bufferProcessor ) )
{
return paContinue;
}
else
{
return stream->callbackResult;
}
}
static HRESULT ZeroAvailableOutputSpace( PaWinDsStream *stream )
{
HRESULT hr;
LPBYTE lpbuf1 = NULL;
LPBYTE lpbuf2 = NULL;
DWORD dwsize1 = 0;
DWORD dwsize2 = 0;
long  bytesEmpty;
hr = QueryOutputSpace( stream, &bytesEmpty );
if (hr != DS_OK) return hr;
if( bytesEmpty == 0 ) return DS_OK;
hr = IDirectSoundBuffer_Lock( stream->pDirectSoundOutputBuffer, stream->outputBufferWriteOffsetBytes,
bytesEmpty, (void **) &lpbuf1, &dwsize1,
(void **) &lpbuf2, &dwsize2, 0);
if (hr == DS_OK)
{
ZeroMemory(lpbuf1, dwsize1);
if(lpbuf2 != NULL)
{
ZeroMemory(lpbuf2, dwsize2);
}
stream->outputBufferWriteOffsetBytes = (stream->outputBufferWriteOffsetBytes + dwsize1 + dwsize2) % stream->outputBufferSizeBytes;
IDirectSoundBuffer_Unlock( stream->pDirectSoundOutputBuffer, lpbuf1, dwsize1, lpbuf2, dwsize2);
stream->finalZeroBytesWritten += dwsize1 + dwsize2;
}
return hr;
}
static void CALLBACK TimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD dw1, DWORD dw2)
{
PaWinDsStream *stream;
int isFinished = 0;
(void) uID;
(void) uMsg;
(void) dw1;
(void) dw2;
stream = (PaWinDsStream *) dwUser;
if( stream == NULL ) return;
if( stream->isActive )
{
if( stream->abortProcessing )
{
isFinished = 1;
}
else if( stream->stopProcessing )
{
if( stream->bufferProcessor.outputChannelCount > 0 )
{
ZeroAvailableOutputSpace( stream );
if( stream->finalZeroBytesWritten >= stream->outputBufferSizeBytes )
{
isFinished = 1;
}
}
else
{
isFinished = 1;
}
}
else
{
int callbackResult = TimeSlice( stream );
if( callbackResult != paContinue )
{
stream->stopProcessing = 1;
}
}
if( isFinished )
{
if( stream->streamRepresentation.streamFinishedCallback != 0 )
stream->streamRepresentation.streamFinishedCallback( stream->streamRepresentation.userData );
stream->isActive = 0;  
SetEvent( stream->processingCompleted );
}
}
}
#ifndef PA_WIN_DS_USE_WMME_TIMER
#ifdef PA_WIN_DS_USE_WAITABLE_TIMER_OBJECT
static void CALLBACK WaitableTimerAPCProc(
LPVOID lpArg,                
DWORD dwTimerLowValue,       
DWORD dwTimerHighValue )     
{
(void)dwTimerLowValue;
(void)dwTimerHighValue;
TimerCallback( 0, 0, (DWORD_PTR)lpArg, 0, 0 );
}
#endif  
PA_THREAD_FUNC ProcessingThreadProc( void *pArg )
{
PaWinDsStream *stream = (PaWinDsStream *)pArg;
LARGE_INTEGER dueTime;
int timerPeriodMs;
timerPeriodMs = (int)(stream->pollingPeriodSeconds * MSECS_PER_SECOND);
if( timerPeriodMs < 1 )
timerPeriodMs = 1;
#ifdef PA_WIN_DS_USE_WAITABLE_TIMER_OBJECT
assert( stream->waitableTimer != NULL );
dueTime.LowPart = timerPeriodMs * 1000 * 10;
dueTime.HighPart = 0;
if( SetWaitableTimer( stream->waitableTimer, &dueTime, timerPeriodMs, WaitableTimerAPCProc, pArg, FALSE ) != 0 )
{
DWORD wfsoResult = 0;
do
{
wfsoResult = WaitForSingleObjectEx( stream->processingCompleted, timerPeriodMs * 10,   TRUE );
}while( wfsoResult == WAIT_TIMEOUT || wfsoResult == WAIT_IO_COMPLETION );
}
CancelWaitableTimer( stream->waitableTimer );
#else
while ( WaitForSingleObject( stream->processingCompleted, timerPeriodMs ) == WAIT_TIMEOUT )
{
TimerCallback( 0, 0, (DWORD_PTR)pArg, 0, 0 );
}
#endif  
SetEvent( stream->processingThreadCompleted );
return 0;
}
#endif  
static PaError CloseStream( PaStream* s )
{
PaError result = paNoError;
PaWinDsStream *stream = (PaWinDsStream*)s;
CloseHandle( stream->processingCompleted );
#ifdef PA_WIN_DS_USE_WAITABLE_TIMER_OBJECT
if( stream->waitableTimer != NULL )
CloseHandle( stream->waitableTimer );
#endif
#ifndef PA_WIN_DS_USE_WMME_TIMER
CloseHandle( stream->processingThreadCompleted );
#endif
if( stream->pDirectSoundOutputBuffer )
{
IDirectSoundBuffer_Stop( stream->pDirectSoundOutputBuffer );
IDirectSoundBuffer_Release( stream->pDirectSoundOutputBuffer );
stream->pDirectSoundOutputBuffer = NULL;
}
if( stream->pDirectSoundPrimaryBuffer )
{
IDirectSoundBuffer_Release( stream->pDirectSoundPrimaryBuffer );
stream->pDirectSoundPrimaryBuffer = NULL;
}
if( stream->pDirectSoundInputBuffer )
{
IDirectSoundCaptureBuffer_Stop( stream->pDirectSoundInputBuffer );
IDirectSoundCaptureBuffer_Release( stream->pDirectSoundInputBuffer );
stream->pDirectSoundInputBuffer = NULL;
}
if( stream->pDirectSoundCapture )
{
IDirectSoundCapture_Release( stream->pDirectSoundCapture );
stream->pDirectSoundCapture = NULL;
}
if( stream->pDirectSound )
{
IDirectSound_Release( stream->pDirectSound );
stream->pDirectSound = NULL;
}
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
if( stream->pDirectSoundFullDuplex8 )
{
IDirectSoundFullDuplex_Release( stream->pDirectSoundFullDuplex8 );
stream->pDirectSoundFullDuplex8 = NULL;
}
#endif
PaUtil_TerminateBufferProcessor( &stream->bufferProcessor );
PaUtil_TerminateStreamRepresentation( &stream->streamRepresentation );
PaUtil_FreeMemory( stream );
return result;
}
static HRESULT ClearOutputBuffer( PaWinDsStream *stream )
{
PaError          result = paNoError;
unsigned char*   pDSBuffData;
DWORD            dwDataLen;
HRESULT          hr;
hr = IDirectSoundBuffer_SetCurrentPosition( stream->pDirectSoundOutputBuffer, 0 );
DBUG(("PaHost_ClearOutputBuffer: IDirectSoundBuffer_SetCurrentPosition returned = 0x%X.\n", hr));
if( hr != DS_OK )
return hr;
if ((hr = IDirectSoundBuffer_Lock( stream->pDirectSoundOutputBuffer, 0, stream->outputBufferSizeBytes, (LPVOID*)&pDSBuffData,
&dwDataLen, NULL, 0, 0)) != DS_OK )
return hr;
ZeroMemory(pDSBuffData, dwDataLen);
if ((hr = IDirectSoundBuffer_Unlock( stream->pDirectSoundOutputBuffer, pDSBuffData, dwDataLen, NULL, 0)) != DS_OK)
return hr;
if ((hr = IDirectSoundBuffer_GetCurrentPosition( stream->pDirectSoundOutputBuffer,
&stream->previousPlayCursor, &stream->outputBufferWriteOffsetBytes )) != DS_OK)
return hr;
return DS_OK;
}
static PaError StartStream( PaStream *s )
{
PaError          result = paNoError;
PaWinDsStream   *stream = (PaWinDsStream*)s;
HRESULT          hr;
stream->callbackResult = paContinue;
PaUtil_ResetBufferProcessor( &stream->bufferProcessor );
ResetEvent( stream->processingCompleted );
#ifndef PA_WIN_DS_USE_WMME_TIMER
ResetEvent( stream->processingThreadCompleted );
#endif
if( stream->bufferProcessor.inputChannelCount > 0 )
{
if( stream->pDirectSoundInputBuffer != NULL )  
{
hr = IDirectSoundCaptureBuffer_Start( stream->pDirectSoundInputBuffer, DSCBSTART_LOOPING );
}
DBUG(("StartStream: DSW_StartInput returned = 0x%X.\n", hr));
if( hr != DS_OK )
{
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( hr );
goto error;
}
}
stream->framesWritten = 0;
stream->callbackFlags = 0;
stream->abortProcessing = 0;
stream->stopProcessing = 0;
if( stream->bufferProcessor.outputChannelCount > 0 )
{
QueryPerformanceCounter( &stream->previousPlayTime );
stream->finalZeroBytesWritten = 0;
hr = ClearOutputBuffer( stream );
if( hr != DS_OK )
{
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( hr );
goto error;
}
if( stream->streamRepresentation.streamCallback && (stream->streamFlags & paPrimeOutputBuffersUsingStreamCallback) )
{
stream->callbackFlags = paPrimingOutput;
TimeSlice( stream );
stream->callbackFlags = 0;
}
if( stream->pDirectSoundOutputBuffer != NULL )  
{
hr = IDirectSoundBuffer_Play( stream->pDirectSoundOutputBuffer, 0, 0, DSBPLAY_LOOPING );
DBUG(("PaHost_StartOutput: IDirectSoundBuffer_Play returned = 0x%X.\n", hr));
if( hr != DS_OK )
{
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( hr );
goto error;
}
stream->outputIsRunning = TRUE;
}
}
if( stream->streamRepresentation.streamCallback )
{
TIMECAPS timecaps;
int timerPeriodMs = (int)(stream->pollingPeriodSeconds * MSECS_PER_SECOND);
if( timerPeriodMs < 1 )
timerPeriodMs = 1;
assert( stream->systemTimerResolutionPeriodMs == 0 );
if( timeGetDevCaps( &timecaps, sizeof(TIMECAPS) ) == MMSYSERR_NOERROR && timecaps.wPeriodMin > 0 )
{
stream->systemTimerResolutionPeriodMs = (UINT)((stream->pollingPeriodSeconds * MSECS_PER_SECOND) * .25);
if( stream->systemTimerResolutionPeriodMs < timecaps.wPeriodMin )
stream->systemTimerResolutionPeriodMs = timecaps.wPeriodMin;
if( stream->systemTimerResolutionPeriodMs > timecaps.wPeriodMax )
stream->systemTimerResolutionPeriodMs = timecaps.wPeriodMax;
if( timeBeginPeriod( stream->systemTimerResolutionPeriodMs ) != MMSYSERR_NOERROR )
stream->systemTimerResolutionPeriodMs = 0;  
}
#ifdef PA_WIN_DS_USE_WMME_TIMER
stream->timerID = timeSetEvent( timerPeriodMs, stream->systemTimerResolutionPeriodMs, (LPTIMECALLBACK) TimerCallback,
(DWORD_PTR) stream, TIME_PERIODIC | TIME_KILL_SYNCHRONOUS );
if( stream->timerID == 0 )
{
stream->isActive = 0;
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( GetLastError() );
goto error;
}
#else
stream->processingThread = CREATE_THREAD( 0, 0, ProcessingThreadProc, stream, 0, &stream->processingThreadId );
if( !stream->processingThread )
{
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( GetLastError() );
goto error;
}
if( !SetThreadPriority( stream->processingThread, THREAD_PRIORITY_TIME_CRITICAL ) )
{
result = paUnanticipatedHostError;
PA_DS_SET_LAST_DIRECTSOUND_ERROR( GetLastError() );
goto error;
}
#endif
}
stream->isActive = 1;
stream->isStarted = 1;
assert( result == paNoError );
return result;
error:
if( stream->pDirectSoundOutputBuffer != NULL && stream->outputIsRunning )
IDirectSoundBuffer_Stop( stream->pDirectSoundOutputBuffer );
stream->outputIsRunning = FALSE;
#ifndef PA_WIN_DS_USE_WMME_TIMER
if( stream->processingThread )
{
#ifdef CLOSE_THREAD_HANDLE
CLOSE_THREAD_HANDLE( stream->processingThread );  
#endif
stream->processingThread = NULL;
}
#endif
return result;
}
static PaError StopStream( PaStream *s )
{
PaError result = paNoError;
PaWinDsStream *stream = (PaWinDsStream*)s;
HRESULT          hr;
int timeoutMsec;
if( stream->streamRepresentation.streamCallback )
{
stream->stopProcessing = 1;
timeoutMsec = (int) (4 * MSECS_PER_SECOND * (stream->hostBufferSizeFrames / stream->streamRepresentation.streamInfo.sampleRate));
WaitForSingleObject( stream->processingCompleted, timeoutMsec );
}
#ifdef PA_WIN_DS_USE_WMME_TIMER
if( stream->timerID != 0 )
{
timeKillEvent(stream->timerID);   
stream->timerID = 0;
}
#else
if( stream->processingThread )
{
if( WaitForSingleObject( stream->processingThreadCompleted, 30*100 ) == WAIT_TIMEOUT )
return paUnanticipatedHostError;
#ifdef CLOSE_THREAD_HANDLE
CloseHandle( stream->processingThread );  
stream->processingThread = NULL;
#endif
}
#endif
if( stream->systemTimerResolutionPeriodMs > 0 ){
timeEndPeriod( stream->systemTimerResolutionPeriodMs );
stream->systemTimerResolutionPeriodMs = 0;
}
if( stream->bufferProcessor.outputChannelCount > 0 )
{
if( stream->pDirectSoundOutputBuffer != NULL )
{
stream->outputIsRunning = FALSE;
hr = IDirectSoundBuffer_Stop( stream->pDirectSoundOutputBuffer );
if( stream->pDirectSoundPrimaryBuffer )
IDirectSoundBuffer_Stop( stream->pDirectSoundPrimaryBuffer );  
}
}
if( stream->bufferProcessor.inputChannelCount > 0 )
{
if( stream->pDirectSoundInputBuffer != NULL )
{
hr = IDirectSoundCaptureBuffer_Stop( stream->pDirectSoundInputBuffer );
}
}
stream->isStarted = 0;
return result;
}
static PaError AbortStream( PaStream *s )
{
PaWinDsStream *stream = (PaWinDsStream*)s;
stream->abortProcessing = 1;
return StopStream( s );
}
static PaError IsStreamStopped( PaStream *s )
{
PaWinDsStream *stream = (PaWinDsStream*)s;
return !stream->isStarted;
}
static PaError IsStreamActive( PaStream *s )
{
PaWinDsStream *stream = (PaWinDsStream*)s;
return stream->isActive;
}
static PaTime GetStreamTime( PaStream *s )
{
(void) s;
return PaUtil_GetTime();
}
static double GetStreamCpuLoad( PaStream* s )
{
PaWinDsStream *stream = (PaWinDsStream*)s;
return PaUtil_GetCpuLoad( &stream->cpuLoadMeasurer );
}
static PaError ReadStream( PaStream* s,
void *buffer,
unsigned long frames )
{
PaWinDsStream *stream = (PaWinDsStream*)s;
(void) buffer;
(void) frames;
(void) stream;
return paNoError;
}
static PaError WriteStream( PaStream* s,
const void *buffer,
unsigned long frames )
{
PaWinDsStream *stream = (PaWinDsStream*)s;
(void) buffer;
(void) frames;
(void) stream;
return paNoError;
}
static signed long GetStreamReadAvailable( PaStream* s )
{
PaWinDsStream *stream = (PaWinDsStream*)s;
(void) stream;
return 0;
}
static signed long GetStreamWriteAvailable( PaStream* s )
{
PaWinDsStream *stream = (PaWinDsStream*)s;
(void) stream;
return 0;
}

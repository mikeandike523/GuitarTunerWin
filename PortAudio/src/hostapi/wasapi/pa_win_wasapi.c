 
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <assert.h>
#ifndef PA_WASAPI_MAX_CONST_DEVICE_COUNT
#define PA_WASAPI_MAX_CONST_DEVICE_COUNT 0  
#endif
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#define PA_WINRT
#define INITGUID
#endif
#include <mmreg.h>   
#if defined(_MSC_VER) && (_MSC_VER >= 1400) || defined(__MINGW64_VERSION_MAJOR)
#include <avrt.h>
#define COBJMACROS
#include <audioclient.h>
#include <endpointvolume.h>
#define INITGUID  
#ifndef _MSC_VER
#include <functiondiscoverykeys_devpkey.h>
#endif
#include <functiondiscoverykeys.h>
#include <mmdeviceapi.h>
#include <devicetopology.h>     
#undef INITGUID
#if (_MSC_VER <= 1600)
#define inline _inline
#endif
#endif
#ifndef __MWERKS__
#include <malloc.h>
#include <memory.h>
#endif
#ifndef PA_WINRT
#include <mmsystem.h>
#endif
#include "pa_util.h"
#include "pa_allocation.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_cpuload.h"
#include "pa_process.h"
#include "pa_win_wasapi.h"
#include "pa_debugprint.h"
#include "pa_ringbuffer.h"
#include "pa_win_coinitialize.h"
#if !defined(NTDDI_VERSION) || (defined(__GNUC__) && (__GNUC__ <= 6) && !defined(__MINGW64__))
#undef WINVER
#undef _WIN32_WINNT
#define WINVER       0x0600  
#define _WIN32_WINNT WINVER
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifndef __unaligned
#define __unaligned
#endif
#ifndef __C89_NAMELESS
#define __C89_NAMELESS
#endif
#ifndef _AVRT_  
typedef enum _AVRT_PRIORITY
{
AVRT_PRIORITY_LOW = -1,
AVRT_PRIORITY_NORMAL,
AVRT_PRIORITY_HIGH,
AVRT_PRIORITY_CRITICAL
} AVRT_PRIORITY, *PAVRT_PRIORITY;
#endif
#include <basetyps.h>  
#include <rpcsal.h>
#include <sal.h>
#ifndef __LPCGUID_DEFINED__
#define __LPCGUID_DEFINED__
typedef const GUID *LPCGUID;
#endif
typedef GUID IID;
typedef GUID CLSID;
#ifndef PROPERTYKEY_DEFINED
#define PROPERTYKEY_DEFINED
typedef struct _tagpropertykey
{
GUID fmtid;
DWORD pid;
}     PROPERTYKEY;
#endif
#ifdef __midl_proxy
#define __MIDL_CONST
#else
#define __MIDL_CONST const
#endif
#ifdef WIN64
#include <wtypes.h>
#define FASTCALL
#include <oleidl.h>
#include <objidl.h>
#else
#ifndef _BLOB_DEFINED
typedef struct _BYTE_BLOB
{
unsigned long clSize;
unsigned char abData[ 1 ];
}     BYTE_BLOB;
typedef    __RPC_unique_pointer BYTE_BLOB *UP_BYTE_BLOB;
#endif
typedef LONGLONG REFERENCE_TIME;
#define NONAMELESSUNION
#endif
#ifndef NT_SUCCESS
typedef LONG NTSTATUS;
#endif
#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 0x0003  
#endif
#ifndef __MINGW_EXTENSION
#if defined(__GNUC__) || defined(__GNUG__)
#define __MINGW_EXTENSION __extension__
#else
#define __MINGW_EXTENSION
#endif
#endif
#include <sdkddkver.h>
#include <propkeydef.h>
#define COBJMACROS
#define INITGUID  
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys.h>
#include <devicetopology.h>     
#undef INITGUID
#endif  
#ifdef PA_WINRT
#define DEVICE_STATE_ACTIVE 0x00000001
typedef enum _EDataFlow
{
eRender                 = 0,
eCapture                = ( eRender + 1 ) ,
eAll                    = ( eCapture + 1 ) ,
EDataFlow_enum_count    = ( eAll + 1 )
}
EDataFlow;
typedef enum _EndpointFormFactor
{
RemoteNetworkDevice       = 0,
Speakers                  = ( RemoteNetworkDevice + 1 ) ,
LineLevel                 = ( Speakers + 1 ) ,
Headphones                = ( LineLevel + 1 ) ,
Microphone                = ( Headphones + 1 ) ,
Headset                   = ( Microphone + 1 ) ,
Handset                   = ( Headset + 1 ) ,
UnknownDigitalPassthrough = ( Handset + 1 ) ,
SPDIF                     = ( UnknownDigitalPassthrough + 1 ) ,
HDMI                      = ( SPDIF + 1 ) ,
UnknownFormFactor         = ( HDMI + 1 )
}
EndpointFormFactor;
#endif
#ifndef GUID_SECT
#define GUID_SECT
#endif
#define __DEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const GUID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __DEFINE_IID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const IID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __DEFINE_CLSID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const CLSID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define PA_DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
__DEFINE_CLSID(pa_CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
#define PA_DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
__DEFINE_IID(pa_IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
PA_DEFINE_IID(IAudioClient,         1cb9ad4c, dbfa, 4c32, b1, 78, c2, f5, 68, a7, 03, b2);
PA_DEFINE_IID(IAudioClient2,        726778cd, f60a, 4eda, 82, de, e4, 76, 10, cd, 78, aa);
PA_DEFINE_IID(IAudioClient3,        7ed4ee07, 8e67, 4cd4, 8c, 1a, 2b, 7a, 59, 87, ad, 42);
PA_DEFINE_IID(IMMEndpoint,          1be09788, 6894, 4089, 85, 86, 9a, 2a, 6c, 26, 5a, c5);
PA_DEFINE_IID(IMMDeviceEnumerator,  a95664d2, 9614, 4f35, a7, 46, de, 8d, b6, 36, 17, e6);
PA_DEFINE_CLSID(IMMDeviceEnumerator,bcde0395, e52f, 467c, 8e, 3d, c4, 57, 92, 91, 69, 2e);
PA_DEFINE_IID(IAudioRenderClient,   f294acfc, 3146, 4483, a7, bf, ad, dc, a7, c2, 60, e2);
PA_DEFINE_IID(IAudioCaptureClient,  c8adbd64, e71e, 48a0, a4, de, 18, 5c, 39, 5c, d3, 17);
PA_DEFINE_IID(IDeviceTopology,      2A07407E, 6497, 4A18, 97, 87, 32, f7, 9b, d0, d9, 8f);
PA_DEFINE_IID(IPart,                AE2DE0E4, 5BCA, 4F2D, aa, 46, 5d, 13, f8, fd, b3, a9);
PA_DEFINE_IID(IKsJackDescription,   4509F757, 2D46, 4637, 8e, 62, ce, 7d, b9, 44, f5, 7b);
__DEFINE_GUID(pa_KSDATAFORMAT_SUBTYPE_PCM,        0x00000001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
__DEFINE_GUID(pa_KSDATAFORMAT_SUBTYPE_ADPCM,      0x00000002, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
__DEFINE_GUID(pa_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 0x00000003, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
#ifdef __IAudioClient2_INTERFACE_DEFINED__
typedef enum _pa_AUDCLNT_STREAMOPTIONS {
pa_AUDCLNT_STREAMOPTIONS_NONE          = 0x00,
pa_AUDCLNT_STREAMOPTIONS_RAW           = 0x01,
pa_AUDCLNT_STREAMOPTIONS_MATCH_FORMAT  = 0x02
} pa_AUDCLNT_STREAMOPTIONS;
typedef struct _pa_AudioClientProperties {
UINT32                   cbSize;
BOOL                     bIsOffload;
AUDIO_STREAM_CATEGORY    eCategory;
pa_AUDCLNT_STREAMOPTIONS Options;
} pa_AudioClientProperties;
#define PA_AUDIOCLIENTPROPERTIES_SIZE_CATEGORY (sizeof(pa_AudioClientProperties) - sizeof(pa_AUDCLNT_STREAMOPTIONS))
#define PA_AUDIOCLIENTPROPERTIES_SIZE_OPTIONS   sizeof(pa_AudioClientProperties)
#endif  
#if !defined(__CYGWIN__) && !defined(_WIN32_WCE)
#define CREATE_THREAD(PROC) (HANDLE)_beginthreadex( NULL, 0, (PROC), stream, 0, &stream->dwThreadId )
#define PA_THREAD_FUNC static unsigned WINAPI
#define PA_THREAD_ID unsigned
#else
#define CREATE_THREAD(PROC) CreateThread( NULL, 0, (PROC), stream, 0, &stream->dwThreadId )
#define PA_THREAD_FUNC static DWORD WINAPI
#define PA_THREAD_ID DWORD
#endif
PA_THREAD_FUNC ProcThreadEvent(void *param);
PA_THREAD_FUNC ProcThreadPoll(void *param);
#ifndef AUDCLNT_E_BUFFER_ERROR
#define AUDCLNT_E_BUFFER_ERROR AUDCLNT_ERR(0x018)
#endif
#ifndef AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED
#define AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED AUDCLNT_ERR(0x019)
#endif
#ifndef AUDCLNT_E_INVALID_DEVICE_PERIOD
#define AUDCLNT_E_INVALID_DEVICE_PERIOD AUDCLNT_ERR(0x020)
#endif
#ifndef AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
#define AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY 0x08000000
#endif
#ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
#define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000
#endif
#define PA_WASAPI_DEVICE_ID_LEN   256
#define PA_WASAPI_DEVICE_NAME_LEN 128
#ifdef PA_WINRT
#define PA_WASAPI_DEVICE_MAX_COUNT 16
#endif
enum { S_INPUT = 0, S_OUTPUT = 1, S_COUNT = 2, S_FULLDUPLEX = 0 };
enum { WASAPI_PACKETS_PER_INPUT_BUFFER = 6 };
#define STATIC_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define PRINT(x) PA_DEBUG(x);
#define PA_SKELETON_SET_LAST_HOST_ERROR( errorCode, errorText ) \
PaUtil_SetLastHostErrorInfo( paWASAPI, errorCode, errorText )
#define PA_WASAPI__IS_FULLDUPLEX(STREAM) ((STREAM)->in.clientProc && (STREAM)->out.clientProc)
#ifndef IF_FAILED_JUMP
#define IF_FAILED_JUMP(hr, label) if(FAILED(hr)) goto label;
#endif
#ifndef IF_FAILED_INTERNAL_ERROR_JUMP
#define IF_FAILED_INTERNAL_ERROR_JUMP(hr, error, label) if(FAILED(hr)) { error = paInternalError; goto label; }
#endif
#define SAFE_CLOSE(h) if ((h) != NULL) { CloseHandle((h)); (h) = NULL; }
#define SAFE_RELEASE(punk) if ((punk) != NULL) { (punk)->lpVtbl->Release((punk)); (punk) = NULL; }
typedef void (*MixMonoToStereoF) (void *__to, const void *__from, UINT32 count);
#ifndef PA_WINRT
typedef BOOL   (WINAPI *FAvRtCreateThreadOrderingGroup)  (PHANDLE,PLARGE_INTEGER,GUID*,PLARGE_INTEGER);
typedef BOOL   (WINAPI *FAvRtDeleteThreadOrderingGroup)  (HANDLE);
typedef BOOL   (WINAPI *FAvRtWaitOnThreadOrderingGroup)  (HANDLE);
typedef HANDLE (WINAPI *FAvSetMmThreadCharacteristics)   (LPCSTR,LPDWORD);
typedef BOOL   (WINAPI *FAvRevertMmThreadCharacteristics)(HANDLE);
typedef BOOL   (WINAPI *FAvSetMmThreadPriority)          (HANDLE,AVRT_PRIORITY);
static HMODULE hDInputDLL = 0;
FAvRtCreateThreadOrderingGroup   pAvRtCreateThreadOrderingGroup = NULL;
FAvRtDeleteThreadOrderingGroup   pAvRtDeleteThreadOrderingGroup = NULL;
FAvRtWaitOnThreadOrderingGroup   pAvRtWaitOnThreadOrderingGroup = NULL;
FAvSetMmThreadCharacteristics    pAvSetMmThreadCharacteristics = NULL;
FAvRevertMmThreadCharacteristics pAvRevertMmThreadCharacteristics = NULL;
FAvSetMmThreadPriority           pAvSetMmThreadPriority = NULL;
#endif
#define _GetProc(fun, type, name)  {                                                        \
fun = (type) GetProcAddress(hDInputDLL,name);       \
if (fun == NULL) {                                  \
PRINT(("GetProcAddr failed for %s" ,name));     \
return FALSE;                                   \
}                                                   \
}                                                       \
#ifdef __cplusplus
extern "C"
{
#endif  
PaError PaWasapi_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
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
typedef struct PaWasapiDeviceInfo
{
#ifndef PA_WINRT
IMMDevice *device;
#endif
WCHAR deviceId[PA_WASAPI_DEVICE_ID_LEN];
DWORD state;
REFERENCE_TIME DefaultDevicePeriod;
REFERENCE_TIME MinimumDevicePeriod;
WAVEFORMATEXTENSIBLE DefaultFormat;
WAVEFORMATEXTENSIBLE MixFormat;
EDataFlow flow;
EndpointFormFactor formFactor;
}
PaWasapiDeviceInfo;
typedef struct
{
PaUtilHostApiRepresentation inheritedHostApiRep;
PaUtilStreamInterface       callbackStreamInterface;
PaUtilStreamInterface       blockingStreamInterface;
PaUtilAllocationGroup      *allocations;
PaWinUtilComInitializationResult comInitializationResult;
UINT32 deviceCount;
PaWasapiDeviceInfo *devInfo;
BOOL useWOW64Workaround;
}
PaWasapiHostApiRepresentation;
typedef struct PaWasapiAudioClientParams
{
PaWasapiDeviceInfo *device_info;
PaStreamParameters  stream_params;
PaWasapiStreamInfo  wasapi_params;
UINT32              frames_per_buffer;
double              sample_rate;
BOOL                blocking;
BOOL                full_duplex;
BOOL                wow64_workaround;
}
PaWasapiAudioClientParams;
typedef struct PaWasapiSubStream
{
IAudioClient        *clientParent;
#ifndef PA_WINRT
IStream             *clientStream;
#endif
IAudioClient        *clientProc;
WAVEFORMATEXTENSIBLE wavex;
UINT32               bufferSize;
REFERENCE_TIME       deviceLatency;
REFERENCE_TIME       period;
double               latencySeconds;
UINT32               framesPerHostCallback;
AUDCLNT_SHAREMODE    shareMode;
UINT32               streamFlags;  
UINT32               flags;
PaWasapiAudioClientParams params;  
UINT32               buffers;             
UINT32               framesPerBuffer;     
BOOL                 userBufferAndHostMatch;
void                *monoBuffer;      
UINT32               monoBufferSize;  
MixMonoToStereoF     monoMixer;          
PaUtilRingBuffer    *tailBuffer;        
void                *tailBufferMemory;  
}
PaWasapiSubStream;
typedef struct PaWasapiHostProcessor
{
PaWasapiHostProcessorCallback processor;
void *userData;
}
PaWasapiHostProcessor;
typedef struct PaWasapiStream
{
PaUtilStreamRepresentation streamRepresentation;
PaUtilCpuLoadMeasurer      cpuLoadMeasurer;
PaUtilBufferProcessor      bufferProcessor;
PaWasapiSubStream          in;
IAudioCaptureClient       *captureClientParent;
#ifndef PA_WINRT
IStream                   *captureClientStream;
#endif
IAudioCaptureClient       *captureClient;
IAudioEndpointVolume      *inVol;
PaWasapiSubStream          out;
IAudioRenderClient        *renderClientParent;
#ifndef PA_WINRT
IStream                   *renderClientStream;
#endif
IAudioRenderClient        *renderClient;
IAudioEndpointVolume      *outVol;
HANDLE event[S_COUNT];
PaUtilHostBufferSizeMode bufferMode;
volatile BOOL running;
PA_THREAD_ID dwThreadId;
HANDLE hThread;
HANDLE hCloseRequest;
HANDLE hThreadStart;         
HANDLE hThreadExit;          
HANDLE hBlockingOpStreamRD;
HANDLE hBlockingOpStreamWR;
PaWasapiHostProcessor hostProcessOverrideOutput;
PaWasapiHostProcessor hostProcessOverrideInput;
BOOL bBlocking;
HANDLE hAvTask;
PaWasapiThreadPriority nThreadPriority;
PaWasapiStreamStateCallback fnStateHandler;
void *pStateHandlerUserData;
}
PaWasapiStream;
static HRESULT MarshalSubStreamComPointers(PaWasapiSubStream *substream);
static HRESULT MarshalStreamComPointers(PaWasapiStream *stream);
static HRESULT UnmarshalSubStreamComPointers(PaWasapiSubStream *substream);
static HRESULT UnmarshalStreamComPointers(PaWasapiStream *stream);
static void ReleaseUnmarshaledSubComPointers(PaWasapiSubStream *substream);
static void ReleaseUnmarshaledComPointers(PaWasapiStream *stream);
static void _StreamOnStop(PaWasapiStream *stream);
static void _StreamFinish(PaWasapiStream *stream);
static void _StreamCleanup(PaWasapiStream *stream);
static HRESULT _PollGetOutputFramesAvailable(PaWasapiStream *stream, UINT32 *available);
static HRESULT _PollGetInputFramesAvailable(PaWasapiStream *stream, UINT32 *available);
static void *PaWasapi_ReallocateMemory(void *prev, size_t size);
static void PaWasapi_FreeMemory(void *ptr);
static PaSampleFormat WaveToPaFormat(const WAVEFORMATEXTENSIBLE *fmtext);
#ifdef PA_WINRT
typedef struct PaWasapiWinrtDeviceInfo
{
WCHAR              id[PA_WASAPI_DEVICE_ID_LEN];
WCHAR              name[PA_WASAPI_DEVICE_NAME_LEN];
EndpointFormFactor formFactor;
}
PaWasapiWinrtDeviceInfo;
typedef struct PaWasapiWinrtDeviceListRole
{
WCHAR                   defaultId[PA_WASAPI_DEVICE_ID_LEN];
PaWasapiWinrtDeviceInfo devices[PA_WASAPI_DEVICE_MAX_COUNT];
UINT32                  deviceCount;
}
PaWasapiWinrtDeviceListRole;
typedef struct PaWasapiWinrtDeviceList
{
PaWasapiWinrtDeviceListRole render;
PaWasapiWinrtDeviceListRole capture;
}
PaWasapiWinrtDeviceList;
static PaWasapiWinrtDeviceList g_DeviceListInfo = { 0 };
#endif
#ifdef PA_WINRT
typedef struct PaWasapiWinrtDeviceListContextEntry
{
PaWasapiWinrtDeviceInfo *info;
EDataFlow                flow;
}
PaWasapiWinrtDeviceListContextEntry;
typedef struct PaWasapiWinrtDeviceListContext
{
PaWasapiWinrtDeviceListContextEntry devices[PA_WASAPI_DEVICE_MAX_COUNT * 2];
}
PaWasapiWinrtDeviceListContext;
#endif
#define LogHostError(HRES) __LogHostError(HRES, __FUNCTION__, __FILE__, __LINE__)
static HRESULT __LogHostError(HRESULT res, const char *func, const char *file, int line)
{
const char *text = NULL;
switch (res)
{
case S_OK: return res;
case E_POINTER                              :text ="E_POINTER"; break;
case E_INVALIDARG                           :text ="E_INVALIDARG"; break;
case AUDCLNT_E_NOT_INITIALIZED              :text ="AUDCLNT_E_NOT_INITIALIZED"; break;
case AUDCLNT_E_ALREADY_INITIALIZED          :text ="AUDCLNT_E_ALREADY_INITIALIZED"; break;
case AUDCLNT_E_WRONG_ENDPOINT_TYPE          :text ="AUDCLNT_E_WRONG_ENDPOINT_TYPE"; break;
case AUDCLNT_E_DEVICE_INVALIDATED           :text ="AUDCLNT_E_DEVICE_INVALIDATED"; break;
case AUDCLNT_E_NOT_STOPPED                  :text ="AUDCLNT_E_NOT_STOPPED"; break;
case AUDCLNT_E_BUFFER_TOO_LARGE             :text ="AUDCLNT_E_BUFFER_TOO_LARGE"; break;
case AUDCLNT_E_OUT_OF_ORDER                 :text ="AUDCLNT_E_OUT_OF_ORDER"; break;
case AUDCLNT_E_UNSUPPORTED_FORMAT           :text ="AUDCLNT_E_UNSUPPORTED_FORMAT"; break;
case AUDCLNT_E_INVALID_SIZE                 :text ="AUDCLNT_E_INVALID_SIZE"; break;
case AUDCLNT_E_DEVICE_IN_USE                :text ="AUDCLNT_E_DEVICE_IN_USE"; break;
case AUDCLNT_E_BUFFER_OPERATION_PENDING     :text ="AUDCLNT_E_BUFFER_OPERATION_PENDING"; break;
case AUDCLNT_E_THREAD_NOT_REGISTERED        :text ="AUDCLNT_E_THREAD_NOT_REGISTERED"; break;
case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED   :text ="AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED"; break;
case AUDCLNT_E_ENDPOINT_CREATE_FAILED       :text ="AUDCLNT_E_ENDPOINT_CREATE_FAILED"; break;
case AUDCLNT_E_SERVICE_NOT_RUNNING          :text ="AUDCLNT_E_SERVICE_NOT_RUNNING"; break;
case AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED     :text ="AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED"; break;
case AUDCLNT_E_EXCLUSIVE_MODE_ONLY          :text ="AUDCLNT_E_EXCLUSIVE_MODE_ONLY"; break;
case AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL :text ="AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL"; break;
case AUDCLNT_E_EVENTHANDLE_NOT_SET          :text ="AUDCLNT_E_EVENTHANDLE_NOT_SET"; break;
case AUDCLNT_E_INCORRECT_BUFFER_SIZE        :text ="AUDCLNT_E_INCORRECT_BUFFER_SIZE"; break;
case AUDCLNT_E_BUFFER_SIZE_ERROR            :text ="AUDCLNT_E_BUFFER_SIZE_ERROR"; break;
case AUDCLNT_E_CPUUSAGE_EXCEEDED            :text ="AUDCLNT_E_CPUUSAGE_EXCEEDED"; break;
case AUDCLNT_E_BUFFER_ERROR                 :text ="AUDCLNT_E_BUFFER_ERROR"; break;
case AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED      :text ="AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED"; break;
case AUDCLNT_E_INVALID_DEVICE_PERIOD        :text ="AUDCLNT_E_INVALID_DEVICE_PERIOD"; break;
#ifdef AUDCLNT_E_INVALID_STREAM_FLAG
case AUDCLNT_E_INVALID_STREAM_FLAG          :text ="AUDCLNT_E_INVALID_STREAM_FLAG"; break;
#endif
#ifdef AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE
case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE :text ="AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE"; break;
#endif
#ifdef AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES
case AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES     :text ="AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES"; break;
#endif
#ifdef AUDCLNT_E_OFFLOAD_MODE_ONLY
case AUDCLNT_E_OFFLOAD_MODE_ONLY            :text ="AUDCLNT_E_OFFLOAD_MODE_ONLY"; break;
#endif
#ifdef AUDCLNT_E_NONOFFLOAD_MODE_ONLY
case AUDCLNT_E_NONOFFLOAD_MODE_ONLY         :text ="AUDCLNT_E_NONOFFLOAD_MODE_ONLY"; break;
#endif
#ifdef AUDCLNT_E_RESOURCES_INVALIDATED
case AUDCLNT_E_RESOURCES_INVALIDATED        :text ="AUDCLNT_E_RESOURCES_INVALIDATED"; break;
#endif
#ifdef AUDCLNT_E_RAW_MODE_UNSUPPORTED
case AUDCLNT_E_RAW_MODE_UNSUPPORTED         :text ="AUDCLNT_E_RAW_MODE_UNSUPPORTED"; break;
#endif
#ifdef AUDCLNT_E_ENGINE_PERIODICITY_LOCKED
case AUDCLNT_E_ENGINE_PERIODICITY_LOCKED    :text ="AUDCLNT_E_ENGINE_PERIODICITY_LOCKED"; break;
#endif
#ifdef AUDCLNT_E_ENGINE_FORMAT_LOCKED
case AUDCLNT_E_ENGINE_FORMAT_LOCKED         :text ="AUDCLNT_E_ENGINE_FORMAT_LOCKED"; break;
#endif
case AUDCLNT_S_BUFFER_EMPTY                 :text ="AUDCLNT_S_BUFFER_EMPTY"; break;
case AUDCLNT_S_THREAD_ALREADY_REGISTERED    :text ="AUDCLNT_S_THREAD_ALREADY_REGISTERED"; break;
case AUDCLNT_S_POSITION_STALLED             :text ="AUDCLNT_S_POSITION_STALLED"; break;
case CO_E_NOTINITIALIZED                    :text ="CO_E_NOTINITIALIZED: you must call CoInitialize() before Pa_OpenStream()"; break;
default:
text = "UNKNOWN ERROR";
}
PRINT(("WASAPI ERROR HRESULT: 0x%X : %s\n [FUNCTION: %s FILE: %s {LINE: %d}]\n", res, text, func, file, line));
#ifndef PA_ENABLE_DEBUG_OUTPUT
(void)func; (void)file; (void)line;
#endif
PA_SKELETON_SET_LAST_HOST_ERROR(res, text);
return res;
}
#define LogPaError(PAERR) __LogPaError(PAERR, __FUNCTION__, __FILE__, __LINE__)
static PaError __LogPaError(PaError err, const char *func, const char *file, int line)
{
if (err == paNoError)
return err;
PRINT(("WASAPI ERROR PAERROR: %i : %s\n [FUNCTION: %s FILE: %s {LINE: %d}]\n", err, Pa_GetErrorText(err), func, file, line));
#ifndef PA_ENABLE_DEBUG_OUTPUT
(void)func; (void)file; (void)line;
#endif
return err;
}
typedef struct ThreadIdleScheduler
{
UINT32 m_idle_microseconds;  
UINT32 m_next_sleep;         
UINT32 m_i;                     
UINT32 m_resolution;         
}
ThreadIdleScheduler;
static void ThreadIdleScheduler_Setup(ThreadIdleScheduler *sched, UINT32 resolution, UINT32 microseconds)
{
assert(microseconds != 0);
assert(resolution != 0);
assert((resolution * 1000) >= microseconds);
memset(sched, 0, sizeof(*sched));
sched->m_idle_microseconds = microseconds;
sched->m_resolution        = resolution;
sched->m_next_sleep        = (resolution * 1000) / microseconds;
}
static inline UINT32 ThreadIdleScheduler_NextSleep(ThreadIdleScheduler *sched)
{
if (++sched->m_i == sched->m_next_sleep)
{
sched->m_i = 0;
return sched->m_resolution;
}
return 0;
}
typedef struct _SystemTimer
{
INT32 granularity;
} SystemTimer;
static LARGE_INTEGER g_SystemTimerFrequency;
static BOOL          g_SystemTimerUseQpc = FALSE;
static BOOL SystemTimer_SetGranularity(SystemTimer *timer, UINT32 granularity)
{
#ifndef PA_WINRT
TIMECAPS caps;
timer->granularity = granularity;
if (timeGetDevCaps(&caps, sizeof(caps)) == MMSYSERR_NOERROR)
{
if (timer->granularity < (INT32)caps.wPeriodMin)
timer->granularity = (INT32)caps.wPeriodMin;
}
if (timeBeginPeriod(timer->granularity) != TIMERR_NOERROR)
{
PRINT(("SetSystemTimer: timeBeginPeriod(1) failed!\n"));
timer->granularity = 10;
return FALSE;
}
#else
(void)granularity;
timer->granularity = 10;
#endif
return TRUE;
}
static void SystemTimer_RestoreGranularity(SystemTimer *timer)
{
#ifndef PA_WINRT
if (timer->granularity != 0)
{
if (timeEndPeriod(timer->granularity) != TIMERR_NOERROR)
{
PRINT(("RestoreSystemTimer: timeEndPeriod(1) failed!\n"));
}
}
#else
(void)timer;
#endif
}
static void SystemTimer_InitializeTimeGetter()
{
g_SystemTimerUseQpc = QueryPerformanceFrequency(&g_SystemTimerFrequency);
}
static inline LONGLONG SystemTimer_GetTime(SystemTimer *timer)
{
(void)timer;
if (g_SystemTimerUseQpc)
{
LARGE_INTEGER now;
QueryPerformanceCounter(&now);
return (now.QuadPart * 1000LL) / g_SystemTimerFrequency.QuadPart;
}
else
{
#ifdef PA_WINRT
return GetTickCount64();
#else
return timeGetTime();
#endif
}
}
static double nano100ToSeconds(REFERENCE_TIME ref)
{
return ((double)ref) * 0.0000001;
}
static REFERENCE_TIME SecondsTonano100(double ref)
{
return (REFERENCE_TIME)(ref / 0.0000001);
}
static REFERENCE_TIME MakeHnsPeriod(UINT32 nFrames, DWORD nSamplesPerSec)
{
return (REFERENCE_TIME)((10000.0 * 1000 / nSamplesPerSec * nFrames) + 0.5);
}
static WORD PaSampleFormatToBitsPerSample(PaSampleFormat format_id)
{
switch (format_id & ~paNonInterleaved)
{
case paFloat32:
case paInt32: return 32;
case paCustomFormat:
case paInt24: return 24;
case paInt16: return 16;
case paInt8:
case paUInt8: return 8;
}
return 0;
}
static PaSampleFormat GetSampleFormatForIO(PaSampleFormat format_id)
{
return ((format_id & ~paNonInterleaved) == paCustomFormat ?
(paInt32 | (format_id & paNonInterleaved ? paNonInterleaved : 0)) : format_id);
}
static UINT32 MakeFramesFromHns(REFERENCE_TIME hnsPeriod, UINT32 nSamplesPerSec)
{
UINT32 nFrames = (UINT32)(     
1.0 * hnsPeriod *         
nSamplesPerSec /         
1000 /                     
10000                     
+ 0.5                     
);
return nFrames;
}
typedef UINT32 (*ALIGN_FUNC) (UINT32 v, UINT32 align);
static UINT32 ALIGN_BWD(UINT32 v, UINT32 align)
{
return ((v - (align ? v % align : 0)));
}
static UINT32 ALIGN_FWD(UINT32 v, UINT32 align)
{
UINT32 remainder = (align ? (v % align) : 0);
if (remainder == 0)
return v;
return v + (align - remainder);
}
static UINT32 ALIGN_NEXT_POW2(UINT32 v)
{
UINT32 v2 = 1;
while (v > (v2 <<= 1)) { }
v = v2;
return v;
}
static UINT32 AlignFramesPerBuffer(UINT32 nFrames, UINT32 nBlockAlign, ALIGN_FUNC pAlignFunc)
{
#define HDA_PACKET_SIZE (128)
UINT32 bytes = nFrames * nBlockAlign;
UINT32 packets;
bytes = pAlignFunc(bytes, HDA_PACKET_SIZE);
if (bytes < HDA_PACKET_SIZE)
bytes = HDA_PACKET_SIZE;
packets = bytes / HDA_PACKET_SIZE;
bytes   = packets * HDA_PACKET_SIZE;
nFrames = bytes / nBlockAlign;
nFrames = ALIGN_FWD(nFrames, 8);
return nFrames;
#undef HDA_PACKET_SIZE
}
static UINT32 GetFramesSleepTime(REFERENCE_TIME nFrames, REFERENCE_TIME nSamplesPerSec)
{
REFERENCE_TIME nDuration;
if (nSamplesPerSec == 0)
return 0;
#define REFTIMES_PER_SEC       10000000LL
#define REFTIMES_PER_MILLISEC  10000LL
nDuration = (REFTIMES_PER_SEC * nFrames) / nSamplesPerSec;
return (UINT32)(nDuration / REFTIMES_PER_MILLISEC);
#undef REFTIMES_PER_SEC
#undef REFTIMES_PER_MILLISEC
}
static UINT32 GetFramesSleepTimeMicroseconds(REFERENCE_TIME nFrames, REFERENCE_TIME nSamplesPerSec)
{
REFERENCE_TIME nDuration;
if (nSamplesPerSec == 0)
return 0;
#define REFTIMES_PER_SEC       10000000LL
#define REFTIMES_PER_MILLISEC  10000LL
nDuration = (REFTIMES_PER_SEC * nFrames) / nSamplesPerSec;
return (UINT32)(nDuration / 10);
#undef REFTIMES_PER_SEC
#undef REFTIMES_PER_MILLISEC
}
#ifndef PA_WINRT
static BOOL SetupAVRT()
{
hDInputDLL = LoadLibraryA("avrt.dll");
if (hDInputDLL == NULL)
return FALSE;
_GetProc(pAvRtCreateThreadOrderingGroup,  FAvRtCreateThreadOrderingGroup,  "AvRtCreateThreadOrderingGroup");
_GetProc(pAvRtDeleteThreadOrderingGroup,  FAvRtDeleteThreadOrderingGroup,  "AvRtDeleteThreadOrderingGroup");
_GetProc(pAvRtWaitOnThreadOrderingGroup,  FAvRtWaitOnThreadOrderingGroup,  "AvRtWaitOnThreadOrderingGroup");
_GetProc(pAvSetMmThreadCharacteristics,   FAvSetMmThreadCharacteristics,   "AvSetMmThreadCharacteristicsA");
_GetProc(pAvRevertMmThreadCharacteristics,FAvRevertMmThreadCharacteristics,"AvRevertMmThreadCharacteristics");
_GetProc(pAvSetMmThreadPriority,          FAvSetMmThreadPriority,          "AvSetMmThreadPriority");
return pAvRtCreateThreadOrderingGroup &&
pAvRtDeleteThreadOrderingGroup &&
pAvRtWaitOnThreadOrderingGroup &&
pAvSetMmThreadCharacteristics &&
pAvRevertMmThreadCharacteristics &&
pAvSetMmThreadPriority;
}
#endif
static void CloseAVRT()
{
#ifndef PA_WINRT
if (hDInputDLL != NULL)
FreeLibrary(hDInputDLL);
hDInputDLL = NULL;
#endif
}
static BOOL IsWow64()
{
#ifndef PA_WINRT
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;
BOOL bIsWow64 = FALSE;
fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
GetModuleHandleA("kernel32"), "IsWow64Process");
if (fnIsWow64Process == NULL)
return FALSE;
if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
return FALSE;
return bIsWow64;
#else
return FALSE;
#endif
}
typedef enum EWindowsVersion
{
WINDOWS_UNKNOWN = 0,
WINDOWS_VISTA_SERVER2008,
WINDOWS_7_SERVER2008R2,
WINDOWS_8_SERVER2012,
WINDOWS_8_1_SERVER2012R2,
WINDOWS_10_SERVER2016,
WINDOWS_FUTURE
}
EWindowsVersion;
#ifndef PA_WINRT
static BOOL IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
typedef ULONGLONG (NTAPI *LPFN_VERSETCONDITIONMASK)(ULONGLONG ConditionMask, DWORD TypeMask, BYTE Condition);
typedef BOOL (WINAPI *LPFN_VERIFYVERSIONINFO)(LPOSVERSIONINFOEXA lpVersionInformation, DWORD dwTypeMask, DWORDLONG dwlConditionMask);
LPFN_VERSETCONDITIONMASK fnVerSetConditionMask;
LPFN_VERIFYVERSIONINFO fnVerifyVersionInfo;
OSVERSIONINFOEXA osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
DWORDLONG dwlConditionMask;
fnVerSetConditionMask = (LPFN_VERSETCONDITIONMASK)GetProcAddress(GetModuleHandleA("kernel32"), "VerSetConditionMask");
fnVerifyVersionInfo = (LPFN_VERIFYVERSIONINFO)GetProcAddress(GetModuleHandleA("kernel32"), "VerifyVersionInfoA");
if ((fnVerSetConditionMask == NULL) || (fnVerifyVersionInfo == NULL))
return FALSE;
dwlConditionMask = fnVerSetConditionMask(
fnVerSetConditionMask(
fnVerSetConditionMask(
0, VER_MAJORVERSION,     VER_GREATER_EQUAL),
VER_MINORVERSION,     VER_GREATER_EQUAL),
VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
osvi.dwMajorVersion    = wMajorVersion;
osvi.dwMinorVersion    = wMinorVersion;
osvi.wServicePackMajor = wServicePackMajor;
return (fnVerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE);
}
#endif
static EWindowsVersion GetWindowsVersion()
{
#ifndef PA_WINRT
static EWindowsVersion version = WINDOWS_UNKNOWN;
if (version == WINDOWS_UNKNOWN)
{
DWORD dwMajorVersion = 0xFFFFFFFFU, dwMinorVersion = 0, dwBuild = 0;
typedef NTSTATUS (WINAPI *LPFN_RTLGETVERSION)(POSVERSIONINFOW lpVersionInformation);
LPFN_RTLGETVERSION fnRtlGetVersion;
#define NTSTATUS_SUCCESS ((NTSTATUS)0x00000000L)
if ((fnRtlGetVersion = (LPFN_RTLGETVERSION)GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion")) != NULL)
{
OSVERSIONINFOW ver = { sizeof(OSVERSIONINFOW), 0, 0, 0, 0, {0} };
PRINT(("WASAPI: getting Windows version with RtlGetVersion()\n"));
if (fnRtlGetVersion(&ver) == NTSTATUS_SUCCESS)
{
dwMajorVersion = ver.dwMajorVersion;
dwMinorVersion = ver.dwMinorVersion;
dwBuild        = ver.dwBuildNumber;
}
}
#undef NTSTATUS_SUCCESS
if (dwMajorVersion == 0xFFFFFFFFU)
{
typedef DWORD (WINAPI *LPFN_GETVERSION)(VOID);
LPFN_GETVERSION fnGetVersion;
if ((fnGetVersion = (LPFN_GETVERSION)GetProcAddress(GetModuleHandleA("kernel32"), "GetVersion")) != NULL)
{
DWORD dwVersion;
PRINT(("WASAPI: getting Windows version with GetVersion()\n"));
dwVersion = fnGetVersion();
dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
if (dwVersion < 0x80000000)
dwBuild = (DWORD)(HIWORD(dwVersion));
}
}
if (dwMajorVersion != 0xFFFFFFFFU)
{
switch (dwMajorVersion)
{
case 0:
case 1:
case 2:
case 3:
case 4:
case 5:
break;  
case 6:
switch (dwMinorVersion)
{
case 0:  version = WINDOWS_VISTA_SERVER2008; break;
case 1:  version = WINDOWS_7_SERVER2008R2;   break;
case 2:  version = WINDOWS_8_SERVER2012;     break;
case 3:  version = WINDOWS_8_1_SERVER2012R2; break;
default: version = WINDOWS_FUTURE;           break;
}
break;
case 10:
switch (dwMinorVersion)
{
case 0:  version = WINDOWS_10_SERVER2016;    break;
default: version = WINDOWS_FUTURE;           break;
}
break;
default:
version = WINDOWS_FUTURE;
break;
}
}
else
{
PRINT(("WASAPI: getting Windows version with VerifyVersionInfo()\n"));
if (IsWindowsVersionOrGreater(10, 0, 0))
version = WINDOWS_10_SERVER2016;
else
if (IsWindowsVersionOrGreater(6, 3, 0))
version = WINDOWS_8_1_SERVER2012R2;
else
if (IsWindowsVersionOrGreater(6, 2, 0))
version = WINDOWS_8_SERVER2012;
else
if (IsWindowsVersionOrGreater(6, 1, 0))
version = WINDOWS_7_SERVER2008R2;
else
if (IsWindowsVersionOrGreater(6, 0, 0))
version = WINDOWS_VISTA_SERVER2008;
else
version = WINDOWS_FUTURE;
}
PRINT(("WASAPI: Windows version = %d\n", version));
}
return version;
#else
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
return WINDOWS_10_SERVER2016;
#else
return WINDOWS_8_SERVER2012;
#endif
#endif
}
static BOOL UseWOW64Workaround()
{
return (IsWow64() && (GetWindowsVersion() == WINDOWS_VISTA_SERVER2008));
}
static UINT32 GetAudioClientVersion()
{
if (GetWindowsVersion() >= WINDOWS_10_SERVER2016)
return 3;
else
if (GetWindowsVersion() >= WINDOWS_8_SERVER2012)
return 2;
return 1;
}
static const IID *GetAudioClientIID()
{
static const IID *cli_iid = NULL;
if (cli_iid == NULL)
{
UINT32 cli_version = GetAudioClientVersion();
switch (cli_version)
{
case 3:  cli_iid = &pa_IID_IAudioClient3; break;
case 2:  cli_iid = &pa_IID_IAudioClient2; break;
default: cli_iid = &pa_IID_IAudioClient;  break;
}
PRINT(("WASAPI: IAudioClient version = %d\n", cli_version));
}
return cli_iid;
}
typedef enum EMixDirection
{
MIX_DIR__1TO2,    
MIX_DIR__2TO1,    
MIX_DIR__2TO1_L   
}
EMixDirection;
#define _WASAPI_MONO_TO_STEREO_MIXER_1_TO_2(TYPE)\
TYPE * __restrict to = (TYPE *)__to;\
const TYPE * __restrict from = (const TYPE *)__from;\
const TYPE * __restrict end = from + count;\
while (from != end)\
{\
to[0] = to[1] = *from ++;\
to += 2;\
}
#define _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_FLT32(TYPE)\
TYPE * __restrict to = (TYPE *)__to;\
const TYPE * __restrict from = (const TYPE *)__from;\
const TYPE * __restrict end = to + count;\
while (to != end)\
{\
*to ++ = (TYPE)((float)(from[0] + from[1]) * 0.5f);\
from += 2;\
}
#define _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_INT32(TYPE)\
TYPE * __restrict to = (TYPE *)__to;\
const TYPE * __restrict from = (const TYPE *)__from;\
const TYPE * __restrict end = to + count;\
while (to != end)\
{\
*to ++ = (TYPE)(((INT32)from[0] + (INT32)from[1]) >> 1);\
from += 2;\
}
#define _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_INT64(TYPE)\
TYPE * __restrict to = (TYPE *)__to;\
const TYPE * __restrict from = (const TYPE *)__from;\
const TYPE * __restrict end = to + count;\
while (to != end)\
{\
*to ++ = (TYPE)(((INT64)from[0] + (INT64)from[1]) >> 1);\
from += 2;\
}
#define _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_L(TYPE)\
TYPE * __restrict to = (TYPE *)__to;\
const TYPE * __restrict from = (const TYPE *)__from;\
const TYPE * __restrict end = to + count;\
while (to != end)\
{\
*to ++ = from[0];\
from += 2;\
}
static void _MixMonoToStereo_1TO2_8(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_1_TO_2(BYTE); }
static void _MixMonoToStereo_1TO2_16(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_1_TO_2(short); }
static void _MixMonoToStereo_1TO2_8_24(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_1_TO_2(int);   }
static void _MixMonoToStereo_1TO2_32(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_1_TO_2(int); }
static void _MixMonoToStereo_1TO2_32f(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_1_TO_2(float); }
static void _MixMonoToStereo_1TO2_24(void *__to, const void *__from, UINT32 count)
{
const UCHAR * __restrict from = (const UCHAR *)__from;
UCHAR * __restrict to = (UCHAR *)__to;
const UCHAR * __restrict end = to + (count * (2 * 3));
while (to != end)
{
to[0] = to[3] = from[0];
to[1] = to[4] = from[1];
to[2] = to[5] = from[2];
from += 3;
to += (2 * 3);
}
}
static void _MixMonoToStereo_2TO1_8(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_INT32(BYTE); }
static void _MixMonoToStereo_2TO1_16(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_INT32(short); }
static void _MixMonoToStereo_2TO1_8_24(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_INT32(int);   }
static void _MixMonoToStereo_2TO1_32(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_INT64(int); }
static void _MixMonoToStereo_2TO1_32f(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_FLT32(float); }
static void _MixMonoToStereo_2TO1_24(void *__to, const void *__from, UINT32 count)
{
const UCHAR * __restrict from = (const UCHAR *)__from;
UCHAR * __restrict to = (UCHAR *)__to;
const UCHAR * __restrict end = to + (count * 3);
PaInt32 tempL, tempR, tempM;
while (to != end)
{
tempL = (((PaInt32)from[0]) << 8);
tempL = tempL | (((PaInt32)from[1]) << 16);
tempL = tempL | (((PaInt32)from[2]) << 24);
tempR = (((PaInt32)from[3]) << 8);
tempR = tempR | (((PaInt32)from[4]) << 16);
tempR = tempR | (((PaInt32)from[5]) << 24);
tempM = (tempL + tempR) >> 1;
to[0] = (UCHAR)(tempM >> 8);
to[1] = (UCHAR)(tempM >> 16);
to[2] = (UCHAR)(tempM >> 24);
from += (2 * 3);
to += 3;
}
}
static void _MixMonoToStereo_2TO1_8_L(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_L(BYTE); }
static void _MixMonoToStereo_2TO1_16_L(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_L(short); }
static void _MixMonoToStereo_2TO1_8_24_L(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_L(int);   }
static void _MixMonoToStereo_2TO1_32_L(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_L(int); }
static void _MixMonoToStereo_2TO1_32f_L(void *__to, const void *__from, UINT32 count) { _WASAPI_MONO_TO_STEREO_MIXER_2_TO_1_L(float); }
static void _MixMonoToStereo_2TO1_24_L(void *__to, const void *__from, UINT32 count)
{
const UCHAR * __restrict from = (const UCHAR *)__from;
UCHAR * __restrict to = (UCHAR *)__to;
const UCHAR * __restrict end = to + (count * 3);
while (to != end)
{
to[0] = from[0];
to[1] = from[1];
to[2] = from[2];
from += (2 * 3);
to += 3;
}
}
static MixMonoToStereoF GetMonoToStereoMixer(const WAVEFORMATEXTENSIBLE *fmtext, EMixDirection dir)
{
PaSampleFormat format = WaveToPaFormat(fmtext);
switch (dir)
{
case MIX_DIR__1TO2:
switch (format & ~paNonInterleaved)
{
case paUInt8:   return _MixMonoToStereo_1TO2_8;
case paInt16:   return _MixMonoToStereo_1TO2_16;
case paInt24:   return (fmtext->Format.wBitsPerSample == 32 ? _MixMonoToStereo_1TO2_8_24 : _MixMonoToStereo_1TO2_24);
case paInt32:   return _MixMonoToStereo_1TO2_32;
case paFloat32: return _MixMonoToStereo_1TO2_32f;
}
break;
case MIX_DIR__2TO1:
switch (format & ~paNonInterleaved)
{
case paUInt8:   return _MixMonoToStereo_2TO1_8;
case paInt16:   return _MixMonoToStereo_2TO1_16;
case paInt24:   return (fmtext->Format.wBitsPerSample == 32 ? _MixMonoToStereo_2TO1_8_24 : _MixMonoToStereo_2TO1_24);
case paInt32:   return _MixMonoToStereo_2TO1_32;
case paFloat32: return _MixMonoToStereo_2TO1_32f;
}
break;
case MIX_DIR__2TO1_L:
switch (format & ~paNonInterleaved)
{
case paUInt8:   return _MixMonoToStereo_2TO1_8_L;
case paInt16:   return _MixMonoToStereo_2TO1_16_L;
case paInt24:   return (fmtext->Format.wBitsPerSample == 32 ? _MixMonoToStereo_2TO1_8_24_L : _MixMonoToStereo_2TO1_24_L);
case paInt32:   return _MixMonoToStereo_2TO1_32_L;
case paFloat32: return _MixMonoToStereo_2TO1_32f_L;
}
break;
}
return NULL;
}
#ifdef PA_WINRT
typedef struct PaActivateAudioInterfaceCompletionHandler
{
IActivateAudioInterfaceCompletionHandler parent;
volatile LONG refs;
volatile LONG done;
struct
{
const IID *iid;
void **obj;
}
in;
struct
{
HRESULT hr;
}
out;
}
PaActivateAudioInterfaceCompletionHandler;
static HRESULT (STDMETHODCALLTYPE PaActivateAudioInterfaceCompletionHandler_QueryInterface)(
IActivateAudioInterfaceCompletionHandler *This, REFIID riid, void **ppvObject)
{
PaActivateAudioInterfaceCompletionHandler *handler = (PaActivateAudioInterfaceCompletionHandler *)This;
if (IsEqualIID(riid, &IID_IUnknown) ||
IsEqualIID(riid, &IID_IAgileObject))
{
IActivateAudioInterfaceCompletionHandler_AddRef((IActivateAudioInterfaceCompletionHandler *)handler);
(*ppvObject) = handler;
return S_OK;
}
return E_NOINTERFACE;
}
static ULONG (STDMETHODCALLTYPE PaActivateAudioInterfaceCompletionHandler_AddRef)(
IActivateAudioInterfaceCompletionHandler *This)
{
PaActivateAudioInterfaceCompletionHandler *handler = (PaActivateAudioInterfaceCompletionHandler *)This;
return InterlockedIncrement(&handler->refs);
}
static ULONG (STDMETHODCALLTYPE PaActivateAudioInterfaceCompletionHandler_Release)(
IActivateAudioInterfaceCompletionHandler *This)
{
PaActivateAudioInterfaceCompletionHandler *handler = (PaActivateAudioInterfaceCompletionHandler *)This;
ULONG refs;
if ((refs = InterlockedDecrement(&handler->refs)) == 0)
{
PaUtil_FreeMemory(handler->parent.lpVtbl);
PaUtil_FreeMemory(handler);
}
return refs;
}
static HRESULT (STDMETHODCALLTYPE PaActivateAudioInterfaceCompletionHandler_ActivateCompleted)(
IActivateAudioInterfaceCompletionHandler *This, IActivateAudioInterfaceAsyncOperation *activateOperation)
{
PaActivateAudioInterfaceCompletionHandler *handler = (PaActivateAudioInterfaceCompletionHandler *)This;
HRESULT hr = S_OK;
HRESULT hrActivateResult = S_OK;
IUnknown *punkAudioInterface = NULL;
hr = IActivateAudioInterfaceAsyncOperation_GetActivateResult(activateOperation, &hrActivateResult, &punkAudioInterface);
if (SUCCEEDED(hr) && SUCCEEDED(hrActivateResult))
{
IUnknown_QueryInterface(punkAudioInterface, handler->in.iid, handler->in.obj);
if ((*handler->in.obj) == NULL)
hrActivateResult = E_FAIL;
}
SAFE_RELEASE(punkAudioInterface);
if (SUCCEEDED(hr))
handler->out.hr = hrActivateResult;
else
handler->out.hr = hr;
InterlockedExchange(&handler->done, TRUE);
return hr;
}
static IActivateAudioInterfaceCompletionHandler *CreateActivateAudioInterfaceCompletionHandler(const IID *iid, void **client)
{
PaActivateAudioInterfaceCompletionHandler *handler = PaUtil_AllocateMemory(sizeof(PaActivateAudioInterfaceCompletionHandler));
memset(handler, 0, sizeof(*handler));
handler->parent.lpVtbl = PaUtil_AllocateMemory(sizeof(*handler->parent.lpVtbl));
handler->parent.lpVtbl->QueryInterface    = &PaActivateAudioInterfaceCompletionHandler_QueryInterface;
handler->parent.lpVtbl->AddRef            = &PaActivateAudioInterfaceCompletionHandler_AddRef;
handler->parent.lpVtbl->Release           = &PaActivateAudioInterfaceCompletionHandler_Release;
handler->parent.lpVtbl->ActivateCompleted = &PaActivateAudioInterfaceCompletionHandler_ActivateCompleted;
handler->refs   = 1;
handler->in.iid = iid;
handler->in.obj = client;
return (IActivateAudioInterfaceCompletionHandler *)handler;
}
#endif
#ifdef PA_WINRT
static HRESULT WinRT_GetDefaultDeviceId(WCHAR *deviceId, UINT32 deviceIdMax, EDataFlow flow)
{
switch (flow)
{
case eRender:
if (g_DeviceListInfo.render.defaultId[0] != 0)
wcsncpy_s(deviceId, deviceIdMax, g_DeviceListInfo.render.defaultId, wcslen(g_DeviceListInfo.render.defaultId));
else
StringFromGUID2(&DEVINTERFACE_AUDIO_RENDER, deviceId, deviceIdMax);
break;
case eCapture:
if (g_DeviceListInfo.capture.defaultId[0] != 0)
wcsncpy_s(deviceId, deviceIdMax, g_DeviceListInfo.capture.defaultId, wcslen(g_DeviceListInfo.capture.defaultId));
else
StringFromGUID2(&DEVINTERFACE_AUDIO_CAPTURE, deviceId, deviceIdMax);
break;
default:
return S_FALSE;
}
return S_OK;
}
#endif
#ifdef PA_WINRT
static HRESULT WinRT_ActivateAudioInterface(const WCHAR *deviceId, const IID *iid, void **client)
{
PaError result = paNoError;
HRESULT hr = S_OK;
IActivateAudioInterfaceAsyncOperation *asyncOp = NULL;
IActivateAudioInterfaceCompletionHandler *handler = CreateActivateAudioInterfaceCompletionHandler(iid, client);
PaActivateAudioInterfaceCompletionHandler *handlerImpl = (PaActivateAudioInterfaceCompletionHandler *)handler;
UINT32 sleepToggle = 0;
hr = ActivateAudioInterfaceAsync(deviceId, iid, NULL, handler, &asyncOp);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
while (SUCCEEDED(hr) && !InterlockedOr(&handlerImpl->done, 0))
{
Sleep(sleepToggle ^= 1);
}
hr = handlerImpl->out.hr;
error:
SAFE_RELEASE(asyncOp);
SAFE_RELEASE(handler);
return hr;
}
#endif
static HRESULT ActivateAudioInterface(const PaWasapiDeviceInfo *deviceInfo, const PaWasapiStreamInfo *streamInfo,
IAudioClient **client)
{
HRESULT hr;
#ifndef PA_WINRT
if (FAILED(hr = IMMDevice_Activate(deviceInfo->device, GetAudioClientIID(), CLSCTX_ALL, NULL, (void **)client)))
return hr;
#else
if (FAILED(hr = WinRT_ActivateAudioInterface(deviceInfo->deviceId, GetAudioClientIID(), (void **)client)))
return hr;
#endif
#ifdef __IAudioClient2_INTERFACE_DEFINED__
if ((streamInfo != NULL) && (GetAudioClientVersion() >= 2))
{
pa_AudioClientProperties audioProps = { 0 };
audioProps.cbSize     = sizeof(pa_AudioClientProperties);
audioProps.bIsOffload = FALSE;
audioProps.eCategory  = (AUDIO_STREAM_CATEGORY)streamInfo->streamCategory;
switch (streamInfo->streamOption)
{
case eStreamOptionRaw:
if (GetWindowsVersion() >= WINDOWS_8_1_SERVER2012R2)
audioProps.Options = pa_AUDCLNT_STREAMOPTIONS_RAW;
break;
case eStreamOptionMatchFormat:
if (GetWindowsVersion() >= WINDOWS_10_SERVER2016)
audioProps.Options = pa_AUDCLNT_STREAMOPTIONS_MATCH_FORMAT;
break;
}
if (FAILED(hr = IAudioClient2_SetClientProperties((IAudioClient2 *)(*client), (AudioClientProperties *)&audioProps)))
{
PRINT(("WASAPI: IAudioClient2_SetClientProperties(IsOffload = %d, Category = %d, Options = %d) failed\n", audioProps.bIsOffload, audioProps.eCategory, audioProps.Options));
LogHostError(hr);
}
else
{
PRINT(("WASAPI: IAudioClient2 set properties: IsOffload = %d, Category = %d, Options = %d\n", audioProps.bIsOffload, audioProps.eCategory, audioProps.Options));
}
}
#endif
return S_OK;
}
#ifdef PA_WINRT
#if !defined(WDK_NTDDI_VERSION) || (WDK_NTDDI_VERSION < NTDDI_WIN10_RS2)
static DWORD SignalObjectAndWait(HANDLE hObjectToSignal, HANDLE hObjectToWaitOn, DWORD dwMilliseconds, BOOL bAlertable)
{
SetEvent(hObjectToSignal);
return WaitForSingleObjectEx(hObjectToWaitOn, dwMilliseconds, bAlertable);
}
#endif
#endif
static void NotifyStateChanged(PaWasapiStream *stream, UINT32 flags, HRESULT hr)
{
if (stream->fnStateHandler == NULL)
return;
if (FAILED(hr))
flags |= paWasapiStreamStateError;
stream->fnStateHandler((PaStream *)stream, flags, hr, stream->pStateHandlerUserData);
}
static void FillBaseDeviceInfo(PaDeviceInfo *deviceInfo, PaHostApiIndex hostApiIndex)
{
deviceInfo->structVersion = 2;
deviceInfo->hostApi       = hostApiIndex;
}
static PaError FillInactiveDeviceInfo(PaWasapiHostApiRepresentation *paWasapi, PaDeviceInfo *deviceInfo)
{
if (deviceInfo->name == NULL)
deviceInfo->name = (char *)PaUtil_GroupAllocateMemory(paWasapi->allocations, 1);
if (deviceInfo->name != NULL)
{
((char *)deviceInfo->name)[0] = 0;
}
else
return paInsufficientMemory;
return paNoError;
}
static PaError FillDeviceInfo(PaWasapiHostApiRepresentation *paWasapi, void *pEndPoints, INT32 index, const WCHAR *defaultRenderId,
const WCHAR *defaultCaptureId, PaDeviceInfo *deviceInfo, PaWasapiDeviceInfo *wasapiDeviceInfo
#ifdef PA_WINRT
, PaWasapiWinrtDeviceListContext *deviceListContext
#endif
)
{
HRESULT hr;
PaError result;
PaUtilHostApiRepresentation *hostApi = (PaUtilHostApiRepresentation *)paWasapi;
#ifdef PA_WINRT
PaWasapiWinrtDeviceListContextEntry *listEntry = &deviceListContext->devices[index];
(void)pEndPoints;
(void)defaultRenderId;
(void)defaultCaptureId;
#endif
#ifndef PA_WINRT
hr = IMMDeviceCollection_Item((IMMDeviceCollection *)pEndPoints, index, &wasapiDeviceInfo->device);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
{
WCHAR *deviceId;
hr = IMMDevice_GetId(wasapiDeviceInfo->device, &deviceId);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
wcsncpy(wasapiDeviceInfo->deviceId, deviceId, PA_WASAPI_DEVICE_ID_LEN - 1);
CoTaskMemFree(deviceId);
}
hr = IMMDevice_GetState(wasapiDeviceInfo->device, &wasapiDeviceInfo->state);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
if (wasapiDeviceInfo->state != DEVICE_STATE_ACTIVE)
{
PRINT(("WASAPI device: %d is not currently available (state:%d)\n", index, wasapiDeviceInfo->state));
}
{
IPropertyStore *pProperty;
IMMEndpoint *endpoint;
PROPVARIANT value;
hr = IMMDevice_OpenPropertyStore(wasapiDeviceInfo->device, STGM_READ, &pProperty);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
{
PropVariantInit(&value);
hr = IPropertyStore_GetValue(pProperty, &PKEY_Device_FriendlyName, &value);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
if ((deviceInfo->name = (char *)PaUtil_GroupAllocateMemory(paWasapi->allocations, PA_WASAPI_DEVICE_NAME_LEN)) == NULL)
{
result = paInsufficientMemory;
PropVariantClear(&value);
goto error;
}
if (value.pwszVal)
WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, (INT32)wcslen(value.pwszVal), (char *)deviceInfo->name, PA_WASAPI_DEVICE_NAME_LEN - 1, 0, 0);
else
_snprintf((char *)deviceInfo->name, PA_WASAPI_DEVICE_NAME_LEN - 1, "baddev%d", index);
PropVariantClear(&value);
PA_DEBUG(("WASAPI:%d| name[%s]\n", index, deviceInfo->name));
}
{
PropVariantInit(&value);
hr = IPropertyStore_GetValue(pProperty, &PKEY_AudioEngine_DeviceFormat, &value);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
memcpy(&wasapiDeviceInfo->DefaultFormat, value.blob.pBlobData, min(sizeof(wasapiDeviceInfo->DefaultFormat), value.blob.cbSize));
PropVariantClear(&value);
}
{
PropVariantInit(&value);
hr = IPropertyStore_GetValue(pProperty, &PKEY_AudioEndpoint_FormFactor, &value);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
#if defined(DUMMYUNIONNAME) && defined(NONAMELESSUNION)
UINT v;
memcpy(&v, (((WORD *)&value.wReserved3) + 1), sizeof(v));
wasapiDeviceInfo->formFactor = (EndpointFormFactor)v;
#else
wasapiDeviceInfo->formFactor = (EndpointFormFactor)value.uintVal;
#endif
PA_DEBUG(("WASAPI:%d| form-factor[%d]\n", index, wasapiDeviceInfo->formFactor));
PropVariantClear(&value);
}
hr = IMMDevice_QueryInterface(wasapiDeviceInfo->device, &pa_IID_IMMEndpoint, (void **)&endpoint);
if (SUCCEEDED(hr))
{
hr = IMMEndpoint_GetDataFlow(endpoint, &wasapiDeviceInfo->flow);
SAFE_RELEASE(endpoint);
}
SAFE_RELEASE(pProperty);
}
#else
wcsncpy(wasapiDeviceInfo->deviceId, listEntry->info->id, PA_WASAPI_DEVICE_ID_LEN - 1);
if ((deviceInfo->name = (char *)PaUtil_GroupAllocateMemory(paWasapi->allocations, PA_WASAPI_DEVICE_NAME_LEN)) == NULL)
{
result = paInsufficientMemory;
goto error;
}
((char *)deviceInfo->name)[0] = 0;
if (listEntry->info->name[0] != 0)
WideCharToMultiByte(CP_UTF8, 0, listEntry->info->name, (INT32)wcslen(listEntry->info->name), (char *)deviceInfo->name, PA_WASAPI_DEVICE_NAME_LEN - 1, 0, 0);
if (deviceInfo->name[0] == 0)  
_snprintf((char *)deviceInfo->name, PA_WASAPI_DEVICE_NAME_LEN - 1, "WASAPI_%s:%d", (listEntry->flow == eRender ? "Output" : "Input"), index);
wasapiDeviceInfo->formFactor = listEntry->info->formFactor;
wasapiDeviceInfo->flow = listEntry->flow;
#endif
if ((defaultRenderId != NULL) && (wcsncmp(wasapiDeviceInfo->deviceId, defaultRenderId, PA_WASAPI_DEVICE_NAME_LEN - 1) == 0))
hostApi->info.defaultOutputDevice = hostApi->info.deviceCount;
if ((defaultCaptureId != NULL) && (wcsncmp(wasapiDeviceInfo->deviceId, defaultCaptureId, PA_WASAPI_DEVICE_NAME_LEN - 1) == 0))
hostApi->info.defaultInputDevice = hostApi->info.deviceCount;
{
IAudioClient *tmpClient;
WAVEFORMATEX *mixFormat;
hr = ActivateAudioInterface(wasapiDeviceInfo, NULL, &tmpClient);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
hr = IAudioClient_GetDevicePeriod(tmpClient, &wasapiDeviceInfo->DefaultDevicePeriod, &wasapiDeviceInfo->MinimumDevicePeriod);
if (FAILED(hr))
{
PA_DEBUG(("WASAPI:%d| failed getting min/default periods by IAudioClient::GetDevicePeriod() with error[%08X], will use 30000/100000 hns\n", index, (UINT32)hr));
wasapiDeviceInfo->DefaultDevicePeriod = 100000;
wasapiDeviceInfo->MinimumDevicePeriod = 30000;
hr = S_OK;
}
hr = IAudioClient_GetMixFormat(tmpClient, &mixFormat);
if (SUCCEEDED(hr))
{
memcpy(&wasapiDeviceInfo->MixFormat, mixFormat, min(sizeof(wasapiDeviceInfo->MixFormat), (sizeof(*mixFormat) + mixFormat->cbSize)));
CoTaskMemFree(mixFormat);
}
#ifdef PA_WINRT
if (SUCCEEDED(hr))
{
wasapiDeviceInfo->state = DEVICE_STATE_ACTIVE;
wasapiDeviceInfo->DefaultFormat = wasapiDeviceInfo->MixFormat;
}
#endif
SAFE_RELEASE(tmpClient);
if (hr != S_OK)
{
LogHostError(hr);
result = paInternalError;
goto error;
}
}
deviceInfo->maxInputChannels = 0;
deviceInfo->maxOutputChannels = 0;
deviceInfo->defaultSampleRate = wasapiDeviceInfo->MixFormat.Format.nSamplesPerSec;
switch (wasapiDeviceInfo->flow)
{
case eRender: {
deviceInfo->maxOutputChannels         = wasapiDeviceInfo->MixFormat.Format.nChannels;
deviceInfo->defaultHighOutputLatency = nano100ToSeconds(wasapiDeviceInfo->DefaultDevicePeriod);
deviceInfo->defaultLowOutputLatency  = nano100ToSeconds(wasapiDeviceInfo->MinimumDevicePeriod);
PA_DEBUG(("WASAPI:%d| def.SR[%d] max.CH[%d] latency{hi[%f] lo[%f]}\n", index, (UINT32)deviceInfo->defaultSampleRate,
deviceInfo->maxOutputChannels, (float)deviceInfo->defaultHighOutputLatency, (float)deviceInfo->defaultLowOutputLatency));
break;}
case eCapture: {
deviceInfo->maxInputChannels        = wasapiDeviceInfo->MixFormat.Format.nChannels;
deviceInfo->defaultHighInputLatency = nano100ToSeconds(wasapiDeviceInfo->DefaultDevicePeriod);
deviceInfo->defaultLowInputLatency  = nano100ToSeconds(wasapiDeviceInfo->MinimumDevicePeriod);
PA_DEBUG(("WASAPI:%d| def.SR[%d] max.CH[%d] latency{hi[%f] lo[%f]}\n", index, (UINT32)deviceInfo->defaultSampleRate,
deviceInfo->maxInputChannels, (float)deviceInfo->defaultHighInputLatency, (float)deviceInfo->defaultLowInputLatency));
break; }
default:
PRINT(("WASAPI:%d| bad Data Flow!\n", index));
result = paInternalError;
goto error;
}
return paNoError;
error:
PRINT(("WASAPI: failed filling device info for device index[%d] - error[%d|%s]\n", index, result, Pa_GetErrorText(result)));
return result;
}
static PaDeviceInfo *AllocateDeviceListMemory(PaWasapiHostApiRepresentation *paWasapi)
{
PaUtilHostApiRepresentation *hostApi = (PaUtilHostApiRepresentation *)paWasapi;
PaDeviceInfo *deviceInfoArray = NULL;
if ((paWasapi->devInfo = (PaWasapiDeviceInfo *)PaUtil_GroupAllocateMemory(paWasapi->allocations,
sizeof(PaWasapiDeviceInfo) * paWasapi->deviceCount)) == NULL)
{
return NULL;
}
memset(paWasapi->devInfo, 0, sizeof(PaWasapiDeviceInfo) * paWasapi->deviceCount);
if (paWasapi->deviceCount != 0)
{
UINT32 i;
UINT32 deviceCount = paWasapi->deviceCount;
#if defined(PA_WASAPI_MAX_CONST_DEVICE_COUNT) && (PA_WASAPI_MAX_CONST_DEVICE_COUNT > 0)
if (deviceCount < PA_WASAPI_MAX_CONST_DEVICE_COUNT)
deviceCount = PA_WASAPI_MAX_CONST_DEVICE_COUNT;
#endif
if ((hostApi->deviceInfos = (PaDeviceInfo **)PaUtil_GroupAllocateMemory(paWasapi->allocations,
sizeof(PaDeviceInfo *) * deviceCount)) == NULL)
{
return NULL;
}
for (i = 0; i < deviceCount; ++i)
hostApi->deviceInfos[i] = NULL;
if ((deviceInfoArray = (PaDeviceInfo *)PaUtil_GroupAllocateMemory(paWasapi->allocations,
sizeof(PaDeviceInfo) * deviceCount)) == NULL)
{
return NULL;
}
memset(deviceInfoArray, 0, sizeof(PaDeviceInfo) * deviceCount);
}
return deviceInfoArray;
}
static PaError CreateDeviceList(PaWasapiHostApiRepresentation *paWasapi, PaHostApiIndex hostApiIndex)
{
PaUtilHostApiRepresentation *hostApi = (PaUtilHostApiRepresentation *)paWasapi;
PaError result = paNoError;
PaDeviceInfo *deviceInfoArray = NULL;
UINT32 i;
WCHAR *defaultRenderId = NULL;
WCHAR *defaultCaptureId = NULL;
#ifndef PA_WINRT
HRESULT hr;
IMMDeviceCollection *pEndPoints = NULL;
IMMDeviceEnumerator *pEnumerator = NULL;
#else
void *pEndPoints = NULL;
IAudioClient *tmpClient;
PaWasapiWinrtDeviceListContext deviceListContext = { 0 };
PaWasapiWinrtDeviceInfo defaultRender = { 0 };
PaWasapiWinrtDeviceInfo defaultCapture = { 0 };
#endif
if ((paWasapi->deviceCount != 0) || (hostApi->info.deviceCount != 0))
return paInternalError;
#ifndef PA_WINRT
hr = CoCreateInstance(&pa_CLSID_IMMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
&pa_IID_IMMDeviceEnumerator, (void **)&pEnumerator);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
{
IMMDevice *device;
hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(pEnumerator, eRender, eMultimedia, &device);
if (hr != S_OK)
{
if (hr != E_NOTFOUND)
{
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
}
}
else
{
hr = IMMDevice_GetId(device, &defaultRenderId);
IMMDevice_Release(device);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
}
hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(pEnumerator, eCapture, eMultimedia, &device);
if (hr != S_OK)
{
if (hr != E_NOTFOUND)
{
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
}
}
else
{
hr = IMMDevice_GetId(device, &defaultCaptureId);
IMMDevice_Release(device);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
}
}
hr = IMMDeviceEnumerator_EnumAudioEndpoints(pEnumerator, eAll, DEVICE_STATE_ACTIVE, &pEndPoints);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
hr = IMMDeviceCollection_GetCount(pEndPoints, &paWasapi->deviceCount);
IF_FAILED_INTERNAL_ERROR_JUMP(hr, result, error);
#else
WinRT_GetDefaultDeviceId(defaultRender.id, STATIC_ARRAY_SIZE(defaultRender.id) - 1, eRender);
defaultRenderId = defaultRender.id;
WinRT_GetDefaultDeviceId(defaultCapture.id, STATIC_ARRAY_SIZE(defaultCapture.id) - 1, eCapture);
defaultCaptureId = defaultCapture.id;
if (g_DeviceListInfo.render.deviceCount == 0)
{
if (SUCCEEDED(WinRT_ActivateAudioInterface(defaultRenderId, GetAudioClientIID(), &tmpClient)))
{
deviceListContext.devices[paWasapi->deviceCount].info = &defaultRender;
deviceListContext.devices[paWasapi->deviceCount].flow = eRender;
paWasapi->deviceCount++;
SAFE_RELEASE(tmpClient);
}
}
else
{
for (i = 0; i < g_DeviceListInfo.render.deviceCount; ++i)
{
deviceListContext.devices[paWasapi->deviceCount].info = &g_DeviceListInfo.render.devices[i];
deviceListContext.devices[paWasapi->deviceCount].flow = eRender;
paWasapi->deviceCount++;
}
}
if (g_DeviceListInfo.capture.deviceCount == 0)
{
if (SUCCEEDED(WinRT_ActivateAudioInterface(defaultCaptureId, GetAudioClientIID(), &tmpClient)))
{
deviceListContext.devices[paWasapi->deviceCount].info = &defaultCapture;
deviceListContext.devices[paWasapi->deviceCount].flow = eCapture;
paWasapi->deviceCount++;
SAFE_RELEASE(tmpClient);
}
}
else
{
for (i = 0; i < g_DeviceListInfo.capture.deviceCount; ++i)
{
deviceListContext.devices[paWasapi->deviceCount].info = &g_DeviceListInfo.capture.devices[i];
deviceListContext.devices[paWasapi->deviceCount].flow = eCapture;
paWasapi->deviceCount++;
}
}
#endif
if ((paWasapi->deviceCount != 0) && ((deviceInfoArray = AllocateDeviceListMemory(paWasapi)) == NULL))
{
result = paInsufficientMemory;
goto error;
}
for (i = 0; i < paWasapi->deviceCount; ++i)
{
PaDeviceInfo *deviceInfo = &deviceInfoArray[i];
PA_DEBUG(("WASAPI: device idx: %02d\n", i));
PA_DEBUG(("WASAPI: ---------------\n"));
FillBaseDeviceInfo(deviceInfo, hostApiIndex);
if ((result = FillDeviceInfo(paWasapi, pEndPoints, i, defaultRenderId, defaultCaptureId,
deviceInfo, &paWasapi->devInfo[i]
#ifdef PA_WINRT
, &deviceListContext
#endif
)) != paNoError)
{
if ((result = FillInactiveDeviceInfo(paWasapi, deviceInfo)) != paNoError)
goto error;
}
hostApi->deviceInfos[i] = deviceInfo;
++hostApi->info.deviceCount;
}
#if defined(PA_WASAPI_MAX_CONST_DEVICE_COUNT) && (PA_WASAPI_MAX_CONST_DEVICE_COUNT > 0)
if ((hostApi->info.deviceCount != 0) && (hostApi->info.deviceCount < PA_WASAPI_MAX_CONST_DEVICE_COUNT))
{
for (i = hostApi->info.deviceCount; i < PA_WASAPI_MAX_CONST_DEVICE_COUNT; ++i)
{
PaDeviceInfo *deviceInfo = &deviceInfoArray[i];
FillBaseDeviceInfo(deviceInfo, hostApiIndex);
if ((result = FillInactiveDeviceInfo(paWasapi, deviceInfo)) != paNoError)
goto error;
hostApi->deviceInfos[i] = deviceInfo;
++hostApi->info.deviceCount;
}
}
#endif
result = paNoError;
PRINT(("WASAPI: device list ok - found %d devices\n", paWasapi->deviceCount));
done:
#ifndef PA_WINRT
CoTaskMemFree(defaultRenderId);
CoTaskMemFree(defaultCaptureId);
SAFE_RELEASE(pEndPoints);
SAFE_RELEASE(pEnumerator);
#endif
return result;
error:
if (result == paNoError)
result = paInternalError;
PRINT(("WASAPI: failed to create device list - error[%d|%s]\n", result, Pa_GetErrorText(result)));
goto done;
}
PaError PaWasapi_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex )
{
PaError result;
PaWasapiHostApiRepresentation *paWasapi;
#ifndef PA_WINRT
if (!SetupAVRT())
{
PRINT(("WASAPI: No AVRT! (not VISTA?)\n"));
return paNoError;
}
#endif
paWasapi = (PaWasapiHostApiRepresentation *)PaUtil_AllocateMemory(sizeof(PaWasapiHostApiRepresentation));
if (paWasapi == NULL)
{
result = paInsufficientMemory;
goto error;
}
memset(paWasapi, 0, sizeof(PaWasapiHostApiRepresentation));  
result = PaWinUtil_CoInitialize(paWASAPI, &paWasapi->comInitializationResult);
if (result != paNoError)
goto error;
paWasapi->allocations = PaUtil_CreateAllocationGroup();
if (paWasapi->allocations == NULL)
{
result = paInsufficientMemory;
goto error;
}
*hostApi                             = &paWasapi->inheritedHostApiRep;
(*hostApi)->info.structVersion         = 1;
(*hostApi)->info.type                 = paWASAPI;
(*hostApi)->info.name                 = "Windows WASAPI";
(*hostApi)->info.deviceCount         = 0;
(*hostApi)->info.defaultInputDevice     = paNoDevice;
(*hostApi)->info.defaultOutputDevice = paNoDevice;
(*hostApi)->Terminate                = Terminate;
(*hostApi)->OpenStream               = OpenStream;
(*hostApi)->IsFormatSupported        = IsFormatSupported;
if ((result = CreateDeviceList(paWasapi, hostApiIndex)) != paNoError)
goto error;
paWasapi->useWOW64Workaround = UseWOW64Workaround();
SystemTimer_InitializeTimeGetter();
PaUtil_InitializeStreamInterface( &paWasapi->callbackStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, GetStreamCpuLoad,
PaUtil_DummyRead, PaUtil_DummyWrite,
PaUtil_DummyGetReadAvailable, PaUtil_DummyGetWriteAvailable );
PaUtil_InitializeStreamInterface( &paWasapi->blockingStreamInterface, CloseStream, StartStream,
StopStream, AbortStream, IsStreamStopped, IsStreamActive,
GetStreamTime, PaUtil_DummyGetCpuLoad,
ReadStream, WriteStream, GetStreamReadAvailable, GetStreamWriteAvailable );
PRINT(("WASAPI: initialized ok\n"));
return paNoError;
error:
PRINT(("WASAPI: failed %s error[%d|%s]\n", __FUNCTION__, result, Pa_GetErrorText(result)));
Terminate((PaUtilHostApiRepresentation *)paWasapi);
return result;
}
static void ReleaseWasapiDeviceInfoList( PaWasapiHostApiRepresentation *paWasapi )
{
UINT32 i;
for (i = 0; i < paWasapi->deviceCount; ++i)
{
#ifndef PA_WINRT
SAFE_RELEASE(paWasapi->devInfo[i].device);
#endif
}
if (paWasapi->allocations != NULL)
PaUtil_GroupFreeMemory(paWasapi->allocations, paWasapi->devInfo);
paWasapi->devInfo = NULL;
paWasapi->deviceCount = 0;
}
static void Terminate( PaUtilHostApiRepresentation *hostApi )
{
PaWasapiHostApiRepresentation *paWasapi = (PaWasapiHostApiRepresentation*)hostApi;
if (paWasapi == NULL)
return;
ReleaseWasapiDeviceInfoList(paWasapi);
if (paWasapi->allocations != NULL)
{
PaUtil_FreeAllAllocations(paWasapi->allocations);
PaUtil_DestroyAllocationGroup(paWasapi->allocations);
}
PaWinUtil_CoUninitialize(paWASAPI, &paWasapi->comInitializationResult);
PaUtil_FreeMemory(paWasapi);
CloseAVRT();
}
static PaWasapiHostApiRepresentation *_GetHostApi(PaError *ret)
{
PaError error;
PaUtilHostApiRepresentation *pApi;
if ((error = PaUtil_GetHostApiRepresentation(&pApi, paWASAPI)) != paNoError)
{
if (ret != NULL)
(*ret) = error;
return NULL;
}
return (PaWasapiHostApiRepresentation *)pApi;
}
static PaError UpdateDeviceList()
{
int i;
PaError ret;
PaWasapiHostApiRepresentation *paWasapi;
PaUtilHostApiRepresentation *hostApi;
hostApi = (PaUtilHostApiRepresentation *)(paWasapi = _GetHostApi(&ret));
if (paWasapi == NULL)
return paNotInitialized;
if (paWasapi->allocations == NULL)
return paNotInitialized;
ReleaseWasapiDeviceInfoList(paWasapi);
if (hostApi->deviceInfos != NULL)
{
for (i = 0; i < hostApi->info.deviceCount; ++i)
{
PaUtil_GroupFreeMemory(paWasapi->allocations, (void *)hostApi->deviceInfos[i]->name);
}
PaUtil_GroupFreeMemory(paWasapi->allocations, hostApi->deviceInfos[0]);
PaUtil_GroupFreeMemory(paWasapi->allocations, hostApi->deviceInfos);
hostApi->deviceInfos = NULL;
hostApi->info.deviceCount = 0;
hostApi->info.defaultInputDevice = paNoDevice;
hostApi->info.defaultOutputDevice = paNoDevice;
}
if ((ret = CreateDeviceList(paWasapi, Pa_HostApiTypeIdToHostApiIndex(paWASAPI))) != paNoError)
return ret;
return paNoError;
}
PaError PaWasapi_UpdateDeviceList()
{
#if defined(PA_WASAPI_MAX_CONST_DEVICE_COUNT) && (PA_WASAPI_MAX_CONST_DEVICE_COUNT > 0)
return UpdateDeviceList();
#else
return paInternalError;
#endif
}
int PaWasapi_GetDeviceCurrentFormat( PaStream *pStream, void *pFormat, unsigned int formatSize, int bOutput )
{
UINT32 size;
WAVEFORMATEXTENSIBLE *format;
PaWasapiStream *stream = (PaWasapiStream *)pStream;
if (stream == NULL)
return paBadStreamPtr;
format = (bOutput == TRUE ? &stream->out.wavex : &stream->in.wavex);
size = min(formatSize, (UINT32)sizeof(*format));
memcpy(pFormat, format, size);
return size;
}
static PaError _GetWasapiDeviceInfoByDeviceIndex( PaWasapiDeviceInfo **info, PaDeviceIndex device )
{
PaError ret;
PaDeviceIndex index;
PaWasapiHostApiRepresentation *paWasapi = _GetHostApi(&ret);
if (paWasapi == NULL)
return paNotInitialized;
if ((ret = PaUtil_DeviceIndexToHostApiDeviceIndex(&index, device, &paWasapi->inheritedHostApiRep)) != paNoError)
return ret;
if ((UINT32)index >= paWasapi->deviceCount)
return paInvalidDevice;
(*info) = &paWasapi->devInfo[ index ];
return paNoError;
}
int PaWasapi_GetDeviceDefaultFormat( void *pFormat, unsigned int formatSize, PaDeviceIndex device )
{
PaError ret;
PaWasapiDeviceInfo *deviceInfo;
UINT32 size;
if (pFormat == NULL)
return paBadBufferPtr;
if (formatSize <= 0)
return paBufferTooSmall;
if ((ret = _GetWasapiDeviceInfoByDeviceIndex(&deviceInfo, device)) != paNoError)
return ret;
size = min(formatSize, (UINT32)sizeof(deviceInfo->DefaultFormat));
memcpy(pFormat, &deviceInfo->DefaultFormat, size);
return size;
}
int PaWasapi_GetDeviceMixFormat( void *pFormat, unsigned int formatSize, PaDeviceIndex device )
{
PaError ret;
PaWasapiDeviceInfo *deviceInfo;
UINT32 size;
if (pFormat == NULL)
return paBadBufferPtr;
if (formatSize <= 0)
return paBufferTooSmall;
if ((ret = _GetWasapiDeviceInfoByDeviceIndex(&deviceInfo, device)) != paNoError)
return ret;
size = min(formatSize, (UINT32)sizeof(deviceInfo->MixFormat));
memcpy(pFormat, &deviceInfo->MixFormat, size);
return size;
}
int PaWasapi_GetDeviceRole( PaDeviceIndex device )
{
PaError ret;
PaWasapiDeviceInfo *deviceInfo;
if ((ret = _GetWasapiDeviceInfoByDeviceIndex(&deviceInfo, device)) != paNoError)
return ret;
return deviceInfo->formFactor;
}
PaError PaWasapi_GetIMMDevice( PaDeviceIndex device, void **pIMMDevice )
{
#ifndef PA_WINRT
PaError ret;
PaWasapiDeviceInfo *deviceInfo;
if (pIMMDevice == NULL)
return paBadBufferPtr;
if ((ret = _GetWasapiDeviceInfoByDeviceIndex(&deviceInfo, device)) != paNoError)
return ret;
(*pIMMDevice) = deviceInfo->device;
return paNoError;
#else
(void)device;
(void)pIMMDevice;
return paIncompatibleStreamHostApi;
#endif
}
PaError PaWasapi_GetFramesPerHostBuffer( PaStream *pStream, unsigned int *pInput, unsigned int *pOutput )
{
PaWasapiStream *stream = (PaWasapiStream *)pStream;
if (stream == NULL)
return paBadStreamPtr;
if (pInput != NULL)
(*pInput) = stream->in.framesPerHostCallback;
if (pOutput != NULL)
(*pOutput) = stream->out.framesPerHostCallback;
return paNoError;
}
static void LogWAVEFORMATEXTENSIBLE(const WAVEFORMATEXTENSIBLE *in)
{
const WAVEFORMATEX *old = (WAVEFORMATEX *)in;
switch (old->wFormatTag)
{
case WAVE_FORMAT_EXTENSIBLE: {
PRINT(("wFormatTag     =WAVE_FORMAT_EXTENSIBLE\n"));
if (IsEqualGUID(&in->SubFormat, &pa_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
{
PRINT(("SubFormat      =KSDATAFORMAT_SUBTYPE_IEEE_FLOAT\n"));
}
else
if (IsEqualGUID(&in->SubFormat, &pa_KSDATAFORMAT_SUBTYPE_PCM))
{
PRINT(("SubFormat      =KSDATAFORMAT_SUBTYPE_PCM\n"));
}
else
{
PRINT(("SubFormat      =CUSTOM GUID{%d:%d:%d:%d%d%d%d%d%d%d%d}\n",
in->SubFormat.Data1,
in->SubFormat.Data2,
in->SubFormat.Data3,
(int)in->SubFormat.Data4[0],
(int)in->SubFormat.Data4[1],
(int)in->SubFormat.Data4[2],
(int)in->SubFormat.Data4[3],
(int)in->SubFormat.Data4[4],
(int)in->SubFormat.Data4[5],
(int)in->SubFormat.Data4[6],
(int)in->SubFormat.Data4[7]));
}
PRINT(("Samples.wValidBitsPerSample =%d\n",  in->Samples.wValidBitsPerSample));
PRINT(("dwChannelMask  =0x%X\n",in->dwChannelMask));
break; }
case WAVE_FORMAT_PCM:        PRINT(("wFormatTag     =WAVE_FORMAT_PCM\n")); break;
case WAVE_FORMAT_IEEE_FLOAT: PRINT(("wFormatTag     =WAVE_FORMAT_IEEE_FLOAT\n")); break;
default:
PRINT(("wFormatTag     =UNKNOWN(%d)\n",old->wFormatTag)); break;
}
PRINT(("nChannels      =%d\n",old->nChannels));
PRINT(("nSamplesPerSec =%d\n",old->nSamplesPerSec));
PRINT(("nAvgBytesPerSec=%d\n",old->nAvgBytesPerSec));
PRINT(("nBlockAlign    =%d\n",old->nBlockAlign));
PRINT(("wBitsPerSample =%d\n",old->wBitsPerSample));
PRINT(("cbSize         =%d\n",old->cbSize));
}
PaSampleFormat WaveToPaFormat(const WAVEFORMATEXTENSIBLE *fmtext)
{
const WAVEFORMATEX *fmt = (WAVEFORMATEX *)fmtext;
switch (fmt->wFormatTag)
{
case WAVE_FORMAT_EXTENSIBLE: {
if (IsEqualGUID(&fmtext->SubFormat, &pa_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
{
if (fmtext->Samples.wValidBitsPerSample == 32)
return paFloat32;
}
else
if (IsEqualGUID(&fmtext->SubFormat, &pa_KSDATAFORMAT_SUBTYPE_PCM))
{
switch (fmt->wBitsPerSample)
{
case 32: return paInt32;
case 24: return paInt24;
case 16: return paInt16;
case  8: return paUInt8;
}
}
break; }
case WAVE_FORMAT_IEEE_FLOAT:
return paFloat32;
case WAVE_FORMAT_PCM: {
switch (fmt->wBitsPerSample)
{
case 32: return paInt32;
case 24: return paInt24;
case 16: return paInt16;
case  8: return paUInt8;
}
break; }
}
return paCustomFormat;
}
static PaError MakeWaveFormatFromParams(WAVEFORMATEXTENSIBLE *wavex, const PaStreamParameters *params,
double sampleRate, BOOL packedOnly)
{
WORD bitsPerSample;
WAVEFORMATEX *old;
DWORD channelMask = 0;
BOOL useExtensible = (params->channelCount > 2);  
PaWasapiStreamInfo *streamInfo = (PaWasapiStreamInfo *)params->hostApiSpecificStreamInfo;
if ((bitsPerSample = PaSampleFormatToBitsPerSample(params->sampleFormat)) == 0)
return paSampleFormatNotSupported;
if ((streamInfo != NULL) && (streamInfo->flags & paWinWasapiUseChannelMask))
{
channelMask   = streamInfo->channelMask;
useExtensible = TRUE;
}
memset(wavex, 0, sizeof(*wavex));
old                    = (WAVEFORMATEX *)wavex;
old->nChannels      = (WORD)params->channelCount;
old->nSamplesPerSec = (DWORD)sampleRate;
old->wBitsPerSample = bitsPerSample;
if ((bitsPerSample != 8) && (bitsPerSample != 16))
{
old->wBitsPerSample = (packedOnly ? bitsPerSample : 32);
useExtensible       = TRUE;
}
if (!useExtensible)
{
old->wFormatTag    = WAVE_FORMAT_PCM;
}
else
{
old->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
old->cbSize        = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
if ((params->sampleFormat & ~paNonInterleaved) == paFloat32)
wavex->SubFormat = pa_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
else
wavex->SubFormat = pa_KSDATAFORMAT_SUBTYPE_PCM;
wavex->Samples.wValidBitsPerSample = bitsPerSample;
if (channelMask != 0)
{
wavex->dwChannelMask = channelMask;
}
else
{
switch (params->channelCount)
{
case 1:  wavex->dwChannelMask = PAWIN_SPEAKER_MONO; break;
case 2:  wavex->dwChannelMask = PAWIN_SPEAKER_STEREO; break;
case 3:  wavex->dwChannelMask = PAWIN_SPEAKER_STEREO|SPEAKER_LOW_FREQUENCY; break;
case 4:  wavex->dwChannelMask = PAWIN_SPEAKER_QUAD; break;
case 5:  wavex->dwChannelMask = PAWIN_SPEAKER_QUAD|SPEAKER_LOW_FREQUENCY; break;
#ifdef PAWIN_SPEAKER_5POINT1_SURROUND
case 6:  wavex->dwChannelMask = PAWIN_SPEAKER_5POINT1_SURROUND; break;
#else
case 6:  wavex->dwChannelMask = PAWIN_SPEAKER_5POINT1; break;
#endif
#ifdef PAWIN_SPEAKER_5POINT1_SURROUND
case 7:  wavex->dwChannelMask = PAWIN_SPEAKER_5POINT1_SURROUND|SPEAKER_BACK_CENTER; break;
#else
case 7:  wavex->dwChannelMask = PAWIN_SPEAKER_5POINT1|SPEAKER_BACK_CENTER; break;
#endif
#ifdef PAWIN_SPEAKER_7POINT1_SURROUND
case 8:  wavex->dwChannelMask = PAWIN_SPEAKER_7POINT1_SURROUND; break;
#else
case 8:  wavex->dwChannelMask = PAWIN_SPEAKER_7POINT1; break;
#endif
default: wavex->dwChannelMask = 0;
}
}
}
old->nBlockAlign     = old->nChannels * (old->wBitsPerSample / 8);
old->nAvgBytesPerSec = old->nSamplesPerSec * old->nBlockAlign;
return paNoError;
}
static HRESULT GetAlternativeSampleFormatExclusive(IAudioClient *client, double sampleRate,
const PaStreamParameters *params, WAVEFORMATEXTENSIBLE *outWavex, BOOL packedSampleFormatOnly)
{
HRESULT hr = !S_OK;
AUDCLNT_SHAREMODE shareMode = AUDCLNT_SHAREMODE_EXCLUSIVE;
WAVEFORMATEXTENSIBLE testFormat;
PaStreamParameters testParams;
int i;
static const PaSampleFormat bestToWorst[] = { paInt32, paInt24, paFloat32, paInt16 };
if (params->channelCount == 1)
{
testParams = (*params);
testParams.channelCount = 2;
if (MakeWaveFormatFromParams(&testFormat, &testParams, sampleRate, packedSampleFormatOnly) == paNoError)
{
if ((hr = IAudioClient_IsFormatSupported(client, shareMode, &testFormat.Format, NULL)) == S_OK)
{
(*outWavex) = testFormat;
return hr;
}
}
for (i = 0; i < STATIC_ARRAY_SIZE(bestToWorst); ++i)
{
testParams.sampleFormat = bestToWorst[i];
if (MakeWaveFormatFromParams(&testFormat, &testParams, sampleRate, packedSampleFormatOnly) == paNoError)
{
if ((hr = IAudioClient_IsFormatSupported(client, shareMode, &testFormat.Format, NULL)) == S_OK)
{
(*outWavex) = testFormat;
return hr;
}
}
}
}
testParams = (*params);
for (i = 0; i < STATIC_ARRAY_SIZE(bestToWorst); ++i)
{
testParams.sampleFormat = bestToWorst[i];
if (MakeWaveFormatFromParams(&testFormat, &testParams, sampleRate, packedSampleFormatOnly) == paNoError)
{
if ((hr = IAudioClient_IsFormatSupported(client, shareMode, &testFormat.Format, NULL)) == S_OK)
{
(*outWavex) = testFormat;
return hr;
}
}
}
return hr;
}
static PaError GetClosestFormat(IAudioClient *client, double sampleRate, const PaStreamParameters *_params,
AUDCLNT_SHAREMODE shareMode, WAVEFORMATEXTENSIBLE *outWavex, BOOL output)
{
PaWasapiStreamInfo *streamInfo   = (PaWasapiStreamInfo *)_params->hostApiSpecificStreamInfo;
WAVEFORMATEX *sharedClosestMatch = NULL;
HRESULT hr                       = !S_OK;
PaStreamParameters params        = (*_params);
const BOOL explicitFormat        = (streamInfo != NULL) && ((streamInfo->flags & paWinWasapiExplicitSampleFormat) == paWinWasapiExplicitSampleFormat);
(void)output;
MakeWaveFormatFromParams(outWavex, &params, sampleRate, FALSE);
if ((GetWindowsVersion() >= WINDOWS_7_SERVER2008R2) &&
(shareMode == AUDCLNT_SHAREMODE_SHARED) &&
((streamInfo != NULL) && (streamInfo->flags & paWinWasapiAutoConvert)))
return paFormatIsSupported;
hr = IAudioClient_IsFormatSupported(client, shareMode, &outWavex->Format, (shareMode == AUDCLNT_SHAREMODE_SHARED ? &sharedClosestMatch : NULL));
if ((hr != S_OK) && (shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE))
{
MakeWaveFormatFromParams(outWavex, &params, sampleRate, TRUE);
hr = IAudioClient_IsFormatSupported(client, shareMode, &outWavex->Format, NULL);
}
if (hr == S_OK)
{
return paFormatIsSupported;
}
else
if (sharedClosestMatch != NULL)
{
WORD bitsPerSample;
if (sharedClosestMatch->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
memcpy(outWavex, sharedClosestMatch, sizeof(WAVEFORMATEXTENSIBLE));
else
memcpy(outWavex, sharedClosestMatch, sizeof(WAVEFORMATEX));
CoTaskMemFree(sharedClosestMatch);
sharedClosestMatch = NULL;
if ((DWORD)sampleRate != outWavex->Format.nSamplesPerSec)
return paInvalidSampleRate;
if ((WORD)params.channelCount != outWavex->Format.nChannels)
{
if ((params.channelCount == 1) && (outWavex->Format.nChannels == 2))
return paFormatIsSupported;
else
return paInvalidChannelCount;
}
if ((bitsPerSample = PaSampleFormatToBitsPerSample(params.sampleFormat)) == 0)
return paSampleFormatNotSupported;
return paFormatIsSupported;
}
else
if ((shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE) && !explicitFormat)
{
if ((hr = GetAlternativeSampleFormatExclusive(client, sampleRate, &params, outWavex, FALSE)) == S_OK)
return paFormatIsSupported;
if ((hr = GetAlternativeSampleFormatExclusive(client, sampleRate, &params, outWavex, TRUE)) == S_OK)
return paFormatIsSupported;
LogHostError(hr);
}
else
{
LogHostError(hr);
}
return paInvalidSampleRate;
}
static PaError IsStreamParamsValid(struct PaUtilHostApiRepresentation *hostApi,
const  PaStreamParameters *inputParameters,
const  PaStreamParameters *outputParameters,
double sampleRate)
{
if (hostApi == NULL)
return paHostApiNotFound;
if ((UINT32)sampleRate == 0)
return paInvalidSampleRate;
if (inputParameters != NULL)
{
if (inputParameters->device == paUseHostApiSpecificDeviceSpecification)
return paInvalidDevice;
if (inputParameters->channelCount > hostApi->deviceInfos[ inputParameters->device ]->maxInputChannels)
return paInvalidChannelCount;
if (inputParameters->hostApiSpecificStreamInfo)
{
PaWasapiStreamInfo *inputStreamInfo = (PaWasapiStreamInfo *)inputParameters->hostApiSpecificStreamInfo;
if ((inputStreamInfo->size != sizeof(PaWasapiStreamInfo)) ||
(inputStreamInfo->version != 1) ||
(inputStreamInfo->hostApiType != paWASAPI))
{
return paIncompatibleHostApiSpecificStreamInfo;
}
}
return paNoError;
}
if (outputParameters != NULL)
{
if (outputParameters->device == paUseHostApiSpecificDeviceSpecification)
return paInvalidDevice;
if (outputParameters->channelCount > hostApi->deviceInfos[ outputParameters->device ]->maxOutputChannels)
return paInvalidChannelCount;
if(outputParameters->hostApiSpecificStreamInfo)
{
PaWasapiStreamInfo *outputStreamInfo = (PaWasapiStreamInfo *)outputParameters->hostApiSpecificStreamInfo;
if ((outputStreamInfo->size != sizeof(PaWasapiStreamInfo)) ||
(outputStreamInfo->version != 1) ||
(outputStreamInfo->hostApiType != paWASAPI))
{
return paIncompatibleHostApiSpecificStreamInfo;
}
}
return paNoError;
}
return (inputParameters || outputParameters ? paNoError : paInternalError);
}
static PaError IsFormatSupported( struct PaUtilHostApiRepresentation *hostApi,
const  PaStreamParameters *inputParameters,
const  PaStreamParameters *outputParameters,
double sampleRate )
{
IAudioClient *tmpClient = NULL;
PaWasapiHostApiRepresentation *paWasapi = (PaWasapiHostApiRepresentation*)hostApi;
PaWasapiStreamInfo *inputStreamInfo = NULL, *outputStreamInfo = NULL;
PaError error;
if ((error = IsStreamParamsValid(hostApi, inputParameters, outputParameters, sampleRate)) != paNoError)
return error;
if (inputParameters != NULL)
{
WAVEFORMATEXTENSIBLE wavex;
HRESULT hr;
PaError answer;
AUDCLNT_SHAREMODE shareMode = AUDCLNT_SHAREMODE_SHARED;
inputStreamInfo = (PaWasapiStreamInfo *)inputParameters->hostApiSpecificStreamInfo;
if (inputStreamInfo && (inputStreamInfo->flags & paWinWasapiExclusive))
shareMode = AUDCLNT_SHAREMODE_EXCLUSIVE;
hr = ActivateAudioInterface(&paWasapi->devInfo[inputParameters->device], inputStreamInfo, &tmpClient);
if (hr != S_OK)
{
LogHostError(hr);
return paInvalidDevice;
}
answer = GetClosestFormat(tmpClient, sampleRate, inputParameters, shareMode, &wavex, FALSE);
SAFE_RELEASE(tmpClient);
if (answer != paFormatIsSupported)
return answer;
}
if (outputParameters != NULL)
{
HRESULT hr;
WAVEFORMATEXTENSIBLE wavex;
PaError answer;
AUDCLNT_SHAREMODE shareMode = AUDCLNT_SHAREMODE_SHARED;
outputStreamInfo = (PaWasapiStreamInfo *)outputParameters->hostApiSpecificStreamInfo;
if (outputStreamInfo && (outputStreamInfo->flags & paWinWasapiExclusive))
shareMode = AUDCLNT_SHAREMODE_EXCLUSIVE;
hr = ActivateAudioInterface(&paWasapi->devInfo[outputParameters->device], outputStreamInfo, &tmpClient);
if (hr != S_OK)
{
LogHostError(hr);
return paInvalidDevice;
}
answer = GetClosestFormat(tmpClient, sampleRate, outputParameters, shareMode, &wavex, TRUE);
SAFE_RELEASE(tmpClient);
if (answer != paFormatIsSupported)
return answer;
}
return paFormatIsSupported;
}
static PaUint32 _GetFramesPerHostBuffer(PaUint32 userFramesPerBuffer, PaTime suggestedLatency, double sampleRate, PaUint32 TimerJitterMs)
{
PaUint32 frames = userFramesPerBuffer + max( userFramesPerBuffer, (PaUint32)(suggestedLatency * sampleRate) );
frames += (PaUint32)((sampleRate * 0.001) * TimerJitterMs);
return frames;
}
static void _RecalculateBuffersCount(PaWasapiSubStream *sub, UINT32 userFramesPerBuffer, UINT32 framesPerLatency,
BOOL fullDuplex, BOOL output)
{
sub->buffers = (userFramesPerBuffer != 0 ? framesPerLatency / userFramesPerBuffer : 1);
if (sub->buffers == 0)
sub->buffers = 1;
if ((sub->shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE) || fullDuplex)
{
BOOL eventMode = ((sub->streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) == AUDCLNT_STREAMFLAGS_EVENTCALLBACK);
if (eventMode)
sub->userBufferAndHostMatch = 1;
if (fullDuplex || eventMode || !output)
sub->buffers = 1;
}
}
static void _CalculateAlignedPeriod(PaWasapiSubStream *pSub, UINT32 *nFramesPerLatency, ALIGN_FUNC pAlignFunc)
{
if (pSub->shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE)
{
(*nFramesPerLatency) = AlignFramesPerBuffer((*nFramesPerLatency),
pSub->wavex.Format.nBlockAlign, pAlignFunc);
}
pSub->period = MakeHnsPeriod((*nFramesPerLatency), pSub->wavex.Format.nSamplesPerSec);
}
static void _CalculatePeriodicity(PaWasapiSubStream *pSub, BOOL output, REFERENCE_TIME *periodicity)
{
if (pSub->shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE)
{
const PaWasapiDeviceInfo *pInfo = pSub->params.device_info;
(*periodicity) = pSub->period;
if (((pSub->streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) == 0) &&
(output && !pSub->params.full_duplex))
{
UINT32 alignedFrames;
REFERENCE_TIME userPeriodicity;
alignedFrames = AlignFramesPerBuffer(pSub->params.frames_per_buffer,
pSub->wavex.Format.nBlockAlign, ALIGN_BWD);
userPeriodicity = MakeHnsPeriod(alignedFrames, pSub->wavex.Format.nSamplesPerSec);
if (userPeriodicity > pSub->period)
userPeriodicity = pSub->period;
if (userPeriodicity < pInfo->MinimumDevicePeriod)
userPeriodicity = pInfo->MinimumDevicePeriod;
(*periodicity) = userPeriodicity;
}
}
else
(*periodicity) = 0;
}
static HRESULT CreateAudioClient(PaWasapiStream *pStream, PaWasapiSubStream *pSub, BOOL output, PaError *pa_error)
{
PaError error;
HRESULT hr;
const PaWasapiDeviceInfo *pInfo  = pSub->params.device_info;
const PaStreamParameters *params = &pSub->params.stream_params;
const double sampleRate          = pSub->params.sample_rate;
const BOOL fullDuplex            = pSub->params.full_duplex;
const UINT32 userFramesPerBuffer = pSub->params.frames_per_buffer;
UINT32 framesPerLatency          = userFramesPerBuffer;
IAudioClient *audioClient        = NULL;
REFERENCE_TIME eventPeriodicity  = 0;
(*pa_error) = paInvalidDevice;
if (!pSub || !pInfo || !params)
{
(*pa_error) = paBadStreamPtr;
return E_POINTER;
}
if ((UINT32)sampleRate == 0)
{
(*pa_error) = paInvalidSampleRate;
return E_INVALIDARG;
}
if (FAILED(hr = ActivateAudioInterface(pInfo, &pSub->params.wasapi_params, &audioClient)))
{
(*pa_error) = paInsufficientMemory;
LogHostError(hr);
goto done;
}
if ((error = GetClosestFormat(audioClient, sampleRate, params, pSub->shareMode, &pSub->wavex, output)) != paFormatIsSupported)
{
(*pa_error) = error;
LogHostError(hr = AUDCLNT_E_UNSUPPORTED_FORMAT);
goto done;  
}
if ((params->channelCount == 1) && (pSub->wavex.Format.nChannels == 2))
{
pSub->monoMixer = GetMonoToStereoMixer(&pSub->wavex, (pInfo->flow == eRender ? MIX_DIR__1TO2 : MIX_DIR__2TO1_L));
if (pSub->monoMixer == NULL)
{
(*pa_error) = paInvalidChannelCount;
LogHostError(hr = AUDCLNT_E_UNSUPPORTED_FORMAT);
goto done;  
}
}
if ((pSub->shareMode != AUDCLNT_SHAREMODE_EXCLUSIVE) &&
(!pSub->streamFlags || ((pSub->streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) == 0)))
{
framesPerLatency = _GetFramesPerHostBuffer(userFramesPerBuffer,
params->suggestedLatency, pSub->wavex.Format.nSamplesPerSec, 0 );
}
else
{
#ifdef PA_WASAPI_FORCE_POLL_IF_LARGE_BUFFER
REFERENCE_TIME overall;
#endif
framesPerLatency += MakeFramesFromHns(SecondsTonano100(params->suggestedLatency), pSub->wavex.Format.nSamplesPerSec);
#ifdef PA_WASAPI_FORCE_POLL_IF_LARGE_BUFFER
overall = MakeHnsPeriod(framesPerLatency, pSub->wavex.Format.nSamplesPerSec);
if (overall >= (106667 * 2) )
{
framesPerLatency = _GetFramesPerHostBuffer(userFramesPerBuffer,
params->suggestedLatency, pSub->wavex.Format.nSamplesPerSec, 0 );
pSub->streamFlags &= ~AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
PRINT(("WASAPI: CreateAudioClient: forcing POLL mode\n"));
}
#endif
}
if (output && fullDuplex)
framesPerLatency = pStream->in.framesPerHostCallback;
if (framesPerLatency == 0)
framesPerLatency = MakeFramesFromHns(pInfo->DefaultDevicePeriod, pSub->wavex.Format.nSamplesPerSec);
if (!output && (pSub->shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE))
{
if ((pSub->streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) == 0)
framesPerLatency /= WASAPI_PACKETS_PER_INPUT_BUFFER;
}
_CalculateAlignedPeriod(pSub, &framesPerLatency, ALIGN_BWD);
if (pSub->shareMode == AUDCLNT_SHAREMODE_SHARED)
{
if (pSub->period < pInfo->DefaultDevicePeriod)
{
pSub->period = pInfo->DefaultDevicePeriod;
framesPerLatency = MakeFramesFromHns(pSub->period, pSub->wavex.Format.nSamplesPerSec);
_CalculateAlignedPeriod(pSub, &framesPerLatency, ALIGN_BWD);
}
}
else
{
if (pSub->period < pInfo->MinimumDevicePeriod)
{
pSub->period = pInfo->MinimumDevicePeriod;
framesPerLatency = MakeFramesFromHns(pSub->period, pSub->wavex.Format.nSamplesPerSec);
_CalculateAlignedPeriod(pSub, &framesPerLatency, ALIGN_FWD);
}
}
{
if (pSub->shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE)
{
static const REFERENCE_TIME MAX_BUFFER_EVENT_DURATION = 500  * 10000;
static const REFERENCE_TIME MAX_BUFFER_POLL_DURATION  = 2000 * 10000;
if (pSub->streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
{
if (pSub->period > MAX_BUFFER_EVENT_DURATION)
{
pSub->period = MAX_BUFFER_EVENT_DURATION;
framesPerLatency = MakeFramesFromHns(pSub->period, pSub->wavex.Format.nSamplesPerSec);
_CalculateAlignedPeriod(pSub, &framesPerLatency, ALIGN_BWD);
}
}
else
{
if (pSub->period > MAX_BUFFER_POLL_DURATION)
{
pSub->period = MAX_BUFFER_POLL_DURATION;
framesPerLatency = MakeFramesFromHns(pSub->period, pSub->wavex.Format.nSamplesPerSec);
_CalculateAlignedPeriod(pSub, &framesPerLatency, ALIGN_BWD);
}
}
}
}
_CalculatePeriodicity(pSub, output, &eventPeriodicity);
hr = IAudioClient_Initialize(audioClient,
pSub->shareMode,
pSub->streamFlags,
pSub->period,
eventPeriodicity,
&pSub->wavex.Format,
NULL);
if (output && SUCCEEDED(hr) && (pSub->shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE))
{
UINT32 maxBufferFrames;
if (FAILED(hr = IAudioClient_GetBufferSize(audioClient, &maxBufferFrames)))
{
(*pa_error) = paInvalidDevice;
LogHostError(hr);
goto done;
}
if (maxBufferFrames >= (framesPerLatency * 2))
{
UINT32 ratio = maxBufferFrames / framesPerLatency;
PRINT(("WASAPI: CreateAudioClient: detected %d times larger buffer than requested, correct to match user latency\n", ratio));
framesPerLatency = MakeFramesFromHns(pSub->period / ratio, pSub->wavex.Format.nSamplesPerSec);
_CalculateAlignedPeriod(pSub, &framesPerLatency, ALIGN_BWD);
if (pSub->period < pInfo->MinimumDevicePeriod)
pSub->period = pInfo->MinimumDevicePeriod;
SAFE_RELEASE(audioClient);
if (FAILED(hr = ActivateAudioInterface(pInfo, &pSub->params.wasapi_params, &audioClient)))
{
(*pa_error) = paInsufficientMemory;
LogHostError(hr);
goto done;
}
_CalculatePeriodicity(pSub, output, &eventPeriodicity);
hr = IAudioClient_Initialize(audioClient,
pSub->shareMode,
pSub->streamFlags,
pSub->period,
eventPeriodicity,
&pSub->wavex.Format,
NULL);
}
}
while ((hr == E_OUTOFMEMORY) && (pSub->period > (100 * 10000)))
{
PRINT(("WASAPI: CreateAudioClient: decreasing buffer size to %d milliseconds\n", (pSub->period / 10000)));
pSub->period -= (100 * 10000);
framesPerLatency = MakeFramesFromHns(pSub->period, pSub->wavex.Format.nSamplesPerSec);
_CalculateAlignedPeriod(pSub, &framesPerLatency, ALIGN_BWD);
SAFE_RELEASE(audioClient);
if (FAILED(hr = ActivateAudioInterface(pInfo, &pSub->params.wasapi_params, &audioClient)))
{
(*pa_error) = paInsufficientMemory;
LogHostError(hr);
goto done;
}
_CalculatePeriodicity(pSub, output, &eventPeriodicity);
hr = IAudioClient_Initialize(audioClient,
pSub->shareMode,
pSub->streamFlags,
pSub->period,
eventPeriodicity,
&pSub->wavex.Format,
NULL);
}
if ((hr == AUDCLNT_E_BUFFER_SIZE_ERROR) || (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED))
{
pSub->period = pInfo->DefaultDevicePeriod;
PRINT(("WASAPI: CreateAudioClient: correcting buffer size/alignment to device default\n"));
SAFE_RELEASE(audioClient);
if (FAILED(hr = ActivateAudioInterface(pInfo, &pSub->params.wasapi_params, &audioClient)))
{
(*pa_error) = paInsufficientMemory;
LogHostError(hr);
goto done;
}
_CalculatePeriodicity(pSub, output, &eventPeriodicity);
hr = IAudioClient_Initialize(audioClient,
pSub->shareMode,
pSub->streamFlags,
pSub->period,
eventPeriodicity,
&pSub->wavex.Format,
NULL);
}
if (FAILED(hr))
{
(*pa_error) = paInvalidDevice;
LogHostError(hr);
goto done;
}
pSub->clientParent = audioClient;
IAudioClient_AddRef(pSub->clientParent);
_RecalculateBuffersCount(pSub, userFramesPerBuffer, MakeFramesFromHns(pSub->period, pSub->wavex.Format.nSamplesPerSec),
fullDuplex, output);
(*pa_error) = paNoError;
done:
SAFE_RELEASE(audioClient);
return hr;
}
static PaError ActivateAudioClientOutput(PaWasapiStream *stream)
{
HRESULT hr;
PaError result;
UINT32 maxBufferSize;
PaTime bufferLatency;
const UINT32 framesPerBuffer = stream->out.params.frames_per_buffer;
if (FAILED(hr = CreateAudioClient(stream, &stream->out, TRUE, &result)))
{
LogPaError(result);
goto error;
}
LogWAVEFORMATEXTENSIBLE(&stream->out.wavex);
stream->outVol = NULL;
if (FAILED(hr = IAudioClient_GetBufferSize(stream->out.clientParent, &maxBufferSize)))
{
LogHostError(hr);
LogPaError(result = paInvalidDevice);
goto error;
}
stream->out.bufferSize = maxBufferSize;
stream->out.framesPerHostCallback = maxBufferSize;
stream->out.framesPerBuffer =
(stream->out.userBufferAndHostMatch ? stream->out.framesPerHostCallback : framesPerBuffer);
bufferLatency = (PaTime)maxBufferSize / stream->out.wavex.Format.nSamplesPerSec;
stream->out.latencySeconds = bufferLatency;
PRINT(("WASAPI::OpenStream(output): framesPerUser[ %d ] framesPerHost[ %d ] latency[ %.02fms ] exclusive[ %s ] wow64_fix[ %s ] mode[ %s ]\n", (UINT32)framesPerBuffer, (UINT32)stream->out.framesPerHostCallback, (float)(stream->out.latencySeconds*1000.0f), (stream->out.shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE ? "YES" : "NO"), (stream->out.params.wow64_workaround ? "YES" : "NO"), (stream->out.streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK ? "EVENT" : "POLL")));
return paNoError;
error:
return result;
}
static PaError ActivateAudioClientInput(PaWasapiStream *stream)
{
HRESULT hr;
PaError result;
UINT32 maxBufferSize;
PaTime bufferLatency;
const UINT32 framesPerBuffer = stream->in.params.frames_per_buffer;
if (FAILED(hr = CreateAudioClient(stream, &stream->in, FALSE, &result)))
{
LogPaError(result);
goto error;
}
LogWAVEFORMATEXTENSIBLE(&stream->in.wavex);
stream->inVol = NULL;
if (FAILED(hr = IAudioClient_GetBufferSize(stream->in.clientParent, &maxBufferSize)))
{
LogHostError(hr);
LogPaError(result = paInvalidDevice);
goto error;
}
stream->in.bufferSize = maxBufferSize;
if (FAILED(hr = IAudioClient_GetStreamLatency(stream->in.clientParent, &stream->in.deviceLatency)))
{
LogHostError(hr);
LogPaError(result = paInvalidDevice);
goto error;
}
stream->in.framesPerHostCallback = maxBufferSize;
stream->in.framesPerBuffer =
(stream->in.userBufferAndHostMatch ? stream->in.framesPerHostCallback : framesPerBuffer);
bufferLatency = (PaTime)maxBufferSize / stream->in.wavex.Format.nSamplesPerSec;
stream->in.latencySeconds = bufferLatency;
PRINT(("WASAPI::OpenStream(input): framesPerUser[ %d ] framesPerHost[ %d ] latency[ %.02fms ] exclusive[ %s ] wow64_fix[ %s ] mode[ %s ]\n", (UINT32)framesPerBuffer, (UINT32)stream->in.framesPerHostCallback, (float)(stream->in.latencySeconds*1000.0f), (stream->in.shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE ? "YES" : "NO"), (stream->in.params.wow64_workaround ? "YES" : "NO"), (stream->in.streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK ? "EVENT" : "POLL")));
return paNoError;
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
HRESULT hr;
PaWasapiHostApiRepresentation *paWasapi = (PaWasapiHostApiRepresentation*)hostApi;
PaWasapiStream *stream = NULL;
int inputChannelCount, outputChannelCount;
PaSampleFormat inputSampleFormat, outputSampleFormat;
PaSampleFormat hostInputSampleFormat, hostOutputSampleFormat;
PaWasapiStreamInfo *inputStreamInfo = NULL, *outputStreamInfo = NULL;
PaWasapiDeviceInfo *info = NULL;
ULONG framesPerHostCallback;
PaUtilHostBufferSizeMode bufferMode;
const BOOL fullDuplex = ((inputParameters != NULL) && (outputParameters != NULL));
BOOL useInputBufferProcessor = (inputParameters != NULL), useOutputBufferProcessor = (outputParameters != NULL);
if ((result = IsStreamParamsValid(hostApi, inputParameters, outputParameters, sampleRate)) != paNoError)
return LogPaError(result);
if ((streamFlags & paPlatformSpecificFlags) != 0)
{
LogPaError(result = paInvalidFlag);  
goto error;
}
if ((stream = (PaWasapiStream *)PaUtil_AllocateMemory(sizeof(PaWasapiStream))) == NULL)
{
LogPaError(result = paInsufficientMemory);
goto error;
}
stream->nThreadPriority = eThreadPriorityAudio;
if (framesPerBuffer == paFramesPerBufferUnspecified)
{
UINT32 framesPerBufferIn  = 0, framesPerBufferOut = 0;
if (inputParameters != NULL)
{
info = &paWasapi->devInfo[inputParameters->device];
framesPerBufferIn = MakeFramesFromHns(info->DefaultDevicePeriod, (UINT32)sampleRate);
}
if (outputParameters != NULL)
{
info = &paWasapi->devInfo[outputParameters->device];
framesPerBufferOut = MakeFramesFromHns(info->DefaultDevicePeriod, (UINT32)sampleRate);
}
framesPerBuffer = max(framesPerBufferIn, framesPerBufferOut);
}
if (framesPerBuffer == 0)
framesPerBuffer = ((UINT32)sampleRate / 100) * 2;
if (inputParameters != NULL)
{
inputChannelCount = inputParameters->channelCount;
inputSampleFormat = GetSampleFormatForIO(inputParameters->sampleFormat);
info              = &paWasapi->devInfo[inputParameters->device];
stream->in.shareMode = AUDCLNT_SHAREMODE_SHARED;
if (inputParameters->hostApiSpecificStreamInfo != NULL)
{
memcpy(&stream->in.params.wasapi_params, inputParameters->hostApiSpecificStreamInfo, min(sizeof(stream->in.params.wasapi_params), ((PaWasapiStreamInfo *)inputParameters->hostApiSpecificStreamInfo)->size));
stream->in.params.wasapi_params.size = sizeof(stream->in.params.wasapi_params);
stream->in.params.stream_params.hostApiSpecificStreamInfo = &stream->in.params.wasapi_params;
inputStreamInfo = &stream->in.params.wasapi_params;
stream->in.flags = inputStreamInfo->flags;
if (inputStreamInfo->flags & paWinWasapiExclusive)
{
stream->nThreadPriority = eThreadPriorityProAudio;
stream->in.shareMode = AUDCLNT_SHAREMODE_EXCLUSIVE;
}
if (inputStreamInfo->flags & paWinWasapiThreadPriority)
{
if ((inputStreamInfo->threadPriority > eThreadPriorityNone) &&
(inputStreamInfo->threadPriority <= eThreadPriorityWindowManager))
stream->nThreadPriority = inputStreamInfo->threadPriority;
}
useInputBufferProcessor = !(inputStreamInfo->flags & paWinWasapiRedirectHostProcessor);
}
stream->in.streamFlags = (stream->in.shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE ? AUDCLNT_STREAMFLAGS_EVENTCALLBACK : 0);
if (paWasapi->useWOW64Workaround)
stream->in.streamFlags = 0;  
else
if (streamCallback == NULL)
stream->in.streamFlags = 0;  
else
if ((inputStreamInfo != NULL) && (inputStreamInfo->flags & paWinWasapiPolling))
stream->in.streamFlags = 0;  
else
if (fullDuplex)
stream->in.streamFlags = 0;  
if ((GetWindowsVersion() >= WINDOWS_7_SERVER2008R2) &&
(stream->in.shareMode == AUDCLNT_SHAREMODE_SHARED) &&
((inputStreamInfo != NULL) && (inputStreamInfo->flags & paWinWasapiAutoConvert)))
stream->in.streamFlags |= (AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
stream->in.params.device_info       = info;
stream->in.params.stream_params     = (*inputParameters);
stream->in.params.frames_per_buffer = framesPerBuffer;
stream->in.params.sample_rate       = sampleRate;
stream->in.params.blocking          = (streamCallback == NULL);
stream->in.params.full_duplex       = fullDuplex;
stream->in.params.wow64_workaround  = paWasapi->useWOW64Workaround;
if ((result = ActivateAudioClientInput(stream)) != paNoError)
{
LogPaError(result);
goto error;
}
hostInputSampleFormat = PaUtil_SelectClosestAvailableFormat(WaveToPaFormat(&stream->in.wavex), inputSampleFormat);
if ((inputStreamInfo != NULL) &&
(inputStreamInfo->flags & paWinWasapiRedirectHostProcessor))
{
stream->hostProcessOverrideInput.processor = inputStreamInfo->hostProcessorInput;
stream->hostProcessOverrideInput.userData = userData;
}
if (FAILED(hr = IAudioClient_GetService(stream->in.clientParent, &pa_IID_IAudioCaptureClient, (void **)&stream->captureClientParent)))
{
LogHostError(hr);
LogPaError(result = paUnanticipatedHostError);
goto error;
}
if (stream->in.params.blocking == TRUE)
{
UINT32 bufferFrames = ALIGN_NEXT_POW2((stream->in.framesPerHostCallback / WASAPI_PACKETS_PER_INPUT_BUFFER) * 2);
UINT32 frameSize    = stream->in.wavex.Format.nBlockAlign;
if ((stream->in.tailBuffer = PaUtil_AllocateMemory(sizeof(PaUtilRingBuffer))) == NULL)
{
LogPaError(result = paInsufficientMemory);
goto error;
}
memset(stream->in.tailBuffer, 0, sizeof(PaUtilRingBuffer));
stream->in.tailBufferMemory = PaUtil_AllocateMemory(frameSize * bufferFrames);
if (stream->in.tailBufferMemory == NULL)
{
LogPaError(result = paInsufficientMemory);
goto error;
}
if (PaUtil_InitializeRingBuffer(stream->in.tailBuffer, frameSize, bufferFrames,    stream->in.tailBufferMemory) != 0)
{
LogPaError(result = paInternalError);
goto error;
}
}
}
else
{
inputChannelCount = 0;
inputSampleFormat = hostInputSampleFormat = paInt16;  
}
if (outputParameters != NULL)
{
outputChannelCount = outputParameters->channelCount;
outputSampleFormat = GetSampleFormatForIO(outputParameters->sampleFormat);
info               = &paWasapi->devInfo[outputParameters->device];
stream->out.shareMode = AUDCLNT_SHAREMODE_SHARED;
if (outputParameters->hostApiSpecificStreamInfo != NULL)
{
memcpy(&stream->out.params.wasapi_params, outputParameters->hostApiSpecificStreamInfo, min(sizeof(stream->out.params.wasapi_params), ((PaWasapiStreamInfo *)outputParameters->hostApiSpecificStreamInfo)->size));
stream->out.params.wasapi_params.size = sizeof(stream->out.params.wasapi_params);
stream->out.params.stream_params.hostApiSpecificStreamInfo = &stream->out.params.wasapi_params;
outputStreamInfo = &stream->out.params.wasapi_params;
stream->out.flags = outputStreamInfo->flags;
if (outputStreamInfo->flags & paWinWasapiExclusive)
{
stream->nThreadPriority = eThreadPriorityProAudio;
stream->out.shareMode = AUDCLNT_SHAREMODE_EXCLUSIVE;
}
if (outputStreamInfo->flags & paWinWasapiThreadPriority)
{
if ((outputStreamInfo->threadPriority > eThreadPriorityNone) &&
(outputStreamInfo->threadPriority <= eThreadPriorityWindowManager))
stream->nThreadPriority = outputStreamInfo->threadPriority;
}
useOutputBufferProcessor = !(outputStreamInfo->flags & paWinWasapiRedirectHostProcessor);
}
stream->out.streamFlags = (stream->out.shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE ? AUDCLNT_STREAMFLAGS_EVENTCALLBACK : 0);
if (paWasapi->useWOW64Workaround)
stream->out.streamFlags = 0;  
else
if (streamCallback == NULL)
stream->out.streamFlags = 0;  
else
if ((outputStreamInfo != NULL) && (outputStreamInfo->flags & paWinWasapiPolling))
stream->out.streamFlags = 0;  
else
if (fullDuplex)
stream->out.streamFlags = 0;  
if ((GetWindowsVersion() >= WINDOWS_7_SERVER2008R2) &&
(stream->out.shareMode == AUDCLNT_SHAREMODE_SHARED) &&
((outputStreamInfo != NULL) && (outputStreamInfo->flags & paWinWasapiAutoConvert)))
stream->out.streamFlags |= (AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
stream->out.params.device_info       = info;
stream->out.params.stream_params     = (*outputParameters);
stream->out.params.frames_per_buffer = framesPerBuffer;
stream->out.params.sample_rate       = sampleRate;
stream->out.params.blocking          = (streamCallback == NULL);
stream->out.params.full_duplex       = fullDuplex;
stream->out.params.wow64_workaround  = paWasapi->useWOW64Workaround;
if ((result = ActivateAudioClientOutput(stream)) != paNoError)
{
LogPaError(result);
goto error;
}
hostOutputSampleFormat = PaUtil_SelectClosestAvailableFormat(WaveToPaFormat(&stream->out.wavex), outputSampleFormat);
if ((outputStreamInfo != NULL) &&
(outputStreamInfo->flags & paWinWasapiRedirectHostProcessor))
{
stream->hostProcessOverrideOutput.processor = outputStreamInfo->hostProcessorOutput;
stream->hostProcessOverrideOutput.userData = userData;
}
if (FAILED(hr = IAudioClient_GetService(stream->out.clientParent, &pa_IID_IAudioRenderClient, (void **)&stream->renderClientParent)))
{
LogHostError(hr);
LogPaError(result = paUnanticipatedHostError);
goto error;
}
}
else
{
outputChannelCount = 0;
outputSampleFormat = hostOutputSampleFormat = paInt16;  
}
if (fullDuplex)
PRINT(("WASAPI::OpenStream: full-duplex mode\n"));
if ((inputParameters != NULL) && (outputParameters != NULL))
{
if ((inputStreamInfo != NULL) && (outputStreamInfo != NULL))
{
if (((inputStreamInfo->flags & paWinWasapiPolling) &&
!(outputStreamInfo->flags & paWinWasapiPolling))
||
(!(inputStreamInfo->flags & paWinWasapiPolling) &&
(outputStreamInfo->flags & paWinWasapiPolling)))
{
LogPaError(result = paInvalidFlag);
goto error;
}
}
}
if (streamCallback)
{
stream->bBlocking = FALSE;
PaUtil_InitializeStreamRepresentation(&stream->streamRepresentation,
&paWasapi->callbackStreamInterface,
streamCallback, userData);
}
else
{
stream->bBlocking = TRUE;
PaUtil_InitializeStreamRepresentation(&stream->streamRepresentation,
&paWasapi->blockingStreamInterface,
streamCallback, userData);
}
PaUtil_InitializeCpuLoadMeasurer(&stream->cpuLoadMeasurer, sampleRate);
if (outputParameters && inputParameters)
{
}
framesPerHostCallback = (outputParameters ? stream->out.framesPerBuffer : stream->in.framesPerBuffer);
bufferMode = paUtilFixedHostBufferSize;
if (inputParameters)  
bufferMode = paUtilBoundedHostBufferSize;
else
if (outputParameters)
{
if ((stream->out.buffers == 1) &&
(!stream->out.streamFlags || ((stream->out.streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) == 0)))
bufferMode = paUtilBoundedHostBufferSize;
}
stream->bufferMode = bufferMode;
if (useInputBufferProcessor || useOutputBufferProcessor)
{
result =  PaUtil_InitializeBufferProcessor(
&stream->bufferProcessor,
inputChannelCount,
inputSampleFormat,
hostInputSampleFormat,
outputChannelCount,
outputSampleFormat,
hostOutputSampleFormat,
sampleRate,
streamFlags,
framesPerBuffer,
framesPerHostCallback,
bufferMode,
streamCallback,
userData);
if (result != paNoError)
{
LogPaError(result);
goto error;
}
}
stream->streamRepresentation.streamInfo.inputLatency =
(useInputBufferProcessor ? PaUtil_GetBufferProcessorInputLatencyFrames(&stream->bufferProcessor) / sampleRate : 0)
+ (inputParameters != NULL ? stream->in.latencySeconds : 0);
stream->streamRepresentation.streamInfo.outputLatency =
(useOutputBufferProcessor ? PaUtil_GetBufferProcessorOutputLatencyFrames(&stream->bufferProcessor) / sampleRate : 0)
+ (outputParameters != NULL ? stream->out.latencySeconds : 0);
stream->streamRepresentation.streamInfo.sampleRate = sampleRate;
(*s) = (PaStream *)stream;
return result;
error:
if (stream != NULL)
CloseStream(stream);
return result;
}
static PaError CloseStream( PaStream* s )
{
PaError result = paNoError;
PaWasapiStream *stream = (PaWasapiStream*)s;
if (IsStreamActive(s))
{
result = AbortStream(s);
}
SAFE_RELEASE(stream->captureClientParent);
SAFE_RELEASE(stream->renderClientParent);
SAFE_RELEASE(stream->out.clientParent);
SAFE_RELEASE(stream->in.clientParent);
SAFE_RELEASE(stream->inVol);
SAFE_RELEASE(stream->outVol);
CloseHandle(stream->event[S_INPUT]);
CloseHandle(stream->event[S_OUTPUT]);
_StreamCleanup(stream);
PaWasapi_FreeMemory(stream->in.monoBuffer);
PaWasapi_FreeMemory(stream->out.monoBuffer);
PaUtil_FreeMemory(stream->in.tailBuffer);
PaUtil_FreeMemory(stream->in.tailBufferMemory);
PaUtil_FreeMemory(stream->out.tailBuffer);
PaUtil_FreeMemory(stream->out.tailBufferMemory);
PaUtil_TerminateBufferProcessor(&stream->bufferProcessor);
PaUtil_TerminateStreamRepresentation(&stream->streamRepresentation);
PaUtil_FreeMemory(stream);
return result;
}
HRESULT UnmarshalSubStreamComPointers(PaWasapiSubStream *substream)
{
#ifndef PA_WINRT
HRESULT hResult = S_OK;
HRESULT hFirstBadResult = S_OK;
substream->clientProc = NULL;
hResult = CoGetInterfaceAndReleaseStream(substream->clientStream, GetAudioClientIID(), (LPVOID*)&substream->clientProc);
substream->clientStream = NULL;
if (hResult != S_OK)
{
hFirstBadResult = (hFirstBadResult == S_OK) ? hResult : hFirstBadResult;
}
return hFirstBadResult;
#else
(void)substream;
return S_OK;
#endif
}
HRESULT UnmarshalStreamComPointers(PaWasapiStream *stream)
{
#ifndef PA_WINRT
HRESULT hResult = S_OK;
HRESULT hFirstBadResult = S_OK;
stream->captureClient = NULL;
stream->renderClient = NULL;
stream->in.clientProc = NULL;
stream->out.clientProc = NULL;
if (NULL != stream->in.clientParent)
{
hResult = UnmarshalSubStreamComPointers(&stream->in);
if (hResult != S_OK)
{
hFirstBadResult = (hFirstBadResult == S_OK) ? hResult : hFirstBadResult;
}
hResult = CoGetInterfaceAndReleaseStream(stream->captureClientStream, &pa_IID_IAudioCaptureClient, (LPVOID*)&stream->captureClient);
stream->captureClientStream = NULL;
if (hResult != S_OK)
{
hFirstBadResult = (hFirstBadResult == S_OK) ? hResult : hFirstBadResult;
}
}
if (NULL != stream->out.clientParent)
{
hResult = UnmarshalSubStreamComPointers(&stream->out);
if (hResult != S_OK)
{
hFirstBadResult = (hFirstBadResult == S_OK) ? hResult : hFirstBadResult;
}
hResult = CoGetInterfaceAndReleaseStream(stream->renderClientStream, &pa_IID_IAudioRenderClient, (LPVOID*)&stream->renderClient);
stream->renderClientStream = NULL;
if (hResult != S_OK)
{
hFirstBadResult = (hFirstBadResult == S_OK) ? hResult : hFirstBadResult;
}
}
return hFirstBadResult;
#else
if (stream->in.clientParent != NULL)
{
stream->in.clientProc = stream->in.clientParent;
IAudioClient_AddRef(stream->in.clientParent);
}
if (stream->out.clientParent != NULL)
{
stream->out.clientProc = stream->out.clientParent;
IAudioClient_AddRef(stream->out.clientParent);
}
if (stream->renderClientParent != NULL)
{
stream->renderClient = stream->renderClientParent;
IAudioRenderClient_AddRef(stream->renderClientParent);
}
if (stream->captureClientParent != NULL)
{
stream->captureClient = stream->captureClientParent;
IAudioCaptureClient_AddRef(stream->captureClientParent);
}
return S_OK;
#endif
}
void ReleaseUnmarshaledSubComPointers(PaWasapiSubStream *substream)
{
SAFE_RELEASE(substream->clientProc);
}
void ReleaseUnmarshaledComPointers(PaWasapiStream *stream)
{
SAFE_RELEASE(stream->captureClient);
SAFE_RELEASE(stream->renderClient);
ReleaseUnmarshaledSubComPointers(&stream->in);
ReleaseUnmarshaledSubComPointers(&stream->out);
}
HRESULT MarshalSubStreamComPointers(PaWasapiSubStream *substream)
{
#ifndef PA_WINRT
HRESULT hResult;
substream->clientStream = NULL;
hResult = CoMarshalInterThreadInterfaceInStream(GetAudioClientIID(), (LPUNKNOWN)substream->clientParent, &substream->clientStream);
if (hResult != S_OK)
goto marshal_sub_error;
return hResult;
marshal_sub_error:
UnmarshalSubStreamComPointers(substream);
ReleaseUnmarshaledSubComPointers(substream);
return hResult;
#else
(void)substream;
return S_OK;
#endif
}
HRESULT MarshalStreamComPointers(PaWasapiStream *stream)
{
#ifndef PA_WINRT
HRESULT hResult = S_OK;
stream->captureClientStream = NULL;
stream->in.clientStream = NULL;
stream->renderClientStream = NULL;
stream->out.clientStream = NULL;
if (NULL != stream->in.clientParent)
{
hResult = MarshalSubStreamComPointers(&stream->in);
if (hResult != S_OK)
goto marshal_error;
hResult = CoMarshalInterThreadInterfaceInStream(&pa_IID_IAudioCaptureClient, (LPUNKNOWN)stream->captureClientParent, &stream->captureClientStream);
if (hResult != S_OK)
goto marshal_error;
}
if (NULL != stream->out.clientParent)
{
hResult = MarshalSubStreamComPointers(&stream->out);
if (hResult != S_OK)
goto marshal_error;
hResult = CoMarshalInterThreadInterfaceInStream(&pa_IID_IAudioRenderClient, (LPUNKNOWN)stream->renderClientParent, &stream->renderClientStream);
if (hResult != S_OK)
goto marshal_error;
}
return hResult;
marshal_error:
UnmarshalStreamComPointers(stream);
ReleaseUnmarshaledComPointers(stream);
return hResult;
#else
(void)stream;
return S_OK;
#endif
}
static PaError StartStream( PaStream *s )
{
HRESULT hr;
PaWasapiStream *stream = (PaWasapiStream*)s;
PaError result = paNoError;
if (IsStreamActive(s))
return paStreamIsNotStopped;
PaUtil_ResetBufferProcessor(&stream->bufferProcessor);
_StreamCleanup(stream);
if ((stream->hCloseRequest = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
{
result = paInsufficientMemory;
goto start_error;
}
if (!stream->bBlocking)
{
stream->hThreadStart = CreateEvent(NULL, TRUE, FALSE, NULL);
stream->hThreadExit  = CreateEvent(NULL, TRUE, FALSE, NULL);
if ((stream->hThreadStart == NULL) || (stream->hThreadExit == NULL))
{
result = paInsufficientMemory;
goto start_error;
}
if ((hr = MarshalStreamComPointers(stream)) != S_OK)
{
PRINT(("Failed marshaling stream COM pointers."));
result = paUnanticipatedHostError;
goto nonblocking_start_error;
}
if ((stream->in.clientParent  && (stream->in.streamFlags  & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)) ||
(stream->out.clientParent && (stream->out.streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)))
{
if ((stream->hThread = CREATE_THREAD(ProcThreadEvent)) == NULL)
{
PRINT(("Failed creating thread: ProcThreadEvent."));
result = paUnanticipatedHostError;
goto nonblocking_start_error;
}
}
else
{
if ((stream->hThread = CREATE_THREAD(ProcThreadPoll)) == NULL)
{
PRINT(("Failed creating thread: ProcThreadPoll."));
result = paUnanticipatedHostError;
goto nonblocking_start_error;
}
}
if (WaitForSingleObject(stream->hThreadStart, 60*1000) == WAIT_TIMEOUT)
{
PRINT(("Failed starting thread: timeout."));
result = paUnanticipatedHostError;
goto nonblocking_start_error;
}
}
else
{
if (stream->out.clientParent != NULL)
{
if ((stream->hBlockingOpStreamWR = CreateEvent(NULL, TRUE, TRUE, NULL)) == NULL)
{
result = paInsufficientMemory;
goto start_error;
}
}
if (stream->in.clientParent != NULL)
{
if ((stream->hBlockingOpStreamRD = CreateEvent(NULL, TRUE, TRUE, NULL)) == NULL)
{
result = paInsufficientMemory;
goto start_error;
}
}
if (stream->in.clientParent != NULL)
{
if ((hr = IAudioClient_Start(stream->in.clientParent)) != S_OK)
{
LogHostError(hr);
result = paUnanticipatedHostError;
goto start_error;
}
}
if (stream->out.clientParent != NULL)
{
if ((hr = IAudioClient_Start(stream->out.clientParent)) != S_OK)
{
LogHostError(hr);
result = paUnanticipatedHostError;
goto start_error;
}
}
stream->captureClient  = stream->captureClientParent;
stream->renderClient   = stream->renderClientParent;
stream->in.clientProc  = stream->in.clientParent;
stream->out.clientProc = stream->out.clientParent;
stream->running = TRUE;
}
return result;
nonblocking_start_error:
SetEvent(stream->hThreadExit);
UnmarshalStreamComPointers(stream);
ReleaseUnmarshaledComPointers(stream);
start_error:
StopStream(s);
return result;
}
void _StreamFinish(PaWasapiStream *stream)
{
if (!stream->bBlocking)
{
SignalObjectAndWait(stream->hCloseRequest, stream->hThreadExit, INFINITE, FALSE);
}
else
{
if (stream->out.clientParent)
SignalObjectAndWait(stream->hCloseRequest, stream->hBlockingOpStreamWR, INFINITE, TRUE);
if (stream->out.clientParent)
SignalObjectAndWait(stream->hCloseRequest, stream->hBlockingOpStreamRD, INFINITE, TRUE);
_StreamOnStop(stream);
}
_StreamCleanup(stream);
stream->running = FALSE;
}
void _StreamCleanup(PaWasapiStream *stream)
{
SAFE_CLOSE(stream->hThread);
SAFE_CLOSE(stream->hThreadStart);
SAFE_CLOSE(stream->hThreadExit);
SAFE_CLOSE(stream->hCloseRequest);
SAFE_CLOSE(stream->hBlockingOpStreamRD);
SAFE_CLOSE(stream->hBlockingOpStreamWR);
}
static PaError StopStream( PaStream *s )
{
_StreamFinish((PaWasapiStream *)s);
return paNoError;
}
static PaError AbortStream( PaStream *s )
{
_StreamFinish((PaWasapiStream *)s);
return paNoError;
}
static PaError IsStreamStopped( PaStream *s )
{
return !((PaWasapiStream *)s)->running;
}
static PaError IsStreamActive( PaStream *s )
{
return ((PaWasapiStream *)s)->running;
}
static PaTime GetStreamTime( PaStream *s )
{
PaWasapiStream *stream = (PaWasapiStream*)s;
(void) stream;
return PaUtil_GetTime();
}
static double GetStreamCpuLoad( PaStream* s )
{
return PaUtil_GetCpuLoad(&((PaWasapiStream *)s)->cpuLoadMeasurer);
}
static PaError ReadStream( PaStream* s, void *_buffer, unsigned long frames )
{
PaWasapiStream *stream = (PaWasapiStream*)s;
HRESULT hr = S_OK;
BYTE *user_buffer = (BYTE *)_buffer;
BYTE *wasapi_buffer = NULL;
DWORD flags = 0;
UINT32 i, available, sleep = 0;
unsigned long processed;
ThreadIdleScheduler sched;
if (!stream->running)
return paStreamIsStopped;
if (stream->captureClient == NULL)
return paBadStreamPtr;
ResetEvent(stream->hBlockingOpStreamRD);
ThreadIdleScheduler_Setup(&sched, 1, 250 );
if (!stream->bufferProcessor.userInputIsInterleaved)
{
user_buffer = (BYTE *)alloca(sizeof(BYTE *) * stream->bufferProcessor.inputChannelCount);
if (user_buffer == NULL)
return paInsufficientMemory;
for (i = 0; i < stream->bufferProcessor.inputChannelCount; ++i)
((BYTE **)user_buffer)[i] = ((BYTE **)_buffer)[i];
}
if ((available = PaUtil_GetRingBufferReadAvailable(stream->in.tailBuffer)) != 0)
{
ring_buffer_size_t buf1_size = 0, buf2_size = 0, read, desired;
void *buf1 = NULL, *buf2 = NULL;
desired = available;
if ((UINT32)desired > frames)
desired = frames;
read = PaUtil_GetRingBufferReadRegions(stream->in.tailBuffer, desired, &buf1, &buf1_size, &buf2, &buf2_size);
if (buf1 != NULL)
{
PaUtil_SetInputFrameCount(&stream->bufferProcessor, buf1_size);
PaUtil_SetInterleavedInputChannels(&stream->bufferProcessor, 0, buf1, stream->bufferProcessor.inputChannelCount);
processed = PaUtil_CopyInput(&stream->bufferProcessor, (void **)&user_buffer, buf1_size);
frames -= processed;
}
if (buf2 != NULL)
{
PaUtil_SetInputFrameCount(&stream->bufferProcessor, buf2_size);
PaUtil_SetInterleavedInputChannels(&stream->bufferProcessor, 0, buf2, stream->bufferProcessor.inputChannelCount);
processed = PaUtil_CopyInput(&stream->bufferProcessor, (void **)&user_buffer, buf2_size);
frames -= processed;
}
PaUtil_AdvanceRingBufferReadIndex(stream->in.tailBuffer, read);
}
while (frames != 0)
{
if (WaitForSingleObject(stream->hCloseRequest, sleep) != WAIT_TIMEOUT)
break;
if ((hr = _PollGetInputFramesAvailable(stream, &available)) != S_OK)
{
LogHostError(hr);
return paUnanticipatedHostError;
}
if (available == 0)
{
if (stream->in.shareMode != AUDCLNT_SHAREMODE_EXCLUSIVE)
{
UINT32 sleep_frames = (frames < stream->in.framesPerHostCallback ? frames : stream->in.framesPerHostCallback);
sleep  = GetFramesSleepTime(sleep_frames, stream->in.wavex.Format.nSamplesPerSec);
sleep /= 4;  
if (sleep > 2)
sleep = 2;
if (sleep == 0)
sleep = ThreadIdleScheduler_NextSleep(&sched);
}
else
{
if ((sleep = ThreadIdleScheduler_NextSleep(&sched)) != 0)
{
Sleep(sleep);
sleep = 0;
}
}
continue;
}
if ((hr = IAudioCaptureClient_GetBuffer(stream->captureClient, &wasapi_buffer, &available, &flags, NULL, NULL)) != S_OK)
{
if (hr != AUDCLNT_S_BUFFER_EMPTY)
{
LogHostError(hr);
goto end;
}
continue;
}
PaUtil_SetInputFrameCount(&stream->bufferProcessor, available);
PaUtil_SetInterleavedInputChannels(&stream->bufferProcessor, 0, wasapi_buffer, stream->bufferProcessor.inputChannelCount);
processed = PaUtil_CopyInput(&stream->bufferProcessor, (void **)&user_buffer, frames);
frames -= processed;
if ((frames == 0) && (available > processed))
{
UINT32 bytes_processed = processed * stream->in.wavex.Format.nBlockAlign;
UINT32 frames_to_save  = available - processed;
PaUtil_WriteRingBuffer(stream->in.tailBuffer, wasapi_buffer + bytes_processed, frames_to_save);
}
if ((hr = IAudioCaptureClient_ReleaseBuffer(stream->captureClient, available)) != S_OK)
{
LogHostError(hr);
goto end;
}
}
end:
SetEvent(stream->hBlockingOpStreamRD);
return (hr != S_OK ? paUnanticipatedHostError : paNoError);
}
static PaError WriteStream( PaStream* s, const void *_buffer, unsigned long frames )
{
PaWasapiStream *stream = (PaWasapiStream*)s;
const BYTE *user_buffer = (const BYTE *)_buffer;
BYTE *wasapi_buffer;
HRESULT hr = S_OK;
UINT32 i, available, sleep = 0;
unsigned long processed;
ThreadIdleScheduler sched;
if (!stream->running)
return paStreamIsStopped;
if (stream->renderClient == NULL)
return paBadStreamPtr;
ResetEvent(stream->hBlockingOpStreamWR);
ThreadIdleScheduler_Setup(&sched, 1, 500 );
if (!stream->bufferProcessor.userOutputIsInterleaved)
{
user_buffer = (const BYTE *)alloca(sizeof(const BYTE *) * stream->bufferProcessor.outputChannelCount);
if (user_buffer == NULL)
return paInsufficientMemory;
for (i = 0; i < stream->bufferProcessor.outputChannelCount; ++i)
((const BYTE **)user_buffer)[i] = ((const BYTE **)_buffer)[i];
}
while (frames != 0)
{
if (WaitForSingleObject(stream->hCloseRequest, sleep) != WAIT_TIMEOUT)
break;
if ((hr = _PollGetOutputFramesAvailable(stream, &available)) != S_OK)
{
LogHostError(hr);
goto end;
}
if (available == 0)
{
UINT32 sleep_frames = (frames < stream->out.framesPerHostCallback ? frames : stream->out.framesPerHostCallback);
sleep  = GetFramesSleepTime(sleep_frames, stream->out.wavex.Format.nSamplesPerSec);
sleep /= 2;  
if (sleep == 0)
sleep = ThreadIdleScheduler_NextSleep(&sched);
continue;
}
if (available > frames)
available = frames;
if ((hr = IAudioRenderClient_GetBuffer(stream->renderClient, available, &wasapi_buffer)) != S_OK)
{
if (hr == AUDCLNT_E_BUFFER_TOO_LARGE)
continue;
LogHostError(hr);
goto end;
}
if (wasapi_buffer == NULL)
continue;
PaUtil_SetOutputFrameCount(&stream->bufferProcessor, available);
PaUtil_SetInterleavedOutputChannels(&stream->bufferProcessor, 0, wasapi_buffer,    stream->bufferProcessor.outputChannelCount);
processed = PaUtil_CopyOutput(&stream->bufferProcessor, (const void **)&user_buffer, frames);
frames -= processed;
if ((hr = IAudioRenderClient_ReleaseBuffer(stream->renderClient, available, 0)) != S_OK)
{
LogHostError(hr);
goto end;
}
}
end:
SetEvent(stream->hBlockingOpStreamWR);
return (hr != S_OK ? paUnanticipatedHostError : paNoError);
}
unsigned long PaUtil_GetOutputFrameCount( PaUtilBufferProcessor* bp )
{
return bp->hostOutputFrameCount[0];
}
static signed long GetStreamReadAvailable( PaStream* s )
{
PaWasapiStream *stream = (PaWasapiStream*)s;
HRESULT hr;
UINT32  available = 0;
if (!stream->running)
return paStreamIsStopped;
if (stream->captureClient == NULL)
return paBadStreamPtr;
if ((hr = _PollGetInputFramesAvailable(stream, &available)) != S_OK)
{
LogHostError(hr);
return paUnanticipatedHostError;
}
available += PaUtil_GetRingBufferReadAvailable(stream->in.tailBuffer);
return available;
}
static signed long GetStreamWriteAvailable( PaStream* s )
{
PaWasapiStream *stream = (PaWasapiStream*)s;
HRESULT hr;
UINT32  available = 0;
if (!stream->running)
return paStreamIsStopped;
if (stream->renderClient == NULL)
return paBadStreamPtr;
if ((hr = _PollGetOutputFramesAvailable(stream, &available)) != S_OK)
{
LogHostError(hr);
return paUnanticipatedHostError;
}
return (signed long)available;
}
static void WaspiHostProcessingLoop( void *inputBuffer,  long inputFrames,
void *outputBuffer, long outputFrames,
void *userData )
{
PaWasapiStream *stream = (PaWasapiStream*)userData;
PaStreamCallbackTimeInfo timeInfo = {0,0,0};
PaStreamCallbackFlags flags = 0;
int callbackResult;
unsigned long framesProcessed;
HRESULT hr;
UINT32 pending;
PaUtil_BeginCpuLoadMeasurement( &stream->cpuLoadMeasurer );
timeInfo.currentTime = PaUtil_GetTime();
if (stream->in.clientProc != NULL)
{
PaTime pending_time;
if ((hr = IAudioClient_GetCurrentPadding(stream->in.clientProc, &pending)) == S_OK)
pending_time = (PaTime)pending / (PaTime)stream->in.wavex.Format.nSamplesPerSec;
else
pending_time = (PaTime)stream->in.latencySeconds;
timeInfo.inputBufferAdcTime = timeInfo.currentTime + pending_time;
}
if (stream->out.clientProc != NULL)
{
PaTime pending_time;
if ((hr = IAudioClient_GetCurrentPadding(stream->out.clientProc, &pending)) == S_OK)
pending_time = (PaTime)pending / (PaTime)stream->out.wavex.Format.nSamplesPerSec;
else
pending_time = (PaTime)stream->out.latencySeconds;
timeInfo.outputBufferDacTime = timeInfo.currentTime + pending_time;
}
PaUtil_BeginBufferProcessing( &stream->bufferProcessor, &timeInfo, flags );
if (stream->bufferProcessor.inputChannelCount > 0)
{
PaUtil_SetInputFrameCount( &stream->bufferProcessor, inputFrames );
PaUtil_SetInterleavedInputChannels( &stream->bufferProcessor,
0,  
inputBuffer,
0 );  
}
if (stream->bufferProcessor.outputChannelCount > 0)
{
PaUtil_SetOutputFrameCount( &stream->bufferProcessor, outputFrames);
PaUtil_SetInterleavedOutputChannels( &stream->bufferProcessor,
0,  
outputBuffer,
0 );  
}
callbackResult = paContinue;
framesProcessed = PaUtil_EndBufferProcessing( &stream->bufferProcessor, &callbackResult );
PaUtil_EndCpuLoadMeasurement( &stream->cpuLoadMeasurer, framesProcessed );
if (callbackResult == paContinue)
{
}
else
if (callbackResult == paAbort)
{
SetEvent(stream->hCloseRequest);
}
else
{
SetEvent(stream->hCloseRequest);
}
}
#ifndef PA_WINRT
static PaError MMCSS_activate(PaWasapiThreadPriority nPriorityClass, HANDLE *ret)
{
static const char *mmcs_name[] =
{
NULL,
"Audio",
"Capture",
"Distribution",
"Games",
"Playback",
"Pro Audio",
"Window Manager"
};
DWORD task_idx = 0;
HANDLE hTask;
if ((UINT32)nPriorityClass >= STATIC_ARRAY_SIZE(mmcs_name))
return paUnanticipatedHostError;
if ((hTask = pAvSetMmThreadCharacteristics(mmcs_name[nPriorityClass], &task_idx)) == NULL)
{
PRINT(("WASAPI: AvSetMmThreadCharacteristics failed: error[%d]\n", GetLastError()));
return paUnanticipatedHostError;
}
{
int    cur_priority          = GetThreadPriority(GetCurrentThread());
DWORD  cur_priority_class = GetPriorityClass(GetCurrentProcess());
PRINT(("WASAPI: thread[ priority-0x%X class-0x%X ]\n", cur_priority, cur_priority_class));
}
(*ret) = hTask;
return paNoError;
}
#endif
#ifndef PA_WINRT
static void MMCSS_deactivate(HANDLE hTask)
{
if (pAvRevertMmThreadCharacteristics(hTask) == FALSE)
{
PRINT(("WASAPI: AvRevertMmThreadCharacteristics failed!\n"));
}
}
#endif
PaError PaWasapi_ThreadPriorityBoost(void **pTask, PaWasapiThreadPriority priorityClass)
{
HANDLE task;
PaError ret;
if (pTask == NULL)
return paUnanticipatedHostError;
#ifndef PA_WINRT
if ((ret = MMCSS_activate(priorityClass, &task)) != paNoError)
return ret;
#else
switch (priorityClass)
{
case eThreadPriorityAudio:
case eThreadPriorityProAudio: {
intptr_t priority_prev = GetThreadPriority(GetCurrentThread());
if (SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST) == FALSE)
return paUnanticipatedHostError;
task = (HANDLE)(priority_prev | 0x80000000);
ret = paNoError;
break; }
default:
return paUnanticipatedHostError;
}
#endif
(*pTask) = task;
return ret;
}
PaError PaWasapi_ThreadPriorityRevert(void *pTask)
{
if (pTask == NULL)
return paUnanticipatedHostError;
#ifndef PA_WINRT
MMCSS_deactivate((HANDLE)pTask);
#else
if (SetThreadPriority(GetCurrentThread(), (int)((intptr_t)pTask & ~0x80000000)) == FALSE)
return paUnanticipatedHostError;
#endif
return paNoError;
}
PaError PaWasapi_GetJackCount(PaDeviceIndex device, int *pJackCount)
{
#ifndef PA_WINRT
PaError ret;
HRESULT hr = S_OK;
PaWasapiDeviceInfo *deviceInfo;
IDeviceTopology *pDeviceTopology = NULL;
IConnector *pConnFrom = NULL;
IConnector *pConnTo = NULL;
IPart *pPart = NULL;
IKsJackDescription *pJackDesc = NULL;
UINT jackCount = 0;
if (pJackCount == NULL)
return paUnanticipatedHostError;
if ((ret = _GetWasapiDeviceInfoByDeviceIndex(&deviceInfo, device)) != paNoError)
return ret;
hr = IMMDevice_Activate(deviceInfo->device, &pa_IID_IDeviceTopology,
CLSCTX_INPROC_SERVER, NULL, (void**)&pDeviceTopology);
IF_FAILED_JUMP(hr, error);
hr = IDeviceTopology_GetConnector(pDeviceTopology, 0, &pConnFrom);
IF_FAILED_JUMP(hr, error);
hr = IConnector_GetConnectedTo(pConnFrom, &pConnTo);
if (HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND) == hr)
{
hr = E_NOINTERFACE;
}
IF_FAILED_JUMP(hr, error);
hr = IConnector_QueryInterface(pConnTo, &pa_IID_IPart, (void**)&pPart);
IF_FAILED_JUMP(hr, error);
hr = IPart_Activate(pPart, CLSCTX_INPROC_SERVER, &pa_IID_IKsJackDescription, (void**)&pJackDesc);
IF_FAILED_JUMP(hr, error);
hr = IKsJackDescription_GetJackCount(pJackDesc, &jackCount);
IF_FAILED_JUMP(hr, error);
(*pJackCount) = jackCount;
ret = paNoError;
error:
SAFE_RELEASE(pDeviceTopology);
SAFE_RELEASE(pConnFrom);
SAFE_RELEASE(pConnTo);
SAFE_RELEASE(pPart);
SAFE_RELEASE(pJackDesc);
LogHostError(hr);
return paNoError;
#else
(void)device;
(void)pJackCount;
return paUnanticipatedHostError;
#endif
}
#ifndef PA_WINRT
static PaWasapiJackConnectionType _ConvertJackConnectionTypeWASAPIToPA(int connType)
{
switch (connType)
{
case eConnTypeUnknown:               return eJackConnTypeUnknown;
#ifdef _KS_
case eConnType3Point5mm:             return eJackConnType3Point5mm;
#else
case eConnTypeEighth:                return eJackConnType3Point5mm;
#endif
case eConnTypeQuarter:               return eJackConnTypeQuarter;
case eConnTypeAtapiInternal:         return eJackConnTypeAtapiInternal;
case eConnTypeRCA:                   return eJackConnTypeRCA;
case eConnTypeOptical:               return eJackConnTypeOptical;
case eConnTypeOtherDigital:          return eJackConnTypeOtherDigital;
case eConnTypeOtherAnalog:           return eJackConnTypeOtherAnalog;
case eConnTypeMultichannelAnalogDIN: return eJackConnTypeMultichannelAnalogDIN;
case eConnTypeXlrProfessional:       return eJackConnTypeXlrProfessional;
case eConnTypeRJ11Modem:             return eJackConnTypeRJ11Modem;
case eConnTypeCombination:           return eJackConnTypeCombination;
}
return eJackConnTypeUnknown;
}
#endif
#ifndef PA_WINRT
static PaWasapiJackGeoLocation _ConvertJackGeoLocationWASAPIToPA(int geoLoc)
{
switch (geoLoc)
{
case eGeoLocRear:             return eJackGeoLocRear;
case eGeoLocFront:            return eJackGeoLocFront;
case eGeoLocLeft:             return eJackGeoLocLeft;
case eGeoLocRight:            return eJackGeoLocRight;
case eGeoLocTop:              return eJackGeoLocTop;
case eGeoLocBottom:           return eJackGeoLocBottom;
#ifdef _KS_
case eGeoLocRearPanel:        return eJackGeoLocRearPanel;
#else
case eGeoLocRearOPanel:       return eJackGeoLocRearPanel;
#endif
case eGeoLocRiser:            return eJackGeoLocRiser;
case eGeoLocInsideMobileLid:  return eJackGeoLocInsideMobileLid;
case eGeoLocDrivebay:         return eJackGeoLocDrivebay;
case eGeoLocHDMI:             return eJackGeoLocHDMI;
case eGeoLocOutsideMobileLid: return eJackGeoLocOutsideMobileLid;
case eGeoLocATAPI:            return eJackGeoLocATAPI;
}
return eJackGeoLocUnk;
}
#endif
#ifndef PA_WINRT
static PaWasapiJackGenLocation _ConvertJackGenLocationWASAPIToPA(int genLoc)
{
switch (genLoc)
{
case eGenLocPrimaryBox: return eJackGenLocPrimaryBox;
case eGenLocInternal:   return eJackGenLocInternal;
#ifdef _KS_
case eGenLocSeparate:   return eJackGenLocSeparate;
#else
case eGenLocSeperate:   return eJackGenLocSeparate;
#endif
case eGenLocOther:      return eJackGenLocOther;
}
return eJackGenLocPrimaryBox;
}
#endif
#ifndef PA_WINRT
static PaWasapiJackPortConnection _ConvertJackPortConnectionWASAPIToPA(int portConn)
{
switch (portConn)
{
case ePortConnJack:                  return eJackPortConnJack;
case ePortConnIntegratedDevice:      return eJackPortConnIntegratedDevice;
case ePortConnBothIntegratedAndJack: return eJackPortConnBothIntegratedAndJack;
case ePortConnUnknown:               return eJackPortConnUnknown;
}
return eJackPortConnJack;
}
#endif
PaError PaWasapi_GetJackDescription(PaDeviceIndex device, int jackIndex, PaWasapiJackDescription *pJackDescription)
{
#ifndef PA_WINRT
PaError ret;
HRESULT hr = S_OK;
PaWasapiDeviceInfo *deviceInfo;
IDeviceTopology *pDeviceTopology = NULL;
IConnector *pConnFrom = NULL;
IConnector *pConnTo = NULL;
IPart *pPart = NULL;
IKsJackDescription *pJackDesc = NULL;
KSJACK_DESCRIPTION jack = { 0 };
if ((ret = _GetWasapiDeviceInfoByDeviceIndex(&deviceInfo, device)) != paNoError)
return ret;
hr = IMMDevice_Activate(deviceInfo->device, &pa_IID_IDeviceTopology,
CLSCTX_INPROC_SERVER, NULL, (void**)&pDeviceTopology);
IF_FAILED_JUMP(hr, error);
hr = IDeviceTopology_GetConnector(pDeviceTopology, 0, &pConnFrom);
IF_FAILED_JUMP(hr, error);
hr = IConnector_GetConnectedTo(pConnFrom, &pConnTo);
if (HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND) == hr)
{
hr = E_NOINTERFACE;
}
IF_FAILED_JUMP(hr, error);
hr = IConnector_QueryInterface(pConnTo, &pa_IID_IPart, (void**)&pPart);
IF_FAILED_JUMP(hr, error);
hr = IPart_Activate(pPart, CLSCTX_INPROC_SERVER, &pa_IID_IKsJackDescription, (void**)&pJackDesc);
IF_FAILED_JUMP(hr, error);
hr = IKsJackDescription_GetJackDescription(pJackDesc, jackIndex, &jack);
IF_FAILED_JUMP(hr, error);
pJackDescription->channelMapping = jack.ChannelMapping;
pJackDescription->color          = jack.Color;
pJackDescription->connectionType = _ConvertJackConnectionTypeWASAPIToPA(jack.ConnectionType);
pJackDescription->genLocation    = _ConvertJackGenLocationWASAPIToPA(jack.GenLocation);
pJackDescription->geoLocation    = _ConvertJackGeoLocationWASAPIToPA(jack.GeoLocation);
pJackDescription->isConnected    = jack.IsConnected;
pJackDescription->portConnection = _ConvertJackPortConnectionWASAPIToPA(jack.PortConnection);
ret = paNoError;
error:
SAFE_RELEASE(pDeviceTopology);
SAFE_RELEASE(pConnFrom);
SAFE_RELEASE(pConnTo);
SAFE_RELEASE(pPart);
SAFE_RELEASE(pJackDesc);
LogHostError(hr);
return ret;
#else
(void)device;
(void)jackIndex;
(void)pJackDescription;
return paUnanticipatedHostError;
#endif
}
PaError PaWasapi_GetAudioClient(PaStream *pStream, void **pAudioClient, int bOutput)
{
PaWasapiStream *stream = (PaWasapiStream *)pStream;
if (stream == NULL)
return paBadStreamPtr;
if (pAudioClient == NULL)
return paUnanticipatedHostError;
(*pAudioClient) = (bOutput == TRUE ? stream->out.clientParent : stream->in.clientParent);
return paNoError;
}
#ifdef PA_WINRT
static void CopyNameOrIdString(WCHAR *dst, const UINT32 dstMaxCount, const WCHAR *src)
{
UINT32 i;
for (i = 0; i < dstMaxCount; ++i)
dst[i] = 0;
if (src != NULL)
{
for (i = 0; (src[i] != 0) && (i < dstMaxCount); ++i)
dst[i] = src[i];
}
}
#endif
PaError PaWasapiWinrt_SetDefaultDeviceId( const unsigned short *pId, int bOutput )
{
#ifdef PA_WINRT
INT32 i;
PaWasapiWinrtDeviceListRole *role = (bOutput ? &g_DeviceListInfo.render : &g_DeviceListInfo.capture);
assert(STATIC_ARRAY_SIZE(role->defaultId) == PA_WASAPI_DEVICE_ID_LEN);
if (pId != NULL)
{
for (i = 0; pId[i] != 0; ++i)
{
if (i >= PA_WASAPI_DEVICE_ID_LEN)
return paBufferTooBig;
}
}
CopyNameOrIdString(role->defaultId, STATIC_ARRAY_SIZE(role->defaultId), pId);
return paNoError;
#else
return paIncompatibleStreamHostApi;
#endif
}
PaError PaWasapiWinrt_PopulateDeviceList( const unsigned short **pId, const unsigned short **pName,
const PaWasapiDeviceRole *pRole, unsigned int count, int bOutput )
{
#ifdef PA_WINRT
UINT32 i, j;
PaWasapiWinrtDeviceListRole *role = (bOutput ? &g_DeviceListInfo.render : &g_DeviceListInfo.capture);
memset(&role->devices, 0, sizeof(role->devices));
role->deviceCount = 0;
if (count == 0)
return paNoError;
else
if (count > PA_WASAPI_DEVICE_MAX_COUNT)
return paBufferTooBig;
if (pId == NULL)
return paInsufficientMemory;
for (i = 0; i < count; ++i)
{
const unsigned short *id = pId[i];
const unsigned short *name = pName[i];
for (j = 0; id[j] != 0; ++j)
{
if (j >= PA_WASAPI_DEVICE_ID_LEN)
return paBufferTooBig;
}
for (j = 0; name[j] != 0; ++j)
{
if (j >= PA_WASAPI_DEVICE_NAME_LEN)
return paBufferTooBig;
}
}
for (i = 0; i < count; ++i)
{
CopyNameOrIdString(role->devices[i].id, STATIC_ARRAY_SIZE(role->devices[i].id), pId[i]);
CopyNameOrIdString(role->devices[i].name, STATIC_ARRAY_SIZE(role->devices[i].name), pName[i]);
role->devices[i].formFactor = (pRole != NULL ? (EndpointFormFactor)pRole[i] : UnknownFormFactor);
role->deviceCount += (role->devices[i].id[0] != 0);
}
return paNoError;
#else
return paIncompatibleStreamHostApi;
#endif
}
PaError PaWasapi_SetStreamStateHandler( PaStream *pStream, PaWasapiStreamStateCallback fnStateHandler, void *pUserData )
{
PaWasapiStream *stream = (PaWasapiStream *)pStream;
if (stream == NULL)
return paBadStreamPtr;
stream->fnStateHandler        = fnStateHandler;
stream->pStateHandlerUserData = pUserData;
return paNoError;
}
HRESULT _PollGetOutputFramesAvailable(PaWasapiStream *stream, UINT32 *available)
{
HRESULT hr;
UINT32 frames  = stream->out.framesPerHostCallback,
padding = 0;
(*available) = 0;
if ((hr = IAudioClient_GetCurrentPadding(stream->out.clientProc, &padding)) != S_OK)
return LogHostError(hr);
frames -= padding;
(*available) = frames;
return hr;
}
HRESULT _PollGetInputFramesAvailable(PaWasapiStream *stream, UINT32 *available)
{
HRESULT hr;
(*available) = 0;
if ((hr = IAudioClient_GetCurrentPadding(stream->in.clientProc, available)) != S_OK)
return LogHostError(hr);
return hr;
}
static HRESULT ProcessOutputBuffer(PaWasapiStream *stream, PaWasapiHostProcessor *processor, UINT32 frames)
{
HRESULT hr;
BYTE *data = NULL;
if ((hr = IAudioRenderClient_GetBuffer(stream->renderClient, frames, &data)) != S_OK)
{
#if 0
if (stream->out.shareMode == AUDCLNT_SHAREMODE_SHARED)
{
#if 0
UINT32 padding = 0;
hr = IAudioClient_GetCurrentPadding(stream->out.clientProc, &padding);
if (hr != S_OK)
return LogHostError(hr);
frames -= padding;
if (frames == 0)
return S_OK;
if ((hr = IAudioRenderClient_GetBuffer(stream->renderClient, frames, &data)) != S_OK)
return LogHostError(hr);
#else
if (hr == AUDCLNT_E_BUFFER_TOO_LARGE)
return S_OK;  
#endif
}
else
return LogHostError(hr);
#else
if (hr == AUDCLNT_E_BUFFER_TOO_LARGE)
return S_OK;  
return LogHostError(hr);
#endif
}
if (stream->out.monoMixer != NULL)
{
UINT32 mono_frames_size = frames * (stream->out.wavex.Format.wBitsPerSample / 8);
if (mono_frames_size > stream->out.monoBufferSize)
{
stream->out.monoBuffer = PaWasapi_ReallocateMemory(stream->out.monoBuffer, (stream->out.monoBufferSize = mono_frames_size));
if (stream->out.monoBuffer == NULL)
{
hr = E_OUTOFMEMORY;
LogHostError(hr);
return hr;
}
}
processor[S_OUTPUT].processor(NULL, 0, (BYTE *)stream->out.monoBuffer, frames, processor[S_OUTPUT].userData);
stream->out.monoMixer(data, stream->out.monoBuffer, frames);
}
else
{
processor[S_OUTPUT].processor(NULL, 0, data, frames, processor[S_OUTPUT].userData);
}
if ((hr = IAudioRenderClient_ReleaseBuffer(stream->renderClient, frames, 0)) != S_OK)
LogHostError(hr);
return hr;
}
static HRESULT ProcessInputBuffer(PaWasapiStream *stream, PaWasapiHostProcessor *processor)
{
HRESULT hr = S_OK;
UINT32 frames;
BYTE *data = NULL;
DWORD flags = 0;
for (;;)
{
if (WaitForSingleObject(stream->hCloseRequest, 0) != WAIT_TIMEOUT)
break;
frames = 0;
if ((hr = _PollGetInputFramesAvailable(stream, &frames)) != S_OK)
return hr;
if (frames == 0)
break;
if ((hr = IAudioCaptureClient_GetBuffer(stream->captureClient, &data, &frames, &flags, NULL, NULL)) != S_OK)
{
if (hr == AUDCLNT_S_BUFFER_EMPTY)
{
hr = S_OK;
break;  
}
return LogHostError(hr);
break;
}
if (stream->in.monoMixer != NULL)
{
UINT32 mono_frames_size = frames * (stream->in.wavex.Format.wBitsPerSample / 8);
if (mono_frames_size > stream->in.monoBufferSize)
{
stream->in.monoBuffer = PaWasapi_ReallocateMemory(stream->in.monoBuffer, (stream->in.monoBufferSize = mono_frames_size));
if (stream->in.monoBuffer == NULL)
{
hr = E_OUTOFMEMORY;
LogHostError(hr);
return hr;
}
}
stream->in.monoMixer(stream->in.monoBuffer, data, frames);
processor[S_INPUT].processor((BYTE *)stream->in.monoBuffer, frames, NULL, 0, processor[S_INPUT].userData);
}
else
{
processor[S_INPUT].processor(data, frames, NULL, 0, processor[S_INPUT].userData);
}
if ((hr = IAudioCaptureClient_ReleaseBuffer(stream->captureClient, frames)) != S_OK)
return LogHostError(hr);
}
return hr;
}
void _StreamOnStop(PaWasapiStream *stream)
{
if (!stream->bBlocking)
{
if (stream->in.clientProc != NULL)
IAudioClient_Stop(stream->in.clientProc);
if (stream->out.clientProc != NULL)
IAudioClient_Stop(stream->out.clientProc);
}
else
{
if (stream->in.clientParent != NULL)
IAudioClient_Stop(stream->in.clientParent);
if (stream->out.clientParent != NULL)
IAudioClient_Stop(stream->out.clientParent);
}
if (stream->hAvTask != NULL)
{
PaWasapi_ThreadPriorityRevert(stream->hAvTask);
stream->hAvTask = NULL;
}
if (stream->streamRepresentation.streamFinishedCallback != NULL)
stream->streamRepresentation.streamFinishedCallback(stream->streamRepresentation.userData);
}
static BOOL PrepareComPointers(PaWasapiStream *stream, BOOL *threadComInitialized)
{
HRESULT hr;
hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
if (FAILED(hr) && (hr != RPC_E_CHANGED_MODE))
{
PRINT(("WASAPI: failed ProcThreadEvent CoInitialize"));
return FALSE;
}
if (hr != RPC_E_CHANGED_MODE)
*threadComInitialized = TRUE;
hr = UnmarshalStreamComPointers(stream);
if (hr != S_OK)
{
PRINT(("WASAPI: Error unmarshaling stream COM pointers. HRESULT: %i\n", hr));
CoUninitialize();
return FALSE;
}
return TRUE;
}
static void FinishComPointers(PaWasapiStream *stream, BOOL threadComInitialized)
{
ReleaseUnmarshaledComPointers(stream);
if (threadComInitialized == TRUE)
CoUninitialize();
}
PA_THREAD_FUNC ProcThreadEvent(void *param)
{
PaWasapiHostProcessor processor[S_COUNT];
HRESULT hr = S_OK;
DWORD dwResult;
PaWasapiStream *stream = (PaWasapiStream *)param;
PaWasapiHostProcessor defaultProcessor;
BOOL setEvent[S_COUNT] = { FALSE, FALSE };
BOOL waitAllEvents = FALSE;
BOOL threadComInitialized = FALSE;
SystemTimer timer;
NotifyStateChanged(stream, paWasapiStreamStateThreadPrepare, ERROR_SUCCESS);
if (!PrepareComPointers(stream, &threadComInitialized))
return (UINT32)paUnanticipatedHostError;
SystemTimer_SetGranularity(&timer, 1);
if ((stream->in.clientProc != NULL) && (stream->out.clientProc != NULL))
{
waitAllEvents = (stream->in.shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE) &&
(stream->out.shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE);
}
defaultProcessor.processor = WaspiHostProcessingLoop;
defaultProcessor.userData  = stream;
processor[S_INPUT] = (stream->hostProcessOverrideInput.processor != NULL ? stream->hostProcessOverrideInput : defaultProcessor);
processor[S_OUTPUT] = (stream->hostProcessOverrideOutput.processor != NULL ? stream->hostProcessOverrideOutput : defaultProcessor);
PaWasapi_ThreadPriorityBoost((void **)&stream->hAvTask, stream->nThreadPriority);
if (stream->event[S_OUTPUT] == NULL)
{
stream->event[S_OUTPUT] = CreateEvent(NULL, FALSE, FALSE, NULL);
setEvent[S_OUTPUT] = TRUE;
}
if (stream->event[S_INPUT] == NULL)
{
stream->event[S_INPUT]  = CreateEvent(NULL, FALSE, FALSE, NULL);
setEvent[S_INPUT] = TRUE;
}
if ((stream->event[S_OUTPUT] == NULL) || (stream->event[S_INPUT] == NULL))
{
PRINT(("WASAPI Thread: failed creating Input/Output event handle\n"));
goto thread_error;
}
stream->running = TRUE;
SetEvent(stream->hThreadStart);
if (stream->in.clientProc)
{
if (setEvent[S_INPUT])
{
if ((hr = IAudioClient_SetEventHandle(stream->in.clientProc, stream->event[S_INPUT])) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
}
if ((hr = IAudioClient_Start(stream->in.clientProc)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
}
if (stream->out.clientProc)
{
if (setEvent[S_OUTPUT])
{
if ((hr = IAudioClient_SetEventHandle(stream->out.clientProc, stream->event[S_OUTPUT])) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
}
if ((hr = ProcessOutputBuffer(stream, processor, stream->out.framesPerBuffer)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
if ((hr = IAudioClient_Start(stream->out.clientProc)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
}
NotifyStateChanged(stream, paWasapiStreamStateThreadStart, ERROR_SUCCESS);
for (;;)
{
dwResult = WaitForMultipleObjects(S_COUNT, stream->event, waitAllEvents, 10*1000);
if (WaitForSingleObject(stream->hCloseRequest, 0) != WAIT_TIMEOUT)
break;
switch (dwResult)
{
case WAIT_TIMEOUT: {
PRINT(("WASAPI Thread: WAIT_TIMEOUT - probably bad audio driver or Vista x64 bug: use paWinWasapiPolling instead\n"));
goto thread_end;
break; }
case WAIT_OBJECT_0 + S_INPUT: {
if (stream->captureClient == NULL)
break;
if ((hr = ProcessInputBuffer(stream, processor)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
break; }
case WAIT_OBJECT_0 + S_OUTPUT: {
if (stream->renderClient == NULL)
break;
if ((hr = ProcessOutputBuffer(stream, processor, stream->out.framesPerBuffer)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
break; }
}
}
thread_end:
_StreamOnStop(stream);
FinishComPointers(stream, threadComInitialized);
SystemTimer_RestoreGranularity(&timer);
stream->running = FALSE;
SetEvent(stream->hThreadExit);
NotifyStateChanged(stream, paWasapiStreamStateThreadStop, hr);
return 0;
thread_error:
SetEvent(stream->hThreadStart);
goto thread_end;
}
static UINT32 GetSleepTime(PaWasapiStream *stream, UINT32 sleepTimeIn, UINT32 sleepTimeOut, UINT32 userFramesOut)
{
UINT32 sleepTime;
if (userFramesOut != 0)
{
UINT32 chunks = stream->out.framesPerHostCallback / userFramesOut;
if (chunks <= 2)
{
sleepTimeOut /= 2;
PRINT(("WASAPI: underrun workaround, sleep [%d] ms - 1/2 of the user buffer[%d] | host buffer[%d]\n", sleepTimeOut, userFramesOut, stream->out.framesPerHostCallback));
}
}
if ((sleepTimeIn != 0) && (sleepTimeOut != 0))
sleepTime = min(sleepTimeIn, sleepTimeOut);
else
sleepTime = (sleepTimeIn ? sleepTimeIn : sleepTimeOut);
return sleepTime;
}
static UINT32 ConfigureLoopSleepTimeAndScheduler(PaWasapiStream *stream, ThreadIdleScheduler *scheduler)
{
UINT32 sleepTime, sleepTimeIn, sleepTimeOut;
UINT32 userFramesIn = stream->in.framesPerHostCallback / WASAPI_PACKETS_PER_INPUT_BUFFER;
UINT32 userFramesOut = stream->out.framesPerBuffer;
if (stream->bufferMode != paUtilFixedHostBufferSize)
{
userFramesOut = (stream->bufferProcessor.framesPerUserBuffer ? stream->bufferProcessor.framesPerUserBuffer :
stream->out.params.frames_per_buffer);
}
sleepTimeIn  = GetFramesSleepTime(userFramesIn, stream->in.wavex.Format.nSamplesPerSec);
sleepTimeOut = GetFramesSleepTime(userFramesOut, stream->out.wavex.Format.nSamplesPerSec);
if (sleepTimeIn > 2)
sleepTimeIn = 2;
sleepTime = GetSleepTime(stream, sleepTimeIn, sleepTimeOut, userFramesOut);
if (sleepTime == 0)
{
sleepTimeIn  = GetFramesSleepTimeMicroseconds(userFramesIn, stream->in.wavex.Format.nSamplesPerSec);
sleepTimeOut = GetFramesSleepTimeMicroseconds(userFramesOut, stream->out.wavex.Format.nSamplesPerSec);
sleepTime = GetSleepTime(stream, sleepTimeIn, sleepTimeOut, userFramesOut);
ThreadIdleScheduler_Setup(scheduler, 1, sleepTime );
sleepTime = 0;
}
return sleepTime;
}
static inline INT32 GetNextSleepTime(SystemTimer *timer, ThreadIdleScheduler *scheduler, LONGLONG startTime,
UINT32 sleepTime)
{
INT32 nextSleepTime;
INT32 procTime;
if (sleepTime == 0)
nextSleepTime = ThreadIdleScheduler_NextSleep(scheduler);
else
nextSleepTime = sleepTime;
procTime = (INT32)(SystemTimer_GetTime(timer) - startTime);
nextSleepTime -= procTime;
if (nextSleepTime < timer->granularity)
nextSleepTime = 0;
else
if (timer->granularity > 1)
nextSleepTime = ALIGN_BWD(nextSleepTime, timer->granularity);
#ifdef PA_WASAPI_LOG_TIME_SLOTS
printf("{%d},", procTime);
#endif
return nextSleepTime;
}
PA_THREAD_FUNC ProcThreadPoll(void *param)
{
PaWasapiHostProcessor processor[S_COUNT];
HRESULT hr = S_OK;
PaWasapiStream *stream = (PaWasapiStream *)param;
PaWasapiHostProcessor defaultProcessor;
INT32 i;
ThreadIdleScheduler scheduler;
SystemTimer timer;
LONGLONG startTime;
UINT32 sleepTime;
INT32 nextSleepTime = 0;  
BOOL threadComInitialized = FALSE;
#ifdef PA_WASAPI_LOG_TIME_SLOTS
LONGLONG startWaitTime;
#endif
NotifyStateChanged(stream, paWasapiStreamStateThreadPrepare, ERROR_SUCCESS);
if (!PrepareComPointers(stream, &threadComInitialized))
return (UINT32)paUnanticipatedHostError;
SystemTimer_SetGranularity(&timer, 1);
sleepTime = ConfigureLoopSleepTimeAndScheduler(stream, &scheduler);
defaultProcessor.processor = WaspiHostProcessingLoop;
defaultProcessor.userData  = stream;
processor[S_INPUT] = (stream->hostProcessOverrideInput.processor != NULL ? stream->hostProcessOverrideInput : defaultProcessor);
processor[S_OUTPUT] = (stream->hostProcessOverrideOutput.processor != NULL ? stream->hostProcessOverrideOutput : defaultProcessor);
PaWasapi_ThreadPriorityBoost((void **)&stream->hAvTask, stream->nThreadPriority);
stream->running = TRUE;
SetEvent(stream->hThreadStart);
if (stream->in.clientProc)
{
if ((hr = IAudioClient_Start(stream->in.clientProc)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
}
if (stream->out.clientProc)
{
if (!PA_WASAPI__IS_FULLDUPLEX(stream))
{
UINT32 frames = 0;
if ((hr = _PollGetOutputFramesAvailable(stream, &frames)) == S_OK)
{
if (stream->bufferMode == paUtilFixedHostBufferSize)
{
while (frames >= stream->out.framesPerBuffer)
{
if ((hr = ProcessOutputBuffer(stream, processor, stream->out.framesPerBuffer)) != S_OK)
{
LogHostError(hr);  
break;
}
frames -= stream->out.framesPerBuffer;
}
}
else
{
if (frames == 0)
frames = stream->out.framesPerBuffer;
if ((stream->out.shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE) && (frames >= (stream->out.framesPerBuffer * 2)))
frames -= stream->out.framesPerBuffer;
if ((hr = ProcessOutputBuffer(stream, processor, frames)) != S_OK)
{
LogHostError(hr);  
}
}
}
else
{
LogHostError(hr);  
}
}
if ((hr = IAudioClient_Start(stream->out.clientProc)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
}
NotifyStateChanged(stream, paWasapiStreamStateThreadStart, ERROR_SUCCESS);
#ifdef PA_WASAPI_LOG_TIME_SLOTS
startWaitTime = SystemTimer_GetTime(&timer);
#endif
if (!PA_WASAPI__IS_FULLDUPLEX(stream))
{
while (WaitForSingleObject(stream->hCloseRequest, nextSleepTime) == WAIT_TIMEOUT)
{
startTime = SystemTimer_GetTime(&timer);
#ifdef PA_WASAPI_LOG_TIME_SLOTS
printf("[%d|%d],", nextSleepTime, (INT32)(startTime - startWaitTime));
#endif
for (i = 0; i < S_COUNT; ++i)
{
switch (i)
{
case S_INPUT: {
if (stream->captureClient == NULL)
break;
if ((hr = ProcessInputBuffer(stream, processor)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
break; }
case S_OUTPUT: {
UINT32 framesAvail;
if (stream->renderClient == NULL)
break;
if ((hr = _PollGetOutputFramesAvailable(stream, &framesAvail)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
if (stream->bufferMode == paUtilFixedHostBufferSize)
{
UINT32 framesProc = stream->out.framesPerBuffer;
if (framesAvail < framesProc)
{
nextSleepTime = 0;
continue;
}
while (framesAvail >= framesProc)
{
if ((hr = ProcessOutputBuffer(stream, processor, framesProc)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
framesAvail -= framesProc;
}
}
else
if (framesAvail != 0)
{
if ((hr = ProcessOutputBuffer(stream, processor, framesAvail)) != S_OK)
{
LogHostError(hr);
goto thread_error;
}
}
break; }
}
}
nextSleepTime = GetNextSleepTime(&timer, &scheduler, startTime, sleepTime);
#ifdef PA_WASAPI_LOG_TIME_SLOTS
startWaitTime = SystemTimer_GetTime(&timer);
#endif
}
}
else
{
while (WaitForSingleObject(stream->hCloseRequest, nextSleepTime) == WAIT_TIMEOUT)
{
UINT32 i_frames = 0, i_processed = 0, o_frames = 0;
BYTE *i_data = NULL, *o_data = NULL, *o_data_host = NULL;
DWORD i_flags = 0;
startTime = SystemTimer_GetTime(&timer);
#ifdef PA_WASAPI_LOG_TIME_SLOTS
printf("[%d|%d],", nextSleepTime, (INT32)(startTime - startWaitTime));
#endif
if ((hr = _PollGetOutputFramesAvailable(stream, &o_frames)) != S_OK)
{
LogHostError(hr);
break;
}
while (o_frames != 0)
{
if ((hr = IAudioCaptureClient_GetBuffer(stream->captureClient, &i_data, &i_frames, &i_flags, NULL, NULL)) != S_OK)
{
if (hr == AUDCLNT_S_BUFFER_EMPTY)
break;  
LogHostError(hr);
break;
}
if (o_frames >= i_frames)
{
UINT32 o_processed = i_frames;
if ((hr = IAudioRenderClient_GetBuffer(stream->renderClient, o_processed, &o_data)) == S_OK)
{
i_processed = i_frames;
o_data_host = o_data;
if (stream->out.monoMixer)
{
UINT32 mono_frames_size = o_processed * (stream->out.wavex.Format.wBitsPerSample / 8);
if (mono_frames_size > stream->out.monoBufferSize)
{
stream->out.monoBuffer = PaWasapi_ReallocateMemory(stream->out.monoBuffer, (stream->out.monoBufferSize = mono_frames_size));
if (stream->out.monoBuffer == NULL)
{
IAudioCaptureClient_ReleaseBuffer(stream->captureClient, 0);
IAudioRenderClient_ReleaseBuffer(stream->renderClient, 0, 0);
LogPaError(paInsufficientMemory);
goto thread_error;
}
}
o_data = (BYTE *)stream->out.monoBuffer;
}
if (stream->in.monoMixer)
{
UINT32 mono_frames_size = i_processed * (stream->in.wavex.Format.wBitsPerSample / 8);
if (mono_frames_size > stream->in.monoBufferSize)
{
stream->in.monoBuffer = PaWasapi_ReallocateMemory(stream->in.monoBuffer, (stream->in.monoBufferSize = mono_frames_size));
if (stream->in.monoBuffer == NULL)
{
IAudioCaptureClient_ReleaseBuffer(stream->captureClient, 0);
IAudioRenderClient_ReleaseBuffer(stream->renderClient, 0, 0);
LogPaError(paInsufficientMemory);
goto thread_error;
}
}
stream->in.monoMixer(stream->in.monoBuffer, i_data, i_processed);
i_data = (BYTE *)stream->in.monoBuffer;
}
processor[S_FULLDUPLEX].processor(i_data, i_processed, o_data, o_processed, processor[S_FULLDUPLEX].userData);
if (stream->out.monoBuffer)
stream->out.monoMixer(o_data_host, stream->out.monoBuffer, o_processed);
if ((hr = IAudioRenderClient_ReleaseBuffer(stream->renderClient, o_processed, 0)) != S_OK)
LogHostError(hr);
o_frames -= o_processed;
}
else
{
if (stream->out.shareMode != AUDCLNT_SHAREMODE_SHARED)
LogHostError(hr);  
}
}
else
{
i_processed = 0;
goto fd_release_buffer_in;
}
fd_release_buffer_in:
if ((hr = IAudioCaptureClient_ReleaseBuffer(stream->captureClient, i_processed)) != S_OK)
{
LogHostError(hr);
break;
}
if (i_processed == 0)
break;
}
nextSleepTime = GetNextSleepTime(&timer, &scheduler, startTime, sleepTime);
#ifdef PA_WASAPI_LOG_TIME_SLOTS
startWaitTime = SystemTimer_GetTime(&timer);
#endif
}
}
thread_end:
_StreamOnStop(stream);
FinishComPointers(stream, threadComInitialized);
SystemTimer_RestoreGranularity(&timer);
stream->running = FALSE;
SetEvent(stream->hThreadExit);
NotifyStateChanged(stream, paWasapiStreamStateThreadStop, hr);
return 0;
thread_error:
SetEvent(stream->hThreadStart);
goto thread_end;
}
void *PaWasapi_ReallocateMemory(void *prev, size_t size)
{
void *ret = realloc(prev, size);
if (ret == NULL)
{
PaWasapi_FreeMemory(prev);
return NULL;
}
return ret;
}
void PaWasapi_FreeMemory(void *ptr)
{
free(ptr);
}

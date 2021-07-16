#ifndef PORTAUDIO_H
#define PORTAUDIO_H
#ifdef __cplusplus
extern "C"
{
#endif  
int Pa_GetVersion( void );
const char* Pa_GetVersionText( void );
#define paMakeVersionNumber(major, minor, subminor) \
(((major)&0xFF)<<16 | ((minor)&0xFF)<<8 | ((subminor)&0xFF))
typedef struct PaVersionInfo {
int versionMajor;
int versionMinor;
int versionSubMinor;
const char *versionControlRevision;
const char *versionText;
} PaVersionInfo;
const PaVersionInfo* Pa_GetVersionInfo( void );
typedef int PaError;
typedef enum PaErrorCode
{
paNoError = 0,
paNotInitialized = -10000,
paUnanticipatedHostError,
paInvalidChannelCount,
paInvalidSampleRate,
paInvalidDevice,
paInvalidFlag,
paSampleFormatNotSupported,
paBadIODeviceCombination,
paInsufficientMemory,
paBufferTooBig,
paBufferTooSmall,
paNullCallback,
paBadStreamPtr,
paTimedOut,
paInternalError,
paDeviceUnavailable,
paIncompatibleHostApiSpecificStreamInfo,
paStreamIsStopped,
paStreamIsNotStopped,
paInputOverflowed,
paOutputUnderflowed,
paHostApiNotFound,
paInvalidHostApi,
paCanNotReadFromACallbackStream,
paCanNotWriteToACallbackStream,
paCanNotReadFromAnOutputOnlyStream,
paCanNotWriteToAnInputOnlyStream,
paIncompatibleStreamHostApi,
paBadBufferPtr
} PaErrorCode;
const char *Pa_GetErrorText( PaError errorCode );
PaError Pa_Initialize( void );
PaError Pa_Terminate( void );
typedef int PaDeviceIndex;
#define paNoDevice ((PaDeviceIndex)-1)
#define paUseHostApiSpecificDeviceSpecification ((PaDeviceIndex)-2)
typedef int PaHostApiIndex;
PaHostApiIndex Pa_GetHostApiCount( void );
PaHostApiIndex Pa_GetDefaultHostApi( void );
typedef enum PaHostApiTypeId
{
paInDevelopment=0,  
paDirectSound=1,
paMME=2,
paASIO=3,
paSoundManager=4,
paCoreAudio=5,
paOSS=7,
paALSA=8,
paAL=9,
paBeOS=10,
paWDMKS=11,
paJACK=12,
paWASAPI=13,
paAudioScienceHPI=14
} PaHostApiTypeId;
typedef struct PaHostApiInfo
{
int structVersion;
PaHostApiTypeId type;
const char *name;
int deviceCount;
PaDeviceIndex defaultInputDevice;
PaDeviceIndex defaultOutputDevice;
} PaHostApiInfo;
const PaHostApiInfo * Pa_GetHostApiInfo( PaHostApiIndex hostApi );
PaHostApiIndex Pa_HostApiTypeIdToHostApiIndex( PaHostApiTypeId type );
PaDeviceIndex Pa_HostApiDeviceIndexToDeviceIndex( PaHostApiIndex hostApi,
int hostApiDeviceIndex );
typedef struct PaHostErrorInfo{
PaHostApiTypeId hostApiType;     
long errorCode;                  
const char *errorText;           
}PaHostErrorInfo;
const PaHostErrorInfo* Pa_GetLastHostErrorInfo( void );
PaDeviceIndex Pa_GetDeviceCount( void );
PaDeviceIndex Pa_GetDefaultInputDevice( void );
PaDeviceIndex Pa_GetDefaultOutputDevice( void );
typedef double PaTime;
typedef unsigned long PaSampleFormat;
#define paFloat32        ((PaSampleFormat) 0x00000001)  
#define paInt32          ((PaSampleFormat) 0x00000002)  
#define paInt24          ((PaSampleFormat) 0x00000004)  
#define paInt16          ((PaSampleFormat) 0x00000008)  
#define paInt8           ((PaSampleFormat) 0x00000010)  
#define paUInt8          ((PaSampleFormat) 0x00000020)  
#define paCustomFormat   ((PaSampleFormat) 0x00010000)  
#define paNonInterleaved ((PaSampleFormat) 0x80000000)  
typedef struct PaDeviceInfo
{
int structVersion;   
const char *name;
PaHostApiIndex hostApi;  
int maxInputChannels;
int maxOutputChannels;
PaTime defaultLowInputLatency;
PaTime defaultLowOutputLatency;
PaTime defaultHighInputLatency;
PaTime defaultHighOutputLatency;
double defaultSampleRate;
} PaDeviceInfo;
const PaDeviceInfo* Pa_GetDeviceInfo( PaDeviceIndex device );
typedef struct PaStreamParameters
{
PaDeviceIndex device;
int channelCount;
PaSampleFormat sampleFormat;
PaTime suggestedLatency;
void *hostApiSpecificStreamInfo;
} PaStreamParameters;
#define paFormatIsSupported (0)
PaError Pa_IsFormatSupported( const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate );
typedef void PaStream;
#define paFramesPerBufferUnspecified  (0)
typedef unsigned long PaStreamFlags;
#define   paNoFlag          ((PaStreamFlags) 0)
#define   paClipOff         ((PaStreamFlags) 0x00000001)
#define   paDitherOff       ((PaStreamFlags) 0x00000002)
#define   paNeverDropInput  ((PaStreamFlags) 0x00000004)
#define   paPrimeOutputBuffersUsingStreamCallback ((PaStreamFlags) 0x00000008)
#define   paPlatformSpecificFlags ((PaStreamFlags)0xFFFF0000)
typedef struct PaStreamCallbackTimeInfo{
PaTime inputBufferAdcTime;   
PaTime currentTime;          
PaTime outputBufferDacTime;  
} PaStreamCallbackTimeInfo;
typedef unsigned long PaStreamCallbackFlags;
#define paInputUnderflow   ((PaStreamCallbackFlags) 0x00000001)
#define paInputOverflow    ((PaStreamCallbackFlags) 0x00000002)
#define paOutputUnderflow  ((PaStreamCallbackFlags) 0x00000004)
#define paOutputOverflow   ((PaStreamCallbackFlags) 0x00000008)
#define paPrimingOutput    ((PaStreamCallbackFlags) 0x00000010)
typedef enum PaStreamCallbackResult
{
paContinue=0,    
paComplete=1,    
paAbort=2        
} PaStreamCallbackResult;
typedef int PaStreamCallback(
const void *input, void *output,
unsigned long frameCount,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData );
PaError Pa_OpenStream( PaStream** stream,
const PaStreamParameters *inputParameters,
const PaStreamParameters *outputParameters,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamFlags streamFlags,
PaStreamCallback *streamCallback,
void *userData );
PaError Pa_OpenDefaultStream( PaStream** stream,
int numInputChannels,
int numOutputChannels,
PaSampleFormat sampleFormat,
double sampleRate,
unsigned long framesPerBuffer,
PaStreamCallback *streamCallback,
void *userData );
PaError Pa_CloseStream( PaStream *stream );
typedef void PaStreamFinishedCallback( void *userData );
PaError Pa_SetStreamFinishedCallback( PaStream *stream, PaStreamFinishedCallback* streamFinishedCallback );
PaError Pa_StartStream( PaStream *stream );
PaError Pa_StopStream( PaStream *stream );
PaError Pa_AbortStream( PaStream *stream );
PaError Pa_IsStreamStopped( PaStream *stream );
PaError Pa_IsStreamActive( PaStream *stream );
typedef struct PaStreamInfo
{
int structVersion;
PaTime inputLatency;
PaTime outputLatency;
double sampleRate;
} PaStreamInfo;
const PaStreamInfo* Pa_GetStreamInfo( PaStream *stream );
PaTime Pa_GetStreamTime( PaStream *stream );
double Pa_GetStreamCpuLoad( PaStream* stream );
PaError Pa_ReadStream( PaStream* stream,
void *buffer,
unsigned long frames );
PaError Pa_WriteStream( PaStream* stream,
const void *buffer,
unsigned long frames );
signed long Pa_GetStreamReadAvailable( PaStream* stream );
signed long Pa_GetStreamWriteAvailable( PaStream* stream );
PaError Pa_GetSampleSize( PaSampleFormat format );
void Pa_Sleep( long msec );
#ifdef __cplusplus
}
#endif  
#endif  

 
#ifndef PA_MAC_CORE_INTERNAL_H__
#define PA_MAC_CORE_INTERNAL_H__
#include <CoreAudio/CoreAudio.h>
#include <CoreServices/CoreServices.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>
#include "portaudio.h"
#include "pa_util.h"
#include "pa_hostapi.h"
#include "pa_stream.h"
#include "pa_allocation.h"
#include "pa_cpuload.h"
#include "pa_process.h"
#include "pa_ringbuffer.h"
#include "pa_mac_core_blocking.h"
#ifdef __cplusplus
extern "C"
{
#endif  
PaError PaMacCore_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
#ifdef __cplusplus
}
#endif  
#define RING_BUFFER_ADVANCE_DENOMINATOR (4)
PaError ReadStream( PaStream* stream, void *buffer, unsigned long frames );
PaError WriteStream( PaStream* stream, const void *buffer, unsigned long frames );
signed long GetStreamReadAvailable( PaStream* stream );
signed long GetStreamWriteAvailable( PaStream* stream );
typedef struct
{
PaUtilHostApiRepresentation inheritedHostApiRep;
PaUtilStreamInterface callbackStreamInterface;
PaUtilStreamInterface blockingStreamInterface;
PaUtilAllocationGroup *allocations;
long devCount;
AudioDeviceID *devIds;  
AudioDeviceID defaultIn;
AudioDeviceID defaultOut;
}
PaMacAUHAL;
typedef struct PaMacCoreDeviceProperties
{
UInt32 safetyOffset;
UInt32 bufferFrameSize;
UInt32 deviceLatency;
Float64 sampleRate;
Float64 samplePeriod;  
}
PaMacCoreDeviceProperties;
typedef struct PaMacCoreStream
{
PaUtilStreamRepresentation streamRepresentation;
PaUtilCpuLoadMeasurer cpuLoadMeasurer;
PaUtilBufferProcessor bufferProcessor;
bool bufferProcessorIsInitialized;
AudioUnit inputUnit;
AudioUnit outputUnit;
AudioDeviceID inputDevice;
AudioDeviceID outputDevice;
size_t userInChan;
size_t userOutChan;
size_t inputFramesPerBuffer;
size_t outputFramesPerBuffer;
PaMacBlio blio;
PaUtilRingBuffer inputRingBuffer;
AudioConverterRef inputSRConverter;
AudioBufferList inputAudioBufferList;
AudioTimeStamp startTime;
volatile uint32_t xrunFlags;  
volatile enum {
STOPPED          = 0,  
CALLBACK_STOPPED = 1,  
STOPPING         = 2,  
ACTIVE           = 3   
} state;
double sampleRate;
PaMacCoreDeviceProperties  inputProperties;
PaMacCoreDeviceProperties  outputProperties;
int timingInformationMutexIsInitialized;
pthread_mutex_t timingInformationMutex;
Float64 timestampOffsetCombined;
Float64 timestampOffsetInputDevice;
Float64 timestampOffsetOutputDevice;
Float64 timestampOffsetCombined_ioProcCopy;
Float64 timestampOffsetInputDevice_ioProcCopy;
Float64 timestampOffsetOutputDevice_ioProcCopy;
}
PaMacCoreStream;
#endif  

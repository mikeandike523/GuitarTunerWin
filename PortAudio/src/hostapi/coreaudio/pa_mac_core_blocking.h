 
#ifndef PA_MAC_CORE_BLOCKING_H_
#define PA_MAC_CORE_BLOCKING_H_
#include "pa_ringbuffer.h"
#include "portaudio.h"
#include "pa_mac_core_utilities.h"
#define PA_MAC_BLIO_BUSY_WAIT_SLEEP_INTERVAL (5)
#define PA_MAC_BLIO_BUSY_WAIT
typedef struct {
PaUtilRingBuffer inputRingBuffer;
PaUtilRingBuffer outputRingBuffer;
ring_buffer_size_t ringBufferFrames;
PaSampleFormat inputSampleFormat;
size_t inputSampleSizeActual;
size_t inputSampleSizePow2;
PaSampleFormat outputSampleFormat;
size_t outputSampleSizeActual;
size_t outputSampleSizePow2;
int inChan;
int outChan;
uint32_t statusFlags;
PaError errors;
#ifdef  PA_MAC_BLIO_MUTEX
volatile bool isInputEmpty;
pthread_mutex_t inputMutex;
pthread_cond_t inputCond;
volatile bool isOutputFull;
pthread_mutex_t outputMutex;
pthread_cond_t outputCond;
#endif
}
PaMacBlio;
PaError initializeBlioRingBuffers(
PaMacBlio *blio,
PaSampleFormat inputSampleFormat,
PaSampleFormat outputSampleFormat,
long ringBufferSizeInFrames,
int inChan,
int outChan );
PaError destroyBlioRingBuffers( PaMacBlio *blio );
PaError resetBlioRingBuffers( PaMacBlio *blio );
int BlioCallback(
const void *input, void *output,
unsigned long frameCount,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData );
PaError waitUntilBlioWriteBufferIsEmpty( PaMacBlio *blio, double sampleRate,
size_t framesPerBuffer );
#endif  

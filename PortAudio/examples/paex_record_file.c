 
#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "pa_util.h"
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif
static ring_buffer_size_t rbs_min(ring_buffer_size_t a, ring_buffer_size_t b)
{
return (a < b) ? a : b;
}
#define FILE_NAME       "audio_data.raw"
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (512)
#define NUM_SECONDS     (10)
#define NUM_CHANNELS    (2)
#define NUM_WRITES_PER_BUFFER   (4)
#define DITHER_FLAG     (0)  
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif
typedef struct
{
unsigned            frameIndex;
int                 threadSyncFlag;
SAMPLE             *ringBufferData;
PaUtilRingBuffer    ringBuffer;
FILE               *file;
void               *threadHandle;
}
paTestData;
static int threadFunctionWriteToRawFile(void* ptr)
{
paTestData* pData = (paTestData*)ptr;
pData->threadSyncFlag = 0;
while (1)
{
ring_buffer_size_t elementsInBuffer = PaUtil_GetRingBufferReadAvailable(&pData->ringBuffer);
if ( (elementsInBuffer >= pData->ringBuffer.bufferSize / NUM_WRITES_PER_BUFFER) ||
pData->threadSyncFlag )
{
void* ptr[2] = {0};
ring_buffer_size_t sizes[2] = {0};
ring_buffer_size_t elementsRead = PaUtil_GetRingBufferReadRegions(&pData->ringBuffer, elementsInBuffer, ptr + 0, sizes + 0, ptr + 1, sizes + 1);
if (elementsRead > 0)
{
int i;
for (i = 0; i < 2 && ptr[i] != NULL; ++i)
{
fwrite(ptr[i], pData->ringBuffer.elementSizeBytes, sizes[i], pData->file);
}
PaUtil_AdvanceRingBufferReadIndex(&pData->ringBuffer, elementsRead);
}
if (pData->threadSyncFlag)
{
break;
}
}
Pa_Sleep(20);
}
pData->threadSyncFlag = 0;
return 0;
}
static int threadFunctionReadFromRawFile(void* ptr)
{
paTestData* pData = (paTestData*)ptr;
while (1)
{
ring_buffer_size_t elementsInBuffer = PaUtil_GetRingBufferWriteAvailable(&pData->ringBuffer);
if (elementsInBuffer >= pData->ringBuffer.bufferSize / NUM_WRITES_PER_BUFFER)
{
void* ptr[2] = {0};
ring_buffer_size_t sizes[2] = {0};
PaUtil_GetRingBufferWriteRegions(&pData->ringBuffer, elementsInBuffer, ptr + 0, sizes + 0, ptr + 1, sizes + 1);
if (!feof(pData->file))
{
ring_buffer_size_t itemsReadFromFile = 0;
int i;
for (i = 0; i < 2 && ptr[i] != NULL; ++i)
{
itemsReadFromFile += (ring_buffer_size_t)fread(ptr[i], pData->ringBuffer.elementSizeBytes, sizes[i], pData->file);
}
PaUtil_AdvanceRingBufferWriteIndex(&pData->ringBuffer, itemsReadFromFile);
pData->threadSyncFlag = 0;
}
else
{
pData->threadSyncFlag = 1;
break;
}
}
Pa_Sleep(20);
}
return 0;
}
typedef int (*ThreadFunctionType)(void*);
static PaError startThread( paTestData* pData, ThreadFunctionType fn )
{
#ifdef _WIN32
typedef unsigned (__stdcall* WinThreadFunctionType)(void*);
pData->threadHandle = (void*)_beginthreadex(NULL, 0, (WinThreadFunctionType)fn, pData, CREATE_SUSPENDED, NULL);
if (pData->threadHandle == NULL) return paUnanticipatedHostError;
SetThreadPriority(pData->threadHandle, THREAD_PRIORITY_ABOVE_NORMAL);
pData->threadSyncFlag = 1;
ResumeThread(pData->threadHandle);
#endif
while (pData->threadSyncFlag) {
Pa_Sleep(10);
}
return paNoError;
}
static int stopThread( paTestData* pData )
{
pData->threadSyncFlag = 1;
while (pData->threadSyncFlag) {
Pa_Sleep(10);
}
#ifdef _WIN32
CloseHandle(pData->threadHandle);
pData->threadHandle = 0;
#endif
return paNoError;
}
static int recordCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
ring_buffer_size_t elementsWriteable = PaUtil_GetRingBufferWriteAvailable(&data->ringBuffer);
ring_buffer_size_t elementsToWrite = rbs_min(elementsWriteable, (ring_buffer_size_t)(framesPerBuffer * NUM_CHANNELS));
const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
(void) outputBuffer;  
(void) timeInfo;
(void) statusFlags;
(void) userData;
data->frameIndex += PaUtil_WriteRingBuffer(&data->ringBuffer, rptr, elementsToWrite);
return paContinue;
}
static int playCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
ring_buffer_size_t elementsToPlay = PaUtil_GetRingBufferReadAvailable(&data->ringBuffer);
ring_buffer_size_t elementsToRead = rbs_min(elementsToPlay, (ring_buffer_size_t)(framesPerBuffer * NUM_CHANNELS));
SAMPLE* wptr = (SAMPLE*)outputBuffer;
(void) inputBuffer;  
(void) timeInfo;
(void) statusFlags;
(void) userData;
data->frameIndex += PaUtil_ReadRingBuffer(&data->ringBuffer, wptr, elementsToRead);
return data->threadSyncFlag ? paComplete : paContinue;
}
static unsigned NextPowerOf2(unsigned val)
{
val--;
val = (val >> 1) | val;
val = (val >> 2) | val;
val = (val >> 4) | val;
val = (val >> 8) | val;
val = (val >> 16) | val;
return ++val;
}
int main(void);
int main(void)
{
PaStreamParameters  inputParameters,
outputParameters;
PaStream*           stream;
PaError             err = paNoError;
paTestData          data = {0};
unsigned            delayCntr;
unsigned            numSamples;
unsigned            numBytes;
printf("patest_record.c\n"); fflush(stdout);
numSamples = NextPowerOf2((unsigned)(SAMPLE_RATE * 0.5 * NUM_CHANNELS));
numBytes = numSamples * sizeof(SAMPLE);
data.ringBufferData = (SAMPLE *) PaUtil_AllocateMemory( numBytes );
if( data.ringBufferData == NULL )
{
printf("Could not allocate ring buffer data.\n");
goto done;
}
if (PaUtil_InitializeRingBuffer(&data.ringBuffer, sizeof(SAMPLE), numSamples, data.ringBufferData) < 0)
{
printf("Failed to initialize ring buffer. Size is not power of 2 ??\n");
goto done;
}
err = Pa_Initialize();
if( err != paNoError ) goto done;
inputParameters.device = Pa_GetDefaultInputDevice();  
if (inputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default input device.\n");
goto done;
}
inputParameters.channelCount = 2;                     
inputParameters.sampleFormat = PA_SAMPLE_TYPE;
inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
inputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream,
&inputParameters,
NULL,                   
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
recordCallback,
&data );
if( err != paNoError ) goto done;
data.file = fopen(FILE_NAME, "wb");
if (data.file == 0) goto done;
err = startThread(&data, threadFunctionWriteToRawFile);
if( err != paNoError ) goto done;
err = Pa_StartStream( stream );
if( err != paNoError ) goto done;
printf("\n=== Now recording to '" FILE_NAME "' for %d seconds!! Please speak into the microphone. ===\n", NUM_SECONDS); fflush(stdout);
delayCntr = 0;
while( delayCntr++ < NUM_SECONDS )
{
printf("index = %d\n", data.frameIndex ); fflush(stdout);
Pa_Sleep(1000);
}
if( err < 0 ) goto done;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto done;
err = stopThread(&data);
if( err != paNoError ) goto done;
fclose(data.file);
data.file = 0;
data.frameIndex = 0;
outputParameters.device = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto done;
}
outputParameters.channelCount = 2;                      
outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
printf("\n=== Now playing back from file '" FILE_NAME "' until end-of-file is reached ===\n"); fflush(stdout);
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
playCallback,
&data );
if( err != paNoError ) goto done;
if( stream )
{
data.file = fopen(FILE_NAME, "rb");
if (data.file != 0)
{
err = startThread(&data, threadFunctionReadFromRawFile);
if( err != paNoError ) goto done;
err = Pa_StartStream( stream );
if( err != paNoError ) goto done;
printf("Waiting for playback to finish.\n"); fflush(stdout);
while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) {
printf("index = %d\n", data.frameIndex ); fflush(stdout);
Pa_Sleep(1000);
}
if( err < 0 ) goto done;
}
err = Pa_CloseStream( stream );
if( err != paNoError ) goto done;
fclose(data.file);
printf("Done.\n"); fflush(stdout);
}
done:
Pa_Terminate();
if( data.ringBufferData )        
PaUtil_FreeMemory( data.ringBufferData );
if( err != paNoError )
{
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
err = 1;           
}
return err;
}

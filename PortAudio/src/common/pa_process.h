#ifndef PA_PROCESS_H
#define PA_PROCESS_H
#include "portaudio.h"
#include "pa_converters.h"
#include "pa_dither.h"
#ifdef __cplusplus
extern "C"
{
#endif  
typedef enum {
paUtilFixedHostBufferSize,
paUtilBoundedHostBufferSize,
paUtilUnknownHostBufferSize,
paUtilVariableHostBufferSizePartialUsageAllowed
}PaUtilHostBufferSizeMode;
typedef struct PaUtilChannelDescriptor{
void *data;
unsigned int stride;   
}PaUtilChannelDescriptor;
typedef struct {
unsigned long framesPerUserBuffer;
unsigned long framesPerHostBuffer;
PaUtilHostBufferSizeMode hostBufferSizeMode;
int useNonAdaptingProcess;
int userOutputSampleFormatIsEqualToHost;
int userInputSampleFormatIsEqualToHost;
unsigned long framesPerTempBuffer;
unsigned int inputChannelCount;
unsigned int bytesPerHostInputSample;
unsigned int bytesPerUserInputSample;
int userInputIsInterleaved;
PaUtilConverter *inputConverter;
PaUtilZeroer *inputZeroer;
unsigned int outputChannelCount;
unsigned int bytesPerHostOutputSample;
unsigned int bytesPerUserOutputSample;
int userOutputIsInterleaved;
PaUtilConverter *outputConverter;
PaUtilZeroer *outputZeroer;
unsigned long initialFramesInTempInputBuffer;
unsigned long initialFramesInTempOutputBuffer;
void *tempInputBuffer;           
void **tempInputBufferPtrs;      
unsigned long framesInTempInputBuffer;  
void *tempOutputBuffer;          
void **tempOutputBufferPtrs;     
unsigned long framesInTempOutputBuffer;  
PaStreamCallbackTimeInfo *timeInfo;
PaStreamCallbackFlags callbackStatusFlags;
int hostInputIsInterleaved;
unsigned long hostInputFrameCount[2];
PaUtilChannelDescriptor *hostInputChannels[2];  
int hostOutputIsInterleaved;
unsigned long hostOutputFrameCount[2];
PaUtilChannelDescriptor *hostOutputChannels[2];  
PaUtilTriangularDitherGenerator ditherGenerator;
double samplePeriod;
PaStreamCallback *streamCallback;
void *userData;
} PaUtilBufferProcessor;
PaError PaUtil_InitializeBufferProcessor( PaUtilBufferProcessor* bufferProcessor,
int inputChannelCount, PaSampleFormat userInputSampleFormat,
PaSampleFormat hostInputSampleFormat,
int outputChannelCount, PaSampleFormat userOutputSampleFormat,
PaSampleFormat hostOutputSampleFormat,
double sampleRate,
PaStreamFlags streamFlags,
unsigned long framesPerUserBuffer,  
unsigned long framesPerHostBuffer,
PaUtilHostBufferSizeMode hostBufferSizeMode,
PaStreamCallback *streamCallback, void *userData );
void PaUtil_TerminateBufferProcessor( PaUtilBufferProcessor* bufferProcessor );
void PaUtil_ResetBufferProcessor( PaUtilBufferProcessor* bufferProcessor );
unsigned long PaUtil_GetBufferProcessorInputLatencyFrames( PaUtilBufferProcessor* bufferProcessor );
unsigned long PaUtil_GetBufferProcessorOutputLatencyFrames( PaUtilBufferProcessor* bufferProcessor );
void PaUtil_SetInputFrameCount( PaUtilBufferProcessor* bufferProcessor,
unsigned long frameCount );
void PaUtil_SetNoInput( PaUtilBufferProcessor* bufferProcessor );
void PaUtil_SetInputChannel( PaUtilBufferProcessor* bufferProcessor,
unsigned int channel, void *data, unsigned int stride );
void PaUtil_SetInterleavedInputChannels( PaUtilBufferProcessor* bufferProcessor,
unsigned int firstChannel, void *data, unsigned int channelCount );
void PaUtil_SetNonInterleavedInputChannel( PaUtilBufferProcessor* bufferProcessor,
unsigned int channel, void *data );
void PaUtil_Set2ndInputFrameCount( PaUtilBufferProcessor* bufferProcessor,
unsigned long frameCount );
void PaUtil_Set2ndInputChannel( PaUtilBufferProcessor* bufferProcessor,
unsigned int channel, void *data, unsigned int stride );
void PaUtil_Set2ndInterleavedInputChannels( PaUtilBufferProcessor* bufferProcessor,
unsigned int firstChannel, void *data, unsigned int channelCount );
void PaUtil_Set2ndNonInterleavedInputChannel( PaUtilBufferProcessor* bufferProcessor,
unsigned int channel, void *data );
void PaUtil_SetOutputFrameCount( PaUtilBufferProcessor* bufferProcessor,
unsigned long frameCount );
void PaUtil_SetNoOutput( PaUtilBufferProcessor* bufferProcessor );
void PaUtil_SetOutputChannel( PaUtilBufferProcessor* bufferProcessor,
unsigned int channel, void *data, unsigned int stride );
void PaUtil_SetInterleavedOutputChannels( PaUtilBufferProcessor* bufferProcessor,
unsigned int firstChannel, void *data, unsigned int channelCount );
void PaUtil_SetNonInterleavedOutputChannel( PaUtilBufferProcessor* bufferProcessor,
unsigned int channel, void *data );
void PaUtil_Set2ndOutputFrameCount( PaUtilBufferProcessor* bufferProcessor,
unsigned long frameCount );
void PaUtil_Set2ndOutputChannel( PaUtilBufferProcessor* bufferProcessor,
unsigned int channel, void *data, unsigned int stride );
void PaUtil_Set2ndInterleavedOutputChannels( PaUtilBufferProcessor* bufferProcessor,
unsigned int firstChannel, void *data, unsigned int channelCount );
void PaUtil_Set2ndNonInterleavedOutputChannel( PaUtilBufferProcessor* bufferProcessor,
unsigned int channel, void *data );
void PaUtil_BeginBufferProcessing( PaUtilBufferProcessor* bufferProcessor,
PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags callbackStatusFlags );
unsigned long PaUtil_EndBufferProcessing( PaUtilBufferProcessor* bufferProcessor,
int *callbackResult );
int PaUtil_IsBufferProcessorOutputEmpty( PaUtilBufferProcessor* bufferProcessor );
unsigned long PaUtil_CopyInput( PaUtilBufferProcessor* bufferProcessor,
void **buffer, unsigned long frameCount );
unsigned long PaUtil_CopyOutput( PaUtilBufferProcessor* bufferProcessor,
const void ** buffer, unsigned long frameCount );
unsigned long PaUtil_ZeroOutput( PaUtilBufferProcessor* bufferProcessor,
unsigned long frameCount );
#ifdef __cplusplus
}
#endif  
#endif  

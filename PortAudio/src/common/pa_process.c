 
#include <assert.h>
#include <string.h>  
#include "pa_process.h"
#include "pa_util.h"
#define PA_FRAMES_PER_TEMP_BUFFER_WHEN_HOST_BUFFER_SIZE_IS_UNKNOWN_    1024
#define PA_MIN_( a, b ) ( ((a)<(b)) ? (a) : (b) )
static unsigned long GCD( unsigned long a, unsigned long b )
{
return (b==0) ? a : GCD( b, a%b);
}
static unsigned long LCM( unsigned long a, unsigned long b )
{
return (a*b) / GCD(a,b);
}
#define PA_MAX_( a, b ) (((a) > (b)) ? (a) : (b))
static unsigned long CalculateFrameShift( unsigned long M, unsigned long N )
{
unsigned long result = 0;
unsigned long i;
unsigned long lcm;
assert( M > 0 );
assert( N > 0 );
lcm = LCM( M, N );
for( i = M; i < lcm; i += M )
result = PA_MAX_( result, i % N );
return result;
}
PaError PaUtil_InitializeBufferProcessor( PaUtilBufferProcessor* bp,
int inputChannelCount, PaSampleFormat userInputSampleFormat,
PaSampleFormat hostInputSampleFormat,
int outputChannelCount, PaSampleFormat userOutputSampleFormat,
PaSampleFormat hostOutputSampleFormat,
double sampleRate,
PaStreamFlags streamFlags,
unsigned long framesPerUserBuffer,
unsigned long framesPerHostBuffer,
PaUtilHostBufferSizeMode hostBufferSizeMode,
PaStreamCallback *streamCallback, void *userData )
{
PaError result = paNoError;
PaError bytesPerSample;
unsigned long tempInputBufferSize, tempOutputBufferSize;
PaStreamFlags tempInputStreamFlags;
if( streamFlags & paNeverDropInput )
{
if( !streamCallback || !(inputChannelCount > 0 && outputChannelCount > 0) ||
framesPerUserBuffer != paFramesPerBufferUnspecified )
return paInvalidFlag;
}
bp->tempInputBuffer = 0;
bp->tempInputBufferPtrs = 0;
bp->tempOutputBuffer = 0;
bp->tempOutputBufferPtrs = 0;
bp->framesPerUserBuffer = framesPerUserBuffer;
bp->framesPerHostBuffer = framesPerHostBuffer;
bp->inputChannelCount = inputChannelCount;
bp->outputChannelCount = outputChannelCount;
bp->hostBufferSizeMode = hostBufferSizeMode;
bp->hostInputChannels[0] = bp->hostInputChannels[1] = 0;
bp->hostOutputChannels[0] = bp->hostOutputChannels[1] = 0;
if( framesPerUserBuffer == 0 )  
{
bp->useNonAdaptingProcess = 1;
bp->initialFramesInTempInputBuffer = 0;
bp->initialFramesInTempOutputBuffer = 0;
if( hostBufferSizeMode == paUtilFixedHostBufferSize
|| hostBufferSizeMode == paUtilBoundedHostBufferSize )
{
bp->framesPerTempBuffer = framesPerHostBuffer;
}
else  
{
bp->framesPerTempBuffer = PA_FRAMES_PER_TEMP_BUFFER_WHEN_HOST_BUFFER_SIZE_IS_UNKNOWN_;
}
}
else
{
bp->framesPerTempBuffer = framesPerUserBuffer;
if( hostBufferSizeMode == paUtilFixedHostBufferSize
&& framesPerHostBuffer % framesPerUserBuffer == 0 )
{
bp->useNonAdaptingProcess = 1;
bp->initialFramesInTempInputBuffer = 0;
bp->initialFramesInTempOutputBuffer = 0;
}
else
{
bp->useNonAdaptingProcess = 0;
if( inputChannelCount > 0 && outputChannelCount > 0 )
{
if( hostBufferSizeMode == paUtilFixedHostBufferSize )
{
unsigned long frameShift =
CalculateFrameShift( framesPerHostBuffer, framesPerUserBuffer );
if( framesPerUserBuffer > framesPerHostBuffer )
{
bp->initialFramesInTempInputBuffer = frameShift;
bp->initialFramesInTempOutputBuffer = 0;
}
else
{
bp->initialFramesInTempInputBuffer = 0;
bp->initialFramesInTempOutputBuffer = frameShift;
}
}
else  
{
bp->initialFramesInTempInputBuffer = 0;
bp->initialFramesInTempOutputBuffer = framesPerUserBuffer;
}
}
else
{
bp->initialFramesInTempInputBuffer = 0;
bp->initialFramesInTempOutputBuffer = 0;
}
}
}
bp->framesInTempInputBuffer = bp->initialFramesInTempInputBuffer;
bp->framesInTempOutputBuffer = bp->initialFramesInTempOutputBuffer;
if( inputChannelCount > 0 )
{
bytesPerSample = Pa_GetSampleSize( hostInputSampleFormat );
if( bytesPerSample > 0 )
{
bp->bytesPerHostInputSample = bytesPerSample;
}
else
{
result = bytesPerSample;
goto error;
}
bytesPerSample = Pa_GetSampleSize( userInputSampleFormat );
if( bytesPerSample > 0 )
{
bp->bytesPerUserInputSample = bytesPerSample;
}
else
{
result = bytesPerSample;
goto error;
}
tempInputStreamFlags = streamFlags;
if( !(tempInputStreamFlags & paDitherOff)  
&& (hostInputSampleFormat & paInt32)  
&& (userInputSampleFormat & paInt24)   ){
tempInputStreamFlags = tempInputStreamFlags | paDitherOff;
}
bp->inputConverter =
PaUtil_SelectConverter( hostInputSampleFormat, userInputSampleFormat, tempInputStreamFlags );
bp->inputZeroer = PaUtil_SelectZeroer( userInputSampleFormat );
bp->userInputIsInterleaved = (userInputSampleFormat & paNonInterleaved)?0:1;
bp->hostInputIsInterleaved = (hostInputSampleFormat & paNonInterleaved)?0:1;
bp->userInputSampleFormatIsEqualToHost = ((userInputSampleFormat & ~paNonInterleaved) == (hostInputSampleFormat & ~paNonInterleaved));
tempInputBufferSize =
bp->framesPerTempBuffer * bp->bytesPerUserInputSample * inputChannelCount;
bp->tempInputBuffer = PaUtil_AllocateMemory( tempInputBufferSize );
if( bp->tempInputBuffer == 0 )
{
result = paInsufficientMemory;
goto error;
}
if( bp->framesInTempInputBuffer > 0 )
memset( bp->tempInputBuffer, 0, tempInputBufferSize );
if( userInputSampleFormat & paNonInterleaved )
{
bp->tempInputBufferPtrs =
(void **)PaUtil_AllocateMemory( sizeof(void*)*inputChannelCount );
if( bp->tempInputBufferPtrs == 0 )
{
result = paInsufficientMemory;
goto error;
}
}
bp->hostInputChannels[0] = (PaUtilChannelDescriptor*)
PaUtil_AllocateMemory( sizeof(PaUtilChannelDescriptor) * inputChannelCount * 2);
if( bp->hostInputChannels[0] == 0 )
{
result = paInsufficientMemory;
goto error;
}
bp->hostInputChannels[1] = &bp->hostInputChannels[0][inputChannelCount];
}
if( outputChannelCount > 0 )
{
bytesPerSample = Pa_GetSampleSize( hostOutputSampleFormat );
if( bytesPerSample > 0 )
{
bp->bytesPerHostOutputSample = bytesPerSample;
}
else
{
result = bytesPerSample;
goto error;
}
bytesPerSample = Pa_GetSampleSize( userOutputSampleFormat );
if( bytesPerSample > 0 )
{
bp->bytesPerUserOutputSample = bytesPerSample;
}
else
{
result = bytesPerSample;
goto error;
}
bp->outputConverter =
PaUtil_SelectConverter( userOutputSampleFormat, hostOutputSampleFormat, streamFlags );
bp->outputZeroer = PaUtil_SelectZeroer( hostOutputSampleFormat );
bp->userOutputIsInterleaved = (userOutputSampleFormat & paNonInterleaved)?0:1;
bp->hostOutputIsInterleaved = (hostOutputSampleFormat & paNonInterleaved)?0:1;
bp->userOutputSampleFormatIsEqualToHost = ((userOutputSampleFormat & ~paNonInterleaved) == (hostOutputSampleFormat & ~paNonInterleaved));
tempOutputBufferSize =
bp->framesPerTempBuffer * bp->bytesPerUserOutputSample * outputChannelCount;
bp->tempOutputBuffer = PaUtil_AllocateMemory( tempOutputBufferSize );
if( bp->tempOutputBuffer == 0 )
{
result = paInsufficientMemory;
goto error;
}
if( bp->framesInTempOutputBuffer > 0 )
memset( bp->tempOutputBuffer, 0, tempOutputBufferSize );
if( userOutputSampleFormat & paNonInterleaved )
{
bp->tempOutputBufferPtrs =
(void **)PaUtil_AllocateMemory( sizeof(void*)*outputChannelCount );
if( bp->tempOutputBufferPtrs == 0 )
{
result = paInsufficientMemory;
goto error;
}
}
bp->hostOutputChannels[0] = (PaUtilChannelDescriptor*)
PaUtil_AllocateMemory( sizeof(PaUtilChannelDescriptor)*outputChannelCount * 2 );
if( bp->hostOutputChannels[0] == 0 )
{
result = paInsufficientMemory;
goto error;
}
bp->hostOutputChannels[1] = &bp->hostOutputChannels[0][outputChannelCount];
}
PaUtil_InitializeTriangularDitherState( &bp->ditherGenerator );
bp->samplePeriod = 1. / sampleRate;
bp->streamCallback = streamCallback;
bp->userData = userData;
return result;
error:
if( bp->tempInputBuffer )
PaUtil_FreeMemory( bp->tempInputBuffer );
if( bp->tempInputBufferPtrs )
PaUtil_FreeMemory( bp->tempInputBufferPtrs );
if( bp->hostInputChannels[0] )
PaUtil_FreeMemory( bp->hostInputChannels[0] );
if( bp->tempOutputBuffer )
PaUtil_FreeMemory( bp->tempOutputBuffer );
if( bp->tempOutputBufferPtrs )
PaUtil_FreeMemory( bp->tempOutputBufferPtrs );
if( bp->hostOutputChannels[0] )
PaUtil_FreeMemory( bp->hostOutputChannels[0] );
return result;
}
void PaUtil_TerminateBufferProcessor( PaUtilBufferProcessor* bp )
{
if( bp->tempInputBuffer )
PaUtil_FreeMemory( bp->tempInputBuffer );
if( bp->tempInputBufferPtrs )
PaUtil_FreeMemory( bp->tempInputBufferPtrs );
if( bp->hostInputChannels[0] )
PaUtil_FreeMemory( bp->hostInputChannels[0] );
if( bp->tempOutputBuffer )
PaUtil_FreeMemory( bp->tempOutputBuffer );
if( bp->tempOutputBufferPtrs )
PaUtil_FreeMemory( bp->tempOutputBufferPtrs );
if( bp->hostOutputChannels[0] )
PaUtil_FreeMemory( bp->hostOutputChannels[0] );
}
void PaUtil_ResetBufferProcessor( PaUtilBufferProcessor* bp )
{
unsigned long tempInputBufferSize, tempOutputBufferSize;
bp->framesInTempInputBuffer = bp->initialFramesInTempInputBuffer;
bp->framesInTempOutputBuffer = bp->initialFramesInTempOutputBuffer;
if( bp->framesInTempInputBuffer > 0 )
{
tempInputBufferSize =
bp->framesPerTempBuffer * bp->bytesPerUserInputSample * bp->inputChannelCount;
memset( bp->tempInputBuffer, 0, tempInputBufferSize );
}
if( bp->framesInTempOutputBuffer > 0 )
{
tempOutputBufferSize =
bp->framesPerTempBuffer * bp->bytesPerUserOutputSample * bp->outputChannelCount;
memset( bp->tempOutputBuffer, 0, tempOutputBufferSize );
}
}
unsigned long PaUtil_GetBufferProcessorInputLatencyFrames( PaUtilBufferProcessor* bp )
{
return bp->initialFramesInTempInputBuffer;
}
unsigned long PaUtil_GetBufferProcessorOutputLatencyFrames( PaUtilBufferProcessor* bp )
{
return bp->initialFramesInTempOutputBuffer;
}
void PaUtil_SetInputFrameCount( PaUtilBufferProcessor* bp,
unsigned long frameCount )
{
if( frameCount == 0 )
bp->hostInputFrameCount[0] = bp->framesPerHostBuffer;
else
bp->hostInputFrameCount[0] = frameCount;
}
void PaUtil_SetNoInput( PaUtilBufferProcessor* bp )
{
assert( bp->inputChannelCount > 0 );
bp->hostInputChannels[0][0].data = 0;
}
void PaUtil_SetInputChannel( PaUtilBufferProcessor* bp,
unsigned int channel, void *data, unsigned int stride )
{
assert( channel < bp->inputChannelCount );
bp->hostInputChannels[0][channel].data = data;
bp->hostInputChannels[0][channel].stride = stride;
}
void PaUtil_SetInterleavedInputChannels( PaUtilBufferProcessor* bp,
unsigned int firstChannel, void *data, unsigned int channelCount )
{
unsigned int i;
unsigned int channel = firstChannel;
unsigned char *p = (unsigned char*)data;
if( channelCount == 0 )
channelCount = bp->inputChannelCount;
assert( firstChannel < bp->inputChannelCount );
assert( firstChannel + channelCount <= bp->inputChannelCount );
assert( bp->hostInputIsInterleaved );
for( i=0; i< channelCount; ++i )
{
bp->hostInputChannels[0][channel+i].data = p;
p += bp->bytesPerHostInputSample;
bp->hostInputChannels[0][channel+i].stride = channelCount;
}
}
void PaUtil_SetNonInterleavedInputChannel( PaUtilBufferProcessor* bp,
unsigned int channel, void *data )
{
assert( channel < bp->inputChannelCount );
assert( !bp->hostInputIsInterleaved );
bp->hostInputChannels[0][channel].data = data;
bp->hostInputChannels[0][channel].stride = 1;
}
void PaUtil_Set2ndInputFrameCount( PaUtilBufferProcessor* bp,
unsigned long frameCount )
{
bp->hostInputFrameCount[1] = frameCount;
}
void PaUtil_Set2ndInputChannel( PaUtilBufferProcessor* bp,
unsigned int channel, void *data, unsigned int stride )
{
assert( channel < bp->inputChannelCount );
bp->hostInputChannels[1][channel].data = data;
bp->hostInputChannels[1][channel].stride = stride;
}
void PaUtil_Set2ndInterleavedInputChannels( PaUtilBufferProcessor* bp,
unsigned int firstChannel, void *data, unsigned int channelCount )
{
unsigned int i;
unsigned int channel = firstChannel;
unsigned char *p = (unsigned char*)data;
if( channelCount == 0 )
channelCount = bp->inputChannelCount;
assert( firstChannel < bp->inputChannelCount );
assert( firstChannel + channelCount <= bp->inputChannelCount );
assert( bp->hostInputIsInterleaved );
for( i=0; i< channelCount; ++i )
{
bp->hostInputChannels[1][channel+i].data = p;
p += bp->bytesPerHostInputSample;
bp->hostInputChannels[1][channel+i].stride = channelCount;
}
}
void PaUtil_Set2ndNonInterleavedInputChannel( PaUtilBufferProcessor* bp,
unsigned int channel, void *data )
{
assert( channel < bp->inputChannelCount );
assert( !bp->hostInputIsInterleaved );
bp->hostInputChannels[1][channel].data = data;
bp->hostInputChannels[1][channel].stride = 1;
}
void PaUtil_SetOutputFrameCount( PaUtilBufferProcessor* bp,
unsigned long frameCount )
{
if( frameCount == 0 )
bp->hostOutputFrameCount[0] = bp->framesPerHostBuffer;
else
bp->hostOutputFrameCount[0] = frameCount;
}
void PaUtil_SetNoOutput( PaUtilBufferProcessor* bp )
{
assert( bp->outputChannelCount > 0 );
bp->hostOutputChannels[0][0].data = 0;
}
void PaUtil_SetOutputChannel( PaUtilBufferProcessor* bp,
unsigned int channel, void *data, unsigned int stride )
{
assert( channel < bp->outputChannelCount );
assert( data != NULL );
bp->hostOutputChannels[0][channel].data = data;
bp->hostOutputChannels[0][channel].stride = stride;
}
void PaUtil_SetInterleavedOutputChannels( PaUtilBufferProcessor* bp,
unsigned int firstChannel, void *data, unsigned int channelCount )
{
unsigned int i;
unsigned int channel = firstChannel;
unsigned char *p = (unsigned char*)data;
if( channelCount == 0 )
channelCount = bp->outputChannelCount;
assert( firstChannel < bp->outputChannelCount );
assert( firstChannel + channelCount <= bp->outputChannelCount );
assert( bp->hostOutputIsInterleaved );
for( i=0; i< channelCount; ++i )
{
PaUtil_SetOutputChannel( bp, channel + i, p, channelCount );
p += bp->bytesPerHostOutputSample;
}
}
void PaUtil_SetNonInterleavedOutputChannel( PaUtilBufferProcessor* bp,
unsigned int channel, void *data )
{
assert( channel < bp->outputChannelCount );
assert( !bp->hostOutputIsInterleaved );
PaUtil_SetOutputChannel( bp, channel, data, 1 );
}
void PaUtil_Set2ndOutputFrameCount( PaUtilBufferProcessor* bp,
unsigned long frameCount )
{
bp->hostOutputFrameCount[1] = frameCount;
}
void PaUtil_Set2ndOutputChannel( PaUtilBufferProcessor* bp,
unsigned int channel, void *data, unsigned int stride )
{
assert( channel < bp->outputChannelCount );
assert( data != NULL );
bp->hostOutputChannels[1][channel].data = data;
bp->hostOutputChannels[1][channel].stride = stride;
}
void PaUtil_Set2ndInterleavedOutputChannels( PaUtilBufferProcessor* bp,
unsigned int firstChannel, void *data, unsigned int channelCount )
{
unsigned int i;
unsigned int channel = firstChannel;
unsigned char *p = (unsigned char*)data;
if( channelCount == 0 )
channelCount = bp->outputChannelCount;
assert( firstChannel < bp->outputChannelCount );
assert( firstChannel + channelCount <= bp->outputChannelCount );
assert( bp->hostOutputIsInterleaved );
for( i=0; i< channelCount; ++i )
{
PaUtil_Set2ndOutputChannel( bp, channel + i, p, channelCount );
p += bp->bytesPerHostOutputSample;
}
}
void PaUtil_Set2ndNonInterleavedOutputChannel( PaUtilBufferProcessor* bp,
unsigned int channel, void *data )
{
assert( channel < bp->outputChannelCount );
assert( !bp->hostOutputIsInterleaved );
PaUtil_Set2ndOutputChannel( bp, channel, data, 1 );
}
void PaUtil_BeginBufferProcessing( PaUtilBufferProcessor* bp,
PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags callbackStatusFlags )
{
bp->timeInfo = timeInfo;
bp->timeInfo->inputBufferAdcTime -= bp->framesInTempInputBuffer * bp->samplePeriod;
bp->timeInfo->outputBufferDacTime += bp->framesInTempOutputBuffer * bp->samplePeriod;
bp->callbackStatusFlags = callbackStatusFlags;
bp->hostInputFrameCount[1] = 0;
bp->hostOutputFrameCount[1] = 0;
}
static unsigned long NonAdaptingProcess( PaUtilBufferProcessor *bp,
int *streamCallbackResult,
PaUtilChannelDescriptor *hostInputChannels,
PaUtilChannelDescriptor *hostOutputChannels,
unsigned long framesToProcess )
{
void *userInput, *userOutput;
unsigned char *srcBytePtr, *destBytePtr;
unsigned int srcSampleStrideSamples;  
unsigned int srcChannelStrideBytes;  
unsigned int destSampleStrideSamples;  
unsigned int destChannelStrideBytes;  
unsigned int i;
unsigned long frameCount;
unsigned long framesToGo = framesToProcess;
unsigned long framesProcessed = 0;
int skipOutputConvert = 0;
int skipInputConvert = 0;
if( *streamCallbackResult == paContinue )
{
do
{
frameCount = PA_MIN_( bp->framesPerTempBuffer, framesToGo );
if( bp->inputChannelCount == 0 )
{
userInput = 0;
}
else  
{
destBytePtr = (unsigned char *)bp->tempInputBuffer;
if( bp->userInputIsInterleaved )
{
destSampleStrideSamples = bp->inputChannelCount;
destChannelStrideBytes = bp->bytesPerUserInputSample;
if( bp->userInputSampleFormatIsEqualToHost && bp->hostInputIsInterleaved
&& bp->hostInputChannels[0][0].data && bp->inputChannelCount == hostInputChannels[0].stride )
{
userInput = hostInputChannels[0].data;
destBytePtr = (unsigned char *)hostInputChannels[0].data;
skipInputConvert = 1;
}
else
{
userInput = bp->tempInputBuffer;
}
}
else  
{
destSampleStrideSamples = 1;
destChannelStrideBytes = frameCount * bp->bytesPerUserInputSample;
if( bp->userInputSampleFormatIsEqualToHost && !bp->hostInputIsInterleaved && bp->hostInputChannels[0][0].data )
{
for( i=0; i<bp->inputChannelCount; ++i )
{
bp->tempInputBufferPtrs[i] = hostInputChannels[i].data;
}
skipInputConvert = 1;
}
else
{
for( i=0; i<bp->inputChannelCount; ++i )
{
bp->tempInputBufferPtrs[i] = ((unsigned char*)bp->tempInputBuffer) +
i * bp->bytesPerUserInputSample * frameCount;
}
}
userInput = bp->tempInputBufferPtrs;
}
if( !bp->hostInputChannels[0][0].data )
{
for( i=0; i<bp->inputChannelCount; ++i )
{
bp->inputZeroer( destBytePtr, destSampleStrideSamples, frameCount );
destBytePtr += destChannelStrideBytes;   
}
}
else
{
if( skipInputConvert )
{
for( i=0; i<bp->inputChannelCount; ++i )
{
hostInputChannels[i].data = ((unsigned char*)hostInputChannels[i].data) +
frameCount * hostInputChannels[i].stride * bp->bytesPerHostInputSample;
}
}
else
{
for( i=0; i<bp->inputChannelCount; ++i )
{
bp->inputConverter( destBytePtr, destSampleStrideSamples,
hostInputChannels[i].data,
hostInputChannels[i].stride,
frameCount, &bp->ditherGenerator );
destBytePtr += destChannelStrideBytes;   
hostInputChannels[i].data = ((unsigned char*)hostInputChannels[i].data) +
frameCount * hostInputChannels[i].stride * bp->bytesPerHostInputSample;
}
}
}
}
if( bp->outputChannelCount == 0 )
{
userOutput = 0;
}
else  
{
if( bp->userOutputIsInterleaved )
{
if( bp->userOutputSampleFormatIsEqualToHost && bp->hostOutputIsInterleaved
&& bp->outputChannelCount == hostOutputChannels[0].stride )
{
userOutput = hostOutputChannels[0].data;
skipOutputConvert = 1;
}
else
{
userOutput = bp->tempOutputBuffer;
}
}
else  
{
if( bp->userOutputSampleFormatIsEqualToHost && !bp->hostOutputIsInterleaved )
{
for( i=0; i<bp->outputChannelCount; ++i )
{
bp->tempOutputBufferPtrs[i] = hostOutputChannels[i].data;
}
skipOutputConvert = 1;
}
else
{
for( i=0; i<bp->outputChannelCount; ++i )
{
bp->tempOutputBufferPtrs[i] = ((unsigned char*)bp->tempOutputBuffer) +
i * bp->bytesPerUserOutputSample * frameCount;
}
}
userOutput = bp->tempOutputBufferPtrs;
}
}
*streamCallbackResult = bp->streamCallback( userInput, userOutput,
frameCount, bp->timeInfo, bp->callbackStatusFlags, bp->userData );
if( *streamCallbackResult == paAbort )
{
}
else
{
bp->timeInfo->inputBufferAdcTime += frameCount * bp->samplePeriod;
bp->timeInfo->outputBufferDacTime += frameCount * bp->samplePeriod;
if( bp->outputChannelCount != 0 && bp->hostOutputChannels[0][0].data )
{
if( skipOutputConvert )
{
for( i=0; i<bp->outputChannelCount; ++i )
{
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
frameCount * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
}
else
{
srcBytePtr = (unsigned char *)bp->tempOutputBuffer;
if( bp->userOutputIsInterleaved )
{
srcSampleStrideSamples = bp->outputChannelCount;
srcChannelStrideBytes = bp->bytesPerUserOutputSample;
}
else  
{
srcSampleStrideSamples = 1;
srcChannelStrideBytes = frameCount * bp->bytesPerUserOutputSample;
}
for( i=0; i<bp->outputChannelCount; ++i )
{
bp->outputConverter(    hostOutputChannels[i].data,
hostOutputChannels[i].stride,
srcBytePtr, srcSampleStrideSamples,
frameCount, &bp->ditherGenerator );
srcBytePtr += srcChannelStrideBytes;   
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
frameCount * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
}
}
framesProcessed += frameCount;
framesToGo -= frameCount;
}
}
while( framesToGo > 0  && *streamCallbackResult == paContinue );
}
if( framesToGo > 0 )
{
frameCount = framesToGo;
if( bp->outputChannelCount != 0 && bp->hostOutputChannels[0][0].data )
{
for( i=0; i<bp->outputChannelCount; ++i )
{
bp->outputZeroer(   hostOutputChannels[i].data,
hostOutputChannels[i].stride,
frameCount );
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
frameCount * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
}
framesProcessed += frameCount;
}
return framesProcessed;
}
static unsigned long AdaptingInputOnlyProcess( PaUtilBufferProcessor *bp,
int *streamCallbackResult,
PaUtilChannelDescriptor *hostInputChannels,
unsigned long framesToProcess )
{
void *userInput, *userOutput;
unsigned char *destBytePtr;
unsigned int destSampleStrideSamples;  
unsigned int destChannelStrideBytes;  
unsigned int i;
unsigned long frameCount;
unsigned long framesToGo = framesToProcess;
unsigned long framesProcessed = 0;
userOutput = 0;
do
{
frameCount = ( bp->framesInTempInputBuffer + framesToGo > bp->framesPerUserBuffer )
? ( bp->framesPerUserBuffer - bp->framesInTempInputBuffer )
: framesToGo;
if( bp->userInputIsInterleaved )
{
destBytePtr = ((unsigned char*)bp->tempInputBuffer) +
bp->bytesPerUserInputSample * bp->inputChannelCount *
bp->framesInTempInputBuffer;
destSampleStrideSamples = bp->inputChannelCount;
destChannelStrideBytes = bp->bytesPerUserInputSample;
userInput = bp->tempInputBuffer;
}
else  
{
destBytePtr = ((unsigned char*)bp->tempInputBuffer) +
bp->bytesPerUserInputSample * bp->framesInTempInputBuffer;
destSampleStrideSamples = 1;
destChannelStrideBytes = bp->framesPerUserBuffer * bp->bytesPerUserInputSample;
for( i=0; i<bp->inputChannelCount; ++i )
{
bp->tempInputBufferPtrs[i] = ((unsigned char*)bp->tempInputBuffer) +
i * bp->bytesPerUserInputSample * bp->framesPerUserBuffer;
}
userInput = bp->tempInputBufferPtrs;
}
for( i=0; i<bp->inputChannelCount; ++i )
{
bp->inputConverter( destBytePtr, destSampleStrideSamples,
hostInputChannels[i].data,
hostInputChannels[i].stride,
frameCount, &bp->ditherGenerator );
destBytePtr += destChannelStrideBytes;   
hostInputChannels[i].data = ((unsigned char*)hostInputChannels[i].data) +
frameCount * hostInputChannels[i].stride * bp->bytesPerHostInputSample;
}
bp->framesInTempInputBuffer += frameCount;
if( bp->framesInTempInputBuffer == bp->framesPerUserBuffer )
{
if( *streamCallbackResult == paContinue )
{
bp->timeInfo->outputBufferDacTime = 0;
*streamCallbackResult = bp->streamCallback( userInput, userOutput,
bp->framesPerUserBuffer, bp->timeInfo,
bp->callbackStatusFlags, bp->userData );
bp->timeInfo->inputBufferAdcTime += bp->framesPerUserBuffer * bp->samplePeriod;
}
bp->framesInTempInputBuffer = 0;
}
framesProcessed += frameCount;
framesToGo -= frameCount;
}while( framesToGo > 0 );
return framesProcessed;
}
static unsigned long AdaptingOutputOnlyProcess( PaUtilBufferProcessor *bp,
int *streamCallbackResult,
PaUtilChannelDescriptor *hostOutputChannels,
unsigned long framesToProcess )
{
void *userInput, *userOutput;
unsigned char *srcBytePtr;
unsigned int srcSampleStrideSamples;  
unsigned int srcChannelStrideBytes;   
unsigned int i;
unsigned long frameCount;
unsigned long framesToGo = framesToProcess;
unsigned long framesProcessed = 0;
do
{
if( bp->framesInTempOutputBuffer == 0 && *streamCallbackResult == paContinue )
{
userInput = 0;
if( bp->userOutputIsInterleaved )
{
userOutput = bp->tempOutputBuffer;
}
else  
{
for( i = 0; i < bp->outputChannelCount; ++i )
{
bp->tempOutputBufferPtrs[i] = ((unsigned char*)bp->tempOutputBuffer) +
i * bp->framesPerUserBuffer * bp->bytesPerUserOutputSample;
}
userOutput = bp->tempOutputBufferPtrs;
}
bp->timeInfo->inputBufferAdcTime = 0;
*streamCallbackResult = bp->streamCallback( userInput, userOutput,
bp->framesPerUserBuffer, bp->timeInfo,
bp->callbackStatusFlags, bp->userData );
if( *streamCallbackResult == paAbort )
{
}
else
{
bp->timeInfo->outputBufferDacTime += bp->framesPerUserBuffer * bp->samplePeriod;
bp->framesInTempOutputBuffer = bp->framesPerUserBuffer;
}
}
if( bp->framesInTempOutputBuffer > 0 )
{
frameCount = PA_MIN_( bp->framesInTempOutputBuffer, framesToGo );
if( bp->userOutputIsInterleaved )
{
srcBytePtr = ((unsigned char*)bp->tempOutputBuffer) +
bp->bytesPerUserOutputSample * bp->outputChannelCount *
(bp->framesPerUserBuffer - bp->framesInTempOutputBuffer);
srcSampleStrideSamples = bp->outputChannelCount;
srcChannelStrideBytes = bp->bytesPerUserOutputSample;
}
else  
{
srcBytePtr = ((unsigned char*)bp->tempOutputBuffer) +
bp->bytesPerUserOutputSample *
(bp->framesPerUserBuffer - bp->framesInTempOutputBuffer);
srcSampleStrideSamples = 1;
srcChannelStrideBytes = bp->framesPerUserBuffer * bp->bytesPerUserOutputSample;
}
for( i=0; i<bp->outputChannelCount; ++i )
{
bp->outputConverter(    hostOutputChannels[i].data,
hostOutputChannels[i].stride,
srcBytePtr, srcSampleStrideSamples,
frameCount, &bp->ditherGenerator );
srcBytePtr += srcChannelStrideBytes;   
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
frameCount * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
bp->framesInTempOutputBuffer -= frameCount;
}
else
{
frameCount = framesToGo;
for( i=0; i<bp->outputChannelCount; ++i )
{
bp->outputZeroer(   hostOutputChannels[i].data,
hostOutputChannels[i].stride,
frameCount );
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
frameCount * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
}
framesProcessed += frameCount;
framesToGo -= frameCount;
}while( framesToGo > 0 );
return framesProcessed;
}
static void CopyTempOutputBuffersToHostOutputBuffers( PaUtilBufferProcessor *bp)
{
unsigned long maxFramesToCopy;
PaUtilChannelDescriptor *hostOutputChannels;
unsigned int frameCount;
unsigned char *srcBytePtr;
unsigned int srcSampleStrideSamples;  
unsigned int srcChannelStrideBytes;  
unsigned int i;
while( bp->framesInTempOutputBuffer > 0 &&
((bp->hostOutputFrameCount[0] + bp->hostOutputFrameCount[1]) > 0) )
{
maxFramesToCopy = bp->framesInTempOutputBuffer;
if( bp->hostOutputFrameCount[0] > 0 )
{
hostOutputChannels = bp->hostOutputChannels[0];
frameCount = PA_MIN_( bp->hostOutputFrameCount[0], maxFramesToCopy );
}
else
{
hostOutputChannels = bp->hostOutputChannels[1];
frameCount = PA_MIN_( bp->hostOutputFrameCount[1], maxFramesToCopy );
}
if( bp->userOutputIsInterleaved )
{
srcBytePtr = ((unsigned char*)bp->tempOutputBuffer) +
bp->bytesPerUserOutputSample * bp->outputChannelCount *
(bp->framesPerUserBuffer - bp->framesInTempOutputBuffer);
srcSampleStrideSamples = bp->outputChannelCount;
srcChannelStrideBytes = bp->bytesPerUserOutputSample;
}
else  
{
srcBytePtr = ((unsigned char*)bp->tempOutputBuffer) +
bp->bytesPerUserOutputSample *
(bp->framesPerUserBuffer - bp->framesInTempOutputBuffer);
srcSampleStrideSamples = 1;
srcChannelStrideBytes = bp->framesPerUserBuffer * bp->bytesPerUserOutputSample;
}
for( i=0; i<bp->outputChannelCount; ++i )
{
assert( hostOutputChannels[i].data != NULL );
bp->outputConverter(    hostOutputChannels[i].data,
hostOutputChannels[i].stride,
srcBytePtr, srcSampleStrideSamples,
frameCount, &bp->ditherGenerator );
srcBytePtr += srcChannelStrideBytes;   
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
frameCount * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
if( bp->hostOutputFrameCount[0] > 0 )
bp->hostOutputFrameCount[0] -= frameCount;
else
bp->hostOutputFrameCount[1] -= frameCount;
bp->framesInTempOutputBuffer -= frameCount;
}
}
static unsigned long AdaptingProcess( PaUtilBufferProcessor *bp,
int *streamCallbackResult, int processPartialUserBuffers )
{
void *userInput, *userOutput;
unsigned long framesProcessed = 0;
unsigned long framesAvailable;
unsigned long endProcessingMinFrameCount;
unsigned long maxFramesToCopy;
PaUtilChannelDescriptor *hostInputChannels, *hostOutputChannels;
unsigned int frameCount;
unsigned char *destBytePtr;
unsigned int destSampleStrideSamples;  
unsigned int destChannelStrideBytes;  
unsigned int i, j;
framesAvailable = bp->hostInputFrameCount[0] + bp->hostInputFrameCount[1]; 
if( processPartialUserBuffers )
endProcessingMinFrameCount = 0;
else
endProcessingMinFrameCount = (bp->framesPerUserBuffer - 1);
CopyTempOutputBuffersToHostOutputBuffers( bp );
while( framesAvailable > endProcessingMinFrameCount )
{
if( bp->framesInTempOutputBuffer == 0 && *streamCallbackResult != paContinue )
{
for( i=0; i<2; ++i )
{
frameCount = bp->hostOutputFrameCount[i];
if( frameCount > 0 )
{
hostOutputChannels = bp->hostOutputChannels[i];
for( j=0; j<bp->outputChannelCount; ++j )
{
bp->outputZeroer(   hostOutputChannels[j].data,
hostOutputChannels[j].stride,
frameCount );
hostOutputChannels[j].data = ((unsigned char*)hostOutputChannels[j].data) +
frameCount * hostOutputChannels[j].stride * bp->bytesPerHostOutputSample;
}
bp->hostOutputFrameCount[i] = 0;
}
}
}
while( bp->framesInTempInputBuffer < bp->framesPerUserBuffer &&
((bp->hostInputFrameCount[0] + bp->hostInputFrameCount[1]) > 0) )
{
maxFramesToCopy = bp->framesPerUserBuffer - bp->framesInTempInputBuffer;
if( bp->hostInputFrameCount[0] > 0 )
{
hostInputChannels = bp->hostInputChannels[0];
frameCount = PA_MIN_( bp->hostInputFrameCount[0], maxFramesToCopy );
}
else
{
hostInputChannels = bp->hostInputChannels[1];
frameCount = PA_MIN_( bp->hostInputFrameCount[1], maxFramesToCopy );
}
if( bp->userInputIsInterleaved )
{
destBytePtr = ((unsigned char*)bp->tempInputBuffer) +
bp->bytesPerUserInputSample * bp->inputChannelCount *
bp->framesInTempInputBuffer;
destSampleStrideSamples = bp->inputChannelCount;
destChannelStrideBytes = bp->bytesPerUserInputSample;
}
else  
{
destBytePtr = ((unsigned char*)bp->tempInputBuffer) +
bp->bytesPerUserInputSample * bp->framesInTempInputBuffer;
destSampleStrideSamples = 1;
destChannelStrideBytes = bp->framesPerUserBuffer * bp->bytesPerUserInputSample;
}
for( i=0; i<bp->inputChannelCount; ++i )
{
bp->inputConverter( destBytePtr, destSampleStrideSamples,
hostInputChannels[i].data,
hostInputChannels[i].stride,
frameCount, &bp->ditherGenerator );
destBytePtr += destChannelStrideBytes;   
hostInputChannels[i].data = ((unsigned char*)hostInputChannels[i].data) +
frameCount * hostInputChannels[i].stride * bp->bytesPerHostInputSample;
}
if( bp->hostInputFrameCount[0] > 0 )
bp->hostInputFrameCount[0] -= frameCount;
else
bp->hostInputFrameCount[1] -= frameCount;
bp->framesInTempInputBuffer += frameCount;
framesAvailable -= frameCount;
framesProcessed += frameCount;
}
if( bp->framesInTempInputBuffer == bp->framesPerUserBuffer &&
bp->framesInTempOutputBuffer == 0 )
{
if( *streamCallbackResult == paContinue )
{
if( bp->userInputIsInterleaved )
{
userInput = bp->tempInputBuffer;
}
else  
{
for( i = 0; i < bp->inputChannelCount; ++i )
{
bp->tempInputBufferPtrs[i] = ((unsigned char*)bp->tempInputBuffer) +
i * bp->framesPerUserBuffer * bp->bytesPerUserInputSample;
}
userInput = bp->tempInputBufferPtrs;
}
if( bp->userOutputIsInterleaved )
{
userOutput = bp->tempOutputBuffer;
}
else  
{
for( i = 0; i < bp->outputChannelCount; ++i )
{
bp->tempOutputBufferPtrs[i] = ((unsigned char*)bp->tempOutputBuffer) +
i * bp->framesPerUserBuffer * bp->bytesPerUserOutputSample;
}
userOutput = bp->tempOutputBufferPtrs;
}
*streamCallbackResult = bp->streamCallback( userInput, userOutput,
bp->framesPerUserBuffer, bp->timeInfo,
bp->callbackStatusFlags, bp->userData );
bp->timeInfo->inputBufferAdcTime += bp->framesPerUserBuffer * bp->samplePeriod;
bp->timeInfo->outputBufferDacTime += bp->framesPerUserBuffer * bp->samplePeriod;
bp->framesInTempInputBuffer = 0;
if( *streamCallbackResult == paAbort )
bp->framesInTempOutputBuffer = 0;
else
bp->framesInTempOutputBuffer = bp->framesPerUserBuffer;
}
else
{
bp->framesInTempInputBuffer = 0;
}
}
CopyTempOutputBuffersToHostOutputBuffers( bp );
}
return framesProcessed;
}
unsigned long PaUtil_EndBufferProcessing( PaUtilBufferProcessor* bp, int *streamCallbackResult )
{
unsigned long framesToProcess, framesToGo;
unsigned long framesProcessed = 0;
if( bp->inputChannelCount != 0 && bp->outputChannelCount != 0
&& bp->hostInputChannels[0][0].data  
&& bp->hostOutputChannels[0][0].data   )
{
assert( (bp->hostInputFrameCount[0] + bp->hostInputFrameCount[1]) ==
(bp->hostOutputFrameCount[0] + bp->hostOutputFrameCount[1]) );
}
assert( *streamCallbackResult == paContinue
|| *streamCallbackResult == paComplete
|| *streamCallbackResult == paAbort );  
if( bp->useNonAdaptingProcess )
{
if( bp->inputChannelCount != 0 && bp->outputChannelCount != 0 )
{
framesToGo = bp->hostOutputFrameCount[0] + bp->hostOutputFrameCount[1];  
do{
unsigned long noInputInputFrameCount;
unsigned long *hostInputFrameCount;
PaUtilChannelDescriptor *hostInputChannels;
unsigned long noOutputOutputFrameCount;
unsigned long *hostOutputFrameCount;
PaUtilChannelDescriptor *hostOutputChannels;
unsigned long framesProcessedThisIteration;
if( !bp->hostInputChannels[0][0].data )
{
noInputInputFrameCount = framesToGo;
hostInputFrameCount = &noInputInputFrameCount;
hostInputChannels = 0;
}
else if( bp->hostInputFrameCount[0] != 0 )
{
hostInputFrameCount = &bp->hostInputFrameCount[0];
hostInputChannels = bp->hostInputChannels[0];
}
else
{
hostInputFrameCount = &bp->hostInputFrameCount[1];
hostInputChannels = bp->hostInputChannels[1];
}
if( !bp->hostOutputChannels[0][0].data )
{
noOutputOutputFrameCount = framesToGo;
hostOutputFrameCount = &noOutputOutputFrameCount;
hostOutputChannels = 0;
}
if( bp->hostOutputFrameCount[0] != 0 )
{
hostOutputFrameCount = &bp->hostOutputFrameCount[0];
hostOutputChannels = bp->hostOutputChannels[0];
}
else
{
hostOutputFrameCount = &bp->hostOutputFrameCount[1];
hostOutputChannels = bp->hostOutputChannels[1];
}
framesToProcess = PA_MIN_( *hostInputFrameCount,
*hostOutputFrameCount );
assert( framesToProcess != 0 );
framesProcessedThisIteration = NonAdaptingProcess( bp, streamCallbackResult,
hostInputChannels, hostOutputChannels,
framesToProcess );
*hostInputFrameCount -= framesProcessedThisIteration;
*hostOutputFrameCount -= framesProcessedThisIteration;
framesProcessed += framesProcessedThisIteration;
framesToGo -= framesProcessedThisIteration;
}while( framesToGo > 0 );
}
else
{
framesToProcess = (bp->inputChannelCount != 0)
? bp->hostInputFrameCount[0]
: bp->hostOutputFrameCount[0];
framesProcessed = NonAdaptingProcess( bp, streamCallbackResult,
bp->hostInputChannels[0], bp->hostOutputChannels[0],
framesToProcess );
framesToProcess = (bp->inputChannelCount != 0)
? bp->hostInputFrameCount[1]
: bp->hostOutputFrameCount[1];
if( framesToProcess > 0 )
{
framesProcessed += NonAdaptingProcess( bp, streamCallbackResult,
bp->hostInputChannels[1], bp->hostOutputChannels[1],
framesToProcess );
}
}
}
else  
{
if( bp->inputChannelCount != 0 && bp->outputChannelCount != 0 )
{
if( bp->hostBufferSizeMode == paUtilVariableHostBufferSizePartialUsageAllowed  )
{
framesProcessed = AdaptingProcess( bp, streamCallbackResult,
0   );
}
else
{
framesProcessed = AdaptingProcess( bp, streamCallbackResult,
1   );
}
}
else if( bp->inputChannelCount != 0 )
{
framesToProcess = bp->hostInputFrameCount[0];
framesProcessed = AdaptingInputOnlyProcess( bp, streamCallbackResult,
bp->hostInputChannels[0], framesToProcess );
framesToProcess = bp->hostInputFrameCount[1];
if( framesToProcess > 0 )
{
framesProcessed += AdaptingInputOnlyProcess( bp, streamCallbackResult,
bp->hostInputChannels[1], framesToProcess );
}
}
else
{
framesToProcess = bp->hostOutputFrameCount[0];
framesProcessed = AdaptingOutputOnlyProcess( bp, streamCallbackResult,
bp->hostOutputChannels[0], framesToProcess );
framesToProcess = bp->hostOutputFrameCount[1];
if( framesToProcess > 0 )
{
framesProcessed += AdaptingOutputOnlyProcess( bp, streamCallbackResult,
bp->hostOutputChannels[1], framesToProcess );
}
}
}
return framesProcessed;
}
int PaUtil_IsBufferProcessorOutputEmpty( PaUtilBufferProcessor* bp )
{
return (bp->framesInTempOutputBuffer) ? 0 : 1;
}
unsigned long PaUtil_CopyInput( PaUtilBufferProcessor* bp,
void **buffer, unsigned long frameCount )
{
PaUtilChannelDescriptor *hostInputChannels;
unsigned int framesToCopy;
unsigned char *destBytePtr;
void **nonInterleavedDestPtrs;
unsigned int destSampleStrideSamples;  
unsigned int destChannelStrideBytes;  
unsigned int i;
hostInputChannels = bp->hostInputChannels[0];
framesToCopy = PA_MIN_( bp->hostInputFrameCount[0], frameCount );
if( bp->userInputIsInterleaved )
{
destBytePtr = (unsigned char*)*buffer;
destSampleStrideSamples = bp->inputChannelCount;
destChannelStrideBytes = bp->bytesPerUserInputSample;
for( i=0; i<bp->inputChannelCount; ++i )
{
bp->inputConverter( destBytePtr, destSampleStrideSamples,
hostInputChannels[i].data,
hostInputChannels[i].stride,
framesToCopy, &bp->ditherGenerator );
destBytePtr += destChannelStrideBytes;   
hostInputChannels[i].data = ((unsigned char*)hostInputChannels[i].data) +
framesToCopy * hostInputChannels[i].stride * bp->bytesPerHostInputSample;
}
*buffer = ((unsigned char *)*buffer) +
framesToCopy * bp->inputChannelCount * bp->bytesPerUserInputSample;
}
else
{
nonInterleavedDestPtrs = (void**)*buffer;
destSampleStrideSamples = 1;
for( i=0; i<bp->inputChannelCount; ++i )
{
destBytePtr = (unsigned char*)nonInterleavedDestPtrs[i];
bp->inputConverter( destBytePtr, destSampleStrideSamples,
hostInputChannels[i].data,
hostInputChannels[i].stride,
framesToCopy, &bp->ditherGenerator );
destBytePtr += bp->bytesPerUserInputSample * framesToCopy;
nonInterleavedDestPtrs[i] = destBytePtr;
hostInputChannels[i].data = ((unsigned char*)hostInputChannels[i].data) +
framesToCopy * hostInputChannels[i].stride * bp->bytesPerHostInputSample;
}
}
bp->hostInputFrameCount[0] -= framesToCopy;
return framesToCopy;
}
unsigned long PaUtil_CopyOutput( PaUtilBufferProcessor* bp,
const void ** buffer, unsigned long frameCount )
{
PaUtilChannelDescriptor *hostOutputChannels;
unsigned int framesToCopy;
unsigned char *srcBytePtr;
void **nonInterleavedSrcPtrs;
unsigned int srcSampleStrideSamples;  
unsigned int srcChannelStrideBytes;  
unsigned int i;
hostOutputChannels = bp->hostOutputChannels[0];
framesToCopy = PA_MIN_( bp->hostOutputFrameCount[0], frameCount );
if( bp->userOutputIsInterleaved )
{
srcBytePtr = (unsigned char*)*buffer;
srcSampleStrideSamples = bp->outputChannelCount;
srcChannelStrideBytes = bp->bytesPerUserOutputSample;
for( i=0; i<bp->outputChannelCount; ++i )
{
bp->outputConverter(    hostOutputChannels[i].data,
hostOutputChannels[i].stride,
srcBytePtr, srcSampleStrideSamples,
framesToCopy, &bp->ditherGenerator );
srcBytePtr += srcChannelStrideBytes;   
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
framesToCopy * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
*buffer = ((unsigned char *)*buffer) +
framesToCopy * bp->outputChannelCount * bp->bytesPerUserOutputSample;
}
else
{
nonInterleavedSrcPtrs = (void**)*buffer;
srcSampleStrideSamples = 1;
for( i=0; i<bp->outputChannelCount; ++i )
{
srcBytePtr = (unsigned char*)nonInterleavedSrcPtrs[i];
bp->outputConverter(    hostOutputChannels[i].data,
hostOutputChannels[i].stride,
srcBytePtr, srcSampleStrideSamples,
framesToCopy, &bp->ditherGenerator );
srcBytePtr += bp->bytesPerUserOutputSample * framesToCopy;
nonInterleavedSrcPtrs[i] = srcBytePtr;
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
framesToCopy * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
}
bp->hostOutputFrameCount[0] += framesToCopy;
return framesToCopy;
}
unsigned long PaUtil_ZeroOutput( PaUtilBufferProcessor* bp, unsigned long frameCount )
{
PaUtilChannelDescriptor *hostOutputChannels;
unsigned int framesToZero;
unsigned int i;
hostOutputChannels = bp->hostOutputChannels[0];
framesToZero = PA_MIN_( bp->hostOutputFrameCount[0], frameCount );
for( i=0; i<bp->outputChannelCount; ++i )
{
bp->outputZeroer(   hostOutputChannels[i].data,
hostOutputChannels[i].stride,
framesToZero );
hostOutputChannels[i].data = ((unsigned char*)hostOutputChannels[i].data) +
framesToZero * hostOutputChannels[i].stride * bp->bytesPerHostOutputSample;
}
bp->hostOutputFrameCount[0] += framesToZero;
return framesToZero;
}

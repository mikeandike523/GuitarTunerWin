 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define SAMPLE_RATE       (44100)
#define FRAMES_PER_BUFFER (128)
#define FREQ_INCR         (300.0 / SAMPLE_RATE)
#define MAX_CHANNELS      (64)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
typedef struct
{
short   interleaved;           
int     numChannels;           
double  phases[MAX_CHANNELS];  
}
paTestData;
static int patestCallback(const void*                     inputBuffer,
void*                           outputBuffer,
unsigned long                   framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags           statusFlags,
void*                           userData)
{
int         frameIndex, channelIndex;
float**     outputs = (float**)outputBuffer;
paTestData* data    = (paTestData*)userData;
(void) inputBuffer;      
if (data->interleaved)
{
float *out = (float*)outputBuffer;       
for( frameIndex=0; frameIndex<(int)framesPerBuffer; frameIndex++ )
{
for( channelIndex=0; channelIndex<data->numChannels; channelIndex++ )
{
*out++ = (float) sin(data->phases[channelIndex]);
data->phases[channelIndex] += FREQ_INCR * (4 + channelIndex);
if( data->phases[channelIndex] >= (2.0 * M_PI) ) data->phases[channelIndex] -= (2.0 * M_PI);
}
}
}
else
{
for( frameIndex=0; frameIndex<(int)framesPerBuffer; frameIndex++ )
{
for( channelIndex=0; channelIndex<data->numChannels; channelIndex++ )
{
outputs[channelIndex][frameIndex] = (float) sin(data->phases[channelIndex]);
data->phases[channelIndex] += FREQ_INCR * (4 + channelIndex);
if( data->phases[channelIndex] >= (2.0 * M_PI) ) data->phases[channelIndex] -= (2.0 * M_PI);
}
}
}
return 0;
}
int test(short interleaved)
{
PaStream*           stream;
PaStreamParameters  outputParameters;
PaError             err;
const PaDeviceInfo* pdi;
paTestData          data;
short               n;
outputParameters.device = Pa_GetDefaultOutputDevice();   
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
return paInvalidDevice;
}
pdi = Pa_GetDeviceInfo(outputParameters.device);
outputParameters.channelCount = pdi->maxOutputChannels;
if (outputParameters.channelCount > MAX_CHANNELS)
outputParameters.channelCount = MAX_CHANNELS;
outputParameters.sampleFormat = paFloat32;               
outputParameters.suggestedLatency = pdi->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
data.interleaved = interleaved;
data.numChannels = outputParameters.channelCount;
for (n = 0; n < data.numChannels; n++)
data.phases[n] = 0.0;  
printf("%d ", data.numChannels);
if (interleaved)
printf("interleaved ");
else
{
printf(" non-interleaved ");
outputParameters.sampleFormat |= paNonInterleaved;
}
printf("channels.\n");
err = Pa_OpenStream(&stream,
NULL,                
&outputParameters,
SAMPLE_RATE,         
FRAMES_PER_BUFFER,   
paClipOff,           
patestCallback,
&data);
if (err == paNoError)
{
err = Pa_StartStream(stream);
if (err == paNoError)
{
printf("Hit ENTER to stop this test.\n");
getchar();
err = Pa_StopStream(stream);
}
Pa_CloseStream( stream );
}
return err;
}
int main(void)
{
PaError err;
printf("PortAudio Test: output sine wave on each channel.\n" );
err = Pa_Initialize();
if (err != paNoError)
goto done;
err = test(1);           
if (err != paNoError)
goto done;
err = test(0);           
if (err != paNoError)
goto done;
printf("Test finished.\n");
done:
if (err)
{
fprintf(stderr, "An error occurred while using the portaudio stream\n");
fprintf(stderr, "Error number: %d\n", err );
fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
}
Pa_Terminate();
return 0;
}

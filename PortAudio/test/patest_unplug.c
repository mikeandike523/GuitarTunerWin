 
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (8)
#define SAMPLE_RATE   (44100)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE        (200)
#define FRAMES_PER_BUFFER  (64)
#define MAX_CHANNELS        (8)
typedef struct
{
short sine[TABLE_SIZE];
int   phases[MAX_CHANNELS];
int   numChannels;
int   sampsToGo;
}
paTestData;
static int inputCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
int finished = 0;
(void) inputBuffer;  
(void) outputBuffer;  
data->sampsToGo -= framesPerBuffer;
if (data->sampsToGo <= 0)
{
data->sampsToGo = 0;
finished = 1;
}
return finished;
}
static int outputCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
short *out = (short*)outputBuffer;
unsigned int i;
int finished = 0;
(void) inputBuffer;  
for( i=0; i<framesPerBuffer; i++ )
{
for (int channelIndex = 0; channelIndex < data->numChannels; channelIndex++)
{
int phase = data->phases[channelIndex];
*out++ = data->sine[phase];
phase += channelIndex + 2;
if( phase >= TABLE_SIZE ) phase -= TABLE_SIZE;
data->phases[channelIndex] = phase;
}
}
return finished;
}
int main(int argc, char **args);
int main(int argc, char **args)
{
PaStreamParameters inputParameters;
PaStreamParameters outputParameters;
PaStream *inputStream;
PaStream *outputStream;
const PaDeviceInfo *deviceInfo;
PaError err;
paTestData data;
int i;
int totalSamps;
int inputDevice = -1;
int outputDevice = -1;
printf("Test unplugging a USB device.\n");
if( argc > 1 ) {
inputDevice = outputDevice = atoi( args[1] );
printf("Using device number %d.\n\n", inputDevice );
} else {
printf("Using default device.\n\n" );
}
memset(&data, 0, sizeof(data));
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (short) (32767.0 * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. ));
}
data.numChannels = 2;
data.sampsToGo = totalSamps =  NUM_SECONDS * SAMPLE_RATE;  
err = Pa_Initialize();
if( err != paNoError ) goto error;
if( inputDevice == -1 )
inputParameters.device = Pa_GetDefaultInputDevice();  
else
inputParameters.device = inputDevice ;
if (inputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default input device.\n");
goto error;
}
if( outputDevice == -1 )
outputParameters.device = Pa_GetDefaultOutputDevice();  
else
outputParameters.device = outputDevice ;
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
inputParameters.channelCount = 2;
inputParameters.sampleFormat = paInt16;
deviceInfo = Pa_GetDeviceInfo( inputParameters.device );
if( deviceInfo == NULL )
{
fprintf( stderr, "No matching input device.\n" );
goto error;
}
inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
inputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&inputStream,
&inputParameters,
NULL,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
0,
inputCallback,
&data );
if( err != paNoError ) goto error;
outputParameters.channelCount = 2;
outputParameters.sampleFormat = paInt16;
deviceInfo = Pa_GetDeviceInfo( outputParameters.device );
if( deviceInfo == NULL )
{
fprintf( stderr, "No matching output device.\n" );
goto error;
}
outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&outputStream,
NULL,
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
(paClipOff | paDitherOff),
outputCallback,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( inputStream );
if( err != paNoError ) goto error;
err = Pa_StartStream( outputStream );
if( err != paNoError ) goto error;
printf("When you hear sound, unplug the USB device.\n");
do
{
Pa_Sleep(500);
printf("Frames remaining = %d\n", data.sampsToGo);
printf("Pa_IsStreamActive(inputStream) = %d\n", Pa_IsStreamActive(inputStream));
printf("Pa_IsStreamActive(outputStream) = %d\n", Pa_IsStreamActive(outputStream));
} while( Pa_IsStreamActive(inputStream) && Pa_IsStreamActive(outputStream) );
err = Pa_CloseStream( inputStream );
if( err != paNoError ) goto error;
err = Pa_CloseStream( outputStream );
if( err != paNoError ) goto error;
Pa_Terminate();
return paNoError;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
fprintf( stderr, "Host Error message: %s\n", Pa_GetLastHostErrorInfo()->errorText );
return err;
}

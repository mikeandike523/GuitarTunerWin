 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define OUTPUT_DEVICE       (Pa_GetDefaultOutputDeviceID())
#define SAMPLE_RATE_1       (44100)
#define SAMPLE_RATE_2       (48000)
#define FRAMES_PER_BUFFER   (256)
#define FREQ_INCR           (0.1)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
typedef struct
{
double phase;
int    numFrames;
} paTestData;
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
int frameIndex;
(void) timeInfo;  
(void) inputBuffer;
for( frameIndex=0; frameIndex<(int)framesPerBuffer; frameIndex++ )
{
float value = (float) 0.3 * sin(data->phase);
*out++ = value;
*out++ = value;
data->phase += FREQ_INCR;
if( data->phase >= (2.0 * M_PI) ) data->phase -= (2.0 * M_PI);
}
data->numFrames += 1;
return 0;
}
int main(void);
int main(void)
{
PaError err;
PaStreamParameters outputParameters;
PaStream *stream1;
PaStream *stream2;
paTestData data1 = {0};
paTestData data2 = {0};
printf("PortAudio Test: two rates.\n" );
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;        
outputParameters.sampleFormat = paFloat32;  
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream1,
NULL,  
&outputParameters,
SAMPLE_RATE_1,
FRAMES_PER_BUFFER,
paClipOff,       
patestCallback,
&data1 );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream1 );
if( err != paNoError ) goto error;
Pa_Sleep( 3 * 1000 );
err = Pa_OpenStream(
&stream2,
NULL,  
&outputParameters,
SAMPLE_RATE_2,
FRAMES_PER_BUFFER,
paClipOff,       
patestCallback,
&data2 );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream2 );
if( err != paNoError ) goto error;
Pa_Sleep( 3 * 1000 );
err = Pa_StopStream( stream2 );
if( err != paNoError ) goto error;
Pa_Sleep( 3 * 1000 );
err = Pa_StopStream( stream1 );
if( err != paNoError ) goto error;
Pa_CloseStream( stream2 );
Pa_CloseStream( stream1 );
Pa_Terminate();
printf("NumFrames = %d on stream1, %d on stream2.\n", data1.numFrames, data2.numFrames );
printf("Test finished.\n");
return err;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

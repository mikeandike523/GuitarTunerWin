 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (20)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (2048)
#define MSEC_PER_BUFFER  ( (FRAMES_PER_BUFFER * 1000) / SAMPLE_RATE )
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
typedef struct
{
float sine[TABLE_SIZE];
int left_phase;
int right_phase;
int sleepTime;
}
paTestData;
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
unsigned long i;
int finished = 0;
(void) inputBuffer;  
for( i=0; i<framesPerBuffer; i++ )
{
*out++ = data->sine[data->left_phase];   
*out++ = data->sine[data->right_phase];   
data->left_phase += 1;
if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
data->right_phase += 3;  
if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
}
if( data->sleepTime > 0 ) Pa_Sleep( data->sleepTime );
data->sleepTime += 1;
return finished;
}
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
paTestData data;
int i;
printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
data.left_phase = data.right_phase = data.sleepTime = 0;
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
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
patestCallback,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
while( data.sleepTime < (2 * MSEC_PER_BUFFER) )
{
printf("SleepTime = %d\n", data.sleepTime );
Pa_Sleep( data.sleepTime );
}
printf("Try to stop stream.\n");
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Terminate();
printf("Test finished.\n");
return err;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

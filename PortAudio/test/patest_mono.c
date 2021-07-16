 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (10)
#define SAMPLE_RATE   (44100)
#define AMPLITUDE     (0.8)
#define FRAMES_PER_BUFFER  (64)
#define OUTPUT_DEVICE Pa_GetDefaultOutputDevice()
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
typedef struct
{
float sine[TABLE_SIZE];
int phase;
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
(void) timeInfo;
(void) statusFlags;
for( i=0; i<framesPerBuffer; i++ )
{
*out++ = data->sine[data->phase];   
data->phase += 1;
if( data->phase >= TABLE_SIZE ) data->phase -= TABLE_SIZE;
}
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
printf("PortAudio Test: output MONO sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (float) (AMPLITUDE * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. ));
}
data.phase = 0;
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = OUTPUT_DEVICE;
outputParameters.channelCount = 1;        
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
printf("Play for %d seconds.\n", NUM_SECONDS ); fflush(stdout);
Pa_Sleep( NUM_SECONDS * 1000 );
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

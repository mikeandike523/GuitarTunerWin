 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (1)
#define SAMPLE_RATE   (44100)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
typedef struct
{
short sine[TABLE_SIZE];
int left_phase;
int right_phase;
unsigned int sampsToGo;
}
paTestData;
PaError TestOnce( void );
static int patest1Callback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData );
static int patest1Callback( const void *inputBuffer, void *outputBuffer,
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
if( data->sampsToGo < framesPerBuffer )
{
for( i=0; i<data->sampsToGo; i++ )
{
*out++ = data->sine[data->left_phase];   
*out++ = data->sine[data->right_phase];   
data->left_phase += 1;
if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
data->right_phase += 3;  
if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
}
for( ; i<framesPerBuffer; i++ )
{
*out++ = 0;  
*out++ = 0;  
}
finished = 1;
}
else
{
for( i=0; i<framesPerBuffer; i++ )
{
*out++ = data->sine[data->left_phase];   
*out++ = data->sine[data->right_phase];   
data->left_phase += 1;
if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
data->right_phase += 3;  
if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
}
data->sampsToGo -= framesPerBuffer;
}
return finished;
}
#ifdef MACINTOSH
int main(void);
int main(void)
{
int i;
PaError err;
int numLoops = 10;
printf("Loop %d times.\n", numLoops );
for( i=0; i<numLoops; i++ )
{
printf("Loop %d out of %d.\n", i+1, numLoops );
err = TestOnce();
if( err < 0 ) return 0;
}
}
#else
int main(int argc, char **argv);
int main(int argc, char **argv)
{
PaError err;
int i, numLoops = 10;
if( argc > 1 )
{
numLoops = atoi(argv[1]);
}
for( i=0; i<numLoops; i++ )
{
printf("Loop %d out of %d.\n", i+1, numLoops );
err = TestOnce();
if( err < 0 ) return 1;
}
printf("Test complete.\n");
return 0;
}
#endif
PaError TestOnce( void )
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
paTestData data;
int i;
int totalSamps;
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (short) (32767.0 * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. ));
}
data.left_phase = data.right_phase = 0;
data.sampsToGo = totalSamps =  NUM_SECONDS * SAMPLE_RATE;  
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();   
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;                       
outputParameters.sampleFormat = paInt16;
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream,
NULL,          
&outputParameters,
SAMPLE_RATE,
1024,          
paClipOff,     
patest1Callback,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Waiting for sound to finish.\n");
Pa_Sleep(1000);
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Terminate();
return paNoError;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

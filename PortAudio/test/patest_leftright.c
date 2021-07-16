 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (8)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (512)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
#define BALANCE_DELTA  (0.001)
typedef struct
{
float sine[TABLE_SIZE];
int left_phase;
int right_phase;
float targetBalance;  
float currentBalance;
} paTestData;
static int patestCallback( const void *inputBuffer,
void *outputBuffer,
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
if( data->currentBalance < data->targetBalance )
{
data->currentBalance += BALANCE_DELTA;
}
else if( data->currentBalance > data->targetBalance )
{
data->currentBalance -= BALANCE_DELTA;
}
*out++ = data->sine[data->left_phase] * (1.0f - data->currentBalance);   
*out++ = data->sine[data->right_phase] * data->currentBalance;   
data->left_phase += 1;
if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
data->right_phase += 3;  
if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
}
return finished;
}
int main(void);
int main(void)
{
PaStream *stream;
PaStreamParameters outputParameters;
PaError err;
paTestData data;
int i;
printf("Play different tone sine waves that alternate between left and right channel.\n");
printf("The low tone should be on the left channel.\n");
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
data.left_phase = data.right_phase = 0;
data.currentBalance = 0.0;
data.targetBalance = 0.0;
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
err = Pa_OpenStream( &stream,
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
printf("Play for several seconds.\n");
for( i=0; i<4; i++ )
{
printf("Hear low sound on left side.\n");
data.targetBalance = 0.01;
Pa_Sleep( 1000 );
printf("Hear high sound on right side.\n");
data.targetBalance = 0.99;
Pa_Sleep( 1000 );
}
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

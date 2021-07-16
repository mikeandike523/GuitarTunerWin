 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (4)
#define SAMPLE_RATE   (44100)
typedef struct
{
float left_phase;
float right_phase;
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
unsigned int i;
(void) inputBuffer;  
for( i=0; i<framesPerBuffer; i++ )
{
*out++ = data->left_phase;   
*out++ = data->right_phase;   
data->left_phase += 0.01f;
if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
data->right_phase += 0.03f;
if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
}
return 0;
}
static paTestData data;
int main(void);
int main(void)
{
PaStream *stream;
PaError err;
printf("PortAudio Test: output sawtooth wave.\n");
data.left_phase = data.right_phase = 0.0;
err = Pa_Initialize();
if( err != paNoError ) goto error;
err = Pa_OpenDefaultStream( &stream,
0,           
2,           
paFloat32,   
SAMPLE_RATE,
256,         
patestCallback,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
Pa_Sleep(NUM_SECONDS*1000);
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

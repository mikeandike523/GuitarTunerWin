 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (3)
#define SAMPLE_RATE   (44100)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
#define BUFFER_TABLE  14
long buffer_table[] = {paFramesPerBufferUnspecified,16,32,64,128,200,256,500,512,600,723,1000,1024,2345};
typedef struct
{
short sine[TABLE_SIZE];
int left_phase;
int right_phase;
unsigned int sampsToGo;
}
paTestData;
PaError TestOnce( int buffersize, PaDeviceIndex );
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
int main(int argc, char **args);
int main(int argc, char **args)
{
int i;
int device = -1;
PaError err;
printf("Test opening streams with different buffer sizes\n");
if( argc > 1 ) {
device=atoi( args[1] );
printf("Using device number %d.\n\n", device );
} else {
printf("Using default device.\n\n" );
}
for (i = 0 ; i < BUFFER_TABLE; i++)
{
printf("Buffer size %ld\n", buffer_table[i]);
err = TestOnce(buffer_table[i], device);
if( err < 0 ) return 0;
}
return 0;
}
PaError TestOnce( int buffersize, PaDeviceIndex device )
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
if( device == -1 )
outputParameters.device = Pa_GetDefaultOutputDevice();  
else
outputParameters.device = device ;
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
buffersize,                    
(paClipOff | paDitherOff),
patest1Callback,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Waiting for sound to finish.\n");
Pa_Sleep(1000*NUM_SECONDS);
err = Pa_CloseStream( stream );
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

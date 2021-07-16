 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "portaudio.h"
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TWOPI (M_PI * 2.0)
#define DEFAULT_BUFFER_SIZE   (32)
typedef struct
{
double left_phase;
double right_phase;
}
paTestData;
static int paminlatCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
unsigned int i;
double left_phaseInc = 0.02;
double right_phaseInc = 0.06;
double left_phase = data->left_phase;
double right_phase = data->right_phase;
for( i=0; i<framesPerBuffer; i++ )
{
left_phase += left_phaseInc;
if( left_phase > TWOPI ) left_phase -= TWOPI;
*out++ = (float) sin( left_phase );
right_phase += right_phaseInc;
if( right_phase > TWOPI ) right_phase -= TWOPI;
*out++ = (float) sin( right_phase );
}
data->left_phase = left_phase;
data->right_phase = right_phase;
return 0;
}
int main( int argc, char **argv );
int main( int argc, char **argv )
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
paTestData data;
int    go;
int    outLatency = 0;
int    minLatency = DEFAULT_BUFFER_SIZE * 2;
int    framesPerBuffer;
double sampleRate = 44100.0;
char   str[256];
char  *line;
printf("pa_minlat - Determine minimum latency for your computer.\n");
printf("  usage:         pa_minlat {userBufferSize}\n");
printf("  for example:   pa_minlat 64\n");
printf("Adjust your stereo until you hear a smooth tone in each speaker.\n");
printf("Then try to find the smallest number of frames that still sounds smooth.\n");
printf("Note that the sound will stop momentarily when you change the number of buffers.\n");
framesPerBuffer = ( argc > 1 ) ? atol( argv[1] ) : DEFAULT_BUFFER_SIZE;
printf("Frames per buffer = %d\n", framesPerBuffer );
data.left_phase = data.right_phase = 0.0;
err = Pa_Initialize();
if( err != paNoError ) goto error;
outLatency = sampleRate * 200.0 / 1000.0;  
go = 1;
while( go )
{
outputParameters.device                    = Pa_GetDefaultOutputDevice();  
outputParameters.channelCount              = 2;                            
outputParameters.sampleFormat              = paFloat32;                    
outputParameters.suggestedLatency          = (double)outLatency / sampleRate;  
outputParameters.hostApiSpecificStreamInfo = NULL;
printf("Latency = %d frames = %6.1f msec.\n", outLatency, outputParameters.suggestedLatency * 1000.0 );
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
sampleRate,
framesPerBuffer,
paClipOff,       
paminlatCallback,
&data );
if( err != paNoError ) goto error;
if( stream == NULL ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("\nMove windows around to see if the sound glitches.\n");
printf("Latency now %d, enter new number of frames, or 'q' to quit: ", outLatency );
line = fgets( str, 256, stdin );
if( line == NULL )
{
go = 0;
}
else
{
{
size_t l = strlen( str ) - 1;
if( str[ l ] == '\n')
str[ l ] = '\0';
}
if( str[0] == 'q' ) go = 0;
else
{
outLatency = atol( str );
if( outLatency < minLatency )
{
printf( "Latency below minimum of %d! Set to minimum!!!\n", minLatency );
outLatency = minLatency;
}
}
}
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
}
printf("A good setting for latency would be somewhat higher than\n");
printf("the minimum latency that worked.\n");
printf("PortAudio: Test finished.\n");
Pa_Terminate();
return 0;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return 1;
}

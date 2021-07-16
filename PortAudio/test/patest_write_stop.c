 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS         (5)
#define SAMPLE_RATE         (44100)
#define FRAMES_PER_BUFFER   (1024)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
float buffer[FRAMES_PER_BUFFER][2];  
float sine[TABLE_SIZE];  
int left_phase = 0;
int right_phase = 0;
int left_inc = 1;
int right_inc = 3;  
int i, j;
int bufferCount;
const int   framesBy2  = FRAMES_PER_BUFFER >> 1;
const float framesBy2f = (float) framesBy2 ;
printf( "PortAudio Test: output silence, followed by one buffer of a ramped sine wave. SR = %d, BufSize = %d\n",
SAMPLE_RATE, FRAMES_PER_BUFFER);
for( i=0; i<TABLE_SIZE; i++ )
{
sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();  
outputParameters.channelCount = 2;        
outputParameters.sampleFormat = paFloat32;  
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency * 5;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
NULL,  
NULL );  
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Playing %d seconds of silence followed by one buffer of a ramped sinusoid.\n", NUM_SECONDS );
bufferCount = ((NUM_SECONDS * SAMPLE_RATE) / FRAMES_PER_BUFFER);
for( j=0; j < FRAMES_PER_BUFFER; j++ )
{
buffer[j][0] = 0;   
buffer[j][1] = 0;   
}
for( i=0; i < bufferCount; i++ )
{
err = Pa_WriteStream( stream, buffer, FRAMES_PER_BUFFER );
if( err != paNoError ) goto error;
}
for( j=0; j < FRAMES_PER_BUFFER; j++ )
{
float ramp = 1;
if( j < framesBy2 )
ramp = j / framesBy2f;
else
ramp = (FRAMES_PER_BUFFER - j) / framesBy2f ;
buffer[j][0] = sine[left_phase] * ramp;   
buffer[j][1] = sine[right_phase] * ramp;   
left_phase += left_inc;
if( left_phase >= TABLE_SIZE ) left_phase -= TABLE_SIZE;
right_phase += right_inc;
if( right_phase >= TABLE_SIZE ) right_phase -= TABLE_SIZE;
}
err = Pa_WriteStream( stream, buffer, FRAMES_PER_BUFFER );
if( err != paNoError ) goto error;
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

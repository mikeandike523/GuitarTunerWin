 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (5)
#define NUM_LOOPS     (4)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (67)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
typedef struct
{
float sine[TABLE_SIZE];
int phase;
unsigned long generatedFramesCount;
volatile int callbackReturnedPaComplete;
volatile int callbackInvokedAfterReturningPaComplete;
char message[100];
}
TestData;
static int TestCallback( const void *input, void *output,
unsigned long frameCount,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
TestData *data = (TestData*)userData;
float *out = (float*)output;
unsigned long i;
float x;
(void) input;        
(void) timeInfo;
(void) statusFlags;
if( data->callbackReturnedPaComplete )
data->callbackInvokedAfterReturningPaComplete = 1;
for( i=0; i<frameCount; i++ )
{
x = data->sine[ data->phase++ ];
if( data->phase >= TABLE_SIZE )
data->phase -= TABLE_SIZE;
*out++ = x;   
*out++ = x;   
}
data->generatedFramesCount += frameCount;
if( data->generatedFramesCount >= (NUM_SECONDS * SAMPLE_RATE) )
{
data->callbackReturnedPaComplete = 1;
return paComplete;
}
else
{
return paContinue;
}
}
static void StreamFinished( void* userData )
{
TestData *data = (TestData *) userData;
printf( "Stream Completed: %s\n", data->message );
}
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
TestData data;
int i, j;
printf( "PortAudio Test: output sine wave. SR = %d, BufSize = %d\n",
SAMPLE_RATE, FRAMES_PER_BUFFER );
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device                    = Pa_GetDefaultOutputDevice();
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount              = 2;                
outputParameters.sampleFormat              = paFloat32;        
outputParameters.suggestedLatency          = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
TestCallback,
&data );
if( err != paNoError ) goto error;
sprintf( data.message, "Loop: XX" );
err = Pa_SetStreamFinishedCallback( stream, &StreamFinished );
if( err != paNoError ) goto error;
printf("Repeating test %d times.\n", NUM_LOOPS );
for( i=0; i < NUM_LOOPS; ++i )
{
data.phase = 0;
data.generatedFramesCount = 0;
data.callbackReturnedPaComplete = 0;
data.callbackInvokedAfterReturningPaComplete = 0;
sprintf( data.message, "Loop: %d", i );
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Play for %d seconds.\n", NUM_SECONDS );
do
{
Pa_Sleep( 500 );
}
while( !data.callbackReturnedPaComplete );
printf( "Callback returned paComplete.\n" );
printf( "Waiting for buffers to finish playing...\n" );
j = 0;
while( (err = Pa_IsStreamActive( stream )) == 1 && j < NUM_SECONDS * 2 )
{
printf(".\n" );
Pa_Sleep( 500 );
++j;
}
if( err < 0 )
{
goto error;
}
else if( err == 1 )
{
printf( "TEST FAILED: Timed out waiting for buffers to finish playing.\n" );
}
else
{
printf("Buffers finished.\n" );
}
if( data.callbackInvokedAfterReturningPaComplete )
{
printf( "TEST FAILED: Callback was invoked after returning paComplete.\n" );
}
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
printf( "sleeping for 1 second...\n" );
Pa_Sleep( 1000 );
}
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

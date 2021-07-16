 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (8)
#define SAMPLE_RATE   (44100)
#define TABLE_SIZE    (200)
#define TEST_UNSIGNED (0)
#if TEST_UNSIGNED
#define TEST_FORMAT   paUInt8
typedef unsigned char sample_t;
#define SILENCE       ((sample_t)0x80)
#else
#define TEST_FORMAT   paInt8
typedef char          sample_t;
#define SILENCE       ((sample_t)0x00)
#endif
#ifndef M_PI
#define M_PI (3.14159265)
#endif
typedef struct
{
sample_t sine[TABLE_SIZE];
int left_phase;
int right_phase;
unsigned int framesToGo;
}
paTestData;
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
sample_t *out = (sample_t*)outputBuffer;
int i;
int framesToCalc;
int finished = 0;
(void) inputBuffer;  
if( data->framesToGo < framesPerBuffer )
{
framesToCalc = data->framesToGo;
data->framesToGo = 0;
finished = 1;
}
else
{
framesToCalc = framesPerBuffer;
data->framesToGo -= framesPerBuffer;
}
for( i=0; i<framesToCalc; i++ )
{
*out++ = data->sine[data->left_phase];   
*out++ = data->sine[data->right_phase];   
data->left_phase += 1;
if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
data->right_phase += 3;  
if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
}
for( ; i<(int)framesPerBuffer; i++ )
{
*out++ = SILENCE;  
*out++ = SILENCE;  
}
return finished;
}
int main(void);
int main(void)
{
PaStreamParameters  outputParameters;
PaStream*           stream;
PaError             err;
paTestData          data;
PaTime              streamOpened;
int                 i, totalSamps;
#if TEST_UNSIGNED
printf("PortAudio Test: output UNsigned 8 bit sine wave.\n");
#else
printf("PortAudio Test: output signed 8 bit sine wave.\n");
#endif
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = SILENCE + (char) (127.0 * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. ));
}
data.left_phase = data.right_phase = 0;
data.framesToGo = totalSamps =  NUM_SECONDS * SAMPLE_RATE;  
err = Pa_Initialize();
if( err != paNoError )
goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;                      
outputParameters.sampleFormat = TEST_FORMAT;
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream( &stream,
NULL,       
&outputParameters,
SAMPLE_RATE,
256,        
paClipOff,  
patestCallback,
&data );
if( err != paNoError )
goto error;
streamOpened = Pa_GetStreamTime( stream );  
err = Pa_StartStream( stream );
if( err != paNoError )
goto error;
while( (Pa_GetStreamTime( stream ) - streamOpened) < (PaTime)NUM_SECONDS / 2.0 )
Pa_Sleep(10);
printf("Stopping Stream.\n");
err = Pa_StopStream( stream );
if( err != paNoError )
goto error;
printf("Pause for 2 seconds.\n");
Pa_Sleep( 2000 );
printf("Starting again.\n");
err = Pa_StartStream( stream );
if( err != paNoError )
goto error;
printf("Waiting for sound to finish.\n");
while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
Pa_Sleep(100);
if( err < 0 )
goto error;
err = Pa_CloseStream( stream );
if( err != paNoError )
goto error;
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

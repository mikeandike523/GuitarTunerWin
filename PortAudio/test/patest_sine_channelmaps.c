 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#ifdef __APPLE__
#include "pa_mac_core.h"
#endif
#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
typedef struct
{
float sine[TABLE_SIZE];
int left_phase;
int right_phase;
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
(void) timeInfo;  
(void) statusFlags;
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
return paContinue;
}
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
paTestData data;
#ifdef __APPLE__
PaMacCoreStreamInfo macInfo;
const SInt32 channelMap[4] = { -1, -1, 0, 1 };
#endif
int i;
printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
printf("Output will be mapped to channels 2 and 3 instead of 0 and 1.\n");
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
data.left_phase = data.right_phase = 0;
err = Pa_Initialize();
if( err != paNoError ) goto error;
#ifdef __APPLE__
PaMacCore_SetupStreamInfo( &macInfo, paMacCorePlayNice );
PaMacCore_SetupChannelMap( &macInfo, channelMap, 4 );
for( i=0; i<4; ++i )
printf( "channel %d name: %s\n", i, PaMacCore_GetChannelName( Pa_GetDefaultOutputDevice(), i, false ) );
#else
printf( "Channel mapping not supported on this platform. Reverting to normal sine test.\n" );
#endif
outputParameters.device = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;        
outputParameters.sampleFormat = paFloat32;  
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
#ifdef __APPLE__
outputParameters.hostApiSpecificStreamInfo = &macInfo;
#else
outputParameters.hostApiSpecificStreamInfo = NULL;
#endif
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
printf("Play for %d seconds.\n", NUM_SECONDS );
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

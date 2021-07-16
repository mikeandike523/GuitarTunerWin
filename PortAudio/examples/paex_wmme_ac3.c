 
#include <stdio.h>
#include <math.h>
#include <windows.h>     
#include <mmsystem.h>    
#include "portaudio.h"
#include "pa_win_wmme.h"
#define NUM_SECONDS         (20)
#define SAMPLE_RATE         (48000)
#define FRAMES_PER_BUFFER   (64)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE          (100)
#define CHANNEL_COUNT       (2)
typedef struct
{
short *buffer;
int bufferSampleCount;
int playbackIndex;
}
paTestData;
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
short *out = (short*)outputBuffer;
unsigned long i,j;
(void) timeInfo;  
(void) statusFlags;
(void) inputBuffer;
for( i=0; i<framesPerBuffer; i++ )
{
for( j = 0; j < CHANNEL_COUNT; ++j ){
*out++ = data->buffer[ data->playbackIndex++ ];
if( data->playbackIndex >= data->bufferSampleCount )
data->playbackIndex = 0;  
}
}
return paContinue;
}
int main(int argc, char* argv[])
{
PaStreamParameters outputParameters;
PaWinMmeStreamInfo wmmeStreamInfo;
PaStream *stream;
PaError err;
paTestData data;
int deviceIndex;
FILE *fp;
const char *fileName = "c:\\test_48k.ac3.spdif";
data.buffer = NULL;
printf("usage: patest_wmme_ac3 fileName [paDeviceIndex]\n");
printf("**IMPORTANT*** The provided file must include the spdif preamble at the start of every AC-3 frame. Using a normal ac3 file won't work.\n");
printf("PortAudio Test: output a raw spdif ac3 stream. SR = %d, BufSize = %d, Chans = %d\n",
SAMPLE_RATE, FRAMES_PER_BUFFER, CHANNEL_COUNT);
if( argc >= 2 )
fileName = argv[1];
printf( "reading spdif ac3 raw stream file %s\n", fileName );
fp = fopen( fileName, "rb" );
if( !fp ){
fprintf( stderr, "error opening spdif ac3 file.\n" );
return -1;
}
fseek( fp, 0, SEEK_END );
data.bufferSampleCount = ftell( fp ) / sizeof(short);
fseek( fp, 0, SEEK_SET );
data.buffer = (short*)malloc( data.bufferSampleCount * sizeof(short) );
if( !data.buffer ){
fprintf( stderr, "error allocating buffer.\n" );
return -1;
}
fread( data.buffer, sizeof(short), data.bufferSampleCount, fp );
fclose( fp );
data.playbackIndex = 0;
err = Pa_Initialize();
if( err != paNoError ) goto error;
deviceIndex = Pa_GetHostApiInfo( Pa_HostApiTypeIdToHostApiIndex( paMME ) )->defaultOutputDevice;
if( argc >= 3 ){
sscanf( argv[1], "%d", &deviceIndex );
}
printf( "using device id %d (%s)\n", deviceIndex, Pa_GetDeviceInfo(deviceIndex)->name );
outputParameters.device = deviceIndex;
outputParameters.channelCount = CHANNEL_COUNT;
outputParameters.sampleFormat = paInt16;  
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
wmmeStreamInfo.size = sizeof(PaWinMmeStreamInfo);
wmmeStreamInfo.hostApiType = paMME;
wmmeStreamInfo.version = 1;
wmmeStreamInfo.flags = paWinMmeWaveFormatDolbyAc3Spdif;
outputParameters.hostApiSpecificStreamInfo = &wmmeStreamInfo;
if( Pa_IsFormatSupported( 0, &outputParameters, SAMPLE_RATE ) == paFormatIsSupported  ){
printf( "Pa_IsFormatSupported reports device will support %d channels.\n", CHANNEL_COUNT );
}else{
printf( "Pa_IsFormatSupported reports device will not support %d channels.\n", CHANNEL_COUNT );
}
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
0,
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
free( data.buffer );
printf("Test finished.\n");
return err;
error:
Pa_Terminate();
free( data.buffer );
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

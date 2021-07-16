 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "portaudio.h"
#define SAMPLE_RATE             (44100)
#define FRAMES_PER_BUFFER       2 
#define NUM_CHANNELS            (2)
#define SUGGESTED_LATENCY_START_SECONDS     (0.0)
#define SUGGESTED_LATENCY_END_SECONDS       (2.0)
#define SUGGESTED_LATENCY_INCREMENT_SECONDS (0.0005)  
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
return paContinue;
}
static void usage()
{
int i;
const PaDeviceInfo *deviceInfo;
const char *channelString;
fprintf( stderr, "PortAudio suggested (requested) vs. resulting (reported) stream latency test\n" );
fprintf( stderr, "Usage: x.exe input-device-index output-device-index sample-rate frames-per-buffer\n" );
fprintf( stderr, "Use -1 for default device index, or use one of these:\n" );
for( i=0; i < Pa_GetDeviceCount(); ++i ){
deviceInfo = Pa_GetDeviceInfo(i);
if( deviceInfo->maxInputChannels > 0 && deviceInfo->maxOutputChannels > 0 )
channelString = "full-duplex";
else if( deviceInfo->maxInputChannels > 0 )
channelString = "input only";
else
channelString = "output only";
fprintf( stderr, "%d (%s, %s, %s)\n", i, deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name, channelString );
}
Pa_Terminate();
exit(-1);
}
int main( int argc, const char* argv[] );
int main( int argc, const char* argv[] )
{
PaStreamParameters inputParameters, outputParameters;
PaStream *stream;
PaError err;
PaTime suggestedLatency;
const PaStreamInfo *streamInfo;
const PaDeviceInfo *deviceInfo;
float sampleRate = SAMPLE_RATE;
int framesPerBuffer = FRAMES_PER_BUFFER;
err = Pa_Initialize();
if( err != paNoError ) goto error;
if( argc > 1 && strcmp(argv[1],"-h") == 0 )
usage();
if( argc > 3 ){
sampleRate = atoi(argv[3]);
}
if( argc > 4 ){
framesPerBuffer = atoi(argv[4]);
}
printf("# sample rate=%f, frames per buffer=%d\n", (float)sampleRate, framesPerBuffer );
inputParameters.device = -1;
if( argc > 1 )
inputParameters.device = atoi(argv[1]);
if( inputParameters.device == -1 ){
inputParameters.device = Pa_GetDefaultInputDevice();
if (inputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default input device available.\n");
goto error;
}
}else{
deviceInfo = Pa_GetDeviceInfo(inputParameters.device);
if( !deviceInfo ){
fprintf(stderr,"Error: Invalid input device index.\n");
usage();
}
if( deviceInfo->maxInputChannels == 0 ){
fprintf(stderr,"Error: Specified input device has no input channels (an output only device?).\n");
usage();
}
}
inputParameters.channelCount = NUM_CHANNELS;
inputParameters.sampleFormat = paFloat32;  
inputParameters.hostApiSpecificStreamInfo = NULL;
deviceInfo = Pa_GetDeviceInfo(inputParameters.device);
printf( "# using input device id %d (%s, %s)\n", inputParameters.device, deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name );
outputParameters.device = -1;
if( argc > 2 )
outputParameters.device = atoi(argv[2]);
if( outputParameters.device == -1 ){
outputParameters.device = Pa_GetDefaultOutputDevice();
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device available.\n");
goto error;
}
}else{
deviceInfo = Pa_GetDeviceInfo(outputParameters.device);
if( !deviceInfo ){
fprintf(stderr,"Error: Invalid output device index.\n");
usage();
}
if( deviceInfo->maxOutputChannels == 0 ){
fprintf(stderr,"Error: Specified output device has no output channels (an input only device?).\n");
usage();
}
}
outputParameters.channelCount = NUM_CHANNELS;
outputParameters.sampleFormat = paFloat32;  
outputParameters.hostApiSpecificStreamInfo = NULL;
deviceInfo = Pa_GetDeviceInfo(outputParameters.device);
printf( "# using output device id %d (%s, %s)\n", outputParameters.device, deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name );
printf( "# suggested latency, half duplex PaStreamInfo::outputLatency, half duplex PaStreamInfo::inputLatency, full duplex PaStreamInfo::outputLatency, full duplex PaStreamInfo::inputLatency\n" );
suggestedLatency = SUGGESTED_LATENCY_START_SECONDS;
while( suggestedLatency <= SUGGESTED_LATENCY_END_SECONDS ){
outputParameters.suggestedLatency = suggestedLatency;
inputParameters.suggestedLatency = suggestedLatency;
printf( "%f, ", suggestedLatency );
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
sampleRate,
framesPerBuffer,
paClipOff,       
patestCallback,
0 );
if( err != paNoError ) goto error;
streamInfo = Pa_GetStreamInfo( stream );
printf( "%f,", streamInfo->outputLatency  );
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
err = Pa_OpenStream(
&stream,
&inputParameters,
NULL,  
sampleRate,
framesPerBuffer,
paClipOff,       
patestCallback,
0 );
if( err != paNoError ) goto error;
streamInfo = Pa_GetStreamInfo( stream );
printf( "%f,", streamInfo->inputLatency  );
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
err = Pa_OpenStream(
&stream,
&inputParameters,
&outputParameters,
sampleRate,
framesPerBuffer,
paClipOff,       
patestCallback,
0 );
if( err != paNoError ) goto error;
streamInfo = Pa_GetStreamInfo( stream );
printf( "%f,%f", streamInfo->outputLatency, streamInfo->inputLatency );
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
printf( "\n" );
suggestedLatency += SUGGESTED_LATENCY_INCREMENT_SECONDS;
}
Pa_Terminate();
printf("# Test finished.\n");
return err;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

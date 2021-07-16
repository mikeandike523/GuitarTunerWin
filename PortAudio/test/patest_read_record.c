 
#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_SECONDS     (5)
#define NUM_CHANNELS    (2)
#define DITHER_FLAG     (0)  
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif
int main(void);
int main(void)
{
PaStreamParameters inputParameters, outputParameters;
PaStream *stream;
PaError err;
SAMPLE *recordedSamples;
int i;
int totalFrames;
int numSamples;
int numBytes;
SAMPLE max, average, val;
printf("patest_read_record.c\n"); fflush(stdout);
totalFrames = NUM_SECONDS * SAMPLE_RATE;  
numSamples = totalFrames * NUM_CHANNELS;
numBytes = numSamples * sizeof(SAMPLE);
recordedSamples = (SAMPLE *) malloc( numBytes );
if( recordedSamples == NULL )
{
printf("Could not allocate record array.\n");
exit(1);
}
for( i=0; i<numSamples; i++ ) recordedSamples[i] = 0;
err = Pa_Initialize();
if( err != paNoError ) goto error;
inputParameters.device = Pa_GetDefaultInputDevice();  
if (inputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default input device.\n");
goto error;
}
inputParameters.channelCount = NUM_CHANNELS;
inputParameters.sampleFormat = PA_SAMPLE_TYPE;
inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
inputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream,
&inputParameters,
NULL,                   
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
NULL,  
NULL );  
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Now recording!!\n"); fflush(stdout);
err = Pa_ReadStream( stream, recordedSamples, totalFrames );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
max = 0;
average = 0;
for( i=0; i<numSamples; i++ )
{
val = recordedSamples[i];
if( val < 0 ) val = -val;  
if( val > max )
{
max = val;
}
average += val;
}
average = average / numSamples;
printf("Sample max amplitude = "PRINTF_S_FORMAT"\n", max );
printf("Sample average = "PRINTF_S_FORMAT"\n", average );
#if 0
{
FILE  *fid;
fid = fopen("recorded.raw", "wb");
if( fid == NULL )
{
printf("Could not open file.");
}
else
{
fwrite( recordedSamples, NUM_CHANNELS * sizeof(SAMPLE), totalFrames, fid );
fclose( fid );
printf("Wrote data to 'recorded.raw'\n");
}
}
#endif
outputParameters.device = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = NUM_CHANNELS;
outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
printf("Begin playback.\n"); fflush(stdout);
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
if( stream )
{
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Waiting for playback to finish.\n"); fflush(stdout);
err = Pa_WriteStream( stream, recordedSamples, totalFrames );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
printf("Done.\n"); fflush(stdout);
}
free( recordedSamples );
Pa_Terminate();
return 0;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return -1;
}

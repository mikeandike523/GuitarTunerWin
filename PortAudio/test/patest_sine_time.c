 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "pa_util.h"
#define NUM_SECONDS   (8.0)
#define NUM_LOOPS     (2)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TWOPI (M_PI * 2.0)
#define TABLE_SIZE   (200)
typedef struct
{
double           left_phase;
double           right_phase;
volatile PaTime  latency;
volatile PaTime  outTime;
volatile long    frameCount;
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
double left_phaseInc = 0.02;
double right_phaseInc = 0.06;
double left_phase = data->left_phase;
double right_phase = data->right_phase;
(void) statusFlags;  
(void) inputBuffer;
data->outTime = timeInfo->outputBufferDacTime;
data->latency = timeInfo->outputBufferDacTime - timeInfo->currentTime;
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
data->frameCount += framesPerBuffer;
return paContinue;
}
static void ReportStreamTime( PaStream *stream, paTestData *data );
static void ReportStreamTime( PaStream *stream, paTestData *data )
{
PaTime  streamTime, outTime;
streamTime = Pa_GetStreamTime( stream );
outTime = data->outTime;
if( outTime < 0.0 )
{
printf("Stream time = %8.1f\n", streamTime );
}
else
{
printf("Stream time = %8.4f, outTime = %8.4f, latency = %8.4f, frames = %ld\n",
streamTime, outTime, data->latency, data->frameCount );
}
fflush(stdout);
}
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
paTestData data = {0};
PaTime startTime;
long maxFrameCount;
double secondsPerLoop;
printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;        
outputParameters.sampleFormat = paFloat32;  
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
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
for (int i = 0; i < 2; i++) {
secondsPerLoop = NUM_SECONDS / NUM_LOOPS;
printf("Play for %3.1f seconds.\n",  secondsPerLoop); fflush(stdout);
data.outTime = -1.0;  
data.frameCount = 0;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
startTime = Pa_GetStreamTime( stream );
maxFrameCount = SAMPLE_RATE * (secondsPerLoop + 2);  
do
{
ReportStreamTime( stream, &data );
Pa_Sleep(100);
if ( data.frameCount > maxFrameCount ) {
fprintf( stderr, "Time not advancing fast enough!\n" );
err = paTimedOut;
goto error;
}
} while( (Pa_GetStreamTime( stream ) - startTime) < secondsPerLoop );
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
if (i < (NUM_LOOPS - 1))
printf("Pause for 2 seconds.\n"); fflush(stdout);
Pa_Sleep( 2000 );
}
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Terminate();
printf("Test PASSED.\n");
return 0;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
printf("Test FAILED.\n");
return 1;
}

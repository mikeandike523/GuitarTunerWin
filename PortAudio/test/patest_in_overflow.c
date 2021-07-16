 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define MAX_SINES     (500)
#define MAX_LOAD      (1.2)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (512)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TWOPI (M_PI * 2.0)
typedef struct paTestData
{
int sineCount;
double phases[MAX_SINES];
int countOverflows;
int inputOverflowCount;
}
paTestData;
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
float out;           
unsigned long i;
int j;
int finished = paContinue;
(void) timeInfo;     
(void) inputBuffer;  
(void) outputBuffer;  
if( data->countOverflows && (statusFlags & paInputOverflow) )
data->inputOverflowCount++;
for( i=0; i<framesPerBuffer; i++ )
{
float output = 0.0;
double phaseInc = 0.02;
double phase;
for( j=0; j<data->sineCount; j++ )
{
phase = data->phases[j];
phase += phaseInc;
if( phase > TWOPI ) phase -= TWOPI;
phaseInc *= 1.02;
if( phaseInc > 0.5 ) phaseInc *= 0.5;
output += (float) sin( phase );
data->phases[j] = phase;
}
out = (float) (output / data->sineCount);
(void) out;  
}
return finished;
}
int main(void);
int main(void)
{
PaStreamParameters inputParameters;
PaStream *stream;
PaError err;
int safeSineCount, stressedSineCount;
int safeOverflowCount, stressedOverflowCount;
paTestData data = {0};
double load;
printf("PortAudio Test: input only, no sound output. Load callback by performing calculations, count input overflows. SR = %d, BufSize = %d. MAX_LOAD = %f\n",
SAMPLE_RATE, FRAMES_PER_BUFFER, (float)MAX_LOAD );
err = Pa_Initialize();
if( err != paNoError ) goto error;
inputParameters.device = Pa_GetDefaultInputDevice();   
if (inputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default input device.\n");
goto error;
}
inputParameters.channelCount = 1;                       
inputParameters.sampleFormat = paFloat32;               
inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
inputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream,
&inputParameters,
NULL,     
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,     
patestCallback,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Establishing load conditions...\n" );
do
{
data.sineCount++;
Pa_Sleep( 100 );
load = Pa_GetStreamCpuLoad( stream );
printf("sineCount = %d, CPU load = %f\n", data.sineCount, load );
}
while( load < 0.5 && data.sineCount < (MAX_SINES-1));
safeSineCount = data.sineCount;
stressedSineCount = (int) (2.0 * data.sineCount * MAX_LOAD );
if( stressedSineCount > MAX_SINES )
stressedSineCount = MAX_SINES;
for( ; data.sineCount < stressedSineCount; data.sineCount++ )
{
Pa_Sleep( 100 );
load = Pa_GetStreamCpuLoad( stream );
printf("STRESSING: sineCount = %d, CPU load = %f\n", data.sineCount, load );
}
printf("Counting overflows for 5 seconds.\n");
data.countOverflows = 1;
Pa_Sleep( 5000 );
stressedOverflowCount = data.inputOverflowCount;
data.countOverflows = 0;
data.sineCount = safeSineCount;
printf("Resuming safe load...\n");
Pa_Sleep( 1500 );
data.inputOverflowCount = 0;
Pa_Sleep( 1500 );
load = Pa_GetStreamCpuLoad( stream );
printf("sineCount = %d, CPU load = %f\n", data.sineCount, load );
printf("Counting overflows for 5 seconds.\n");
data.countOverflows = 1;
Pa_Sleep( 5000 );
safeOverflowCount = data.inputOverflowCount;
printf("Stop stream.\n");
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Terminate();
if( stressedOverflowCount == 0 )
printf("Test failed, no input overflows detected under stress.\n");
else if( safeOverflowCount != 0 )
printf("Test failed, %d unexpected overflows detected under safe load.\n", safeOverflowCount);
else
printf("Test passed, %d expected input overflows detected under stress, 0 unexpected overflows detected under safe load.\n", stressedOverflowCount );
return err;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

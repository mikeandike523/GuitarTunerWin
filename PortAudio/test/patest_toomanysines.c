 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define MAX_SINES     (1000)
#define MAX_LOAD      (1.2)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (512)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TWOPI (M_PI * 2.0)
typedef struct paTestData
{
int numSines;
double phases[MAX_SINES];
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
int j;
int finished = 0;
(void) inputBuffer;  
for( i=0; i<framesPerBuffer; i++ )
{
float output = 0.0;
double phaseInc = 0.02;
double phase;
for( j=0; j<data->numSines; j++ )
{
phase = data->phases[j];
phase += phaseInc;
if( phase > TWOPI ) phase -= TWOPI;
phaseInc *= 1.02;
if( phaseInc > 0.5 ) phaseInc *= 0.5;
output += (float) sin( phase );
data->phases[j] = phase;
}
*out++ = (float) (output / data->numSines);
}
return finished;
}
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
int numStress;
paTestData data = {0};
double load;
printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d. MAX_LOAD = %f\n",
SAMPLE_RATE, FRAMES_PER_BUFFER, MAX_LOAD );
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();   
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 1;                       
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
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
do
{        Pa_Sleep( 100 );
load = Pa_GetStreamCpuLoad( stream );
printf("numSines = %d, CPU load = %f\n", data.numSines, load );
if( load < 0.3 )
{
data.numSines += 10;
}
else if( load < 0.4 )
{
data.numSines += 2;
}
else
{
data.numSines += 1;
}
}
while( load < 0.5 );
numStress = (int) (2.0 * data.numSines * MAX_LOAD );
if( numStress > MAX_SINES )
numStress = MAX_SINES;
for( ; data.numSines < numStress; data.numSines+=2 )
{
Pa_Sleep( 200 );
load = Pa_GetStreamCpuLoad( stream );
printf("STRESSING: numSines = %d, CPU load = %f\n", data.numSines, load );
}
printf("Suffer for 5 seconds.\n");
Pa_Sleep( 5000 );
printf("Stop stream.\n");
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

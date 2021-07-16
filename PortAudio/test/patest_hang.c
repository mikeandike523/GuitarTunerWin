 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define SAMPLE_RATE       (44100)
#define FRAMES_PER_BUFFER (1024)
#ifndef M_PI
#define M_PI              (3.14159265)
#endif
#define TWOPI             (M_PI * 2.0)
typedef struct paTestData
{
int    sleepFor;
double phase;
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
int finished = 0;
double phaseInc = 0.02;
double phase = data->phase;
(void) inputBuffer;  
for( i=0; i<framesPerBuffer; i++ )
{
phase += phaseInc;
if( phase > TWOPI ) phase -= TWOPI;
*out++ = (float) sin( phase );  
}
if( data->sleepFor > 0 )
{
Pa_Sleep( data->sleepFor );
}
data->phase = phase;
return finished;
}
int main(void);
int main(void)
{
PaStream*           stream;
PaStreamParameters  outputParameters;
PaError             err;
int                 i;
paTestData          data = {0};
printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n",
SAMPLE_RATE, FRAMES_PER_BUFFER );
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 1;                      
outputParameters.sampleFormat = paFloat32;              
outputParameters.hostApiSpecificStreamInfo = NULL;
outputParameters.suggestedLatency          = Pa_GetDeviceInfo(outputParameters.device)
->defaultLowOutputLatency;
err = Pa_OpenStream(&stream,
NULL,                     
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,                
patestCallback,
&data);
if (err != paNoError) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
for(i=0; i <= 1000; i += 100)
{
printf("Sleep for %d milliseconds in audio callback.\n", i );
data.sleepFor = i;
Pa_Sleep( ((i<1000) ? 1000 : i) );
}
printf("Suffer for 10 seconds.\n");
Pa_Sleep( 10000 );
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

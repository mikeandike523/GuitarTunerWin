 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define MAX_SINES     (2000)
#define MAX_USAGE     (0.5)
#define SAMPLE_RATE   (44100)
#define FREQ_TO_PHASE_INC(freq)   (freq/(float)SAMPLE_RATE)
#define MIN_PHASE_INC  FREQ_TO_PHASE_INC(200.0f)
#define MAX_PHASE_INC  (MIN_PHASE_INC * (1 << 5))
#define FRAMES_PER_BUFFER  (512)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TWOPI (M_PI * 2.0)
#define TABLE_SIZE   (1024)
typedef struct paTestData
{
int numSines;
float sine[TABLE_SIZE + 1];  
float phases[MAX_SINES];
}
paTestData;
float LookupSine( paTestData *data, float phase );
float LookupSine( paTestData *data, float phase )
{
float fIndex = phase*TABLE_SIZE;
int   index = (int) fIndex;
float fract = fIndex - index;
float lo = data->sine[index];
float hi = data->sine[index+1];
float val = lo + fract*(hi-lo);
return val;
}
static int patestCallback(const void*                     inputBuffer,
void*                           outputBuffer,
unsigned long                   framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags           statusFlags,
void*                           userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
float outSample;
float scaler;
int numForScale;
unsigned long i;
int j;
int finished = 0;
(void) inputBuffer;  
numForScale = data->numSines;
if( numForScale < 8 ) numForScale = 8;   
scaler = 1.0f / numForScale;
for( i=0; i<framesPerBuffer; i++ )
{
float output = 0.0;
float phaseInc = MIN_PHASE_INC;
float phase;
for( j=0; j<data->numSines; j++ )
{
phase = data->phases[j];
phase += phaseInc;
if( phase >= 1.0 ) phase -= 1.0;
output += LookupSine(data, phase);
data->phases[j] = phase;
phaseInc *= 1.02f;
if( phaseInc > MAX_PHASE_INC ) phaseInc = MIN_PHASE_INC;
}
outSample = (float) (output * scaler);
*out++ = outSample;  
*out++ = outSample;  
}
return finished;
}
int main(void);
int main(void)
{
int                 i;
PaStream*           stream;
PaStreamParameters  outputParameters;
PaError             err;
paTestData          data = {0};
double              load;
printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
data.sine[TABLE_SIZE] = data.sine[0];  
err = Pa_Initialize();
if( err != paNoError )
goto error;
outputParameters.device                    = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount              = 2;                            
outputParameters.sampleFormat              = paFloat32;                    
outputParameters.hostApiSpecificStreamInfo = NULL;
outputParameters.suggestedLatency          = Pa_GetDeviceInfo(outputParameters.device)
->defaultHighOutputLatency;
err = Pa_OpenStream(&stream,
NULL,                
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,           
patestCallback,
&data);
if( err != paNoError )
goto error;
err = Pa_StartStream( stream );
if( err != paNoError )
goto error;
do  {
data.numSines += 10;
Pa_Sleep(200);
load = Pa_GetStreamCpuLoad(stream);
printf("numSines = %d, CPU load = %f\n", data.numSines, load );
fflush(stdout);
} while((load < MAX_USAGE) && (data.numSines < MAX_SINES));
Pa_Sleep(2000);      
err = Pa_StopStream( stream );
if( err != paNoError )
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

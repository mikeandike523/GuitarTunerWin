 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS        (10)
#define SAMPLE_RATE        (44100)
#define FRAMES_PER_BUFFER  (512)
#define LEFT_FREQ          (SAMPLE_RATE/256.0)   
#define RIGHT_FREQ         (500.0)
#define AMPLITUDE          (1.0)
#define TEST_UINT8    (0)
#define TEST_INT8     (0)
#define TEST_INT16    (1)
#define TEST_FLOAT32  (0)
#if TEST_UINT8
#define TEST_FORMAT         paUInt8
typedef unsigned char       SAMPLE_t;
#define SAMPLE_ZERO         (0x80)
#define DOUBLE_TO_SAMPLE(x) (SAMPLE_ZERO + (SAMPLE_t)(127.0 * (x)))
#define FORMAT_NAME         "Unsigned 8 Bit"
#elif TEST_INT8
#define TEST_FORMAT         paInt8
typedef char                SAMPLE_t;
#define SAMPLE_ZERO         (0)
#define DOUBLE_TO_SAMPLE(x) (SAMPLE_ZERO + (SAMPLE_t)(127.0 * (x)))
#define FORMAT_NAME         "Signed 8 Bit"
#elif TEST_INT16
#define TEST_FORMAT         paInt16
typedef short               SAMPLE_t;
#define SAMPLE_ZERO         (0)
#define DOUBLE_TO_SAMPLE(x) (SAMPLE_ZERO + (SAMPLE_t)(32767 * (x)))
#define FORMAT_NAME         "Signed 16 Bit"
#elif TEST_FLOAT32
#define TEST_FORMAT         paFloat32
typedef float               SAMPLE_t;
#define SAMPLE_ZERO         (0.0)
#define DOUBLE_TO_SAMPLE(x) ((SAMPLE_t)(x))
#define FORMAT_NAME         "Float 32 Bit"
#endif
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
typedef struct
{
double left_phase;
double right_phase;
unsigned int framesToGo;
}
paTestData;
static int patestCallback( const void *inputBuffer,
void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
SAMPLE_t *out = (SAMPLE_t *)outputBuffer;
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
data->left_phase += (LEFT_FREQ / SAMPLE_RATE);
if( data->left_phase > 1.0) data->left_phase -= 1.0;
*out++ = DOUBLE_TO_SAMPLE( AMPLITUDE * sin( (data->left_phase * M_PI * 2. )));
data->right_phase += (RIGHT_FREQ / SAMPLE_RATE);
if( data->right_phase > 1.0) data->right_phase -= 1.0;
*out++ = DOUBLE_TO_SAMPLE( AMPLITUDE * sin( (data->right_phase * M_PI * 2. )));
}
for( ; i<(int)framesPerBuffer; i++ )
{
*out++ = SAMPLE_ZERO;  
*out++ = SAMPLE_ZERO;  
}
return finished;
}
int main(void);
int main(void)
{
PaStream *stream;
PaStreamParameters outputParameters;
PaError err;
paTestData data;
int totalSamps;
printf("PortAudio Test: output " FORMAT_NAME "\n");
data.left_phase = data.right_phase = 0.0;
data.framesToGo = totalSamps =  NUM_SECONDS * SAMPLE_RATE;  
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device           = Pa_GetDefaultOutputDevice();  
outputParameters.channelCount     = 2;                            
outputParameters.sampleFormat     = TEST_FORMAT;                  
outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream( &stream,
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
printf("Waiting %d seconds for sound to finish.\n", NUM_SECONDS );
while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
if( err < 0 ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Terminate();
printf("PortAudio Test Finished: " FORMAT_NAME "\n");
return err;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

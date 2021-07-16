 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define OUTPUT_DEVICE       (Pa_GetDefaultOutputDevice())
#define SAMPLE_RATE         (44100)
#define FRAMES_PER_BUFFER   (64)
#define MIN_FREQ            (100.0f)
#define CalcPhaseIncrement(freq)  ((freq)/SAMPLE_RATE)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (400)
typedef struct
{
float sine[TABLE_SIZE + 1];  
float phase_increment;
float left_phase;
float right_phase;
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
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
int i;
(void) inputBuffer;  
for( i=0; i<framesPerBuffer; i++ )
{
*out++ = LookupSine(data, data->left_phase);   
*out++ = LookupSine(data, data->right_phase);   
data->left_phase += data->phase_increment;
if( data->left_phase >= 1.0f ) data->left_phase -= 1.0f;
data->right_phase += (data->phase_increment * 1.5f);  
if( data->right_phase >= 1.0f ) data->right_phase -= 1.0f;
}
return 0;
}
int main(void);
int main(void)
{
PaStream *stream;
PaStreamParameters outputParameters;
PaError err;
paTestData data;
int i;
int done = 0;
printf("PortAudio Test: enter letter then hit ENTER.\n" );
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = 0.90f * (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
data.sine[TABLE_SIZE] = data.sine[0];  
data.left_phase = data.right_phase = 0.0;
data.phase_increment = CalcPhaseIncrement(MIN_FREQ);
err = Pa_Initialize();
if( err != paNoError ) goto error;
printf("PortAudio Test: output device = %d\n", OUTPUT_DEVICE );
outputParameters.device = OUTPUT_DEVICE;
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;          
outputParameters.sampleFormat = paFloat32;  
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
printf("Requested output latency = %.4f seconds.\n", outputParameters.suggestedLatency );
printf("%d frames per buffer.\n.", FRAMES_PER_BUFFER );
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff|paDitherOff,       
patestCallback,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Play ASCII keyboard. Hit 'q' to stop. (Use RETURN key on Mac)\n");
fflush(stdout);
while ( !done )
{
float  freq;
int index;
char c;
do
{
c = getchar();
}
while( c < ' ');  
if( c == 'q' ) done = 1;
index = c % 26;
freq = MIN_FREQ + (index * 40.0);
data.phase_increment = CalcPhaseIncrement(freq);
}
printf("Call Pa_StopStream()\n");
err = Pa_StopStream( stream );
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

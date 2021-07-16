 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (44100)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
typedef struct paTestData
{
float sine[TABLE_SIZE];
float amplitude;
int   left_phase;
int   right_phase;
}
paTestData;
static int sineCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo *timeInfo,
PaStreamCallbackFlags statusFlags, void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
float amplitude = data->amplitude;
unsigned int i;
(void) inputBuffer;
for( i=0; i<framesPerBuffer; i++ )
{
*out++ = amplitude * data->sine[data->left_phase];   
*out++ = amplitude * data->sine[data->right_phase];   
data->left_phase += 1;
if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
data->right_phase += 3;  
if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
}
return 0;
}
PaError PlaySine( paTestData *data, PaStreamFlags flags, float amplitude );
PaError PlaySine( paTestData *data, PaStreamFlags flags, float amplitude )
{
PaStream*           stream;
PaStreamParameters  outputParameters;
PaError             err;
data->left_phase = data->right_phase = 0;
data->amplitude  = amplitude;
err = Pa_Initialize();
if (err != paNoError)
goto done;
outputParameters.device = Pa_GetDefaultOutputDevice();   
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto done;
}
outputParameters.channelCount = 2;                       
outputParameters.hostApiSpecificStreamInfo = NULL;
outputParameters.sampleFormat = paFloat32;       
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )
->defaultLowOutputLatency;    
err = Pa_OpenStream( &stream,
NULL,                               
&outputParameters,
SAMPLE_RATE,
1024,                               
flags,
sineCallback,
(void*)data );
if (err != paNoError)
goto done;
err = Pa_StartStream( stream );
if (err != paNoError)
goto done;
Pa_Sleep( NUM_SECONDS * 1000 );
printf("CPULoad = %8.6f\n", Pa_GetStreamCpuLoad(stream));
err = Pa_CloseStream( stream );
done:
Pa_Sleep( 250 );   
Pa_Terminate();
return err;
}
int main(void);
int main(void)
{
PaError     err;
paTestData  DATA;
int         i;
float       amplitude = 4.0 / (1<<15);
printf("PortAudio Test: output EXTREMELY QUIET sine wave with and without dithering.\n");
for( i=0; i<TABLE_SIZE; i++ )
{
DATA.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
printf("\nNo treatment..\n"); fflush(stdout);
err = PlaySine( &DATA, paClipOff | paDitherOff, amplitude );
if( err < 0 ) goto done;
printf("\nClip..\n");
fflush(stdout);
err = PlaySine( &DATA, paDitherOff, amplitude );
if( err < 0 ) goto done;
printf("\nClip and Dither..\n");
fflush(stdout);
err = PlaySine( &DATA, paNoFlag, amplitude );
done:
if (err)
{
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
err = 1;  
}         
else
printf("\n(Don't forget to turn the VOLUME DOWN after listening so carefully.)\n");
return err;   
}

 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define NUM_SECONDS   (4)
#define SAMPLE_RATE   (44100)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
typedef struct paTestData
{
float sine[TABLE_SIZE];
float amplitude;
int left_phase;
int right_phase;
}
paTestData;
PaError PlaySine( paTestData *data, unsigned long flags, float amplitude );
static int sineCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
float amplitude = data->amplitude;
unsigned int i;
(void) inputBuffer;  
(void) timeInfo;
(void) statusFlags;
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
int main(void);
int main(void)
{
PaError err;
paTestData data;
int i;
printf("PortAudio Test: output sine wave with and without clipping.\n");
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
printf("\nHalf amplitude. Should sound like sine wave.\n"); fflush(stdout);
err = PlaySine( &data, paClipOff | paDitherOff, 0.5f );
if( err < 0 ) goto error;
printf("\nFull amplitude. Should sound like sine wave.\n"); fflush(stdout);
err = PlaySine( &data, paClipOff | paDitherOff, 0.999f );
if( err < 0 ) goto error;
printf("\nOver range with clipping and dithering turned OFF. Should sound very nasty.\n");
fflush(stdout);
err = PlaySine( &data, paClipOff | paDitherOff, 1.1f );
if( err < 0 ) goto error;
printf("\nOver range with clipping and dithering turned ON.  Should sound smoother than previous.\n");
fflush(stdout);
err = PlaySine( &data, paNoFlag, 1.1f );
if( err < 0 ) goto error;
printf("\nOver range with paClipOff but dithering ON.\n"
"That forces clipping ON so it should sound the same as previous.\n");
fflush(stdout);
err = PlaySine( &data, paClipOff, 1.1f );
if( err < 0 ) goto error;
return 0;
error:
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return 1;
}
PaError PlaySine( paTestData *data, unsigned long flags, float amplitude )
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
data->left_phase = data->right_phase = 0;
data->amplitude = amplitude;
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
1024,
flags,
sineCallback,
data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
Pa_Sleep( NUM_SECONDS * 1000 );
printf("CPULoad = %8.6f\n", Pa_GetStreamCpuLoad( stream ) );
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Terminate();
return paNoError;
error:
return err;
}

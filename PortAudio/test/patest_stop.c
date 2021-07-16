 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define OUTPUT_DEVICE       (Pa_GetDefaultOutputDevice())
#define SLEEP_DUR           (200)
#define SAMPLE_RATE         (44100)
#define FRAMES_PER_BUFFER   (256)
#define LATENCY_SECONDS     (3.f)
#define FRAMES_PER_NOTE     (SAMPLE_RATE/2)
#define MAX_REPEATS         (2)
#define FUNDAMENTAL         (400.0f / SAMPLE_RATE)
#define NOTE_0              (FUNDAMENTAL * 1.0f / 1.0f)
#define NOTE_1              (FUNDAMENTAL * 5.0f / 4.0f)
#define NOTE_2              (FUNDAMENTAL * 4.0f / 3.0f)
#define NOTE_3              (FUNDAMENTAL * 3.0f / 2.0f)
#define NOTE_4              (FUNDAMENTAL * 2.0f / 1.0f)
#define MODE_FINISH    (0)
#define MODE_STOP      (1)
#define MODE_ABORT     (2)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (400)
typedef struct
{
float  waveform[TABLE_SIZE + 1];  
float  phase_increment;
float  phase;
float *tune;
int    notesPerTune;
int    frameCounter;
int    noteCounter;
int    repeatCounter;
PaTime outTime;
int    stopMode;
int    done;
}
paTestData;
int TestStopMode( paTestData *data );
float LookupWaveform( paTestData *data, float phase );
float LookupWaveform( paTestData *data, float phase )
{
float fIndex = phase*TABLE_SIZE;
int   index = (int) fIndex;
float fract = fIndex - index;
float lo = data->waveform[index];
float hi = data->waveform[index+1];
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
float value;
unsigned int i = 0;
int finished = paContinue;
(void) inputBuffer;      
(void) timeInfo;
(void) statusFlags;
if( !data->done )
{
for( i=0; i<framesPerBuffer; i++ )
{
if( data->frameCounter >= FRAMES_PER_NOTE )
{
data->noteCounter += 1;
data->frameCounter = 0;
if( data->noteCounter >= data->notesPerTune )
{
data->noteCounter = 0;
data->repeatCounter += 1;
if( data->repeatCounter >= MAX_REPEATS )
{
data->done = 1;
if( data->stopMode == MODE_FINISH )
{
finished = paComplete;
break;
}
}
}
data->phase_increment = data->tune[data->noteCounter];
}
value = LookupWaveform(data, data->phase);
*out++ = value;   
*out++ = value;   
data->phase += data->phase_increment;
if( data->phase >= 1.0f ) data->phase -= 1.0f;
data->frameCounter += 1;
}
}
for( ; i<framesPerBuffer; i++ )
{
*out++ = 0;  
*out++ = 0;  
}
return finished;
}
int main(void);
int main(void)
{
paTestData data;
int i;
float simpleTune[] = { NOTE_0, NOTE_1, NOTE_2, NOTE_3, NOTE_4, NOTE_3, NOTE_2, NOTE_1 };
printf("PortAudio Test: play song and test stopping. ask for %f seconds latency\n", LATENCY_SECONDS );
for( i=0; i<TABLE_SIZE; i++ )
{
data.waveform[i] = (float) (
(0.2 * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. )) +
(0.2 * sin( ((double)(3*i)/(double)TABLE_SIZE) * M_PI * 2. )) +
(0.1 * sin( ((double)(5*i)/(double)TABLE_SIZE) * M_PI * 2. ))
);
}
data.waveform[TABLE_SIZE] = data.waveform[0];  
data.tune = &simpleTune[0];
data.notesPerTune = sizeof(simpleTune) / sizeof(float);
printf("Test MODE_FINISH - callback returns 1.\n");
printf("Should hear entire %d note tune repeated twice.\n", data.notesPerTune);
data.stopMode = MODE_FINISH;
if( TestStopMode( &data ) != paNoError )
{
printf("Test of MODE_FINISH failed!\n");
goto error;
}
printf("Test MODE_STOP - stop when song is done.\n");
printf("Should hear entire %d note tune repeated twice.\n", data.notesPerTune);
data.stopMode = MODE_STOP;
if( TestStopMode( &data ) != paNoError )
{
printf("Test of MODE_STOP failed!\n");
goto error;
}
printf("Test MODE_ABORT - abort immediately.\n");
printf("Should hear last repetition cut short by %f seconds.\n", LATENCY_SECONDS);
data.stopMode = MODE_ABORT;
if( TestStopMode( &data ) != paNoError )
{
printf("Test of MODE_ABORT failed!\n");
goto error;
}
return 0;
error:
return 1;
}
int TestStopMode( paTestData *data )
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
data->done = 0;
data->phase = 0.0;
data->frameCounter = 0;
data->noteCounter = 0;
data->repeatCounter = 0;
data->phase_increment = data->tune[data->noteCounter];
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = OUTPUT_DEVICE;
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;           
outputParameters.sampleFormat = paFloat32;   
outputParameters.suggestedLatency = LATENCY_SECONDS;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,             
paClipOff,       
patestCallback,
data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
if( data->stopMode == MODE_FINISH )
{
while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
{
Pa_Sleep( SLEEP_DUR );
}
if( err < 0 ) goto error;
}
else
{
while( data->repeatCounter < MAX_REPEATS )
{
Pa_Sleep( SLEEP_DUR );
}
}
if( data->stopMode == MODE_ABORT )
{
printf("Call Pa_AbortStream()\n");
err = Pa_AbortStream( stream );
}
else
{
printf("Call Pa_StopStream()\n");
err = Pa_StopStream( stream );
}
if( err != paNoError ) goto error;
printf("Call Pa_CloseStream()\n"); fflush(stdout);
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

 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "pa_util.h"
#define NUM_BEEPS           (3)
#define SAMPLE_RATE         (44100)
#define SAMPLE_PERIOD       (1.0/44100.0)
#define FRAMES_PER_BUFFER   (256)
#define BEEP_DURATION       (400)
#define IDLE_DURATION       (SAMPLE_RATE*2)       
#define SLEEP_MSEC          (50)
#define STATE_BKG_IDLE      (0)
#define STATE_BKG_BEEPING   (1)
typedef struct
{
float        leftPhase;
float        rightPhase;
int          state;
int          beepCountdown;
int          idleCountdown;
}
paTestData;
static void InitializeTestData( paTestData *testData )
{
testData->leftPhase = 0;
testData->rightPhase = 0;
testData->state = STATE_BKG_BEEPING;
testData->beepCountdown = BEEP_DURATION;
testData->idleCountdown = IDLE_DURATION;
}
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo *timeInfo,
PaStreamCallbackFlags statusFlags, void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
unsigned int i;
int result = paContinue;
(void) inputBuffer;
(void) timeInfo;
(void) statusFlags;
for( i=0; i<framesPerBuffer; i++ )
{
switch( data->state )
{
case STATE_BKG_IDLE:
*out++ = 0.0;   
*out++ = 0.0;   
--data->idleCountdown;
if( data->idleCountdown <= 0 ) result = paComplete;
break;
case STATE_BKG_BEEPING:
if( data->beepCountdown <= 0 )
{
data->state = STATE_BKG_IDLE;
*out++ = 0.0;   
*out++ = 0.0;   
}
else
{
*out++ = data->leftPhase;   
*out++ = data->rightPhase;   
data->leftPhase += 0.01f;
if( data->leftPhase >= 1.0f ) data->leftPhase -= 2.0f;
data->rightPhase += 0.03f;
if( data->rightPhase >= 1.0f ) data->rightPhase -= 2.0f;
}
--data->beepCountdown;
break;
}
}
return result;
}
static PaError DoTest( int flags )
{
PaStream *stream;
PaError    err = paNoError;
paTestData data;
PaStreamParameters outputParameters;
InitializeTestData( &data );
outputParameters.device = Pa_GetDefaultOutputDevice();
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;
outputParameters.hostApiSpecificStreamInfo = NULL;
outputParameters.sampleFormat = paFloat32;
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
err = Pa_OpenStream(
&stream,
NULL,                          
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,             
paClipOff | flags,       
patestCallback,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("hear \"BEEP\"\n" );
fflush(stdout);
while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(SLEEP_MSEC);
if( err < 0 ) goto error;
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
return err;
error:
return err;
}
int main(void);
int main(void)
{
PaError    err = paNoError;
int        i;
err = Pa_Initialize();
if( err != paNoError ) goto error;
printf("PortAudio Test: Testing stream playback with no priming.\n");
printf("PortAudio Test: you should see BEEP before you hear it.\n");
printf("BEEP %d times.\n", NUM_BEEPS );
for( i=0; i< NUM_BEEPS; ++i )
{
err = DoTest( 0 );
if( err != paNoError )
goto error;
}
printf("PortAudio Test: Testing stream playback with priming.\n");
printf("PortAudio Test: you should see BEEP around the same time you hear it.\n");
for( i=0; i< NUM_BEEPS; ++i )
{
err = DoTest( paPrimeOutputBuffersUsingStreamCallback );
if( err != paNoError )
goto error;
}
printf("Test finished.\n");
Pa_Terminate();
return err;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

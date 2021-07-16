 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "pa_util.h"
#define NUM_BEEPS           (6)
#define SAMPLE_RATE         (44100)
#define SAMPLE_PERIOD       (1.0/44100.0)
#define FRAMES_PER_BUFFER   (256)
#define BEEP_DURATION       (400)
#define LATENCY_MSEC        (2000)
#define SLEEP_MSEC          (10)
#define TIMEOUT_MSEC        (15000)
#define STATE_BKG_IDLE      (0)
#define STATE_BKG_PENDING   (1)
#define STATE_BKG_BEEPING   (2)
typedef struct
{
float        left_phase;
float        right_phase;
int          state;
volatile int requestBeep;   
PaTime       beepTime;
int          beepCount;
double       latency;     
}
paTestData;
static unsigned long GenerateRandomNumber( void );
static unsigned long GenerateRandomNumber( void )
{
static unsigned long randSeed = 99887766;   
randSeed = (randSeed * 196314165) + 907633515;
return randSeed;
}
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo *timeInfo,
PaStreamCallbackFlags statusFlags, void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
unsigned int i;
(void) inputBuffer;
data->latency = timeInfo->outputBufferDacTime - timeInfo->currentTime;
for( i=0; i<framesPerBuffer; i++ )
{
switch( data->state )
{
case STATE_BKG_IDLE:
if( data->requestBeep )
{
int random = GenerateRandomNumber() >> 14;
data->beepTime = timeInfo->outputBufferDacTime + (( (double)(random + SAMPLE_RATE)) * SAMPLE_PERIOD );
data->state = STATE_BKG_PENDING;
}
*out++ = 0.0;   
*out++ = 0.0;   
break;
case STATE_BKG_PENDING:
if( (timeInfo->outputBufferDacTime + (i*SAMPLE_PERIOD)) >= data->beepTime )
{
data->state = STATE_BKG_BEEPING;
data->beepCount = BEEP_DURATION;
data->left_phase = data->right_phase = 0.0;
}
*out++ = 0.0;   
*out++ = 0.0;   
break;
case STATE_BKG_BEEPING:
if( data->beepCount <= 0 )
{
data->state = STATE_BKG_IDLE;
data->requestBeep = 0;
*out++ = 0.0;   
*out++ = 0.0;   
}
else
{
*out++ = data->left_phase;   
*out++ = data->right_phase;   
data->left_phase += 0.01f;
if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
data->right_phase += 0.03f;
if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
}
data->beepCount -= 1;
break;
default:
data->state = STATE_BKG_IDLE;
break;
}
}
return 0;
}
int main(void);
int main(void)
{
PaStream *stream;
PaError    err;
paTestData DATA;
int        i, timeout;
PaTime     previousTime;
PaStreamParameters outputParameters;
printf("PortAudio Test: you should see BEEP at the same time you hear it.\n");
printf("Wait for a few seconds random delay between BEEPs.\n");
printf("BEEP %d times.\n", NUM_BEEPS );
DATA.left_phase = DATA.right_phase = 0.0;
DATA.state = STATE_BKG_IDLE;
DATA.requestBeep = 0;
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
outputParameters.channelCount = 2;
outputParameters.hostApiSpecificStreamInfo = NULL;
outputParameters.sampleFormat = paFloat32;
outputParameters.suggestedLatency = (double)LATENCY_MSEC / 1000;
err = Pa_OpenStream(
&stream,
NULL,                          
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,             
paClipOff,       
patestCallback,
&DATA );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("started\n");
fflush(stdout);
previousTime = Pa_GetStreamTime( stream );
for( i=0; i<NUM_BEEPS; i++ )
{
DATA.requestBeep = 1;
timeout = TIMEOUT_MSEC;
while( (DATA.requestBeep == 1) && (timeout-- > 0 ) ) Pa_Sleep(SLEEP_MSEC);
if( timeout <= 0 )
{
fprintf( stderr, "Timed out waiting for background to acknowledge request.\n" );
goto error;
}
printf("calc beep for %9.3f, latency = %6.3f\n", DATA.beepTime, DATA.latency );
fflush(stdout);
timeout =  TIMEOUT_MSEC + (10000/SLEEP_MSEC);
while( (Pa_GetStreamTime( stream ) < DATA.beepTime) && (timeout-- > 0 ) )
{
Pa_Sleep(SLEEP_MSEC);
}
if( timeout <= 0 )
{
fprintf( stderr, "Timed out waiting for time. Now = %9.3f, Beep for %9.3f.\n",
PaUtil_GetTime(), DATA.beepTime );
goto error;
}
printf("hear \"BEEP\" at %9.3f, delta = %9.3f\n",
Pa_GetStreamTime( stream ), (DATA.beepTime - previousTime) );
fflush(stdout);
previousTime = DATA.beepTime;
}
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

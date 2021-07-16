 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define SAMPLE_RATE         (44100)
#define FRAMES_PER_BUFFER   (1024)
#define TONE_SECONDS        (1)       
#define TONE_FADE_SECONDS   (.04)     
#define GAP_SECONDS         (.25)      
#define BLIP_SECONDS        (.035)    
#define NUM_REPEATS         (3)
#ifndef M_PI
#define M_PI (3.14159265)
#endif
#define TABLE_SIZE          (2048)
typedef struct
{
float sine[TABLE_SIZE+1];
int repeatCount;
double phase;
double lowIncrement, highIncrement;
int gap1Length, toneLength, toneFadesLength, gap2Length, blipLength;
int gap1Countdown, toneCountdown, gap2Countdown, blipCountdown;
}
TestData;
static void RetriggerTestSignalGenerator( TestData *data )
{
data->phase = 0.;
data->gap1Countdown = data->gap1Length;
data->toneCountdown = data->toneLength;
data->gap2Countdown = data->gap2Length;
data->blipCountdown = data->blipLength;
}
static void ResetTestSignalGenerator( TestData *data )
{
data->repeatCount = 0;
RetriggerTestSignalGenerator( data );
}
static void InitTestSignalGenerator( TestData *data )
{
int signalLengthModBufferLength, i;
for( i=0; i<TABLE_SIZE; i++ )
{
data->sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
data->sine[TABLE_SIZE] = data->sine[0];  
data->lowIncrement = (330. / SAMPLE_RATE) * TABLE_SIZE;
data->highIncrement = (1760. / SAMPLE_RATE) * TABLE_SIZE;
data->gap1Length = GAP_SECONDS * SAMPLE_RATE;
data->toneLength = TONE_SECONDS * SAMPLE_RATE;
data->toneFadesLength = TONE_FADE_SECONDS * SAMPLE_RATE;
data->gap2Length = GAP_SECONDS * SAMPLE_RATE;
data->blipLength = BLIP_SECONDS * SAMPLE_RATE;
signalLengthModBufferLength = (data->gap1Length + data->toneLength + data->gap2Length + data->blipLength) % FRAMES_PER_BUFFER;
if( signalLengthModBufferLength > 0 )
data->toneLength += signalLengthModBufferLength;
ResetTestSignalGenerator( data );
}
#define MIN( a, b ) (((a)<(b))?(a):(b))
static void GenerateTestSignal( TestData *data, float *stereo, int frameCount )
{
int framesGenerated = 0;
float output;
long index;
float fraction;
int count, i;
while( framesGenerated < frameCount && data->repeatCount < NUM_REPEATS )
{
if( framesGenerated < frameCount && data->gap1Countdown > 0 ){
count = MIN( frameCount - framesGenerated, data->gap1Countdown );
for( i=0; i < count; ++i )
{
*stereo++ = 0.f;
*stereo++ = 0.f;
}
data->gap1Countdown -= count;
framesGenerated += count;
}
if( framesGenerated < frameCount && data->toneCountdown > 0 ){
count = MIN( frameCount - framesGenerated, data->toneCountdown );
for( i=0; i < count; ++i )
{
index = (long)data->phase;
fraction = data->phase - index;
output = data->sine[ index ] + (data->sine[ index + 1 ] - data->sine[ index ]) * fraction;
data->phase += data->lowIncrement;
while( data->phase >= TABLE_SIZE )
data->phase -= TABLE_SIZE;
if( data->toneCountdown < data->toneFadesLength )
{
output *= (-cos(((float)data->toneCountdown / (float)data->toneFadesLength) * M_PI) + 1.) * .5;
}
else if( data->toneCountdown > data->toneLength - data->toneFadesLength )
{
output *= (cos(((float)(data->toneCountdown - (data->toneLength - data->toneFadesLength)) / (float)data->toneFadesLength) * M_PI) + 1.) * .5;
}
output *= .5;  
*stereo++ = output;
*stereo++ = output;
data->toneCountdown--;
}
framesGenerated += count;
}
if( framesGenerated < frameCount && data->gap2Countdown > 0 ){
count = MIN( frameCount - framesGenerated, data->gap2Countdown );
for( i=0; i < count; ++i )
{
*stereo++ = 0.f;
*stereo++ = 0.f;
}
data->gap2Countdown -= count;
framesGenerated += count;
}
if( framesGenerated < frameCount && data->blipCountdown > 0 ){
count = MIN( frameCount - framesGenerated, data->blipCountdown );
for( i=0; i < count; ++i )
{
index = (long)data->phase;
fraction = data->phase - index;
output = data->sine[ index ] + (data->sine[ index + 1 ] - data->sine[ index ]) * fraction;
data->phase += data->highIncrement;
while( data->phase >= TABLE_SIZE )
data->phase -= TABLE_SIZE;
output *= (-cos( ((float)data->blipCountdown / (float)data->blipLength) * 2. * M_PI) + 1.) * .5;
*stereo++ = output;
*stereo++ = output;
data->blipCountdown--;
}
framesGenerated += count;
}
if( data->blipCountdown == 0 )
{
RetriggerTestSignalGenerator( data );
data->repeatCount++;
}
}
if( framesGenerated < frameCount )
{
count = frameCount - framesGenerated;
for( i=0; i < count; ++i )
{
*stereo++ = 0.f;
*stereo++ = 0.f;
}
}
}
static int IsTestSignalFinished( TestData *data )
{
if( data->repeatCount >= NUM_REPEATS )
return 1;
else
return 0;
}
static int TestCallback1( const void *inputBuffer, void *outputBuffer,
unsigned long frameCount,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
(void) inputBuffer;  
(void) timeInfo;
(void) statusFlags;
GenerateTestSignal( (TestData*)userData, (float*)outputBuffer, frameCount );
if( IsTestSignalFinished( (TestData*)userData ) )
return paComplete;
else
return paContinue;
}
volatile int testCallback2Finished = 0;
static int TestCallback2( const void *inputBuffer, void *outputBuffer,
unsigned long frameCount,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
(void) inputBuffer;  
(void) timeInfo;
(void) statusFlags;
GenerateTestSignal( (TestData*)userData, (float*)outputBuffer, frameCount );
if( IsTestSignalFinished( (TestData*)userData ) )
testCallback2Finished = 1;
return paContinue;
}
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
TestData data;
float writeBuffer[ FRAMES_PER_BUFFER * 2 ];
printf("PortAudio Test: check that stopping stream plays out all queued samples. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
InitTestSignalGenerator( &data );
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();  
outputParameters.channelCount = 2;        
outputParameters.sampleFormat = paFloat32;  
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
ResetTestSignalGenerator( &data );
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
TestCallback1,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("\nPlaying 'tone-blip' %d times using callback, stops by returning paComplete from callback.\n", NUM_REPEATS );
printf("If final blip is not intact, callback+paComplete implementation may be faulty.\n\n" );
while( (err = Pa_IsStreamActive( stream )) == 1 )
Pa_Sleep( 2 );
if( err != 0 ) goto error;
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Sleep( 500 );
ResetTestSignalGenerator( &data );
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
TestCallback1,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("\nPlaying 'tone-blip' %d times using callback, stops by returning paComplete from callback.\n", NUM_REPEATS );
printf("If final blip is not intact or is followed by garbage, callback+paComplete implementation may be faulty.\n\n" );
while( (err = Pa_IsStreamActive( stream )) == 1 )
Pa_Sleep( 5 );
printf("Waiting 5 seconds after paComplete before stopping the stream. Tests that buffers are flushed correctly.\n");
Pa_Sleep( 5000 );
if( err != 0 ) goto error;
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Sleep( 500 );
ResetTestSignalGenerator( &data );
testCallback2Finished = 0;
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
TestCallback2,
&data );
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("\nPlaying 'tone-blip' %d times using callback, stops by calling Pa_StopStream.\n", NUM_REPEATS );
printf("If final blip is not intact, callback+Pa_StopStream implementation may be faulty.\n\n" );
while( !testCallback2Finished )
Pa_Sleep( 2 );
Pa_Sleep( 500 );
err = Pa_StopStream( stream );
if( err != paNoError ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
Pa_Sleep( 500 );
ResetTestSignalGenerator( &data );
err = Pa_OpenStream(
&stream,
NULL,  
&outputParameters,
SAMPLE_RATE,
FRAMES_PER_BUFFER,
paClipOff,       
NULL,  
NULL );  
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("\nPlaying 'tone-blip' %d times using Pa_WriteStream, stops by calling Pa_StopStream.\n", NUM_REPEATS );
printf("If final blip is not intact, Pa_WriteStream+Pa_StopStream implementation may be faulty.\n\n" );
do{
GenerateTestSignal( &data, writeBuffer, FRAMES_PER_BUFFER );
err = Pa_WriteStream( stream, writeBuffer, FRAMES_PER_BUFFER );
if( err != paNoError ) goto error;
}while( !IsTestSignalFinished( &data ) );
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

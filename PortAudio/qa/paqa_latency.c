 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "loopback/src/qa_tools.h"
#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)
typedef struct
{
float sine[TABLE_SIZE];
int left_phase;
int right_phase;
char message[20];
int minFramesPerBuffer;
int maxFramesPerBuffer;
int callbackCount;
PaTime minDeltaDacTime;
PaTime maxDeltaDacTime;
PaStreamCallbackTimeInfo previousTimeInfo;
}
paTestData;
int g_testsPassed = 0;
int g_testsFailed = 0;
static int patestCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
unsigned long i;
(void) timeInfo;  
(void) statusFlags;
(void) inputBuffer;
if( data->minFramesPerBuffer > framesPerBuffer )
{
data->minFramesPerBuffer = framesPerBuffer;
}
if( data->maxFramesPerBuffer < framesPerBuffer )
{
data->maxFramesPerBuffer = framesPerBuffer;
}
if( data->callbackCount > 0 )
{
PaTime delta = timeInfo->outputBufferDacTime - data->previousTimeInfo.outputBufferDacTime;
if( data->minDeltaDacTime > delta )
{
data->minDeltaDacTime = delta;
}
if( data->maxDeltaDacTime < delta )
{
data->maxDeltaDacTime = delta;
}
}
data->previousTimeInfo = *timeInfo;
for( i=0; i<framesPerBuffer; i++ )
{
*out++ = data->sine[data->left_phase];   
*out++ = data->sine[data->right_phase];   
data->left_phase += 1;
if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
data->right_phase += 3;  
if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
}
data->callbackCount += 1;
return paContinue;
}
PaError paqaCheckLatency( PaStreamParameters *outputParamsPtr,
paTestData *dataPtr, double sampleRate, unsigned long framesPerBuffer )
{
PaError err;
PaStream *stream;
const PaStreamInfo* streamInfo;
dataPtr->minFramesPerBuffer = 9999999;
dataPtr->maxFramesPerBuffer = 0;
dataPtr->minDeltaDacTime = 9999999.0;
dataPtr->maxDeltaDacTime = 0.0;
dataPtr->callbackCount = 0;
printf("Stream parameter: suggestedOutputLatency = %g\n", outputParamsPtr->suggestedLatency );
if( framesPerBuffer == paFramesPerBufferUnspecified ){
printf("Stream parameter: user framesPerBuffer = paFramesPerBufferUnspecified\n" );
}else{
printf("Stream parameter: user framesPerBuffer = %lu\n", framesPerBuffer );
}
err = Pa_OpenStream(
&stream,
NULL,  
outputParamsPtr,
sampleRate,
framesPerBuffer,
paClipOff,       
patestCallback,
dataPtr );
if( err != paNoError ) goto error1;
streamInfo = Pa_GetStreamInfo( stream );
printf("Stream info: inputLatency  = %g\n", streamInfo->inputLatency );
printf("Stream info: outputLatency = %g\n", streamInfo->outputLatency );
err = Pa_StartStream( stream );
if( err != paNoError ) goto error2;
printf("Play for %d seconds.\n", NUM_SECONDS );
Pa_Sleep( NUM_SECONDS * 1000 );
printf("  minFramesPerBuffer = %4d\n", dataPtr->minFramesPerBuffer );
printf("  maxFramesPerBuffer = %4d\n", dataPtr->maxFramesPerBuffer );
printf("  minDeltaDacTime = %f\n", dataPtr->minDeltaDacTime );
printf("  maxDeltaDacTime = %f\n", dataPtr->maxDeltaDacTime );
err = Pa_StopStream( stream );
if( err != paNoError ) goto error2;
err = Pa_CloseStream( stream );
Pa_Sleep( 1 * 1000 );
printf("-------------------------------------\n");
return err;
error2:
Pa_CloseStream( stream );
error1:
printf("-------------------------------------\n");
return err;
}
static int paqaNoopCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
(void)inputBuffer;
(void)outputBuffer;
(void)framesPerBuffer;
(void)timeInfo;
(void)statusFlags;
(void)userData;
return paContinue;
}
static int paqaCheckMultipleSuggested( PaDeviceIndex deviceIndex, int isInput )
{
int i;
int numLoops = 10;
PaError err;
PaStream *stream;
PaStreamParameters streamParameters;
const PaStreamInfo* streamInfo;
double lowLatency;
double highLatency;
double finalLatency;
double sampleRate = SAMPLE_RATE;
const PaDeviceInfo *pdi = Pa_GetDeviceInfo( deviceIndex );
double previousLatency = 0.0;
int numChannels = 1;
float toleranceRatio = 1.0;
printf("------------------------ paqaCheckMultipleSuggested - %s\n",
(isInput ? "INPUT" : "OUTPUT") );
if( isInput )
{
lowLatency = pdi->defaultLowInputLatency;
highLatency = pdi->defaultHighInputLatency;
numChannels = (pdi->maxInputChannels < 2) ? 1 : 2;
}
else
{
lowLatency = pdi->defaultLowOutputLatency;
highLatency = pdi->defaultHighOutputLatency;
numChannels = (pdi->maxOutputChannels < 2) ? 1 : 2;
}
streamParameters.channelCount = numChannels;
streamParameters.device = deviceIndex;
streamParameters.hostApiSpecificStreamInfo = NULL;
streamParameters.sampleFormat = paFloat32;
sampleRate = pdi->defaultSampleRate;
printf(" lowLatency  = %g\n", lowLatency );
printf(" highLatency = %g\n", highLatency );
printf(" numChannels = %d\n", numChannels );
printf(" sampleRate  = %g\n", sampleRate );
if( (highLatency - lowLatency) < 0.001 )
{
numLoops = 1;
}
for( i=0; i<numLoops; i++ )
{
if( numLoops == 1 )
streamParameters.suggestedLatency = lowLatency;
else
streamParameters.suggestedLatency = lowLatency + ((highLatency - lowLatency) * i /(numLoops - 1));
printf("   suggestedLatency[%d] = %6.4f\n", i, streamParameters.suggestedLatency );
err = Pa_OpenStream(
&stream,
(isInput ? &streamParameters : NULL),
(isInput ? NULL : &streamParameters),
sampleRate,
paFramesPerBufferUnspecified,
paClipOff,       
paqaNoopCallback,
NULL );
if( err != paNoError ) goto error;
streamInfo = Pa_GetStreamInfo( stream );
err = Pa_CloseStream( stream );
if( isInput )
{
finalLatency = streamInfo->inputLatency;
}
else
{
finalLatency = streamInfo->outputLatency;
}
printf("          finalLatency = %6.4f\n", finalLatency );
toleranceRatio = ( (i == 0) || (i == ( numLoops - 1 )) ) ? 0.1 : 1.0;
QA_ASSERT_CLOSE( "final latency should be close to suggested latency",
streamParameters.suggestedLatency, finalLatency, (streamParameters.suggestedLatency * toleranceRatio) );
if( i == 0 )
{
previousLatency = finalLatency;
}
}
if( numLoops > 1 )
{
QA_ASSERT_TRUE( " final latency should increase with suggested latency", (finalLatency > previousLatency) );
}
return 0;
error:
return -1;
}
static int paqaVerifySuggestedLatency( void )
{
PaDeviceIndex id;
int result = 0;
const PaDeviceInfo *pdi;
int numDevices = Pa_GetDeviceCount();
printf("\n ------------------------ paqaVerifySuggestedLatency\n");
for( id=0; id<numDevices; id++ )             
{
pdi = Pa_GetDeviceInfo( id );
printf("\nUsing device #%d: '%s' (%s)\n", id, pdi->name, Pa_GetHostApiInfo(pdi->hostApi)->name);
if( pdi->maxOutputChannels > 0 )
{
if( paqaCheckMultipleSuggested( id, 0 ) < 0 )
{
printf("OUTPUT CHECK FAILED !!! #%d: '%s'\n", id, pdi->name);
result -= 1;
}
}
if( pdi->maxInputChannels > 0 )
{
if( paqaCheckMultipleSuggested( id, 1 ) < 0 )
{
printf("INPUT CHECK FAILED !!! #%d: '%s'\n", id, pdi->name);
result -= 1;
}
}
}
return result;
}
static int paqaVerifyDeviceInfoLatency( void )
{
PaDeviceIndex id;
const PaDeviceInfo *pdi;
int numDevices = Pa_GetDeviceCount();
printf("\n ------------------------ paqaVerifyDeviceInfoLatency\n");
for( id=0; id<numDevices; id++ )  
{
pdi = Pa_GetDeviceInfo( id );
printf("Using device #%d: '%s' (%s)\n", id, pdi->name, Pa_GetHostApiInfo(pdi->hostApi)->name);
if( pdi->maxOutputChannels > 0 )
{
printf("  Output defaultLowOutputLatency  = %f seconds\n", pdi->defaultLowOutputLatency);
printf("  Output defaultHighOutputLatency = %f seconds\n", pdi->defaultHighOutputLatency);
QA_ASSERT_TRUE( "defaultLowOutputLatency should be > 0", (pdi->defaultLowOutputLatency > 0.0) );
QA_ASSERT_TRUE( "defaultHighOutputLatency should be > 0", (pdi->defaultHighOutputLatency > 0.0) );
QA_ASSERT_TRUE( "defaultHighOutputLatency should be >= Low", (pdi->defaultHighOutputLatency >= pdi->defaultLowOutputLatency) );
}
if( pdi->maxInputChannels > 0 )
{
printf("  Input defaultLowInputLatency  = %f seconds\n", pdi->defaultLowInputLatency);
printf("  Input defaultHighInputLatency = %f seconds\n", pdi->defaultHighInputLatency);
QA_ASSERT_TRUE( "defaultLowInputLatency should be > 0", (pdi->defaultLowInputLatency > 0.0) );
QA_ASSERT_TRUE( "defaultHighInputLatency should be > 0", (pdi->defaultHighInputLatency > 0.0) );
QA_ASSERT_TRUE( "defaultHighInputLatency should be >= Low", (pdi->defaultHighInputLatency >= pdi->defaultLowInputLatency) );
}
}
return 0;
error:
return -1;
}
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaError err;
paTestData data;
const PaDeviceInfo *deviceInfo;
int i;
int framesPerBuffer;
double sampleRate = SAMPLE_RATE;
printf("\nPortAudio QA: investigate output latency.\n");
for( i=0; i<TABLE_SIZE; i++ )
{
data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
}
data.left_phase = data.right_phase = 0;
err = Pa_Initialize();
if( err != paNoError ) goto error;
if( paqaVerifyDeviceInfoLatency() < 0 ) goto error;
if( paqaVerifySuggestedLatency() < 0 ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();  
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto error;
}
printf("\n\nNow running Audio Output Tests...\n");
printf("-------------------------------------\n");
outputParameters.channelCount = 2;        
outputParameters.sampleFormat = paFloat32;  
deviceInfo = Pa_GetDeviceInfo( outputParameters.device );
printf("Using device #%d: '%s' (%s)\n", outputParameters.device, deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
printf("Device info: defaultLowOutputLatency  = %f seconds\n", deviceInfo->defaultLowOutputLatency);
printf("Device info: defaultHighOutputLatency = %f seconds\n", deviceInfo->defaultHighOutputLatency);
sampleRate = deviceInfo->defaultSampleRate;
printf("Sample Rate for following tests: %g\n", sampleRate);
outputParameters.hostApiSpecificStreamInfo = NULL;
printf("-------------------------------------\n");
printf("------------- Try a very small buffer.\n");
framesPerBuffer = 9;
outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
err = paqaCheckLatency( &outputParameters, &data, sampleRate, framesPerBuffer );
if( err != paNoError ) goto error;
printf("------------- 64 frame buffer with 1.1 * defaultLow latency.\n");
framesPerBuffer = 64;
outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency * 1.1;
err = paqaCheckLatency( &outputParameters, &data, sampleRate, framesPerBuffer );
if( err != paNoError ) goto error;
printf("------------- Try a huge buffer.\n");
framesPerBuffer = 16*1024;
outputParameters.suggestedLatency = ((double)framesPerBuffer) / sampleRate;  
err = paqaCheckLatency( &outputParameters, &data, sampleRate, framesPerBuffer );
if( err != paNoError ) goto error;
printf("------------- Try suggestedLatency = 0.0\n");
outputParameters.suggestedLatency = 0.0;
err = paqaCheckLatency( &outputParameters, &data, sampleRate, paFramesPerBufferUnspecified );
if( err != paNoError ) goto error;
printf("------------- Try suggestedLatency = defaultLowOutputLatency\n");
outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
err = paqaCheckLatency( &outputParameters, &data, sampleRate, paFramesPerBufferUnspecified );
if( err != paNoError ) goto error;
printf("------------- Try suggestedLatency = defaultHighOutputLatency\n");
outputParameters.suggestedLatency = deviceInfo->defaultHighOutputLatency;
err = paqaCheckLatency( &outputParameters, &data, sampleRate, paFramesPerBufferUnspecified );
if( err != paNoError ) goto error;
printf("------------- Try suggestedLatency = defaultHighOutputLatency * 4\n");
outputParameters.suggestedLatency = deviceInfo->defaultHighOutputLatency * 4;
err = paqaCheckLatency( &outputParameters, &data, sampleRate, paFramesPerBufferUnspecified );
if( err != paNoError ) goto error;
Pa_Terminate();
printf("SUCCESS - test finished.\n");
return err;
error:
Pa_Terminate();
fprintf( stderr, "ERROR - test failed.\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return err;
}

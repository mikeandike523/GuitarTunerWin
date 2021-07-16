 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#ifndef M_PI
#define M_PI (3.14159265)
#endif
#define SAMPLE_RATE (44100)
typedef struct
{
float sine[100];
int phase;
int sampsToGo;
}
patest1data;
static int patest1Callback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
patest1data *data = (patest1data*)userData;
float *in = (float*)inputBuffer;
float *out = (float*)outputBuffer;
int framesToCalc = framesPerBuffer;
unsigned long i = 0;
int finished;
if( data->sampsToGo < framesPerBuffer )
{
framesToCalc = data->sampsToGo;
finished = paComplete;
}
else
{
finished = paContinue;
}
for( ; i<framesToCalc; i++ )
{
*out++ = *in++ * data->sine[data->phase];   
*out++ = *in++ * data->sine[data->phase++];   
if( data->phase >= 100 )
data->phase = 0;
}
data->sampsToGo -= framesToCalc;
for( ; i<framesPerBuffer; i++ )
{
*out++ = 0;  
*out++ = 0;  
}
return finished;
}
int main(int argc, char* argv[]);
int main(int argc, char* argv[])
{
PaStream                *stream;
PaError                 err;
patest1data             data;
int                     i;
PaStreamParameters      inputParameters, outputParameters;
const PaHostErrorInfo*  herr;
printf("patest1.c\n"); fflush(stdout);
printf("Ring modulate input for 20 seconds.\n"); fflush(stdout);
for( i=0; i<100; i++ )
data.sine[i] = sin( ((double)i/100.) * M_PI * 2. );
data.phase = 0;
data.sampsToGo = SAMPLE_RATE * 20;         
err = Pa_Initialize();
inputParameters.device = Pa_GetDefaultInputDevice();     
if (inputParameters.device == paNoDevice) {
fprintf(stderr, "Error: No input default device.\n");
goto done;
}
inputParameters.channelCount = 2;                        
inputParameters.sampleFormat = paFloat32;                
inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
inputParameters.hostApiSpecificStreamInfo = NULL;
outputParameters.device = Pa_GetDefaultOutputDevice();   
if (outputParameters.device == paNoDevice) {
fprintf(stderr,"Error: No default output device.\n");
goto done;
}
outputParameters.channelCount = 2;                       
outputParameters.sampleFormat = paFloat32;               
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
outputParameters.hostApiSpecificStreamInfo = NULL;
err = Pa_OpenStream(
&stream,
&inputParameters,
&outputParameters,
(double)SAMPLE_RATE,  
512,                  
paClipOff,            
patest1Callback,
&data );
if( err != paNoError ) goto done;
err = Pa_StartStream( stream );
if( err != paNoError ) goto done;
printf( "Press any key to end.\n" ); fflush(stdout);
getc( stdin );  
err = Pa_AbortStream( stream );
if( err != paNoError ) goto done;
printf( "Waiting for stream to complete...\n" );
while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(1000);
if( err < 0 ) goto done;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto done;
done:
Pa_Terminate();
if( err != paNoError )
{
fprintf( stderr, "An error occurred while using portaudio\n" );
if( err == paUnanticipatedHostError )
{
fprintf( stderr, " unanticipated host error.\n");
herr = Pa_GetLastHostErrorInfo();
if (herr)
{
fprintf( stderr, " Error number: %ld\n", herr->errorCode );
if (herr->errorText)
fprintf( stderr, " Error text: %s\n", herr->errorText );
}
else
fprintf( stderr, " Pa_GetLastHostErrorInfo() failed!\n" );
}
else
{
fprintf( stderr, " Error number: %d\n", err );
fprintf( stderr, " Error text: %s\n", Pa_GetErrorText( err ) );
}
err = 1;           
}
printf( "bye\n" );
return err;
}

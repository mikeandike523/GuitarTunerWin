 
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <memory.h>
#include <pthread.h>
#include "portaudio.h"
#define SAMPLE_RATE         (44100)
#define FRAMES_PER_BUFFER   (2048)
static float s_buffer[FRAMES_PER_BUFFER][2];  
void *stop_thread_proc(void *arg)
{
PaStream *stream = (PaStream *)arg;
PaTime time;
for (int i = 0; i < 20; i++)
{
time = Pa_GetStreamTime( stream );
printf("Stream time = %f\n", time);
fflush(stdout);
usleep(100 * 1000);
}
printf("Call Pa_StopStream()\n");
fflush(stdout);
PaError err = Pa_StopStream( stream );
printf("Pa_StopStream() returned %d\n", err);
fflush(stdout);
return stream;
}
int main(void);
int main(void)
{
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
int result;
pthread_t thread;
printf( "PortAudio Test: output silence and stop from another thread. SR = %d, BufSize = %d\n",
SAMPLE_RATE, FRAMES_PER_BUFFER);
err = Pa_Initialize();
if( err != paNoError ) goto error;
outputParameters.device = Pa_GetDefaultOutputDevice();  
outputParameters.channelCount = 2;        
outputParameters.sampleFormat = paFloat32;  
outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency * 5;
outputParameters.hostApiSpecificStreamInfo = NULL;
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
result = pthread_create(&thread, NULL  , stop_thread_proc, stream);
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
memset( s_buffer, 0, sizeof(s_buffer) );
while( Pa_IsStreamActive(stream) > 0 )
{
err = Pa_WriteStream( stream, s_buffer, FRAMES_PER_BUFFER );
printf("Pa_WriteStream returns %d = %s\n", err, Pa_GetErrorText( err ));
if( err != paNoError )
{
err = paNoError;
break;
};
}
printf("Try to join the thread that called Pa_StopStream().\n");
result = pthread_join( thread, NULL );
printf("pthread_join returned %d\n", result);
printf("Call Pa_CloseStream\n");
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

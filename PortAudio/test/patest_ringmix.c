 
#include "stdio.h"
#include "portaudio.h"
static int myCallback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
const float *in  = (const float *) inputBuffer;
float *out = (float *) outputBuffer;
float leftInput, rightInput;
unsigned int i;
for( i=0; i<framesPerBuffer; i++ )
{
leftInput = *in++;       
rightInput = *in++;
*out++ = leftInput * rightInput;             
*out++ = 0.5f * (leftInput + rightInput);    
}
return 0;
}
int main(void)
{
PaStream *stream;
Pa_Initialize();
Pa_OpenDefaultStream(
&stream,
2, 2,                
paFloat32,  44100.0,
64,                  
myCallback, NULL );
Pa_StartStream( stream );
Pa_Sleep( 10000 );     
Pa_StopStream( stream );
Pa_CloseStream( stream );
Pa_Terminate();
return 0;
}

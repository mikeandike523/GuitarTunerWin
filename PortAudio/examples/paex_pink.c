 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define PINK_MAX_RANDOM_ROWS   (30)
#define PINK_RANDOM_BITS       (24)
#define PINK_RANDOM_SHIFT      ((sizeof(long)*8)-PINK_RANDOM_BITS)
typedef struct
{
long      pink_Rows[PINK_MAX_RANDOM_ROWS];
long      pink_RunningSum;    
int       pink_Index;         
int       pink_IndexMask;     
float     pink_Scalar;        
}
PinkNoise;
static unsigned long GenerateRandomNumber( void );
void InitializePinkNoise( PinkNoise *pink, int numRows );
float GeneratePinkNoise( PinkNoise *pink );
static unsigned long GenerateRandomNumber( void )
{
static unsigned long randSeed = 22222;
randSeed = (randSeed * 196314165) + 907633515;
return randSeed;
}
void InitializePinkNoise( PinkNoise *pink, int numRows )
{
int i;
long pmax;
pink->pink_Index = 0;
pink->pink_IndexMask = (1<<numRows) - 1;
pmax = (numRows + 1) * (1<<(PINK_RANDOM_BITS-1));
pink->pink_Scalar = 1.0f / pmax;
for( i=0; i<numRows; i++ ) pink->pink_Rows[i] = 0;
pink->pink_RunningSum = 0;
}
#define PINK_MEASURE
#ifdef PINK_MEASURE
float pinkMax = -999.0;
float pinkMin =  999.0;
#endif
float GeneratePinkNoise( PinkNoise *pink )
{
long newRandom;
long sum;
float output;
pink->pink_Index = (pink->pink_Index + 1) & pink->pink_IndexMask;
if( pink->pink_Index != 0 )
{
int numZeros = 0;
int n = pink->pink_Index;
while( (n & 1) == 0 )
{
n = n >> 1;
numZeros++;
}
pink->pink_RunningSum -= pink->pink_Rows[numZeros];
newRandom = ((long)GenerateRandomNumber()) >> PINK_RANDOM_SHIFT;
pink->pink_RunningSum += newRandom;
pink->pink_Rows[numZeros] = newRandom;
}
newRandom = ((long)GenerateRandomNumber()) >> PINK_RANDOM_SHIFT;
sum = pink->pink_RunningSum + newRandom;
output = pink->pink_Scalar * sum;
#ifdef PINK_MEASURE
if( output > pinkMax ) pinkMax = output;
else if( output < pinkMin ) pinkMin = output;
#endif
return output;
}
#define PINK_TEST
#ifdef PINK_TEST
typedef struct
{
PinkNoise   leftPink;
PinkNoise   rightPink;
unsigned int sampsToGo;
}
paTestData;
static int patestCallback(const void*                     inputBuffer,
void*                           outputBuffer,
unsigned long                   framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags           statusFlags,
void*                           userData)
{
int finished;
int i;
int numFrames;
paTestData *data = (paTestData*)userData;
float *out = (float*)outputBuffer;
(void) inputBuffer;  
if( data->sampsToGo < framesPerBuffer )
{
numFrames = data->sampsToGo;
finished = 1;
}
else
{
numFrames = framesPerBuffer;
finished = 0;
}
for( i=0; i<numFrames; i++ )
{
*out++ = GeneratePinkNoise( &data->leftPink );
*out++ = GeneratePinkNoise( &data->rightPink );
}
data->sampsToGo -= numFrames;
return finished;
}
int main(void);
int main(void)
{
PaStream*           stream;
PaError             err;
paTestData          data;
PaStreamParameters  outputParameters;
int                 totalSamps;
static const double SR  = 44100.0;
static const int    FPB = 2048;  
InitializePinkNoise( &data.leftPink,  12 );
InitializePinkNoise( &data.rightPink, 16 );
{
int i;
float pink;
for( i=0; i<20; i++ )
{
pink = GeneratePinkNoise( &data.leftPink );
printf("Pink = %f\n", pink );
}
}
data.sampsToGo = totalSamps = (int)(60.0 * SR);    
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
outputParameters.suggestedLatency =
Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
err = Pa_OpenStream(&stream,
NULL,                               
&outputParameters,
SR,                                 
FPB,                                
paClipOff,  
patestCallback,
&data);
if( err != paNoError ) goto error;
err = Pa_StartStream( stream );
if( err != paNoError ) goto error;
printf("Stereo pink noise for one minute...\n");
while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
if( err < 0 ) goto error;
err = Pa_CloseStream( stream );
if( err != paNoError ) goto error;
#ifdef PINK_MEASURE
printf("Pink min = %f, max = %f\n", pinkMin, pinkMax );
#endif
Pa_Terminate();
return 0;
error:
Pa_Terminate();
fprintf( stderr, "An error occurred while using the portaudio stream\n" );
fprintf( stderr, "Error number: %d\n", err );
fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
return 0;
}
#endif  

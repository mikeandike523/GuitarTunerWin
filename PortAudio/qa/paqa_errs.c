 
#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#define MODE_INPUT        (0)
#define MODE_OUTPUT       (1)
#define FRAMES_PER_BUFFER (64)
#define SAMPLE_RATE       (44100.0)
typedef struct PaQaData
{
unsigned long  framesLeft;
int            numChannels;
int            bytesPerSample;
int            mode;
}
PaQaData;
static int gNumPassed = 0;  
static int gNumFailed = 0;
#define EXPECT(_exp) \
do \
{ \
if ((_exp)) {\
gNumPassed++; \
} \
else { \
printf("\nERROR - 0x%x - %s for %s\n", result, Pa_GetErrorText(result), #_exp ); \
gNumFailed++; \
goto error; \
} \
} while(0)
#define HOPEFOR(_exp) \
do \
{ \
if ((_exp)) {\
gNumPassed++; \
} \
else { \
printf("\nERROR - 0x%x - %s for %s\n", result, Pa_GetErrorText(result), #_exp ); \
gNumFailed++; \
} \
} while(0)
static int QaCallback( const void*                      inputBuffer,
void*                            outputBuffer,
unsigned long                    framesPerBuffer,
const PaStreamCallbackTimeInfo*  timeInfo,
PaStreamCallbackFlags            statusFlags,
void*                            userData )
{
unsigned long   i;
unsigned char*  out = (unsigned char *) outputBuffer;
PaQaData*       data = (PaQaData *) userData;
(void)inputBuffer;  
if( data->mode == MODE_OUTPUT )
{
unsigned long numBytes = framesPerBuffer * data->numChannels * data->bytesPerSample;
for( i=0; i<numBytes; i++ )
{
*out++ = 0;
}
}
if( data->framesLeft > framesPerBuffer )
{
data->framesLeft -= framesPerBuffer;
return 0;
}
else
{
data->framesLeft = 0;
return 1;
}
}
static PaDeviceIndex FindInputOnlyDevice(void)
{
PaDeviceIndex result = Pa_GetDefaultInputDevice();
if( result != paNoDevice && Pa_GetDeviceInfo(result)->maxOutputChannels == 0 )
return result;
for( result = 0; result < Pa_GetDeviceCount(); ++result )
{
if( Pa_GetDeviceInfo(result)->maxOutputChannels == 0 )
return result;
}
return paNoDevice;
}
static PaDeviceIndex FindOutputOnlyDevice(void)
{
PaDeviceIndex result = Pa_GetDefaultOutputDevice();
if( result != paNoDevice && Pa_GetDeviceInfo(result)->maxInputChannels == 0 )
return result;
for( result = 0; result < Pa_GetDeviceCount(); ++result )
{
if( Pa_GetDeviceInfo(result)->maxInputChannels == 0 )
return result;
}
return paNoDevice;
}
static int TestBadOpens( void )
{
PaStream*           stream = NULL;
PaError             result;
PaQaData            myData;
PaStreamParameters  ipp, opp;
const PaDeviceInfo* info = NULL;
myData.framesLeft = (unsigned long) (SAMPLE_RATE * 100);  
myData.numChannels = 1;
myData.mode = MODE_OUTPUT;
ipp.device                    = opp.device                    = paNoDevice;
ipp.channelCount              = opp.channelCount              = 0;  
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
info = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
ipp.suggestedLatency          = info ? info->defaultLowInputLatency : 0.100;
info = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());
opp.suggestedLatency          = info ? info->defaultLowOutputLatency : 0.100;
HOPEFOR(((result = Pa_OpenStream(&stream, &ipp, &opp,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidDevice));
HOPEFOR(((result = Pa_OpenStream(&stream, NULL, NULL,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidDevice));
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = Pa_GetDeviceCount();  
opp.channelCount = 0;           opp.device = paNoDevice;
HOPEFOR(((result = Pa_OpenStream(&stream, &ipp, NULL,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidDevice));
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = paNoDevice;  
opp.channelCount = 0;           opp.device = Pa_GetDeviceCount();
HOPEFOR(((result = Pa_OpenStream(&stream, NULL, &opp,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidDevice));
if (Pa_GetDefaultInputDevice() != paNoDevice) {
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = Pa_GetDefaultInputDevice();
opp.channelCount = 0;           opp.device = paNoDevice;     
HOPEFOR(((result = Pa_OpenStream(&stream, &ipp, NULL,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidChannelCount));
}
if (Pa_GetDefaultOutputDevice() != paNoDevice) {
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = paNoDevice;  
opp.channelCount = 0;           opp.device = Pa_GetDefaultOutputDevice();
HOPEFOR(((result = Pa_OpenStream(&stream, NULL, &opp,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidChannelCount));
}
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 2;           ipp.device = Pa_GetDefaultInputDevice();         
opp.channelCount = 2;           opp.device = paNoDevice;
HOPEFOR(((result = Pa_OpenStream(&stream, &ipp, &opp,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidDevice));
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 2;           ipp.device = paNoDevice;
opp.channelCount = 2;           opp.device = Pa_GetDefaultOutputDevice();
HOPEFOR(((result = Pa_OpenStream(&stream, &ipp, &opp,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidDevice));
if (Pa_GetDefaultOutputDevice() != paNoDevice) {
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = paNoDevice;            
opp.channelCount = 2;           opp.device = Pa_GetDefaultOutputDevice();     
HOPEFOR(((result = Pa_OpenStream(NULL, &ipp, &opp,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paBadStreamPtr));
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = paNoDevice;
opp.channelCount = 2;           opp.device = Pa_GetDefaultOutputDevice();
HOPEFOR(((result = Pa_OpenStream(&stream, NULL, &opp,
1.0, FRAMES_PER_BUFFER,  
paClipOff, QaCallback, &myData )) == paInvalidSampleRate));
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = paNoDevice;
opp.channelCount = 2;           opp.device = Pa_GetDefaultOutputDevice();
HOPEFOR(((result = Pa_OpenStream(&stream, NULL, &opp,
10000000.0, FRAMES_PER_BUFFER,  
paClipOff, QaCallback, &myData )) == paInvalidSampleRate));
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = paNoDevice;
opp.channelCount = 2;           opp.device = Pa_GetDefaultOutputDevice();
HOPEFOR(((result = Pa_OpenStream(&stream, NULL, &opp,
SAMPLE_RATE, FRAMES_PER_BUFFER,
255,                       
QaCallback, &myData )) == paInvalidFlag));
}
if( FindInputOnlyDevice() != paNoDevice )
{
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 0;           ipp.device = paNoDevice;  
opp.channelCount = 2;           opp.device = FindInputOnlyDevice();
HOPEFOR(((result = Pa_OpenStream(&stream, NULL, &opp,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidChannelCount));
}
if( FindOutputOnlyDevice() != paNoDevice )
{
ipp.hostApiSpecificStreamInfo = opp.hostApiSpecificStreamInfo = NULL;
ipp.sampleFormat              = opp.sampleFormat              = paFloat32;
ipp.channelCount = 2;           ipp.device = FindOutputOnlyDevice();
opp.channelCount = 0;           opp.device = paNoDevice;   
HOPEFOR(((result = Pa_OpenStream(&stream, &ipp, NULL,
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paInvalidChannelCount));
}
if( stream != NULL ) Pa_CloseStream( stream );
return result;
}
static int TestBadActions( void )
{
PaStream*           stream = NULL;
const PaDeviceInfo* deviceInfo = NULL;
PaError             result = 0;
PaQaData            myData;
PaStreamParameters  opp;
const PaDeviceInfo* info = NULL;
myData.framesLeft = (unsigned long)(SAMPLE_RATE * 100);  
myData.numChannels = 1;
myData.mode = MODE_OUTPUT;
opp.device                    = Pa_GetDefaultOutputDevice();  
opp.channelCount              = 2;                            
opp.hostApiSpecificStreamInfo = NULL;
opp.sampleFormat              = paFloat32;
info = Pa_GetDeviceInfo(opp.device);
opp.suggestedLatency          = info ? info->defaultLowOutputLatency : 0.100;
if (opp.device != paNoDevice) {
HOPEFOR(((result = Pa_OpenStream(&stream, NULL,  
&opp,           
SAMPLE_RATE, FRAMES_PER_BUFFER,
paClipOff, QaCallback, &myData )) == paNoError));
}
HOPEFOR(((deviceInfo = Pa_GetDeviceInfo(paNoDevice))    == NULL));
HOPEFOR(((deviceInfo = Pa_GetDeviceInfo(87654))    == NULL));
HOPEFOR(((result = Pa_StartStream(NULL))    == paBadStreamPtr));
HOPEFOR(((result = Pa_StopStream(NULL))     == paBadStreamPtr));
HOPEFOR(((result = Pa_IsStreamStopped(NULL)) == paBadStreamPtr));
HOPEFOR(((result = Pa_IsStreamActive(NULL)) == paBadStreamPtr));
HOPEFOR(((result = Pa_CloseStream(NULL))    == paBadStreamPtr));
HOPEFOR(((result = Pa_SetStreamFinishedCallback(NULL, NULL)) == paBadStreamPtr));
HOPEFOR(((result = !Pa_GetStreamInfo(NULL))));
HOPEFOR(((result = Pa_GetStreamTime(NULL))  == 0.0));
HOPEFOR(((result = Pa_GetStreamCpuLoad(NULL))  == 0.0));
HOPEFOR(((result = Pa_ReadStream(NULL, NULL, 0))  == paBadStreamPtr));
HOPEFOR(((result = Pa_WriteStream(NULL, NULL, 0))  == paBadStreamPtr));
if (stream != NULL) Pa_CloseStream(stream);
return result;
}
int main(void);
int main(void)
{
PaError result;
EXPECT(((result = Pa_Initialize()) == paNoError));
TestBadOpens();
TestBadActions();
error:
Pa_Terminate();
printf("QA Report: %d passed, %d failed.\n", gNumPassed, gNumFailed);
return 0;
}

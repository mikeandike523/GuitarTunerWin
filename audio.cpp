#include "audio.h"
#include "portaudio.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <windows.h>
namespace audio{
#define CLIP_LEVEL 0.95f
#define DEVICE_RESCAN_DELAY 500
#define LATENCY_REDUCTION 2.0
#define FRAMES_PER_BUFFER 64
static PaStream *stream;
int stream_err=0;  
static DeviceInfo capture_device;
static DeviceInfo playback_device;
static DeviceInfo output_device;
static DeviceInfo input_device;
static int pa_is_init=0;
static const int DOWN_SAMPLING = 16;
float table[1024];
static int head = 0;
static int sample_rate = 44100;
static int sc = 0;
static std::vector<DeviceInfo> input_devices;
static float gain = 1.0;
static void (*modal_routine)(char * message);
int get_down_sampling(){
return DOWN_SAMPLING;
}
int get_sample_rate(){
return sample_rate;
}
static int audio_callback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
float *out = (float*)outputBuffer;
(void) timeInfo; 
(void) statusFlags;
float *in = (float*) inputBuffer;
for(unsigned long i=0;i<framesPerBuffer;i++){
float c;
c=*in++;
c*=gain;
if(c>CLIP_LEVEL)
c=CLIP_LEVEL;
if(c<-CLIP_LEVEL)
c=-CLIP_LEVEL;
table[head]=c;
*out++=c;
sc++;
if(sc==DOWN_SAMPLING){
head++;
sc=0;
}
if(head==1024)
head=0;
}
return paContinue;
}
static int audio_callback_no_playback( const void *inputBuffer, void *outputBuffer,
unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo* timeInfo,
PaStreamCallbackFlags statusFlags,
void *userData )
{
(void) outputBuffer;
(void) timeInfo; 
(void) statusFlags;
float *in = (float*) inputBuffer;
for(unsigned long i=0;i<framesPerBuffer;i++){
float c;
if(capture_device.channels==2)
{
float a = *in++;
float b = *in++;
c=0.5*(a+b);
}else{
c=*in++;
}
table[head]=c;
sc++;
if(sc==DOWN_SAMPLING){
head++;
sc=0;
}
if(head==1024)
head=0;
}
return paContinue;
}
static int is_desired_input_device(DeviceInfo dinf){
std::string name = std::string(dinf.name);
std::transform(name.begin(), name.end(), name.begin(),
[](unsigned char c){ return std::tolower(c); });    
if(name.find("rocksmith")!=-1||name.find("usb")!=-1)
return 1;
return 0;
}
DeviceInfo find_input_device(){
DeviceInfo dinf=audio::null_device();
for( int i=0; i<input_devices.size(); i++ )
{
DeviceInfo dinfo = input_devices[i];
if(is_desired_input_device(dinfo))
dinf=dinfo;
}
return dinf;
}
DeviceInfo find_output_device(int refresh){
if(refresh){
DeviceInfo dinf;
int def_out = Pa_GetDefaultOutputDevice();  
dinf.pa_id=-1;
if(def_out!=paNoDevice){
dinf.pa_id=def_out;
const PaDeviceInfo *deviceInfo;
deviceInfo = Pa_GetDeviceInfo( dinf.pa_id);
dinf.name=deviceInfo->name;
dinf.channels=deviceInfo->maxOutputChannels;
dinf.latency=deviceInfo->defaultLowOutputLatency;
dinf.Fs=deviceInfo->defaultSampleRate;
}
output_device = dinf;
}
return output_device;
}
int audio_init(){
if(!pa_is_init)
{Pa_Initialize();
pa_is_init=1;}
return 0;
}
int init(void (*MODAL_ROUTINE)(char * message)){
audio_init();
modal_routine=MODAL_ROUTINE;
input_device=null_device();
output_device=null_device();
return 0;
}
int terminate(){
if(pa_is_init)
{Pa_Terminate();
pa_is_init=0;}
return 0;
}
int reset(){
terminate();
audio_init();
return 0;
}
int play(DeviceInfo input_device, float input_gain,int playback_enabled){
capture_device=input_device;
gain=input_gain;
playback_device=find_output_device(1);
if(capture_device.pa_id==-1&&playback_device.pa_id==-1){
modal_routine("Device Failure.\nMake sure you specified a valid input device\nand your PC's default audio output is working properly.");
return 1;
}else if(capture_device.pa_id==-1){
modal_routine("Please specify an input device.");
return 1;
}else if(playback_device.pa_id==-1){
modal_routine("Device Failure.\nMake sure your PC's default audio output is working properly.");
return 1;
}
if(capture_device.Fs!=playback_device.Fs){
modal_routine("Device Failure.\nSampling Frequency Mismatch.");
return 1;
}
if(capture_device.Fs!=44100||playback_device.Fs!=44100){
char warning[256];
sprintf(warning,"Warning,\n44100Hz audio devices are recommended.");
modal_routine(warning);
}
float latency=playback_device.latency/LATENCY_REDUCTION;
if(capture_device.latency/LATENCY_REDUCTION<latency)
latency=capture_device.latency/LATENCY_REDUCTION;
sample_rate=capture_device.Fs;
PaStreamParameters outputParameters;
outputParameters.device=playback_device.pa_id;
outputParameters.channelCount =  1;  
outputParameters.sampleFormat = paFloat32;
outputParameters.suggestedLatency = latency;
outputParameters.hostApiSpecificStreamInfo = NULL;
PaStreamParameters inputParameters;
inputParameters.device=capture_device.pa_id;
inputParameters.channelCount =  1;   
inputParameters.sampleFormat = paFloat32; 
inputParameters.suggestedLatency = latency;
inputParameters.hostApiSpecificStreamInfo = NULL;
if(playback_enabled){
stream_err=Pa_OpenStream(
&stream,
&inputParameters, 
&outputParameters,
capture_device.Fs,
FRAMES_PER_BUFFER,
paClipOff,
audio_callback,
NULL );
}else{
stream_err=Pa_OpenStream(
&stream,
&inputParameters, 
NULL,
capture_device.Fs,
FRAMES_PER_BUFFER,
paClipOff,
audio_callback_no_playback,
NULL );
}
if(stream_err==0){
Pa_StartStream( stream );
}else{
modal_routine("Audio Streaming Failure.\nMake sure you specified a valid input device\nand your PC's default audio output is working properly.\nBoth Input and Output must be Mono compatible.");
};
return stream_err;
}
int stop(){
if(stream_err==0){
int err=Pa_StopStream( stream );
err=Pa_CloseStream( stream );
return err;
}
return 1;
}
float * get_table(){
return table;
}
std::vector<DeviceInfo>& list_input_devices(int refresh){
if(refresh){
input_devices.clear();
Sleep(DEVICE_RESCAN_DELAY);
reset();
int numDevices;
numDevices = Pa_GetDeviceCount();
const PaDeviceInfo *deviceInfo;
for( int i=0; i<numDevices; i++ )
{
deviceInfo = Pa_GetDeviceInfo( i );
int hostid =deviceInfo->hostApi;
const PaHostApiInfo * hostinf = Pa_GetHostApiInfo(hostid);
if(std::string(hostinf->name).find("MME")==-1)
continue;
DeviceInfo dinfo;
dinfo.pa_id=i;
dinfo.name=deviceInfo->name;
printf("%s\n",dinfo.name);
if(deviceInfo->maxInputChannels>0){
dinfo.channels=deviceInfo->maxInputChannels;
dinfo.latency=deviceInfo->defaultLowInputLatency;
dinfo.Fs=deviceInfo->defaultSampleRate;
input_devices.push_back(dinfo);
}
}
}
return input_devices;
}
DeviceInfo null_device(){
DeviceInfo inf;
const char name[1]={'\0'};
inf.name=name;
inf.pa_id=-1;
inf.Fs=0;
inf.latency=0;
inf.channels=0;
return inf;
}
}
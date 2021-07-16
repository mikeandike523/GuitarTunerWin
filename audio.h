#ifndef AUDIO_H
#define AUDIO_H
#include "portaudio.h"
#include <vector>
namespace audio{
typedef struct DeviceInfo {int pa_id; int Fs; float latency; const char * name; int channels;} DeviceInfo;
DeviceInfo find_input_device();  
DeviceInfo find_output_device(int refresh);
int play(DeviceInfo input_device,float input_gain,int playback_enabled);
int stop();
int init(void (*MODAL_ROUTINE)(char * message));
int terminate();
float * get_table();
int get_down_sampling();
int get_sample_rate();
DeviceInfo null_device();
std::vector<DeviceInfo>& list_input_devices(int refresh);
int reset();
}
#endif
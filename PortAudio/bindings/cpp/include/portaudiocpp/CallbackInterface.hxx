#ifndef INCLUDED_PORTAUDIO_CALLBACKINTERFACE_HXX
#define INCLUDED_PORTAUDIO_CALLBACKINTERFACE_HXX
#include "portaudio.h"
namespace portaudio
{
class CallbackInterface
{
public:
virtual ~CallbackInterface() {}
virtual int paCallbackFun(const void *inputBuffer, void *outputBuffer, unsigned long numFrames,
const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags) = 0;
};
namespace impl
{
extern "C"
{
int callbackInterfaceToPaCallbackAdapter(const void *inputBuffer, void *outputBuffer, unsigned long numFrames,
const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
void *userData);
}  
}
}  
#endif  

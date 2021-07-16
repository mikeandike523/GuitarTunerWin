#include "portaudiocpp/CallbackInterface.hxx"
namespace portaudio
{
namespace impl
{
int callbackInterfaceToPaCallbackAdapter(const void *inputBuffer, void *outputBuffer, unsigned long numFrames,
const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
CallbackInterface *cb = static_cast<CallbackInterface *>(userData);
return cb->paCallbackFun(inputBuffer, outputBuffer, numFrames, timeInfo, statusFlags);
}
}  
}  

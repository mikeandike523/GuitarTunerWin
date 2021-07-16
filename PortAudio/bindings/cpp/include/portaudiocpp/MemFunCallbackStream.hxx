#ifndef INCLUDED_PORTAUDIO_MEMFUNCALLBACKSTREAM_HXX
#define INCLUDED_PORTAUDIO_MEMFUNCALLBACKSTREAM_HXX
#include "portaudio.h"
#include "portaudiocpp/CallbackStream.hxx"
#include "portaudiocpp/CallbackInterface.hxx"
#include "portaudiocpp/StreamParameters.hxx"
#include "portaudiocpp/Exception.hxx"
#include "portaudiocpp/InterfaceCallbackStream.hxx"
namespace portaudio
{
template<typename T>
class MemFunCallbackStream : public CallbackStream
{
public:
typedef int (T::*CallbackFunPtr)(const void *, void *, unsigned long, const PaStreamCallbackTimeInfo *,
PaStreamCallbackFlags);
MemFunCallbackStream()
{
}
MemFunCallbackStream(const StreamParameters &parameters, T &instance, CallbackFunPtr memFun) : adapter_(instance, memFun)
{
open(parameters);
}
~MemFunCallbackStream()
{
close();
}
void open(const StreamParameters &parameters, T &instance, CallbackFunPtr memFun)
{
adapter_.init(instance, memFun);
open(parameters);
}
private:
MemFunCallbackStream(const MemFunCallbackStream &);  
MemFunCallbackStream &operator=(const MemFunCallbackStream &);  
class MemFunToCallbackInterfaceAdapter : public CallbackInterface
{
public:
MemFunToCallbackInterfaceAdapter() {}
MemFunToCallbackInterfaceAdapter(T &instance, CallbackFunPtr memFun) : instance_(&instance), memFun_(memFun) {}
void init(T &instance, CallbackFunPtr memFun)
{
instance_ = &instance;
memFun_ = memFun;
}
int paCallbackFun(const void *inputBuffer, void *outputBuffer, unsigned long numFrames,
const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
{
return (instance_->*memFun_)(inputBuffer, outputBuffer, numFrames, timeInfo, statusFlags);
}
private:
T *instance_;
CallbackFunPtr memFun_;
};
MemFunToCallbackInterfaceAdapter adapter_;
void open(const StreamParameters &parameters)
{
PaError err = Pa_OpenStream(&stream_, parameters.inputParameters().paStreamParameters(), parameters.outputParameters().paStreamParameters(),
parameters.sampleRate(), parameters.framesPerBuffer(), parameters.flags(), &impl::callbackInterfaceToPaCallbackAdapter,
static_cast<void *>(&adapter_));
if (err != paNoError)
throw PaException(err);
}
};
}  
#endif  

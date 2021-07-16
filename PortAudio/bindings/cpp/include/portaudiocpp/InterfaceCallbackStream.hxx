#ifndef INCLUDED_PORTAUDIO_INTERFACECALLBACKSTREAM_HXX
#define INCLUDED_PORTAUDIO_INTERFACECALLBACKSTREAM_HXX
#include "portaudio.h"
#include "portaudiocpp/CallbackStream.hxx"
namespace portaudio
{
class StreamParameters;
class CallbackInterface;
}
namespace portaudio
{
class InterfaceCallbackStream : public CallbackStream
{
public:
InterfaceCallbackStream();
InterfaceCallbackStream(const StreamParameters &parameters, CallbackInterface &instance);
~InterfaceCallbackStream();
void open(const StreamParameters &parameters, CallbackInterface &instance);
private:
InterfaceCallbackStream(const InterfaceCallbackStream &);  
InterfaceCallbackStream &operator=(const InterfaceCallbackStream &);  
};
}  
#endif  

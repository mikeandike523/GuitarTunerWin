#ifndef INCLUDED_PORTAUDIO_CALLBACKSTREAM_HXX
#define INCLUDED_PORTAUDIO_CALLBACKSTREAM_HXX
#include "portaudio.h"
#include "portaudiocpp/Stream.hxx"
namespace portaudio
{
class CallbackStream : public Stream
{
protected:
CallbackStream();
virtual ~CallbackStream();
public:
double cpuLoad() const;
private:
CallbackStream(const CallbackStream &);  
CallbackStream &operator=(const CallbackStream &);  
};
}  
#endif  

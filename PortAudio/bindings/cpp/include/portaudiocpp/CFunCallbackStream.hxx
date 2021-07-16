#ifndef INCLUDED_PORTAUDIO_CFUNCALLBACKSTREAM_HXX
#define INCLUDED_PORTAUDIO_CFUNCALLBACKSTREAM_HXX
#include "portaudio.h"
#include "portaudiocpp/CallbackStream.hxx"
namespace portaudio
{
class StreamParameters;
}
namespace portaudio
{
class CFunCallbackStream : public CallbackStream
{
public:
CFunCallbackStream();
CFunCallbackStream(const StreamParameters &parameters, PaStreamCallback *funPtr, void *userData);
~CFunCallbackStream();
void open(const StreamParameters &parameters, PaStreamCallback *funPtr, void *userData);
private:
CFunCallbackStream(const CFunCallbackStream &);  
CFunCallbackStream &operator=(const CFunCallbackStream &);  
};
}  
#endif  

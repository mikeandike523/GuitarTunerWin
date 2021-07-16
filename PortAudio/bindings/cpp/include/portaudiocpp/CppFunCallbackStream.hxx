#ifndef INCLUDED_PORTAUDIO_CPPFUNCALLBACKSTREAM_HXX
#define INCLUDED_PORTAUDIO_CPPFUNCALLBACKSTREAM_HXX
#include "portaudio.h"
#include "portaudiocpp/CallbackStream.hxx"
namespace portaudio
{
class StreamParameters;
}
namespace portaudio
{
namespace impl
{
extern "C"
{
int cppCallbackToPaCallbackAdapter(const void *inputBuffer, void *outputBuffer, unsigned long numFrames,
const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
void *userData);
}  
}
class FunCallbackStream : public CallbackStream
{
public:
typedef int (*CallbackFunPtr)(const void *inputBuffer, void *outputBuffer, unsigned long numFrames,
const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
void *userData);
struct CppToCCallbackData
{
CppToCCallbackData();
CppToCCallbackData(CallbackFunPtr funPtr, void *userData);
void init(CallbackFunPtr funPtr, void *userData);
CallbackFunPtr funPtr;
void *userData;
};
FunCallbackStream();
FunCallbackStream(const StreamParameters &parameters, CallbackFunPtr funPtr, void *userData);
~FunCallbackStream();
void open(const StreamParameters &parameters, CallbackFunPtr funPtr, void *userData);
private:
FunCallbackStream(const FunCallbackStream &);  
FunCallbackStream &operator=(const FunCallbackStream &);  
CppToCCallbackData adapterData_;
void open(const StreamParameters &parameters);
};
}  
#endif  

#ifndef INCLUDED_PORTAUDIO_STREAM_HXX
#define INCLUDED_PORTAUDIO_STREAM_HXX
#include "portaudio.h"
namespace portaudio
{
class StreamParameters;
}
namespace portaudio
{
class Stream
{
public:
virtual ~Stream();
virtual void close();
bool isOpen() const;
void setStreamFinishedCallback(PaStreamFinishedCallback *callback);
void start();
void stop();
void abort();
bool isStopped() const;
bool isActive() const;
PaTime inputLatency() const;
PaTime outputLatency() const;
double sampleRate() const;
PaTime time() const;
const PaStream *paStream() const;
PaStream *paStream();
protected:
Stream();  
PaStream *stream_;
private:
Stream(const Stream &);  
Stream &operator=(const Stream &);  
};
}  
#endif  

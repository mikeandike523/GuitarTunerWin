#include "portaudiocpp/Stream.hxx"
#include <cstddef>
#include "portaudiocpp/Exception.hxx"
#include "portaudiocpp/System.hxx"
namespace portaudio
{
Stream::Stream() : stream_(NULL)
{
}
Stream::~Stream()
{
}
void Stream::close()
{
if (isOpen() && System::exists())
{
PaError err = Pa_CloseStream(stream_);
stream_ = NULL;
if (err != paNoError)
throw PaException(err);
}
}
bool Stream::isOpen() const
{
return (stream_ != NULL);
}
void Stream::setStreamFinishedCallback(PaStreamFinishedCallback *callback)
{
PaError err = Pa_SetStreamFinishedCallback(stream_, callback);
if (err != paNoError)
throw PaException(err);
}
void Stream::start()
{
PaError err = Pa_StartStream(stream_);
if (err != paNoError)
throw PaException(err);
}
void Stream::stop()
{
PaError err = Pa_StopStream(stream_);
if (err != paNoError)
throw PaException(err);
}
void Stream::abort()
{
PaError err = Pa_AbortStream(stream_);
if (err != paNoError)
throw PaException(err);
}
bool Stream::isStopped() const
{
PaError ret = Pa_IsStreamStopped(stream_);
if (ret < 0)
throw PaException(ret);
return (ret == 1);
}
bool Stream::isActive() const
{
PaError ret = Pa_IsStreamActive(stream_);
if (ret < 0)
throw PaException(ret);
return (ret == 1);
}
PaTime Stream::inputLatency() const
{
const PaStreamInfo *info = Pa_GetStreamInfo(stream_);
if (info == NULL)
{
throw PaException(paInternalError);
return PaTime(0.0);
}
return info->inputLatency;
}
PaTime Stream::outputLatency() const
{
const PaStreamInfo *info = Pa_GetStreamInfo(stream_);
if (info == NULL)
{
throw PaException(paInternalError);
return PaTime(0.0);
}
return info->outputLatency;
}
double Stream::sampleRate() const
{
const PaStreamInfo *info = Pa_GetStreamInfo(stream_);
if (info == NULL)
{
throw PaException(paInternalError);
return 0.0;
}
return info->sampleRate;
}
PaTime Stream::time() const
{
return Pa_GetStreamTime(stream_);
}
const PaStream *Stream::paStream() const
{
return stream_;
}
PaStream *Stream::paStream()
{
return stream_;
}
}  

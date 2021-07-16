#ifndef INCLUDED_PORTAUDIO_SINGLEDIRECTIONSTREAMPARAMETERS_HXX
#define INCLUDED_PORTAUDIO_SINGLEDIRECTIONSTREAMPARAMETERS_HXX
#include <cstddef>
#include "portaudio.h"
#include "portaudiocpp/System.hxx"
#include "portaudiocpp/SampleDataFormat.hxx"
namespace portaudio
{
class Device;
}
namespace portaudio
{
class DirectionSpecificStreamParameters
{
public:
static DirectionSpecificStreamParameters null();
DirectionSpecificStreamParameters();
DirectionSpecificStreamParameters(const Device &device, int numChannels, SampleDataFormat format,
bool interleaved, PaTime suggestedLatency, void *hostApiSpecificStreamInfo);
void setDevice(const Device &device);
void setNumChannels(int numChannels);
void setSampleFormat(SampleDataFormat format, bool interleaved = true);
void setHostApiSpecificSampleFormat(PaSampleFormat format, bool interleaved = true);
void setSuggestedLatency(PaTime latency);
void setHostApiSpecificStreamInfo(void *streamInfo);
PaStreamParameters *paStreamParameters();
const PaStreamParameters *paStreamParameters() const;
Device &device() const;
int numChannels() const;
SampleDataFormat sampleFormat() const;
bool isSampleFormatInterleaved() const;
bool isSampleFormatHostApiSpecific() const;
PaSampleFormat hostApiSpecificSampleFormat() const;
PaTime suggestedLatency() const;
void *hostApiSpecificStreamInfo() const;
private:
PaStreamParameters paStreamParameters_;
};
}  
#endif  

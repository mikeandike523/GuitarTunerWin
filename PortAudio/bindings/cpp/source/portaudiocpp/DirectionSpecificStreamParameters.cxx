#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
#include "portaudiocpp/Device.hxx"
namespace portaudio
{
DirectionSpecificStreamParameters DirectionSpecificStreamParameters::null()
{
DirectionSpecificStreamParameters tmp;
tmp.paStreamParameters_.device = paNoDevice;
tmp.paStreamParameters_.channelCount = 0;
return tmp;
}
DirectionSpecificStreamParameters::DirectionSpecificStreamParameters()
{
}
DirectionSpecificStreamParameters::DirectionSpecificStreamParameters(const Device &device, int numChannels,
SampleDataFormat format, bool interleaved, PaTime suggestedLatency, void *hostApiSpecificStreamInfo)
{
setDevice(device);
setNumChannels(numChannels);
setSampleFormat(format, interleaved);
setSuggestedLatency(suggestedLatency);
setHostApiSpecificStreamInfo(hostApiSpecificStreamInfo);
}
void DirectionSpecificStreamParameters::setDevice(const Device &device)
{
paStreamParameters_.device = device.index();
}
void DirectionSpecificStreamParameters::setNumChannels(int numChannels)
{
paStreamParameters_.channelCount = numChannels;
}
void DirectionSpecificStreamParameters::setSampleFormat(SampleDataFormat format, bool interleaved)
{
paStreamParameters_.sampleFormat = static_cast<PaSampleFormat>(format);
if (!interleaved)
paStreamParameters_.sampleFormat |= paNonInterleaved;
}
void DirectionSpecificStreamParameters::setHostApiSpecificSampleFormat(PaSampleFormat format, bool interleaved)
{
paStreamParameters_.sampleFormat = format;
paStreamParameters_.sampleFormat |= paCustomFormat;
if (!interleaved)
paStreamParameters_.sampleFormat |= paNonInterleaved;
}
void DirectionSpecificStreamParameters::setSuggestedLatency(PaTime latency)
{
paStreamParameters_.suggestedLatency = latency;
}
void DirectionSpecificStreamParameters::setHostApiSpecificStreamInfo(void *streamInfo)
{
paStreamParameters_.hostApiSpecificStreamInfo = streamInfo;
}
PaStreamParameters *DirectionSpecificStreamParameters::paStreamParameters()
{
if (paStreamParameters_.channelCount > 0 && paStreamParameters_.device != paNoDevice)
return &paStreamParameters_;
else
return NULL;
}
const PaStreamParameters *DirectionSpecificStreamParameters::paStreamParameters() const
{
if (paStreamParameters_.channelCount > 0 && paStreamParameters_.device != paNoDevice)
return &paStreamParameters_;
else
return NULL;
}
Device &DirectionSpecificStreamParameters::device() const
{
return System::instance().deviceByIndex(paStreamParameters_.device);
}
int DirectionSpecificStreamParameters::numChannels() const
{
return paStreamParameters_.channelCount;
}
SampleDataFormat DirectionSpecificStreamParameters::sampleFormat() const
{
if (isSampleFormatHostApiSpecific())
return INVALID_FORMAT;
else
return static_cast<SampleDataFormat>(paStreamParameters_.sampleFormat & ~paNonInterleaved);
}
bool DirectionSpecificStreamParameters::isSampleFormatInterleaved() const
{
return ((paStreamParameters_.sampleFormat & paNonInterleaved) == 0);
}
bool DirectionSpecificStreamParameters::isSampleFormatHostApiSpecific() const
{
return ((paStreamParameters_.sampleFormat & paCustomFormat) == 0);
}
PaSampleFormat DirectionSpecificStreamParameters::hostApiSpecificSampleFormat() const
{
if (isSampleFormatHostApiSpecific())
return paStreamParameters_.sampleFormat & ~paCustomFormat & ~paNonInterleaved;
else
return 0;
}
PaTime DirectionSpecificStreamParameters::suggestedLatency() const
{
return paStreamParameters_.suggestedLatency;
}
void *DirectionSpecificStreamParameters::hostApiSpecificStreamInfo() const
{
return paStreamParameters_.hostApiSpecificStreamInfo;
}
}  

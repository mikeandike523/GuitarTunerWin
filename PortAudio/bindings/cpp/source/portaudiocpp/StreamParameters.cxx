#include "portaudiocpp/StreamParameters.hxx"
#include <cstddef>
#include "portaudiocpp/Device.hxx"
namespace portaudio
{
StreamParameters::StreamParameters()
{
}
StreamParameters::StreamParameters(const DirectionSpecificStreamParameters &inputParameters,
const DirectionSpecificStreamParameters &outputParameters, double sampleRate, unsigned long framesPerBuffer,
PaStreamFlags flags) : inputParameters_(inputParameters), outputParameters_(outputParameters),
sampleRate_(sampleRate), framesPerBuffer_(framesPerBuffer), flags_(flags)
{
}
void StreamParameters::setSampleRate(double sampleRate)
{
sampleRate_ = sampleRate;
}
void StreamParameters::setFramesPerBuffer(unsigned long framesPerBuffer)
{
framesPerBuffer_ = framesPerBuffer;
}
void StreamParameters::setFlag(PaStreamFlags flag)
{
flags_ |= flag;
}
void StreamParameters::unsetFlag(PaStreamFlags flag)
{
flags_ &= ~flag;
}
void StreamParameters::clearFlags()
{
flags_ = paNoFlag;
}
void StreamParameters::setInputParameters(const DirectionSpecificStreamParameters &parameters)
{
inputParameters_ = parameters;
}
void StreamParameters::setOutputParameters(const DirectionSpecificStreamParameters &parameters)
{
outputParameters_ = parameters;
}
bool StreamParameters::isSupported() const
{
return (Pa_IsFormatSupported(inputParameters_.paStreamParameters(),
outputParameters_.paStreamParameters(), sampleRate_) == paFormatIsSupported);
}
double StreamParameters::sampleRate() const
{
return sampleRate_;
}
unsigned long StreamParameters::framesPerBuffer() const
{
return framesPerBuffer_;
}
PaStreamFlags StreamParameters::flags() const
{
return flags_;
}
bool StreamParameters::isFlagSet(PaStreamFlags flag) const
{
return ((flags_ & flag) != 0);
}
DirectionSpecificStreamParameters &StreamParameters::inputParameters()
{
return inputParameters_;
}
const DirectionSpecificStreamParameters &StreamParameters::inputParameters() const
{
return inputParameters_;
}
DirectionSpecificStreamParameters &StreamParameters::outputParameters()
{
return outputParameters_;
}
const DirectionSpecificStreamParameters &StreamParameters::outputParameters() const
{
return outputParameters_;
}
}  

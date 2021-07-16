#ifndef INCLUDED_PORTAUDIO_STREAMPARAMETERS_HXX
#define INCLUDED_PORTAUDIO_STREAMPARAMETERS_HXX
#include "portaudio.h"
#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
namespace portaudio
{
class StreamParameters
{
public:
StreamParameters();
StreamParameters(const DirectionSpecificStreamParameters &inputParameters,
const DirectionSpecificStreamParameters &outputParameters, double sampleRate,
unsigned long framesPerBuffer, PaStreamFlags flags);
void setInputParameters(const DirectionSpecificStreamParameters &parameters);
void setOutputParameters(const DirectionSpecificStreamParameters &parameters);
void setSampleRate(double sampleRate);
void setFramesPerBuffer(unsigned long framesPerBuffer);
void setFlag(PaStreamFlags flag);
void unsetFlag(PaStreamFlags flag);
void clearFlags();
bool isSupported() const;
DirectionSpecificStreamParameters &inputParameters();
const DirectionSpecificStreamParameters &inputParameters() const;
DirectionSpecificStreamParameters &outputParameters();
const DirectionSpecificStreamParameters &outputParameters() const;
double sampleRate() const;
unsigned long framesPerBuffer() const;
PaStreamFlags flags() const;
bool isFlagSet(PaStreamFlags flag) const;
private:
DirectionSpecificStreamParameters inputParameters_;
DirectionSpecificStreamParameters outputParameters_;
double sampleRate_;
unsigned long framesPerBuffer_;
PaStreamFlags flags_;
};
}  
#endif  

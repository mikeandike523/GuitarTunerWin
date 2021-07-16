 
#include <iostream>
#include <cmath>
#include <cassert>
#include <cstddef>
#include "portaudiocpp/PortAudioCpp.hxx"
const int NUM_SECONDS = 5;
const double SAMPLE_RATE = 44100.0;
const int FRAMES_PER_BUFFER = 64;
const int TABLE_SIZE = 200;
class SineGenerator
{
public:
SineGenerator(int tableSize) : tableSize_(tableSize), leftPhase_(0), rightPhase_(0)
{
const double PI = 3.14159265;
table_ = new float[tableSize];
for (int i = 0; i < tableSize; ++i)
{
table_[i] = 0.125f * (float)sin(((double)i/(double)tableSize)*PI*2.);
}
}
~SineGenerator()
{
delete[] table_;
}
int generate(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
{
assert(outputBuffer != NULL);
float **out = static_cast<float **>(outputBuffer);
for (unsigned int i = 0; i < framesPerBuffer; ++i)
{
out[0][i] = table_[leftPhase_];
out[1][i] = table_[rightPhase_];
leftPhase_ += 1;
if (leftPhase_ >= tableSize_)
leftPhase_ -= tableSize_;
rightPhase_ += 3;
if (rightPhase_ >= tableSize_)
rightPhase_ -= tableSize_;
}
return paContinue;
}
private:
float *table_;
int tableSize_;
int leftPhase_;
int rightPhase_;
};
int main(int, char *[]);
int main(int, char *[])
{
try
{
SineGenerator sineGenerator(TABLE_SIZE);
std::cout << "Setting up PortAudio..." << std::endl;
portaudio::AutoSystem autoSys;
portaudio::System &sys = portaudio::System::instance();
portaudio::DirectionSpecificStreamParameters outParams(sys.defaultOutputDevice(), 2, portaudio::FLOAT32, false, sys.defaultOutputDevice().defaultLowOutputLatency(), NULL);
portaudio::StreamParameters params(portaudio::DirectionSpecificStreamParameters::null(), outParams, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff);
std::cout << "Opening stereo output stream..." << std::endl;
portaudio::MemFunCallbackStream<SineGenerator> stream(params, sineGenerator, &SineGenerator::generate);
std::cout << "Starting playback for " << NUM_SECONDS << " seconds." << std::endl;
stream.start();
sys.sleep(NUM_SECONDS * 1000);
std::cout << "Closing stream..." <<std::endl;
stream.stop();
stream.close();
sys.terminate();
std::cout << "Test finished." << std::endl;
}
catch (const portaudio::PaException &e)
{
std::cout << "A PortAudio error occurred: " << e.paErrorText() << std::endl;
}
catch (const portaudio::PaCppException &e)
{
std::cout << "A PortAudioCpp error occurred: " << e.what() << std::endl;
}
catch (const std::exception &e)
{
std::cout << "A generic exception occurred: " << e.what() << std::endl;
}
catch (...)
{
std::cout << "An unknown exception occurred." << std::endl;
}
return 0;
}

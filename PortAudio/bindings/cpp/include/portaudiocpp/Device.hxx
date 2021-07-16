#ifndef INCLUDED_PORTAUDIO_DEVICE_HXX
#define INCLUDED_PORTAUDIO_DEVICE_HXX
#include <iterator>
#include "portaudio.h"
#include "portaudiocpp/SampleDataFormat.hxx"
namespace portaudio
{
class System;
class HostApi;
}
namespace portaudio
{
class Device
{
public:
PaDeviceIndex index() const;
const char *name() const;
int maxInputChannels() const;
int maxOutputChannels() const;
PaTime defaultLowInputLatency() const;
PaTime defaultHighInputLatency() const;
PaTime defaultLowOutputLatency() const;
PaTime defaultHighOutputLatency() const;
double defaultSampleRate() const;
bool isInputOnlyDevice() const;  
bool isOutputOnlyDevice() const;  
bool isFullDuplexDevice() const;  
bool isSystemDefaultInputDevice() const;  
bool isSystemDefaultOutputDevice() const;  
bool isHostApiDefaultInputDevice() const;  
bool isHostApiDefaultOutputDevice() const;  
bool operator==(const Device &rhs) const;
bool operator!=(const Device &rhs) const;
HostApi &hostApi();
const HostApi &hostApi() const;
private:
PaDeviceIndex index_;
const PaDeviceInfo *info_;
private:
friend class System;
explicit Device(PaDeviceIndex index);
~Device();
Device(const Device &);  
Device &operator=(const Device &);  
};
}  
#endif  

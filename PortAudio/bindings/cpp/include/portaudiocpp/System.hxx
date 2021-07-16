#ifndef INCLUDED_PORTAUDIO_SYSTEM_HXX
#define INCLUDED_PORTAUDIO_SYSTEM_HXX
#include "portaudio.h"
namespace portaudio
{
class Device;
class Stream;
class HostApi;
}
namespace portaudio
{
class System
{
public:
class HostApiIterator;  
class DeviceIterator;  
static int version();
static const char *versionText();
static void initialize();
static void terminate();
static System &instance();
static bool exists();
HostApiIterator hostApisBegin();
HostApiIterator hostApisEnd();
HostApi &defaultHostApi();
HostApi &hostApiByTypeId(PaHostApiTypeId type);
HostApi &hostApiByIndex(PaHostApiIndex index);
int hostApiCount();
DeviceIterator devicesBegin();
DeviceIterator devicesEnd();
Device &defaultInputDevice();
Device &defaultOutputDevice();
Device &deviceByIndex(PaDeviceIndex index);
int deviceCount();
static Device &nullDevice();
void sleep(long msec);
int sizeOfSample(PaSampleFormat format);
private:
System();
~System();
static System *instance_;
static int initCount_;
static HostApi **hostApis_;
static Device **devices_;
static Device *nullDevice_;
};
}  
#endif  

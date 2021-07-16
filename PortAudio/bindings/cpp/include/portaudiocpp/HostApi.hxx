#ifndef INCLUDED_PORTAUDIO_HOSTAPI_HXX
#define INCLUDED_PORTAUDIO_HOSTAPI_HXX
#include "portaudio.h"
#include "portaudiocpp/System.hxx"
namespace portaudio
{
class Device;
}
namespace portaudio
{
class HostApi
{
public:
typedef System::DeviceIterator DeviceIterator;
PaHostApiTypeId typeId() const;
PaHostApiIndex index() const;
const char *name() const;
int deviceCount() const;
DeviceIterator devicesBegin();
DeviceIterator devicesEnd();
Device &defaultInputDevice() const;
Device &defaultOutputDevice() const;
bool operator==(const HostApi &rhs) const;
bool operator!=(const HostApi &rhs) const;
private:
const PaHostApiInfo *info_;
Device **devices_;
private:
friend class System;
explicit HostApi(PaHostApiIndex index);
~HostApi();
HostApi(const HostApi &);  
HostApi &operator=(const HostApi &);  
};
}
#endif  

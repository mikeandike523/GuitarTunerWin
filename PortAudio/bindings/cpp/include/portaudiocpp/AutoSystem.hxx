#ifndef INCLUDED_PORTAUDIO_AUTOSYSTEM_HXX
#define INCLUDED_PORTAUDIO_AUTOSYSTEM_HXX
#include "portaudiocpp/System.hxx"
namespace portaudio
{
class AutoSystem
{
public:
AutoSystem(bool initialize = true)
{
if (initialize)
System::initialize();
}
~AutoSystem()
{
if (System::exists())
System::terminate();
}
void initialize()
{
System::initialize();
}
void terminate()
{
System::terminate();
}
};
}  
#endif  

#ifndef INCLUDED_PORTAUDIO_SYSTEMDEVICEITERATOR_HXX
#define INCLUDED_PORTAUDIO_SYSTEMDEVICEITERATOR_HXX
#include <iterator>
#include <cstddef>
#include "portaudiocpp/System.hxx"
namespace portaudio
{
class Device;
class HostApi;
}
namespace portaudio
{
class System::DeviceIterator
{
public:
typedef std::bidirectional_iterator_tag iterator_category;
typedef Device value_type;
typedef ptrdiff_t difference_type;
typedef Device * pointer;
typedef Device & reference;
Device &operator*() const;
Device *operator->() const;
DeviceIterator &operator++();
DeviceIterator operator++(int);
DeviceIterator &operator--();
DeviceIterator operator--(int);
bool operator==(const DeviceIterator &rhs) const;
bool operator!=(const DeviceIterator &rhs) const;
private:
friend class System;
friend class HostApi;
Device **ptr_;
};
}  
#endif  

#ifndef INCLUDED_PORTAUDIO_SYSTEMHOSTAPIITERATOR_HXX
#define INCLUDED_PORTAUDIO_SYSTEMHOSTAPIITERATOR_HXX
#include <iterator>
#include <cstddef>
#include "portaudiocpp/System.hxx"
namespace portaudio
{
class HostApi;
}
namespace portaudio
{
class System::HostApiIterator
{
public:
typedef std::bidirectional_iterator_tag iterator_category;
typedef Device value_type;
typedef ptrdiff_t difference_type;
typedef HostApi * pointer;
typedef HostApi & reference;
HostApi &operator*() const;
HostApi *operator->() const;
HostApiIterator &operator++();
HostApiIterator operator++(int);
HostApiIterator &operator--();
HostApiIterator operator--(int);
bool operator==(const HostApiIterator &rhs) const;
bool operator!=(const HostApiIterator &rhs) const;
private:
friend class System;
HostApi **ptr_;
};
}  
#endif  

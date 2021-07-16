#ifndef INCLUDED_PORTAUDIO_EXCEPTION_HXX
#define INCLUDED_PORTAUDIO_EXCEPTION_HXX
#include <exception>
#include "portaudio.h"
namespace portaudio
{
class Exception : public std::exception
{
public:
virtual ~Exception() throw() {}
virtual const char *what() const throw() = 0;
};
class PaException : public Exception
{
public:
explicit PaException(PaError error);
const char *what() const throw();
PaError paError() const;
const char *paErrorText() const;
bool isHostApiError() const;  
long lastHostApiError() const;
const char *lastHostApiErrorText() const;
bool operator==(const PaException &rhs) const;
bool operator!=(const PaException &rhs) const;
private:
PaError error_;
};
class PaCppException : public Exception
{
public:
enum ExceptionSpecifier
{
UNABLE_TO_ADAPT_DEVICE
};
PaCppException(ExceptionSpecifier specifier);
const char *what() const throw();
ExceptionSpecifier specifier() const;
bool operator==(const PaCppException &rhs) const;
bool operator!=(const PaCppException &rhs) const;
private:
ExceptionSpecifier specifier_;
};
}  
#endif  

#include "portaudiocpp/Exception.hxx"
namespace portaudio
{
PaException::PaException(PaError error) : error_(error)
{
}
const char *PaException::what() const throw()
{
return paErrorText();
}
PaError PaException::paError() const
{
return error_;
}
const char *PaException::paErrorText() const
{
return Pa_GetErrorText(error_);
}
bool PaException::isHostApiError() const
{
return (error_ == paUnanticipatedHostError);
}
long PaException::lastHostApiError() const
{
return Pa_GetLastHostErrorInfo()->errorCode;
}
const char *PaException::lastHostApiErrorText() const
{
return Pa_GetLastHostErrorInfo()->errorText;
}
bool PaException::operator==(const PaException &rhs) const
{
return (error_ == rhs.error_);
}
bool PaException::operator!=(const PaException &rhs) const
{
return !(*this == rhs);
}
PaCppException::PaCppException(ExceptionSpecifier specifier) : specifier_(specifier)
{
}
const char *PaCppException::what() const throw()
{
switch (specifier_)
{
case UNABLE_TO_ADAPT_DEVICE:
{
return "Unable to adapt the given device to the specified host api specific device extension";
}
}
return "Unknown exception";
}
PaCppException::ExceptionSpecifier PaCppException::specifier() const
{
return specifier_;
}
bool PaCppException::operator==(const PaCppException &rhs) const
{
return (specifier_ == rhs.specifier_);
}
bool PaCppException::operator!=(const PaCppException &rhs) const
{
return !(*this == rhs);
}
}  

#ifndef PA_WIN_UTIL_H
#define PA_WIN_UTIL_H
#include "portaudio.h"
#ifdef __cplusplus
extern "C"
{
#endif  
void PaWinUtil_SetLastSystemErrorInfo( PaHostApiTypeId hostApiType, long winError );
#ifdef __cplusplus
}
#endif  
#endif  

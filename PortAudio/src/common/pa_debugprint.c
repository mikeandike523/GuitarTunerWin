 
#include <stdio.h>
#include <stdarg.h>
#include "pa_debugprint.h"
#if defined(_MSC_VER) && defined(PA_ENABLE_MSVC_DEBUG_OUTPUT)
#define WIN32_LEAN_AND_MEAN  
#include "windows.h"
#endif
static PaUtilLogCallback userCB = NULL;
void PaUtil_SetDebugPrintFunction(PaUtilLogCallback cb)
{
userCB = cb;
}
#if _MSC_VER
#define VSNPRINTF  _vsnprintf
#else
#define VSNPRINTF  vsnprintf
#endif
#define PA_LOG_BUF_SIZE 2048
void PaUtil_DebugPrint( const char *format, ... )
{
#if defined(_MSC_VER) && defined(PA_ENABLE_MSVC_DEBUG_OUTPUT)
{
char buf[PA_LOG_BUF_SIZE];
va_list ap;
va_start(ap, format);
VSNPRINTF(buf, sizeof(buf), format, ap);
buf[sizeof(buf)-1] = 0;
OutputDebugStringA(buf);
va_end(ap);
}
#endif
if (userCB != NULL)
{
char strdump[PA_LOG_BUF_SIZE];
va_list ap;
va_start(ap, format);
VSNPRINTF(strdump, sizeof(strdump), format, ap);
strdump[sizeof(strdump)-1] = 0;
userCB(strdump);
va_end(ap);
}
else
{
va_list ap;
va_start(ap, format);
vfprintf(stderr, format, ap);
va_end(ap);
fflush(stderr);
}
}

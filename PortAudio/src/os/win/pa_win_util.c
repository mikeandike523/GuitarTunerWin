 
#include <windows.h>
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#include <sys/timeb.h>  
#else
#include <mmsystem.h>  
#if (defined(WIN32) && (defined(_MSC_VER) && (_MSC_VER >= 1200))) && !defined(_WIN32_WCE)  
#pragma comment( lib, "winmm.lib" )
#endif
#endif
#include "pa_util.h"
#if PA_TRACK_MEMORY
static int numAllocations_ = 0;
#endif
void *PaUtil_AllocateMemory( long size )
{
void *result = GlobalAlloc( GPTR, size );
#if PA_TRACK_MEMORY
if( result != NULL ) numAllocations_ += 1;
#endif
return result;
}
void PaUtil_FreeMemory( void *block )
{
if( block != NULL )
{
GlobalFree( block );
#if PA_TRACK_MEMORY
numAllocations_ -= 1;
#endif
}
}
int PaUtil_CountCurrentlyAllocatedBlocks( void )
{
#if PA_TRACK_MEMORY
return numAllocations_;
#else
return 0;
#endif
}
void Pa_Sleep( long msec )
{
Sleep( msec );
}
static int usePerformanceCounter_;
static double secondsPerTick_;
void PaUtil_InitializeClock( void )
{
LARGE_INTEGER ticksPerSecond;
if( QueryPerformanceFrequency( &ticksPerSecond ) != 0 )
{
usePerformanceCounter_ = 1;
secondsPerTick_ = 1.0 / (double)ticksPerSecond.QuadPart;
}
else
{
usePerformanceCounter_ = 0;
}
}
double PaUtil_GetTime( void )
{
LARGE_INTEGER time;
if( usePerformanceCounter_ )
{
QueryPerformanceCounter( &time );
return time.QuadPart * secondsPerTick_;
}
else
{
#ifndef UNDER_CE
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
return GetTickCount64() * .001;
#else
return timeGetTime() * .001;
#endif
#else
return GetTickCount() * .001;
#endif
}
}
void PaWinUtil_SetLastSystemErrorInfo( PaHostApiTypeId hostApiType, long winError )
{
wchar_t wide_msg[1024];  
FormatMessageW(
FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
NULL,
winError,
MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
wide_msg,
1024,
NULL
);
char msg[1024];
WideCharToMultiByte( CP_UTF8, 0, wide_msg, -1, msg, 1024, NULL, NULL );
PaUtil_SetLastHostErrorInfo( hostApiType, winError, msg );
}

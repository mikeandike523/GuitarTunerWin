 
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(_WIN64)
#if !defined(_MSC_VER)
#include <new>
#include <assert.h>
#define iasiothiscallresolver_sourcefile 1
#include "iasiothiscallresolver.h"
#undef iasiothiscallresolver_sourcefile
#undef ASIOInit
extern IASIO* theAsioDriver;
#if defined(__BCPLUSPLUS__) || defined(__BORLANDC__)          
#define CALL_THISCALL_0( resultName, thisPtr, funcOffset )\
void *this_ = (thisPtr);                                                \
__asm {                                                                 \
mov     ecx, this_            ;                                     \
mov     eax, [ecx]            ;                                     \
call    [eax+funcOffset]      ;                                     \
mov     resultName, eax       ;                                     \
}
#define CALL_VOID_THISCALL_1( thisPtr, funcOffset, param1 )\
void *this_ = (thisPtr);                                                \
__asm {                                                                 \
mov     eax, param1           ;                                     \
push    eax                   ;                                     \
mov     ecx, this_            ;                                     \
mov     eax, [ecx]            ;                                     \
call    [eax+funcOffset]      ;                                     \
}
#define CALL_THISCALL_1( resultName, thisPtr, funcOffset, param1 )\
void *this_ = (thisPtr);                                                \
__asm {                                                                 \
mov     eax, param1           ;                                     \
push    eax                   ;                                     \
mov     ecx, this_            ;                                     \
mov     eax, [ecx]            ;                                     \
call    [eax+funcOffset]      ;                                     \
mov     resultName, eax       ;                                     \
}
#define CALL_THISCALL_1_DOUBLE( resultName, thisPtr, funcOffset, param1 )\
void *this_ = (thisPtr);                                                \
void *doubleParamPtr_ (&param1);                                        \
__asm {                                                                 \
mov     eax, doubleParamPtr_  ;                                     \
push    [eax+4]               ;                                     \
push    [eax]                 ;                                     \
mov     ecx, this_            ;                                     \
mov     eax, [ecx]            ;                                     \
call    [eax+funcOffset]      ;                                     \
mov     resultName, eax       ;                                     \
}
#define CALL_THISCALL_2( resultName, thisPtr, funcOffset, param1, param2 )\
void *this_ = (thisPtr);                                                \
__asm {                                                                 \
mov     eax, param2           ;                                     \
push    eax                   ;                                     \
mov     eax, param1           ;                                     \
push    eax                   ;                                     \
mov     ecx, this_            ;                                     \
mov     eax, [ecx]            ;                                     \
call    [eax+funcOffset]      ;                                     \
mov     resultName, eax       ;                                     \
}
#define CALL_THISCALL_4( resultName, thisPtr, funcOffset, param1, param2, param3, param4 )\
void *this_ = (thisPtr);                                                \
__asm {                                                                 \
mov     eax, param4           ;                                     \
push    eax                   ;                                     \
mov     eax, param3           ;                                     \
push    eax                   ;                                     \
mov     eax, param2           ;                                     \
push    eax                   ;                                     \
mov     eax, param1           ;                                     \
push    eax                   ;                                     \
mov     ecx, this_            ;                                     \
mov     eax, [ecx]            ;                                     \
call    [eax+funcOffset]      ;                                     \
mov     resultName, eax       ;                                     \
}
#elif defined(__GNUC__)
#define CALL_THISCALL_0( resultName, thisPtr, funcOffset )                  \
__asm__ __volatile__ ("movl (%1), %%edx\n\t"                            \
"call *"#funcOffset"(%%edx)\n\t"                  \
:"=a"(resultName)             \
:"c"(thisPtr)                  \
: "%edx"                  \
);                                                 \
#define CALL_VOID_THISCALL_1( thisPtr, funcOffset, param1 )                 \
__asm__ __volatile__ ("pushl %0\n\t"                                    \
"movl (%1), %%edx\n\t"                            \
"call *"#funcOffset"(%%edx)\n\t"                  \
:                             \
:"r"(param1),                  \
"c"(thisPtr)                                     \
: "%edx"                  \
);                                                 \
#define CALL_THISCALL_1( resultName, thisPtr, funcOffset, param1 )          \
__asm__ __volatile__ ("pushl %1\n\t"                                    \
"movl (%2), %%edx\n\t"                            \
"call *"#funcOffset"(%%edx)\n\t"                  \
:"=a"(resultName)             \
:"r"(param1),                  \
"c"(thisPtr)                                     \
: "%edx"                  \
);                                                \
#define CALL_THISCALL_1_DOUBLE( resultName, thisPtr, funcOffset, param1 )   \
do {                                                                    \
double param1f64 = param1;                \
double *param1f64Ptr = &param1f64;          \
__asm__ __volatile__ ("pushl 4(%1)\n\t"                                \
"pushl (%1)\n\t"                                 \
"movl (%2), %%edx\n\t"                           \
"call *"#funcOffset"(%%edx);\n\t"                \
: "=a"(resultName)           \
: "r"(param1f64Ptr),          \
"c"(thisPtr),                                    \
"m"(*param1f64Ptr)             \
: "%edx"                 \
);                                               \
} while (0);                                                            \
#define CALL_THISCALL_2( resultName, thisPtr, funcOffset, param1, param2 )  \
__asm__ __volatile__ ("pushl %1\n\t"                                    \
"pushl %2\n\t"                                    \
"movl (%3), %%edx\n\t"                            \
"call *"#funcOffset"(%%edx)\n\t"                  \
:"=a"(resultName)             \
:"r"(param2),                  \
"r"(param1),                                     \
"c"(thisPtr)                                     \
: "%edx"                  \
);                                                \
#define CALL_THISCALL_4( resultName, thisPtr, funcOffset, param1, param2, param3, param4 )\
__asm__ __volatile__ ("pushl %1\n\t"                                    \
"pushl %2\n\t"                                    \
"pushl %3\n\t"                                    \
"pushl %4\n\t"                                    \
"movl (%5), %%edx\n\t"                            \
"call *"#funcOffset"(%%edx)\n\t"                  \
:"=a"(resultName)             \
:"r"(param4),                 \
"r"(param3),                                     \
"r"(param2),                                     \
"r"(param1),                                     \
"c"(thisPtr)                                     \
: "%edx"                  \
);                                                \
#endif
IASIOThiscallResolver IASIOThiscallResolver::instance;
IASIOThiscallResolver::IASIOThiscallResolver()
{
}
IASIOThiscallResolver::IASIOThiscallResolver(IASIO* that)
: that_( that )
{
}
HRESULT STDMETHODCALLTYPE IASIOThiscallResolver::QueryInterface(REFIID riid, void **ppv)
{
(void)riid;      
assert( false );  
*ppv = NULL;
return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE IASIOThiscallResolver::AddRef()
{
assert( false );  
return 1;
}
ULONG STDMETHODCALLTYPE IASIOThiscallResolver::Release()
{
assert( false );  
return 1;
}
ASIOBool IASIOThiscallResolver::init(void *sysHandle)
{
ASIOBool result;
CALL_THISCALL_1( result, that_, 12, sysHandle );
return result;
}
void IASIOThiscallResolver::getDriverName(char *name)
{
CALL_VOID_THISCALL_1( that_, 16, name );
}
long IASIOThiscallResolver::getDriverVersion()
{
ASIOBool result;
CALL_THISCALL_0( result, that_, 20 );
return result;
}
void IASIOThiscallResolver::getErrorMessage(char *string)
{
CALL_VOID_THISCALL_1( that_, 24, string );
}
ASIOError IASIOThiscallResolver::start()
{
ASIOBool result;
CALL_THISCALL_0( result, that_, 28 );
return result;
}
ASIOError IASIOThiscallResolver::stop()
{
ASIOBool result;
CALL_THISCALL_0( result, that_, 32 );
return result;
}
ASIOError IASIOThiscallResolver::getChannels(long *numInputChannels, long *numOutputChannels)
{
ASIOBool result;
CALL_THISCALL_2( result, that_, 36, numInputChannels, numOutputChannels );
return result;
}
ASIOError IASIOThiscallResolver::getLatencies(long *inputLatency, long *outputLatency)
{
ASIOBool result;
CALL_THISCALL_2( result, that_, 40, inputLatency, outputLatency );
return result;
}
ASIOError IASIOThiscallResolver::getBufferSize(long *minSize, long *maxSize,
long *preferredSize, long *granularity)
{
ASIOBool result;
CALL_THISCALL_4( result, that_, 44, minSize, maxSize, preferredSize, granularity );
return result;
}
ASIOError IASIOThiscallResolver::canSampleRate(ASIOSampleRate sampleRate)
{
ASIOBool result;
CALL_THISCALL_1_DOUBLE( result, that_, 48, sampleRate );
return result;
}
ASIOError IASIOThiscallResolver::getSampleRate(ASIOSampleRate *sampleRate)
{
ASIOBool result;
CALL_THISCALL_1( result, that_, 52, sampleRate );
return result;
}
ASIOError IASIOThiscallResolver::setSampleRate(ASIOSampleRate sampleRate)
{    
ASIOBool result;
CALL_THISCALL_1_DOUBLE( result, that_, 56, sampleRate );
return result;
}
ASIOError IASIOThiscallResolver::getClockSources(ASIOClockSource *clocks, long *numSources)
{
ASIOBool result;
CALL_THISCALL_2( result, that_, 60, clocks, numSources );
return result;
}
ASIOError IASIOThiscallResolver::setClockSource(long reference)
{
ASIOBool result;
CALL_THISCALL_1( result, that_, 64, reference );
return result;
}
ASIOError IASIOThiscallResolver::getSamplePosition(ASIOSamples *sPos, ASIOTimeStamp *tStamp)
{
ASIOBool result;
CALL_THISCALL_2( result, that_, 68, sPos, tStamp );
return result;
}
ASIOError IASIOThiscallResolver::getChannelInfo(ASIOChannelInfo *info)
{
ASIOBool result;
CALL_THISCALL_1( result, that_, 72, info );
return result;
}
ASIOError IASIOThiscallResolver::createBuffers(ASIOBufferInfo *bufferInfos,
long numChannels, long bufferSize, ASIOCallbacks *callbacks)
{
ASIOBool result;
CALL_THISCALL_4( result, that_, 76, bufferInfos, numChannels, bufferSize, callbacks );
return result;
}
ASIOError IASIOThiscallResolver::disposeBuffers()
{
ASIOBool result;
CALL_THISCALL_0( result, that_, 80 );
return result;
}
ASIOError IASIOThiscallResolver::controlPanel()
{
ASIOBool result;
CALL_THISCALL_0( result, that_, 84 );
return result;
}
ASIOError IASIOThiscallResolver::future(long selector,void *opt)
{
ASIOBool result;
CALL_THISCALL_2( result, that_, 88, selector, opt );
return result;
}
ASIOError IASIOThiscallResolver::outputReady()
{
ASIOBool result;
CALL_THISCALL_0( result, that_, 92 );
return result;
}
ASIOError IASIOThiscallResolver::ASIOInit(ASIODriverInfo *info)
{
new((void*)&instance) IASIOThiscallResolver( theAsioDriver );
theAsioDriver = &instance;
	return ::ASIOInit(info);
}
#endif  
#endif  

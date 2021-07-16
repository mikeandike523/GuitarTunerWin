 
#include <windows.h>
#include <objbase.h>
#include "portaudio.h"
#include "pa_util.h"
#include "pa_debugprint.h"
#include "pa_win_coinitialize.h"
#include "pa_win_util.h"
#if (defined(WIN32) && (defined(_MSC_VER) && (_MSC_VER >= 1200))) && !defined(_WIN32_WCE) && !(defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP))  
#pragma comment( lib, "ole32.lib" )
#endif
#define PAWINUTIL_COM_INITIALIZED       (0xb38f)
#define PAWINUTIL_COM_NOT_INITIALIZED   (0xf1cd)
PaError PaWinUtil_CoInitialize( PaHostApiTypeId hostApiType, PaWinUtilComInitializationResult *comInitializationResult )
{
HRESULT hr;
comInitializationResult->state = PAWINUTIL_COM_NOT_INITIALIZED;
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY != WINAPI_FAMILY_APP)
hr = CoInitialize(0);  
#else
hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#endif
if( FAILED(hr) && hr != RPC_E_CHANGED_MODE )
{
PA_DEBUG(("CoInitialize(0) failed. hr=%d\n", hr));
if( hr == E_OUTOFMEMORY )
return paInsufficientMemory;
PaWinUtil_SetLastSystemErrorInfo( hostApiType, hr );
return paUnanticipatedHostError;
}
if( hr != RPC_E_CHANGED_MODE )
{
comInitializationResult->state = PAWINUTIL_COM_INITIALIZED;
comInitializationResult->initializingThreadId = GetCurrentThreadId();
}
return paNoError;
}
void PaWinUtil_CoUninitialize( PaHostApiTypeId hostApiType, PaWinUtilComInitializationResult *comInitializationResult )
{
if( comInitializationResult->state != PAWINUTIL_COM_NOT_INITIALIZED
&& comInitializationResult->state != PAWINUTIL_COM_INITIALIZED ){
PA_DEBUG(("ERROR: PaWinUtil_CoUninitialize called without calling PaWinUtil_CoInitialize\n"));
}
if( comInitializationResult->state == PAWINUTIL_COM_INITIALIZED )
{
DWORD currentThreadId = GetCurrentThreadId();
if( comInitializationResult->initializingThreadId != currentThreadId )
{
PA_DEBUG(("ERROR: failed PaWinUtil_CoUninitialize calling thread[%lu] does not match initializing thread[%lu]\n",
currentThreadId, comInitializationResult->initializingThreadId));
}
else
{
CoUninitialize();
comInitializationResult->state = PAWINUTIL_COM_NOT_INITIALIZED;
}
}
}

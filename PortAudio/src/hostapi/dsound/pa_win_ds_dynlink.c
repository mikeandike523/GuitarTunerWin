 
#include "pa_win_ds_dynlink.h"
#include "pa_debugprint.h"
PaWinDsDSoundEntryPoints paWinDsDSoundEntryPoints = { 0, 0, 0, 0, 0, 0, 0 };
static HRESULT WINAPI DummyDllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
(void)rclsid;  
(void)riid;  
(void)ppv;  
return CLASS_E_CLASSNOTAVAILABLE;
}
static HRESULT WINAPI DummyDirectSoundCreate(LPGUID lpcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
(void)lpcGuidDevice;  
(void)ppDS;  
(void)pUnkOuter;  
return E_NOTIMPL;
}
static HRESULT WINAPI DummyDirectSoundEnumerateW(LPDSENUMCALLBACKW lpDSEnumCallback, LPVOID lpContext)
{
(void)lpDSEnumCallback;  
(void)lpContext;  
return E_NOTIMPL;
}
static HRESULT WINAPI DummyDirectSoundEnumerateA(LPDSENUMCALLBACKA lpDSEnumCallback, LPVOID lpContext)
{
(void)lpDSEnumCallback;  
(void)lpContext;  
return E_NOTIMPL;
}
static HRESULT WINAPI DummyDirectSoundCaptureCreate(LPGUID lpcGUID, LPDIRECTSOUNDCAPTURE *lplpDSC, LPUNKNOWN pUnkOuter)
{
(void)lpcGUID;  
(void)lplpDSC;  
(void)pUnkOuter;  
return E_NOTIMPL;
}
static HRESULT WINAPI DummyDirectSoundCaptureEnumerateW(LPDSENUMCALLBACKW lpDSCEnumCallback, LPVOID lpContext)
{
(void)lpDSCEnumCallback;  
(void)lpContext;  
return E_NOTIMPL;
}
static HRESULT WINAPI DummyDirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA lpDSCEnumCallback, LPVOID lpContext)
{
(void)lpDSCEnumCallback;  
(void)lpContext;  
return E_NOTIMPL;
}
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
static HRESULT WINAPI DummyDirectSoundFullDuplexCreate8(
LPCGUID pcGuidCaptureDevice,
LPCGUID pcGuidRenderDevice,
LPCDSCBUFFERDESC pcDSCBufferDesc,
LPCDSBUFFERDESC pcDSBufferDesc,
HWND hWnd,
DWORD dwLevel,
LPDIRECTSOUNDFULLDUPLEX * ppDSFD,
LPDIRECTSOUNDCAPTUREBUFFER8 * ppDSCBuffer8,
LPDIRECTSOUNDBUFFER8 * ppDSBuffer8,
LPUNKNOWN pUnkOuter)
{
(void)pcGuidCaptureDevice;  
(void)pcGuidRenderDevice;  
(void)pcDSCBufferDesc;  
(void)pcDSBufferDesc;  
(void)hWnd;  
(void)dwLevel;  
(void)ppDSFD;  
(void)ppDSCBuffer8;  
(void)ppDSBuffer8;  
(void)pUnkOuter;  
return E_NOTIMPL;
}
#endif  
void PaWinDs_InitializeDSoundEntryPoints(void)
{
paWinDsDSoundEntryPoints.hInstance_ = LoadLibraryA("dsound.dll");
if( paWinDsDSoundEntryPoints.hInstance_ != NULL )
{
paWinDsDSoundEntryPoints.DllGetClassObject =
(HRESULT (WINAPI *)(REFCLSID, REFIID , LPVOID *))
GetProcAddress( paWinDsDSoundEntryPoints.hInstance_, "DllGetClassObject" );
if( paWinDsDSoundEntryPoints.DllGetClassObject == NULL )
paWinDsDSoundEntryPoints.DllGetClassObject = DummyDllGetClassObject;
paWinDsDSoundEntryPoints.DirectSoundCreate =
(HRESULT (WINAPI *)(LPGUID, LPDIRECTSOUND *, LPUNKNOWN))
GetProcAddress( paWinDsDSoundEntryPoints.hInstance_, "DirectSoundCreate" );
if( paWinDsDSoundEntryPoints.DirectSoundCreate == NULL )
paWinDsDSoundEntryPoints.DirectSoundCreate = DummyDirectSoundCreate;
paWinDsDSoundEntryPoints.DirectSoundEnumerateW =
(HRESULT (WINAPI *)(LPDSENUMCALLBACKW, LPVOID))
GetProcAddress( paWinDsDSoundEntryPoints.hInstance_, "DirectSoundEnumerateW" );
if( paWinDsDSoundEntryPoints.DirectSoundEnumerateW == NULL )
paWinDsDSoundEntryPoints.DirectSoundEnumerateW = DummyDirectSoundEnumerateW;
paWinDsDSoundEntryPoints.DirectSoundEnumerateA =
(HRESULT (WINAPI *)(LPDSENUMCALLBACKA, LPVOID))
GetProcAddress( paWinDsDSoundEntryPoints.hInstance_, "DirectSoundEnumerateA" );
if( paWinDsDSoundEntryPoints.DirectSoundEnumerateA == NULL )
paWinDsDSoundEntryPoints.DirectSoundEnumerateA = DummyDirectSoundEnumerateA;
paWinDsDSoundEntryPoints.DirectSoundCaptureCreate =
(HRESULT (WINAPI *)(LPGUID, LPDIRECTSOUNDCAPTURE *, LPUNKNOWN))
GetProcAddress( paWinDsDSoundEntryPoints.hInstance_, "DirectSoundCaptureCreate" );
if( paWinDsDSoundEntryPoints.DirectSoundCaptureCreate == NULL )
paWinDsDSoundEntryPoints.DirectSoundCaptureCreate = DummyDirectSoundCaptureCreate;
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateW =
(HRESULT (WINAPI *)(LPDSENUMCALLBACKW, LPVOID))
GetProcAddress( paWinDsDSoundEntryPoints.hInstance_, "DirectSoundCaptureEnumerateW" );
if( paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateW == NULL )
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateW = DummyDirectSoundCaptureEnumerateW;
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateA =
(HRESULT (WINAPI *)(LPDSENUMCALLBACKA, LPVOID))
GetProcAddress( paWinDsDSoundEntryPoints.hInstance_, "DirectSoundCaptureEnumerateA" );
if( paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateA == NULL )
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateA = DummyDirectSoundCaptureEnumerateA;
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
paWinDsDSoundEntryPoints.DirectSoundFullDuplexCreate8 =
(HRESULT (WINAPI *)(LPCGUID, LPCGUID, LPCDSCBUFFERDESC, LPCDSBUFFERDESC,
HWND, DWORD, LPDIRECTSOUNDFULLDUPLEX *, LPDIRECTSOUNDCAPTUREBUFFER8 *,
LPDIRECTSOUNDBUFFER8 *, LPUNKNOWN))
GetProcAddress( paWinDsDSoundEntryPoints.hInstance_, "DirectSoundFullDuplexCreate" );
if( paWinDsDSoundEntryPoints.DirectSoundFullDuplexCreate8 == NULL )
paWinDsDSoundEntryPoints.DirectSoundFullDuplexCreate8 = DummyDirectSoundFullDuplexCreate8;
#endif
}
else
{
DWORD errorCode = GetLastError();  
PA_DEBUG(("Couldn't load dsound.dll error code: %d \n",errorCode));
paWinDsDSoundEntryPoints.DirectSoundCreate = DummyDirectSoundCreate;
paWinDsDSoundEntryPoints.DirectSoundEnumerateW = DummyDirectSoundEnumerateW;
paWinDsDSoundEntryPoints.DirectSoundEnumerateA = DummyDirectSoundEnumerateA;
paWinDsDSoundEntryPoints.DirectSoundCaptureCreate = DummyDirectSoundCaptureCreate;
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateW = DummyDirectSoundCaptureEnumerateW;
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateA = DummyDirectSoundCaptureEnumerateA;
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
paWinDsDSoundEntryPoints.DirectSoundFullDuplexCreate8 = DummyDirectSoundFullDuplexCreate8;
#endif
}
}
void PaWinDs_TerminateDSoundEntryPoints(void)
{
if( paWinDsDSoundEntryPoints.hInstance_ != NULL )
{
paWinDsDSoundEntryPoints.DirectSoundCreate = 0;
paWinDsDSoundEntryPoints.DirectSoundEnumerateW = 0;
paWinDsDSoundEntryPoints.DirectSoundEnumerateA = 0;
paWinDsDSoundEntryPoints.DirectSoundCaptureCreate = 0;
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateW = 0;
paWinDsDSoundEntryPoints.DirectSoundCaptureEnumerateA = 0;
FreeLibrary( paWinDsDSoundEntryPoints.hInstance_ );
paWinDsDSoundEntryPoints.hInstance_ = NULL;
}
}

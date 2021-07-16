 
#ifndef INCLUDED_PA_DSOUND_DYNLINK_H
#define INCLUDED_PA_DSOUND_DYNLINK_H
#ifdef __BORLANDC__
#if !defined(WIN32)
#define WIN32
#endif
#endif
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
#define DIRECTSOUND_VERSION 0x0800
#else
#define DIRECTSOUND_VERSION 0x0300
#endif
#include <dsound.h>
#ifdef __cplusplus
extern "C"
{
#endif  
typedef struct
{
HINSTANCE hInstance_;
HRESULT (WINAPI *DllGetClassObject)(REFCLSID , REFIID , LPVOID *);
HRESULT (WINAPI *DirectSoundCreate)(LPGUID, LPDIRECTSOUND *, LPUNKNOWN);
HRESULT (WINAPI *DirectSoundEnumerateW)(LPDSENUMCALLBACKW, LPVOID);
HRESULT (WINAPI *DirectSoundEnumerateA)(LPDSENUMCALLBACKA, LPVOID);
HRESULT (WINAPI *DirectSoundCaptureCreate)(LPGUID, LPDIRECTSOUNDCAPTURE *, LPUNKNOWN);
HRESULT (WINAPI *DirectSoundCaptureEnumerateW)(LPDSENUMCALLBACKW, LPVOID);
HRESULT (WINAPI *DirectSoundCaptureEnumerateA)(LPDSENUMCALLBACKA, LPVOID);
#ifdef PAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE
HRESULT (WINAPI *DirectSoundFullDuplexCreate8)(
LPCGUID, LPCGUID, LPCDSCBUFFERDESC, LPCDSBUFFERDESC,
HWND, DWORD, LPDIRECTSOUNDFULLDUPLEX *, LPDIRECTSOUNDCAPTUREBUFFER8 *,
LPDIRECTSOUNDBUFFER8 *, LPUNKNOWN );
#endif
}PaWinDsDSoundEntryPoints;
extern PaWinDsDSoundEntryPoints paWinDsDSoundEntryPoints;
void PaWinDs_InitializeDSoundEntryPoints(void);
void PaWinDs_TerminateDSoundEntryPoints(void);
#ifdef __cplusplus
}
#endif  
#endif  

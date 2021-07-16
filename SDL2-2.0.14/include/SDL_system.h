 
#ifndef SDL_system_h_
#define SDL_system_h_
#include "SDL_stdinc.h"
#include "SDL_keyboard.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __WIN32__
	
typedef void (SDLCALL * SDL_WindowsMessageHook)(void *userdata, void *hWnd, unsigned int message, Uint64 wParam, Sint64 lParam);
extern DECLSPEC void SDLCALL SDL_SetWindowsMessageHook(SDL_WindowsMessageHook callback, void *userdata);
extern DECLSPEC int SDLCALL SDL_Direct3D9GetAdapterIndex( int displayIndex );
typedef struct IDirect3DDevice9 IDirect3DDevice9;
extern DECLSPEC IDirect3DDevice9* SDLCALL SDL_RenderGetD3D9Device(SDL_Renderer * renderer);
extern DECLSPEC SDL_bool SDLCALL SDL_DXGIGetOutputInfo( int displayIndex, int *adapterIndex, int *outputIndex );
#endif  
#ifdef __LINUX__
extern DECLSPEC int SDLCALL SDL_LinuxSetThreadPriority(Sint64 threadID, int priority);
#endif  
	
#ifdef __IPHONEOS__
#define SDL_iOSSetAnimationCallback(window, interval, callback, callbackParam) SDL_iPhoneSetAnimationCallback(window, interval, callback, callbackParam)
extern DECLSPEC int SDLCALL SDL_iPhoneSetAnimationCallback(SDL_Window * window, int interval, void (*callback)(void*), void *callbackParam);
#define SDL_iOSSetEventPump(enabled) SDL_iPhoneSetEventPump(enabled)
extern DECLSPEC void SDLCALL SDL_iPhoneSetEventPump(SDL_bool enabled);
#endif  
#ifdef __ANDROID__
extern DECLSPEC void * SDLCALL SDL_AndroidGetJNIEnv(void);
extern DECLSPEC void * SDLCALL SDL_AndroidGetActivity(void);
extern DECLSPEC int SDLCALL SDL_GetAndroidSDKVersion(void);
extern DECLSPEC SDL_bool SDLCALL SDL_IsAndroidTV(void);
extern DECLSPEC SDL_bool SDLCALL SDL_IsChromebook(void);
extern DECLSPEC SDL_bool SDLCALL SDL_IsDeXMode(void);
extern DECLSPEC void SDLCALL SDL_AndroidBackButton(void);
#define SDL_ANDROID_EXTERNAL_STORAGE_READ   0x01
#define SDL_ANDROID_EXTERNAL_STORAGE_WRITE  0x02
extern DECLSPEC const char * SDLCALL SDL_AndroidGetInternalStoragePath(void);
extern DECLSPEC int SDLCALL SDL_AndroidGetExternalStorageState(void);
extern DECLSPEC const char * SDLCALL SDL_AndroidGetExternalStoragePath(void);
extern DECLSPEC SDL_bool SDLCALL SDL_AndroidRequestPermission(const char *permission);
#endif  
#ifdef __WINRT__
typedef enum
{
SDL_WINRT_PATH_INSTALLED_LOCATION,
SDL_WINRT_PATH_LOCAL_FOLDER,
SDL_WINRT_PATH_ROAMING_FOLDER,
SDL_WINRT_PATH_TEMP_FOLDER
} SDL_WinRT_Path;
typedef enum
{
SDL_WINRT_DEVICEFAMILY_UNKNOWN,
SDL_WINRT_DEVICEFAMILY_DESKTOP,
SDL_WINRT_DEVICEFAMILY_MOBILE,
SDL_WINRT_DEVICEFAMILY_XBOX,
} SDL_WinRT_DeviceFamily;
extern DECLSPEC const wchar_t * SDLCALL SDL_WinRTGetFSPathUNICODE(SDL_WinRT_Path pathType);
extern DECLSPEC const char * SDLCALL SDL_WinRTGetFSPathUTF8(SDL_WinRT_Path pathType);
extern DECLSPEC SDL_WinRT_DeviceFamily SDLCALL SDL_WinRTGetDeviceFamily();
#endif  
extern DECLSPEC SDL_bool SDLCALL SDL_IsTablet(void);
extern DECLSPEC void SDLCALL SDL_OnApplicationWillTerminate(void);
extern DECLSPEC void SDLCALL SDL_OnApplicationDidReceiveMemoryWarning(void);
extern DECLSPEC void SDLCALL SDL_OnApplicationWillResignActive(void);
extern DECLSPEC void SDLCALL SDL_OnApplicationDidEnterBackground(void);
extern DECLSPEC void SDLCALL SDL_OnApplicationWillEnterForeground(void);
extern DECLSPEC void SDLCALL SDL_OnApplicationDidBecomeActive(void);
#ifdef __IPHONEOS__
extern DECLSPEC void SDLCALL SDL_OnApplicationDidChangeStatusBarOrientation(void);
#endif
#ifdef __cplusplus
}
#endif
#include "close_code.h"
#endif  

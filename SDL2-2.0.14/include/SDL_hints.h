 
#ifndef SDL_hints_h_
#define SDL_hints_h_
#include "SDL_stdinc.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif
#define SDL_HINT_FRAMEBUFFER_ACCELERATION   "SDL_FRAMEBUFFER_ACCELERATION"
#define SDL_HINT_RENDER_DRIVER              "SDL_RENDER_DRIVER"
#define SDL_HINT_RENDER_OPENGL_SHADERS      "SDL_RENDER_OPENGL_SHADERS"
#define SDL_HINT_RENDER_DIRECT3D_THREADSAFE "SDL_RENDER_DIRECT3D_THREADSAFE"
#define SDL_HINT_RENDER_DIRECT3D11_DEBUG    "SDL_RENDER_DIRECT3D11_DEBUG"
#define SDL_HINT_RENDER_LOGICAL_SIZE_MODE       "SDL_RENDER_LOGICAL_SIZE_MODE"
#define SDL_HINT_RENDER_SCALE_QUALITY       "SDL_RENDER_SCALE_QUALITY"
#define SDL_HINT_RENDER_VSYNC               "SDL_RENDER_VSYNC"
#define SDL_HINT_VIDEO_ALLOW_SCREENSAVER    "SDL_VIDEO_ALLOW_SCREENSAVER"
#define SDL_HINT_VIDEO_EXTERNAL_CONTEXT    "SDL_VIDEO_EXTERNAL_CONTEXT"
#define SDL_HINT_VIDEO_X11_XVIDMODE         "SDL_VIDEO_X11_XVIDMODE"
#define SDL_HINT_VIDEO_X11_XINERAMA         "SDL_VIDEO_X11_XINERAMA"
#define SDL_HINT_VIDEO_X11_XRANDR           "SDL_VIDEO_X11_XRANDR"
#define SDL_HINT_VIDEO_X11_WINDOW_VISUALID      "SDL_VIDEO_X11_WINDOW_VISUALID"
#define SDL_HINT_VIDEO_X11_NET_WM_PING      "SDL_VIDEO_X11_NET_WM_PING"
#define SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR "SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR"
#define SDL_HINT_VIDEO_X11_FORCE_EGL "SDL_VIDEO_X11_FORCE_EGL"
#define SDL_HINT_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN    "SDL_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN"
#define SDL_HINT_WINDOWS_INTRESOURCE_ICON       "SDL_WINDOWS_INTRESOURCE_ICON"
#define SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL "SDL_WINDOWS_INTRESOURCE_ICON_SMALL"
#define SDL_HINT_WINDOWS_ENABLE_MESSAGELOOP "SDL_WINDOWS_ENABLE_MESSAGELOOP"
#define SDL_HINT_GRAB_KEYBOARD              "SDL_GRAB_KEYBOARD"
#define SDL_HINT_MOUSE_DOUBLE_CLICK_TIME    "SDL_MOUSE_DOUBLE_CLICK_TIME"
#define SDL_HINT_MOUSE_DOUBLE_CLICK_RADIUS    "SDL_MOUSE_DOUBLE_CLICK_RADIUS"
#define SDL_HINT_MOUSE_NORMAL_SPEED_SCALE    "SDL_MOUSE_NORMAL_SPEED_SCALE"
#define SDL_HINT_MOUSE_RELATIVE_SPEED_SCALE    "SDL_MOUSE_RELATIVE_SPEED_SCALE"
#define SDL_HINT_MOUSE_RELATIVE_SCALING "SDL_MOUSE_RELATIVE_SCALING"
#define SDL_HINT_MOUSE_RELATIVE_MODE_WARP    "SDL_MOUSE_RELATIVE_MODE_WARP"
#define SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH "SDL_MOUSE_FOCUS_CLICKTHROUGH"
#define SDL_HINT_TOUCH_MOUSE_EVENTS    "SDL_TOUCH_MOUSE_EVENTS"
#define SDL_HINT_MOUSE_TOUCH_EVENTS    "SDL_MOUSE_TOUCH_EVENTS"
#define SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS   "SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS"
#define SDL_HINT_IDLE_TIMER_DISABLED "SDL_IOS_IDLE_TIMER_DISABLED"
#define SDL_HINT_ORIENTATIONS "SDL_IOS_ORIENTATIONS"
#define SDL_HINT_APPLE_TV_CONTROLLER_UI_EVENTS "SDL_APPLE_TV_CONTROLLER_UI_EVENTS"
#define SDL_HINT_APPLE_TV_REMOTE_ALLOW_ROTATION "SDL_APPLE_TV_REMOTE_ALLOW_ROTATION"
#define SDL_HINT_IOS_HIDE_HOME_INDICATOR "SDL_IOS_HIDE_HOME_INDICATOR"
#define SDL_HINT_ACCELEROMETER_AS_JOYSTICK "SDL_ACCELEROMETER_AS_JOYSTICK"
#define SDL_HINT_TV_REMOTE_AS_JOYSTICK "SDL_TV_REMOTE_AS_JOYSTICK"
#define SDL_HINT_XINPUT_ENABLED "SDL_XINPUT_ENABLED"
#define SDL_HINT_XINPUT_USE_OLD_JOYSTICK_MAPPING "SDL_XINPUT_USE_OLD_JOYSTICK_MAPPING"
#define SDL_HINT_GAMECONTROLLERTYPE "SDL_GAMECONTROLLERTYPE"
#define SDL_HINT_GAMECONTROLLERCONFIG "SDL_GAMECONTROLLERCONFIG"
#define SDL_HINT_GAMECONTROLLERCONFIG_FILE "SDL_GAMECONTROLLERCONFIG_FILE"
#define SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES "SDL_GAMECONTROLLER_IGNORE_DEVICES"
#define SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES_EXCEPT "SDL_GAMECONTROLLER_IGNORE_DEVICES_EXCEPT"
#define SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS "SDL_GAMECONTROLLER_USE_BUTTON_LABELS"
#define SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS "SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"
#define SDL_HINT_JOYSTICK_HIDAPI "SDL_JOYSTICK_HIDAPI"
#define SDL_HINT_JOYSTICK_HIDAPI_PS4 "SDL_JOYSTICK_HIDAPI_PS4"
#define SDL_HINT_JOYSTICK_HIDAPI_PS5 "SDL_JOYSTICK_HIDAPI_PS5"
#define SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE "SDL_JOYSTICK_HIDAPI_PS4_RUMBLE"
#define SDL_HINT_JOYSTICK_HIDAPI_STEAM "SDL_JOYSTICK_HIDAPI_STEAM"
#define SDL_HINT_JOYSTICK_HIDAPI_SWITCH "SDL_JOYSTICK_HIDAPI_SWITCH"
#define SDL_HINT_JOYSTICK_HIDAPI_XBOX   "SDL_JOYSTICK_HIDAPI_XBOX"
#define SDL_HINT_JOYSTICK_HIDAPI_CORRELATE_XINPUT   "SDL_JOYSTICK_HIDAPI_CORRELATE_XINPUT"
#define SDL_HINT_JOYSTICK_HIDAPI_GAMECUBE "SDL_JOYSTICK_HIDAPI_GAMECUBE"
#define SDL_HINT_ENABLE_STEAM_CONTROLLERS "SDL_ENABLE_STEAM_CONTROLLERS"
#define SDL_HINT_JOYSTICK_RAWINPUT "SDL_JOYSTICK_RAWINPUT"
#define SDL_HINT_JOYSTICK_THREAD "SDL_JOYSTICK_THREAD"
#define SDL_HINT_LINUX_JOYSTICK_DEADZONES "SDL_LINUX_JOYSTICK_DEADZONES"
#define SDL_HINT_ALLOW_TOPMOST "SDL_ALLOW_TOPMOST"
#define SDL_HINT_TIMER_RESOLUTION "SDL_TIMER_RESOLUTION"
#define SDL_HINT_QTWAYLAND_CONTENT_ORIENTATION "SDL_QTWAYLAND_CONTENT_ORIENTATION"
#define SDL_HINT_QTWAYLAND_WINDOW_FLAGS "SDL_QTWAYLAND_WINDOW_FLAGS"
#define SDL_HINT_THREAD_STACK_SIZE              "SDL_THREAD_STACK_SIZE"
#define SDL_HINT_THREAD_PRIORITY_POLICY         "SDL_THREAD_PRIORITY_POLICY"
#define SDL_HINT_THREAD_FORCE_REALTIME_TIME_CRITICAL "SDL_THREAD_FORCE_REALTIME_TIME_CRITICAL"
#define SDL_HINT_VIDEO_HIGHDPI_DISABLED "SDL_VIDEO_HIGHDPI_DISABLED"
#define SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK "SDL_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK"
#define SDL_HINT_VIDEO_WIN_D3DCOMPILER              "SDL_VIDEO_WIN_D3DCOMPILER"
#define SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT    "SDL_VIDEO_WINDOW_SHARE_PIXEL_FORMAT"
#define SDL_HINT_WINRT_PRIVACY_POLICY_URL "SDL_WINRT_PRIVACY_POLICY_URL"
#define SDL_HINT_WINRT_PRIVACY_POLICY_LABEL "SDL_WINRT_PRIVACY_POLICY_LABEL"
#define SDL_HINT_WINRT_HANDLE_BACK_BUTTON "SDL_WINRT_HANDLE_BACK_BUTTON"
#define SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES    "SDL_VIDEO_MAC_FULLSCREEN_SPACES"
#define SDL_HINT_MAC_BACKGROUND_APP    "SDL_MAC_BACKGROUND_APP"
#define SDL_HINT_ANDROID_APK_EXPANSION_MAIN_FILE_VERSION "SDL_ANDROID_APK_EXPANSION_MAIN_FILE_VERSION"
#define SDL_HINT_ANDROID_APK_EXPANSION_PATCH_FILE_VERSION "SDL_ANDROID_APK_EXPANSION_PATCH_FILE_VERSION"
#define SDL_HINT_IME_INTERNAL_EDITING "SDL_IME_INTERNAL_EDITING"
#define SDL_HINT_ANDROID_TRAP_BACK_BUTTON "SDL_ANDROID_TRAP_BACK_BUTTON"
#define SDL_HINT_ANDROID_BLOCK_ON_PAUSE "SDL_ANDROID_BLOCK_ON_PAUSE"
#define SDL_HINT_ANDROID_BLOCK_ON_PAUSE_PAUSEAUDIO "SDL_ANDROID_BLOCK_ON_PAUSE_PAUSEAUDIO"
#define SDL_HINT_RETURN_KEY_HIDES_IME "SDL_RETURN_KEY_HIDES_IME"
#define SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT   "SDL_EMSCRIPTEN_KEYBOARD_ELEMENT"
#define SDL_HINT_EMSCRIPTEN_ASYNCIFY   "SDL_EMSCRIPTEN_ASYNCIFY"
#define SDL_HINT_NO_SIGNAL_HANDLERS   "SDL_NO_SIGNAL_HANDLERS"
#define SDL_HINT_WINDOWS_NO_CLOSE_ON_ALT_F4 "SDL_WINDOWS_NO_CLOSE_ON_ALT_F4"
#define SDL_HINT_BMP_SAVE_LEGACY_FORMAT "SDL_BMP_SAVE_LEGACY_FORMAT"
#define SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING "SDL_WINDOWS_DISABLE_THREAD_NAMING"
#define SDL_HINT_RPI_VIDEO_LAYER           "SDL_RPI_VIDEO_LAYER"
#define SDL_HINT_VIDEO_DOUBLE_BUFFER      "SDL_VIDEO_DOUBLE_BUFFER"
#define SDL_HINT_OPENGL_ES_DRIVER   "SDL_OPENGL_ES_DRIVER"
#define SDL_HINT_AUDIO_RESAMPLING_MODE   "SDL_AUDIO_RESAMPLING_MODE"
#define SDL_HINT_AUDIO_CATEGORY   "SDL_AUDIO_CATEGORY"
#define SDL_HINT_RENDER_BATCHING  "SDL_RENDER_BATCHING"
#define SDL_HINT_AUTO_UPDATE_JOYSTICKS  "SDL_AUTO_UPDATE_JOYSTICKS"
#define SDL_HINT_AUTO_UPDATE_SENSORS    "SDL_AUTO_UPDATE_SENSORS"
#define SDL_HINT_EVENT_LOGGING   "SDL_EVENT_LOGGING"
#define SDL_HINT_WAVE_RIFF_CHUNK_SIZE   "SDL_WAVE_RIFF_CHUNK_SIZE"
#define SDL_HINT_WAVE_TRUNCATION   "SDL_WAVE_TRUNCATION"
#define SDL_HINT_WAVE_FACT_CHUNK   "SDL_WAVE_FACT_CHUNK"
#define SDL_HINT_DISPLAY_USABLE_BOUNDS "SDL_DISPLAY_USABLE_BOUNDS"
#define SDL_HINT_AUDIO_DEVICE_APP_NAME "SDL_AUDIO_DEVICE_APP_NAME"
#define SDL_HINT_AUDIO_DEVICE_STREAM_NAME "SDL_AUDIO_DEVICE_STREAM_NAME"
#define SDL_HINT_PREFERRED_LOCALES "SDL_PREFERRED_LOCALES"
typedef enum
{
SDL_HINT_DEFAULT,
SDL_HINT_NORMAL,
SDL_HINT_OVERRIDE
} SDL_HintPriority;
extern DECLSPEC SDL_bool SDLCALL SDL_SetHintWithPriority(const char *name,
const char *value,
SDL_HintPriority priority);
extern DECLSPEC SDL_bool SDLCALL SDL_SetHint(const char *name,
const char *value);
extern DECLSPEC const char * SDLCALL SDL_GetHint(const char *name);
extern DECLSPEC SDL_bool SDLCALL SDL_GetHintBoolean(const char *name, SDL_bool default_value);
typedef void (SDLCALL *SDL_HintCallback)(void *userdata, const char *name, const char *oldValue, const char *newValue);
extern DECLSPEC void SDLCALL SDL_AddHintCallback(const char *name,
SDL_HintCallback callback,
void *userdata);
extern DECLSPEC void SDLCALL SDL_DelHintCallback(const char *name,
SDL_HintCallback callback,
void *userdata);
extern DECLSPEC void SDLCALL SDL_ClearHints(void);
#ifdef __cplusplus
}
#endif
#include "close_code.h"
#endif  

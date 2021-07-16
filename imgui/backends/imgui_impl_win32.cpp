 
#include "imgui.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>
#include <dwmapi.h>
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include <xinput.h>
typedef DWORD (WINAPI *PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*);
typedef DWORD (WINAPI *PFN_XInputGetState)(DWORD, XINPUT_STATE*);
#endif
struct ImGui_ImplWin32_Data
{
HWND                        hWnd;
INT64                       Time;
INT64                       TicksPerSecond;
ImGuiMouseCursor            LastMouseCursor;
bool                        HasGamepad;
bool                        WantUpdateHasGamepad;
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
HMODULE                     XInputDLL;
PFN_XInputGetCapabilities   XInputGetCapabilities;
PFN_XInputGetState          XInputGetState;
#endif
ImGui_ImplWin32_Data()      { memset(this, 0, sizeof(*this)); }
};
static ImGui_ImplWin32_Data* ImGui_ImplWin32_GetBackendData()
{
return ImGui::GetCurrentContext() ? (ImGui_ImplWin32_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}
bool    ImGui_ImplWin32_Init(void* hwnd)
{
ImGuiIO& io = ImGui::GetIO();
IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");
INT64 perf_frequency, perf_counter;
if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&perf_frequency))
return false;
if (!::QueryPerformanceCounter((LARGE_INTEGER*)&perf_counter))
return false;
ImGui_ImplWin32_Data* bd = IM_NEW(ImGui_ImplWin32_Data)();
io.BackendPlatformUserData = (void*)bd;
io.BackendPlatformName = "imgui_impl_win32";
io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;          
io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;           
bd->hWnd = (HWND)hwnd;
bd->WantUpdateHasGamepad = true;
bd->TicksPerSecond = perf_frequency;
bd->Time = perf_counter;
bd->LastMouseCursor = ImGuiMouseCursor_COUNT;
io.ImeWindowHandle = hwnd;
io.KeyMap[ImGuiKey_Tab] = VK_TAB;
io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
io.KeyMap[ImGuiKey_Home] = VK_HOME;
io.KeyMap[ImGuiKey_End] = VK_END;
io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
io.KeyMap[ImGuiKey_Space] = VK_SPACE;
io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
io.KeyMap[ImGuiKey_A] = 'A';
io.KeyMap[ImGuiKey_C] = 'C';
io.KeyMap[ImGuiKey_V] = 'V';
io.KeyMap[ImGuiKey_X] = 'X';
io.KeyMap[ImGuiKey_Y] = 'Y';
io.KeyMap[ImGuiKey_Z] = 'Z';
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
const char* xinput_dll_names[] =
{
"xinput1_4.dll",    
"xinput1_3.dll",    
"xinput9_1_0.dll",  
"xinput1_2.dll",    
"xinput1_1.dll"     
};
for (int n = 0; n < IM_ARRAYSIZE(xinput_dll_names); n++)
if (HMODULE dll = ::LoadLibraryA(xinput_dll_names[n]))
{
bd->XInputDLL = dll;
bd->XInputGetCapabilities = (PFN_XInputGetCapabilities)::GetProcAddress(dll, "XInputGetCapabilities");
bd->XInputGetState = (PFN_XInputGetState)::GetProcAddress(dll, "XInputGetState");
break;
}
#endif  
return true;
}
void    ImGui_ImplWin32_Shutdown()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
if (bd->XInputDLL)
::FreeLibrary(bd->XInputDLL);
#endif  
io.BackendPlatformName = NULL;
io.BackendPlatformUserData = NULL;
IM_DELETE(bd);
}
static bool ImGui_ImplWin32_UpdateMouseCursor()
{
ImGuiIO& io = ImGui::GetIO();
if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
return false;
ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
{
::SetCursor(NULL);
}
else
{
LPTSTR win32_cursor = IDC_ARROW;
switch (imgui_cursor)
{
case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
}
::SetCursor(::LoadCursor(NULL, win32_cursor));
}
return true;
}
static void ImGui_ImplWin32_UpdateMousePos()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
IM_ASSERT(bd->hWnd != 0);
if (io.WantSetMousePos)
{
POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
if (::ClientToScreen(bd->hWnd, &pos))
::SetCursorPos(pos.x, pos.y);
}
io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
POINT pos;
if (HWND active_window = ::GetForegroundWindow())
if (active_window == bd->hWnd || ::IsChild(active_window, bd->hWnd))
if (::GetCursorPos(&pos) && ::ScreenToClient(bd->hWnd, &pos))
io.MousePos = ImVec2((float)pos.x, (float)pos.y);
}
static void ImGui_ImplWin32_UpdateGamepads()
{
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
memset(io.NavInputs, 0, sizeof(io.NavInputs));
if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
return;
if (bd->WantUpdateHasGamepad)
{
XINPUT_CAPABILITIES caps;
bd->HasGamepad = bd->XInputGetCapabilities ? (bd->XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS) : false;
bd->WantUpdateHasGamepad = false;
}
io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
XINPUT_STATE xinput_state;
if (bd->HasGamepad && bd->XInputGetState && bd->XInputGetState(0, &xinput_state) == ERROR_SUCCESS)
{
const XINPUT_GAMEPAD& gamepad = xinput_state.Gamepad;
io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
#define MAP_BUTTON(NAV_NO, BUTTON_ENUM)     { io.NavInputs[NAV_NO] = (gamepad.wButtons & BUTTON_ENUM) ? 1.0f : 0.0f; }
#define MAP_ANALOG(NAV_NO, VALUE, V0, V1)   { float vn = (float)(VALUE - V0) / (float)(V1 - V0); if (vn > 1.0f) vn = 1.0f; if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) io.NavInputs[NAV_NO] = vn; }
MAP_BUTTON(ImGuiNavInput_Activate,      XINPUT_GAMEPAD_A);               
MAP_BUTTON(ImGuiNavInput_Cancel,        XINPUT_GAMEPAD_B);               
MAP_BUTTON(ImGuiNavInput_Menu,          XINPUT_GAMEPAD_X);               
MAP_BUTTON(ImGuiNavInput_Input,         XINPUT_GAMEPAD_Y);               
MAP_BUTTON(ImGuiNavInput_DpadLeft,      XINPUT_GAMEPAD_DPAD_LEFT);       
MAP_BUTTON(ImGuiNavInput_DpadRight,     XINPUT_GAMEPAD_DPAD_RIGHT);      
MAP_BUTTON(ImGuiNavInput_DpadUp,        XINPUT_GAMEPAD_DPAD_UP);         
MAP_BUTTON(ImGuiNavInput_DpadDown,      XINPUT_GAMEPAD_DPAD_DOWN);       
MAP_BUTTON(ImGuiNavInput_FocusPrev,     XINPUT_GAMEPAD_LEFT_SHOULDER);   
MAP_BUTTON(ImGuiNavInput_FocusNext,     XINPUT_GAMEPAD_RIGHT_SHOULDER);  
MAP_BUTTON(ImGuiNavInput_TweakSlow,     XINPUT_GAMEPAD_LEFT_SHOULDER);   
MAP_BUTTON(ImGuiNavInput_TweakFast,     XINPUT_GAMEPAD_RIGHT_SHOULDER);  
MAP_ANALOG(ImGuiNavInput_LStickLeft,    gamepad.sThumbLX,  -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
MAP_ANALOG(ImGuiNavInput_LStickRight,   gamepad.sThumbLX,  +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
MAP_ANALOG(ImGuiNavInput_LStickUp,      gamepad.sThumbLY,  +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
MAP_ANALOG(ImGuiNavInput_LStickDown,    gamepad.sThumbLY,  -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32767);
#undef MAP_BUTTON
#undef MAP_ANALOG
}
#endif  
}
void    ImGui_ImplWin32_NewFrame()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
IM_ASSERT(bd != NULL && "Did you call ImGui_ImplWin32_Init()?");
RECT rect = { 0, 0, 0, 0 };
::GetClientRect(bd->hWnd, &rect);
io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
INT64 current_time = 0;
::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
io.DeltaTime = (float)(current_time - bd->Time) / bd->TicksPerSecond;
bd->Time = current_time;
io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
io.KeySuper = false;
ImGui_ImplWin32_UpdateMousePos();
ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
if (bd->LastMouseCursor != mouse_cursor)
{
bd->LastMouseCursor = mouse_cursor;
ImGui_ImplWin32_UpdateMouseCursor();
}
ImGui_ImplWin32_UpdateGamepads();
}
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef DBT_DEVNODES_CHANGED
#define DBT_DEVNODES_CHANGED 0x0007
#endif
#if 0
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
if (ImGui::GetCurrentContext() == NULL)
return 0;
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
switch (msg)
{
case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
{
int button = 0;
if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
::SetCapture(hwnd);
io.MouseDown[button] = true;
return 0;
}
case WM_LBUTTONUP:
case WM_RBUTTONUP:
case WM_MBUTTONUP:
case WM_XBUTTONUP:
{
int button = 0;
if (msg == WM_LBUTTONUP) { button = 0; }
if (msg == WM_RBUTTONUP) { button = 1; }
if (msg == WM_MBUTTONUP) { button = 2; }
if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
io.MouseDown[button] = false;
if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
::ReleaseCapture();
return 0;
}
case WM_MOUSEWHEEL:
io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
return 0;
case WM_MOUSEHWHEEL:
io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
return 0;
case WM_KEYDOWN:
case WM_SYSKEYDOWN:
if (wParam < 256)
io.KeysDown[wParam] = 1;
return 0;
case WM_KEYUP:
case WM_SYSKEYUP:
if (wParam < 256)
io.KeysDown[wParam] = 0;
return 0;
case WM_KILLFOCUS:
memset(io.KeysDown, 0, sizeof(io.KeysDown));
return 0;
case WM_CHAR:
if (wParam > 0 && wParam < 0x10000)
io.AddInputCharacterUTF16((unsigned short)wParam);
return 0;
case WM_SETCURSOR:
if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
return 1;
return 0;
case WM_DEVICECHANGE:
if ((UINT)wParam == DBT_DEVNODES_CHANGED)
bd->WantUpdateHasGamepad = true;
return 0;
}
return 0;
}
static BOOL _IsWindowsVersionOrGreater(WORD major, WORD minor, WORD)
{
typedef LONG(WINAPI* PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);
static PFN_RtlVerifyVersionInfo RtlVerifyVersionInfoFn = NULL;
	if (RtlVerifyVersionInfoFn == NULL)
		if (HMODULE ntdllModule = ::GetModuleHandleA("ntdll.dll"))
			RtlVerifyVersionInfoFn = (PFN_RtlVerifyVersionInfo)GetProcAddress(ntdllModule, "RtlVerifyVersionInfo");
if (RtlVerifyVersionInfoFn == NULL)
return FALSE;
RTL_OSVERSIONINFOEXW versionInfo = { };
ULONGLONG conditionMask = 0;
versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
versionInfo.dwMajorVersion = major;
	versionInfo.dwMinorVersion = minor;
	VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	return (RtlVerifyVersionInfoFn(&versionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask) == 0) ? TRUE : FALSE;
}
#define _IsWindowsVistaOrGreater()   _IsWindowsVersionOrGreater(HIBYTE(0x0600), LOBYTE(0x0600), 0)  
#define _IsWindows8OrGreater()       _IsWindowsVersionOrGreater(HIBYTE(0x0602), LOBYTE(0x0602), 0)  
#define _IsWindows8Point1OrGreater() _IsWindowsVersionOrGreater(HIBYTE(0x0603), LOBYTE(0x0603), 0)  
#define _IsWindows10OrGreater()      _IsWindowsVersionOrGreater(HIBYTE(0x0A00), LOBYTE(0x0A00), 0)  
#ifndef DPI_ENUMS_DECLARED
typedef enum { PROCESS_DPI_UNAWARE = 0, PROCESS_SYSTEM_DPI_AWARE = 1, PROCESS_PER_MONITOR_DPI_AWARE = 2 } PROCESS_DPI_AWARENESS;
typedef enum { MDT_EFFECTIVE_DPI = 0, MDT_ANGULAR_DPI = 1, MDT_RAW_DPI = 2, MDT_DEFAULT = MDT_EFFECTIVE_DPI } MONITOR_DPI_TYPE;
#endif
#ifndef _DPI_AWARENESS_CONTEXTS_
DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    (DPI_AWARENESS_CONTEXT)-3
#endif
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 (DPI_AWARENESS_CONTEXT)-4
#endif
typedef HRESULT(WINAPI* PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);                      
typedef HRESULT(WINAPI* PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);         
typedef DPI_AWARENESS_CONTEXT(WINAPI* PFN_SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);  
void ImGui_ImplWin32_EnableDpiAwareness()
{
if (_IsWindows10OrGreater())
{
static HINSTANCE user32_dll = ::LoadLibraryA("user32.dll");  
if (PFN_SetThreadDpiAwarenessContext SetThreadDpiAwarenessContextFn = (PFN_SetThreadDpiAwarenessContext)::GetProcAddress(user32_dll, "SetThreadDpiAwarenessContext"))
{
SetThreadDpiAwarenessContextFn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
return;
}
}
if (_IsWindows8Point1OrGreater())
{
static HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll");  
if (PFN_SetProcessDpiAwareness SetProcessDpiAwarenessFn = (PFN_SetProcessDpiAwareness)::GetProcAddress(shcore_dll, "SetProcessDpiAwareness"))
{
SetProcessDpiAwarenessFn(PROCESS_PER_MONITOR_DPI_AWARE);
return;
}
}
#if _WIN32_WINNT >= 0x0600
::SetProcessDPIAware();
#endif
}
#if defined(_MSC_VER) && !defined(NOGDI)
#pragma comment(lib, "gdi32")    
#endif
float ImGui_ImplWin32_GetDpiScaleForMonitor(void* monitor)
{
UINT xdpi = 96, ydpi = 96;
if (_IsWindows8Point1OrGreater())
{
		static HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll");  
		static PFN_GetDpiForMonitor GetDpiForMonitorFn = NULL;
		if (GetDpiForMonitorFn == NULL && shcore_dll != NULL)
GetDpiForMonitorFn = (PFN_GetDpiForMonitor)::GetProcAddress(shcore_dll, "GetDpiForMonitor");
		if (GetDpiForMonitorFn != NULL)
		{
			GetDpiForMonitorFn((HMONITOR)monitor, MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
IM_ASSERT(xdpi == ydpi);  
			return xdpi / 96.0f;
		}
}
#ifndef NOGDI
const HDC dc = ::GetDC(NULL);
xdpi = ::GetDeviceCaps(dc, LOGPIXELSX);
ydpi = ::GetDeviceCaps(dc, LOGPIXELSY);
IM_ASSERT(xdpi == ydpi);  
::ReleaseDC(NULL, dc);
#endif
return xdpi / 96.0f;
}
float ImGui_ImplWin32_GetDpiScaleForHwnd(void* hwnd)
{
HMONITOR monitor = ::MonitorFromWindow((HWND)hwnd, MONITOR_DEFAULTTONEAREST);
return ImGui_ImplWin32_GetDpiScaleForMonitor(monitor);
}
#if defined(_MSC_VER)
#pragma comment(lib, "dwmapi")   
#endif
void ImGui_ImplWin32_EnableAlphaCompositing(void* hwnd)
{
if (!_IsWindowsVistaOrGreater())
return;
BOOL composition;
if (FAILED(::DwmIsCompositionEnabled(&composition)) || !composition)
return;
BOOL opaque;
DWORD color;
if (_IsWindows8OrGreater() || (SUCCEEDED(::DwmGetColorizationColor(&color, &opaque)) && !opaque))
{
HRGN region = ::CreateRectRgn(0, 0, -1, -1);
DWM_BLURBEHIND bb = {};
bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
bb.hRgnBlur = region;
bb.fEnable = TRUE;
::DwmEnableBlurBehindWindow((HWND)hwnd, &bb);
::DeleteObject(region);
}
else
{
DWM_BLURBEHIND bb = {};
bb.dwFlags = DWM_BB_ENABLE;
::DwmEnableBlurBehindWindow((HWND)hwnd, &bb);
}
}

 
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <SDL.h>
#include <SDL_syswm.h>
#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif
#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE    SDL_VERSION_ATLEAST(2,0,4)
#define SDL_HAS_VULKAN                      SDL_VERSION_ATLEAST(2,0,6)
struct ImGui_ImplSDL2_Data
{
SDL_Window* Window;
Uint64      Time;
bool        MousePressed[3];
SDL_Cursor* MouseCursors[ImGuiMouseCursor_COUNT];
char*       ClipboardTextData;
bool        MouseCanUseGlobalState;
ImGui_ImplSDL2_Data()   { memset(this, 0, sizeof(*this)); }
};
static ImGui_ImplSDL2_Data* ImGui_ImplSDL2_GetBackendData()
{
return ImGui::GetCurrentContext() ? (ImGui_ImplSDL2_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}
static const char* ImGui_ImplSDL2_GetClipboardText(void*)
{
ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();
if (bd->ClipboardTextData)
SDL_free(bd->ClipboardTextData);
bd->ClipboardTextData = SDL_GetClipboardText();
return bd->ClipboardTextData;
}
static void ImGui_ImplSDL2_SetClipboardText(void*, const char* text)
{
SDL_SetClipboardText(text);
}
bool ImGui_ImplSDL2_ProcessEvent(const SDL_Event* event)
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();
switch (event->type)
{
case SDL_MOUSEWHEEL:
{
if (event->wheel.x > 0) io.MouseWheelH += 1;
if (event->wheel.x < 0) io.MouseWheelH -= 1;
if (event->wheel.y > 0) io.MouseWheel += 1;
if (event->wheel.y < 0) io.MouseWheel -= 1;
return true;
}
case SDL_MOUSEBUTTONDOWN:
{
if (event->button.button == SDL_BUTTON_LEFT) { bd->MousePressed[0] = true; }
if (event->button.button == SDL_BUTTON_RIGHT) { bd->MousePressed[1] = true; }
if (event->button.button == SDL_BUTTON_MIDDLE) { bd->MousePressed[2] = true; }
return true;
}
case SDL_TEXTINPUT:
{
io.AddInputCharactersUTF8(event->text.text);
return true;
}
case SDL_KEYDOWN:
case SDL_KEYUP:
{
int key = event->key.keysym.scancode;
IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
io.KeysDown[key] = (event->type == SDL_KEYDOWN);
io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
io.KeySuper = false;
#else
io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
return true;
}
}
return false;
}
static bool ImGui_ImplSDL2_Init(SDL_Window* window)
{
ImGuiIO& io = ImGui::GetIO();
IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");
ImGui_ImplSDL2_Data* bd = IM_NEW(ImGui_ImplSDL2_Data)();
io.BackendPlatformUserData = (void*)bd;
io.BackendPlatformName = "imgui_impl_sdl";
io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;        
io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;         
bd->Window = window;
io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
io.SetClipboardTextFn = ImGui_ImplSDL2_SetClipboardText;
io.GetClipboardTextFn = ImGui_ImplSDL2_GetClipboardText;
io.ClipboardUserData = NULL;
bd->MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
bd->MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
bd->MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
bd->MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
bd->MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
bd->MouseCursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
const char* sdl_backend = SDL_GetCurrentVideoDriver();
const char* global_mouse_whitelist[] = { "windows", "cocoa", "x11", "DIVE", "VMAN" };
bd->MouseCanUseGlobalState = false;
for (int n = 0; n < IM_ARRAYSIZE(global_mouse_whitelist); n++)
if (strncmp(sdl_backend, global_mouse_whitelist[n], strlen(global_mouse_whitelist[n])) == 0)
bd->MouseCanUseGlobalState = true;
#ifdef _WIN32
SDL_SysWMinfo wmInfo;
SDL_VERSION(&wmInfo.version);
SDL_GetWindowWMInfo(window, &wmInfo);
io.ImeWindowHandle = wmInfo.info.win.window;
#else
(void)window;
#endif
return true;
}
bool ImGui_ImplSDL2_InitForOpenGL(SDL_Window* window, void* sdl_gl_context)
{
IM_UNUSED(sdl_gl_context);  
return ImGui_ImplSDL2_Init(window);
}
bool ImGui_ImplSDL2_InitForVulkan(SDL_Window* window)
{
#if !SDL_HAS_VULKAN
IM_ASSERT(0 && "Unsupported");
#endif
return ImGui_ImplSDL2_Init(window);
}
bool ImGui_ImplSDL2_InitForD3D(SDL_Window* window)
{
#if !defined(_WIN32)
IM_ASSERT(0 && "Unsupported");
#endif
return ImGui_ImplSDL2_Init(window);
}
bool ImGui_ImplSDL2_InitForMetal(SDL_Window* window)
{
return ImGui_ImplSDL2_Init(window);
}
void ImGui_ImplSDL2_Shutdown()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();
if (bd->ClipboardTextData)
SDL_free(bd->ClipboardTextData);
for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
SDL_FreeCursor(bd->MouseCursors[cursor_n]);
io.BackendPlatformName = NULL;
io.BackendPlatformUserData = NULL;
IM_DELETE(bd);
}
static void ImGui_ImplSDL2_UpdateMousePosAndButtons()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();
if (io.WantSetMousePos)
SDL_WarpMouseInWindow(bd->Window, (int)io.MousePos.x, (int)io.MousePos.y);
else
io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
int mx, my;
Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
io.MouseDown[0] = bd->MousePressed[0] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;   
io.MouseDown[1] = bd->MousePressed[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
io.MouseDown[2] = bd->MousePressed[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
bd->MousePressed[0] = bd->MousePressed[1] = bd->MousePressed[2] = false;
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IOS)
SDL_Window* focused_window = SDL_GetKeyboardFocus();
if (bd->Window == focused_window)
{
if (bd->MouseCanUseGlobalState)
{
int wx, wy;
SDL_GetWindowPosition(focused_window, &wx, &wy);
SDL_GetGlobalMouseState(&mx, &my);
mx -= wx;
my -= wy;
}
io.MousePos = ImVec2((float)mx, (float)my);
}
bool any_mouse_button_down = ImGui::IsAnyMouseDown();
SDL_CaptureMouse(any_mouse_button_down ? SDL_TRUE : SDL_FALSE);
#else
if (SDL_GetWindowFlags(bd->Window) & SDL_WINDOW_INPUT_FOCUS)
io.MousePos = ImVec2((float)mx, (float)my);
#endif
}
static void ImGui_ImplSDL2_UpdateMouseCursor()
{
ImGuiIO& io = ImGui::GetIO();
if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
return;
ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();
ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
{
SDL_ShowCursor(SDL_FALSE);
}
else
{
SDL_SetCursor(bd->MouseCursors[imgui_cursor] ? bd->MouseCursors[imgui_cursor] : bd->MouseCursors[ImGuiMouseCursor_Arrow]);
SDL_ShowCursor(SDL_TRUE);
}
}
static void ImGui_ImplSDL2_UpdateGamepads()
{
ImGuiIO& io = ImGui::GetIO();
memset(io.NavInputs, 0, sizeof(io.NavInputs));
if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
return;
SDL_GameController* game_controller = SDL_GameControllerOpen(0);
if (!game_controller)
{
io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
return;
}
#define MAP_BUTTON(NAV_NO, BUTTON_NO)       { io.NavInputs[NAV_NO] = (SDL_GameControllerGetButton(game_controller, BUTTON_NO) != 0) ? 1.0f : 0.0f; }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float vn = (float)(SDL_GameControllerGetAxis(game_controller, AXIS_NO) - V0) / (float)(V1 - V0); if (vn > 1.0f) vn = 1.0f; if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) io.NavInputs[NAV_NO] = vn; }
const int thumb_dead_zone = 8000;            
MAP_BUTTON(ImGuiNavInput_Activate,      SDL_CONTROLLER_BUTTON_A);                
MAP_BUTTON(ImGuiNavInput_Cancel,        SDL_CONTROLLER_BUTTON_B);                
MAP_BUTTON(ImGuiNavInput_Menu,          SDL_CONTROLLER_BUTTON_X);                
MAP_BUTTON(ImGuiNavInput_Input,         SDL_CONTROLLER_BUTTON_Y);                
MAP_BUTTON(ImGuiNavInput_DpadLeft,      SDL_CONTROLLER_BUTTON_DPAD_LEFT);        
MAP_BUTTON(ImGuiNavInput_DpadRight,     SDL_CONTROLLER_BUTTON_DPAD_RIGHT);       
MAP_BUTTON(ImGuiNavInput_DpadUp,        SDL_CONTROLLER_BUTTON_DPAD_UP);          
MAP_BUTTON(ImGuiNavInput_DpadDown,      SDL_CONTROLLER_BUTTON_DPAD_DOWN);        
MAP_BUTTON(ImGuiNavInput_FocusPrev,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);     
MAP_BUTTON(ImGuiNavInput_FocusNext,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);    
MAP_BUTTON(ImGuiNavInput_TweakSlow,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);     
MAP_BUTTON(ImGuiNavInput_TweakFast,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);    
MAP_ANALOG(ImGuiNavInput_LStickLeft,    SDL_CONTROLLER_AXIS_LEFTX, -thumb_dead_zone, -32768);
MAP_ANALOG(ImGuiNavInput_LStickRight,   SDL_CONTROLLER_AXIS_LEFTX, +thumb_dead_zone, +32767);
MAP_ANALOG(ImGuiNavInput_LStickUp,      SDL_CONTROLLER_AXIS_LEFTY, -thumb_dead_zone, -32767);
MAP_ANALOG(ImGuiNavInput_LStickDown,    SDL_CONTROLLER_AXIS_LEFTY, +thumb_dead_zone, +32767);
io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
#undef MAP_BUTTON
#undef MAP_ANALOG
}
void ImGui_ImplSDL2_NewFrame()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();
IM_ASSERT(bd != NULL && "Did you call ImGui_ImplSDL2_Init()?");
int w, h;
int display_w, display_h;
SDL_GetWindowSize(bd->Window, &w, &h);
if (SDL_GetWindowFlags(bd->Window) & SDL_WINDOW_MINIMIZED)
w = h = 0;
SDL_GL_GetDrawableSize(bd->Window, &display_w, &display_h);
io.DisplaySize = ImVec2((float)w, (float)h);
if (w > 0 && h > 0)
io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);
static Uint64 frequency = SDL_GetPerformanceFrequency();
Uint64 current_time = SDL_GetPerformanceCounter();
io.DeltaTime = bd->Time > 0 ? (float)((double)(current_time - bd->Time) / frequency) : (float)(1.0f / 60.0f);
bd->Time = current_time;
ImGui_ImplSDL2_UpdateMousePosAndButtons();
ImGui_ImplSDL2_UpdateMouseCursor();
ImGui_ImplSDL2_UpdateGamepads();
}

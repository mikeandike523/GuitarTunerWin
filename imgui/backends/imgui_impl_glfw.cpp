 
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>    
#endif
#define GLFW_HAS_WINDOW_TOPMOST       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200)  
#define GLFW_HAS_WINDOW_HOVERED       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300)  
#define GLFW_HAS_WINDOW_ALPHA         (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300)  
#define GLFW_HAS_PER_MONITOR_DPI      (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300)  
#define GLFW_HAS_VULKAN               (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200)  
#ifdef GLFW_RESIZE_NESW_CURSOR   
#define GLFW_HAS_NEW_CURSORS          (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3400)  
#else
#define GLFW_HAS_NEW_CURSORS          (0)
#endif
enum GlfwClientApi
{
GlfwClientApi_Unknown,
GlfwClientApi_OpenGL,
GlfwClientApi_Vulkan
};
struct ImGui_ImplGlfw_Data
{
GLFWwindow*             Window;
GlfwClientApi           ClientApi;
double                  Time;
bool                    MouseJustPressed[ImGuiMouseButton_COUNT];
GLFWcursor*             MouseCursors[ImGuiMouseCursor_COUNT];
bool                    InstalledCallbacks;
GLFWmousebuttonfun      PrevUserCallbackMousebutton;
GLFWscrollfun           PrevUserCallbackScroll;
GLFWkeyfun              PrevUserCallbackKey;
GLFWcharfun             PrevUserCallbackChar;
GLFWmonitorfun          PrevUserCallbackMonitor;
ImGui_ImplGlfw_Data()   { memset(this, 0, sizeof(*this)); }
};
static ImGui_ImplGlfw_Data* ImGui_ImplGlfw_GetBackendData()
{
return ImGui::GetCurrentContext() ? (ImGui_ImplGlfw_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}
static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data)
{
return glfwGetClipboardString((GLFWwindow*)user_data);
}
static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
{
glfwSetClipboardString((GLFWwindow*)user_data, text);
}
void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
if (bd->PrevUserCallbackMousebutton != NULL)
bd->PrevUserCallbackMousebutton(window, button, action, mods);
if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(bd->MouseJustPressed))
bd->MouseJustPressed[button] = true;
}
void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
if (bd->PrevUserCallbackScroll != NULL)
bd->PrevUserCallbackScroll(window, xoffset, yoffset);
ImGuiIO& io = ImGui::GetIO();
io.MouseWheelH += (float)xoffset;
io.MouseWheel += (float)yoffset;
}
void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
if (bd->PrevUserCallbackKey != NULL)
bd->PrevUserCallbackKey(window, key, scancode, action, mods);
ImGuiIO& io = ImGui::GetIO();
if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown))
{
if (action == GLFW_PRESS)
io.KeysDown[key] = true;
if (action == GLFW_RELEASE)
io.KeysDown[key] = false;
}
io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef _WIN32
io.KeySuper = false;
#else
io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}
void ImGui_ImplGlfw_CharCallback(GLFWwindow* window, unsigned int c)
{
ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
if (bd->PrevUserCallbackChar != NULL)
bd->PrevUserCallbackChar(window, c);
ImGuiIO& io = ImGui::GetIO();
io.AddInputCharacter(c);
}
void ImGui_ImplGlfw_MonitorCallback(GLFWmonitor*, int)
{
	 
}
static bool ImGui_ImplGlfw_Init(GLFWwindow* window, bool install_callbacks, GlfwClientApi client_api)
{
ImGuiIO& io = ImGui::GetIO();
IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");
ImGui_ImplGlfw_Data* bd = IM_NEW(ImGui_ImplGlfw_Data)();
io.BackendPlatformUserData = (void*)bd;
io.BackendPlatformName = "imgui_impl_glfw";
io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;          
io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;           
bd->Window = window;
bd->Time = 0.0;
io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
io.ClipboardUserData = bd->Window;
#if defined(_WIN32)
io.ImeWindowHandle = (void*)glfwGetWin32Window(bd->Window);
#endif
GLFWerrorfun prev_error_callback = glfwSetErrorCallback(NULL);
bd->MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
bd->MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
bd->MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
glfwSetErrorCallback(prev_error_callback);
bd->PrevUserCallbackMousebutton = NULL;
bd->PrevUserCallbackScroll = NULL;
bd->PrevUserCallbackKey = NULL;
bd->PrevUserCallbackChar = NULL;
bd->PrevUserCallbackMonitor = NULL;
if (install_callbacks)
{
bd->InstalledCallbacks = true;
bd->PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
bd->PrevUserCallbackScroll = glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
bd->PrevUserCallbackKey = glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
bd->PrevUserCallbackChar = glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
bd->PrevUserCallbackMonitor = glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);
}
bd->ClientApi = client_api;
return true;
}
bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window, bool install_callbacks)
{
return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_OpenGL);
}
bool ImGui_ImplGlfw_InitForVulkan(GLFWwindow* window, bool install_callbacks)
{
return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Vulkan);
}
bool ImGui_ImplGlfw_InitForOther(GLFWwindow* window, bool install_callbacks)
{
return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Unknown);
}
void ImGui_ImplGlfw_Shutdown()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
if (bd->InstalledCallbacks)
{
glfwSetMouseButtonCallback(bd->Window, bd->PrevUserCallbackMousebutton);
glfwSetScrollCallback(bd->Window, bd->PrevUserCallbackScroll);
glfwSetKeyCallback(bd->Window, bd->PrevUserCallbackKey);
glfwSetCharCallback(bd->Window, bd->PrevUserCallbackChar);
glfwSetMonitorCallback(bd->PrevUserCallbackMonitor);
}
for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
glfwDestroyCursor(bd->MouseCursors[cursor_n]);
io.BackendPlatformName = NULL;
io.BackendPlatformUserData = NULL;
IM_DELETE(bd);
}
static void ImGui_ImplGlfw_UpdateMousePosAndButtons()
{
ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
ImGuiIO& io = ImGui::GetIO();
for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
{
io.MouseDown[i] = bd->MouseJustPressed[i] || glfwGetMouseButton(bd->Window, i) != 0;
bd->MouseJustPressed[i] = false;
}
const ImVec2 mouse_pos_backup = io.MousePos;
io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
#ifdef __EMSCRIPTEN__
const bool focused = true;  
#else
const bool focused = glfwGetWindowAttrib(bd->Window, GLFW_FOCUSED) != 0;
#endif
if (focused)
{
if (io.WantSetMousePos)
{
glfwSetCursorPos(bd->Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
}
else
{
double mouse_x, mouse_y;
glfwGetCursorPos(bd->Window, &mouse_x, &mouse_y);
io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
}
}
}
static void ImGui_ImplGlfw_UpdateMouseCursor()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(bd->Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
return;
ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
{
glfwSetInputMode(bd->Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}
else
{
glfwSetCursor(bd->Window, bd->MouseCursors[imgui_cursor] ? bd->MouseCursors[imgui_cursor] : bd->MouseCursors[ImGuiMouseCursor_Arrow]);
glfwSetInputMode(bd->Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
}
static void ImGui_ImplGlfw_UpdateGamepads()
{
ImGuiIO& io = ImGui::GetIO();
memset(io.NavInputs, 0, sizeof(io.NavInputs));
if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
return;
#define MAP_BUTTON(NAV_NO, BUTTON_NO)       { if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) io.NavInputs[NAV_NO] = 1.0f; }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); if (v > 1.0f) v = 1.0f; if (io.NavInputs[NAV_NO] < v) io.NavInputs[NAV_NO] = v; }
int axes_count = 0, buttons_count = 0;
const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
MAP_BUTTON(ImGuiNavInput_Activate,   0);      
MAP_BUTTON(ImGuiNavInput_Cancel,     1);      
MAP_BUTTON(ImGuiNavInput_Menu,       2);      
MAP_BUTTON(ImGuiNavInput_Input,      3);      
MAP_BUTTON(ImGuiNavInput_DpadLeft,   13);     
MAP_BUTTON(ImGuiNavInput_DpadRight,  11);     
MAP_BUTTON(ImGuiNavInput_DpadUp,     10);     
MAP_BUTTON(ImGuiNavInput_DpadDown,   12);     
MAP_BUTTON(ImGuiNavInput_FocusPrev,  4);      
MAP_BUTTON(ImGuiNavInput_FocusNext,  5);      
MAP_BUTTON(ImGuiNavInput_TweakSlow,  4);      
MAP_BUTTON(ImGuiNavInput_TweakFast,  5);      
MAP_ANALOG(ImGuiNavInput_LStickLeft, 0,  -0.3f,  -0.9f);
MAP_ANALOG(ImGuiNavInput_LStickRight,0,  +0.3f,  +0.9f);
MAP_ANALOG(ImGuiNavInput_LStickUp,   1,  +0.3f,  +0.9f);
MAP_ANALOG(ImGuiNavInput_LStickDown, 1,  -0.3f,  -0.9f);
#undef MAP_BUTTON
#undef MAP_ANALOG
if (axes_count > 0 && buttons_count > 0)
io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
else
io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
}
void ImGui_ImplGlfw_NewFrame()
{
ImGuiIO& io = ImGui::GetIO();
ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
IM_ASSERT(bd != NULL && "Did you call ImGui_ImplGlfw_InitForXXX()?");
int w, h;
int display_w, display_h;
glfwGetWindowSize(bd->Window, &w, &h);
glfwGetFramebufferSize(bd->Window, &display_w, &display_h);
io.DisplaySize = ImVec2((float)w, (float)h);
if (w > 0 && h > 0)
io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);
double current_time = glfwGetTime();
io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
bd->Time = current_time;
ImGui_ImplGlfw_UpdateMousePosAndButtons();
ImGui_ImplGlfw_UpdateMouseCursor();
ImGui_ImplGlfw_UpdateGamepads();
}

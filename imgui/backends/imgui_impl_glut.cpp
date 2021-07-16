 
#include "imgui.h"
#include "imgui_impl_glut.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#ifdef _MSC_VER
#pragma warning (disable: 4505)  
#endif
static int g_Time = 0;           
bool ImGui_ImplGLUT_Init()
{
ImGuiIO& io = ImGui::GetIO();
#ifdef FREEGLUT
io.BackendPlatformName = "imgui_impl_glut (freeglut)";
#else
io.BackendPlatformName = "imgui_impl_glut";
#endif
g_Time = 0;
io.KeyMap[ImGuiKey_Tab]         = '\t';  
io.KeyMap[ImGuiKey_LeftArrow]   = 256 + GLUT_KEY_LEFT;
io.KeyMap[ImGuiKey_RightArrow]  = 256 + GLUT_KEY_RIGHT;
io.KeyMap[ImGuiKey_UpArrow]     = 256 + GLUT_KEY_UP;
io.KeyMap[ImGuiKey_DownArrow]   = 256 + GLUT_KEY_DOWN;
io.KeyMap[ImGuiKey_PageUp]      = 256 + GLUT_KEY_PAGE_UP;
io.KeyMap[ImGuiKey_PageDown]    = 256 + GLUT_KEY_PAGE_DOWN;
io.KeyMap[ImGuiKey_Home]        = 256 + GLUT_KEY_HOME;
io.KeyMap[ImGuiKey_End]         = 256 + GLUT_KEY_END;
io.KeyMap[ImGuiKey_Insert]      = 256 + GLUT_KEY_INSERT;
io.KeyMap[ImGuiKey_Delete]      = 127;
io.KeyMap[ImGuiKey_Backspace]   = 8;   
io.KeyMap[ImGuiKey_Space]       = ' ';
io.KeyMap[ImGuiKey_Enter]       = 13;  
io.KeyMap[ImGuiKey_Escape]      = 27;
io.KeyMap[ImGuiKey_KeyPadEnter] = 13;  
io.KeyMap[ImGuiKey_A]           = 'A';
io.KeyMap[ImGuiKey_C]           = 'C';
io.KeyMap[ImGuiKey_V]           = 'V';
io.KeyMap[ImGuiKey_X]           = 'X';
io.KeyMap[ImGuiKey_Y]           = 'Y';
io.KeyMap[ImGuiKey_Z]           = 'Z';
return true;
}
void ImGui_ImplGLUT_InstallFuncs()
{
glutReshapeFunc(ImGui_ImplGLUT_ReshapeFunc);
glutMotionFunc(ImGui_ImplGLUT_MotionFunc);
glutPassiveMotionFunc(ImGui_ImplGLUT_MotionFunc);
glutMouseFunc(ImGui_ImplGLUT_MouseFunc);
#ifdef __FREEGLUT_EXT_H__
glutMouseWheelFunc(ImGui_ImplGLUT_MouseWheelFunc);
#endif
glutKeyboardFunc(ImGui_ImplGLUT_KeyboardFunc);
glutKeyboardUpFunc(ImGui_ImplGLUT_KeyboardUpFunc);
glutSpecialFunc(ImGui_ImplGLUT_SpecialFunc);
glutSpecialUpFunc(ImGui_ImplGLUT_SpecialUpFunc);
}
void ImGui_ImplGLUT_Shutdown()
{
}
void ImGui_ImplGLUT_NewFrame()
{
ImGuiIO& io = ImGui::GetIO();
int current_time = glutGet(GLUT_ELAPSED_TIME);
int delta_time_ms = (current_time - g_Time);
if (delta_time_ms <= 0)
delta_time_ms = 1;
io.DeltaTime = delta_time_ms / 1000.0f;
g_Time = current_time;
ImGui::NewFrame();
}
static void ImGui_ImplGLUT_UpdateKeyboardMods()
{
ImGuiIO& io = ImGui::GetIO();
int mods = glutGetModifiers();
io.KeyCtrl = (mods & GLUT_ACTIVE_CTRL) != 0;
io.KeyShift = (mods & GLUT_ACTIVE_SHIFT) != 0;
io.KeyAlt = (mods & GLUT_ACTIVE_ALT) != 0;
}
void ImGui_ImplGLUT_KeyboardFunc(unsigned char c, int x, int y)
{
ImGuiIO& io = ImGui::GetIO();
if (c >= 32)
io.AddInputCharacter((unsigned int)c);
if (c >= 1 && c <= 26)
io.KeysDown[c] = io.KeysDown[c - 1 + 'a'] = io.KeysDown[c - 1 + 'A'] = true;
else if (c >= 'a' && c <= 'z')
io.KeysDown[c] = io.KeysDown[c - 'a' + 'A'] = true;
else if (c >= 'A' && c <= 'Z')
io.KeysDown[c] = io.KeysDown[c - 'A' + 'a'] = true;
else
io.KeysDown[c] = true;
ImGui_ImplGLUT_UpdateKeyboardMods();
(void)x; (void)y;  
}
void ImGui_ImplGLUT_KeyboardUpFunc(unsigned char c, int x, int y)
{
ImGuiIO& io = ImGui::GetIO();
if (c >= 1 && c <= 26)
io.KeysDown[c] = io.KeysDown[c - 1 + 'a'] = io.KeysDown[c - 1 + 'A'] = false;
else if (c >= 'a' && c <= 'z')
io.KeysDown[c] = io.KeysDown[c - 'a' + 'A'] = false;
else if (c >= 'A' && c <= 'Z')
io.KeysDown[c] = io.KeysDown[c - 'A' + 'a'] = false;
else
io.KeysDown[c] = false;
ImGui_ImplGLUT_UpdateKeyboardMods();
(void)x; (void)y;  
}
void ImGui_ImplGLUT_SpecialFunc(int key, int x, int y)
{
ImGuiIO& io = ImGui::GetIO();
if (key + 256 < IM_ARRAYSIZE(io.KeysDown))
io.KeysDown[key + 256] = true;
ImGui_ImplGLUT_UpdateKeyboardMods();
(void)x; (void)y;  
}
void ImGui_ImplGLUT_SpecialUpFunc(int key, int x, int y)
{
ImGuiIO& io = ImGui::GetIO();
if (key + 256 < IM_ARRAYSIZE(io.KeysDown))
io.KeysDown[key + 256] = false;
ImGui_ImplGLUT_UpdateKeyboardMods();
(void)x; (void)y;  
}
void ImGui_ImplGLUT_MouseFunc(int glut_button, int state, int x, int y)
{
ImGuiIO& io = ImGui::GetIO();
io.MousePos = ImVec2((float)x, (float)y);
int button = -1;
if (glut_button == GLUT_LEFT_BUTTON) button = 0;
if (glut_button == GLUT_RIGHT_BUTTON) button = 1;
if (glut_button == GLUT_MIDDLE_BUTTON) button = 2;
if (button != -1 && state == GLUT_DOWN)
io.MouseDown[button] = true;
if (button != -1 && state == GLUT_UP)
io.MouseDown[button] = false;
}
#ifdef __FREEGLUT_EXT_H__
void ImGui_ImplGLUT_MouseWheelFunc(int button, int dir, int x, int y)
{
ImGuiIO& io = ImGui::GetIO();
io.MousePos = ImVec2((float)x, (float)y);
if (dir > 0)
io.MouseWheel += 1.0;
else if (dir < 0)
io.MouseWheel -= 1.0;
(void)button;  
}
#endif
void ImGui_ImplGLUT_ReshapeFunc(int w, int h)
{
ImGuiIO& io = ImGui::GetIO();
io.DisplaySize = ImVec2((float)w, (float)h);
}
void ImGui_ImplGLUT_MotionFunc(int x, int y)
{
ImGuiIO& io = ImGui::GetIO();
io.MousePos = ImVec2((float)x, (float)y);
}

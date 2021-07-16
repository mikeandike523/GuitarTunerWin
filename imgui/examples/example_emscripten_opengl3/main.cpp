 
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>
#include <SDL.h>
#include <SDL_opengles2.h>
SDL_Window*     g_Window = NULL;
SDL_GLContext   g_GLContext = NULL;
static void main_loop(void*);
int main(int, char**)
{
if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
{
printf("Error: %s\n", SDL_GetError());
return -1;
}
const char* glsl_version = "#version 100";
SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
SDL_DisplayMode current;
SDL_GetCurrentDisplayMode(0, &current);
SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
g_Window = SDL_CreateWindow("Dear ImGui Emscripten example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
g_GLContext = SDL_GL_CreateContext(g_Window);
if (!g_GLContext)
{
fprintf(stderr, "Failed to initialize WebGL context!\n");
return 1;
}
SDL_GL_SetSwapInterval(1);  
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
io.IniFilename = NULL;
ImGui::StyleColorsDark();
ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
ImGui_ImplOpenGL3_Init(glsl_version);
#ifndef IMGUI_DISABLE_FILE_FUNCTIONS
io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);
#endif
emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
}
static void main_loop(void* arg)
{
ImGuiIO& io = ImGui::GetIO();
IM_UNUSED(arg);  
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
SDL_Event event;
while (SDL_PollEvent(&event))
{
ImGui_ImplSDL2_ProcessEvent(&event);
}
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplSDL2_NewFrame();
ImGui::NewFrame();
if (show_demo_window)
ImGui::ShowDemoWindow(&show_demo_window);
{
static float f = 0.0f;
static int counter = 0;
ImGui::Begin("Hello, world!");                                 
ImGui::Text("This is some useful text.");                      
ImGui::Checkbox("Demo Window", &show_demo_window);             
ImGui::Checkbox("Another Window", &show_another_window);
ImGui::SliderFloat("float", &f, 0.0f, 1.0f);                   
ImGui::ColorEdit3("clear color", (float*)&clear_color);        
if (ImGui::Button("Button"))                                   
counter++;
ImGui::SameLine();
ImGui::Text("counter = %d", counter);
ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
ImGui::End();
}
if (show_another_window)
{
ImGui::Begin("Another Window", &show_another_window);          
ImGui::Text("Hello from another window!");
if (ImGui::Button("Close Me"))
show_another_window = false;
ImGui::End();
}
ImGui::Render();
SDL_GL_MakeCurrent(g_Window, g_GLContext);
glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
glClear(GL_COLOR_BUFFER_BIT);
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
SDL_GL_SwapWindow(g_Window);
}

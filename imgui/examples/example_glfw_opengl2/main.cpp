 
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
static void glfw_error_callback(int error, const char* description)
{
fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
int main(int, char**)
{
glfwSetErrorCallback(glfw_error_callback);
if (!glfwInit())
return 1;
GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL2 example", NULL, NULL);
if (window == NULL)
return 1;
glfwMakeContextCurrent(window);
glfwSwapInterval(1);  
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
ImGui::StyleColorsDark();
ImGui_ImplGlfw_InitForOpenGL(window, true);
ImGui_ImplOpenGL2_Init();
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
while (!glfwWindowShouldClose(window))
{
glfwPollEvents();
ImGui_ImplOpenGL2_NewFrame();
ImGui_ImplGlfw_NewFrame();
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
int display_w, display_h;
glfwGetFramebufferSize(window, &display_w, &display_h);
glViewport(0, 0, display_w, display_h);
glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
glClear(GL_COLOR_BUFFER_BIT);
ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
glfwMakeContextCurrent(window);
glfwSwapBuffers(window);
}
ImGui_ImplOpenGL2_Shutdown();
ImGui_ImplGlfw_Shutdown();
ImGui::DestroyContext();
glfwDestroyWindow(window);
glfwTerminate();
return 0;
}

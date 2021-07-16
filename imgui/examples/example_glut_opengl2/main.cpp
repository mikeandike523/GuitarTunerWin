 
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#ifdef _MSC_VER
#pragma warning (disable: 4505)  
#endif
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
void my_display_code()
{
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
}
void glut_display_func()
{
ImGui_ImplOpenGL2_NewFrame();
ImGui_ImplGLUT_NewFrame();
my_display_code();
ImGui::Render();
ImGuiIO& io = ImGui::GetIO();
glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
glClear(GL_COLOR_BUFFER_BIT);
ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
glutSwapBuffers();
glutPostRedisplay();
}
int main(int argc, char** argv)
{
glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
glutInitWindowSize(1280, 720);
glutCreateWindow("Dear ImGui GLUT+OpenGL2 Example");
glutDisplayFunc(glut_display_func);
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
ImGui::StyleColorsDark();
ImGui_ImplGLUT_Init();
ImGui_ImplGLUT_InstallFuncs();
ImGui_ImplOpenGL2_Init();
glutMainLoop();
ImGui_ImplOpenGL2_Shutdown();
ImGui_ImplGLUT_Shutdown();
ImGui::DestroyContext();
return 0;
}

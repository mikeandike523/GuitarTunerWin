 
#include "imgui.h"
#include "imgui_impl_marmalade.h"
#include <stdio.h>
#include <s3eKeyboard.h>
#include <s3ePointer.h>
#include <IwGx.h>
int main(int, char**)
{
IwGxInit();
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
ImGui::StyleColorsDark();
ImGui_Marmalade_Init(true);
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
while (true)
{
if (s3eDeviceCheckQuitRequest())
break;
s3eKeyboardUpdate();
s3ePointerUpdate();
ImGui_Marmalade_NewFrame();
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
IwGxSetColClear(clear_color.x * 255, clear_color.y * 255, clear_color.z * 255, clear_color.w * 255);
IwGxClear();
ImGui_Marmalade_RenderDrawData(ImGui::GetDrawData());
IwGxSwapBuffers();
s3eDeviceYield(0);
}
ImGui_Marmalade_Shutdown();
ImGui::DestroyContext();
IwGxTerminate();
return 0;
}

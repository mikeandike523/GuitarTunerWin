 
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include <stdio.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>
#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>
static WGPUDevice    wgpu_device = NULL;
static WGPUSurface   wgpu_surface = NULL;
static WGPUSwapChain wgpu_swap_chain = NULL;
static int           wgpu_swap_chain_width = 0;
static int           wgpu_swap_chain_height = 0;
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static bool init_wgpu();
static void main_loop(void* window);
static void print_glfw_error(int error, const char* description);
static void print_wgpu_error(WGPUErrorType error_type, const char* message, void*);
int main(int, char**)
{
glfwSetErrorCallback(print_glfw_error);
if (!glfwInit())
return 1;
glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+WebGPU example", NULL, NULL);
if (!window)
{
glfwTerminate();
return 1;
}
if (!init_wgpu())
{
if (window)
glfwDestroyWindow(window);
glfwTerminate();
return 1;
}
glfwShowWindow(window);
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
io.IniFilename = NULL;
ImGui::StyleColorsDark();
ImGui_ImplGlfw_InitForOther(window, true);
ImGui_ImplWGPU_Init(wgpu_device, 3, WGPUTextureFormat_RGBA8Unorm);
#ifndef IMGUI_DISABLE_FILE_FUNCTIONS
io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);
#endif
emscripten_set_main_loop_arg(main_loop, window, 0, false);
return 0;
}
static bool init_wgpu()
{
wgpu_device = emscripten_webgpu_get_device();
if (!wgpu_device)
return false;
wgpuDeviceSetUncapturedErrorCallback(wgpu_device, print_wgpu_error, NULL);
wgpu::SurfaceDescriptorFromCanvasHTMLSelector html_surface_desc = {};
html_surface_desc.selector = "#canvas";
wgpu::SurfaceDescriptor surface_desc = {};
surface_desc.nextInChain = &html_surface_desc;
wgpu::Instance instance = {};
wgpu_surface = instance.CreateSurface(&surface_desc).Release();
return true;
}
static void main_loop(void* window)
{
glfwPollEvents();
int width, height;
glfwGetFramebufferSize((GLFWwindow*) window, &width, &height);
if (width != wgpu_swap_chain_width && height != wgpu_swap_chain_height)
{
ImGui_ImplWGPU_InvalidateDeviceObjects();
if (wgpu_swap_chain)
wgpuSwapChainRelease(wgpu_swap_chain);
wgpu_swap_chain_width = width;
wgpu_swap_chain_height = height;
WGPUSwapChainDescriptor swap_chain_desc = {};
swap_chain_desc.usage = WGPUTextureUsage_RenderAttachment;
swap_chain_desc.format = WGPUTextureFormat_RGBA8Unorm;
swap_chain_desc.width = width;
swap_chain_desc.height = height;
swap_chain_desc.presentMode = WGPUPresentMode_Fifo;
wgpu_swap_chain = wgpuDeviceCreateSwapChain(wgpu_device, wgpu_surface, &swap_chain_desc);
ImGui_ImplWGPU_CreateDeviceObjects();
}
ImGui_ImplWGPU_NewFrame();
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
WGPURenderPassColorAttachment color_attachments = {};
color_attachments.loadOp = WGPULoadOp_Clear;
color_attachments.storeOp = WGPUStoreOp_Store;
color_attachments.clearColor = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
color_attachments.view = wgpuSwapChainGetCurrentTextureView(wgpu_swap_chain);
WGPURenderPassDescriptor render_pass_desc = {};
render_pass_desc.colorAttachmentCount = 1;
render_pass_desc.colorAttachments = &color_attachments;
render_pass_desc.depthStencilAttachment = NULL;
WGPUCommandEncoderDescriptor enc_desc = {};
WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(wgpu_device, &enc_desc);
WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
wgpuRenderPassEncoderEndPass(pass);
WGPUCommandBufferDescriptor cmd_buffer_desc = {};
WGPUCommandBuffer cmd_buffer = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
WGPUQueue queue = wgpuDeviceGetQueue(wgpu_device);
wgpuQueueSubmit(queue, 1, &cmd_buffer);
}
static void print_glfw_error(int error, const char* description)
{
printf("Glfw Error %d: %s\n", error, description);
}
static void print_wgpu_error(WGPUErrorType error_type, const char* message, void*)
{
const char* error_type_lbl = "";
switch (error_type)
{
case WGPUErrorType_Validation:  error_type_lbl = "Validation"; break;
case WGPUErrorType_OutOfMemory: error_type_lbl = "Out of memory"; break;
case WGPUErrorType_Unknown:     error_type_lbl = "Unknown"; break;
case WGPUErrorType_DeviceLost:  error_type_lbl = "Device lost"; break;
default:                        error_type_lbl = "Unknown";
}
printf("%s error: %s\n", error_type_lbl, message);
}

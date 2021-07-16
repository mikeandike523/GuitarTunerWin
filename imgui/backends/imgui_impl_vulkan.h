 
#pragma once
#include "imgui.h"       
#if defined(IMGUI_IMPL_VULKAN_NO_PROTOTYPES) && !defined(VK_NO_PROTOTYPES)
#define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>
struct ImGui_ImplVulkan_InitInfo
{
VkInstance                      Instance;
VkPhysicalDevice                PhysicalDevice;
VkDevice                        Device;
uint32_t                        QueueFamily;
VkQueue                         Queue;
VkPipelineCache                 PipelineCache;
VkDescriptorPool                DescriptorPool;
uint32_t                        Subpass;
uint32_t                        MinImageCount;           
uint32_t                        ImageCount;              
VkSampleCountFlagBits           MSAASamples;             
const VkAllocationCallbacks*    Allocator;
void                            (*CheckVkResultFn)(VkResult err);
};
IMGUI_IMPL_API bool     ImGui_ImplVulkan_Init(ImGui_ImplVulkan_InitInfo* info, VkRenderPass render_pass);
IMGUI_IMPL_API void     ImGui_ImplVulkan_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplVulkan_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplVulkan_RenderDrawData(ImDrawData* draw_data, VkCommandBuffer command_buffer, VkPipeline pipeline = VK_NULL_HANDLE);
IMGUI_IMPL_API bool     ImGui_ImplVulkan_CreateFontsTexture(VkCommandBuffer command_buffer);
IMGUI_IMPL_API void     ImGui_ImplVulkan_DestroyFontUploadObjects();
IMGUI_IMPL_API void     ImGui_ImplVulkan_SetMinImageCount(uint32_t min_image_count);  
IMGUI_IMPL_API bool     ImGui_ImplVulkan_LoadFunctions(PFN_vkVoidFunction(*loader_func)(const char* function_name, void* user_data), void* user_data = NULL);
struct ImGui_ImplVulkanH_Frame;
struct ImGui_ImplVulkanH_Window;
IMGUI_IMPL_API void                 ImGui_ImplVulkanH_CreateOrResizeWindow(VkInstance instance, VkPhysicalDevice physical_device, VkDevice device, ImGui_ImplVulkanH_Window* wnd, uint32_t queue_family, const VkAllocationCallbacks* allocator, int w, int h, uint32_t min_image_count);
IMGUI_IMPL_API void                 ImGui_ImplVulkanH_DestroyWindow(VkInstance instance, VkDevice device, ImGui_ImplVulkanH_Window* wnd, const VkAllocationCallbacks* allocator);
IMGUI_IMPL_API VkSurfaceFormatKHR   ImGui_ImplVulkanH_SelectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space);
IMGUI_IMPL_API VkPresentModeKHR     ImGui_ImplVulkanH_SelectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkPresentModeKHR* request_modes, int request_modes_count);
IMGUI_IMPL_API int                  ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode);
struct ImGui_ImplVulkanH_Frame
{
VkCommandPool       CommandPool;
VkCommandBuffer     CommandBuffer;
VkFence             Fence;
VkImage             Backbuffer;
VkImageView         BackbufferView;
VkFramebuffer       Framebuffer;
};
struct ImGui_ImplVulkanH_FrameSemaphores
{
VkSemaphore         ImageAcquiredSemaphore;
VkSemaphore         RenderCompleteSemaphore;
};
struct ImGui_ImplVulkanH_Window
{
int                 Width;
int                 Height;
VkSwapchainKHR      Swapchain;
VkSurfaceKHR        Surface;
VkSurfaceFormatKHR  SurfaceFormat;
VkPresentModeKHR    PresentMode;
VkRenderPass        RenderPass;
VkPipeline          Pipeline;                
bool                ClearEnable;
VkClearValue        ClearValue;
uint32_t            FrameIndex;              
uint32_t            ImageCount;              
uint32_t            SemaphoreIndex;          
ImGui_ImplVulkanH_Frame*            Frames;
ImGui_ImplVulkanH_FrameSemaphores*  FrameSemaphores;
ImGui_ImplVulkanH_Window()
{
memset(this, 0, sizeof(*this));
PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
ClearEnable = true;
}
};

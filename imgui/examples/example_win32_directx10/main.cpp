 
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#include <d3d10_1.h>
#include <d3d10.h>
#include <tchar.h>
static ID3D10Device*            g_pd3dDevice = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D10RenderTargetView*  g_mainRenderTargetView = NULL;
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int main(int, char**)
{
WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
::RegisterClassEx(&wc);
HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX10 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
if (!CreateDeviceD3D(hwnd))
{
CleanupDeviceD3D();
::UnregisterClass(wc.lpszClassName, wc.hInstance);
return 1;
}
::ShowWindow(hwnd, SW_SHOWDEFAULT);
::UpdateWindow(hwnd);
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
ImGui::StyleColorsDark();
ImGui_ImplWin32_Init(hwnd);
ImGui_ImplDX10_Init(g_pd3dDevice);
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool done = false;
while (!done)
{
MSG msg;
while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
{
::TranslateMessage(&msg);
::DispatchMessage(&msg);
if (msg.message == WM_QUIT)
done = true;
}
if (done)
break;
ImGui_ImplDX10_NewFrame();
ImGui_ImplWin32_NewFrame();
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
const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
g_pd3dDevice->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
g_pd3dDevice->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
g_pSwapChain->Present(1, 0);  
}
ImGui_ImplDX10_Shutdown();
ImGui_ImplWin32_Shutdown();
ImGui::DestroyContext();
CleanupDeviceD3D();
::DestroyWindow(hwnd);
::UnregisterClass(wc.lpszClassName, wc.hInstance);
return 0;
}
bool CreateDeviceD3D(HWND hWnd)
{
DXGI_SWAP_CHAIN_DESC sd;
ZeroMemory(&sd, sizeof(sd));
sd.BufferCount = 2;
sd.BufferDesc.Width = 0;
sd.BufferDesc.Height = 0;
sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
sd.BufferDesc.RefreshRate.Numerator = 60;
sd.BufferDesc.RefreshRate.Denominator = 1;
sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
sd.OutputWindow = hWnd;
sd.SampleDesc.Count = 1;
sd.SampleDesc.Quality = 0;
sd.Windowed = TRUE;
sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
UINT createDeviceFlags = 0;
if (D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice) != S_OK)
return false;
CreateRenderTarget();
return true;
}
void CleanupDeviceD3D()
{
CleanupRenderTarget();
if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}
void CreateRenderTarget()
{
ID3D10Texture2D* pBackBuffer;
g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
pBackBuffer->Release();
}
void CleanupRenderTarget()
{
if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
return true;
switch (msg)
{
case WM_SIZE:
if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
{
CleanupRenderTarget();
g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
CreateRenderTarget();
}
return 0;
case WM_SYSCOMMAND:
if ((wParam & 0xfff0) == SC_KEYMENU)  
return 0;
break;
case WM_DESTROY:
::PostQuitMessage(0);
return 0;
}
return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

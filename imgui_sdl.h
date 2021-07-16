#pragma once
struct ImDrawData;
struct SDL_Renderer;
namespace ImGuiSDL
{
	 
	void Initialize(SDL_Renderer* renderer, int windowWidth, int windowHeight);
	 
	 
	void Deinitialize();
	 
	 
	void Render(ImDrawData* drawData);
}

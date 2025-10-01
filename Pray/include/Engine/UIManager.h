#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Window.h"
#include "World/ChunkManager.h"
#include "Renderer/MasterRenderer.h"

class UIManager {

public:
	UIManager(Window* window);

	~UIManager();

	void debugChunkManager(ChunkManager* chunkManager);
	void fpsCounter(float& deltaTime);
	void renderStats(MasterRenderer* masterRenderer);
	void beginFrame();
	void endFrame();

	
private:
	bool wireframeMode = false; // default off

};


#endif



#include "Window.h"
#include "UIManager.h"
#include "Renderer/MasterRenderer.h"
#include "Player/Player.h"
#include "InputStruct.h"

//#include world in future

class VoxelEngine {

public:
	VoxelEngine(int width, int height);
	~VoxelEngine();
	void run();

private:
	Window* window;
	UIManager* ui;
	MasterRenderer* renderer;

	Shader* shader;
	//will change from chunk to chunk manager etc
	ChunkManager* chunkManager;
	//Chunk* chunk;
	Player* player;

	InputState inputState;
	void processInput(float dt);

};


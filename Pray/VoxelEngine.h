#include "Window.h"
#include "MasterRenderer.h"
#include "Player.h"
#include "InputStruct.h"

//#include world in future

class VoxelEngine {

public:
	VoxelEngine(int width, int height);
	~VoxelEngine();
	void run();

private:
	Window* window;
	MasterRenderer* renderer;

	Shader* shader;
	//will change from chunk to chunk manager etc
	Chunk* chunk;
	Player* player;

	InputState inputState;
	void processInput(float dt);

};


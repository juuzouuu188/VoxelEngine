#include "Engine/VoxelEngine.h"
#include "Engine/Window.h"
#include "Renderer/MasterRenderer.h"
#include "Player/Player.h"
#include "InputStruct.h"
#include "World/Chunk.h"
#include <glm/gtc/matrix_transform.hpp>

VoxelEngine::VoxelEngine(int width, int height)
{
    window = new Window(width, height, "Voxel Engine");

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    ChunkRenderer::LoadCubeTextures(); // load all shared textures once

    // Initialize shader and renderer
    shader = new Shader("shaders/Cube.vert", "shaders/Cube.frag");
    renderer = new MasterRenderer();
    player = new Player(glm::vec3(10.0f, 1.0f, 5.0f));
    chunkManager = new ChunkManager(15);

    //this is for debugging purposes of one chunk
   //  chunk = new Chunk();
   // chunk->Load();           // allocate memory
   // chunk->setUpSphere();

    // Create 6 test chunks
    for (int x = 0; x < 3; ++x) {
        for (int z = 0; z < 2; ++z) {
            chunkManager->GetOrCreateChunk(x, z); // this will add them to loadList
        }
    }
}

VoxelEngine::~VoxelEngine() {
    delete player;
    delete shader;
    delete renderer;
	delete window;
    delete chunkManager;
}


void VoxelEngine::processInput(float dt) {
    GLFWwindow* win = window->getGLFWwindow();

    // Keyboard input
    inputState.moveForward = (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS);
    inputState.moveBackward = (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS);
    inputState.moveLeft = (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS);
    inputState.moveRight = (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS);

    // Mouse input
    double newX, newY;
    glfwGetCursorPos(win, &newX, &newY);

    inputState.mouseDeltaX = newX - inputState.mouseX;
    inputState.mouseDeltaY = inputState.mouseY - newY; // invert Y if needed
    inputState.mouseMoved = (inputState.mouseDeltaX != 0.0 || inputState.mouseDeltaY != 0.0);

    inputState.mouseX = newX;
    inputState.mouseY = newY;

    // Update player with this input
    if (player)
        player->update(dt, inputState);

}

void VoxelEngine::run() {
    // Setup view and projection matrices once
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        (float)1280 / (float)720,
        0.1f, 100.0f);


    float lastFrame = 0.0f;

    while (!window->shouldClose()) {

        float currentFrame = glfwGetTime();   // time in seconds since GLFW was initialized
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);  // pass deltaTime to player update
 

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = player->getCamera().getViewMatrix();
      
       // renderer->drawChunk(chunk, shader, view, projection);
        // Update chunk manager with camera position and view
        chunkManager->Update(deltaTime, player->getCamera().getPosition(), view);
        renderer->drawWorld(chunkManager->getRenderList(), shader, view, projection);
        window->swapBuffers();
        window->pollEvents();
    }

}


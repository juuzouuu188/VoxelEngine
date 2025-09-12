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
    ui = new UIManager(window);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    ChunkRenderer::LoadCubeTextures(); // load all shared textures once

    // Initialize shader and renderer
    shader = new Shader("shaders/Cube.vert", "shaders/Cube.frag");
    renderer = new MasterRenderer();
    player = new Player(glm::vec3(10.0f, 1.0f, 5.0f));
    chunkManager = new ChunkManager(50);


   // Create a square tile of chunks centered around (0, 0)
  //  int radius = 7; // number of chunks from center to edge
    //for (int x = -radius; x < radius; ++x) {
      //  for (int z = -radius; z < radius; ++z) {
        //    chunkManager->GetOrCreateChunk(x, z); // Adds chunk to load list
          //  std::cout << "Created chunk at (" << x << ", " << z << ")\n";
       // }
   // }
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
   // glm::mat4 projection = glm::perspective(glm::radians(45.0f),
     //   (float)1280 / (float)720,
       // 0.1f, 100.0f);


    float lastFrame = 0.0f;

    while (!window->shouldClose()) {
        // Get current window size
        int width, height;
        glfwGetFramebufferSize(window->getGLFWwindow(), &width, &height);

        // Update viewport
        glViewport(0, 0, width, height);

        // Update projection matrix dynamically
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)width / (float)height,
            0.1f, 100.0f);


        float currentFrame = glfwGetTime();   // time in seconds since GLFW was initialized
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);  // pass deltaTime to player update
 

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = player->getCamera().getViewMatrix();

        //3d pipeline
        chunkManager->Update(deltaTime, player->getCamera().getPosition(), view);
        renderer->drawWorld(chunkManager->getRenderList(), shader, view, projection);


        // // --- UI ---   (ui pipeline)
        ui->beginFrame();
        ui->fpsCounter(deltaTime);
        ui->debugChunkManager(chunkManager);
        ui->endFrame();

        window->swapBuffers();
        window->pollEvents();
    }

}


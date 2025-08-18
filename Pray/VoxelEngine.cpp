#include "VoxelEngine.h"
#include "Window.h"
#include "MasterRenderer.h"
#include "Player.h"
#include "InputStruct.h"
#include <glm/gtc/matrix_transform.hpp>

VoxelEngine::VoxelEngine(int width, int height)
    : cube(glm::vec3(0.0f, 0.0f, -3.0f), 0) // Create cube at Z = -3 with textureId 0
{
    // Create the window
    window = new Window(width, height, "Voxel Engine");

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Initialize shader and renderer
    shader = new Shader("Cube.vert", "Cube.frag");
    renderer = new MasterRenderer();
    player = new Player(glm::vec3(0.0f, 1.0f, 3.0f));
}

VoxelEngine::~VoxelEngine() {
    delete player;
    delete shader;
    delete renderer;
	delete window;
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
        (float)800 / (float)600,
        0.1f, 100.0f);

    //glm::mat4 view = glm::mat4(1.0f); // camera at origin looking down -Z
  //  view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));


    float lastFrame = 0.0f;

    while (!window->shouldClose()) {

        float currentFrame = glfwGetTime();   // time in seconds since GLFW was initialized
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);  // pass deltaTime to player update


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = player->getCamera().getViewMatrix();

        renderer->drawCube(cube, *shader, view, projection);

        window->swapBuffers();
        window->pollEvents();
    }

}


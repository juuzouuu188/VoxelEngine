#include "MasterRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

//keep cube vertices here for debugging and move to mesh class later
// Cube vertices (positions + UVs)
static float vertices[] = {
    // positions          // uvs
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f
};

static unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,  // back
    4, 5, 6, 6, 7, 4,  // front
    0, 4, 7, 7, 3, 0,  // left
    1, 5, 6, 6, 2, 1,  // right
    3, 2, 6, 6, 7, 3,  // top
    0, 1, 5, 5, 4, 0   // bottom
};

MasterRenderer::MasterRenderer() : vao(), vbo(nullptr), ebo(nullptr) {
	initCube();
}

MasterRenderer::~MasterRenderer() {
    if (vbo) delete vbo;
    if (ebo) delete ebo;
}

void MasterRenderer::initCube() {
    vao.Bind();

    vbo = new VBO(vertices, sizeof(vertices));
    ebo = new EBO(indices, sizeof(indices));

    // Position attribute
    vao.LinkVBO(*vbo, 0, 3, 5 * sizeof(float), (void*)0);
    // UV attribute
    vao.LinkVBO(*vbo, 1, 2, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    vao.Unbind();
}

void MasterRenderer::drawCube(const Cube& cube, Shader& shader, const glm::mat4& view, const glm::mat4& projection){
    shader.Activate();
    // static rotation variable persists across frames
    static float rotation = 0.0f;
    rotation += 0.0001f; // adjust speed as desired

    // Create model matrix with translation and rotation
    glm::mat4 model = glm::translate(glm::mat4(1.0f), cube.getPosition());
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 1.0f, 0.0f)); // rotate around Y-axis

    // Pass matrices to shader
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Draw the cube
    vao.Bind();
    ebo->Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    vao.Unbind();
}




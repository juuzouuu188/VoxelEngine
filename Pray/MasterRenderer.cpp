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

void MasterRenderer::drawCube(const glm::vec3& position, CubeType type, Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.Activate();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    vao.Bind();
    ebo->Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    vao.Unbind();
}

// Draw all cubes in a chunk
void MasterRenderer::drawChunk(const Chunk& chunk, const glm::vec3& chunkPos, Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    Cube*** cubes = chunk.getCubes();

    for (int x = 0; x < Chunk::CHUNK_SIZE; ++x) {
        for (int y = 0; y < Chunk::CHUNK_SIZE; ++y) {
            for (int z = 0; z < Chunk::CHUNK_SIZE; ++z) {
                // maybe check to skip air/"empty"
                glm::vec3 worldPos = chunkPos + glm::vec3(x, y, z);
                drawCube(worldPos, cubes[x][y][z].getType(), shader, view, projection);
            }
        }
    }
}





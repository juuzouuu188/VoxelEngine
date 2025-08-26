#include "MasterRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>


MasterRenderer::MasterRenderer() {
}

void MasterRenderer::drawWorld() {
    //this is debugging purposes
    //add projection and view matrices later
}


void MasterRenderer::drawChunk(Chunk* chunk, Shader* shader, const glm::mat4& view, const glm::mat4& projection) {
    ChunkRenderer chunkRenderer(chunk, shader);
    chunkRenderer.Draw(meshRenderer, view, projection);
}


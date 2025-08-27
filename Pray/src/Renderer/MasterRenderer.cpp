#include "Renderer/MasterRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>


MasterRenderer::MasterRenderer() {
   
}

void MasterRenderer::drawWorld(const std::vector<std::shared_ptr<Chunk>> renderList, Shader* shader,
    const glm::mat4& view, const glm::mat4& projection) {

    // Only create ChunkRenderers for chunks that don't have one yet
    for (const auto& chunk : renderList) {

        // Check if we already have a renderer for this chunk
        auto it = std::find_if(chunkRenderers.begin(), chunkRenderers.end(),
            [&](const std::shared_ptr<ChunkRenderer>& r) {
                return r->getChunk() == chunk.get();
            });

        if (it == chunkRenderers.end()) {
            // Create a new persistent ChunkRenderer
            chunkRenderers.push_back(std::make_shared<ChunkRenderer>(chunk.get(), shader));
        }
    }

    // Draw all chunk renderers
    for (auto& chunkRenderer : chunkRenderers) {
        chunkRenderer->Draw(meshRenderer, view, projection);
    }
}


void MasterRenderer::drawChunk(Chunk* chunk, Shader* shader, const glm::mat4& view, const glm::mat4& projection) {
    ChunkRenderer chunkRenderer(chunk, shader);
    chunkRenderer.Draw(meshRenderer, view, projection);
}


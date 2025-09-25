#include "Renderer/MasterRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>


MasterRenderer::MasterRenderer() {
   
}

void MasterRenderer::drawWorld(const std::vector<std::shared_ptr<Chunk>> renderList, Shader* shader,
    const glm::mat4& view, const glm::mat4& projection) {

    int chunkIndex = 0;
    for (const auto& chunk : renderList) {
        if (!chunk || !chunk->IsLoaded() || !chunk->isSetUp())
            continue;

        // Check if a ChunkRenderer exists for this chunk
        auto it = std::find_if(chunkRenderers.begin(), chunkRenderers.end(),
            [&](const std::shared_ptr<ChunkRenderer>& r) {
                return r->getChunk() == chunk.get();
            });

        std::shared_ptr<ChunkRenderer> rendererPtr;
        if (it == chunkRenderers.end()) {
            rendererPtr = std::make_shared<ChunkRenderer>(chunk.get(), shader);
            chunkRenderers.push_back(rendererPtr);
           // std::cout << "Created ChunkRenderer for chunk " << chunkIndex << "\n";
        }
        else {
            rendererPtr = *it;
        }

        // Rebuild mesh if needed
        if (chunk->needsRebuild() ) {
            if (useGreedyMesh) {
                //  std::cout << "Rebuilding mesh for chunk " << chunkIndex << "\n";
                rendererPtr->BuildGreedyMeshes();
            }
            else {
                rendererPtr->BuildMeshes();
            }
            chunk->setNeedsRebuild(false);
        }

        // Draw the chunk
        rendererPtr->Draw(meshRenderer, view, projection);
      //  std::cout << "Drawing chunk " << chunkIndex++  << "\n";
    }

}

size_t MasterRenderer::totalVertices() {

    size_t count = 0;
    if (chunkRenderers.size() != 0) {
        for (auto& cr : chunkRenderers) {
            count += cr->getVertexCount();
        }
    }
    return count;
}


//void MasterRenderer::drawChunk(Chunk* chunk, Shader* shader, const glm::mat4& view, const glm::mat4& projection) {
  //  ChunkRenderer chunkRenderer(chunk, shader);
    //chunkRenderer.Draw(meshRenderer, view, projection);
//}

void MasterRenderer::setGreedyMesh(bool b) {
    useGreedyMesh = b;
}

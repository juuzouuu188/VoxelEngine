 #pragma once
#ifndef MASTERRENDERER_H
#define MASTERRENDERER_H

#include "Renderer/ChunkRenderer.h"
#include "World/ChunkManager.h"
#include "MeshRenderer.h"
#include "Shader.h"
#include <glm/glm.hpp>

// MasterRenderer now just orchestrates drawing
class MasterRenderer {

public:
    MasterRenderer();
    ~MasterRenderer() = default;

    //will change this to have chunk manager take it later

    void drawWorld(const std::vector<std::shared_ptr<Chunk>> renderList, Shader* shader, const glm::mat4& view, const glm::mat4& projection);
    void drawChunk(Chunk* chunk, Shader* shader, const glm::mat4& view, const glm::mat4& projection);


    //Debug
    size_t totalVertices();
    void setGreedyMesh(bool b);
    bool getGreedyMesh() { return useGreedyMesh; }
private:
    MeshRenderer meshRenderer; // Reusable mesh renderer for all meshes
    std::vector<std::shared_ptr<ChunkRenderer>> chunkRenderers;
    //    void drawChunk(Chunk* chunk, Shader* shader, const glm::mat4& view, const glm::mat4& projection);


    bool useGreedyMesh = true; // default
};

#endif

#pragma once
#ifndef MASTERRENDERER_H
#define MASTERRENDERER_H

#include "Renderer/ChunkRenderer.h"
#include "MeshRenderer.h"
#include "Shader.h"
#include <glm/glm.hpp>

// MasterRenderer now just orchestrates drawing
class MasterRenderer {

public:
    MasterRenderer();
    ~MasterRenderer() = default;

    //will change this to have chunk manager take it later

    void drawWorld(); //will add camera position andview vectors later
    void drawChunk(Chunk* chunk, Shader* shader, const glm::mat4& view, const glm::mat4& projection);

private:
    //chunk manager will be here in future
    MeshRenderer meshRenderer; // Reusable mesh renderer for all meshes
   // ChunkRenderer chunkRenderer; this will be a chunk manager for multiple chunks when we have a chunk manager
    //    void drawChunk(Chunk* chunk, Shader* shader, const glm::mat4& view, const glm::mat4& projection);
};

#endif

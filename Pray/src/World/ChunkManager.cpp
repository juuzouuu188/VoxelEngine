#include "World/ChunkManager.h"
#include "Renderer/ChunkRenderer.h"
#include "Renderer/MeshRenderer.h"
#include "Renderer/Shader.h"
#include <glm/glm.hpp>
#include <cmath>

ChunkManager::ChunkManager(int renderDistance)
    : renderDistance(renderDistance),
    m_cameraPosition(0.0f), m_cameraView(0.0f) {
}

// --- Main update ---
void ChunkManager::Update(float dt, const glm::vec3& cameraPos, const glm::vec3& cameraView) {
    UpdateLoadList();
    UpdateSetupList();
    UpdateRebuildList();
    UpdateUnloadList();
    UpdateVisibilityList(cameraPos);

    if (m_cameraPosition != cameraPos || m_cameraView != cameraView || m_forceVisibilityUpdate) {
        UpdateRenderList();
        m_forceVisibilityUpdate = false;
    }

    m_cameraPosition = cameraPos;
    m_cameraView = cameraView;
}

// --- Draw all visible chunks ---
void ChunkManager::DrawAll(Shader* shader,MeshRenderer& meshRenderer, const glm::mat4& view, const glm::mat4& proj) {
    for (auto& chunk : m_renderList) {
        if (!chunk) continue;

        // Create a temporary ChunkRenderer for this chunk
        ChunkRenderer chunkRenderer(chunk.get(), shader);

        // Draw the chunk using the MeshRenderer
        chunkRenderer.Draw(meshRenderer, view, proj);
    }
}

// --- Get or create chunk ---
std::shared_ptr<Chunk> ChunkManager::GetOrCreateChunk(int x, int z) {
    ChunkCoords coords{ x, z };
    auto it = chunks.find(coords);
    if (it != chunks.end()) return it->second;

    auto chunk = std::make_shared<Chunk>();
    chunks[coords] = chunk;
    m_loadList.push_back(chunk);
    return chunk;
}

// --- Pipeline stages (simplified since ChunkRenderer handles meshes) ---
void ChunkManager::UpdateLoadList() {
    int loadedThisFrame = 0;
    for (auto& chunk : m_loadList) {
        if (loadedThisFrame >= MAX_CHUNKS_PER_FRAME) break;
        // For now, loading just marks it ready
        loadedThisFrame++;
        m_forceVisibilityUpdate = true;
    }
    m_loadList.clear();
}

void ChunkManager::UpdateSetupList() {
    m_setupList.clear(); // ChunkRenderer handles setup
}

void ChunkManager::UpdateRebuildList() {
    m_rebuildList.clear(); // ChunkRenderer rebuilds meshes if chunk.isDirty
}

void ChunkManager::UpdateUnloadList() {
    for (auto& chunk : m_unloadList) {
        // Could free memory here if needed
        m_forceVisibilityUpdate = true;
    }
    m_unloadList.clear();
}

void ChunkManager::UpdateVisibilityList(const glm::vec3& cameraPos) {
    m_visibilityList.clear();
    float maxDistance = static_cast<float>(renderDistance * Chunk::CHUNK_SIZE);

    for (auto& [coords, chunk] : chunks) {
        glm::vec3 center = chunk->getCubes() ? glm::vec3(coords.x, 0, coords.z) : glm::vec3(0);
        float dist = glm::length(cameraPos - center);
        if (dist <= maxDistance) m_visibilityList.push_back(chunk);
    }

    m_forceVisibilityUpdate = true;
}

void ChunkManager::UpdateRenderList() {
    m_renderList.clear();
    for (auto& chunk : m_visibilityList) {
        m_renderList.push_back(chunk); // All visible chunks are drawn
    }
}

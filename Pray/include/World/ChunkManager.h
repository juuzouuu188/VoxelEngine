#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include "Chunk.h"

// Key for identifying chunks by grid position
struct ChunkCoords {
    int x, z;
    bool operator==(const ChunkCoords& other) const {
        return x == other.x && z == other.z;
    }
};

// Hash function for unordered_map
struct ChunkCoordsHash {
    std::size_t operator()(const ChunkCoords& coords) const {
        return std::hash<int>()(coords.x) ^ (std::hash<int>()(coords.z) << 1);
    }
};

//using hashmaps so you load and unload chunks in the correct order and efficiently
//will finish this later

class ChunkManager {

public:
    ChunkManager(int renderDistance);

    void Update(float dt, const glm::vec3& cameraPosition, const glm::vec3& cameraView);
    void DrawAll(class Shader* shader, MeshRenderer& meshRenderer, const glm::mat4& view, const glm::mat4& proj);

private:
    // --- internal update stages ---
    void UpdateAsyncChunker();
    void UpdateLoadList();
    void UpdateSetupList();
    void UpdateRebuildList();
    void UpdateFlagsList();
    void UpdateUnloadList();
    void UpdateVisibilityList(const glm::vec3& cameraPos); //distance based for now
    void UpdateRenderList();

    // Helper: get or create chunk at coordinates
    std::shared_ptr<Chunk> GetOrCreateChunk(int x, int z);

private:
    std::unordered_map<ChunkCoords, std::shared_ptr<Chunk>, ChunkCoordsHash> chunks;

    std::vector<std::shared_ptr<Chunk>> m_loadList;
    std::vector<std::shared_ptr<Chunk>> m_setupList;
    std::vector<std::shared_ptr<Chunk>> m_rebuildList;
    std::vector<std::shared_ptr<Chunk>> m_unloadList;
    std::vector<std::shared_ptr<Chunk>> m_visibilityList;
    std::vector<std::shared_ptr<Chunk>> m_renderList;

    glm::vec3 m_cameraPosition;
    glm::vec3 m_cameraView;

    const int renderDistance; // in chunks
    const int MAX_CHUNKS_PER_FRAME = 2;
    bool m_forceVisibilityUpdate = false;
    glm::vec3 m_cameraPosition;
    glm::vec3 m_cameraView;

};

#endif
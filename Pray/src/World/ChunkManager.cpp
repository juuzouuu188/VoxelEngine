#include "World/ChunkManager.h"
#include "Renderer/ChunkRenderer.h"
#include "Renderer/MeshRenderer.h"
#include "Renderer/Shader.h"
#include <glm/glm.hpp>
#include <cmath>

ChunkManager::ChunkManager(int renderDistance)
    : renderDistance(renderDistance),
    m_cameraPosition(0.0f), m_cameraView(0.0f){
}

// --- Main update ---
void ChunkManager::Update(float dt, const glm::vec3& cameraPos, const glm::mat4& cameraView) {
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


// --- Get or create chunk ---
std::shared_ptr<Chunk> ChunkManager::GetOrCreateChunk(int x, int z) {
    return GetOrCreateChunk(ChunkCoords{ x, z });
}

std::shared_ptr<Chunk> ChunkManager::GetOrCreateChunk(const ChunkCoords& coords) {
    auto it = chunks.find(coords);
    if (it != chunks.end()) return it->second;

    // Create chunk with its coords
    auto chunk = std::make_shared<Chunk>(coords);
    chunks[coords] = chunk;
    m_loadList.push_back(chunk);

    std::cout << "Chunk at (" << coords.x << ", " << coords.z << ") created and added to load list.\n";
    return chunk;
}


// --- Pipeline stages ---
void ChunkManager::UpdateLoadList() {
    int lNumOfChunksLoaded = 0;

    // Iterate through the load list, but only load up to ASYNC_NUM_CHUNKS_PER_FRAME
    for (auto it = m_loadList.begin();
        it != m_loadList.end() && lNumOfChunksLoaded != ASYNC_NUM_CHUNKS_PER_FRAME;
        ++it)
    {
        std::shared_ptr<Chunk> pChunk = *it; // Dereference iterator

        if (!pChunk->IsLoaded()) {
            pChunk->Load();
            lNumOfChunksLoaded++;
            m_setupList.push_back(pChunk);
            m_forceVisibilityUpdate = true;
            std::cout << "Some chunk is loaded \n";
        }
    }

    // Clear the load list at the end of the frame
    m_loadList.clear();
}

void ChunkManager::UpdateSetupList() {
    //Setup any chunks that have not been set up

    // Iterate through the set up list
    for (auto it = m_setupList.begin(); it != m_setupList.end(); ++it)
    {
        std::shared_ptr<Chunk> pChunk = *it; // Dereference iterator

        if (pChunk->IsLoaded() && !pChunk->isSetUp()) {
            //for now and debugging we will just setup all chunks as spheres
            pChunk->setUpSphere();

            //Explicitly mark as needing rebuild
            pChunk->setNeedsRebuild(true);
            m_rebuildList.push_back(pChunk);
            std::cout << "Some chunk is setup \n";

            if (pChunk->isSetUp() == true) {
                m_forceVisibilityUpdate = true;
            }
        }
    }
    //clear the list everyframe
    m_setupList.clear();
}

void ChunkManager::UpdateRebuildList() {
    //Rebuild any chunks that are in the rebuild list
    int chunksMarkedThisFrame = 0;

    for (auto& pChunk : m_rebuildList) { 

        if (chunksMarkedThisFrame >= ASYNC_NUM_CHUNKS_PER_FRAME)
            break;

        if (pChunk->IsLoaded() && pChunk->isSetUp() && pChunk->needsRebuild()) {

            // Mark the chunk itself
           // pChunk->setNeedsRebuild(true);
            std::cout << "Some chunk is rebuilt \n";
            chunksMarkedThisFrame++;
            m_forceVisibilityUpdate = true;

            // Neighbor offsets
            const ChunkCoords offsets[4] = {
                {1, 0}, {-1, 0}, {0, 1}, {0, -1}
            };

            // Flag neighbors
            for (const auto& off : offsets) {
                ChunkCoords neighborCoords;
                neighborCoords.x = GetChunkX(pChunk) + off.x;
                neighborCoords.z = GetChunkZ(pChunk) + off.z;

                auto it = chunks.find(neighborCoords);
                if (it != chunks.end()) {
                    it->second->setNeedsRebuild(true);
                }
            }
        }
    }

    m_rebuildList.clear();
}


void ChunkManager::UpdateUnloadList() {
    //Unload any chunks that are in the unload list
    //logic for pushing this will be done in visibility 
    for (auto& pChunk : m_unloadList) {
        if (pChunk->IsLoaded()) {
            pChunk->UnLoad();
            m_forceVisibilityUpdate = true;
        }
    }
    m_unloadList.clear();
}

void ChunkManager::UpdateVisibilityList(const glm::vec3& cameraPos) {
    m_visibilityList.clear();  // Clear previous visibility list

    for (auto& pair : chunks) {
        auto& chunk = pair.second;
        if (!chunk || !chunk->IsLoaded() || !chunk->isSetUp())
            continue;

        // Compute the chunk's world center position
        glm::vec3 chunkPos = glm::vec3(pair.first.x * Chunk::CHUNK_SIZE,
            0.0f,  // Assuming Y is vertical; adjust if needed
            pair.first.z * Chunk::CHUNK_SIZE);
        glm::vec3 chunkCenter = chunkPos + glm::vec3(Chunk::CHUNK_SIZE / 2.0f);

        // Compute distance to camera
        float distance = glm::length(chunkCenter - cameraPos);

        // Add chunk to visibility list if within render distance
        if (distance <= renderDistance * Chunk::CHUNK_SIZE) {
            m_visibilityList.push_back(chunk);
          //  std::cout << "Chunk at (" << pair.first.x << ", " << pair.first.z << ") is visible. \n";
            //this works
        }
        else {
            m_unloadList.push_back(chunk);
        }
    }
}
void ChunkManager::UpdateRenderList() {
    m_renderList.clear();  // Clear the render list first

    for (auto& chunk : m_visibilityList) {
        if (chunk && chunk->IsLoaded() && chunk->isSetUp()) {
            m_renderList.push_back(chunk);
        }
    }
}



void ChunkManager::UpdateFlagsList() {

}


int ChunkManager::GetChunkX(const std::shared_ptr<Chunk>& chunk) {
    for (auto& pair : chunks) {
        if (pair.second == chunk) return pair.first.x;
    }
    return 0; // fallback, should not happen if chunk exists
}

int ChunkManager::GetChunkZ(const std::shared_ptr<Chunk>& chunk) {
    for (auto& pair : chunks) {
        if (pair.second == chunk) return pair.first.z;
    }
    return 0;
}
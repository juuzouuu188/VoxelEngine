#include "Renderer/ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Chunk* chunk, Shader* shader)
    : chunk(chunk), shader(shader)
{

    BuildMeshes();
}
void ChunkRenderer::BuildMeshes() {
    // Currently hybrid approach, between multiple meshes and texture atlas
    meshes.clear();

    Cube*** cubes = chunk->getCubes();
    int CHUNK_SIZE = Chunk::CHUNK_SIZE;
    float cubeSize = Chunk::CUBE_SIZE; // scale cubes
    int CHUNK_HEIGHT = Chunk::CHUNK_HEIGHT;

    // Compute the world offset for this chunk based on its grid coordinates
    ChunkCoords coords = chunk->getCoords(); // Chunk now stores its coords

    glm::vec3 chunkWorldOffset = glm::vec3(
        coords.x * CHUNK_SIZE * cubeSize,  // world X
        0.0f,                              // world Y
        coords.z * CHUNK_SIZE * cubeSize   // world Z
    );


    // Step 1: Group cubes by type
    std::unordered_map<CubeType, std::vector<Vertex>> verticesByType;
    std::unordered_map<CubeType, std::vector<GLuint>> indicesByType;
    std::unordered_map<CubeType, GLuint> indexCountByType;

    // this is naive, will make greedy algo later
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                Cube& cube = cubes[x][y][z];
                if (cube.isAir()) continue;

                CubeType type = cube.getType();
                if (indexCountByType.find(type) == indexCountByType.end())
                    indexCountByType[type] = 0;

                // Scale cube position and add chunk offset
                glm::vec3 pos(x, y, z);
                pos *= cubeSize;
                pos += chunkWorldOffset;  // world space coordinates

                auto addFace = [&](const glm::vec3& normal) {
                    int nx = x + (int)normal.x;
                    int ny = y + (int)normal.y;
                    int nz = z + (int)normal.z;

                    bool neighborExists = false;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_HEIGHT &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        neighborExists = !cubes[nx][ny][nz].isAir();
                    }

                    if (!neighborExists) {
                        glm::vec3 p0, p1, p2, p3;
                        if (normal.x == 1) {       // +X face
                            p0 = pos + glm::vec3(cubeSize, 0, 0);
                            p1 = pos + glm::vec3(cubeSize, 0, cubeSize);
                            p2 = pos + glm::vec3(cubeSize, cubeSize, cubeSize);
                            p3 = pos + glm::vec3(cubeSize, cubeSize, 0);
                        }
                        else if (normal.x == -1) { // -X face
                            p0 = pos + glm::vec3(0, 0, 0);
                            p1 = pos + glm::vec3(0, cubeSize, 0);
                            p2 = pos + glm::vec3(0, cubeSize, cubeSize);
                            p3 = pos + glm::vec3(0, 0, cubeSize);
                        }
                        else if (normal.y == 1) {  // +Y face
                            p0 = pos + glm::vec3(0, cubeSize, 0);
                            p1 = pos + glm::vec3(cubeSize, cubeSize, 0);
                            p2 = pos + glm::vec3(cubeSize, cubeSize, cubeSize);
                            p3 = pos + glm::vec3(0, cubeSize, cubeSize);
                        }
                        else if (normal.y == -1) { // -Y face
                            p0 = pos + glm::vec3(0, 0, 0);
                            p1 = pos + glm::vec3(0, 0, cubeSize);
                            p2 = pos + glm::vec3(cubeSize, 0, cubeSize);
                            p3 = pos + glm::vec3(cubeSize, 0, 0);
                        }
                        else if (normal.z == 1) {  // +Z face
                            p0 = pos + glm::vec3(0, 0, cubeSize);
                            p1 = pos + glm::vec3(0, cubeSize, cubeSize);
                            p2 = pos + glm::vec3(cubeSize, cubeSize, cubeSize);
                            p3 = pos + glm::vec3(cubeSize, 0, cubeSize);
                        }
                        else if (normal.z == -1) { // -Z face
                            p0 = pos + glm::vec3(0, 0, 0);
                            p1 = pos + glm::vec3(cubeSize, 0, 0);
                            p2 = pos + glm::vec3(cubeSize, cubeSize, 0);
                            p3 = pos + glm::vec3(0, cubeSize, 0);
                        }

                        glm::vec3 color(1.0f, 1.0f, 1.0f);
                        verticesByType[type].push_back(Vertex{ p0, normal, color, glm::vec2(0,0) });
                        verticesByType[type].push_back(Vertex{ p1, normal, color, glm::vec2(1,0) });
                        verticesByType[type].push_back(Vertex{ p2, normal, color, glm::vec2(1,1) });
                        verticesByType[type].push_back(Vertex{ p3, normal, color, glm::vec2(0,1) });

                        GLuint idx = indexCountByType[type];
                        indicesByType[type].push_back(idx);
                        indicesByType[type].push_back(idx + 1);
                        indicesByType[type].push_back(idx + 2);
                        indicesByType[type].push_back(idx);
                        indicesByType[type].push_back(idx + 2);
                        indicesByType[type].push_back(idx + 3);

                        indexCountByType[type] += 4;
                    }
                    };

                addFace(glm::vec3(1, 0, 0));
                addFace(glm::vec3(-1, 0, 0));
                addFace(glm::vec3(0, 1, 0));
                addFace(glm::vec3(0, -1, 0));
                addFace(glm::vec3(0, 0, 1));
                addFace(glm::vec3(0, 0, -1));
            }
        }
    }

    // Step 2: Create one mesh per texture group
    for (auto& pair : verticesByType) {
        CubeType type = pair.first;
        auto& verts = pair.second;
        auto& inds = indicesByType[type];

        std::vector<Texture*> tex;
        if (ChunkRenderer::cubeTextures.find(type) != ChunkRenderer::cubeTextures.end())
            tex.push_back(ChunkRenderer::cubeTextures[type].get());

        if (!verts.empty())
            meshes.push_back(std::make_unique<Mesh>(verts, inds, tex));
    }
}



void ChunkRenderer::Draw(MeshRenderer& meshRenderer, const glm::mat4& view, const glm::mat4& projection) {

    if (!shader) return;

    // Replace with your actual camera matrices
    //glm::mat4 view = glm::mat4(1.0f);
    //glm::mat4 projection = glm::mat4(1.0f);

    for (auto& mesh : meshes) {
        meshRenderer.setMesh(*mesh);   // Update the mesh in the renderer if you have a setter
        meshRenderer.drawMesh(*shader, view, projection);

    }

}

//std::unordered_map<CubeType, Texture> ChunkRenderer::cubeTextures;  woah this broke the program

std::unordered_map<CubeType, std::unique_ptr<Texture>> ChunkRenderer::cubeTextures;
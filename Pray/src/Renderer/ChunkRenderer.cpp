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

    //do greedy slicing for each face?

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

void ChunkRenderer::BuildGreedyMeshes() {
    meshes.clear();

    Cube*** cubes = chunk->getCubes();
    int CHUNK_SIZE = Chunk::CHUNK_SIZE;
    int CHUNK_HEIGHT = Chunk::CHUNK_HEIGHT;
    float cubeSize = Chunk::CUBE_SIZE;

    ChunkCoords coords = chunk->getCoords();
    glm::vec3 chunkWorldOffset = glm::vec3(
        coords.x * CHUNK_SIZE * cubeSize,
        0.0f,
        coords.z * CHUNK_SIZE * cubeSize
    );

    std::unordered_map<CubeType, std::vector<Vertex>> verticesByType;
    std::unordered_map<CubeType, std::vector<GLuint>> indicesByType;
    std::unordered_map<CubeType, GLuint> indexCountByType;

    auto addQuad = [&](CubeType type,
        const glm::vec3& p0, const glm::vec3& p1,
        const glm::vec3& p2, const glm::vec3& p3,
        const glm::vec3& normal,
        int width, int height)
        {
            if (indexCountByType.find(type) == indexCountByType.end())
                indexCountByType[type] = 0;

            GLuint idx = indexCountByType[type];
            glm::vec3 color(1.0f);

            // Calculate proper UV coordinates
            glm::vec2 uv0, uv1, uv2, uv3;

            if (normal.x != 0) {
                uv0 = glm::vec2(p0.z, p0.y);
                uv1 = glm::vec2(p1.z, p1.y);
                uv2 = glm::vec2(p2.z, p2.y);
                uv3 = glm::vec2(p3.z, p3.y);
            }
            else if (normal.y != 0) {
                uv0 = glm::vec2(p0.x, p0.z);
                uv1 = glm::vec2(p1.x, p1.z);
                uv2 = glm::vec2(p2.x, p2.z);
                uv3 = glm::vec2(p3.x, p3.z);
            }
            else if (normal.z != 0) {
                uv0 = glm::vec2(p0.x, p0.y);
                uv1 = glm::vec2(p1.x, p1.y);
                uv2 = glm::vec2(p2.x, p2.y);
                uv3 = glm::vec2(p3.x, p3.y);
            }

            float uvScale = 1.0f / cubeSize;
            uv0 *= uvScale;
            uv1 *= uvScale;
            uv2 *= uvScale;
            uv3 *= uvScale;

            verticesByType[type].push_back({ p0, normal, color, uv0 });
            verticesByType[type].push_back({ p1, normal, color, uv1 });
            verticesByType[type].push_back({ p2, normal, color, uv2 });
            verticesByType[type].push_back({ p3, normal, color, uv3 });

            indicesByType[type].push_back(idx);
            indicesByType[type].push_back(idx + 1);
            indicesByType[type].push_back(idx + 2);
            indicesByType[type].push_back(idx);
            indicesByType[type].push_back(idx + 2);
            indicesByType[type].push_back(idx + 3);

            indexCountByType[type] += 4;
        };

    // ========== PROPER GREEDY MESHING ==========
    auto greedyPlane = [&](glm::vec3 normal) {
        int u, v, fixedAxis;
        if (normal.x != 0) { fixedAxis = 0; u = 1; v = 2; }
        else if (normal.y != 0) { fixedAxis = 1; u = 0; v = 2; }
        else { fixedAxis = 2; u = 0; v = 1; }

        int sizeFixed = (fixedAxis == 0) ? CHUNK_SIZE : (fixedAxis == 1) ? CHUNK_HEIGHT : CHUNK_SIZE;
        int sizeU = (u == 0) ? CHUNK_SIZE : (u == 1) ? CHUNK_HEIGHT : CHUNK_SIZE;
        int sizeV = (v == 0) ? CHUNK_SIZE : (v == 1) ? CHUNK_HEIGHT : CHUNK_SIZE;

        // Process each slice along the fixed axis
        for (int fixed = 0; fixed < sizeFixed; ++fixed) {
            std::vector<CubeType> mask(sizeU * sizeV, CubeType_Air);
            std::vector<bool> merged(sizeU * sizeV, false);

            // Build mask for this slice - check face visibility in the CURRENT direction
            for (int i = 0; i < sizeU; ++i) {
                for (int j = 0; j < sizeV; ++j) {
                    int x = (fixedAxis == 0) ? fixed : (u == 0 ? i : j);
                    int y = (fixedAxis == 1) ? fixed : (u == 1 ? i : j);
                    int z = (fixedAxis == 2) ? fixed : (u == 2 ? i : j);

                    Cube& cube = cubes[x][y][z];
                    if (cube.isAir()) {
                        mask[i * sizeV + j] = CubeType_Air;
                        continue;
                    }

                    // Check if this specific face is visible in the current direction
                    int nx = x + (int)normal.x;
                    int ny = y + (int)normal.y;
                    int nz = z + (int)normal.z;

                    bool isFaceVisible = false;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_HEIGHT &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        isFaceVisible = cubes[nx][ny][nz].isAir();
                    }
                    else {
                        isFaceVisible = true; // Chunk boundary
                    }

                    mask[i * sizeV + j] = isFaceVisible ? cube.getType() : CubeType_Air;
                }
            }

            // TRUE GREEDY MESHING ALGORITHM
            for (int j = 0; j < sizeV; ++j) {
                for (int i = 0; i < sizeU; ++i) {
                    if (mask[i * sizeV + j] == CubeType_Air || merged[i * sizeV + j]) {
                        continue;
                    }

                    CubeType currentType = mask[i * sizeV + j];

                    // Find maximum width for this row
                    int width = 1;
                    while (i + width < sizeU &&
                        mask[(i + width) * sizeV + j] == currentType &&
                        !merged[(i + width) * sizeV + j]) {
                        width++;
                    }

                    // Find maximum height for this width
                    int height = 1;
                    bool canExtendHeight = true;

                    for (int h = 1; j + h < sizeV && canExtendHeight; h++) {
                        // Check if entire next row is mergeable
                        for (int w = 0; w < width; w++) {
                            int idx = (i + w) * sizeV + (j + h);
                            if (mask[idx] != currentType || merged[idx]) {
                                canExtendHeight = false;
                                break;
                            }
                        }
                        if (canExtendHeight) {
                            height++;
                        }
                    }

                    // Mark all positions in this rectangle as merged
                    for (int w = 0; w < width; w++) {
                        for (int h = 0; h < height; h++) {
                            merged[(i + w) * sizeV + (j + h)] = true;
                        }
                    }

                    // Build the merged quad
                    glm::vec3 pos(0.0f);
                    pos[fixedAxis] = fixed;
                    pos[u] = i;
                    pos[v] = j;
                    pos *= cubeSize;
                    pos += chunkWorldOffset;

                    float quadWidth = width * cubeSize;
                    float quadHeight = height * cubeSize;

                    glm::vec3 p0, p1, p2, p3;

                    if (normal.x == 1) {
                        p0 = pos + glm::vec3(cubeSize, 0, 0);
                        p1 = p0 + glm::vec3(0, 0, quadHeight);
                        p2 = p1 + glm::vec3(0, quadWidth, 0);
                        p3 = p0 + glm::vec3(0, quadWidth, 0);
                    }
                    else if (normal.x == -1) {
                        p0 = pos;
                        p1 = p0 + glm::vec3(0, quadWidth, 0);
                        p2 = p1 + glm::vec3(0, 0, quadHeight);
                        p3 = p0 + glm::vec3(0, 0, quadHeight);
                    }
                    else if (normal.y == 1) {
                        p0 = pos + glm::vec3(0, cubeSize, 0);
                        p1 = p0 + glm::vec3(quadWidth, 0, 0);
                        p2 = p1 + glm::vec3(0, 0, quadHeight);
                        p3 = p0 + glm::vec3(0, 0, quadHeight);
                    }
                    else if (normal.y == -1) {
                        p0 = pos;
                        p1 = p0 + glm::vec3(0, 0, quadHeight);
                        p2 = p1 + glm::vec3(quadWidth, 0, 0);
                        p3 = p0 + glm::vec3(quadWidth, 0, 0);
                    }
                    else if (normal.z == 1) {
                        p0 = pos + glm::vec3(0, 0, cubeSize);
                        p1 = p0 + glm::vec3(quadWidth, 0, 0);
                        p2 = p1 + glm::vec3(0, quadHeight, 0);
                        p3 = p0 + glm::vec3(0, quadHeight, 0);
                    }
                    else if (normal.z == -1) {
                        p0 = pos;
                        p1 = p0 + glm::vec3(quadWidth, 0, 0);
                        p2 = p1 + glm::vec3(0, quadHeight, 0);
                        p3 = p0 + glm::vec3(0, quadHeight, 0);
                    }

                    addQuad(currentType, p0, p1, p2, p3, normal, width, height);
                }
            }
        }
        };

    // Process all 6 directions
    greedyPlane(glm::vec3(1, 0, 0));  // +X
    greedyPlane(glm::vec3(-1, 0, 0)); // -X
    greedyPlane(glm::vec3(0, 1, 0));  // +Y
    greedyPlane(glm::vec3(0, -1, 0)); // -Y
    greedyPlane(glm::vec3(0, 0, 1));  // +Z
    greedyPlane(glm::vec3(0, 0, -1)); // -Z

    // Create final meshes
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


size_t ChunkRenderer::getVertexCount() const {
    size_t count = 0;
    const auto& selfMeshes = this->meshes;  // explicitly use member variable
    for (const auto& meshPtr : selfMeshes) {
        if (meshPtr)
            count += meshPtr->getVertices();
    }
    return count;
}


//Iterate through each axis
  //Build a 2D mask for each axis
  //Detect Largest rectangles in the mask
  //Convert Rectangle into quad
  //Add quad to mesh
  //repeat for the entire mask
  //Create final mesh



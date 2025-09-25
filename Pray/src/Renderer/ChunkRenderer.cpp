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

            glm::vec2 uv0, uv1, uv2, uv3;

            // Compute UVs depending on face orientation
            if (normal.x == 1) {         // +X
                uv0 = glm::vec2(0, 0);
                uv1 = glm::vec2(0, height);
                uv2 = glm::vec2(width, height);
                uv3 = glm::vec2(width, 0);
            }
            else if (normal.x == -1) {   // -X
                uv0 = glm::vec2(0, 0);
                uv1 = glm::vec2(width, 0);
                uv2 = glm::vec2(width, height);
                uv3 = glm::vec2(0, height);
            }
            else if (normal.y == 1) {    // +Y
                uv0 = glm::vec2(0, 0);
                uv1 = glm::vec2(width, 0);
                uv2 = glm::vec2(width, height);
                uv3 = glm::vec2(0, height);
            }
            else if (normal.y == -1) {   // -Y
                uv0 = glm::vec2(0, 0);
                uv1 = glm::vec2(0, height);
                uv2 = glm::vec2(width, height);
                uv3 = glm::vec2(width, 0);
            }
            else if (normal.z == 1) {    // +Z
                uv0 = glm::vec2(0, 0);
                uv1 = glm::vec2(width, 0);
                uv2 = glm::vec2(width, height);
                uv3 = glm::vec2(0, height);
            }
            else if (normal.z == -1) {   // -Z
                uv0 = glm::vec2(0, 0);
                uv1 = glm::vec2(0, height);
                uv2 = glm::vec2(width, height);
                uv3 = glm::vec2(width, 0);
            }

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


    // Greedy meshing per face direction
    auto greedyPlane = [&](glm::vec3 normal) {
        // Choose plane orientation
        int u1, u2, fixedAxis;
        if (normal.x != 0) { fixedAxis = 0; u1 = 1; u2 = 2; }
        else if (normal.y != 0) { fixedAxis = 1; u1 = 0; u2 = 2; }
        else { fixedAxis = 2; u1 = 0; u2 = 1; }

        int sizeFixed = (fixedAxis == 0) ? CHUNK_SIZE : (fixedAxis == 1) ? CHUNK_HEIGHT : CHUNK_SIZE;
        int sizeU1 = (u1 == 0) ? CHUNK_SIZE : (u1 == 1) ? CHUNK_HEIGHT : CHUNK_SIZE;
        int sizeU2 = (u2 == 0) ? CHUNK_SIZE : (u2 == 1) ? CHUNK_HEIGHT : CHUNK_SIZE;

        std::vector<CubeType> mask(sizeU1 * sizeU2);
        std::vector<bool> processed(sizeU1 * sizeU2);

        for (int d = 0; d < sizeFixed; ++d) {
            // Build mask for this slice
            for (int i = 0; i < sizeU1; ++i) {
                for (int j = 0; j < sizeU2; ++j) {
                    int x = (fixedAxis == 0) ? d : (u1 == 0 ? i : j);
                    int y = (fixedAxis == 1) ? d : (u1 == 1 ? i : j);
                    int z = (fixedAxis == 2) ? d : (u1 == 2 ? i : j);

                    Cube& cube = cubes[x][y][z];

                    // Neighbor check (same as BuildMeshes)
                    int nx = x + (int)normal.x;
                    int ny = y + (int)normal.y;
                    int nz = z + (int)normal.z;
                    bool neighborExists = false;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_HEIGHT &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        neighborExists = !cubes[nx][ny][nz].isAir();
                    }

                    CubeType t = CubeType_Air;
                    if (!cube.isAir() && !neighborExists)
                        t = cube.getType();

                    mask[i * sizeU2 + j] = t;
                    processed[i * sizeU2 + j] = false;
                }
            }

            // Greedy merge
            for (int i = 0; i < sizeU1; ++i) {
                for (int j = 0; j < sizeU2; ++j) {
                    if (mask[i * sizeU2 + j] == CubeType_Air || processed[i * sizeU2 + j])
                        continue;

                    CubeType type = mask[i * sizeU2 + j];

                    // Find width (along u1)
                    int width = 1;
                    while (i + width < sizeU1 &&
                        mask[(i + width) * sizeU2 + j] == type &&
                        !processed[(i + width) * sizeU2 + j]) {
                        width++;
                    }

                    // Find height (along u2)
                    int height = 1;
                    bool done = false;
                    while (j + height < sizeU2 && !done) {
                        for (int k = 0; k < width; ++k) {
                            if (mask[(i + k) * sizeU2 + (j + height)] != type ||
                                processed[(i + k) * sizeU2 + (j + height)]) {
                                done = true;
                                break;
                            }
                        }
                        if (!done) height++;
                    }

                    // Mark processed
                    for (int dx = 0; dx < width; ++dx)
                        for (int dy = 0; dy < height; ++dy)
                            processed[(i + dx) * sizeU2 + (j + dy)] = true;

                    // Build quad origin (base cube corner in world space)
                    glm::vec3 pos(0.0f);
                    pos[fixedAxis] = d;
                    pos[u1] = i;
                    pos[u2] = j;
                    pos *= cubeSize;
                    pos += chunkWorldOffset;

                    // world-space extents in each direction
                    float w = width * cubeSize;
                    float h = height * cubeSize;

                    glm::vec3 p0, p1, p2, p3;

                    // Use same winding/orientation as BuildMeshes, but scaled by width/height.
                    if (normal.x == 1) {       // +X face (fixed x at pos.x + cubeSize)
                        p0 = pos + glm::vec3(cubeSize, 0.0f, 0.0f);
                        p1 = p0 + glm::vec3(0.0f, 0.0f, h);
                        p2 = p1 + glm::vec3(0.0f, w, 0.0f);
                        p3 = p0 + glm::vec3(0.0f, w, 0.0f);
                    }
                    else if (normal.x == -1) { // -X face
                        p0 = pos;
                        p1 = p0 + glm::vec3(0.0f, w, 0.0f);
                        p2 = p1 + glm::vec3(0.0f, 0.0f, h);
                        p3 = p0 + glm::vec3(0.0f, 0.0f, h);
                    }
                    else if (normal.y == 1) {  // +Y face
                        p0 = pos + glm::vec3(0.0f, cubeSize, 0.0f);
                        p1 = p0 + glm::vec3(w, 0.0f, 0.0f);
                        p2 = p1 + glm::vec3(0.0f, 0.0f, h);
                        p3 = p0 + glm::vec3(0.0f, 0.0f, h);
                    }
                    else if (normal.y == -1) { // -Y face
                        p0 = pos;
                        p1 = p0 + glm::vec3(0.0f, 0.0f, h);
                        p2 = p1 + glm::vec3(w, 0.0f, 0.0f);
                        p3 = p0 + glm::vec3(w, 0.0f, 0.0f);
                    }
                    else if (normal.z == 1) {  // +Z face
                        p0 = pos + glm::vec3(0.0f, 0.0f, cubeSize);
                        p1 = p0 + glm::vec3(w, 0.0f, 0.0f);
                        p2 = p1 + glm::vec3(0.0f, h, 0.0f);
                        p3 = p0 + glm::vec3(0.0f, h, 0.0f);
                    }
                    else if (normal.z == -1) { // -Z face
                        p0 = pos;
                        p1 = p0 + glm::vec3(w, 0.0f, 0.0f);
                        p2 = p1 + glm::vec3(0.0f, h, 0.0f);
                        p3 = p0 + glm::vec3(0.0f, h, 0.0f);
                    }

                    // IMPORTANT: pass width/height (in cubes) for tiled UVs
                    addQuad(type, p0, p1, p2, p3, normal, width, height);
                }
            }
        }
        };

    // Build planes for all 6 directions
    greedyPlane(glm::vec3(1, 0, 0));
    greedyPlane(glm::vec3(-1, 0, 0));
    greedyPlane(glm::vec3(0, 1, 0));
    greedyPlane(glm::vec3(0, -1, 0));
    greedyPlane(glm::vec3(0, 0, 1));
    greedyPlane(glm::vec3(0, 0, -1));

    // Build final meshes per CubeType (just like BuildMeshes)
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



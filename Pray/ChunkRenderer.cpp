#include "ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Chunk* chunk, Shader* shader)
    : chunk(chunk), shader(shader)
{
    BuildMeshes(); // generate meshes when constructed
}

void ChunkRenderer::BuildMeshes() {
    meshes.clear();

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures; // add textures if needed

    Cube*** cubes = chunk->getCubes();
    int CHUNK_SIZE = Chunk::CHUNK_SIZE;
    GLuint indexCount = 0;


    //initial naive implementation to generate mesh data
    //will use greedy algorithm later to reduce vertices
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                Cube& cube = cubes[x][y][z];  // Cube is a value
                if (cube.isAir()) continue;

                glm::vec3 pos(x, y, z);

                auto addFace = [&](const glm::vec3& offset, const glm::vec3& normal) {
                    int nx = x + (int)offset.x;
                    int ny = y + (int)offset.y;
                    int nz = z + (int)offset.z;

                    bool neighborExists = nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_SIZE &&
                        nz >= 0 && nz < CHUNK_SIZE &&
                        !cubes[nx][ny][nz].isAir();

                    if (!neighborExists) {
                        // 4 vertices per face
                        glm::vec3 right = glm::vec3(normal.y, normal.z, normal.x); // simple example
                        glm::vec3 up = glm::cross(normal, right);

                        
                        glm::vec3 color(1.0f, 1.0f, 1.0f); // default white color
                        vertices.push_back(Vertex{ pos + right + up, normal, color, glm::vec2(1, 1) });
                        vertices.push_back(Vertex{ pos + right,       normal, color, glm::vec2(1, 0) });
                        vertices.push_back(Vertex{ pos + up,          normal, color, glm::vec2(0, 1) });
                        vertices.push_back(Vertex{ pos,               normal, color, glm::vec2(0, 0) });

                        indices.push_back(indexCount);
                        indices.push_back(indexCount + 1);
                        indices.push_back(indexCount + 2);
                        indices.push_back(indexCount);
                        indices.push_back(indexCount + 2);
                        indices.push_back(indexCount + 3);

                        indexCount += 4;
                    }
                    };

                addFace(glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));
                addFace(glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0));
                addFace(glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
                addFace(glm::vec3(0, -1, 0), glm::vec3(0, -1, 0));
                addFace(glm::vec3(0, 0, 1), glm::vec3(0, 0, 1));
                addFace(glm::vec3(0, 0, -1), glm::vec3(0, 0, -1));
            }
        }
    }

    if (!vertices.empty()) {
        meshes.push_back(std::make_unique<Mesh>(vertices, indices, textures));
    }
}

void ChunkRenderer::Draw(MeshRenderer& meshRenderer) {

    if (!shader) return;

    // Replace with your actual camera matrices
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    for (auto& mesh : meshes) {
        meshRenderer.setMesh(*mesh);   // Update the mesh in the renderer if you have a setter
        meshRenderer.drawMesh(*shader, view, projection);
    }

}

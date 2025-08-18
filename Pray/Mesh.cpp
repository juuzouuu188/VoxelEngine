#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
    : vertices(vertices), indices(indices), textures(textures), ebo(indices)
{
    vao.Bind();

    VBO vbo(vertices);
//    EBO ebo(indices);

    vao.LinkVBO(vbo, 0, 3, sizeof(Vertex), (void*)0); // position
    vao.LinkVBO(vbo, 1, 3, sizeof(Vertex), (void*)offsetof(Vertex, normal)); // normal
    vao.LinkVBO(vbo, 2, 2, sizeof(Vertex), (void*)offsetof(Vertex, texUV)); // UV

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

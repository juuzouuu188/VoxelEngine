#include "Renderer/Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture*>& textures)
    : vertices(vertices), indices(indices), textures(textures),
    vbo(vertices),  // initialize member VBO
    ebo(indices)    // initialize member EBO
{
    vao.Bind();
    vbo.Bind();  // ensure VBO is bound before linking


    //layout(location = 0) in vec3 aPos;      // Vertex position
    //layout(location = 1) in vec3 aNormal;   // Normal
    //layout(location = 2) in vec3 aColor;    // Vertex color
    //layout(location = 3) in vec2 aTexCoord; // Texture coordinates

    // Link the member VBO to VAO
    vao.LinkVBO(vbo, 0, 3, sizeof(Vertex), (void*)0); // position
    vao.LinkVBO(vbo, 1, 3, sizeof(Vertex), (void*)offsetof(Vertex, normal)); // normal

    //vao.LinkVBO(vbo, 2, 2, sizeof(Vertex), (void*)offsetof(Vertex, texUV)); // UV
   //these two were the culprit for my texture mapping issues
    vao.LinkVBO(vbo, 2, 3, sizeof(Vertex), (void*)offsetof(Vertex, color));     // aColor
    vao.LinkVBO(vbo, 3, 2, sizeof(Vertex), (void*)offsetof(Vertex, texUV));     // aTexCoord

    ebo.Bind();  // bind EBO to VAO

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}


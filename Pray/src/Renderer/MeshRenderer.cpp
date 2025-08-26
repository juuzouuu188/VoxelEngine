#include "Renderer/MeshRenderer.h"


void MeshRenderer::setPosition(const glm::vec3& pos) {
    position = pos;
}

void MeshRenderer::setScale(const glm::vec3& scl) {
    scale = scl;
}

void MeshRenderer::setRotation(float angle, const glm::vec3& axis) {
    rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

void MeshRenderer::drawMesh(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    if (!mesh) return; // nothing to draw
    shader.Activate();

    // Build model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = model * rotation;
    model = glm::scale(model, scale);

    // Pass matrices to shader
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

   
    //This is where we handle VAO, textures, and glDraw
    mesh->vao.Bind();
    mesh->ebo.Bind();


    // bind textures if you’re using them
    for (size_t i = 0; i < mesh->textures.size(); i++) {
        if (!mesh->textures[i]) continue; // skip null textures
        mesh->textures[i]->Bind(i);  // note the pointer
       // mesh->textures[0]->Bind();  
        //might adjust this to string for later
        shader.setInt("texture" + std::to_string(i), (int)i);
       //shader.setInt("texture1", 0);
    }

    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

    mesh->vao.Unbind();
}
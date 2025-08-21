#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

//gonna have one mesh renderer handle everything
class MeshRenderer {

public:
	MeshRenderer() = default; // no mesh initially

	// Transform setters
	void setMesh(Mesh& mesh) { this->mesh = &mesh; } // store pointer
	void setPosition(const glm::vec3& pos);
	void setScale(const glm::vec3& scl);
	void setRotation(float angle, const glm::vec3& axis);

	// Draws the mesh with its transform applied
	//these are supplied by the camera
	void drawMesh(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
	Mesh* mesh = nullptr;   // store a pointer instead of reference
	glm::vec3 position{ 0.0f };
	glm::vec3 scale{ 1.0f };
	glm::mat4 rotation{ 1.0f };

};
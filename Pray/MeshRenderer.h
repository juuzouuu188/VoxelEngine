#pragma once
#include "Mesh.h"
#include "Shader.h"

class MeshRenderer {

public:
	MeshRenderer(Mesh& mesh);

	void setPosition();
	void setScale();
	void setRotation();

	void draw();

private:
	Mesh& mesh;
	glm::vec3 position{ 0.0f };
	glm::vec3 scale{ 1.0f };
	glm::mat4 rotation{ 1.0f };

};
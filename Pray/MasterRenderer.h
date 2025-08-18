#pragma once
#ifndef MASTERRENDERER_H
#define MASTERRENDERER_H
#include <glm/glm.hpp>
#include <vector>
#include "Cube.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class MasterRenderer {

public:
	MasterRenderer();
	~MasterRenderer();

	void drawCube(const Cube& cube, Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
	VAO vao;
	VBO* vbo; //point since we initliase after data
	EBO* ebo;

	void initCube();
};

#endif

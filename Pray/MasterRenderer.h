#pragma once
#ifndef MASTERRENDERER_H
#define MASTERRENDERER_H
#include "Cube.h"
#include "Chunk.h"
#include "Shader.h"
#include "Mesh.h"
#include <glm/glm.hpp>

//major rehaul coming soon
//moving the fuunctions to the mesh class, to abstract the rendering process

class MasterRenderer {

public:
	MasterRenderer();
	~MasterRenderer();

	// Draw a single cube
	//when drawing a cube, we need to pass the position
	void drawCube(const glm::vec3& position, CubeType type, Shader& shader, const glm::mat4& view, const glm::mat4& projection);

	// Draw all cubes in a chunk
	void drawChunk(const Chunk& chunk, const glm::vec3& chunkPos, Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
	//VAO vao;
	//VBO* vbo; //point since we initliase after data
	//EBO* ebo;

	void initCube();
};

#endif

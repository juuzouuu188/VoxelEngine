#pragma once
#ifndef CUBE_H
#define CUBE_H
#include <glm/glm.hpp>

//only responsible for this cube is to hold its position and its textureId
class Cube {
public:
	Cube(glm::vec3 position, int textureId);

	glm::vec3 getPosition() const { return position; }
	int getTextureId() const { return textureId; }

private:
	glm::vec3 position;
	int textureId; 
};

#endif
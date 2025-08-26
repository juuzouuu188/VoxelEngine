#pragma once
#pragma once
#ifndef CHUNK_H
#define CHUNK_H

#include "Cube.h"
#include <glm/glm.hpp>

class Chunk {

public:
	Chunk();
	~Chunk();

	bool isDirty;
	void Update(float dt);
	bool checkDirty() const { return isDirty; }
	void clearDirty() { isDirty = false; }

	static const int CHUNK_SIZE = 16;

	// Just data access
	Cube*** getCubes() const { return m_pCubes; }

	//debugging purposes
	void setUpSphere();
	static constexpr float CUBE_SIZE = 0.5; // adjust for mini cubes

private: // The Cubes data
	Cube*** m_pCubes;

};

#endif

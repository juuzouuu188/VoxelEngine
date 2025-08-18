#include "Chunk.h"

Chunk::Chunk() { //create the blocks
	m_pCubes = new Cube **[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++ ) {
		m_pCubes[i] = new Cube * [CHUNK_SIZE];
		for (int j = 0; j < CHUNK_SIZE; j++) {
			m_pCubes[i][j] = new Cube[CHUNK_SIZE];
		}
	}
}

Chunk::~Chunk() { // Delete the blocks
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			delete[] m_pCubes[i][j];
		}
		delete[] m_pCubes[i];
	}
	delete[] m_pCubes;
}

void Chunk::Update(float dt) {
	//in future this would hold logic for updating the chunk, like generating terrain or handling block changes
};
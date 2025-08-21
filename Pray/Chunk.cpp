#include "Chunk.h"

Chunk::Chunk() { //create the blocks
	isDirty = false;
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

void Chunk::setUpSphere() {
    int center = CHUNK_SIZE / 2;
    float radius = CHUNK_SIZE / 2.0f;

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                float dx = x - center;
                float dy = y - center;
                float dz = z - center;

                // Distance from the center
                float distance = sqrt(dx * dx + dy * dy + dz * dz);

                if (distance <= radius) {
                    m_pCubes[x][y][z].setActive(true);

                    // 3D checkerboard pattern
                    if ((x % 2 + y % 2 + z % 2) % 2 == 0) {
                        m_pCubes[x][y][z].setType(CubeType_Grass);
                    }
                    else {
                        m_pCubes[x][y][z].setType(CubeType_Sand);
                    }
                }
                else {
                    m_pCubes[x][y][z].setActive(false);
                }
            }
        }
    }

    isDirty = true; // Mark chunk as needing mesh rebuild
}


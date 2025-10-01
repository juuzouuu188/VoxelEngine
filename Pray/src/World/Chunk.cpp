#include "World/Chunk.h"
#include <iostream>

// New constructor that takes coordinates
Chunk::Chunk(const ChunkCoords& coords)
    : coords(coords),          // store world position
    isLoaded(false),
    _isSetUp(false),
    isDirty(false),
    rebuildNeeded(true),
    m_pCubes(nullptr)
{
    // Memory allocation is done in Load()
}

Chunk::~Chunk() {
    UnLoad();
}

void Chunk::UnLoad() {
    if (m_pCubes) {
        for (int i = 0; i < CHUNK_SIZE; ++i) {
            for (int j = 0; j < CHUNK_HEIGHT; ++j) {
                delete[] m_pCubes[i][j];
            }
            delete[] m_pCubes[i];
        }
        delete[] m_pCubes;
        m_pCubes = nullptr;
    }


    // Reset state flags
    isLoaded = false;
    _isSetUp = false;
    isDirty = false;
    rebuildNeeded = true;
}


void Chunk::Update(float dt) {
	//in future this would hold logic for updating the chunk, like generating terrain or handling block changes
};

Perlin noise(0.025f, 4, 0.3f, 0.3f);

void Chunk::setUp() {
   
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            float worldX = coords.x * CHUNK_SIZE + x;
            float worldZ = coords.z * CHUNK_SIZE + z;

            float height = noise.get(worldX, worldZ);
            height = (height + 1.0f) * 0.5f;
            int maxHeight = (int)(height * CHUNK_HEIGHT*0.25);
            std::cout << maxHeight << std::endl;

           
            for (int y = 0; y < CHUNK_HEIGHT; ++y) {  // iterate all y
                if (y <= maxHeight) {
                    m_pCubes[x][y][z].setActive(true);

                    // Checker pattern
                    if ( y <= 20)
                        m_pCubes[x][y][z].setType(CubeType_Sand);
                    else
                        m_pCubes[x][y][z].setType(CubeType_Grass);
                }
                else {
                    m_pCubes[x][y][z].setActive(false); // leave as air
                }
            }

        }
    }

    rebuildNeeded = true; // Mark chunk as needing mesh rebuild
    _isSetUp = true; // Mark chunk as set up
}

void Chunk::setUpSphere() {
    int center = CHUNK_SIZE / 2;
    float radius = (CHUNK_SIZE / 2.0f) * CUBE_SIZE; // scale radius by cube size

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                // Scale positions by cube size
                float dx = (x - center) * CUBE_SIZE;
                float dy = (y - center) * CUBE_SIZE;
                float dz = (z - center) * CUBE_SIZE;

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

    rebuildNeeded = true; // Mark chunk as needing mesh rebuild
    _isSetUp = true; // Mark chunk as set up
}


void Chunk::Load() {

    if (isLoaded) return;

    // Allocate cubes only if not already allocated
    if (!m_pCubes) {
        m_pCubes = new Cube * *[CHUNK_SIZE];
        for (int i = 0; i < CHUNK_SIZE; i++) {
            m_pCubes[i] = new Cube * [CHUNK_HEIGHT];
            for (int j = 0; j < CHUNK_HEIGHT; j++) {
                m_pCubes[i][j] = new Cube[CHUNK_SIZE];
            }
        }
    }

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for(int y = 0; y < CHUNK_HEIGHT; ++y)
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            m_pCubes[x][y][z].setActive(false);
        }
    }

    // Optionally initialize cubes with terrain / sphere / default blocks
    // setUpSphere(); or some procedural generation

    isLoaded = true;
};

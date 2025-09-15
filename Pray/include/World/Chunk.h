#pragma once
#pragma once
#ifndef CHUNK_H
#define CHUNK_H

#include "Cube.h"
#include "WorldCoords.h"


//just for testing
#include "Perlin.h"
#include <glm/glm.hpp>

class Chunk {

public:
	Chunk(const ChunkCoords& coords);
	~Chunk()
        ;
    // --- Lifecycle ---
    void Load();          // Allocates cubes & initializes data
    void setUp();         // Finishes setup (lighting, neighbors, etc.)
    bool isSetUp() const { return _isSetUp; }
    void UnLoad(); 

    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 256;
    static constexpr float CUBE_SIZE = 1.0f; // adjust for mini cubes

    // --- Data access ---
    Cube*** getCubes() const { return m_pCubes; }
    bool IsLoaded() const { return isLoaded; }
    void setLoaded(bool loaded) { isLoaded = loaded; }

    // --- Coordinates ---
    const ChunkCoords& getCoords() const { return coords; }

    // --- State flags ---
    bool checkDirty() const { return isDirty; }
    void setDirty() { isDirty = true; }
    void clearDirty() { isDirty = false; }

    bool needsRebuild() const { return rebuildNeeded; }
    void setNeedsRebuild(bool rebuild) { rebuildNeeded = rebuild; }

    // --- Logic ---
    void Update(float dt);

    // --- Debugging ---
    void setUpSphere();


 private: 
    // The Cubes data
	Cube*** m_pCubes;
    ChunkCoords coords;   // Position in the world grid

    bool isLoaded;
	bool _isSetUp;
	bool isDirty;
    bool rebuildNeeded;
};

#endif

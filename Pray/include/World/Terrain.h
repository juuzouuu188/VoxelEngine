#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H
#include "CHUNK.h"
#include "Perlin.h"
#include <vector>
#include <memory>

//this should own a perlin noise generator and generate heightmaps for chunks
//may handle biomes later

class Terrain {

public:
	Terrain(unsigned int seed = 1337);
	~Terrain();


	// Generates voxel data for a given chunk
	void GenerateChunkData(std::shared_ptr<Chunk> chunk);

private:
	//Perlin perlinNoise;
	unsigned int seed;
};

#endif

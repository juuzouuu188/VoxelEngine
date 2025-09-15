#pragma once
#ifndef PERLIN_H
#define PERLIN_H
#include "stb/stb_perlin.h"

//height map generator
class Perlin {

public:
	//can play around with these values
	Perlin(float frequency , int octaves , float lacunarity , float gain);
	~Perlin();

	//Basic Noise
	float get(float x, float y, float z = 0.0f) const;

	//Ridge Noise (mountain-like)
	float getRidge(float x, float y, float z = 0.0f) const;

	float getTurbulence(float x, float y, float z = 0.0f) const;

	//Config Setters
	void setFrequency(float frequency) { this->frequency = frequency; }
	void setOctaves(int octaves) { this->octaves = octaves; }
	void setLacunarity(float lacunarity) { this->lacunarity = lacunarity; }
	void setGain(float gain) { this->gain = gain; }


private:
	// Typical values to start playing with:
	//     octaves    =   6     -- number of "octaves" of noise3() to sum
	//     lacunarity = ~ 2.0   -- spacing between successive octaves (use exactly 2.0 for wrapping output)
	//     gain       =   0.5   -- relative weighting applied to each successive octave
	//     offset     =   1.0?  -- used to invert the ridges, may need to be larger, not sure
	float frequency;
	int octaves;
	float lacunarity;
	float gain;

};

#endif

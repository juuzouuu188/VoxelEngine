#include "World/Perlin.h"

Perlin::Perlin(float frequency, int octaves, float lacunarity, float gain) :
	frequency(frequency), octaves(octaves), lacunarity(lacunarity), gain(gain) {

}

Perlin::~Perlin() {

}

float Perlin::get(float x, float y, float z) const {
	return stb_perlin_fbm_noise3(x * frequency, y * frequency, z * frequency, lacunarity, gain, octaves);
}

float Perlin::getRidge(float x, float y, float z) const {
	return stb_perlin_ridge_noise3(x * frequency, y * frequency, z * frequency, lacunarity, gain, 
		1.0f , //offset, tweak later
		octaves);
}

float Perlin::getTurbulence(float x, float y, float z) const {
	return stb_perlin_turbulence_noise3(x * frequency, y * frequency, z * frequency, lacunarity, gain, octaves);
}
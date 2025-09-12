#pragma once
#include "World/Chunk.h"
#include <vector>
#include <memory>
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"   // for Texture
#include <unordered_map> // for std::unordered_map

//job is to store the a reference to the chunk data and render it
//handle drawing all meshes in the chunk 
//possible handle chunk updates in future, ie placing a block or removing a block 

//need to pass in chunk cooords in position relatuve to world origin.

class ChunkRenderer {
public:
	
	ChunkRenderer(Chunk* chunk,Shader* shader);


	// Map from CubeType to Texture, using unique_ptr for automatic memory management
	static std::unordered_map<CubeType, std::unique_ptr<Texture>> cubeTextures;

	static void LoadCubeTextures() {
		if (!cubeTextures.empty()) return; // already loaded
		//ensure Enum type matches the correct number of channels in the texture
		cubeTextures[CubeType_Grass] = std::make_unique<Texture>("Textures/grass.jpg", "diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
		cubeTextures[CubeType_Dirt] = std::make_unique<Texture>("Textures/dirt.png", "diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		cubeTextures[CubeType_Stone] = std::make_unique<Texture>("Textures/stone.png", "diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		cubeTextures[CubeType_Wood] = std::make_unique<Texture>("Textures/wood.png", "diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		cubeTextures[CubeType_Sand] = std::make_unique<Texture>("Textures/sand.jpeg", "diffuse", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	}


	// Draws the chunk using the shared mesh renderer
	void Draw(MeshRenderer& meshRenderer,const glm::mat4& view, const glm::mat4& projection);
	void BuildMeshes();
	Chunk* getChunk() const { return chunk;  }

private:
	//private cause each chunk renderer handles one chunk
	Chunk* chunk;
	Shader* shader;


	std::vector<std::unique_ptr<Mesh>> meshes;  // Meshes that make up this chunk

};
#pragma once
#include "Chunk.h"
#include <vector>
#include <memory>
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Shader.h"

//job is to store the a reference to the chunk data and render it
//handle drawing all meshes in the chunk 
//possible handle chunk updates in future, ie placing a block or removing a block etc


class ChunkRenderer {
public:
	
	ChunkRenderer(Chunk* chunk,Shader* shader);
	
	// Draws the chunk using the shared mesh renderer
	void Draw(MeshRenderer& meshRenderer);

private:
	//private cause each chunk renderer handles one chunk
	Chunk* chunk;
	Shader* shader;

	std::vector<std::unique_ptr<Mesh>> meshes;  // Meshes that make up this chunk

	void BuildMeshes();
};
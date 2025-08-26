#pragma once
#ifndef MESH_H
#define MESH_H

#include<string>
#include<vector>

#include "VAO.h"
#include "VBO.h"
#include "Renderer/EBO.h"
#include "World/Cube.h"
#include "World/Chunk.h"
#include "Vertex.h"
#include "Texture.h"
#include <cstddef>


//not including camera as th player owns it
//just a data class

class Mesh {
public:
	//vectors as dynamic arrays and memory allocation
	//Vertex Struct to seperate the data
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture*> textures; //changed to points for memory 

	VAO vao;
	VBO vbo;
	EBO ebo;

	// Initializes the mesh
	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture*>& textures);

	//support other advanced features later
	
};


#endif
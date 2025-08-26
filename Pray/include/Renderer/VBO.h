#ifndef VBO_H
#define VBO_H
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Vertex.h"
#include <vector>

class VBO {
public:
	GLuint ID;
	VBO(std::vector<Vertex>& vertices);

	void Bind();
	void Unbind();
	void Delete();

};


#endif

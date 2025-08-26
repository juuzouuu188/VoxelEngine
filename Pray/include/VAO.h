#ifndef VAO_H
#define VAO_H
#include <glad/glad.h>
#include "VBO.h"

class VAO {
public:
	GLuint ID;
	VAO();

	void LinkVBO(VBO& vbo, GLuint layout, GLint size, GLsizei stride, const void* offset);
	void Bind();
	void Unbind();
	void Delete();
};

#endif
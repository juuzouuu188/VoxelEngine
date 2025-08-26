#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"Shader.h"

class Texture
{
public:
	std::string type; // "diffuse", "specular", etc.
	GLuint ID;
	GLenum texType;
	
	
	Texture(const char* image, const std::string& typeName, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind(GLuint slot = 0) const;
	// Unbinds a texture
	void Unbind() const;
	// Deletes a texture
	void Delete();
};
#endif
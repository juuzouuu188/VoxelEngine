#version 330 core
layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec3 aNormal;   // Normal
layout(location = 2) in vec3 aColor;    // Vertex color
layout(location = 3) in vec2 aTexCoord; // Texture coordinates

out vec3 FragColor;   // Pass per-vertex color to fragment shader
out vec2 TexCoord;    // Pass texture coords to fragment shader
out vec3 Normal;      // Pass normal to fragment shader
out vec3 FragPos;     // World position for lighting

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    FragColor = aColor;
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
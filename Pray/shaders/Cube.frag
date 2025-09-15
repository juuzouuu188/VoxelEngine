#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
  FragColor = texture(texture1, TexCoord);
  //this is for debugging
  //  FragColor = vec4(texture1, 0.0, 1.0); // show UV coords as red/green
}

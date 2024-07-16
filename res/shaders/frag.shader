#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords; // the input variable from the vertex shader (same name and same type)  

uniform sampler2D u_texture;

void main()
{
    FragColor = texture(u_texture, TexCoords);
} 
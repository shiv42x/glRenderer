#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0

out vec4 vertexColor; 

uniform vec4 color;

void main()
{
    gl_Position = vec4(aPos, 1.0); 
    vertexColor = color;
}
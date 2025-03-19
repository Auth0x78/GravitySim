#version 440 core

layout (location = 0) out vec4 color;
in vec3 fragCol;

void main()
{
	
	color = vec4(fragCol, 1.0);
}
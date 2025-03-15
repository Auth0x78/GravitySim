#version 440 core

layout (location = 0) out vec4 color;
in vec3 fragUV;

void main()
{
	
	color = vec4(fragUV,1.0);

}
#version 440 core

layout (location = 0) in vec3 p;
out vec3 fragCol;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 material;

void main()
{
	fragCol = material;
	gl_Position = projection * view * model * vec4(p, 1.0);
	
	gl_PointSize = 25.0;
}
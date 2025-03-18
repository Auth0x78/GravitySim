#version 440 core

layout (location = 0) in vec3 p;
out vec4 fragUV;

uniform mat4 view;
uniform mat4 projection;
//uniform mat4 model;

void main()
{
	fragUV = normalize(projection * view * vec4(p, 1.0));
	gl_Position = projection * view * vec4(p, 1.0);
	
	gl_PointSize = 50.0;
}
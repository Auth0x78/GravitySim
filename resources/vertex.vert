#version 440 core

layout (location = 0) in vec3 p;
out vec3 fragUV;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragUV = p + vec3(0.1);
	gl_Position = projection * view * vec4(p, 1.0);
	
	gl_PointSize = 10.0;
}
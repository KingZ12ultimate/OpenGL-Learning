#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 instanceModelMatrix;
layout (location = 7) in vec3 aColor;

out vec3 Color;

layout(std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

void main()
{
	gl_Position = projection * view * instanceModelMatrix * vec4(aPos, 1.0);
	Color = aColor;
}
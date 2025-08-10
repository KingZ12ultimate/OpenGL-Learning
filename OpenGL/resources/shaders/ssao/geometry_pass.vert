#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} vs_out;

layout(std140, binding = 0) uniform Matrices {
	mat4 projection;
	mat4 view;
	mat4 model;
};

uniform bool invertNormals = false;

void main()
{
	float s = invertNormals ? -1.0 : 1.0;
	vec4 viewPos = view * model * vec4(aPos, 1.0);
	vs_out.FragPos = viewPos.xyz;
	vs_out.Normal = transpose(inverse(mat3(view * model))) * (s * aNormal);
	vs_out.TexCoords = aTexCoords;
	gl_Position = projection * viewPos;
}

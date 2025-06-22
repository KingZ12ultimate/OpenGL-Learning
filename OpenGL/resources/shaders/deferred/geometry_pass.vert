#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instancedModelMatrix;

out VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} vs_out;

layout(std140, binding = 0) uniform Matrices {
	mat4 projection;
	mat4 view;
};

void main()
{
	vs_out.FragPos = vec3(instancedModelMatrix * vec4(aPos, 1.0));
	vs_out.Normal = transpose(inverse(mat3(instancedModelMatrix))) * aNormal;
	vs_out.TexCoords = aTexCoords;
	gl_Position = projection * view * instancedModelMatrix * vec4(aPos, 1.0);
}

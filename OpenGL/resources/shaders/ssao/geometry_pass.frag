#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} fs_in;

struct Material {
	sampler2D texture_diffuse[1];
	sampler2D texture_specular[1];
};
uniform Material material;
uniform bool hasMaterial;

void main()
{
	gPosition = fs_in.FragPos;
	gNormal = normalize(fs_in.Normal);
	gAlbedoSpec.rgb = hasMaterial ? texture(material.texture_diffuse[0], fs_in.TexCoords).rgb : vec3(0.95);
	gAlbedoSpec.a = hasMaterial ? texture(material.texture_specular[0], fs_in.TexCoords).r : 1.0;
}

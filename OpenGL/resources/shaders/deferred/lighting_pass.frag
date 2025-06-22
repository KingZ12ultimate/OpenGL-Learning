#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
	vec3 position;
	vec3 color;
};

const int NUM_LIGHTS = 64;
uniform Light lights[NUM_LIGHTS];
uniform vec3 viewPos;
uniform float ambient = 0.1;

void main()
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	vec3 lighting = ambient * Albedo;
	vec3 viewDir = normalize(viewPos - FragPos);
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		vec3 lightDir = normalize(lights[i].position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].color;
		float attenuation = 8.0 / dot(lights[i].position - FragPos, lights[i].position - FragPos);
		lighting += diffuse * attenuation;
	}

	FragColor = vec4(lighting, 1.0);
}

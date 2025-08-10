#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

struct Light
{
	vec3 position;
	vec3 color;

	float linear;
	float quadratic;
};
uniform Light light;

// All calculations are done in View-Space;
void main()
{
	vec3 fragPos = texture(gPosition, TexCoords).xyz;
	vec3 normal = texture(gNormal, TexCoords).xyz;
	vec3 albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float specTexVal = texture(gAlbedoSpec, TexCoords).a;
	float ambientOcclusion = texture(ssao, TexCoords).r;

	// ambient
	vec3 ambient = 0.3 * albedo * ambientOcclusion;
	vec3 viewDir = normalize(-fragPos);

	// diffuse
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * light.color;
	
	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
	vec3 specular = spec * specTexVal * light.color;

	// attenuation
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * dist * dist);
	
	vec3 lighting = ambient + (diffuse + specular) * attenuation;
	FragColor = vec4(lighting, 1.0);
}

#version 460 core
out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D normal_texture;
uniform vec3 lightPos;
uniform bool white;

void main()
{
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	vec3 lightColor = vec3(1.0);
	vec3 ambient = 0.1 * lightColor;
	
    vec3 normal = texture(normal_texture, fs_in.TexCoords).rgb;
	normal = 2.0 * normal - 1.0; // map to [-1, 1] range

	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float diff = 1.0 - max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;

	float distanceSquared = dot(lightPos - fs_in.FragPos, lightPos - fs_in.FragPos);
	float attenuation = 10.0 / distanceSquared;

	vec3 result = (ambient + diffuse + specular) * color * attenuation;
	FragColor = vec4(white ? vec3(1.0) : result, 1.0);

	// Gamma correction
	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
}

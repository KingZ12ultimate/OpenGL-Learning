#version 460 core

out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float nearPlane;
uniform float farPlane;

vec3 normal, lightDir;


float linearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // convert from [0, 1] to [-1, 1]
	return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

float pcf(vec2 texCoords, float bias, float currentDepth)
{
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			vec2 offset = vec2(x, y) * texelSize;
			float closestDepth = texture(shadowMap, texCoords + offset).r;
			shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
		}
	}
	return shadow / 9.0; // average the results
}

float CalculateShadow(vec4 fragPosLightSpace)
{
	// perform perspective division for perspective projection matrices
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5; // transform to [0, 1] range
	float currentDepth = projCoords.z;
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float shadow = projCoords.z > 1.0 ? 0.0 : pcf(projCoords.xy, bias, currentDepth);

	return shadow;
}

void main()
{
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	normal = normalize(fs_in.Normal);
	vec3 lightColor = vec3(0.3);
	
	// ambient
	vec3 ambient = 0.15 * lightColor;
	
	// diffuse
	lightDir = normalize(lightPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;

	float shadow = CalculateShadow(fs_in.FragPosLightSpace);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

	FragColor = vec4(lighting, 1.0);

	// Gamma correction
	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
}

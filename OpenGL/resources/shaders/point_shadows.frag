#version 460 core

out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float farPlane;

vec3 normal, lightDir;

 vec3 sampleOffsetDirections[20] = vec3[]
 (
	vec3( 1, 1, 1), vec3( 1,-1, 1), vec3(-1,-1, 1), vec3(-1, 1, 1),
	vec3( 1, 1,-1), vec3( 1,-1,-1), vec3(-1,-1,-1), vec3(-1, 1,-1),
	vec3( 1, 1, 0), vec3( 1,-1, 0), vec3(-1,-1, 0), vec3(-1, 1, 0),
	vec3( 1, 0, 1), vec3(-1, 0, 1), vec3( 1, 0,-1), vec3(-1, 0,-1),
	vec3( 0, 1, 1), vec3( 0,-1, 1), vec3( 0,-1,-1), vec3( 0, 1,-1)
 );


float pcf(vec3 texCoords, float bias, float currentDepth)
{
	float shadow = 0.0;
	float samples = 4.0; // number of samples is actually samples^3
	float offset = 0.1;
	for (float x = -offset; x < offset; x += 2.0 * offset / samples)
	{
		for (float y = -offset; y < offset; y += 2.0 * offset / samples)
		{
			for (float z = -offset; z < offset; z += 2.0 * offset / samples)
			{
				float closestDepth = texture(shadowMap, texCoords + vec3(x, y, z)).r;
				closestDepth *= farPlane; // scale to [0, farPlane]
				if (currentDepth - bias > closestDepth)
					shadow += 1.0;
			}
		}
	}

	return shadow / pow(samples, 3.0); // average the results
}

float CalculateShadow(vec3 fragPos)
{
	vec3 fragToLight = fragPos - lightPos;
	float currentDepth = length(fragToLight);
	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0 + viewDistance / farPlane) / 25.0;
	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(shadowMap, fragToLight
		 + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= farPlane; // scale to [0, farPlane]
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}

	return shadow / float(samples); // average the results
}

void main()
{
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	normal = normalize(fs_in.Normal);
	vec3 lightColor = vec3(0.3);
	
	// ambient
	vec3 ambient = 0.15 * lightColor;
	
	// diffuse
	lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;

	float shadow = CalculateShadow(fs_in.FragPos);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

	// float closest = texture(shadowMap, -lightDir).r;
	FragColor = vec4(lighting, 1.0);

	// Gamma correction
	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
}

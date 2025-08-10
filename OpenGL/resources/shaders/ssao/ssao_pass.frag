#version 460 core

out float FragColor;

in vec2 TexCoords;

const int kernelSize = 64;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTex;
uniform vec3 samples[kernelSize];

layout(std140, binding = 0) uniform Matrices {
	mat4 projection;
	
	// not used in this shader but necessary for the UBO to work
	mat4 view;
	mat4 model;
};

uniform float radius = 0.5, bias = 0.025;
uniform int width, height;

const vec2 noiseScale = vec2(float(width) / 4.0, float(height) / 4.0);

void main()
{
	vec3 fragPos = texture(gPosition, TexCoords).xyz;
	vec3 normal = texture(gNormal, TexCoords).rgb;
	vec3 randomVec = texture(noiseTex, TexCoords * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < kernelSize; i++)
	{
		vec3 samplePos = TBN * samples[i];
		samplePos = fragPos + samplePos * radius;

		vec4 offset = vec4(samplePos, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture(gPosition, offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = occlusion;
}

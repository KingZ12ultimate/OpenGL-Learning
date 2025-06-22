#version 460 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D hdrBuffer;
uniform float exposure = 1.0;
uniform bool g;

void main()
{
	const float gamma = 2.2;

	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	hdrColor = g ? pow(hdrColor, vec3(gamma)) : hdrColor;

	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	mapped = g ? pow(mapped, vec3(1.0 / gamma)) : mapped;
	
	FragColor = vec4(mapped, 1.0);
}

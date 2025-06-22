#version 460 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure = 0.5;

void main()
{
	vec3 hdrColor = texture(scene, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
	hdrColor += bloomColor;
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

	const float gamma = 2.2;
	result = pow(result, vec3(1.0 / gamma));
	
	FragColor = vec4(result, 1.0);
}

#version 460 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

const float offest = 1.0 / 300.0;

void main()
{             
//	vec2 offsets[9] = vec2[](
//		vec2(-offest,  offest),  // top-left
//		vec2( 0.0f,    offest),  // top-center
//		vec2( offest,  offest),  // top-right
//		vec2(-offest,  0.0f),    // center-left
//		vec2( 0.0f,    0.0f),    // center-center
//		vec2( offest,  0.0f),    // center-right
//		vec2(-offest, -offest),  // bottom-left
//		vec2( 0.0f,   -offest),  // bottom-center
//		vec2( offest, -offest)   // bottom-right
//	);
//
//	float kernel[9] = float[](
//		1, 1, 1,
//		1, -8, 1,
//		1, 1, 1
//	);
//
//	vec3 sampleTex[9];
//	for (int i = 0; i < 9; i++)
//	{
//		sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
//	}
//	vec3 col = vec3(0.0);
//	for (int i = 0; i < 9; i++)
//	{
//		col += sampleTex[i] * kernel[i];
//	}

	FragColor = vec4(vec3(texture(screenTexture, TexCoords).r), 1.0);
}

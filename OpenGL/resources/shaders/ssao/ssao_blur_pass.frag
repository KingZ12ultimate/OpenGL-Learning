#version 460 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;
uniform int noiseTexDim;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
	float result = 0.0;
	int halfSize = noiseTexDim / 2;
	for (int x = -halfSize; x < halfSize; x++)
	{
		for (int y = -halfSize; y < halfSize; y++)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssaoInput, TexCoords + offset).r;
		}
	}
	FragColor = result / (noiseTexDim * noiseTexDim);
}

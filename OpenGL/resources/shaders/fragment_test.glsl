#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

float linearize(float depth);

float zNear = 0.1;
float zFar = 100.0;

void main()
{
//    float depth = linearize(gl_FragCoord.z);
//    FragColor = vec4(vec3(depth), 1.0);
    FragColor = texture(texture1, TexCoords);
}

float linearize(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

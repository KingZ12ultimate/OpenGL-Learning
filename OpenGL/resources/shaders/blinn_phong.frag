#version 460 core
out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main()
{
	vec3 color = texture(texture1, fs_in.TexCoords).rgb;
	vec3 ambient = 0.2 * color;
	
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * color;

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	float spec = 0.0;
	float shininess = 8.0;
	if (blinn)
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), shininess * 4.0);
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	}
	vec3 specular = spec * color;

	float distanceSquared = dot(lightPos - fs_in.FragPos, lightPos - fs_in.FragPos);
	float attenuation = 1.0 / distanceSquared;

	vec3 result = (ambient + diffuse + specular) * attenuation;
	FragColor = vec4(result, 1.0);

	// Gamma correction
	float gamma = 2.2;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
}

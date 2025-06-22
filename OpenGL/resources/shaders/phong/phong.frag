#version 460 core
out vec4 FragColor;

#define NUM_DIFFUSE_TEXTURES 4
#define NUM_SPECULAR_TEXTURES 2

struct Material {
    sampler2D diffuse_texture[NUM_DIFFUSE_TEXTURES];
    sampler2D specular_texture[NUM_SPECULAR_TEXTURES];
    sampler2D emissionMap;
    float shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NUM_POINT_LIGHTS 4

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
     vec3 result = CalcDirLight(dirLight, normal, viewDir);
    // phase 2: Point lights
    // for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    //    result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
    // phase 3: Spot light
    // result += CalcSpotLight(spotLight, normal, FragPos, viewDir);
    
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient, diffuse, specular;
    for (uint i = 0; i < NUM_DIFFUSE_TEXTURES; i++)
    {
        ambient += texture(material.diffuse_texture[i], TexCoords).rgb;
        diffuse += texture(material.diffuse_texture[i], TexCoords).rgb;
    }

    for (uint i = 0; i < NUM_SPECULAR_TEXTURES; i++)
    {
        specular += texture(material.specular_texture[i], TexCoords).rgb;
    }
    
    ambient *= light.ambient;
    diffuse *= light.diffuse * diff;
    specular *= light.specular * spec;

    return ambient + diffuse + specular;
}
//
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//    // diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//
//    // attenuation
//    float _distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * _distance + light.quadratic * (_distance * _distance));
//
//    vec3 ambient = light.ambient * texture(material.diffuseMap, TexCoords).rgb;
//    vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, TexCoords).rgb;
//    vec3 specular = light.specular * spec * texture(material.specularMap, TexCoords).rgb;
//
//    return (ambient + diffuse + specular) * attenuation;
//}
//
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//
//    float theta = dot(lightDir, normalize(-light.direction));
//    float epsilon = light.cutOff - light.outerCutOff;
//    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//
//    // diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//
//    // attenuation
//    float _distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * _distance + light.quadratic * (_distance * _distance));
//
//    vec3 ambient = light.ambient * texture(material.diffuseMap, TexCoords).rgb;
//    vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, TexCoords).rgb;
//    vec3 specular = light.specular * spec * texture(material.specularMap, TexCoords).rgb;
//
//    ambient *= attenuation;
//    return ambient + (diffuse + specular) * attenuation * intensity;
//}

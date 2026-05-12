#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};
#define POINT_LIGHT_COUNT 1 
uniform PointLight pointLights[POINT_LIGHT_COUNT];

struct Material {
    sampler2D normalMap;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D roughnessMap;
};
uniform Material material;

uniform vec3 viewPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 tiledUV) {
    vec3 lightDir = normalize(-light.direction);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);

    // Roughness/Shininess 계산
    float roughness = texture(material.roughnessMap, tiledUV).r;
    float shininess = (1.0 - roughness) * 128.0;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(shininess, 1.0));

    // 텍스처 샘플링 (반드시 tiledUV를 사용해야 합니다!)
    vec3 texDiffuse = vec3(texture(material.diffuse, tiledUV));
    vec3 texSpecular = vec3(texture(material.specular, tiledUV));

    // combine results
    vec3 ambient  = light.ambient  * texDiffuse;
    vec3 diffuse  = light.diffuse  * diff * texDiffuse;
    vec3 specular = light.specular * spec * texSpecular;
    
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTex, vec3 specTex, vec2 tiledUV) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular
    float roughness = texture(material.roughnessMap, tiledUV).r;
    float shininess = (1.0 - roughness) * 128.0;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    


    vec3 ambient = light.ambient * diffTex;
    vec3 diffuse = light.diffuse * diff * diffTex;
    vec3 specular = light.specular * spec * specTex;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

void main() {
    float scale = 20.0;
    vec2 tiledTexCoords = TexCoords * scale;

    vec3 normal = texture(material.normalMap, tiledTexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
    
    vec3 nnormal = normalize(TBN * normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 diffColor = vec3(texture(material.diffuse, tiledTexCoords));
    vec3 specColor = vec3(texture(material.specular, tiledTexCoords));

    vec3 result = calcDirLight(dirLight, nnormal, viewDir, tiledTexCoords);

    for(int i = 0; i < POINT_LIGHT_COUNT; i++) {
        result += calcPointLight(pointLights[i], nnormal, FragPos, viewDir, diffColor, specColor, tiledTexCoords); 
    }

    vec3 color = pow(result, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}
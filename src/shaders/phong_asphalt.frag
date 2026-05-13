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
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    sampler2D roughnessMap;

    bool useDiffuseMap;
    bool useSpecularMap;
    bool useRoughnessMap;
    bool useNormalMap;

    vec3  baseDiffuse;
    vec3  baseSpecular;
    float baseRoughness; 
};
uniform Material material;

uniform vec3 viewPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, vec3 specColor, float roughness) {
    vec3 lightDir = normalize(-light.direction);
    
    // 1. Diffuse shading (난반사)
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 2. Specular shading (정반사)
    vec3 reflectDir = reflect(-lightDir, normal);

    // Roughness(0~1)를 Shininess(1~128)로 변환
    float shininess = (1.0 - roughness) * 128.0;
    
    // 지수(spec) 계산 - shininess가 0이 되지 않도록 max(..., 1.0) 처리
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(shininess, 1.0));

    // 3. 결합 (Combine)
    // 인자로 받은 diffColor와 specColor를 사용합니다.
    vec3 ambient  = light.ambient  * diffColor;
    vec3 diffuse  = light.diffuse  * diff * diffColor;
    vec3 specular = light.specular * spec * specColor;
    
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTex, vec3 specTex, float roughness) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular
    float shininess = (1.0 - roughness) * 128.0;
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(shininess, 1.0));

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
    float scale = 1.0;
    vec2 tiledTexCoords = TexCoords * scale;

    // 1. Normal 계산
    vec3 nnormal = normalize(Normal);

    if(material.useNormalMap) {
        vec3 normal = texture(material.normalMap, tiledTexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);   
        nnormal = normalize(TBN * normal);
        
    }
    
    vec3 viewDir = normalize(viewPos - FragPos);

    // 2. Diffuse 색상 결정
    vec3 diffColor;
    if(material.useDiffuseMap)
        diffColor = texture(material.diffuseMap, tiledTexCoords).rgb;
    else
        diffColor = material.baseDiffuse;

    // 3. Specular 색상 결정
    vec3 specColor;
    if(material.useSpecularMap)
        specColor = texture(material.specularMap, tiledTexCoords).rgb;
    else
        specColor = material.baseSpecular;

    // 4. Roughness(광택도) 결정
    float roughness;
    if(material.useRoughnessMap)
        roughness = texture(material.roughnessMap, tiledTexCoords).r;
    else
        roughness = material.baseRoughness;

    // 5. 최종 조명 계산
    vec3 result = calcDirLight(dirLight, nnormal, viewDir, diffColor, specColor, roughness);

    // 포인트 라이트 루프
    for(int i = 0; i < POINT_LIGHT_COUNT; i++) {
        result += calcPointLight(pointLights[i], nnormal, FragPos, viewDir, diffColor, specColor, roughness); 
    }

    // 6. Gamma Correction (SRGB 변환)
    vec3 color = pow(result, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}
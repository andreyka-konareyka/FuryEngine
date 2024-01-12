#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D textureDiffuse;
    sampler2D textureSpecular;
	sampler2D textureNormal;
    float shininess;
	
	int diffuseTextureEnabled;
	int specularTextureEnabled;
	int normalTextureEnabled;
	
	vec3 diffuseColor;
	vec3 specularColor;
	float opacity;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
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

#define NR_POINT_LIGHTS 4

in VS_out{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
	
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

uniform sampler2D shadowMap;


// function prototypes
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 newDirLight);
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);


vec4 getDiffuse(vec2 texCoords)
{
	if (material.diffuseTextureEnabled == 0)
	{
		return vec4(material.diffuseColor, material.opacity);
	}
	
	return texture(material.textureDiffuse, texCoords);
}
vec4 getSpecular(vec2 texCoords)
{
	if (material.specularTextureEnabled == 0)
	{
		return vec4(material.specularColor, 1.0);
	}
	
	return texture(material.textureSpecular, texCoords);
}




void main()
{
    // properties
    vec3 norm = normalize(vs_out.Normal);
	
	if (material.normalTextureEnabled != 0)
	{
		norm = vec3(texture(material.textureNormal, vs_out.TexCoords).rgb);
		norm = normalize(norm * 2.0 - 1.0);
	}
	
    vec3 viewDir = normalize(viewPos - vs_out.FragPos);
	
	vec3 new_light_dir = normalize(vs_out.TangentLightPos - vs_out.TangentFragPos);
	vec3 new_view_dir = normalize(vs_out.TangentViewPos - vs_out.TangentFragPos);
	
	if (material.normalTextureEnabled == 0)
	{
		new_light_dir = normalize(-dirLight.direction);
		new_view_dir = viewDir;
	}
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec4 result = CalcDirLight(dirLight, norm, new_view_dir, new_light_dir);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, vs_out.FragPos, viewDir);    
    // phase 3: spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = result;
}

// calculates the color when using a directional light.
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 newDirLight)
{
    // vec3 lightDir = normalize(-light.direction);
    vec3 lightDir = newDirLight;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec4 ambient = vec4(light.ambient, 1) * getDiffuse(vs_out.TexCoords);
    vec4 diffuse = vec4(light.diffuse, 1) * diff * getDiffuse(vs_out.TexCoords);
    vec4 specular = vec4(light.specular, 1) * spec * getSpecular(vs_out.TexCoords);

    float shadow = ShadowCalculation(vs_out.FragPosLightSpace);
    //return vec4(shadow, 0, 0, 1);
	
	vec4 colorDiffSpec = diffuse + specular;
    vec4 lighting = (ambient + vec4((1.0 - shadow) * (colorDiffSpec.rgb), colorDiffSpec.a));
    return lighting;
    //return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec4 ambient = vec4(light.ambient, 1) * getDiffuse(vs_out.TexCoords);
    vec4 diffuse = vec4(light.diffuse, 1) * diff * getDiffuse(vs_out.TexCoords);
    vec4 specular = vec4(light.specular, 1) * spec * getSpecular(vs_out.TexCoords);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec4 ambient = vec4(light.ambient, 1) * getDiffuse(vs_out.TexCoords);
    vec4 diffuse = vec4(light.diffuse, 1) * diff * getDiffuse(vs_out.TexCoords);
    vec4 specular = vec4(light.specular, 1) * spec * getSpecular(vs_out.TexCoords);
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}




float ShadowCalculation(vec4 fragPosLightSpace){
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow

    vec3 lightDir = normalize(-dirLight.direction);
    vec3 normal = normalize(vs_out.Normal);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;

    //if (dot(normal, viewPos) <= 0){
    //    shadow = 1.0;
    //}

    return shadow;
}
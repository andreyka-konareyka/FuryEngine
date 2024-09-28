#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in vec4 FragPosLightSpace;


// material parameters
struct Material {
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;

    int albedoEnabled;
    int normalEnabled;
    int metallicEnabled;
    int roughnessEnabled;
    int aoEnabled;

    vec3 albedoColor;
    float metallic;
    float roughness;
    float ao;
    float opacity;
};
uniform Material material;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Shadow Map
uniform sampler2D shadowMap;
uniform int shadowMapEnabled;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;


struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

const float PI = 3.14159265359;



//! --- Получение данных материала ---
vec4 getAlbedo(vec2 texCoords);
vec3 getNormal(vec2 texCoords);
vec4 getMetallic(vec2 texCoords);
vec4 getRoughness(vec2 texCoords);
vec4 getAo(vec2 texCoords);


// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anyways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    if (material.normalEnabled != 0)
    {
        vec3 tangentNormal = getNormal(TexCoords).xyz * 2.0 - 1.0;

        vec3 Q1  = dFdx(WorldPos);
        vec3 Q2  = dFdy(WorldPos);
        vec2 st1 = dFdx(TexCoords);
        vec2 st2 = dFdy(TexCoords);

        vec3 N   = normalize(Normal);
        vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
        vec3 B  = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);

        return normalize(TBN * tangentNormal);
    }
    else
    {
        return normalize(Normal);
    }
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   
// ----------------------------------------------------------------------------
float ShadowCalculation(vec4 fragPosLightSpace)
{
	if (shadowMapEnabled == 0)
	{
		return 0;
	}
	
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
    vec3 normal = getNormalFromMap();
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.005) * 0.4;
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
// ----------------------------------------------------------------------------
void main()
{
    // material properties
    vec3 albedo = pow(getAlbedo(TexCoords).rgb, vec3(2.2));
    float metallic = getMetallic(TexCoords).r;
    float roughness = getRoughness(TexCoords).r;
    float ao = getAo(TexCoords).r;
       
    // input lighting data
    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    float shadow = ShadowCalculation(FragPosLightSpace);
    shadow *= 0.75;
    vec4 color = vec4(ambient + Lo, getAlbedo(TexCoords).a);
    color.rgb = color.rgb * (1.0 - shadow);

    // HDR tonemapping
    color.rgb = color.rgb / (color.rgb + vec3(1.0));
    // gamma correct
    color.rgb = pow(color.rgb, vec3(1.0/2.2));

    FragColor = color;
    //FragColor = vec4(color , 1.0);
}




//! --- Получение данных материала ---

vec4 getAlbedo(vec2 texCoords)
{
    if (material.albedoEnabled == 0)
    {
        return vec4(material.albedoColor, material.opacity);
    }

    return texture(material.albedoMap, texCoords);
}

vec3 getNormal(vec2 texCoords)
{
    if (material.normalEnabled == 0)
    {
        return vec3(0.5, 0.5, 1);
    }

    return texture(material.normalMap, texCoords).xyz;
}

vec4 getMetallic(vec2 texCoords)
{
    if (material.metallicEnabled == 0)
    {
        float m = material.metallic;
        return vec4(m, m, m, 1);
    }

    return texture(material.metallicMap, texCoords);
}

vec4 getRoughness(vec2 texCoords)
{
    if (material.roughnessEnabled == 0)
    {
        float r = material.roughness;
        return vec4(r, r, r, 1);
    }

    return texture(material.roughnessMap, texCoords);
}

vec4 getAo(vec2 texCoords)
{
    if (material.aoEnabled == 0)
    {
        float ao = material.ao;
        return vec4(ao, ao, ao, 1);
    }

    return texture(material.aoMap, texCoords);
}
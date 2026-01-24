#version 460 core

out vec4 FragColor;
in vec2 TexCoord;

layout(location = 13) uniform sampler2D shadowMap;
layout(location = 14) uniform sampler2D currentTexture;
layout(location = 15) uniform sampler2D depthMVPTexture;
layout(location = 16) uniform bool shadowMapEnabled;
layout(location = 17) uniform bool pointLightEnabled;
layout(location = 25) uniform bool volumetricLightEnabled;
layout(location = 18) uniform vec3 dirLightPos;
layout(location = 19) uniform vec3 pntLightPos;

layout(location = 24) uniform mat4 viewMat;
layout(location = 26) uniform mat4 projMat;

layout(location = 30) uniform sampler2D positionTexture;
layout(location = 31) uniform sampler2D normalTexture;
layout(location = 32) uniform sampler2D ambientTexture;
layout(location = 33) uniform sampler2D diffuseTexture;
layout(location = 34) uniform sampler2D specularTexture;

// SSAO
layout(location = 35) uniform sampler2D depthTexture;
layout(location = 36) uniform sampler2D noiseTexture;
layout(location = 37) uniform bool ssaoEnabled;
layout(location = 38) uniform bool ssaoOnlyEnabled;

// lighting constants
vec3 Ia = vec3(0.1, 0.1, 0.1);
vec3 Id = vec3(0.7, 0.7, 0.7);
vec3 Is = vec3(0.2, 0.2, 0.2);

float constant = 1.0;
float linear = 0.7;
float quadratic = 0.14;
float shadow = 0.0;
vec3 dirLightPos_eye;
vec3 pntLightPos_eye;

// info from textures
vec3 position;
vec3 normal;
vec3 Ka;
vec3 Kd;
vec3 Ks;
bool isSphere;

layout(std140) uniform Kernals                                                                  
{                                                                                              
    vec4 kernals[64];                                                                           
}; 

vec3 calcBPLight() {
    // if(Ka == vec3(1) && Kd == vec3(1) && Ks == vec3(1)) return vec3(1);
    if(isSphere) return Kd;
    vec3 L = normalize(dirLightPos_eye - position);
    vec3 N = normalize(normal);
    vec3 V = normalize(-position);
    vec3 H = normalize(L + V);
    float Ns = 225.0;

    vec3 ambient = Ka * Ia;
    vec3 diffuse = Kd * Id * max(0.0, dot(L, N));
    vec3 specular = Ks * Is * pow(max(0.0, dot(H, N)), Ns);

    // return ambient + diffuse + specular;
    return (ambient + (diffuse + specular) * (1.0 - shadow));
}

vec3 calcPointLight() {
    vec3 L = normalize(pntLightPos_eye - position);
    vec3 N = normalize(normal);
    vec3 V = normalize(-position);
    vec3 H = normalize(L + V);  
    float Ns = 225.0;

    vec3 ambient = Ka * Ia;
    vec3 diffuse = Kd * Id * max(0.0, dot(L, N));
    vec3 specular = Ks * Is * pow(max(0.0, dot(H, N)), Ns);
    
    float dis = length(pntLightPos_eye - position);
    float attenuation = 1.0 / (constant + linear * dis + quadratic * (dis * dis));   
    // float attenuation = min(1.0 / (constant + linear * dis + quadratic * (dis * dis)), 1.0f);   

   return attenuation * (ambient + diffuse + specular);
}

float shadowCalculation(){
    vec3 L = normalize(dirLightPos_eye - position);
    vec3 N = normalize(normal);
    vec4 depthMVP = texture(depthMVPTexture, TexCoord);
    vec3 shadowCoords = depthMVP.xyz / depthMVP.w; 
    shadowCoords = shadowCoords * 0.5 + 0.5;
    float bias = max(0.05 * (1.0 - max(0.0, dot(L, N))), 0.005);
    float closestDepth  = texture(shadowMap, shadowCoords.xy).r;
    float currentDepth  = shadowCoords.z - bias;
    float visibility = currentDepth > closestDepth ? 1.0 : 0.0;
    return visibility;
}

vec4 SSAO(){
    vec2 noiseScale = vec2(1000, 1000); 
    const float radius = 0.5;
    //const float bias = 0.025;
    const float bias = 0.0;
    const float kernalSize = 64.0;

    float depth  = texture(depthTexture, TexCoord).r;
    if(depth == 1.0) return vec4(0);

    mat4 invProj = inverse(projMat);
    vec4 position = vec4(position, 1.0f);  
    vec3 randVec = normalize(texture(noiseTexture, TexCoord * noiseScale).xyz);
    vec3 N = texture(normalTexture, TexCoord).xyz; 
    vec3 T = normalize(randVec - N * dot(randVec, N));   
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    float ao = 0.0f, ao2 = 0.0f;
    for(int i = 0; i < kernalSize; ++i)                                                                 
    {                                                                                           
	    vec4 sampleEye = position + vec4(TBN * kernals[i].xyz * radius, 0.0);                   
	    vec4 sampleP = projMat * sampleEye;                                                        
	    sampleP /= sampleP.w;                                                                   
	    sampleP = sampleP * 0.5 + 0.5;                                                          
	    float sampleDepth = texture(depthTexture, sampleP.xy).r;                                   
	    vec4 invP = invProj * vec4(vec3(sampleP.xy, sampleDepth) * 2.0 - 1.0, 1.0);             
	    invP /= invP.w;                                                                         
	    if(sampleDepth >= sampleP.z + bias || length(invP.xyz - position.xyz) > radius)                         
	    {                                                                                      
            ao += 1.0f;                                                                       
	    }    
	}                
	vec4 fragAO = vec4(vec3(ao / kernalSize), 1.0);                                                        
    return fragAO;
}

void main() {
    vec4 texColor = texture(currentTexture, TexCoord);
    position = texture(positionTexture, TexCoord).rgb;
    normal = texture(normalTexture, TexCoord).rgb;
    Ka = texture(ambientTexture, TexCoord).rgb;
    Kd = texture(diffuseTexture, TexCoord).rgb;
    Ks = texture(specularTexture, TexCoord).rgb;
    isSphere = texture(normalTexture, TexCoord).a < 0.5;

    dirLightPos_eye = (viewMat * vec4(dirLightPos, 1.0)).xyz;
    pntLightPos_eye = (viewMat * vec4(pntLightPos, 1.0)).xyz;

    if(volumetricLightEnabled){
        if(shadowMapEnabled) shadow = shadowCalculation();
        else shadow = 0.0;
        // FragColor = texColor * shadow;
        FragColor = vec4(calcBPLight(), 1.0);
    }
    else{
        FragColor = texColor;
    }

    if(pointLightEnabled){
        FragColor += vec4(calcPointLight(), 0.0);   
    }

    
    if(ssaoEnabled){
        vec4 aoFactor = SSAO();
        FragColor *= aoFactor; 
    }

    if(ssaoOnlyEnabled){
        vec4 aoFactor = SSAO();
        FragColor = aoFactor; 
    }
}
#version 460 core

vec4 FragColor;
in vec2 TexCoord;

layout(location = 0) out vec4 scene; 
layout(location = 1) out vec4 lightMap; 

layout(location = 9) uniform bool isTextureNormalized;
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
layout(location = 39) uniform vec2 frameSize;
layout(location = 40) uniform sampler2D obstacleTexture;
layout(location = 41) uniform vec2 lightPositionOnScreen;

layout(location = 42) uniform bool nprEnabled;

// point light shadow
layout(location = 43) uniform float far_plane;
layout(location = 44) uniform samplerCube pointShadowMap;
layout(location = 45) uniform bool pointShadowMapEnabled;
layout(location = 46) uniform sampler2D worldPosition;


// lighting constants
vec3 Ia = vec3(0.1, 0.1, 0.1);
vec3 Id = vec3(0.7, 0.7, 0.7);
vec3 Is = vec3(0.2, 0.2, 0.2);

float constant = 1.0;
float linear = 0.7;
float quadratic = 0.14;
float shadow = 0.0;
const int stepCount = 3;
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

    float diffusef = max(0.0, dot(L, N));
    float specularf = pow(max(0.0, dot(H, N)), Ns);

    if(nprEnabled){
        diffusef = floor(diffusef * stepCount) / stepCount; // Quantize diffuse
        specularf = floor(specularf * stepCount) / stepCount; // Quantize specular
    }

    vec3 ambient = Ka * Ia;
    vec3 diffuse = Kd * Id * diffusef;
    vec3 specular = Ks * Is * specularf;

    // return ambient + diffuse + specular;
    return (ambient + (diffuse + specular) * (1.0 - shadow));
}

vec3 calcPointLight() {
    if (isSphere) return Kd;
    vec3 L = normalize(pntLightPos_eye - position);
    vec3 N = normalize(normal);
    vec3 V = normalize(-position);
    vec3 H = normalize(L + V);  
    float Ns = 225.0;

    float diffusef = max(0.0, dot(L, N));
    float specularf = pow(max(0.0, dot(H, N)), Ns);

    if(nprEnabled){
        diffusef = floor(diffusef * stepCount) / stepCount; // Quantize diffuse
        specularf = floor(specularf * stepCount) / stepCount; // Quantize specular
    }

    vec3 ambient = Ka * Ia;
    vec3 diffuse = Kd * Id * diffusef;
    vec3 specular = Ks * Is * specularf;
    
    float dis = length(pntLightPos_eye - position);
    float attenuation = 1.0 / (constant + linear * dis + quadratic * (dis * dis));   
    // float attenuation = min(1.0 / (constant + linear * dis + quadratic * (dis * dis)), 1.0f);   

    //return attenuation * (ambient + diffuse + specular);
    return attenuation * (ambient + (diffuse + specular) * (1.0 - shadow));
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

float pointShadowCalculation() {
    vec3 worldPos = texture(worldPosition, TexCoord).rgb;
    vec3 fragToLight = worldPos - pntLightPos;
    float closestDepth = texture(pointShadowMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float visibility = (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
    return visibility;
}

vec4 SSAO(){
    vec2 noiseScale = vec2(frameSize.x / 4.0, frameSize.y / 4.0); 
    const float radius = 0.5;
    float bias = 0.025;
    bias = bias * 0.01 / (150.0 - bias);
    const int kernalSize = 64;

    float depth  = texture(depthTexture, TexCoord).r;
    if(depth == 1.0) return vec4(0);

    mat4 invProj = inverse(projMat);
    vec4 position = vec4(position, 1.0f);  
    vec3 randVec = normalize(texture(noiseTexture, TexCoord * noiseScale).xyz);
    vec3 N = texture(normalTexture, TexCoord).xyz; 
    vec3 T = normalize(randVec - N * dot(randVec, N));   
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    float ao = 0.0f;
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
    float fkernalSize = kernalSize;
    vec4 fragAO = vec4(vec3(ao / fkernalSize), 1.0);                                                        
    return fragAO;
}

vec4 VolumetricLight(){
    float decay = 0.96815f;
    float exposure = 0.2f;
    float density = 0.926f;
    float weight = 0.58767f;
    int NUM_SAMPLES = 100;
    float sampleWeight = 0.4f;
    vec2 tc = TexCoord;
    vec2 deltaTexCoord =  tc - lightPositionOnScreen;
    deltaTexCoord *= 1.0 / float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;
    vec4 color = texture2D(obstacleTexture, tc) * sampleWeight;
    for(int i=0; i < NUM_SAMPLES ; i++)
    {
        tc -= deltaTexCoord;
        vec4 s = texture2D(obstacleTexture, tc) * sampleWeight;
        s *= illuminationDecay * weight;
        color += s;
        illuminationDecay *= decay;
    }
    return color * exposure;
}

void main() {
    lightMap = vec4(0.0);
    vec4 texColor = texture(currentTexture, TexCoord);
    if(isTextureNormalized){
        texColor = normalize(texColor) * 0.5 + 0.5;
    }

    position = texture(positionTexture, TexCoord).rgb;
    normal = texture(normalTexture, TexCoord).rgb;
    Ka = texture(ambientTexture, TexCoord).rgb;
    Kd = texture(diffuseTexture, TexCoord).rgb;
    Ks = texture(specularTexture, TexCoord).rgb;
    isSphere = texture(normalTexture, TexCoord).a < 0.5;

    dirLightPos_eye = (viewMat * vec4(dirLightPos, 1.0)).xyz;
    pntLightPos_eye = (viewMat * vec4(pntLightPos, 1.0)).xyz;

    bool shading = false;

    if(volumetricLightEnabled){
        if(shadowMapEnabled) shadow = shadowCalculation();
        else shadow = 0.0;
        shading = true;
        FragColor = vec4(calcBPLight(), 1.0);
        FragColor += VolumetricLight();
        lightMap += VolumetricLight() / 3.0;
    }

    if(pointLightEnabled && volumetricLightEnabled){
        shading = true;
        if (pointShadowMapEnabled) shadow = pointShadowCalculation();
        else shadow = 0.0;
        FragColor += vec4(calcPointLight(), 0.0);   
        lightMap += vec4(calcPointLight(), 0.0) * 4.0;
    }
    else if(pointLightEnabled){
        shading = true;
        if (pointShadowMapEnabled) shadow = pointShadowCalculation();
        else shadow = 0.0;
        FragColor = vec4(calcPointLight(), 0.0);  
        lightMap += vec4(calcPointLight(), 0.0) * 4.0;
    }

    if(shading == false){
        FragColor = texColor;
    }

    if(ssaoEnabled){
        vec4 aoFactor = SSAO();
        // ssao will overrides external light source
        //FragColor *= aoFactor; 
        FragColor.rgb *= mix(vec3(1.0), aoFactor.rgb, 0.5);
    }

    if(ssaoOnlyEnabled){
        vec4 aoFactor = SSAO();
        FragColor = aoFactor; 
    }

    scene = FragColor;
}
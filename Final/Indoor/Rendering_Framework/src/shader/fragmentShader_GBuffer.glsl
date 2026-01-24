#version 460 core

in vec3 fragPositionWorld; 
in vec3 fragNormalWorld; 
in vec2 fragTexCoord;  
in vec4 fragposLightSpace;
in vec3 fragShadingPosition;
in vec3 fragShadingNormal;
in mat3 TBN;

layout(location = 0) out vec3 gPositionWorld; 
layout(location = 1) out vec3 gNormalWorld;  
layout(location = 2) out vec3 gAmbient;      
layout(location = 3) out vec3 gDiffuse;     
layout(location = 4) out vec3 gSpecular;     
layout(location = 5) out vec4 posLightSpace;     
layout(location = 6) out vec3 gShadingPosition;     
layout(location = 7) out vec4 gShadingNormal;  

layout(location = 28) uniform bool isSphere;
layout(location = 29) uniform int normalMapUsed;
layout(location = 30) uniform int texUsed;
layout(location = 31) uniform vec3 Kd;    
layout(location = 32) uniform vec3 Ka;    
layout(location = 33) uniform vec3 Ks;    

uniform sampler2D diffuseTexture;
layout(location = 34) uniform sampler2D normalMapTexture;

void main() {
    posLightSpace = fragposLightSpace;
    gPositionWorld = fragPositionWorld;
    gNormalWorld = normalize(fragNormalWorld) * 0.5 + 0.5;
    gDiffuse = texture(diffuseTexture, fragTexCoord).rgb;
    gSpecular = Ks;
    gAmbient = Ka;
    gShadingPosition = fragShadingPosition;

    if(texUsed == 1){
        vec4 texColor = texture(diffuseTexture, fragTexCoord);
        if(texColor.a < 0.5) discard;
	    gDiffuse = texColor.rgb;
    }
    else{
        gDiffuse = Kd;
    }

    vec3 normal = fragShadingNormal;
    if (normalMapUsed == 1) {
        vec3 normalFromMap = texture(normalMapTexture, fragTexCoord).rgb;
        normalFromMap = normalize(normalFromMap * 2.0 - 1.0); 
        normal = normalize(TBN * normalFromMap);  
    }
    gShadingNormal = vec4(normal, isSphere ? 0.0 : 1.0);
}

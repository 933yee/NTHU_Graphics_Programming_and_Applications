#version 460 core

layout(location = 0) uniform mat4 modelMat ;
layout(location = 1) uniform mat4 viewMat ;
layout(location = 2) uniform mat4 projMat ;

layout(location = 3) in vec3 position; 
layout(location = 4) in vec2 texCoord;  
layout(location = 5) in vec3 normal;
layout(location = 6) in vec3 tangent; 
layout(location = 7) in vec3 bitangent; 

layout(location = 10) uniform mat4 lightSpaceMatrix; 

layout(location = 29) uniform int normalMapUsed;
layout(location = 30) uniform int texUsed;

out vec3 fragPositionWorld; 
out vec3 fragNormalWorld;  
out vec2 fragTexCoord;    
out vec4 fragposLightSpace;
out vec3 fragShadingPosition;
out vec3 fragShadingNormal;
out mat3 TBN;


void main() {
    vec4 worldPosition = modelMat * vec4(position, 1.0);
    fragPositionWorld = worldPosition.xyz;
    fragNormalWorld = mat3(transpose(inverse(modelMat))) * normal; 
    fragTexCoord = texCoord;
    fragposLightSpace = lightSpaceMatrix * modelMat * vec4(position, 1.0);

    fragShadingPosition = (viewMat * modelMat * vec4(position, 1.0)).xyz;
    fragShadingNormal = mat3(viewMat  * modelMat) * normal;

    if (normalMapUsed == 1) {
        vec3 T = normalize(vec3(viewMat * modelMat * vec4(tangent, 0.0f)));  
        vec3 B = normalize(vec3(viewMat * modelMat * vec4(bitangent, 0.0f))); 
        vec3 N = normalize(vec3(viewMat * modelMat * vec4(normal, 0.0f)));     
        TBN = mat3(T, B, N);
    } else {
        TBN = mat3(1.0);
    }

    gl_Position = projMat * viewMat * worldPosition; 
}

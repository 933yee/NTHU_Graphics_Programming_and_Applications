#version 460 core

layout(location = 0) uniform mat4 modelMat ;
layout(location = 1) uniform mat4 viewMat ;
layout(location = 2) uniform mat4 projMat ;

layout(location = 3) in vec3 position; 
layout(location = 4) in vec2 texCoord;  
layout(location = 5) in vec3 normal;
layout(location = 12) uniform mat4 lightSpaceMatrix; 


out vec2 f_uv;   
out vec4 f_posLightSpace;

void main() {
    f_posLightSpace = lightSpaceMatrix * modelMat * vec4(position, 1.0);
    f_uv = texCoord;
    gl_Position = projMat * viewMat *  modelMat * vec4(position, 1.0);
}

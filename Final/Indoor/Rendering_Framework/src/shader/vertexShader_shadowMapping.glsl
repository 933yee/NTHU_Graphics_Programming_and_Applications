#version 460 core

layout(location = 0) uniform mat4 modelMat ;
layout(location = 10) uniform mat4 lightSpaceMatrix; 

layout(location = 3) in vec3 position; 
layout(location = 4) in vec2 texCoord;  
layout(location = 5) in vec3 normal;

void main() {
    gl_Position = lightSpaceMatrix * modelMat * vec4(position, 1.0);
}

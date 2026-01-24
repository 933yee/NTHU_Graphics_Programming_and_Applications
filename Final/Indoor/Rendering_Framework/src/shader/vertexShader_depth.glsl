#version 460 core

layout(location = 0) uniform mat4 modelMat ;
layout(location = 1) uniform mat4 viewMat ;
layout(location = 2) uniform mat4 projMat ;

layout(location = 3) in vec3 position; 
layout(location = 4) in vec2 texCoord;  
layout(location = 5) in vec3 normal;

void main() {
    gl_Position = projMat * viewMat * modelMat * vec4(position, 1.0);
}

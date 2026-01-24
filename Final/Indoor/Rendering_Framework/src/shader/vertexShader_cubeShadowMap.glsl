#version 460 core

layout(location = 0) uniform mat4 modelMat ;

layout(location = 3) in vec3 position; 

void main() {
    gl_Position = modelMat * vec4(position, 1.0);
}

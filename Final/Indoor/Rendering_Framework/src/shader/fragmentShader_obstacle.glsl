#version 460 core

out vec4 FragColor;

layout(location = 28) uniform bool isSphere;

void main() {
	if(isSphere) FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	else FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
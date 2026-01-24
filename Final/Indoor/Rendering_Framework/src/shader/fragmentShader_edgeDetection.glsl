#version 460 core

layout(location = 31) uniform sampler2D gNormal;
layout(location = 4) uniform sampler2D scene;
layout(location = 5) uniform vec2 texelSize; // 1.0 / screen resolution
layout(location = 6) uniform bool objectOnly;

out vec4 fragColor;

in VS_OUT
{
    vec2 texcoord;
} fs_in;

void main() {
    // Sample normals and depth
    vec3 nC = texture(gNormal, fs_in.texcoord).rgb;                    
    vec3 nN = texture(gNormal, fs_in.texcoord + vec2(0.0, texelSize.y)).rgb; // Top
    vec3 nS = texture(gNormal, fs_in.texcoord - vec2(0.0, texelSize.y)).rgb; // Bottom
    vec3 nE = texture(gNormal, fs_in.texcoord + vec2(texelSize.x, 0.0)).rgb; // Right
    vec3 nW = texture(gNormal, fs_in.texcoord - vec2(texelSize.x, 0.0)).rgb; // Left

    vec3 dy = (nN - nS)* 0.5;
    vec3 dx = (nE - nW)* 0.5;
    float edgeNormal = length(sqrt(dx*dx + dy*dy));

    float edgeLight = 0.0;
    if (objectOnly == false) {
        vec3 lC = texture(scene, fs_in.texcoord).rgb;   
        vec3 lN = texture(scene, fs_in.texcoord + vec2(0.0, texelSize.y)).rgb; // Top
        vec3 lS = texture(scene, fs_in.texcoord - vec2(0.0, texelSize.y)).rgb; // Bottom
        vec3 lE = texture(scene, fs_in.texcoord + vec2(texelSize.x, 0.0)).rgb; // Right
        vec3 lW = texture(scene, fs_in.texcoord - vec2(texelSize.x, 0.0)).rgb; // Left

        vec3 dy = (lN - lS)* 0.5;
        vec3 dx = (lE - lW)* 0.5;
        edgeLight = length(sqrt(dx * dx + dy * dy));
        // edgeLight = length(lN - lS) + length(lE - lW);
    }

    float edge = edgeNormal + edgeLight;
    fragColor = vec4(vec3(edge > 0.1 ? 0.0 : 1.0), 1.0); // Black edges on white background
}

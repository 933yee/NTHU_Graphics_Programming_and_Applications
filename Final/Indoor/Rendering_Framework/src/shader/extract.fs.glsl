#version 410

out vec4 fragColor;

in VS_OUT
{
    vec2 texcoord;
} fs_in;
    
uniform sampler2D scene;
uniform sampler2D isSphere_tex;

void main() {
    vec3 color = texture(scene, fs_in.texcoord).rgb;
    float isSphere = texture(isSphere_tex, fs_in.texcoord).a;
    fragColor = isSphere < 0.5 ? vec4(color, 1.0) : vec4(0.0);
}
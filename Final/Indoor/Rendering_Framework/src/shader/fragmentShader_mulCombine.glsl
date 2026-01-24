#version 430

out vec4 fragColor;

in VS_OUT
{
    vec2 texcoord;
} fs_in;
    
layout(location = 2) uniform sampler2D scene;
layout(location = 3) uniform sampler2D edges;

void main()
{
    vec3 color = texture(scene, fs_in.texcoord).rgb;      
    vec3 edge = texture(edges, fs_in.texcoord).rgb;
    color *= edge;
    fragColor = vec4(color, 1.0);
}
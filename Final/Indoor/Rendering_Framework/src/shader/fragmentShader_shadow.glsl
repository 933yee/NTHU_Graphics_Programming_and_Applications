#version 460 core
layout(location = 6) uniform int texUsed;
layout(location = 7) uniform vec3 Kd;
out vec4 FragColor;

uniform sampler2D tex;
layout(location = 13) uniform sampler2D shadowMap;

in vec2 f_uv;
in vec4 f_posLightSpace;

void main() {
    vec4 texColor;
    if(texUsed == 1) {
        texColor = texture(tex, f_uv);
        if(texColor.a < 0.5) discard;
    } else {
        texColor = vec4(Kd, 1.0);
    }

    vec3 projCoords = f_posLightSpace.xyz / f_posLightSpace.w; 
    projCoords = projCoords * 0.5 + 0.5;

    float bias = max(0.005 * (1.0 - dot(normalize(f_posLightSpace.xyz), vec3(0, 0, 1))), 0.0005);

    float closestDepth  = texture(shadowMap, projCoords.xy).r;
    float currentDepth  = projCoords.z - bias;

    float shadow = currentDepth > closestDepth ? 0.5 : 1.0;

    FragColor = shadow * texColor;
    FragColor = vec4(vec3(closestDepth), 1.0f);

}
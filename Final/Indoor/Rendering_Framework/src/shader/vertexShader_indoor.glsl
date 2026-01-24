#version 460 core

layout(location = 0) uniform mat4 modelMat ;
layout(location = 1) uniform mat4 viewMat ;
layout(location = 2) uniform mat4 projMat ;

layout(location = 3) in vec3 position; 
layout(location = 4) in vec2 texCoord;  
layout(location = 5) in vec3 normal;

layout(location = 21) uniform bool useBloom = false;


out DIRLIGHT{
    vec3 N;
    vec3 L;
    vec3 V;
} dirLight;

out PNTLIGHT{
    vec3 N;
    vec3 L;
    vec3 V;
} pntLight;

out vec2 f_uv;

vec3 dir_light_pos = vec3(-2.845, 2.028, -1.293);
vec3 pnt_light_pos = vec3(1.87659, 0.4625 , 0.103928);

void main() {
    gl_Position = projMat * viewMat  * modelMat * vec4(position, 1.0);
    f_uv = texCoord;

    pnt_light_pos = (viewMat * vec4(pnt_light_pos, 1.0)).xyz;
    dir_light_pos = (viewMat * vec4(dir_light_pos, 1.0)).xyz;

    // shading
    vec4 P = viewMat * modelMat * vec4(position, 1.0);
    pntLight.N = mat3(viewMat  * modelMat) * normal;
    pntLight.L = pnt_light_pos - P.xyz;
    pntLight.V = -P.xyz;

    P = viewMat * modelMat * vec4(position, 1.0);
    dirLight.N = mat3(viewMat  * modelMat) * normal;
    dirLight.L = dir_light_pos - P.xyz;
    dirLight.V = -P.xyz;
}

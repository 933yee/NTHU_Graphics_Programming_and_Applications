#version 430 core

layout(location = 30) uniform int texUsed ;
layout(location = 31) uniform vec3 Kd;
layout(location = 32) uniform vec3 Ka;
layout(location = 33) uniform vec3 Ks;
layout(location = 23) uniform float Ns;

layout(location = 20) uniform bool useBPLight = true;
layout(location = 21) uniform bool useBloom = false;

in DIRLIGHT{
    vec3 N;
    vec3 L;
    vec3 V;
} dirLight;

in PNTLIGHT{
    vec3 N;
    vec3 L;
    vec3 V;
} pntLight;

in vec2 f_uv;

out vec4 fragColor;

uniform sampler2D tex;

vec3 Ia = vec3(0.1, 0.1, 0.1);
vec3 Id = vec3(0.7, 0.7, 0.7);
vec3 Is = vec3(0.2, 0.2, 0.2);

float constant = 1.0;
float linear = 0.7;
float quadratic = 0.14;

vec3 calcBPLight(vec3 diffuse) {
    vec3 L = normalize(dirLight.L);
    vec3 N = normalize(dirLight.N);
    vec3 H = normalize(normalize(dirLight.L) + normalize(dirLight.V));

    return Ka * Ia + diffuse * Id * max(0.0, dot(L, N)) + Ks * Is * pow(max(0.0, dot(H, N)), Ns);
}

vec3 calcPointLight(vec3 diffuse) {
    vec3 L = normalize(pntLight.L);
    vec3 N = normalize(pntLight.N);
    vec3 H = normalize(normalize(pntLight.L) + normalize(pntLight.V));
    
    float distance    = length(pntLight.L);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));   

    return attenuation * calcBPLight(diffuse);
}

void main(){
    vec3 diffuse;
    
    vec4 texColor;
    if(texUsed == 1){
	    texColor = texture(tex, f_uv);
        if(texColor.a < 0.5) discard;
        diffuse = texColor.rgb;
    }
    else{
        diffuse = Kd;
    }

    if (useBloom) {
        fragColor = vec4(calcBPLight(diffuse), 1.0);
        fragColor += vec4(calcPointLight(diffuse), 1.0);
    }
    else if (useBPLight) {
        fragColor = vec4(calcBPLight(diffuse), 1.0);
    }
    else {
        fragColor = vec4(diffuse, 1.0);
    }

}
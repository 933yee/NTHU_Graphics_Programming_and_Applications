// Fragment Shader
#version 410
layout(location = 0) out vec4 fragColor;

in VertexData {
    vec3 N;
    vec3 L;
    vec3 H;
    vec2 texcoord;
} vertexData;

uniform sampler2D tex;
uniform int mode;

void main() {
    if(mode == 0){
        vec4 texColor = vec4(vertexData.N, 1.0f);
        fragColor = texColor;
    }else{
        vec4 texColor = vec4(texture(tex,vertexData.texcoord).rgb, 1.0f);
        fragColor = texColor;
    }

}
#version 410

out vec4 fragColor;

in VS_OUT
{
    vec2 texcoord;
} fs_in;
    
uniform sampler2D scene;
uniform sampler2D bloomBlur;
float exposure = 0.5;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, fs_in.texcoord).rgb;      
    vec3 bloomColor = texture(bloomBlur, fs_in.texcoord).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(hdrColor, vec3(1.0 / gamma));

    //vec4 texColor = texture(tex, fs_in.texcoord);
    //fragColor = vec4(1.0) - texColor;
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    fragColor = vec4(hdrColor, 1.0);
}
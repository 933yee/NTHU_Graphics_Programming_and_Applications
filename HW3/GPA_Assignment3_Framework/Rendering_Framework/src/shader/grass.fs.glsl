#version 460 core

layout (location = 0) out vec4 fragColor ;
uniform sampler2DArray texGrass;

in vec3 f_viewVertex ;
in vec3 f_viewNormal ;
in vec3 f_worldVertex ;
in vec3 f_worldNormal ;
in vec3 f_uv ;

vec3 Ka = vec3(0.1);
vec3 Kd = vec3(0.8);
vec3 Ks = vec3(0.1);
vec3 intensity = vec3(1.0);
vec3 lightDirection= vec3(0.3, 0.7, 0.5);
in float f_textureLayer;

void main() {
    vec4 texel = texture(texGrass, vec3(f_uv.xy, f_textureLayer));
    if (texel.a < 0.5) {
        discard;
    }
    vec3 lightDir = normalize(lightDirection);
    vec3 normal = normalize(f_worldNormal);
    vec3 viewDir = normalize(-f_worldVertex);
    vec3 ambient = Ka * intensity;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = Kd * diff * intensity;
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); 
    vec3 specular = Ks * spec * intensity;
    vec3 lighting = ambient + diffuse + specular;
    vec3 finalColor = texel.rgb * lighting;
    vec4 shaderColor = vec4(finalColor, texel.a);
    fragColor = vec4(1.0f) - exp(-shaderColor * 3.0f);
}
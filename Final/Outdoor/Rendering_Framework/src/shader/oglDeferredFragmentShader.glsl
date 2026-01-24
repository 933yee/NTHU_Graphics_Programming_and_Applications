#version 430 core

layout(location = 0) out vec4 color_out;
layout(location = 1) uniform sampler2D gbuf_worldVertex;
layout(location = 2) uniform sampler2D gbuf_worldNormal;
layout(location = 3) uniform sampler2D gbuf_diffuse;
layout(location = 4) uniform sampler2D gbuf_specular;

layout(location = 5) uniform int deferred_type;
layout(location = 24) uniform vec3 viewPos;


// GBUFFER
const int WORLDSPACEVERTEX = 0;
const int WORLDSPACENORMAL = 1;
const int AMBIENT = 2;
const int DIFFUSE = 3;
const int SPECULAR = 4;
const int DEFAULT = 5;

struct light_t
{
	vec3 position;
	uint pad0;
	vec3 color;
	uint pad1;
};
uniform vec3 eye_position;


struct fragment_info_t
{
	vec3 color;
	vec3 normal;
	vec3 ws_coord;
	float specular_power;
};


void main() {
	switch(deferred_type) {
	case WORLDSPACEVERTEX:
		color_out = normalize(texelFetch(gbuf_worldVertex, ivec2(gl_FragCoord.xy), 0)) * 0.5 + 0.5;
		break;
	case WORLDSPACENORMAL:
		color_out = normalize(texelFetch(gbuf_worldNormal, ivec2(gl_FragCoord.xy), 0)) * 0.5 + 0.5;
		break;
	case AMBIENT:
		color_out = texelFetch(gbuf_diffuse, ivec2(gl_FragCoord.xy), 0);
		break;
	case DIFFUSE:
		color_out = texelFetch(gbuf_diffuse, ivec2(gl_FragCoord.xy), 0);
		break;
	case SPECULAR:
		color_out = vec4(texelFetch(gbuf_specular, ivec2(gl_FragCoord.xy), 0).rgb, 1.0);
		break;
	case DEFAULT:
		//Blinn-Phong
		vec4 result = vec4(0, 0, 0, 1);

		vec3 worldVertex = texelFetch(gbuf_worldVertex, ivec2(gl_FragCoord.xy), 0).xyz;
		vec3 worldNormal = texelFetch(gbuf_worldNormal, ivec2(gl_FragCoord.xy), 0).rgb;
		vec3 diffuseMap = texelFetch(gbuf_diffuse, ivec2(gl_FragCoord.xy), 0).rgb;
		vec3 specularMap = texelFetch(gbuf_specular, ivec2(gl_FragCoord.xy), 0).rgb;
		float Shininess = texelFetch(gbuf_specular, ivec2(gl_FragCoord.xy), 0).a;

		vec3 L = vec3(0.4, 0.5, 0.8); // world space
		vec3 Ia = vec3(0.2);	// la
		vec3 Id = vec3(0.64);  // ld
		vec3 Is = vec3(0.16);  // ls

		vec3 albedo = texelFetch(gbuf_diffuse, ivec2(gl_FragCoord.xy), 0).rgb;
		// Ambient
		vec3 ambient = Ia * albedo;
		// Diffuse
		// vec3 lightDir = normalize(L - worldVertex);
		vec3 lightDir = vec3(0.4, 0.5, 0.8);
		vec3 normal = normalize(worldNormal);
		vec3 diffuse = Id * diffuseMap * max(dot(lightDir, normal), 0.0);

		// Specular
		vec3 viewDir = normalize(viewPos - worldVertex);
		// vec3 viewDir = normalize(worldVertex - viewPos);
		// vec3 viewDir = normalize((viewMat * worldVertex).xyz);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		vec3 specular = Is * specularMap * pow(max(dot(normal, halfwayDir), 0.0), Shininess);

		color_out = vec4(ambient+diffuse+specular,1.0);

		color_out = vec4(pow(ambient + diffuse + specular, vec3(0.5)), 1.0);
		break;
	}

}
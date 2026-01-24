#version 430 core

in vec4 f_worldVertex;
in vec4 f_worldNormal;
in vec3 f_viewVertex;
in vec3 f_uv;
in vec3 f_tangent;
in vec3 f_bitangent;

// layout (location = 0) out vec4 fragColor ;
layout(location = 0) out vec4 gb_worldVertex;
layout(location = 1) out vec4 gb_worldNormal;
// layout(location = 2) out vec4 gb_ambient;
layout(location = 2) out vec4 gb_diffuse;
layout(location = 3) out vec4 gb_specular;

layout(location = 2) uniform int pixelProcessId;
layout(location = 4) uniform sampler2D albedoTexture ;
layout(location = 10) uniform sampler2D airplaneTexture;
layout(location = 11) uniform sampler2D magicStoneTexture;
layout(location = 12) uniform sampler2D magicStoneTextureNorm;

layout(location = 0) uniform mat4 modelMat;
layout(location = 7) uniform mat4 viewMat;

layout(location = 3) uniform int isNormalMapping;


vec4 withFog(vec4 color){
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1) ;
	const float MAX_DIST = 400.0 ;
	const float MIN_DIST = 350.0 ;
	
	float dis = length(f_viewVertex) ;
	float fogFactor = (MAX_DIST - dis) / (MAX_DIST - MIN_DIST) ;
	fogFactor = clamp(fogFactor, 0.0, 1.0) ;
	fogFactor = fogFactor * fogFactor ;
	
	vec4 colorWithFog = mix(FOG_COLOR, color, fogFactor) ;
	return colorWithFog ;
}


void terrainPass(){
	gb_diffuse = texture(albedoTexture, f_uv.xy) ;
	gb_specular = vec4(vec3(0.0), 1.0);
	// fragColor = withFog(texel); 
	// fragColor.a = 1.0;	
}

void airplanePass() {
	gb_diffuse = texture(airplaneTexture, f_uv.xy);
	gb_specular = vec4(vec3(1.0), 32.0);
	// fragColor = withFog(texel);
	// fragColor.a = 1.0;
}

void magicStonePass() {
	gb_diffuse = texture(magicStoneTexture, f_uv.xy);
	gb_specular = vec4(vec3(1.0), 32.0);
	if (isNormalMapping == 1) {
		mat3 tTBN;
		tTBN[0] = f_tangent;
		tTBN[1] = f_bitangent;
		tTBN[2] = f_worldNormal.xyz;
		gb_worldNormal = vec4(tTBN * (texture(magicStoneTextureNorm, f_uv.xy).xyz * 2.0 - vec3(1.0)), 0.0);
	}
	// gb_worldNormal = vec4(normalize(mat3(modelMat) * texture(magicStoneTextureNorm, f_uv.xy).xyz * 2.0 - vec3(1.0)), 0.0 );
	// gb_worldNormal = normalize( inverse(modelMat) * (texture(magicStoneTextureNorm, f_uv.xy) * 2.0 - vec4(1.0)));
	//inverse(modelMat) * 
	// fragColor = withFog(texel);
	// fragColor.a = 1.0;
}

void pureColor(){
	gb_diffuse = vec4(1.0, 0.0, 0.0, 1.0);
	// fragColor = withFog(vec4(1.0, 0.0, 0.0, 1.0)) ;
}

void main(){	
	gb_worldVertex = f_worldVertex;
	gb_worldNormal = f_worldNormal;
	// gb_specular = vec4(vec3(0.0), 1.0);

	if(pixelProcessId == 5){
		pureColor() ;
	}
	else if(pixelProcessId == 7){
		terrainPass() ;
	}
	else if (pixelProcessId == 8) {
		airplanePass();
	}
	else if (pixelProcessId == 9) {
		magicStonePass();
	}
	else{
		pureColor() ;
	}
}
#version 430 core

layout(location = 0) in vec3 v_vertex;
layout(location = 1) in vec3 v_normal ;
layout(location = 2) in vec3 v_uv ;
layout(location = 3) in vec3 v_offset;
layout(location = 4) in vec3 v_tangent;
layout(location = 5) in vec3 v_bitangent;

out vec4 f_worldVertex;
out vec4 f_worldNormal;
out vec3 f_viewVertex ;
out vec3 f_uv ;
out vec3 f_tangent;
out vec3 f_bitangent;


layout(location = 0) uniform mat4 modelMat ;
layout(location = 5) uniform sampler2D elevationMap ;
layout(location = 6) uniform sampler2D normalMap ;
layout(location = 7) uniform mat4 viewMat ;
layout(location = 8) uniform mat4 projMat ;
layout(location = 9) uniform mat4 terrainVToUVMat;
layout(location = 1) uniform int vertexProcessIdx ;


void commonProcess(){
	vec4 worldVertex = modelMat * vec4(v_vertex, 1.0); // +vec4(v_offset, 0.0);
	vec4 worldNormal = modelMat * vec4(v_normal, 0.0) ;

	vec4 viewVertex = viewMat * worldVertex ;
	vec4 viewNormal = viewMat * worldNormal ;
	
	f_worldVertex = worldVertex;
	f_worldNormal = worldNormal;
	f_viewVertex = viewVertex.xyz;
	f_uv = v_uv ;
	f_tangent = v_tangent;
	f_bitangent = v_bitangent;

	gl_Position = projMat * viewVertex ;
}

void terrainProcess(){
	vec4 worldVertex = modelMat * vec4(v_vertex, 1.0) ;
	// calculate uv
	vec4 uv = terrainVToUVMat * worldVertex;
	uv.y = uv.z ;
	// get height from map
	float h = texture(elevationMap, uv.xy).r ;
	worldVertex.y = h;
	// get normal from map
	vec4 worldNormal = texture(normalMap, uv.xy) ;
	// [0, 1] -> [-1, 1]
	worldNormal = worldNormal * 2.0 - 1.0 ;
		
	// transformation	
	vec4 viewVertex = viewMat * worldVertex;
	vec4 viewNormal = viewMat * vec4(worldNormal.rgb, 0) ;
	
	f_worldVertex = worldVertex;
	f_worldNormal = worldNormal;
	f_viewVertex = viewVertex.xyz;
	f_uv = uv.xyz ;
	f_tangent = v_tangent;
	f_bitangent = v_bitangent;

	gl_Position = projMat * viewVertex ;
}

void magicStoneProcess() {
	vec4 worldVertex = modelMat * vec4(v_vertex, 1.0) + vec4(v_offset, 0.0);
	vec4 worldNormal = modelMat * vec4(v_normal, 0.0);

	vec4 viewVertex = viewMat * worldVertex;
	vec4 viewNormal = viewMat * worldNormal;

	f_worldVertex = worldVertex;
	f_worldNormal = worldNormal;
	f_viewVertex = viewVertex.xyz;
	f_uv = v_uv;
	f_tangent = v_tangent;
	f_bitangent = v_bitangent;

	gl_Position = projMat * viewVertex;
}

void main(){
	if(vertexProcessIdx == 0){
		commonProcess() ;
	}
	else if(vertexProcessIdx == 3){
		terrainProcess() ;
	}
	else if (vertexProcessIdx == 4) {
		magicStoneProcess();
	}
	else{
		commonProcess() ;
	}	
}
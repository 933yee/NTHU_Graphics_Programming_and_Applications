#version 460 core
layout(location = 0) in vec3 v_vertex;
layout(location = 1) in vec3 v_uv;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec4 v_worldPosOffset;

layout(location = 1) uniform mat4 viewMat;
layout(location = 2) uniform mat4 projMat;
layout(location = 4) uniform mat4 modelMat;  
layout(location = 5) uniform float textureLayer;
flat out float f_textureLayer;

out vec3 f_viewVertex ;
out vec3 f_viewNormal ;
out vec3 f_worldVertex ;
out vec3 f_worldNormal ;
out vec3 f_uv ;

 void main() {    
    vec4 worldVertex = modelMat * vec4(v_vertex, 1.0) + v_worldPosOffset ;
//     vec4 worldVertex = modelMat * vec4(v_vertex, 1.0) ;
     // the vertex¡¦s w must be 1
     worldVertex.w = 1.0;
     vec4 worldNormal = modelMat * vec4(v_normal, 0.0) ;
     f_worldVertex = worldVertex.xyz ;
     f_worldNormal = worldNormal.xyz ;
     vec4 viewVertex = viewMat * worldVertex ;
     vec4 viewNormal = viewMat * worldNormal;
     f_viewVertex = viewVertex.xyz ;
     f_viewNormal = viewNormal.xyz ;
     f_uv = v_uv ;
     f_textureLayer = textureLayer;
     gl_Position = projMat * viewVertex ;
 } 
#version 430 core

void main() {
	vec4[4] vertices = vec4[4](
		vec4(-1.0, -1.0, 1.0, 1.0),
		vec4(1.0, -1.0, 1.0, 1.0),
		vec4(-1.0, 1.0, 1.0, 1.0),
		vec4(1.0, 1.0, 1.0, 1.0)
		);

	gl_Position = vertices[gl_VertexID];
}
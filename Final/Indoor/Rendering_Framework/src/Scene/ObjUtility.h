#pragma once

#include <glad/glad.h>
#include <Rendering_Framework/src/Rendering/Camera/Camera.h>
#include <Rendering_Framework/src/Rendering/RendererBase.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/Rendering/Shader.h>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

struct texture_data
{
	texture_data() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char *data;
};

struct Shape
{
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	GLuint vbo_tangent;
	GLuint vbo_bitangent;
	int drawCount;
	int materialID;
};

struct Material
{
	int texUsed; // 0: no texture, 1: with texture
	GLuint diffuse_tex;
	int normal_tex_used;
	GLuint normal_tex;
	glm::vec3 Kd;
	glm::vec3 Ka;
	glm::vec3 Ks;
	float Ns;
};

texture_data loadImg(const char *path);
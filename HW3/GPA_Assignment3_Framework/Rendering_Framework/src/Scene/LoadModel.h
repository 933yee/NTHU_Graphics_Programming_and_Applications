#pragma once
#include <glad/glad.h>
#include <assimp\cimport.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

struct Shape
{
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	int drawCount;
	int materialID;
};
struct Material
{
	GLuint diffuse_tex;
};

typedef struct _texture_data
{
	_texture_data() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
} texture_data;
texture_data loadImg(const char* path);
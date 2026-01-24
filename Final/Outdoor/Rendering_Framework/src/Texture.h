#pragma once
#include "stb_image.h"

#include <string>
#include <vector>
#include <glad/glad.h>

class Texture 
{
public:
	Texture(std::string imagePath);

private:
	void loadTexture(std::string path);
	GLuint setUpTexture();

public:
	GLuint getTextureID() { return id; };

private:
	GLuint id;
	stbi_uc* data = nullptr;
	int width, height, channel;
};
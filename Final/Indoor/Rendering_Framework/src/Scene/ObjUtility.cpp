#pragma once
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <TinyOBJ/tiny_obj_loader.h>
#include <stb_image.h>

texture_data loadImg(const char* path)
{
	texture_data texture;
	int n;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = stbi_load(path, &texture.width, &texture.height, &n, 4);
	printf("Loaded texture: %s, width: %d, height: %d\n", path, texture.width, texture.height);
	if (data != NULL)
	{
		texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
		memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
		stbi_image_free(data);
	}
	return texture;
}
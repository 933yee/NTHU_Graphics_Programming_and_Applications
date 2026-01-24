#include "Texture.h"

#include <iostream>

Texture::Texture(std::string imagePath)
{
	loadTexture(imagePath);
    this->id = setUpTexture();
    stbi_image_free(data);
}

void Texture::loadTexture(std::string path)
{	
	this->data = stbi_load(path.c_str(), &width, &height, &channel, 4);
    if (!data) {
        std::cout << "ERROR::TEXTURE::Can't load texture image:" << path << std::endl;
    }
    else {
        std::cout << "DEBUG::TEXTURE::Successfully load texture:" << path << std::endl;
    }
}

GLuint Texture::setUpTexture()
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

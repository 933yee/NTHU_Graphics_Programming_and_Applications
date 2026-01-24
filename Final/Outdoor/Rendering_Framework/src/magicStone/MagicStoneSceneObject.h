#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <Rendering_Framework\src\SceneManager.h>
#include <Rendering_Framework\src\Mesh.h>
#include <Rendering_Framework\src\Texture.h>

class MagicStoneSceneObject
{
public:
	MagicStoneSceneObject();

	void update();

private:
	void setWorldPosOffset();

private:
	int m_numIndex;

	Mesh* m_mesh = nullptr;
	Texture* m_texture = nullptr;
	Texture* m_textureNorm = nullptr;

	GLuint m_vao;
	GLuint m_magicStoneMapHandle;
	GLuint m_magicStoneNormMapHandle;

	glm::mat4 m_modelMat;
	glm::vec3 m_worldPosOffset;
};
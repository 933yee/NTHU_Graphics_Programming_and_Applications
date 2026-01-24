#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <Rendering_Framework\src\SceneManager.h>
#include <Rendering_Framework\src\Mesh.h>
#include <Rendering_Framework\src\Texture.h>

class AirplaneSceneObject
{
public:
	AirplaneSceneObject();

	void update();
	void updateState(glm::mat4 modelMat);

private:
	int m_numIndex;
	
	Mesh* m_mesh = nullptr;
	Texture* m_texture = nullptr;

	GLuint m_vao;
	GLuint m_airplaneMapHandle;

	glm::mat4 m_modelMat;
};
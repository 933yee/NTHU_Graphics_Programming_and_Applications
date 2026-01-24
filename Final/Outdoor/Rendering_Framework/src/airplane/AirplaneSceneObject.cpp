#include "AirplaneSceneObject.h"

AirplaneSceneObject::AirplaneSceneObject()
{
	// load mesh
	this->m_mesh = new Mesh("assets/airplane.obj");
	this->m_vao = this->m_mesh->getVAO();
	this->m_numIndex = this->m_mesh->getNumIndex();
	// load texture
	this->m_texture = new Texture("assets/Airplane_smooth_DefaultMaterial_BaseMap.jpg");
	this->m_airplaneMapHandle = this->m_texture->getTextureID();
}

void AirplaneSceneObject::updateState(glm::mat4 modelMat)
{
	this->m_modelMat = modelMat;
}

void AirplaneSceneObject::update()
{
	glBindVertexArray(this->m_vao);

	glActiveTexture(SceneManager::Instance()->m_airplaneTexUnit);
	glBindTexture(GL_TEXTURE_2D, this->m_airplaneMapHandle);

	glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, SceneManager::Instance()->m_vs_commonProcess);
	glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, SceneManager::Instance()->m_fs_airplanePass);
	glUniformMatrix4fv(SceneManager::Instance()->m_modelMatHandle, 1, false, glm::value_ptr(this->m_modelMat));

	int indicesPointer = 0;
	glDrawElements(GL_TRIANGLES, this->m_numIndex, GL_UNSIGNED_INT, (GLvoid*)(indicesPointer));

	glBindVertexArray(0);
}

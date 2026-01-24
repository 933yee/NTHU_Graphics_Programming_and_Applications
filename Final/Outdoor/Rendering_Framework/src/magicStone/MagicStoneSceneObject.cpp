#include "MagicStoneSceneObject.h"

MagicStoneSceneObject::MagicStoneSceneObject()
{
	// TODO
	this->m_worldPosOffset = glm::vec3(25.92, 18.27, 11.75);
	this->m_modelMat = glm::mat4(1.0);
	// load mesh
	this->m_mesh = new Mesh("assets/MagicRock/magicRock.obj");
	this->m_vao = this->m_mesh->getVAO();
	this->m_numIndex = this->m_mesh->getNumIndex();
	setWorldPosOffset();
	// load texture
	this->m_texture = new Texture("assets/MagicRock/StylMagicRocks_AlbedoTransparency.png");
	this->m_magicStoneMapHandle = this->m_texture->getTextureID();
	// load texture norm
	this->m_textureNorm = new Texture("assets/MagicRock/StylMagicRocks_NormalOpenGL.png");
	this->m_magicStoneNormMapHandle = this->m_textureNorm->getTextureID();
}

void MagicStoneSceneObject::setWorldPosOffset()
{
	glBindVertexArray(this->m_vao);
	glVertexAttrib3f(3, m_worldPosOffset.x, m_worldPosOffset.y, m_worldPosOffset.z);
	glBindVertexArray(0);
}

void MagicStoneSceneObject::update()
{
	// TODO
	glBindVertexArray(this->m_vao);

	glActiveTexture(SceneManager::Instance()->m_magicStoneTexUnit);
	glBindTexture(GL_TEXTURE_2D, this->m_magicStoneMapHandle);

	glActiveTexture(SceneManager::Instance()->m_magicStoneTexNormUnit);
	glBindTexture(GL_TEXTURE_2D, this->m_magicStoneNormMapHandle);

	glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, SceneManager::Instance()->m_fs_magicStonePass);
	glUniformMatrix4fv(SceneManager::Instance()->m_modelMatHandle, 1, false, glm::value_ptr(this->m_modelMat));

	int indicesPointer = 0;
	glDrawElements(GL_TRIANGLES, this->m_numIndex, GL_UNSIGNED_INT, (GLvoid*)(indicesPointer));

	glBindVertexArray(0);
}

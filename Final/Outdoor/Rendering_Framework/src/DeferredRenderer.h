#pragma once

#include <vector>
#include "Shader.h"
#include <glad\glad.h>
#include "GBuffer.h"
#include <glm\gtc\matrix_transform.hpp>

class DeferredRenderer
{
public:
	DeferredRenderer();

private:
	ShaderProgram* m_shaderProgram = nullptr;
	int m_frameWidth;
	int m_frameHeight;

	GLuint VAO;

	GLint worldVertexMapHandle;
	GLint worldNormalMapHandle;
	GLint diffuseMapHandle;
	GLint specularMapHandle;

	GLuint outputTypeHandle;
	GLuint viewPosHandle;

	GBuffer::OutputType m_deferredOutput;

	glm::vec3 m_viewPos;

public:
	void renderPass();
	void setDeferredOutput(GBuffer::OutputType outputType);
	void setViewPos(glm::vec3 playerViewOrg) { this->m_viewPos = playerViewOrg;};
public:
	void resize(const int w, const int h);
	bool initialize(const int w, const int h, ShaderProgram* shaderProgram);

private:
	bool setUpShader();
};

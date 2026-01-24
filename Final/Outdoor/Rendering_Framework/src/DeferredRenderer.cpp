#include "DeferredRenderer.h"

DeferredRenderer::DeferredRenderer()
{
	this->m_deferredOutput = GBuffer::OutputType::DEFAULT;
}

void DeferredRenderer::renderPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	this->m_shaderProgram->useProgram();

	GBuffer* gbuffer = GBuffer::Instance();
	gbuffer->bindAndActive();

	glUniform1i(this->outputTypeHandle, GLuint(this->m_deferredOutput));
	glUniform3f(this->viewPosHandle, this->m_viewPos.x, this->m_viewPos.y, this->m_viewPos.z);

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DeferredRenderer::setDeferredOutput(GBuffer::OutputType outputType)
{
	m_deferredOutput = outputType;
}

void DeferredRenderer::resize(const int w, const int h)
{
	this->m_frameWidth = w;
	this->m_frameHeight = h;

	GBuffer* gbuffer = GBuffer::Instance();
	gbuffer->resizeTexture(this->m_frameWidth, this->m_frameHeight);

}

bool DeferredRenderer::initialize(const int w, const int h, ShaderProgram* shaderProgram)
{
	this->m_shaderProgram = shaderProgram;

	this->resize(w, h);
	const bool flag = this->setUpShader();

	if (!flag) {
		return false;
	}

	GBuffer* gbuffer = GBuffer::Instance();
	gbuffer->init(this->m_frameWidth, this->m_frameHeight);

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	return true;
}

bool DeferredRenderer::setUpShader()
{
	if (this->m_shaderProgram == nullptr) {
		return false;
	}
	this->m_shaderProgram->useProgram();
	// shader attributes binding
	const GLuint programId = this->m_shaderProgram->programId();
	
	GBuffer* gbuffer = GBuffer::Instance();
	gbuffer->worldVertexMapHandle = 1;
	gbuffer->worldNormalMapHandle = 2;
	gbuffer->diffuseMapHandle = 3;
	gbuffer->specularMapHandle = 4;

	this->outputTypeHandle = 5;
	this->viewPosHandle = 24;

}

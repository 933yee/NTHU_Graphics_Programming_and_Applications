#pragma once
#include <glad\glad.h>

class GBuffer
{
public:
	enum OutputType {
		WORLDSPACEVERTEX,
		WORLDSPACENORMAL,
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		DEFAULT
	};
public:
	GBuffer();

	virtual ~GBuffer() {}

	static GBuffer* Instance() {
		static GBuffer* m_instance = nullptr;
		if (m_instance == nullptr) {
			m_instance = new GBuffer();
		}
		return m_instance;
	}

public:
	void init(int w, int h);
	void resizeTexture(int w, int h);
	void bindAndActive();
	void bindFrameBuffer(); 

public:
	GLuint FBO;
	// TextureID
	GLuint worldVertexMap;
	GLuint worldNormalMap;
	GLuint diffuseMap;
	GLuint specularMap;
	GLuint depthMap;
	// 
	GLuint worldVertexMapHandle;
	GLuint worldNormalMapHandle;
	GLuint diffuseMapHandle;
	GLuint specularMapHandle;
};
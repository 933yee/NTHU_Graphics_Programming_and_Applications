#pragma once
#ifndef GBUFFER_H
#define GBUFFER_H

#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <glm/glm.hpp>
#include <string>

enum class GBufferTexture {
	Position = 0,
	Normal = 1,
	Ambient = 2,
	Diffuse = 3,
	Specular = 4,
	DepthMVP = 5,
	ShadingPosition = 6,
	ShadingNormal = 7
};

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			class GBuffer {
			public:
				explicit GBuffer(const Camera* camera, const int w, const int h);
				virtual ~GBuffer();

				GBuffer(const GBuffer&) = delete;
				GBuffer(const GBuffer&&) = delete;
				GBuffer& operator=(const GBuffer&) = delete;

				void init(const Camera* camera, const int w, const int h);
				void resize(const int w, const int h);
				void beginRender();
				void endRender();
				GLuint getTexture(GBufferTexture texture) const;

			private:
				OPENGL::RendererBase* m_renderer = nullptr;
				GLuint m_gPositionWorld; 
				GLuint m_gNormalWorld;    
				GLuint m_gAmbient;        
				GLuint m_gDiffuse;        
				GLuint m_gSpecular;       
				GLuint m_rboDepth;        
				GLuint m_gBuffer;
				GLuint m_depthMVP;
				GLuint m_gShadingPosition;
				GLuint m_gShadingNormal;
			};
		}
	}
}

#endif

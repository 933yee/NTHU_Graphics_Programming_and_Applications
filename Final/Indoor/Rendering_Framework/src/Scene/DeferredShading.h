#pragma once
#ifndef DEFERREDSHADING_H
#define DEFERREDSHADING_H
#include <Rendering_Framework/src/Scene/GBuffer.h>
#include <Rendering_Framework/src/Scene/ShadowMapping.h>
#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <Rendering_Framework/src/Scene/RIndoor.h>
#include <Rendering_Framework/src/RenderWidgets/Interface.h>
#include <glm/glm.hpp>
#include <string>

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			class DeferredShading {
			public:
				explicit DeferredShading(const Camera* camera, const int w, const int h);
				virtual ~DeferredShading();

				DeferredShading(const DeferredShading&) = delete;
				DeferredShading(const DeferredShading&&) = delete;
				DeferredShading& operator=(const DeferredShading&) = delete;

				void init(const Camera* camera, const int w, const int h);
				void render(GBuffer& gBuffer, ShadowMapping& shadowMapping, GLuint psmap_tex, Interface& gui, const Camera* camera);
				void resize(const int w, const int h);

			private:
				OPENGL::RendererBase* m_renderer = nullptr;
				GLuint screenQuadVAO = 0;
				GLuint screenQuadVBO = 0;
				int m_frameWidth;
				int m_frameHeight;
			};
		}
	}
}

#endif

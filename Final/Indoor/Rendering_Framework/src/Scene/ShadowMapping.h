#pragma once
#ifndef SHADOWMAPPING_H
#define SHADOWMAPPING_H

#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <glm/glm.hpp>
#include <string>

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			class ShadowMapping {
			public:
				explicit ShadowMapping(const Camera* camera, const int w, const int h);
				virtual ~ShadowMapping();

				ShadowMapping(const ShadowMapping&) = delete;
				ShadowMapping(const ShadowMapping&&) = delete;
				ShadowMapping& operator=(const ShadowMapping&) = delete;

				void init(const Camera* camera, const int w, const int h);
				void updateLightMatrix(glm::vec3 lightPos);
				void beginRender(glm::vec3 lightPos);
				void endRender();
				void renderGBuffer();
				GLuint getShadowMap() const;

			private:
				OPENGL::RendererBase* m_shadow_map_renderer = nullptr;
				OPENGL::RendererBase* m_shadow = nullptr;
				GLuint m_shadowMap;
				GLuint m_shadowFBO;
				glm::mat4 m_lightSpaceMatrix;
				int m_resolution;
				int m_windowWidth;
				int m_windowHeight;
			};
		}
	}
}

#endif

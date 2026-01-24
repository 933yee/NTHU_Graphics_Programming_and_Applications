#pragma once
#ifndef SSAO_H
#define SSAO_H

#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <Rendering_Framework/src/RenderWidgets/Interface.h>
#include <glm/glm.hpp>
#include <string>
#include <time.h>

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			class SSAO {
			public:
				explicit SSAO(const int w, const int h);
				virtual ~SSAO();

				SSAO(const SSAO&) = delete;
				SSAO(const SSAO&&) = delete;
				SSAO& operator=(const SSAO&) = delete;

				void init(const int w, const int h);
				void resize(const int w, const int h);
				void beginRenderDepth();
				void endRenderDepth();
				void render(Interface& gui);
				GLuint getDepthMap() { return m_depthMap; }

			private:
				OPENGL::RendererBase* m_renderer = nullptr;
				GLuint m_uboSSAOKernal;
				GLuint m_noiseMap;
				GLuint m_depthMap;
				GLuint m_depthFBO;
			};
		}
	}
}

#endif

#pragma once
#ifndef FXAA_H
#define FXAA_H

#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Rendering_Framework/src/Rendering/FrameBuffer.h>
#include <Rendering_Framework/src/Scene/GBuffer.h>

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			class FXAA {
			public:

				explicit FXAA(const int w, const int h);
				virtual ~FXAA();

				FXAA(const FXAA&) = delete;
				FXAA(const FXAA&&) = delete;
				FXAA& operator=(const FXAA&) = delete;

				void init(const int w, const int h);
				void update(const Camera* camera);
				void render(OPENGL::FrameBuffer* scene_fb, bool fxaaEnabled);
				void resize(const int w, const int h);
			private:
				OPENGL::RendererBase* m_renderer = nullptr;
				GLuint m_textureHandle;

				float m_frameWidth;
				float m_frameHeight;
			};
		}
	}
}

#endif
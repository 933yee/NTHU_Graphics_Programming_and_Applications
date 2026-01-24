#pragma once
#ifndef NPR_H
#define NPR_H

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
			class NPR {
			public:

				explicit NPR(const int w, const int h);
				virtual ~NPR();

				NPR(const NPR&) = delete;
				NPR(const NPR&&) = delete;
				NPR& operator=(const NPR&) = delete;

				void init(const int w, const int h);
				void update(const Camera* camera);
				void renderEdgeDetection(OPENGL::FrameBuffer* scene_fb, GBuffer* gBuffer, bool objOnly);
				void renderEdgeCombine(OPENGL::FrameBuffer* scene_fb, OPENGL::FrameBuffer* edge_fb);
				void resize(const int w, const int h);
			private:
				OPENGL::RendererBase* m_DetectionRenderer = nullptr;
				OPENGL::RendererBase* m_CombineRenderer = nullptr;
				GLuint m_textureHandle;

				float m_frameWidth;
				float m_frameHeight;
			};
		}
	}
}

#endif
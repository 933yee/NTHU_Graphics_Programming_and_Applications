#pragma once
#ifndef RBloomS_H
#define RBloomS_H

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
			class Bloom {
			public:

				explicit Bloom(const Camera* camera, const int w, const int h);
				virtual ~Bloom();

				Bloom(const Bloom&) = delete;
				Bloom(const Bloom&&) = delete;
				Bloom& operator=(const Bloom&) = delete;

				void init(const Camera* camera, const int w, const int h);
				void loadScene();
				void update(const Camera* camera);
				void renderExtractedBall(OPENGL::FrameBuffer* scene_fb, GBuffer* gBuffer);
				void renderBlur(OPENGL::FrameBuffer* scene_fb, bool horizontal);
				void renderBloom(OPENGL::FrameBuffer* scene_fb, OPENGL::FrameBuffer* effect_fb);
				void renderGBuffer(const Camera* camera, glm::vec3 pos);
				std::vector<Material> materials;
				std::vector<Shape> shapes;

			private:
				OPENGL::RendererBase* m_renderer = nullptr;
				glm::mat4 m_modelMat;
				glm::mat4 m_viewMat;
				glm::mat4 m_projMat;
				GLuint m_textureHandle;

				GLuint horizontalLoc;

				OPENGL::ShaderProgram* bloom_program = nullptr;
				OPENGL::ShaderProgram* blur_program = nullptr;
				OPENGL::ShaderProgram* extract_program = nullptr;
			};
		}
	}
}

#endif
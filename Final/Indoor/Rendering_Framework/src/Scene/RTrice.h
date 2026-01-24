#pragma once
#ifndef RTriceS_H
#define RTriceS_H

#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Rendering_Framework/src/RenderWidgets/Interface.h>

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			class Trice {
			public:

				explicit Trice(const Camera* camera, const int w, const int h);
				virtual ~Trice();

				Trice(const Trice&) = delete;
				Trice(const Trice&&) = delete;
				Trice& operator=(const Trice&) = delete;

				void init(const Camera* camera, const int w, const int h);
				void loadModel();
				void update(const Camera* camera);
				void resize(const int w, const int h);
				void render(const Camera* camera);
				void renderGBuffer(const Camera* camera, bool toogleNormalMapping);
				void renderMVP();
				std::vector<Material> materials;
				std::vector<Shape> shapes;

			private:
				OPENGL::RendererBase *m_renderer = nullptr;
				glm::mat4 m_modelMat;
				glm::mat4 m_viewMat;
				glm::mat4 m_projMat;
				GLuint m_textureHandle;
			};
		}
	}
}

#endif
#pragma once
#ifndef RIndoorS_H
#define RIndoorS_H

#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace INANOA
{
	namespace SCENE
	{
		namespace EXPERIMENTAL
		{
			class Indoor
			{
			public:
				explicit Indoor(const Camera *camera, const int w, const int h);
				virtual ~Indoor();

				Indoor(const Indoor &) = delete;
				Indoor(const Indoor &&) = delete;
				Indoor &operator=(const Indoor &) = delete;

				void init(const Camera *camera, const int w, const int h);
				void loadScene();
				void update(const Camera *camera);
				void resize(const int w, const int h);
				void render(const Camera *camera, bool useBloom);
				void render(const Camera *camera);
				void renderGBuffer(const Camera *camera);
				void renderMVP(const Camera* camera);
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
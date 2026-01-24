#pragma once
#ifndef VOLUMETRICLIGHT_H
#define VOLUMETRICLIGHT_H

#include <Rendering_Framework/src/Scene/ObjUtility.h>
#include <Rendering_Framework/src/RenderWidgets/Interface.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <string>
#include <glm/gtc/type_ptr.hpp>

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			class VolumetricLight {
			public:
				explicit VolumetricLight(const int w, const int h);
				virtual ~VolumetricLight();

				VolumetricLight(const VolumetricLight&) = delete;
				VolumetricLight(const VolumetricLight&&) = delete;
				VolumetricLight& operator=(const VolumetricLight&) = delete;

				void init(const int w, const int h);
				void loadModel();

				void beginRenderObstacle(const Camera* camera);
				void endRenderObstacle();
				void render(const Camera* camera, const int w, const int h, Interface& gui);

			private:
				void VolumetricLight::calculateScreenCoordinates(
					const glm::vec3& worldPosition,
					const Camera* camera,
					const int viewport[4],
					double& screenX, double& screenY, double& screenZ
				);

				glm::vec3 m_lightPosition = glm::vec3(0.0f);
				OPENGL::RendererBase* m_renderer = nullptr;
				GLuint m_obstacleMap;
				GLuint m_FBO;
				std::vector<Material> materials;
				std::vector<Shape> shapes;
				glm::mat4 m_modelMat;
			};
		}
	}
}

#endif

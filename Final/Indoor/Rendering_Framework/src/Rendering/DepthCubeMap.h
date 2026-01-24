#pragma once
#ifndef DepthCubeMap_H
#define DepthCubeMap_H
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <Rendering_Framework/src/Rendering/Shader.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

namespace INANOA {
	namespace OPENGL {
		class DepthCubeMap {
		public:

			explicit DepthCubeMap();
			virtual ~DepthCubeMap();

			DepthCubeMap(const DepthCubeMap&) = delete;
			DepthCubeMap(const DepthCubeMap&&) = delete;
			DepthCubeMap& operator=(const DepthCubeMap&) = delete;

			void init();

			void start_render(glm::vec3 light_pos);
			void end_render();
			
			void update_light_pos(glm::vec3 light_pos);


			inline unsigned int getFBOtex() { return this->fbo_tex; }

		private:
			void bind_matrix(); // bind view and proj matrix to shader

			ShaderProgram* m_shader = NULL;
			
			unsigned int fbo = 0u;
			unsigned int rbo = 0u;

			unsigned int fbo_tex = 0u;

			const float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
			const float near = 0.22f;
			const float far = 10.0f;

			glm::vec3 light_pos;
			glm::mat4 proj_mat;
			std::vector<glm::mat4> pv_mat;

			const std::vector<glm::vec3> cube_dir = { 
				{1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, 
				{0.0, 1.0, 0.0}, {0.0, -1.0, 0.0}, 
				{0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}
			};
			const std::vector<glm::vec3> ups = {
				{0.0, -1.0, 0.0}, {0.0, -1.0, 0.0},
				{0.0, 0.0, 1.0}, {0.0, 0.0, -1.0},
				{0.0, -1.0, 0.0}, {0.0, -1.0, 0.0}
			};
		};
		
	}
}

#endif
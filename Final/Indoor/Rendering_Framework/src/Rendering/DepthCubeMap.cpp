#include "DepthCubeMap.h"
#include <iostream>

namespace INANOA {
	namespace OPENGL {
		DepthCubeMap::DepthCubeMap() {
			this->init();
		}
		DepthCubeMap::~DepthCubeMap() {}

		void DepthCubeMap::init() {
			// shader
			const std::string vsFile = "src\\shader\\vertexShader_cubeShadowMap.glsl";
			const std::string gsFile = "src\\shader\\geometryShader_cubeShadowMap.glsl";
			const std::string fsFile = "src\\shader\\fragmentShader_cubeShadowMap.glsl";
			this->m_shader = ShaderProgram::createShaderProgramWithGeometryShader(vsFile, gsFile, fsFile);

			if (this->m_shader == NULL) {
				printf("Failed to initialize cube shadow mapping shader program.\n");
				return;
			}

			// cube texture
			glGenTextures(1, &this->fbo_tex);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->fbo_tex);
			for (unsigned int i = 0; i < 6; i++) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			// depth map frame buffer
			glGenFramebuffers(1, &this->fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->fbo_tex, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// setup projection matrix
			this->proj_mat = glm::perspective(glm::radians(90.0f), this->aspect, this->near, this->far);

		}

		void DepthCubeMap::bind_matrix() {
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::SHADOW_TRANSFORMS, 6, GL_FALSE, &this->pv_mat[0][0][0]);
			glUniform3fv(SHADER_PARAMETER_BINDING::LIGHT_POS, 1, &this->light_pos[0]);
			glUniform1f(SHADER_PARAMETER_BINDING::FAR_PLANE, this->far);
		}

		void DepthCubeMap::update_light_pos(glm::vec3 light_pos) {
			this->light_pos = light_pos;
			this->pv_mat.clear();
			for (int i = 0; i < 6; i++) {
				this->pv_mat.push_back(this->proj_mat * glm::lookAt(light_pos, light_pos + this->cube_dir[i], this->ups[i]));
			}
		}

		void DepthCubeMap::start_render(glm::vec3 light_pos) {
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			m_shader->useProgram();
			update_light_pos(light_pos);
			bind_matrix();
		}

		void DepthCubeMap::end_render() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

	}
}
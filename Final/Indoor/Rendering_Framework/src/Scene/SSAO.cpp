#include "SSAO.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {

            SSAO::SSAO(const int w, const int h){
				this->init(w, h);
			}

            SSAO::~SSAO() {
				delete m_renderer;
				m_renderer = nullptr;
            }

			void SSAO::init(const int w, const int h) {
				m_renderer = new OPENGL::RendererBase();
				const std::string vsFile = "src\\shader\\vertexShader_depth.glsl";
				const std::string fsFile = "src\\shader\\fragmentShader_depth.glsl";

				if (!m_renderer->init(vsFile, fsFile, w, h)) {
					printf("Failed to initialize shadow mapping shader program.\n");
					return;
				}

				glGenFramebuffers(1, &m_depthFBO);

				glGenTextures(1, &m_depthMap);
				glBindTexture(GL_TEXTURE_2D, m_depthMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					printf("Failed to initialize Shadow Map Framebuffer.\n");

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glGenBuffers(1, &m_uboSSAOKernal);
				glBindBuffer(GL_UNIFORM_BUFFER, m_uboSSAOKernal);
				glBufferData(GL_UNIFORM_BUFFER, 64 * sizeof(glm::vec4), 0, GL_STATIC_DRAW);
				glm::vec4* uniformSSAOKernalPtr = (glm::vec4*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				srand((unsigned int)time(0));
				for (int i = 0; i < 64; ++i)
				{
					float scale = (float)i / (float) 64;
					scale *= glm::mix(0.1f, 1.0f, scale * scale);
					uniformSSAOKernalPtr[i] =
						glm::vec4(
							glm::normalize(glm::vec3(
								rand() / (float)RAND_MAX * 2.0f - 1.0f, 
								rand() / (float)RAND_MAX * 2.0f - 1.0f, 
								rand() / (float)RAND_MAX
							)
						) * scale, 0);
				}
				glUnmapBuffer(GL_UNIFORM_BUFFER);

				glGenTextures(1, &m_noiseMap);
				glBindTexture(GL_TEXTURE_2D, m_noiseMap);
				const int noiseSize = 4;
				glm::vec3 noiseData[noiseSize * noiseSize];
				for (int i = 0; i < noiseSize * noiseSize; ++i)
				{
					noiseData[i] = glm::normalize(
						glm::vec3(
							rand() / (float)RAND_MAX * 2.0 - 1.0,
							rand() / (float)RAND_MAX * 2.0 - 1.0,
							0.0f
						)
					);
				}
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, &noiseData[0][0]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			void SSAO::resize(const int w, const int h) {
				glDeleteTextures(1, &m_depthMap);
				glDeleteFramebuffers(1, &m_depthFBO);

				glGenFramebuffers(1, &m_depthFBO);

				glGenTextures(1, &m_depthMap);
				glBindTexture(GL_TEXTURE_2D, m_depthMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					printf("Failed to initialize Shadow Map Framebuffer.\n");
			}
          
			void SSAO::beginRenderDepth() {
				glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				m_renderer->useProgram();
			}

			void SSAO::endRenderDepth() {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			void SSAO::render(Interface& gui) {
				glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboSSAOKernal);
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, m_depthMap);
				glUniform1i(SHADER_PARAMETER_BINDING::DEPTH_TEXTURE, 10);
				glActiveTexture(GL_TEXTURE11);
				glBindTexture(GL_TEXTURE_2D, m_noiseMap);
				glUniform1i(SHADER_PARAMETER_BINDING::NOISE_TEXTURE, 11);

				glUniform1i(SHADER_PARAMETER_BINDING::SSAO_ENABLE, gui.toggleSSAO);
				glUniform1i(SHADER_PARAMETER_BINDING::SSAO_ONLY_ENABLE, gui.toggleOnlySSAO);
			}

		} 
	}
} 

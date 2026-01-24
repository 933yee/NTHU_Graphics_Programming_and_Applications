#include "NPR.h"
#include "Rendering_Framework/src/Rendering/Shader.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			NPR::NPR(const int w, const int h) {
				this->init(w, h);
			}

			NPR::~NPR() {
			}

			void NPR::init(const int w, const int h) {
				INANOA::OPENGL::RendererBase* DetectionRenderer = new INANOA::OPENGL::RendererBase();
				INANOA::OPENGL::RendererBase* CombineRenderer = new INANOA::OPENGL::RendererBase();
				const std::string vsFile = "src\\shader\\framebuffer.vs.glsl";
				const std::string fsFileD = "src\\shader\\fragmentShader_edgeDetection.glsl";
				const std::string fsFileC = "src\\shader\\fragmentShader_mulCombine.glsl";

				// create vertex shader and fragment shader
				if (!DetectionRenderer->init(vsFile, fsFileD, w, h)) {
					printf("Failed to initialize edge detection shader program.\n");
					return;
				}
				if (!CombineRenderer->init(vsFile, fsFileC, w, h)) {
					printf("Failed to initialize multiply combine shader program.\n");
					return;
				}

				m_DetectionRenderer = DetectionRenderer;
				m_CombineRenderer = CombineRenderer;

				this->resize(w, h);
			}

			void NPR::resize(const int w, const int h) {
				this->m_frameHeight = h;
				this->m_frameWidth = w;
			}

			void NPR::renderEdgeDetection(OPENGL::FrameBuffer* scene_fb, GBuffer* gBuffer, bool objOnly) {
				m_DetectionRenderer->useProgram();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glDisable(GL_DEPTH_TEST);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gBuffer->getTexture(GBufferTexture::Normal));
				glUniform1i(SHADER_PARAMETER_BINDING::NORMAL_TEXTURE, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, scene_fb->getFBOtex2());
				glUniform1i(4, 1);

				glUniform2f(5, 1.0 / m_frameWidth, 1.0 / m_frameHeight);
				glUniform1i(6, objOnly);

				glBindVertexArray(scene_fb->getVAO());
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glBindVertexArray(0);
				glEnable(GL_DEPTH_TEST);
			}

			void NPR::renderEdgeCombine(OPENGL::FrameBuffer* scene_fb, OPENGL::FrameBuffer* edge_fb) {
				m_CombineRenderer->useProgram();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glDisable(GL_DEPTH_TEST);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, scene_fb->getFBOtex());
				glUniform1i(2, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, edge_fb->getFBOtex());
				glUniform1i(3, 1);

				glBindVertexArray(scene_fb->getVAO());
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glBindVertexArray(0);
				glEnable(GL_DEPTH_TEST);
			}

			void NPR::update(const Camera* camera) {
			}

		}
	}
}

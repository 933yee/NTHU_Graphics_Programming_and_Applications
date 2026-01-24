#include "FXAA.h"
#include "Rendering_Framework/src/Rendering/Shader.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			FXAA::FXAA(const int w, const int h) {
				this->init(w, h);
			}

			FXAA::~FXAA() {
			}

			void FXAA::init(const int w, const int h) {
				INANOA::OPENGL::RendererBase* renderer = new INANOA::OPENGL::RendererBase();
				const std::string vsFile = "src\\shader\\framebuffer.vs.glsl";
				const std::string fsFile = "src\\shader\\fragmentShader_fxaa.glsl";

				// create vertex shader and fragment shader
				if (!renderer->init(vsFile, fsFile, w, h)) {
					printf("Failed to initialize FXAA shader program.\n");
					return;
				}
				m_renderer = renderer;
				m_textureHandle = m_renderer->getUniformLocation("tex");
				glUniform1i(m_textureHandle, 0);

				this->resize(w, h);
			}

			void FXAA::resize(const int w, const int h) {
				this->m_frameHeight = h;
				this->m_frameWidth = w;
			}

			void FXAA::render(OPENGL::FrameBuffer *scene_fb, bool fxaaEnabled) {
				m_renderer->useProgram();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glDisable(GL_DEPTH_TEST);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, scene_fb->getFBOtex());

				glUniform1i(3, fxaaEnabled);
				glUniform2f(4, m_frameWidth, m_frameHeight);

				glBindVertexArray(scene_fb->getVAO());
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glBindVertexArray(0);
				glEnable(GL_DEPTH_TEST);
			}

			void FXAA::update(const Camera* camera) {
			}

		}
	}
}

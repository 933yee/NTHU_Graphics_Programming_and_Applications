#include "DeferredShading.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {

            DeferredShading::DeferredShading(const Camera* camera, const int w, const int h){
				this->init(camera, w, h);
			}

            DeferredShading::~DeferredShading() {
				delete m_renderer;
				m_renderer = nullptr;
            }

            void DeferredShading::init(const Camera* camera, const int w, const int h) {
                this->m_frameWidth = w;
                this->m_frameHeight = h;
                m_renderer = new OPENGL::RendererBase();
                const std::string vsFile2 = "src\\shader\\vertexShader_deferredShading.glsl";
                const std::string fsFile2 = "src\\shader\\fragmentShader_deferredShading.glsl";

                if (!m_renderer->init(vsFile2, fsFile2, w, h)) {
                    printf("Failed to initialize screen shader program.\n");
                    return;
                }

                // Initialize screen quad VAO/VBO
                float screenQuadVertices[] = {
                    // Positions   // TexCoords
                    -1.0f,  1.0f,  0.0f, 1.0f, 
                    -1.0f, -1.0f,  0.0f, 0.0f,
                     1.0f, -1.0f,  1.0f, 0.0f,
                    -1.0f,  1.0f,  0.0f, 1.0f,
                     1.0f, -1.0f,  1.0f, 0.0f,
                     1.0f,  1.0f,  1.0f, 1.0f 
                };

                glGenVertexArrays(1, &screenQuadVAO);
                glGenBuffers(1, &screenQuadVBO);

                glBindVertexArray(screenQuadVAO);
                glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), &screenQuadVertices, GL_STATIC_DRAW);

                // Positions
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                // TexCoords
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
                glEnableVertexAttribArray(1);

                glBindVertexArray(0);
            }

            void DeferredShading::resize(const int w, const int h) {
                m_frameHeight = h;
                m_frameWidth = w;
            }

            void DeferredShading::render(GBuffer& gBuffer, ShadowMapping& shadowMapping, GLuint psmap_tex, Interface &gui, const Camera* camera) {
                glViewport(0, 0, m_frameWidth, m_frameHeight);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                m_renderer->useProgram();
                // Shadow Map
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, shadowMapping.getShadowMap());
                glUniform1i(SHADER_PARAMETER_BINDING::SHADOW_MAP, 0);
                // Current texture
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(gui.deferredShadingMode));
                glUniform1i(SHADER_PARAMETER_BINDING::GBUFFER_TEXTURE, 1);

                if (gui.deferredShadingMode == GBufferTexture::Position) {
                    glUniform1i(SHADER_PARAMETER_BINDING::IS_TEXTURE_NORMALIZED, true);
                }
                else {
                    glUniform1i(SHADER_PARAMETER_BINDING::IS_TEXTURE_NORMALIZED, false);
                }

                // Light Map
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(GBufferTexture::DepthMVP));
                glUniform1i(SHADER_PARAMETER_BINDING::DEPTHMVP_TEXTURE, 2);
                // Position
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(GBufferTexture::ShadingPosition));
                glUniform1i(SHADER_PARAMETER_BINDING::POSITION_TEXTURE, 3);
                // Normal
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(GBufferTexture::ShadingNormal));
                glUniform1i(SHADER_PARAMETER_BINDING::NORMAL_TEXTURE, 4);
                // Ambient
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(GBufferTexture::Ambient));
                glUniform1i(SHADER_PARAMETER_BINDING::AMBIENT_TEXTURE, 5);
                // Diffuse
                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(GBufferTexture::Diffuse));
                glUniform1i(SHADER_PARAMETER_BINDING::DIFFUSE_TEXTURE, 6);
                // Specular
                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(GBufferTexture::Specular));
                glUniform1i(SHADER_PARAMETER_BINDING::SPECULAR_TEXTURE, 7);
                // World Position
                glActiveTexture(GL_TEXTURE8);
                glBindTexture(GL_TEXTURE_2D, gBuffer.getTexture(GBufferTexture::Position));
                glUniform1i(SHADER_PARAMETER_BINDING::WORLD_POSITION_TEXTURE, 8);
                // Point Shadow Map
                glActiveTexture(GL_TEXTURE9);
                glBindTexture(GL_TEXTURE_CUBE_MAP, psmap_tex);
                glUniform1i(SHADER_PARAMETER_BINDING::POINT_SHADOW_MAP, 9);
                glUniform1f(SHADER_PARAMETER_BINDING::FAR_PLANE, 10.0);
                glUniform1i(SHADER_PARAMETER_BINDING::POINT_SHADOW_MAP_ENABLE, gui.togglePointShadow);

                glUniform1i(SHADER_PARAMETER_BINDING::SHADOW_MAP_ENABLE, gui.toggleDirectionalShadow);
                glUniform1i(SHADER_PARAMETER_BINDING::VOL_LIGHT_ENABLE, gui.toggleVolumetricLight);
                glUniform1i(SHADER_PARAMETER_BINDING::POINT_LIGHT_ENABLE, gui.togglePointLight);
                glUniform3fv(SHADER_PARAMETER_BINDING::DIR_LIGHT_POS, 1, glm::value_ptr(gui.volumetricLightPosition));
                glUniform3fv(SHADER_PARAMETER_BINDING::PNT_LIGHT_POS, 1, glm::value_ptr(gui.pointLightPosition));
                glUniformMatrix4fv(SHADER_PARAMETER_BINDING::VIEW_MAT_POS, 1, false, glm::value_ptr(camera->viewMatrix()));
                glUniformMatrix4fv(SHADER_PARAMETER_BINDING::PROJ_MAT_POS, 1, false, glm::value_ptr(camera->projMatrix()));
                glUniform2f(SHADER_PARAMETER_BINDING::FRAME_SIZE, m_frameWidth, m_frameHeight);
                glUniform1i(SHADER_PARAMETER_BINDING::NPR_ENABLE, gui.toggleNPR || gui.toggleNPRObjOnly);

                glBindVertexArray(screenQuadVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
		}
	} 
} 

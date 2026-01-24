#include "ShadowMapping.h"

namespace INANOA {
    namespace SCENE {
        namespace EXPERIMENTAL {

            ShadowMapping::ShadowMapping(const Camera* camera, const int w, const int h) {
                this->init(camera, w, h);
            }

            ShadowMapping::~ShadowMapping() {
                delete m_shadow_map_renderer;
                m_shadow_map_renderer = nullptr;
                delete m_shadow;
                m_shadow = nullptr;
            }

            void ShadowMapping::init(const Camera* camera, const int w, const int h) {
                this->m_resolution = 1024;
                this->m_windowWidth = w;
                this->m_windowHeight = h;

                m_shadow_map_renderer = new OPENGL::RendererBase();
                const std::string vsFile = "src\\shader\\vertexShader_shadowMapping.glsl";
                const std::string fsFile = "src\\shader\\fragmentShader_shadowMapping.glsl";

                if (!m_shadow_map_renderer->init(vsFile, fsFile, w, h)) {
                    printf("Failed to initialize shadow mapping shader program.\n");
                    return;
                }

                glGenFramebuffers(1, &m_shadowFBO);

                glGenTextures(1, &m_shadowMap);
                glBindTexture(GL_TEXTURE_2D, m_shadowMap);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_resolution, m_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    printf("Failed to initialize Shadow Map Framebuffer.\n");

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // Light Space Matrix
                glm::vec3 lightPos = glm::vec3(-2.845, 2.028, -1.293);
                glm::vec3 lightTarget = glm::vec3(0.542, -0.141, -0.422);
                float range = 5.0f;
                float nearPlane = float(0.1f);
                float farPlane = float(10.0f);
                glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 lightProjection = glm::ortho(-range, range, -range, range, nearPlane, farPlane);
                m_lightSpaceMatrix = lightProjection * lightView;
            }

            void ShadowMapping::updateLightMatrix(glm::vec3 lightPos) {
                glm::vec3 lightTarget = glm::vec3(0.542, -0.141, -0.422);
                float range = 5.0f;
                float nearPlane = float(0.1f);
                float farPlane = float(10.0f);
                glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 lightProjection = glm::ortho(-range, range, -range, range, nearPlane, farPlane);
                m_lightSpaceMatrix = lightProjection * lightView;
            }

            void ShadowMapping::beginRender(glm::vec3 lightPos) {
                glEnable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowFBO);
                glViewport(0, 0, m_resolution, m_resolution);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                m_shadow_map_renderer->useProgram();
                updateLightMatrix(lightPos);
                glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_SPACE_MATRIX, 1, GL_FALSE, &m_lightSpaceMatrix[0][0]);
            }

            void ShadowMapping::endRender() {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(0, 0, m_windowWidth, m_windowHeight);
            }

            void ShadowMapping::renderGBuffer() {
                glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_SPACE_MATRIX, 1, GL_FALSE, &m_lightSpaceMatrix[0][0]);
            }

            GLuint ShadowMapping::getShadowMap() const {
                return m_shadowMap;
            }
        }
    }
}

#include "GBuffer.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {

            GBuffer::GBuffer(const Camera* camera, const int w, const int h){
				this->init(camera, w, h);
			}

            GBuffer::~GBuffer() {
				delete m_renderer;
				m_renderer = nullptr;
            }

            void GBuffer::init(const Camera* camera, const int w, const int h) {
                m_renderer = new OPENGL::RendererBase();
                const std::string vsFile = "src\\shader\\vertexShader_GBuffer.glsl";
                const std::string fsFile = "src\\shader\\fragmentShader_GBuffer.glsl";

                if (!m_renderer->init(vsFile, fsFile, w, h)) {
                    printf("Failed to initialize Deferred Shading shader program.\n");
                    return;
                }
                
                glGenFramebuffers(1, &m_gBuffer);
                glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

                // World Space Vertex
                glGenTextures(1, &m_gPositionWorld);
                glBindTexture(GL_TEXTURE_2D, m_gPositionWorld);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPositionWorld, 0);

                // World Space Normal
                glGenTextures(1, &m_gNormalWorld);
                glBindTexture(GL_TEXTURE_2D, m_gNormalWorld);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormalWorld, 0);

                // Ambient
                glGenTextures(1, &m_gAmbient);
                glBindTexture(GL_TEXTURE_2D, m_gAmbient);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAmbient, 0);

                // Diffuse
                glGenTextures(1, &m_gDiffuse);
                glBindTexture(GL_TEXTURE_2D, m_gDiffuse);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gDiffuse, 0);

                // Specular
                glGenTextures(1, &m_gSpecular);
                glBindTexture(GL_TEXTURE_2D, m_gSpecular);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_gSpecular, 0);

                // DepthMVP
                glGenTextures(1, &m_depthMVP);
                glBindTexture(GL_TEXTURE_2D, m_depthMVP);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_depthMVP, 0);

                // Depth
                glGenRenderbuffers(1, &m_rboDepth);
                glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);

                // Vertex
                glGenTextures(1, &m_gShadingPosition);
                glBindTexture(GL_TEXTURE_2D, m_gShadingPosition);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, m_gShadingPosition, 0);

                // Normal
                glGenTextures(1, &m_gShadingNormal);
                glBindTexture(GL_TEXTURE_2D, m_gShadingNormal);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, m_gShadingNormal, 0);

                GLenum attachments[8] = {
                    GL_COLOR_ATTACHMENT0,
                    GL_COLOR_ATTACHMENT1,
                    GL_COLOR_ATTACHMENT2,
                    GL_COLOR_ATTACHMENT3,
                    GL_COLOR_ATTACHMENT4,
                    GL_COLOR_ATTACHMENT5,
                    GL_COLOR_ATTACHMENT6,
                    GL_COLOR_ATTACHMENT7
                };
                glDrawBuffers(8, attachments);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            void GBuffer::resize(const int w, const int h) {
                // Delete framebuffers and textures
                glDeleteTextures(1, &m_gPositionWorld);
                glDeleteTextures(1, &m_gNormalWorld);
                glDeleteTextures(1, &m_gAmbient);
                glDeleteTextures(1, &m_gDiffuse);
                glDeleteTextures(1, &m_gSpecular);
                glDeleteTextures(1, &m_depthMVP);
                glDeleteTextures(1, &m_gShadingPosition);
                glDeleteTextures(1, &m_gShadingNormal);
                glDeleteRenderbuffers(1, &m_rboDepth);
                glDeleteFramebuffers(1, &m_gBuffer);

                glGenFramebuffers(1, &m_gBuffer);
                glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

                // World Space Vertex
                glGenTextures(1, &m_gPositionWorld);
                glBindTexture(GL_TEXTURE_2D, m_gPositionWorld);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPositionWorld, 0);

                // World Space Normal
                glGenTextures(1, &m_gNormalWorld);
                glBindTexture(GL_TEXTURE_2D, m_gNormalWorld);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormalWorld, 0);

                // Ambient
                glGenTextures(1, &m_gAmbient);
                glBindTexture(GL_TEXTURE_2D, m_gAmbient);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAmbient, 0);

                // Diffuse
                glGenTextures(1, &m_gDiffuse);
                glBindTexture(GL_TEXTURE_2D, m_gDiffuse);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gDiffuse, 0);

                // Specular
                glGenTextures(1, &m_gSpecular);
                glBindTexture(GL_TEXTURE_2D, m_gSpecular);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_gSpecular, 0);

                // DepthMVP
                glGenTextures(1, &m_depthMVP);
                glBindTexture(GL_TEXTURE_2D, m_depthMVP);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_depthMVP, 0);

                // Depth
                glGenRenderbuffers(1, &m_rboDepth);
                glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);

                // Vertex
                glGenTextures(1, &m_gShadingPosition);
                glBindTexture(GL_TEXTURE_2D, m_gShadingPosition);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, m_gShadingPosition, 0);

                // Normal
                glGenTextures(1, &m_gShadingNormal);
                glBindTexture(GL_TEXTURE_2D, m_gShadingNormal);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, m_gShadingNormal, 0);

                GLenum attachments[8] = {
                    GL_COLOR_ATTACHMENT0,
                    GL_COLOR_ATTACHMENT1,
                    GL_COLOR_ATTACHMENT2,
                    GL_COLOR_ATTACHMENT3,
                    GL_COLOR_ATTACHMENT4,
                    GL_COLOR_ATTACHMENT5,
                    GL_COLOR_ATTACHMENT6,
                    GL_COLOR_ATTACHMENT7
                };
                glDrawBuffers(8, attachments);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

			void GBuffer::beginRender() {
				glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                m_renderer->useProgram();
			}

            void GBuffer::endRender() {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            GLuint GBuffer::getTexture(GBufferTexture texture) const {
                switch (texture) {
                    case GBufferTexture::Position: return m_gPositionWorld;
                    case GBufferTexture::Normal: return m_gNormalWorld;
                    case GBufferTexture::Ambient: return m_gAmbient;
                    case GBufferTexture::Diffuse: return m_gDiffuse;
                    case GBufferTexture::Specular: return m_gSpecular;
                    case GBufferTexture::DepthMVP: return m_depthMVP;
                    case GBufferTexture::ShadingPosition: return m_gShadingPosition;
                    case GBufferTexture::ShadingNormal: return m_gShadingNormal;
                    default: return 0;
                }
            }
		} 
	}
} 

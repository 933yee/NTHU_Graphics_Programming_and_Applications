#include "FrameBuffer.h"
#include <iostream>

namespace INANOA {
	namespace OPENGL {
		FrameBuffer::FrameBuffer(const int w, const int h) {
			this->init(w, h);
		}
		FrameBuffer::~FrameBuffer() {}

		void FrameBuffer::init(const int w, const int h) {
			glGenVertexArrays(1, &this->vao);
			glBindVertexArray(this->vao);

			glGenBuffers(1, &this->vbo);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(this->window_vertex), this->window_vertex, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (const GLvoid*)(sizeof(GL_FLOAT) * 2));

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glGenFramebuffers(1, &this->fbo);

		}
		void FrameBuffer::resize(const int w, const int h) {
			glDeleteRenderbuffers(1, &this->rbo);
			glDeleteTextures(1, &this->fbo_tex);
			glDeleteTextures(1, &this->fbo_tex2);
			glGenRenderbuffers(1, &this->rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
			glDeleteFramebuffers(1, &this->fbo);
			glGenFramebuffers(1, &this->fbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, w, h);

			// reset frame buffer texture
			glGenTextures(1, &this->fbo_tex);
			glBindTexture(GL_TEXTURE_2D, this->fbo_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glGenTextures(1, &this->fbo_tex2);
			glBindTexture(GL_TEXTURE_2D, this->fbo_tex2);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
			glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->fbo_tex, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->fbo_tex2, 0);

		}

		void FrameBuffer::render() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glDisable(GL_DEPTH_TEST);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->fbo_tex);

			glBindVertexArray(this->vao);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
			
		}

		void FrameBuffer::bind_2tex() {
			GLenum attachments[] = {
					GL_COLOR_ATTACHMENT0,
					GL_COLOR_ATTACHMENT1
			};

			glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
			glDrawBuffers(2, attachments);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.0, 0.0, 0.0, 1.0);
		}

		void FrameBuffer::bind() {
			glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.0, 0.0, 0.0, 1.0);
		}

		void FrameBuffer::unbind() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

	}
}
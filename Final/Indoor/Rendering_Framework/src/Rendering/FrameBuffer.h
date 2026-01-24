#pragma once
#ifndef FrameBuffer_H
#define FrameBuffer_H
#include <glad/glad.h>

namespace INANOA {
	namespace OPENGL {
		
		class FrameBuffer {
		public:

			explicit FrameBuffer(const int w, const int h);
			virtual ~FrameBuffer();

			FrameBuffer(const FrameBuffer&) = delete;
			FrameBuffer(const FrameBuffer&&) = delete;
			FrameBuffer& operator=(const FrameBuffer&) = delete;

			void init(const int w, const int h);
			void resize(const int w, const int h);
			void render();

			void bind();
			void unbind();
			void bind_2tex();

			inline unsigned int getFBOtex() { return this->fbo_tex; }
			inline unsigned int getFBOtex2() { return this->fbo_tex2; }
			inline unsigned int getVAO() { return this->vao; }

		private:
			const GLfloat window_vertex[16] = {
				 1.0f, -1.0f, 1.0f, 0.0f,
				-1.0f, -1.0f, 0.0f, 0.0f,
				-1.0f,  1.0f, 0.0f, 1.0f,
				 1.0f,  1.0f, 1.0f, 1.0f,
			};

			unsigned int vao = 0u;
			unsigned int vbo = 0u;
			unsigned int fbo = 0u;
			unsigned int rbo = 0u;

			unsigned int fbo_tex = 0u;
			unsigned int fbo_tex2 = 0u;
		};
		
	}
}

#endif